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
MODULE_PARM_DESC(addr1, "Address of the force_sig_seccomp function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the force_sig_ptrace_errno_trap function");

static unsigned long addr3 = 0;
module_param(addr3, ulong, 0444);
MODULE_PARM_DESC(addr3, "Address of the force_sig_fault_trapno function");

static unsigned long addr4 = 0;
module_param(addr4, ulong, 0444);
MODULE_PARM_DESC(addr4, "Address of the send_sig_fault_trapno function");

// 定义回调函数类型
typedef int (*force_sig_seccomp_t)(int syscall, int reason, bool force_coredump);
typedef int (*force_sig_ptrace_errno_trap_t)(int errno, void __user *addr);
typedef int (*force_sig_fault_trapno_t)(int sig, int code, void __user *addr, int trapno);
typedef int (*send_sig_fault_trapno_t)(int sig, int code, void __user *addr, int trapno,
			  struct task_struct *t);

// 回调函数指针
static force_sig_seccomp_t force_sig_seccomp_callback = NULL;
static force_sig_ptrace_errno_trap_t force_sig_ptrace_errno_trap_callbcak = NULL;
static force_sig_fault_trapno_t force_sig_fault_trapno_callback = NULL;
static send_sig_fault_trapno_t send_sig_fault_trapno_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
    int syscall = __NR_write; // 示例系统调用号
    int reason = 1; // 示例原因
    bool force_coredump = false; // 是否强制核心转储
    void __user *addr = (void __user *)current->mm->start_code;
    struct pid *pid_struct;
    int signal = SIGKILL; // 默认信号为 SIGKILL
    int priv = 0;
    pid_t target_pid;

    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    force_sig_seccomp_callback = (force_sig_seccomp_t)addr1;
    force_sig_ptrace_errno_trap_callbcak = (force_sig_ptrace_errno_trap_t)addr2;
    force_sig_fault_trapno_callback = (force_sig_fault_trapno_t)addr3;
    send_sig_fault_trapno_callback = (send_sig_fault_trapno_t)addr4;

    ret = force_sig_seccomp_callback(syscall, reason, force_coredump);
    printk(KERN_INFO "force_sig_seccomp ret:%d\n", ret);
    ret = force_sig_ptrace_errno_trap_callbcak(0, addr);
    printk(KERN_INFO "force_sig_ptrace_errno_trap ret:%d\n", ret);
    ret = force_sig_fault_trapno_callback(SIGUSR1, 0, addr, 0);
    printk(KERN_INFO "force_sig_fault_trapno ret:%d\n", ret);
    ret = send_sig_fault_trapno_callback(SIGUSR1, 0, addr, 0, current);
    printk(KERN_INFO "send_sig_fault_trapno ret:%d\n", ret);

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
