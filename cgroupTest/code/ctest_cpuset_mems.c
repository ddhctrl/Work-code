#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/kallsyms.h>
#include <linux/cgroup.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/namei.h>
#include <linux/errno.h>

//模块参数, 用于传递地址
static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the cpuset_print_current_mems_allowed function");

static unsigned long addr2 = 0;
module_param(addr2, ulong, 0444);
MODULE_PARM_DESC(addr2, "Address of the __cpuset_memory_pressure_bump function");

typedef void (*cpuset_print_current_mems_allowed_t)(void);
typedef void (*__cpuset_memory_pressure_bump_t)(void);

// 回调函数指针
static cpuset_print_current_mems_allowed_t cpuset_mems_callback = NULL;
static __cpuset_memory_pressure_bump_t cpuset_mem_pressure_callback = NULL;

// 通过文件指针获取 dentry 并调用 cgroupstats_build
// static int my_cgroupstats_build()
// {
//     int ret;

//     return ret;
// }

static int __init test_module_init(void)
{
    // 将地址转换为函数指针
    cpuset_mems_callback = (cpuset_print_current_mems_allowed_t)addr1;
    if (!cpuset_mems_callback) {
        printk(KERN_ERR "Failed to get cpuset_print_current_mems_allowed function address\n");
        return -EFAULT;
    }

    cpuset_mem_pressure_callback = (__cpuset_memory_pressure_bump_t)addr2;
    if (!cpuset_mem_pressure_callback) {
        printk(KERN_ERR "Failed to get __cpuset_memory_pressure_bump function address\n");
        return -EFAULT;
    }

    cpuset_mems_callback();
    printk(KERN_INFO "print success\n");

    cpuset_mem_pressure_callback();
    printk(KERN_INFO "Display memory_pressure success\n");

    return 0;
}

static void __exit test_module_exit(void)
{
    printk(KERN_INFO "test module exited\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A module to call cgroupstats_build function");
