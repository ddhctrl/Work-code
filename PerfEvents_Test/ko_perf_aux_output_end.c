#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include "/home/kanghong.chen/workspace/linux-rt-6.1.96/kernel/events/internal.h"

static int __init test_module_init(void) {
    struct perf_event_mmap_page	u_p;
    struct perf_buffer my_rb = {
        .aux_priv = NULL,
        .aux_nr_pages = 0,
        .aux_watermark = 1,
        .user_page = &u_p,
    };
    struct perf_output_handle p_handle = {
        .aux_flags = 0,
        .rb = &my_rb,

    };
    pr_info("Loading test_module\n");

    refcount_set(&my_rb.aux_refcount, 0);
    perf_aux_output_end(&p_handle, 0);

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
