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
MODULE_PARM_DESC(addr1, "Address of the force_sig_fault function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the send_sig_fault function");

// 定义回调函数类型
typedef int (*force_sig_fault_t)(int sig, int code, void __user *addr);
typedef int (*send_sig_fault_t)(int sig, int code, void __user *addr, struct task_struct *t);

// 回调函数指针
static force_sig_fault_t force_sig_fault_callback = NULL;
static send_sig_fault_t send_sig_fault_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
    void __user *addr = (void __user *)current->mm->start_code;
    struct pid *pid_struct;
    int signal = SIGKILL; // 默认信号为 SIGKILL
    int priv = 0;
    short lsb = 4;
    pid_t target_pid;

    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    force_sig_fault_callback = (force_sig_fault_t)addr1;
    send_sig_fault_callback = (send_sig_fault_t)addr2;

    ret = force_sig_fault_callback(SIGUSR1, 0, addr);
    printk(KERN_INFO "force_sig_fault ret:%d\n", ret);
    ret = send_sig_fault_callback(SIGUSR1, 0, addr, current);
    printk(KERN_INFO "send_sig_fault ret:%d\n", ret);
    ret = send_sig_mceerr(BUS_MCEERR_AR, addr, lsb, current);
    printk(KERN_INFO "send_sig_mceerr ret:%d\n", ret);

        // 获取当前进程的 PID
    target_pid = task_pid_nr(current);

    // 获取目标进程的 pid 结构
    pid_struct = find_get_pid(target_pid);
    if (!pid_struct) {
        printk(KERN_ALERT "Failed to find pid %d\n", target_pid);
        return -ESRCH;
    }

    // 调用 kill_pid 函数发送信号
    ret = kill_pid(pid_struct, signal, priv);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to send signal: %d\n", ret);
    } else {
        printk(KERN_INFO "Signal %d sent successfully to current process with pid %d\n", signal, target_pid);
    }

    // 释放 pid 结构
    put_pid(pid_struct);

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
