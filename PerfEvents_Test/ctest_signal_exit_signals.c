#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Set JOBCTL_PENDING_MASK and then create a thread");

static struct workqueue_struct *my_wq;
static struct work_struct my_work;

static void my_work_func(struct work_struct *work)
{
    struct task_struct *task = current; // 获取当前进程
    struct task_struct *thread;

    // 设置 JOBCTL_PENDING_MASK 标志
    task->jobctl |= JOBCTL_PENDING_MASK;

    recalc_sigpending();

    // 创建一个新线程
    pr_info("Created new thread\n");
    thread = kthread_run((void *)kthread_should_stop, NULL, "test_thread");
    if (IS_ERR(thread)) {
        pr_err("Failed to create thread\n");
    } else {
        pr_info("Thread created successfully\n");
    }
}

static int __init my_module_init(void)
{
    pr_info("Module loaded\n");

    // 创建工作队列
    my_wq = create_singlethread_workqueue("my_wq");
    if (!my_wq) {
        pr_err("Failed to create workqueue\n");
        return -ENOMEM;
    }

    // 初始化工作
    INIT_WORK(&my_work, my_work_func);

    // 将工作添加到工作队列
    queue_work(my_wq, &my_work);

    return 0;
}

static void __exit my_module_exit(void)
{
    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);
    pr_info("Module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
