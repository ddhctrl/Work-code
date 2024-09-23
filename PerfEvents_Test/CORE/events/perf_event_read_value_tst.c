#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/uaccess.h>



static struct perf_event *event;
static u64 enabled, running;

static int __init test_init(void)
{
    struct perf_event_attr attr;
    int cpu = smp_processor_id();
    struct task_struct *task = current;
    u64 count;
    pr_info("Initializing test module\n");

    memset(&attr, 0, sizeof(struct perf_event_attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.disabled = 1;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;

    event = perf_event_create_kernel_counter(&attr, cpu, task, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    // 启动性能事件
    perf_event_enable(event);

    // 读取性能事件的值
    count = perf_event_read_value(event, &enabled, &running);
    pr_info("Event count: %llu\n", count);
    pr_info("Enabled time: %llu\n", enabled);
    pr_info("Running time: %llu\n", running);

    return 0;
}

static void __exit test_exit(void)
{
    pr_info("Exiting test module\n");

    // 停止性能事件
    perf_event_disable(event);

    // 释放性能事件
    perf_event_release_kernel(event);
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
