#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cpuset.h>
#include <linux/rcupdate.h>

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the cpuset_cpus_allowed_fallback function");

// 定义回调函数类型
typedef bool (*cpuset_fallback_cb)(struct task_struct *tsk);

// 回调函数指针
static cpuset_fallback_cb cpuset_fallback_callback = NULL;

// 初始化模块
static int __init test_module_init(void)
{
    struct task_struct *tsk = current; // 使用当前进程作为示例

    printk(KERN_INFO "Loading test module...\n");

    // 设置回调函数指针
    cpuset_fallback_callback = (cpuset_fallback_cb)addr;

    // 检查回调函数指针是否有效
    if (cpuset_fallback_callback) {
        bool result = cpuset_fallback_callback(tsk);
        printk(KERN_INFO "cpuset_cpus_allowed_fallback returned: %d\n", result);
    } else {
        printk(KERN_ERR "Failed to get cpuset_cpus_allowed_fallback function address\n");
        return -EFAULT;
    }

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
MODULE_DESCRIPTION("A module to call cpuset_cpus_allowed_fallback function");
