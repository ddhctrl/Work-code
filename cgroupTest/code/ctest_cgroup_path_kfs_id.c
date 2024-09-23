#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cgroup_path_from_kernfs_id function");

// 定义函数指针类型
typedef void (*cgroup_path_from_kernfs_id_t)(u64 id, char *buf, size_t buflen);

// 函数地址
static cgroup_path_from_kernfs_id_t cgroup_path_from_kernfs_id_ptr;

// 回调函数
void my_callback(u64 id) {
    char buf[256];

    if (cgroup_path_from_kernfs_id_ptr) {
        cgroup_path_from_kernfs_id_ptr(id, buf, sizeof(buf));
        pr_info("cgroup path: %s\n", buf);
    } else {
        pr_err("Function pointer is NULL\n");
    }
}

static int __init test_module_init(void)
{
    // 获取函数地址
    cgroup_path_from_kernfs_id_ptr = (cgroup_path_from_kernfs_id_t)addr;

    pr_info("Calling callback with id 1\n");
    my_callback(1);

    return 0;
}

static void __exit test_module_exit(void)
{
    pr_info("Unloading test module\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A simple kernel module to call cgroup_path_from_kernfs_id");
MODULE_VERSION("1.0");

module_init(test_module_init);
module_exit(test_module_exit);
