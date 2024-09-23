#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/fdtable.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_pmu_resched function");
// 定义回调函数类型
typedef int(*perf_event_stop_t)(struct perf_event *event, int restart);

// 回调函数指针
static perf_event_stop_t perf_event_stop_callback = NULL;
static struct perf_event *event = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    int ret;
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
    attr.enable_on_exec = 1;
    attr.config = ARMV8_PMUV3_PERFCTR_CPU_CYCLES;

    // 创建内核计数器
    event = perf_event_create_kernel_counter(&attr, -1, current, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    // 设置回调函数指针
    perf_event_stop_callback = (perf_event_stop_t)addr1;

    printk(KERN_INFO "perf_event_stop_callback set to %lx\n", addr1);

    ret = perf_event_stop_callback(event, 0);

    printk(KERN_INFO "perf_event_stop success, ret:%d\n", ret);

    return 0;
}

// 卸载模块
static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Unloading test module...\n");
    perf_event_release_kernel(event);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
