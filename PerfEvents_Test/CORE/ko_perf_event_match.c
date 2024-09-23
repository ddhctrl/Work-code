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
typedef bool(*exclusive_event_installable_t)(struct perf_event *event,
					struct perf_event_context *ctx);

// 回调函数指针
static exclusive_event_installable_t exclusive_event_installable_callback = NULL;

static struct perf_event *event1 = NULL;

static int __init test_module_init(void)
{
    struct perf_event_attr attr = {};
    bool ret;

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
    event1 = perf_event_create_kernel_counter(&attr, 0, current, NULL, NULL);
    if (IS_ERR(event1)) {
        printk(KERN_ERR "Failed to create perf event1: %ld\n", PTR_ERR(event1));
        return PTR_ERR(event1);
    }
    printk(KERN_INFO "perf_event: %p\n", event1);
    printk(KERN_INFO "event->pmu: %p\n", event1->pmu);
    printk(KERN_INFO "event->ctx: %p\n", event1->ctx);

    event1->pmu->capabilities |= PERF_PMU_CAP_EXCLUSIVE;
    exclusive_event_installable_callback = (exclusive_event_installable_t)addr1;
    printk(KERN_INFO "call exclusive_event_match, func address: %lx\n", addr1);
    ret = exclusive_event_installable_callback(event1, event1->ctx);
    printk(KERN_INFO "exclusive_event_match ret: %d\n", ret);

    return 0;
}

static void __exit test_module_exit(void)
{

    printk(KERN_INFO "Unloading test module...\n");
    if (event1)
    {
        event1->pmu->capabilities &= ~PERF_PMU_CAP_EXCLUSIVE;
        perf_event_release_kernel(event1);
    }
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
