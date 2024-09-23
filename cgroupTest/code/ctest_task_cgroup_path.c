#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

// 函数原型声明
int task_cgroup_path(struct task_struct *task, char *buf, size_t buflen);

// 模块参数，用于传递进程 PID
static pid_t pid = -1;
module_param(pid, int, 0444);
MODULE_PARM_DESC(pid, "PID of the task");

// 初始化函数
static int __init test_module_init(void)
{
    struct task_struct *task;
    char *buf;
    size_t buflen = 256;
    int ret;

    if (pid == -1) {
        pr_err("PID not specified\n");
        return -EINVAL;
    }

    task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task) {
        pr_err("Task with PID %d not found\n", pid);
        return -ESRCH;
    }

    buf = kmalloc(buflen, GFP_KERNEL);
    if (!buf) {
        put_task_struct(task);
        return -ENOMEM;
    }

    ret = task_cgroup_path(task, buf, buflen);
    if (ret < 0) {
        pr_err("Failed to get cgroup path for task with PID %d\n", pid);
    } else {
        pr_info("Cgroup path for task with PID %d: %s\n", pid, buf);
    }

    kfree(buf);
    put_task_struct(task);
    return 0;
}

// 清理函数
static void __exit test_module_exit(void)
{
    pr_info("Unloading test module\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A simple kernel module to call task_cgroup_path");
MODULE_VERSION("1.0");

module_init(test_module_init);
module_exit(test_module_exit);
