#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/cred.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_VERSION("1.0");

// 模块参数，用于接收进程 PID
static int target_pid = -1;
module_param(target_pid, int, 0644);
MODULE_PARM_DESC(target_pid, "The PID of the target process");

// 模块加载函数
static int __init test_module_init(void)
{
    struct task_struct *task;
    pid_t target_pid;
    int signal = SIGUSR1; // 要发送的信号
    int priv = 0; // 是否是特权信号
    int ret;

    printk(KERN_INFO "test_module: Module loaded\n");

    target_pid = task_pid_nr(current);

    // 获取当前进程的 task_struct
    task = current;

    // 发送 SIGKILL 信号，强制终止指定进程
    // printk(KERN_INFO "test_module: Sending SIGKILL to process %d\n", target_pid);
    printk(KERN_INFO "test_module: Sending SIGKILL to current process\n");
    force_sig(SIGKILL);
    //force_sig(SIGTSTP);
    //force_sig(SIGCONT);
    printk(KERN_INFO "test_module: Sending SIGUSR1 to current process\n");
    ret = send_sig(signal, task, priv);
    if (ret) {
        printk(KERN_ALERT "Failed to send signal: %d\n", ret);
    } else {
        printk(KERN_INFO "Signal sent successfully to current process with pid %d\n", target_pid);
    }

    return ret;
}

// 模块卸载函数
static void __exit test_module_exit(void)
{
    printk(KERN_INFO "test_module: Module unloaded\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
