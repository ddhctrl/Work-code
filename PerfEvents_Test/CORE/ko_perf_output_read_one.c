#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/perf_event.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>
#include "/home/kanghong.chen/workspace/linux-rt-6.1.96/kernel/events/internal.h"

static struct task_struct *child_task;
static struct perf_event *event;
static struct perf_output_handle handle;

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_output_read function");
// 定义回调函数类型
typedef void(*perf_output_read_t)(struct perf_output_handle *handle,
			     struct perf_event *event);

// 回调函数指针
static perf_output_read_t perf_output_read_callback = NULL;

/* 模拟一些计算任务 */
static void do_some_work(void) {
    volatile int i;
    for (i = 0; i < 100000000; i++) {
        cpu_relax(); // 内核中的忙等待
    }
}

/* 子任务的内核线程函数 */
static int child_function(void *data) {
    printk(KERN_INFO "Child task started\n");
    do_some_work();  // 子任务执行计算任务
    printk(KERN_INFO "Child task finished\n");
    return 0;
}

static int init_perf_output_handle(struct perf_output_handle *handle)
{
    // 分配 perf_buffer 结构
    handle->rb = kmalloc(sizeof(struct perf_buffer) + sizeof(void *) * 1, GFP_KERNEL);
    if (!handle->rb)
        return -ENOMEM;

    // 设置页数为 1
    handle->rb->nr_pages = 1;

    // 分配页指针数组（因为页数为1，所以是一个指针）
    handle->rb->data_pages[0] = (void *)__get_free_page(GFP_KERNEL);
    if (!handle->rb->data_pages[0])
    {
        kfree(handle->rb);
        return -ENOMEM;
    }

    // 初始化 handle 结构
    handle->page = 0;                                  // 当前页索引为 0
    handle->addr = handle->rb->data_pages[0];           // 当前页的起始地址
    handle->size = PAGE_SIZE;                           // 当前页的大小为 PAGE_SIZE

    return 0;  // 成功
}

// 释放 handle->rb 资源
static void free_perf_output_handle(struct perf_output_handle *handle)
{
    if (handle->rb) {
        if (handle->rb->data_pages) {
            if (handle->rb->data_pages[0]) {
                free_page((unsigned long)handle->rb->data_pages[0]);
            }
        }
        kfree(handle->rb);
    }
}

/* 初始化内核模块时，创建性能事件并执行任务 */
static int __init perf_event_module_init(void) {
    struct perf_event_attr pe;
    s64 count;
    u64 enabled_time, running_time;
    int ret = 0;

    memset(&handle, 0, sizeof(struct perf_output_handle));
    ret = init_perf_output_handle(&handle);
    if (ret) {
        printk(KERN_ERR "Error creating perf event\n");
        return PTR_ERR(event);
    }


    /* 初始化 perf_event_attr 结构 */
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_CPU_CYCLES;  // 计数 CPU 周期数
    pe.disabled = 0;  // 创建时禁用
    pe.inherit = 1;   // 子任务继承
    pe.exclude_kernel = 1;  // 不计入内核时间
    pe.exclude_hv = 1;      // 不计入 hypervisor 时间

    /* 创建内核级性能事件 */
    event = perf_event_create_kernel_counter(&pe, -1, current, NULL, NULL);
    if (IS_ERR(event)) {
        printk(KERN_ERR "Error creating perf event\n");
        free_perf_output_handle(&handle);
        return PTR_ERR(event);
    }


    /* 创建子任务 */
    child_task = kthread_run(child_function, NULL, "child_task");
    if (IS_ERR(child_task)) {
        printk(KERN_ERR "Error creating child task\n");
        free_perf_output_handle(&handle);
        return PTR_ERR(child_task);
    }

    perf_output_read_callback = (perf_output_read_t)addr1;
    perf_output_read_callback(&handle, event);

    /* 等待子任务结束 */
    kthread_stop(child_task);

    /* 父任务也执行一些计算任务 */
    do_some_work();

    /* 禁用性能事件并读取计数值 */
    count = perf_event_read_value(event, &enabled_time, &running_time);

    printk(KERN_INFO "CPU cycles: %lld, Enabled time: %llu, Running time: %llu\n", 
        count, enabled_time, running_time);

    /* 释放性能事件 */
    perf_event_release_kernel(event);

    free_perf_output_handle(&handle);
    return 0;
}

/* 卸载内核模块 */
static void __exit perf_event_module_exit(void) {
    printk(KERN_INFO "Module exiting\n");
}

module_init(perf_event_module_init);
module_exit(perf_event_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module using perf_event_create_kernel_counter");
