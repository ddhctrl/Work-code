#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>

#define SIGTERM 15

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the ignore_signals function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the flush_itimer_signals function");

static unsigned long addr3 = 0;
module_param(addr3, ulong, 0444);
MODULE_PARM_DESC(addr3, "Address of the flush_itimer_signals function");

// 定义回调函数类型
typedef void (*ignore_signals_t)(struct task_struct *t);
typedef void (*flush_itimer_signals_t)(void);
typedef bool (*unhandled_signal_t)(struct task_struct *tsk, int sig);

// 回调函数指针
static ignore_signals_t ignore_signals_callback = NULL;
static flush_itimer_signals_t flush_itimer_signals_callback = NULL;
static unhandled_signal_t unhandled_signal_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct task_struct *task = current;
    bool ret;
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    ignore_signals_callback = (ignore_signals_t)addr1;
    flush_itimer_signals_callback = (flush_itimer_signals_t)addr2;
    unhandled_signal_callback = (unhandled_signal_t)addr3;

    printk(KERN_INFO "ignore signaling");
    ignore_signals_callback(task);
    printk(KERN_INFO "flush_itimer_signaling");
    flush_itimer_signals_callback();

    ret = unhandled_signal_callback(task, SIGTERM);
    printk(KERN_INFO "unhandled signal ret:%d", ret);

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
