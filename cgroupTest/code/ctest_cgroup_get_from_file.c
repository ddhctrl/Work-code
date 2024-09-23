#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cgroup-defs.h>
#include <linux/cgroup.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#define PROCFS_NAME "cgfd"
#define PROCFS_FPATH_NAME "cgpath"

MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_file_fd;
static struct proc_dir_entry *proc_file_path;

static ssize_t cgfd_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    int fd;
    char *kbuf;
    struct cgroup *cgrp;

    /* 获取fd */
    kbuf = kmalloc(count + 1, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    if (copy_from_user(kbuf, buffer, count)) {
        kfree(kbuf);
        return -EFAULT;
    }
    kbuf[count] = '\0';

    if (kstrtoint(kbuf, 10, &fd)) {
        kfree(kbuf);
        return -EINVAL;
    }
    kfree(kbuf);

    pr_info("内核空间获取的fd: %d\n", fd);
    cgrp = cgroup_get_from_fd(fd);
    if (IS_ERR(cgrp)) {
        pr_err("Failed to get cgroup from file fd: %ld\n", PTR_ERR(cgrp));
        return PTR_ERR(cgrp);
    }

    pr_info("Successfully got cgroup from fd\n");

    cgroup_put(cgrp);  // 释放 cgroup 引用

    return count;
}

static ssize_t cgpath_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    char *kbuf_fpath;
    struct cgroup *cgrp_fp;
     /* 获取file path */
    kbuf_fpath = kmalloc(count + 1, GFP_KERNEL);
    if (!kbuf_fpath)
        return -ENOMEM;

    if (copy_from_user(kbuf_fpath, buffer, count)) {
        kfree(kbuf_fpath);
        return -EFAULT;
    }
    kbuf_fpath[count-1] = '\0';

    pr_info("内核空间获取的path: %s\n", kbuf_fpath);
    cgrp_fp = cgroup_get_from_path(kbuf_fpath);
    if (IS_ERR(cgrp_fp)) {
        pr_err("Failed to get cgroup from file path: %ld\n", PTR_ERR(cgrp_fp));
        return PTR_ERR(cgrp_fp);
    }

    pr_info("Successfully got cgroup from path\n");

    cgroup_put(cgrp_fp);
    kfree(kbuf_fpath);

    return count;
}

static struct proc_ops proc_fd_fops = {
    .proc_write = cgfd_write,
};

static struct proc_ops proc_path_fops = {
    .proc_write = cgpath_write,
};

static int __init test_module_init(void)
{
    proc_file_fd = proc_create(PROCFS_NAME, 0666, NULL, &proc_fd_fops);
    if (!proc_file_fd) {
        pr_err("Failed to create proc file fd\n");
        return -ENOMEM;
    }

    proc_file_path = proc_create(PROCFS_FPATH_NAME, 0666, NULL, &proc_path_fops);
    if (!proc_file_path) {
        pr_err("Failed to create proc file path\n");
        return -ENOMEM;
    }

    pr_info("cgfd module loaded\n");
    return 0;
}

static void __exit test_module_exit(void)
{
    proc_remove(proc_file_fd);
    proc_remove(proc_file_path);
    pr_info("EXIT!\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
MODULE_AUTHOR("CKH");
MODULE_VERSION("1.1");
