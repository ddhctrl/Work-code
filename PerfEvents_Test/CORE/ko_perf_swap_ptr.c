#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

static unsigned long addr1 = 0;
module_param(addr1, ulong, 0444);
MODULE_PARM_DESC(addr1, "Address of the swap_ptr function");
// 定义回调函数类型
typedef void(*swap_ptr_t)(void *l, void *r);

// 回调函数指针
static swap_ptr_t swap_ptr_callback = NULL;

/* 初始化模块 */
static int __init my_module_init(void)
{
    void *ptr1, *ptr2;
    void *temp1, *temp2;

    pr_info("Loading my_module\n");

    /* 动态分配两个内存块 */
    temp1 = kmalloc(100, GFP_KERNEL);
    if (!temp1)
        return -ENOMEM;

    temp2 = kmalloc(100, GFP_KERNEL);
    if (!temp2) {
        kfree(temp1);
        return -ENOMEM;
    }

    /* 初始化指针变量 */
    ptr1 = temp1;
    ptr2 = temp2;

    pr_info("Before swap: ptr1 = %p, ptr2 = %p\n", ptr1, ptr2);

    /* 调用 swap_ptr 交换指针 */
    swap_ptr_callback = (swap_ptr_t)addr1;
    swap_ptr_callback(&ptr1, &ptr2);

    pr_info("After swap: ptr1 = %p, ptr2 = %p\n", ptr1, ptr2);

    /* 释放分配的内存 */
    kfree(temp1);
    kfree(temp2);

    return 0;
}

/* 卸载模块 */
static void __exit my_module_exit(void)
{
    pr_info("Unloading my_module\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple module to demonstrate swap_ptr function");
