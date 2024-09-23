#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/list.h>
#include <linux/sched/signal.h>

#include "/home/kanghong.chen/workspace/linux-rt-6.1.96/kernel/cgroup/cgroup-internal.h"

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_cancel_fork function");

// 定义回调函数类型
typedef void(*pids_cancel_attach_t)(struct cgroup_taskset *tset);

// 回调函数指针
static pids_cancel_attach_t pids_cancel_attach_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct cgroup_taskset tset;

    printk(KERN_INFO "Loading test module...\n");

    INIT_LIST_HEAD(tset.csets);

    // 设置回调函数指针
    pids_cancel_attach_callback = (pids_cancel_attach_t)addr;

    pids_cancel_attach_callback(&tset);

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
