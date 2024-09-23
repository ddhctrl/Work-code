#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cgroup.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/rcupdate.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_VERSION("1.0");

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_path_from_kernfs_id function");

// 定义函数指针类型
typedef int (*cgroup_parse_float_t)(const char *input, unsigned dec_shift, s64 *v);

// 函数地址
static cgroup_parse_float_t cgroup_parse_float_ptr = NULL;

// 回调函数
static int my_callback(const char *input, unsigned dec_shift, s64 *v) {
    if (cgroup_parse_float_ptr) {
        return cgroup_parse_float_ptr(input, dec_shift, v);
    } else {
        pr_err("Function pointer is NULL\n");
        return -1;
    }
}

static int __init test_module_init(void)
{
    int ret;
    char *buf = "122.333";
    s64 v;

    cgroup_parse_float_ptr = (cgroup_parse_float_t)addr;
    ret = my_callback(buf, 2, &v); 

    if (!ret) {
        pr_info("ret:%d, v:%lld\n", ret, v);
        pr_info("Successfully called cgroup_parse_float\n");
    } else {
        pr_err("Failed to call cgroup_parse_float\n");
    }
    return 0;
}

static void __exit test_module_exit(void)
{
    pr_info("Unloading test_module\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
