#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/perf_event.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/mm.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the perf_get_page_size function");
// 定义回调函数类型
typedef u64(*perf_get_page_size_t)(unsigned long addr);

// 回调函数指针
static perf_get_page_size_t perf_get_page_size_callback = NULL;

static int __init test_module_init(void)
{
    u64 ret;
    unsigned long addr;
    printk(KERN_INFO "Loading test module...\n");

    addr = (unsigned long)current->mm->start_code;
    perf_get_page_size_callback = (perf_get_page_size_t)addr1;
    printk(KERN_INFO "call perf_get_page_size, func address: %lx\n", addr1);
    // 创建内核计数器
    ret = perf_get_page_size_callback(addr);
    printk(KERN_INFO "page size for address %lx: %llu bytes\n", addr1, ret);
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
