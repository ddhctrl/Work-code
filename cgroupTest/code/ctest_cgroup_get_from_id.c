#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cgroup.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/rcupdate.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A simple kernel module to call cgroup_get_from_id");
MODULE_VERSION("1.0");

static u64 test_cgroup_id = 1;
module_param(test_cgroup_id, ullong, 0444);
MODULE_PARM_DESC(test_cgroup_id, "The cgroup ID to get");

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_path_from_kernfs_id function");

// 定义函数指针类型
typedef struct cgroup_subsys_state *(*css_from_id_t)(int id, struct cgroup_subsys *ss);

// 函数地址
static css_from_id_t css_from_id_ptr = NULL;

// 回调函数
static struct cgroup_subsys_state *my_callback(int id, struct cgroup_subsys *ss) {
    if (css_from_id_ptr) {
        return css_from_id_ptr(id, ss);
    } else {
        pr_err("Function pointer is NULL\n");
        return NULL;
    }
}

static int __init test_module_init(void)
{
    struct cgroup *cgrp;
    struct cgroup_subsys_state *css;

    pr_info("Loading test_module with cgroup_id = %llu\n", test_cgroup_id);

    cgrp = cgroup_get_from_id(test_cgroup_id);
    if (IS_ERR(cgrp)) {
        pr_err("Failed to get cgroup with id %llu, error: %ld\n", test_cgroup_id, PTR_ERR(cgrp));
        return PTR_ERR(cgrp);
    }

    pr_info("Successfully got cgroup with id %llu\n", test_cgroup_id);

    cgroup_put(cgrp);

    css_from_id_ptr = (css_from_id_t)addr;
    rcu_read_lock();
    css = my_callback(test_cgroup_id, &io_cgrp_subsys); // 需要用实际的cgroup_subsys替换
    rcu_read_unlock();

    if (css) {
        pr_info("Successfully called css_from_id\n");
    } else {
        pr_err("Failed to call css_from_id\n");
    }
    return 0;
}

static void __exit test_module_exit(void)
{
    pr_info("Unloading test_module\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
