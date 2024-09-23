#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/fdtable.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static unsigned long addr1 = 0;
static int fd = -1;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_pmu_resched function");
module_param(fd, int, 0444);
MODULE_PARM_DESC(fd, "File descriptor of the perf_event");

// 定义回调函数类型
typedef void (*perf_pmu_resched_t)(struct pmu *pmu);

// 回调函数指针
static perf_pmu_resched_t perf_pmu_resched_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct pmu *pmu;
    struct perf_event *event;
    struct file *event_file;
    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    perf_pmu_resched_callback = (perf_pmu_resched_t)addr1;

    printk(KERN_INFO "perf_pmu_resched_callback set to %lx\n", addr1);

    // 获取文件对象
    event_file = fget(fd);
    if (!event_file) {
        printk(KERN_ERR "Failed to get file from fd\n");
        return -EINVAL;
    }

    // 获取 perf_event 对象
    event = event_file->private_data;
    if (!event) {
        printk(KERN_ERR "Failed to get perf event from file\n");
        fput(event_file);
        return -EINVAL;
    }

    // 获取 pmu 对象
    pmu = event->pmu;
    if (!pmu) {
        printk(KERN_ERR "Failed to get PMU from event\n");
        fput(event_file);
        return -EINVAL;
    }

    perf_pmu_resched_callback(pmu);

    printk(KERN_INFO "perf_pmu_resched success");

    fput(event_file);

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
