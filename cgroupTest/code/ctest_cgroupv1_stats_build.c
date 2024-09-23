#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/kallsyms.h>
#include <linux/cgroup.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/namei.h>
#include <linux/errno.h>

//模块参数, 用于传递地址
static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroupstats_build function");

typedef int (*cgroupstats_build_t)(struct cgroupstats *stats, struct dentry *dentry);

// 回调函数指针
static cgroupstats_build_t cgroupstats_build_callback = NULL;

// 通过文件指针获取 dentry 并调用 cgroupstats_build
static int my_cgroupstats_build(struct file *f)
{
    struct dentry *dentry;
    struct cgroupstats *stats;
    int ret;

    if (!f) {
        printk(KERN_ERR "Invalid file pointer\n");
        return -EBADF;
    }

    // 获取 dentry
    dentry = f->f_path.dentry;

    // 分配并初始化 cgroupstats 结构
    stats = kmalloc(sizeof(struct cgroupstats), GFP_KERNEL);
    if (!stats) {
        printk(KERN_ERR "Failed to allocate memory for cgroupstats\n");
        return -ENOMEM;
    }

    if (!f) {
        printk(KERN_ERR "Failed to get file from path\n");
        return -ENOMEM;
    }

    if (!dentry) {
        printk(KERN_ERR "file dentry is NULL, f->f_path:%p\n", &f->f_path);
        return -ENOMEM;
    }
    printk(KERN_INFO "f->f_path:%p, dentry:%p\n", &f->f_path, dentry);

    // 调用 cgroupstats_build 函数
    ret = cgroupstats_build_callback(stats, dentry);
    if (ret < 0) {
        printk(KERN_ERR "cgroupstats_build failed: %d\n", ret);
    } else {
        printk(KERN_INFO "cgroupstats_build succeeded\n");
    }

    // 释放资源
    kfree(stats);

    return ret;
}

static int __init test_module_init(void)
{
    struct file *file = NULL;

    // 将地址转换为函数指针
    cgroupstats_build_callback = (cgroupstats_build_t)addr;

    if (!cgroupstats_build_callback) {
        printk(KERN_ERR "Failed to get cgroupstats_build function address\n");
        return -EFAULT;
    }

    file = filp_open("/sys/fs/cgroup/pids", O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "Failed to open /sys/fs/cgroup/pids\n");
        return PTR_ERR(file);
    }

    // 调用 my_cgroupstats_build 函数
    my_cgroupstats_build(file);

    // 关闭文件
    filp_close(file, NULL);

    return 0;
}

static void __exit test_module_exit(void)
{
    printk(KERN_INFO "test module exited\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A module to call cgroupstats_build function");
