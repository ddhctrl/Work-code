#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_e_css function");

// 定义回调函数类型
typedef struct cgroup_subsys_state *(*cgroup_e_css_t)(struct cgroup *cgrp, struct cgroup_subsys *ss);

// 回调函数指针
static cgroup_e_css_t cgroup_e_css_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct cgroup *cgrp;
    struct cgroup_subsys *ss;
    struct cgroup_subsys_state *css;

    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    cgroup_e_css_callback = (cgroup_e_css_t)addr;

     // 获取当前进程的 cgroup
    cgrp = task_cgroup(current, io_cgrp_id);

    // 获取目标 cgroup 子系统
    ss = &io_cgrp_subsys; 

    // 调用 cgroup_e_css
    css = cgroup_e_css_callback(cgrp, ss);
    if (css)
        pr_info("cgroup_e_css returned a valid css\n");
    else
        pr_info("cgroup_e_css returned NULL\n");

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
