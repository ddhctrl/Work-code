#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_event_read_local function");
// 定义回调函数类型
typedef int(*exclusive_event_match_t)(struct perf_event *e1, struct perf_event *e2);

// 回调函数指针
static exclusive_event_match_t exclusive_event_match_callback = NULL;
static struct perf_event *event = NULL;
static struct perf_event *event2 = NULL;

static int __init test_module_init(void)
{
    struct perf_event_attr attr = {};
    struct perf_event_attr attr2 = {};
    int ret;

    printk(KERN_INFO "Loading test module...\n");

    // 设置 perf_event_attr
    attr.type = PERF_TYPE_RAW;
    attr.size = sizeof(struct perf_event_attr);
    attr.pinned = 0;
    attr.disabled = 0; // Event is initially enabled
    attr.exclude_user = 0;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.exclude_host = 1;
    attr.config = ARMV8_PMUV3_PERFCTR_CPU_CYCLES;

    attr2.type = PERF_TYPE_HARDWARE;
    attr2.size = sizeof(struct perf_event_attr);
    attr2.disabled = 0; // Event is initially enabled
    attr2.exclude_user = 0;
    attr2.exclude_kernel = 1;
    attr2.exclude_hv = 1;
    attr2.exclude_host = 1;
    attr2.config = PERF_COUNT_HW_CPU_CYCLES;

    // 创建内核计数器
    event = perf_event_create_kernel_counter(&attr, -1, current, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    event2 = perf_event_create_kernel_counter(&attr2, -1, current, NULL, NULL);
    if (IS_ERR(event2)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event2));
        return PTR_ERR(event2);
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

    exclusive_event_match_callback = (exclusive_event_match_t)addr1;
    ret = exclusive_event_match_callback(event, event2);
    printk(KERN_INFO "event match ret: %d\n", ret);

    return 0;
}

static void __exit test_module_exit(void)
{

    printk(KERN_INFO "Unloading test module...\n");
    // 释放内核计数器
    perf_event_release_kernel(event);
    perf_event_release_kernel(event2);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
