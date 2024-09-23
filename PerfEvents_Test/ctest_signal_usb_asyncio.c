#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/cred.h>
#include <linux/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");

extern int kill_pid_usb_asyncio(int sig, int errno, sigval_t addr, struct pid *pid, const struct cred *cred);

static int __init test_module_init(void)
{
    struct pid *pid_struct;
    int signal = SIGUSR1; // 要发送的信号
    sigval_t value;
    const struct cred *cred = current_cred(); // 获取当前进程的凭证
    int ret;

    // 获取当前进程的 PID
    pid_t target_pid = task_pid_nr(current);

    value.sival_int = 42; // 设置信号的附加值

    // 获取目标进程的 pid 结构
    pid_struct = find_get_pid(target_pid);
    if (!pid_struct) {
        printk(KERN_ALERT "Failed to find pid %d\n", target_pid);
        return -ESRCH;
    }

    // 调用 kill_pid_usb_asyncio 函数
    ret = kill_pid_usb_asyncio(signal, 0, value, pid_struct, cred);
    if (ret) {
        printk(KERN_ALERT "Failed to send signal: %d\n", ret);
    } else {
        printk(KERN_INFO "Signal sent successfully to pid %d\n", target_pid);
    }

    // 释放 pid 结构
    put_pid(pid_struct);

    return ret;
}

static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Test module exit\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
