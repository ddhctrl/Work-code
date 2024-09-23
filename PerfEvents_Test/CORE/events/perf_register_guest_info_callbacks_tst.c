#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/rcupdate.h>
#include <linux/static_call.h>

/* 定义回调函数 */
static unsigned int example_guest_state(void)
{
	printk(KERN_INFO "example_guest_state called\n");
	return 0;
}

static unsigned long example_guest_get_ip(void)
{
	printk(KERN_INFO "example_guest_get_ip called\n");
	return 0;
}

static unsigned int example_handle_intel_pt_intr(void)
{
	printk(KERN_INFO "example_handle_intel_pt_intr called\n");
	return 0;
}

/* 定义perf_guest_info_callbacks结构 */
static struct perf_guest_info_callbacks example_callbacks = {
	.state = example_guest_state,
	.get_ip = example_guest_get_ip,
	.handle_intel_pt_intr = example_handle_intel_pt_intr,
};

/* 模块初始化函数 */
static int __init test_module_init(void)
{
	printk(KERN_INFO "Loading test module...\n");
	perf_register_guest_info_callbacks(&example_callbacks);
	printk(KERN_INFO "perf_register_guest_info_callbacks called\n");
	return 0;
}

/* 模块退出函数 */
static void __exit test_module_exit(void)
{
	printk(KERN_INFO "Unloading test module...\n");
}

module_init(test_module_init);
module_exit(test_module_exit);

MODULE_LICENSE("GPL");
