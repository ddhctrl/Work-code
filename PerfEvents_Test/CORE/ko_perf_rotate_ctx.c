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
MODULE_PARM_DESC(addr1, "Address of the rotate_ctx function");
// 定义回调函数类型
typedef void(*rotate_ctx_t)(struct perf_event_context *ctx, struct perf_event *event);

// 回调函数指针
static rotate_ctx_t rotate_ctx_callback = NULL;
static struct perf_event *event = NULL;

static int __init test_module_init(void)
{
    struct perf_event_attr attr = {};

    printk(KERN_INFO "Loading test module...\n");

    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.disabled = 0; // Event is initially enabled
    attr.exclude_user = 0;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.exclude_host = 1;
    attr.config = PERF_COUNT_HW_CPU_CYCLES;

    // 创建内核计数器
    event = perf_event_create_kernel_counter(&attr, -1, current, NULL, NULL);
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

    rotate_ctx_callback = (rotate_ctx_t)addr1;
    rotate_ctx_callback(event->ctx, event);
    printk(KERN_INFO "rotate ctx success!\n");

    return 0;
}

static void __exit test_module_exit(void)
{

    printk(KERN_INFO "Unloading test module...\n");
    // 释放内核计数器
    perf_event_release_kernel(event);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
