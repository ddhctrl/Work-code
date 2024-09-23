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
MODULE_PARM_DESC(addr1, "Address of the force_sig_mceerr function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the force_sig_bnderr function");

static unsigned long addr3 = 0;
module_param(addr3, ulong, 0444);
MODULE_PARM_DESC(addr3, "Address of the force_sig_pkuerr function");

// 定义回调函数类型
typedef int (*force_sig_mceerr_t)(int code, void __user *addr, short lsb);
typedef int (*force_sig_bnderr_t)(void __user *addr, void __user *lower, void __user *upper);
typedef int (*force_sig_pkuerr_t)(void __user *addr, u32 pkey);

// 回调函数指针
static force_sig_mceerr_t force_sig_mceerr_callback = NULL;
static force_sig_bnderr_t force_sig_bnderr_callback = NULL;
static force_sig_pkuerr_t force_sig_pkuerr_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
    void __user *addr = (void __user *)current->mm->start_code; // 设定错误地址
    void __user *lower = (void __user *)0x0; // 边界下限
    void __user *upper = (void __user *)0xFFFFFFFF; // 边界上限
    u32 pkey = 0;
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    force_sig_mceerr_callback = (force_sig_mceerr_t)addr1;
    force_sig_bnderr_callback = (force_sig_bnderr_t)addr2;
    force_sig_pkuerr_callback = (force_sig_pkuerr_t)addr3;

    ret = force_sig_mceerr_callback(SIGBUS, (void __user *)current->mm->start_code, PAGE_SHIFT);
    printk(KERN_INFO "force_sig_mceerr ret:%d\n", ret);
    ret = force_sig_bnderr_callback(addr, lower, upper);
    printk(KERN_INFO "force_sig_bnderr ret:%d\n", ret);
    ret = force_sig_pkuerr_callback(addr, pkey);
    printk(KERN_INFO "force_sig_pkuerr ret:%d\n", ret);

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
