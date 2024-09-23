#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the calculate_sigpending function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the recalc_sigpending_and_wake function");

// 定义回调函数类型
typedef void (*calculate_sigpending_t)(void);
typedef void (*recalc_sigpending_and_wake_t)(struct task_struct *t);

// 回调函数指针
static calculate_sigpending_t cal_sigpending_callback = NULL;
static recalc_sigpending_and_wake_t recalc_sigpending_and_wake_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct task_struct *task = current;
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    cal_sigpending_callback = (calculate_sigpending_t)addr1;
    recalc_sigpending_and_wake_callback = (recalc_sigpending_and_wake_t)addr2;

    // 检查回调函数指针是否有效
    printk(KERN_INFO "calculate_sigpending");
    cal_sigpending_callback();
    printk(KERN_INFO "recalc_sigpending");
    recalc_sigpending_and_wake_callback(task);

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
