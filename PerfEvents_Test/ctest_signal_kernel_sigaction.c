#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>

// 初始化模块
static int __init test_module_init(void)
{
    printk(KERN_INFO "Loading test module...\n");

    kernel_sigaction(SIGUSR1, SIG_IGN);

    printk(KERN_INFO "Signal handler for SIGUSR1 is set\n");

    return 0;
}

// 卸载模块
static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Unloading test module...\n");
    kernel_sigaction(SIGUSR1, SIG_DFL);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
