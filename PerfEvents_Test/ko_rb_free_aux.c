#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include "/home/kanghong.chen/workspace/linux-rt-6.1.96/kernel/events/internal.h"

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_output_skip function");
// 定义回调函数类型
typedef void(*rb_free_aux_t)(struct perf_buffer *rb);
// 回调函数指针
static rb_free_aux_t rb_free_aux_callback = NULL;

static int __init test_module_init(void) {
    struct perf_buffer my_rb = {
        .aux_priv = NULL,
        .aux_nr_pages = 0,
    };
    pr_info("Loading test_module\n");

    refcount_set(&my_rb.aux_refcount, 1);
    rb_free_aux_callback = (rb_free_aux_t)addr1;
    rb_free_aux_callback(&my_rb);

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
