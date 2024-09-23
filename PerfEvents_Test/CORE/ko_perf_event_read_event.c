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
MODULE_PARM_DESC(addr1, "Address of the perf_event_read_event function");
// 定义回调函数类型
typedef void(*perf_event_read_event_t)(struct perf_event *event,
			struct task_struct *task);

// 回调函数指针
static perf_event_read_event_t perf_event_read_event_callback = NULL;

static struct perf_event *event = NULL;

static int __init test_module_init(void)
{
    struct perf_event_attr attr = {};

    printk(KERN_INFO "Loading test module...\n");

    // 设置 perf_event_attr
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.disabled = 0; // Event is initially enabled
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.inherit = 1; // 子进程继承性能事件
    attr.inherit_stat = 1;
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

    perf_event_read_event_callback = (perf_event_read_event_t)addr1;
    printk(KERN_INFO "call perf_event_read_event, func address: %lx\n", addr1);
    perf_event_read_event_callback(event, current);

    return 0;
}

static void __exit test_module_exit(void)
{

    printk(KERN_INFO "Unloading test module...\n");

    if (event)
    {
        // 释放内核计数器
        perf_event_release_kernel(event);
    }
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
