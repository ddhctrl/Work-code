#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/pid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A module to print task_struct address of a given PID.");
MODULE_VERSION("0.1");

static int pid = -1;
module_param(pid, int, 0644);
MODULE_PARM_DESC(pid, "The PID of the target process");

static int __init task_struct_address_init(void) {
    struct task_struct *task;
    struct pid *pid_struct;

    if (pid == -1) {
        printk(KERN_INFO "No PID provided\n");
        return -EINVAL;
    }

    pid_struct = find_get_pid(pid);
    task = pid_task(pid_struct, PIDTYPE_PID);

    if (task) {
        printk(KERN_INFO "task_struct address for PID %d: %p\n", pid, task);
    } else {
        printk(KERN_INFO "No task found for PID %d\n", pid);
    }

    return 0;
}

static void __exit task_struct_address_exit(void) {
    printk(KERN_INFO "Module unloaded\n");
}

module_init(task_struct_address_init);
module_exit(task_struct_address_exit);
