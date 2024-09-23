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
MODULE_PARM_DESC(addr1, "Address of the send_sig_perf function");

// 定义回调函数类型
typedef int (*send_sig_perf_t)(void __user *addr, u32 type, u64 sig_data);

// 回调函数指针
static send_sig_perf_t send_sig_perf_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
    void __user *addr = (void __user *)current->mm->start_code; // 设定错误地址
    u32 type = 0;
    u64 sig_data = 12345; //test signal data
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    send_sig_perf_callback = (send_sig_perf_t)addr1;

    ret = send_sig_perf_callback(addr, type, sig_data);
    printk(KERN_INFO "send_sig_perf ret:%d\n", ret);

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
