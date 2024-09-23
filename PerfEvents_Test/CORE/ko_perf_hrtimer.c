#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_get_page_size function");
// 定义回调函数类型
typedef int(*perf_mux_hrtimer_restart_t)(struct perf_cpu_context *cpuctx);

// 回调函数指针
static perf_mux_hrtimer_restart_t perf_mux_hrtimer_restart_callback = NULL;

static struct pmu test_pmu;

static void test_pmu_enable(struct pmu *pmu)
{
	pr_info("test_pmu: enable\n");
}

static void test_pmu_disable(struct pmu *pmu)
{
	pr_info("test_pmu: disable\n");
}

static int test_pmu_event_init(struct perf_event *event)
{
	pr_info("test_pmu: event_init\n");
	return 0;
}

static int test_pmu_event_add(struct perf_event *event, int flags)
{
	pr_info("test_pmu: event_add\n");
	return 0;
}

static void test_pmu_event_start(struct perf_event *event, int flags)
{
	pr_info("test_pmu: event_start\n");
}

static void test_pmu_event_stop(struct perf_event *event, int flags)
{
	pr_info("test_pmu: event_stop\n");
}

static void test_pmu_event_del(struct perf_event *event, int flags)
{
	pr_info("test_pmu: event_del\n");
}

static void test_pmu_event_read(struct perf_event *event)
{
	pr_info("test_pmu: event_read\n");
}

static int __init test_module_init(void)
{
    int ret;
    struct perf_cpu_context *cpuctx;

    pr_info("Loading test_module\n");

    test_pmu = (struct pmu) {
		.name = "test_pmu",
		.type = PERF_TYPE_HARDWARE,
		.pmu_enable = test_pmu_enable,
		.pmu_disable = test_pmu_disable,
		.event_init = test_pmu_event_init,
		.add = test_pmu_event_add,
		.del = test_pmu_event_del,
		.start = test_pmu_event_start,
		.stop = test_pmu_event_stop,
		.read = test_pmu_event_read,
	};

    //device_initialize(my_pmu->dev);
    test_pmu.task_ctx_nr = -1; // 设置无效的任务上下文编号
    test_pmu.hrtimer_interval_ms = 100; // 设置定时器间隔为 100 毫秒

    // 注册 PMU
    ret = perf_pmu_register(&test_pmu, "test_pmu", -1);
    if (ret) {
        pr_err("Failed to register PMU: %d\n", ret);
        return ret;
    }
    pr_info("PMU and performance event registered successfully\n");

    perf_mux_hrtimer_restart_callback = (perf_mux_hrtimer_restart_t)addr1;
    cpuctx = per_cpu_ptr(test_pmu.pmu_cpu_context, 0);
    cpuctx->ctx.rotate_necessary = 1;

    smp_wmb(); 
    
    // 重启高精度定时器
    ret = perf_mux_hrtimer_restart_callback(cpuctx);
    if (ret) {
        pr_err("Failed to restart hrtimer: %d\n", ret);
        return ret;
    }
    pr_info("restart hrtimer successfully\n");

    return 0;
}

static void __exit test_module_exit(void)
{
    pr_info("unreister test_pmu\n");
    perf_pmu_unregister(&test_pmu);
    pr_info("Unloading test_module\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");