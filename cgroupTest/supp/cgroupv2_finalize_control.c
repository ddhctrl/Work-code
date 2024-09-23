#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_e_css function");

// 定义回调函数类型
typedef void (*cgroup_finalize_control_t)(struct cgroup *cgrp, int ret);

// 回调函数指针
static cgroup_finalize_control_t cgroup_finalize_control_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct cgroup *cgrp;
    int ret = 1;
    const char *cgroup_path = "/Example";

    printk(KERN_INFO "Loading test module...\n");

    cgrp = cgroup_get_from_path(cgroup_path);
    if (IS_ERR(cgrp))
    {
        pr_err("Failed to get cgroup from path: %s, error: %ld\n", cgroup_path, PTR_ERR(cgrp));
        return PTR_ERR(cgrp);
    }

    // 设置回调函数指针
    cgroup_finalize_control_callback = (cgroup_finalize_control_t)addr;

    cgroup_finalize_control_callback(cgrp, ret);

    cgroup_put(cgrp);

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
