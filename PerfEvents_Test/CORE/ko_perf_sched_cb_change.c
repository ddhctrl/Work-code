#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/fdtable.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_sched_cb_inc function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the perf_sched_cb_dec function");

// 定义回调函数类型
typedef void (*perf_sched_cb_inc_t)(struct pmu *pmu);
typedef void (*perf_sched_cb_dec_t)(struct pmu *pmu);

// 回调函数指针
static perf_sched_cb_inc_t perf_sched_cb_inc_callback = NULL;
static perf_sched_cb_dec_t perf_sched_cb_dec_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct pmu *pmu;
    struct perf_event *event;
    struct file *event_file;
    struct perf_event_attr attr = {};
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    perf_sched_cb_inc_callback = (perf_sched_cb_inc_t)addr1;
    perf_sched_cb_dec_callback = (perf_sched_cb_dec_t)addr2;

    printk(KERN_INFO "perf_sched_cb_inc_callback set to %lx\n", addr1);
    printk(KERN_INFO "perf_sched_cb_dec_callback set to %lx\n", addr2);

    // 设置 perf_event_attr
    attr.type = PERF_TYPE_RAW;
    attr.size = sizeof(struct perf_event_attr);
    attr.pinned = 1;
    attr.disabled = 0; // Event is initially enabled
    attr.exclude_user = 0;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.exclude_host = 1;
    attr.config = ARMV8_PMUV3_PERFCTR_CPU_CYCLES;

    // 创建内核计数器
    event = perf_event_create_kernel_counter(&attr, -1, current, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    // 获取 pmu 对象
    pmu = event->pmu;
    if (!pmu) {
        printk(KERN_ERR "Failed to get PMU from event\n");
        fput(event_file);
        return -EINVAL;
    }

    perf_sched_cb_inc_callback(pmu);
    printk(KERN_INFO "perf_sched_cb_inc success");

    perf_sched_cb_dec_callback(pmu);
    printk(KERN_INFO "perf_sched_cb_dec success");

    return 0;
}

// 卸载模块
static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Unloading test module...\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
