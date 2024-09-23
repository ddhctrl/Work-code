#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/errno.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the do_no_restart_syscall function");

// 定义回调函数类型
typedef long (*do_no_restart_syscall_t)(struct restart_block *param);

// 回调函数指针
static do_no_restart_syscall_t do_no_restart_syscall_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    long ret;
    struct restart_block *restart = NULL;

    printk(KERN_INFO "Loading test module...\n");

    restart = &current->restart_block;

    // 设置回调函数指针
    do_no_restart_syscall_callback = (do_no_restart_syscall_t)addr1;

    ret = do_no_restart_syscall_callback(restart);
    printk(KERN_INFO "do_no_restart_syscall ret:%ld\n", ret);

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
