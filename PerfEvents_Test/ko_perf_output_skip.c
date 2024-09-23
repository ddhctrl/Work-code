#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the perf_output_skip function");
// 定义回调函数类型
typedef unsigned int (*perf_output_skip_t)(struct perf_output_handle *handle,
			      unsigned int len);
// 回调函数指针
static perf_output_skip_t perf_output_skip_callback = NULL;

static struct perf_event *event;
static struct perf_event_attr attr;

// 处理性能事件的函数
static void process_perf_event(struct perf_event *event) {
    struct perf_output_handle handle;
    struct perf_sample_data sample_data;
    unsigned int ret = 0;
    unsigned int skip_length = 128; // 要跳过的长度

    // 初始化 perf_sample_data
    memset(&sample_data, 0, sizeof(sample_data));

    perf_output_skip_callback = (perf_output_skip_t)addr2;

    // 跳过指定长度的数据
    ret = perf_output_skip_callback(&handle, skip_length);
    pr_info("perf output skip ret: %d\n", ret);

    // 结束输出操作
    //perf_output_end(&handle);
}

static int __init test_module_init(void) {
    pr_info("Loading test_module\n");

    // 配置 perf_event_attr 结构体
    memset(&attr, 0, sizeof(struct perf_event_attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.disabled = 1;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.read_format = PERF_FORMAT_GROUP;

    // 创建性能事件
    event = perf_event_create_kernel_counter(&attr, -1, current, NULL, NULL);
    if (IS_ERR(event)) {
        pr_err("Error creating perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    // 处理性能事件
    process_perf_event(event);

    // 释放性能事件
    perf_event_release_kernel(event);

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
