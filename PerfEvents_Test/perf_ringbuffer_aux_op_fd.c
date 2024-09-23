#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/fdtable.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A simple Linux driver to get perf_event from fd and call perf_aux_output_begin");
MODULE_VERSION("0.1");

#define PROCFS_NAME "perf_fd"

static struct perf_event *event;

static ssize_t perf_fd_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    int fd;
    struct file *f;
    char kbuf[16];
    struct perf_event *evt;
    struct perf_output_handle handle;
    void *result;

    if (count > 15)
        return -EINVAL;

    if (copy_from_user(kbuf, buffer, count))
        return -EFAULT;

    kbuf[count] = '\0';
    if (kstrtoint(kbuf, 10, &fd))
        return -EINVAL;

    // Get the file structure from the file descriptor
    f = fget(fd);
    if (!f) {
        printk(KERN_ERR "fget failed\n");
        return -ENOENT;
    }

    // Get the perf_event structure from the file structure
    evt = f->private_data;
    if (!evt) {
        printk(KERN_ERR "file->private_data is NULL\n");
        fput(f);
        return -ENOENT;
    }

    event = evt;
    fput(f);

    printk(KERN_INFO "Received fd: %d, event: %p, rb: %p\n", fd, event, event->rb);

    result = perf_aux_output_begin(&handle, event);
    if (!result) {
        printk(KERN_ERR "perf_aux_output_begin failed\n");
        perf_event_release_kernel(event);
        return -1;
    }
    printk(KERN_INFO "perf_aux_output_begin succeeded\n");

    return count;
}

static const struct proc_ops perf_fd_fops = {
    .proc_write = perf_fd_write,
};

static int __init test_perf_module_init(void) {
    proc_create(PROCFS_NAME, 0666, NULL, &perf_fd_fops);
    printk(KERN_INFO "Module loaded, /proc/%s created\n", PROCFS_NAME);

    return 0;
}

static void __exit test_perf_module_exit(void) {
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "Module unloaded, /proc/%s removed\n", PROCFS_NAME);
}

module_init(test_perf_module_init);
module_exit(test_perf_module_exit);
