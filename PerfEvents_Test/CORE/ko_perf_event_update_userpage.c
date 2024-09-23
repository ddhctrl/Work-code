#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>
#include <linux/preempt.h>
#include <linux/cgroup.h>
#include <linux/sched.h>
#include <linux/fs.h>

static struct perf_event *event;
static struct perf_event_mmap_page *user_page;
static struct perf_buffer *rb;
static struct cgroup *cgrp;

static int __init test_module_init(void)
{
    pr_info("Initializing mymodule\n");

    // 获取 cgroup
    cgrp = cgroup_get_from_path("/sys/fs/cgroup");
    if (!cgrp) {
        pr_err("Failed to get cgroup\n");
        return -ENOENT;
    }

    // 分配并初始化 perf_event 结构体
    event = kzalloc(sizeof(struct perf_event), GFP_KERNEL);
    if (!event) {
        pr_err("Failed to allocate memory for perf_event\n");
        cgroup_put(cgrp);
        return -ENOMEM;
    }

    // 初始化事件字段
    event->hw.sample_period = 6000;
    event->ctx = kzalloc(sizeof(struct perf_event_context), GFP_KERNEL);
    if (!event->ctx) {
        pr_err("Failed to allocate memory for perf_event_context\n");
        kfree(user_page);
        kfree(rb);
        kfree(event);
        cgroup_put(cgrp);
        return -ENOMEM;
    }
    event->ctx->time = ktime_get_ns();

    // 分配并初始化 perf_event_mmap_page
    user_page = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (!user_page) {
        pr_err("Failed to allocate memory for user_page\n");
        kfree(event->ctx);
        kfree(event);
        cgroup_put(cgrp);
        return -ENOMEM;
    }

    // 初始化 user_page（示例，不一定是实际需要的初始化）
    user_page->version = 1;
    user_page->compat_version = 0;
    user_page->lock = 0;

    // 关联 cgroup
    event->cgrp = cgrp;
    
    event->rb = NULL;

    // 调用 perf_event_update_userpage
    perf_event_update_userpage(event);

    return 0;
}

static void __exit test_module_exit(void)
{
    pr_info("Exiting test module\n");

    // 释放内存
    kfree(event->ctx);
    kfree(user_page);
    kfree(event);
    cgroup_put(event->cgrp);
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
