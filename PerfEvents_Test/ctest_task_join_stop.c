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


// 定义回调函数类型
typedef void (*task_join_group_stop_t)(struct task_struct *task);

// 回调函数指针
static task_join_group_stop_t task_stop_callback = NULL;
struct task_struct *task;

int thread_fn(void *data)
{
    struct signal_struct *sig = current->signal;
    pr_info("Thread function started\n");

    sig->group_stop_count = 1;
    // 调用 task_join_group_stop
    task_stop_callback(current);

    // 模拟一些工作
    while (!kthread_should_stop()) {
        msleep(1000);
    }

    pr_info("Thread function stopping\n");
    return 0;
}

// 初始化模块
static int __init test_module_init(void)
{
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    task_stop_callback = (task_join_group_stop_t)addr1;

    task = kthread_run(thread_fn, NULL, "test_kthread");
    if (IS_ERR(task)) {
        pr_err("Failed to create thread\n");
        return PTR_ERR(task);
    }

    return 0;
}

// 卸载模块
static void __exit test_module_exit(void)
{
    if (task) {
        kthread_stop(task);
        pr_info("Thread stopped\n");
    }
    printk(KERN_INFO "Unloading test module...\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
