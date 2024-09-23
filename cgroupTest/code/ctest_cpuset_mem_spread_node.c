#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/sched.h>
#include <linux/slab.h>

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cpuset_slab_spread_node function");

typedef int (*cpuset_slab_spread_node_t)(void);

// 回调函数指针
static cpuset_slab_spread_node_t cpuset_slab_spread_node_callback = NULL;

// 声明外部函数
extern int cpuset_mem_spread_node(void);

static int __init test_module_init(void)
{
    int mem_node, slab_node;
    cpuset_slab_spread_node_callback = (cpuset_slab_spread_node_t)addr;

    printk(KERN_INFO "Loading test module...\n");

    // 调用 cpuset_mem_spread_node 函数
    mem_node = cpuset_mem_spread_node();
    printk(KERN_INFO "cpuset_mem_spread_node returned: %d\n", mem_node);

    slab_node = cpuset_slab_spread_node_callback();
    printk(KERN_INFO "cpuset_slab_spread_node returned: %d\n", slab_node);

    return 0;
}

static void __exit test_module_exit(void)
{
    printk(KERN_INFO "Unloading test module...\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("A module to call cpuset_mem_spread_node function");
