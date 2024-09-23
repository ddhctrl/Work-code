#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

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

static int __init test_pmu_init(void)
{
	int ret;

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

	ret = perf_pmu_register(&test_pmu, "test_pmu", -1);
	if (ret) {
		pr_err("test_pmu: failed to register pmu\n");
		return ret;
	}

	pr_info("test_pmu: registered\n");

	return 0;
}

static void __exit test_pmu_exit(void)
{
	perf_pmu_unregister(&test_pmu);
	pr_info("test_pmu: unregistered\n");
}

module_init(test_pmu_init);
module_exit(test_pmu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Test PMU unregister module");

