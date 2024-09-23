#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/err.h>

// 假设事件和刷新值
#define SAMPLE_REFRESH_VALUE 1000

static struct perf_event *event = NULL;

static int __init test_module_init(void) {
    struct perf_event_attr pe_attr;
    int cpu = smp_processor_id();  // 获取当前 CPU ID
    int ret;

    // 初始化 perf_event_attr 结构体
    memset(&pe_attr, 0, sizeof(struct perf_event_attr));
    pe_attr.type = PERF_TYPE_HARDWARE;
    pe_attr.size = sizeof(struct perf_event_attr);
    pe_attr.config = PERF_COUNT_HW_CPU_CYCLES;
    pe_attr.sample_period = 100000;  // 设置采样周期
    pe_attr.sample_type = PERF_SAMPLE_PERIOD;
    pe_attr.inherit = 0;  // 确保 inherit 为假
    pe_attr.disabled = 1;
    pe_attr.exclude_kernel = 1;
    pe_attr.exclude_hv = 1;

    // 创建性能事件
    event = perf_event_create_kernel_counter(&pe_attr, cpu, NULL, NULL, NULL);
    if (IS_ERR(event)) {
        pr_err("Failed to create perf event\n");
        return PTR_ERR(event);
    }

    // 调用 perf_event_refresh 函数
    ret = perf_event_refresh(event, SAMPLE_REFRESH_VALUE);
    if (ret) {
        pr_err("perf_event_refresh failed: %d\n", ret);
        perf_event_release_kernel(event);
        return ret;
    }

    pr_info("perf_event_refresh succeeded\n");
    return 0;
}

static void __exit test_module_exit(void) {
    if (event) {
        perf_event_release_kernel(event);
    }
    pr_info("Module unloaded\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
