#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include "/home/kanghong.chen/workspace/linux-rt-6.1.96/kernel/events/internal.h"

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_output_skip function");
// 定义回调函数类型
typedef int(*rb_alloc_aux_t)(struct perf_buffer *rb, struct perf_event *event,
		 pgoff_t pgoff, int nr_pages, long watermark, int flags);
// 回调函数指针
static rb_alloc_aux_t rb_alloc_aux_callback = NULL;

static struct perf_event *alloc_perf_event(void)
{
    struct perf_event_attr attr;
    struct perf_event *event;
    int cpu = smp_processor_id();
    memset(&attr, 0, sizeof(attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.size = sizeof(attr);
    attr.sample_type = PERF_SAMPLE_IP | PERF_SAMPLE_TID;
    attr.sample_period = 1000;
    attr.wakeup_events = 1;
    attr.precise_ip = 1;
    attr.exclude_kernel = 0;
    attr.exclude_hv = 0;
    event = perf_event_create_kernel_counter(&attr, cpu, NULL, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "perf_event_create_kernel_counter failed with error: %ld\n", PTR_ERR(event));
        return NULL;
    }
    return event;
}

static int __init test_module_init(void) {
    static struct perf_buffer my_rb;
    static struct perf_event *my_event;
    int ret = 0;
    pgoff_t pgoff = 0;
    int nr_pages = 4;
    long watermark = 0;
    int flags = 0;
    pr_info("Loading test_module\n");

    my_event = alloc_perf_event();

    rb_alloc_aux_callback = (rb_alloc_aux_t)addr1;
    rb_alloc_aux_callback(&my_rb, my_event, pgoff, nr_pages, watermark, flags);
    if (ret == 0) {
        printk(KERN_INFO "rb_alloc_aux successful.\n");
    } else {
        printk(KERN_ERR "rb_alloc_aux failed with error: %d\n", ret);
    }
    perf_event_release_kernel(my_event);

    return 0;
}

static void __exit test_module_exit(void) {
    pr_info("Unloading test_module\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A kernel module to test perf_output_skip");
