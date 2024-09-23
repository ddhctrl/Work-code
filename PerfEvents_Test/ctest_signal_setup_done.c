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
MODULE_PARM_DESC(addr1, "Address of the calculate_sigpending function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the calculate_sigpending function");

// 定义回调函数类型
typedef void (*signal_setup_done_t)(int failed, struct ksignal *ksig, int stepping);
typedef void (*force_exit_sig_t)(int sig);

// 回调函数指针
static signal_setup_done_t setup_done_callback = NULL;
static force_exit_sig_t force_exit_sig_callbcak = NULL;
static struct ksignal ksig;

// 初始化模块
static int __init test_module_init(void)
{
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    setup_done_callback = (signal_setup_done_t)addr1;
    force_exit_sig_callbcak = (force_exit_sig_t)addr2;

    ksig.sig = SIGSEGV;
    printk(KERN_INFO "1.step in signal_setup_done\n");
    setup_done_callback(1, &ksig, 0);
    printk(KERN_INFO "2.step in signal_setup_done\n");
    setup_done_callback(0, &ksig, 1);
    printk(KERN_INFO "step in force_exit_sig\n");
    force_exit_sig_callbcak(ksig.sig);

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
