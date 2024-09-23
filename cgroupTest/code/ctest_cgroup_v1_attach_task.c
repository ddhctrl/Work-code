#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cgroup.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

static int pid_from = 0;
static int pid_to = 0;
module_param(pid_from, int, 0);
MODULE_PARM_DESC(pid_from, "PID of the source task");
module_param(pid_to, int, 0);
MODULE_PARM_DESC(pid_to, "PID of the target task");

static int __init cgroup_attach_task_all_init(void)
{
    struct task_struct *task_from;
    struct task_struct *task_to;
    int retval;

    if (pid_from <= 0 || pid_to <= 0) {
        pr_err("Invalid PIDs: pid_from=%d, pid_to=%d\n", pid_from, pid_to);
        return -EINVAL;
    }

    rcu_read_lock();
    task_from = pid_task(find_vpid(pid_from), PIDTYPE_PID);
    task_to = pid_task(find_vpid(pid_to), PIDTYPE_PID);
    rcu_read_unlock();

    if (!task_from || !task_to) {
        pr_err("Failed to find tasks: pid_from=%d, pid_to=%d\n", pid_from, pid_to);
        return -ESRCH;
    }

    retval = cgroup_attach_task_all(task_from, task_to);
    if (retval)
        pr_err("Failed to attach task: %d\n", retval);
    else
        pr_info("Successfully attached task from %d to %d\n", pid_from, pid_to);

    return retval;
}

static void __exit cgroup_attach_task_all_exit(void)
{
    pr_info("cgroup_attach_task_all module exited\n");
}

module_init(cgroup_attach_task_all_init);
module_exit(cgroup_attach_task_all_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("Example module to attach a task to a cgroup");
