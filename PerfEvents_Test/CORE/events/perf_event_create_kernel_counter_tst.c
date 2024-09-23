#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

static struct perf_event *event = NULL;

static int __init test_perf_event_init(void) {
    struct perf_event_attr pe;
    int cpu = smp_processor_id();
    struct task_struct *task = current;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    event = perf_event_create_kernel_counter(&pe, cpu, task, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    printk(KERN_INFO "Perf event created successfully\n");

    return 0;
}

static void __exit test_perf_event_exit(void) {
    if (event) {
        perf_event_release_kernel(event);
        printk(KERN_INFO "Perf event released successfully\n");
    }
}

module_init(test_perf_event_init);
module_exit(test_perf_event_exit);

MODULE_LICENSE("GPL");
