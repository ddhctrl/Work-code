#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/compat.h>
#include <linux/signal.h>
#include <linux/uaccess.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the force_sig_fault function");

// 定义回调函数类型
typedef int (*copy_siginfo_from_user32_t)(struct kernel_siginfo *to,
			                            const struct compat_siginfo __user *ufrom);

// 回调函数指针
static copy_siginfo_from_user32_t copy_siginfo_from_user32_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
    struct kernel_siginfo kernel_siginfo;
    struct compat_siginfo __user user_compat_siginfo;

    user_compat_siginfo.si_signo = SIGUSR1;
    user_compat_siginfo.si_code = SI_QUEUE;
    user_compat_siginfo.si_value.sival_int = 1234;

    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    copy_siginfo_from_user32_callback = (copy_siginfo_from_user32_t)addr1;

    ret = copy_siginfo_from_user32_callback(&kernel_siginfo, &user_compat_siginfo);
    if (ret == -EFAULT) {
        printk(KERN_ERR "Failed to copy siginfo from user space.\n");
    } else {
        printk(KERN_INFO "Successfully copied siginfo from user space.\n");
    }

    return ret;
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
