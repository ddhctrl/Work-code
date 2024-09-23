#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_FILENAME "notify_die_address"
static char notify_die_address[64] = {0};

static ssize_t proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos) {
    if (count > sizeof(notify_die_address) - 1)
        return -EINVAL;

    if (copy_from_user(notify_die_address, buffer, count))
        return -EFAULT;

    notify_die_address[count] = '\0'; // 添加字符串终止符
    printk(KERN_INFO "notify_die_address set to: %s\n", notify_die_address);
    return count;
}

static ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *pos) {
    char addr_str[20];
    int len = snprintf(addr_str, sizeof(addr_str), "%s\n", notify_die_address);

    if (*pos >= len)
        return 0;

    if (copy_to_user(buffer, addr_str, len))
        return -EFAULT;

    *pos += len;
    return len;
}

static const struct proc_ops proc_ops = {
    .proc_write = proc_write,
    .proc_read = proc_read,
};

static int __init proc_init(void) {
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_FILENAME, 0666, NULL, &proc_ops);
    if (!entry) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_FILENAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "/proc/%s created\n", PROC_FILENAME);
    return 0;
}

static void __exit proc_exit(void) {
    remove_proc_entry(PROC_FILENAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_FILENAME);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_AUTHOR("CKH");
MODULE_LICENSE("GPL");
