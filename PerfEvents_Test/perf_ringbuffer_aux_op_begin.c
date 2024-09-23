#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>
#include <linux/ring_buffer.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/fdtable.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A simple Linux driver to call perf_aux_output_begin");

#define PROCFS_NAME "perf_fd"

static struct perf_event *event;
static struct perf_output_handle handle;

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
    void *result;
    void *aux_data;
    int ret;

    printk(KERN_INFO "perf_aux_output_begin start\n");
    // Allocate memory for the event structure
    
    event = alloc_perf_event();
    if (!event) {
        printk(KERN_ERR "Failed to allocate perf_event\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "Event: %p, rb: %p\n", event, event->rb);

    perf_event_enable(event);

    // Call the perf_aux_output_begin function
    result = perf_aux_output_begin(&handle, event);
    // if (!result) {
    //     printk(KERN_ERR "perf_aux_output_begin failed\n");
    //     perf_event_release_kernel(event);
    //     return -1;
    // }

    printk(KERN_INFO "perf_aux_output_begin succeeded\n");

    perf_aux_output_flag(&handle, PERF_AUX_FLAG_TRUNCATED);

    handle.event = NULL;
    printk(KERN_INFO "handle->event: %p\n", handle.event);

    aux_data = perf_get_aux(&handle);
    printk(KERN_INFO "aux_data: %p\n", aux_data);

    ret = perf_aux_output_skip(&handle, 1024);
    printk(KERN_ERR "perf_aux_output_skip ret:%d\n", ret);

    return 0;
}

static void __exit test_module_exit(void) {
    if (event) {
        perf_event_disable(event);
        perf_event_release_kernel(event);
    }
    printk(KERN_INFO "Exiting test module\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
