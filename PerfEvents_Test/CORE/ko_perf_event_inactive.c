#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>

static struct perf_event *event = NULL;
static u64 prev_count = 0;

static void perf_event_callback(struct perf_event *event, struct perf_sample_data *data, struct pt_regs *regs)
{
    u64 count = perf_event_read_value(event, NULL, NULL);
    printk(KERN_INFO "CPU cycles: %llu\n", count - prev_count);
    prev_count = count;
}

static int __init test_module_init(void)
{
    struct perf_event_attr attr = {};

    printk(KERN_INFO "Loading test module...\n");

    // 设置 perf_event_attr
    attr.type = PERF_TYPE_RAW;
    attr.size = sizeof(struct perf_event_attr);
    attr.pinned = 0;
    attr.disabled = 1; // Event is initially enabled
    attr.exclude_user = 0;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.exclude_host = 1;
    attr.config = ARMV8_PMUV3_PERFCTR_CPU_CYCLES;
    attr.enable_on_exec = 1;

    // 创建内核计数器
    event = perf_event_create_kernel_counter(&attr, -1, current, perf_event_callback, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    // 打印一些调试信息
    printk(KERN_INFO "perf_event: %p\n", event);
    printk(KERN_INFO "event->state: %d\n", event->state);
    printk(KERN_INFO "event->ctx: %p\n", event->ctx);

    if (!event->ctx) {
        printk(KERN_ERR "event->ctx is NULL\n");
        perf_event_release_kernel(event);
        return -EINVAL;
    }

    printk(KERN_INFO "Calling perf_event_disable...\n");
    perf_event_disable(event);

    return 0;
}

static void __exit test_module_exit(void)
{

    printk(KERN_INFO "Unloading test module...\n");
    perf_event_release_kernel(event);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
