#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/atomic.h>
#include <linux/printk.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/types.h>

// 包含内部头文件
#include "/home/kanghong.chen/workspace/linux-rt-6.1.12/kernel/events/internal.h"

MODULE_LICENSE("GPL");

#define PERF_MMAP_OPEN_ADDR 0xffff80000823e2d0

typedef void (*perf_mmap_open_t)(struct vm_area_struct *vma);

static struct vm_area_struct vma;
static struct perf_event event;
static struct perf_buffer buf;
static struct pmu pmu;

// 函数指针
perf_mmap_open_t perf_mmap_open_fp = (perf_mmap_open_t)PERF_MMAP_OPEN_ADDR;

static int __init perf_mmap_open_test_init(void)
{
    pr_info("perf_mmap_open_test: Module loaded\n");

    // 初始化 perf_buffer 结构体
    atomic_set(&buf.mmap_count, 0);
    atomic_set(&buf.aux_mmap_count, 0);

    // 初始化 perf_event 结构体
    atomic_set(&event.mmap_count, 0);
    event.rb = &buf;  // 注意这里改成了 perf_buffer
    event.pmu = &pmu;

    // 初始化 pmu 结构体
    pmu.event_mapped = NULL; // 如果需要，可以设置为有效的回调函数

    // 初始化 vm_area_struct
    vma.vm_file = (struct file *)kmalloc(sizeof(struct file), GFP_KERNEL);
    if (!vma.vm_file) {
        pr_err("perf_mmap_open_test: Failed to allocate vm_file\n");
        return -ENOMEM;
    }
    vma.vm_file->private_data = &event;
    vma.vm_pgoff = 0; // 或设置为非零值以测试 aux_mmap_count

    // 通过函数指针调用函数
    perf_mmap_open_fp(&vma);

    // 检查结果
    pr_info("mmap_count: %d\n", atomic_read(&event.mmap_count));
    pr_info("buf.mmap_count: %d\n", atomic_read(&event.rb->mmap_count));
    pr_info("buf.aux_mmap_count: %d\n", atomic_read(&event.rb->aux_mmap_count));

    return 0;
}

static void __exit perf_mmap_open_test_exit(void)
{
    kfree(vma.vm_file);
    pr_info("perf_mmap_open_test: Module unloaded\n");
}

module_init(perf_mmap_open_test_init);
module_exit(perf_mmap_open_test_exit);

