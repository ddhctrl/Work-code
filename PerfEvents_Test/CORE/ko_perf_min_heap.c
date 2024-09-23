#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *task;

static void code_to_measure(void) {
    int sum = 0;
    for (int i = 0; i < 10000000; ++i) {
        sum += 1;
    }
}

static int perf_thread(void *data) {
    struct perf_event_attr pe1, pe2, pe3, pe4;
    struct perf_event *event1, *event2, *event3, *event4;
    int cpu = smp_processor_id();

    // 配置第一个 perf_event_attr 结构体
    memset(&pe1, 0, sizeof(struct perf_event_attr));
    pe1.type = PERF_TYPE_HARDWARE;
    pe1.size = sizeof(struct perf_event_attr);
    pe1.config = PERF_COUNT_HW_CPU_CYCLES;
    pe1.disabled = 1;
    pe1.exclude_kernel = 1;
    pe1.exclude_hv = 1;
    pe1.read_format = PERF_FORMAT_GROUP;

    // 配置第二个 perf_event_attr 结构体
    memset(&pe2, 0, sizeof(struct perf_event_attr));
    pe2.type = PERF_TYPE_HARDWARE;
    pe2.size = sizeof(struct perf_event_attr);
    pe2.config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
    pe2.disabled = 1;
    pe2.exclude_kernel = 1;
    pe2.exclude_hv = 1;

    // 配置第三个 perf_event_attr 结构体
    memset(&pe3, 0, sizeof(struct perf_event_attr));
    pe3.type = PERF_TYPE_SOFTWARE;
    pe3.size = sizeof(struct perf_event_attr);
    pe3.config = PERF_COUNT_SW_TASK_CLOCK;
    pe3.disabled = 1;
    pe3.exclude_kernel = 1;
    pe3.exclude_hv = 1;

    // 配置第四个 perf_event_attr 结构体
    memset(&pe4, 0, sizeof(struct perf_event_attr));
    pe4.type = PERF_TYPE_SOFTWARE;
    pe4.size = sizeof(struct perf_event_attr);
    pe4.config = PERF_COUNT_SW_PAGE_FAULTS;
    pe4.disabled = 1;
    pe4.exclude_kernel = 1;
    pe4.exclude_hv = 1;

    // 创建第一个性能事件
    event1 = perf_event_create_kernel_counter(&pe1, cpu, current, NULL, NULL);
    if (IS_ERR(event1)) {
        pr_err("Error creating event1: %ld\n", PTR_ERR(event1));
        return PTR_ERR(event1);
    }

    // 创建第二个性能事件，作为第一个事件的子事件
    event2 = perf_event_create_kernel_counter(&pe2, cpu, current, NULL, NULL);
    if (IS_ERR(event2)) {
        pr_err("Error creating event2: %ld\n", PTR_ERR(event2));
        perf_event_release_kernel(event1);
        return PTR_ERR(event2);
    }

    // 创建第三个性能事件，作为第一个事件的子事件
    event3 = perf_event_create_kernel_counter(&pe3, cpu, current, NULL, NULL);
    if (IS_ERR(event3)) {
        pr_err("Error creating event3: %ld\n", PTR_ERR(event3));
        perf_event_release_kernel(event2);
        perf_event_release_kernel(event1);
        return PTR_ERR(event3);
    }

    // 创建第四个性能事件，作为第一个事件的子事件
    event4 = perf_event_create_kernel_counter(&pe4, cpu, current, NULL, NULL);
    if (IS_ERR(event4)) {
        pr_err("Error creating event4: %ld\n", PTR_ERR(event4));
        perf_event_release_kernel(event3);
        perf_event_release_kernel(event2);
        perf_event_release_kernel(event1);
        return PTR_ERR(event4);
    }

    // 释放性能事件
    perf_event_release_kernel(event4);
    perf_event_release_kernel(event3);
    perf_event_release_kernel(event2);
    perf_event_release_kernel(event1);

    return 0;
}

static int __init my_module_init(void)
{
    pr_info("Loading my_module\n");
    task = kthread_run(perf_thread, NULL, "perf_thread");
    if (IS_ERR(task)) {
        pr_err("Error creating kernel thread: %ld\n", PTR_ERR(task));
        return PTR_ERR(task);
    }
    return 0;
}

static void __exit my_module_exit(void)
{
    pr_info("Unloading my_module\n");
    if (task) {
        kthread_stop(task);
    }
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A kernel module to test perf_event_create_kernel_counter");
