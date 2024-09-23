#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

static int __init clear_jobctl_init(void)
{
    struct task_struct *task;

    for_each_process(task) {
        if (task->pid == current->pid) {
            task->jobctl &= ~(JOBCTL_TRAP_MASK | JOBCTL_TRAP_FREEZE);
            printk(KERN_INFO "Cleared JOBCTL_TRAP_MASK and JOBCTL_TRAP_FREEZE for process %d\n", task->pid);
            break;
        }
    }

    return 0;
}

static void __exit clear_jobctl_exit(void)
{
    printk(KERN_INFO "Module exit\n");
}

module_init(clear_jobctl_init);
module_exit(clear_jobctl_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Clear jobctl flags");
MODULE_AUTHOR("Your Name");
