#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/sched/signal.h>
#include <linux/spinlock.h>
#include <linux/rwlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h> 

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_cancel_fork function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the cgroup_can_fork function");

// 定义回调函数类型
typedef void(*cgroup_cancel_fork_t)(struct task_struct *child,
			struct kernel_clone_args *kargs);

typedef int(*cgroup_can_fork_t)(struct task_struct *child,
			struct kernel_clone_args *kargs);            

// 回调函数指针
static cgroup_cancel_fork_t cgroup_cancel_fork_callback = NULL;
static cgroup_can_fork_t cgroup_can_fork_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
    struct task_struct *tsk = current; // 使用当前进程作为示例
    struct kernel_clone_args kargs = {
        .flags = 0,
        .cset = task_css_set(current),
        .cgrp = NULL,
    };

    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    cgroup_cancel_fork_callback = (cgroup_cancel_fork_t)addr;
    cgroup_can_fork_callback = (cgroup_can_fork_t)addr2;

    ret = cgroup_can_fork_callback(tsk, &kargs);
    if (ret)
    {
        printk(KERN_INFO "cgroup_can_fork error! ret:%d\n", ret);
        return ret;
    }

    cgroup_cancel_fork_callback(tsk, &kargs);

    return 0;
}

// 卸载模块
static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Unloading test module...\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
