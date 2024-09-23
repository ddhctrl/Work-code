#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/mm.h>

static struct perf_event *event = NULL;
static struct page **pages = NULL;
static int nr_pages = 8;  // 设置缓冲区大小，单位为页

static void perf_event_callback(struct perf_event *event, struct perf_sample_data *data, struct pt_regs *regs)
{
    struct perf_output_handle handle;
    int ret;
    unsigned int size = 128;  // 要读取的数据大小（字节）

    // 尝试从缓冲区开始向前读取数据
    ret = perf_output_begin_forward(&handle, data, event, size);
    if (ret) {
        printk(KERN_ERR "Failed to begin perf output: %d\n", ret);
        return;
    }

    // 这里可以处理读取到的数据
    // handle中已经包含了从缓冲区读取的数据，您可以根据需要处理这些数据
    printk(KERN_INFO "Read perf output forward: size = %u\n", size);

    // 结束访问缓冲区
    perf_output_end(&handle);
}

static int __init test_module_init(void)
{
    struct perf_event_attr attr = {};

    printk(KERN_INFO "Loading test module...\n");

    // 设置 perf_event_attr
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.sample_period = 1000;  // 设置采样周期
    attr.sample_type = PERF_SAMPLE_READ;
    attr.disabled = 0;
    attr.exclude_kernel = 0;
    attr.exclude_hv = 0;
    attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;

    // 创建内核计数器
    event = perf_event_create_kernel_counter(&attr, -1, current, perf_event_callback, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Failed to create perf event: %ld\n", PTR_ERR(event));
        return PTR_ERR(event);
    }

    // 打印一些调试信息
    printk(KERN_INFO "perf_event: %p\n", event);
    printk(KERN_INFO "event->state: %d\n", event->state);
    printk(KERN_INFO "event->ctx: %p\n", event->ctx);

    if (!event->ctx) {
        printk(KERN_ERR "event->ctx is NULL\n");
        perf_event_release_kernel(event);
        return -EINVAL;
    }

    pages = alloc_pages_exact(nr_pages * PAGE_SIZE, GFP_KERNEL);
    if (!pages) {
        printk(KERN_ERR "Failed to allocate pages for buffer\n");
        perf_event_release_kernel(event);
        return -ENOMEM;
    }

    // 关联缓冲区
    if (mmap(event, pages, nr_pages)) {
        printk(KERN_ERR "Failed to map perf event buffer\n");
        free_pages_exact(pages, nr_pages * PAGE_SIZE);
        perf_event_release_kernel(event);
        return -ENOMEM;
    }

    return 0;
}

static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Unloading test module...\n");
    // 释放缓冲区
    if (pages) {
        free_pages_exact(pages, nr_pages * PAGE_SIZE);
    }
    perf_event_release_kernel(event);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
