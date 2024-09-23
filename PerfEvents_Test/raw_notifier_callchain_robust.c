#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/notifier.h>

#define EVENT_A 0x01    
#define EVENT_B 0x02

static RAW_NOTIFIER_HEAD(raw_chain_list);   // 定义通知链列表

// 定义回调函数
static int raw_notifier_callback(struct notifier_block *nb, unsigned long event, void *v)
{
    printk(KERN_INFO "raw_notifier_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

// 定义通知块
// 指定通知回调函数
static struct notifier_block raw_notif = { .notifier_call = raw_notifier_callback };

static int __init notifier_chains_init(void)
{
    int ret;

    //注册
    printk(KERN_INFO "Registering notifier chains\n");
    ret = raw_notifier_chain_register(&raw_chain_list, &raw_notif);
    if (ret) {
        printk(KERN_ERR "Failed to register notifier\n");
        return ret;
    }
    printk(KERN_ERR "notifier registered success\n");

    //触发事件通知
    printk(KERN_INFO "Calling raw notifier chain\n");
    ret = raw_notifier_call_chain_robust(&raw_chain_list, EVENT_A, EVENT_B, NULL);
    printk(KERN_INFO "Notifier chain called, result: %d\n", ret);

    return 0;
}

static void __exit notifier_chains_exit(void)
{
    printk(KERN_INFO "Unregistering notifier chains\n");
    raw_notifier_chain_unregister(&raw_chain_list, &raw_notif);
}

module_init(notifier_chains_init);
module_exit(notifier_chains_exit);

MODULE_AUTHOR("CKH");
MODULE_LICENSE("GPL");
