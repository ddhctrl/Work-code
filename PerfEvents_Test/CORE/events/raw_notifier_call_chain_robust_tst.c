#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Test module for raw_notifier_call_chain_robust");
MODULE_VERSION("1.0");

/* 定义并初始化原始通知链头部 */
static RAW_NOTIFIER_HEAD(test_raw_notifier_chain);

/* 定义回调函数 */
static int notifier_callback1(struct notifier_block *nb, unsigned long val_up, void *data)
{
    pr_info("Notifier Callback 1: val_up=%lu, Data=%s\n", val_up, (char *)data);
    return NOTIFY_OK;
}

static int notifier_callback2(struct notifier_block *nb, unsigned long val_up, void *data)
{
    pr_info("Notifier Callback 2: val_up=%lu, Data=%s\n", val_up, (char *)data);
    if (val_up > 100) {
        pr_info("Notifier Callback 2: Triggering a fallback using val_down\n");
        return NOTIFY_STOP;
    }
    return NOTIFY_OK;
}

static int notifier_callback3(struct notifier_block *nb, unsigned long val_up, void *data)
{
    pr_info("Notifier Callback 3: val_up=%lu, Data=%s\n", val_up, (char *)data);
    return NOTIFY_OK;
}

/* 定义 notifier_block 实例 */
static struct notifier_block nb1 = {
    .notifier_call = notifier_callback1,
    .priority = 1,
};

static struct notifier_block nb2 = {
    .notifier_call = notifier_callback2,
    .priority = 2,
};

static struct notifier_block nb3 = {
    .notifier_call = notifier_callback3,
    .priority = 3,
};

static int __init test_raw_notifier_init(void)
{
    int ret;

    pr_info("Test Raw Notifier Robust Module Loaded\n");

    /* 注册回调函数到原始通知链 */
    ret = raw_notifier_chain_register(&test_raw_notifier_chain, &nb1);
    if (ret) {
        pr_err("Failed to register nb1\n");
        return ret;
    }

    ret = raw_notifier_chain_register(&test_raw_notifier_chain, &nb2);
    if (ret) {
        pr_err("Failed to register nb2\n");
        raw_notifier_chain_unregister(&test_raw_notifier_chain, &nb1);
        return ret;
    }

    ret = raw_notifier_chain_register(&test_raw_notifier_chain, &nb3);
    if (ret) {
        pr_err("Failed to register nb3\n");
        raw_notifier_chain_unregister(&test_raw_notifier_chain, &nb1);
        raw_notifier_chain_unregister(&test_raw_notifier_chain, &nb2);
        return ret;
    }

    pr_info("Registered three notifier callbacks\n");

    /* 触发通知链 */
    pr_info("Calling raw_notifier_call_chain_robust with val_up=50, val_down=200 and data='Test Data'\n");
    raw_notifier_call_chain_robust(&test_raw_notifier_chain, 50, 200, "Test Data");

    pr_info("Calling raw_notifier_call_chain_robust with val_up=150, val_down=200 and data='Test Data'\n");
    raw_notifier_call_chain_robust(&test_raw_notifier_chain, 150, 200, "Test Data");

    return 0;
}

static void __exit test_raw_notifier_exit(void)
{
    pr_info("Test Raw Notifier Robust Module Unloaded\n");

    /* 注销回调函数 */
    raw_notifier_chain_unregister(&test_raw_notifier_chain, &nb1);
    raw_notifier_chain_unregister(&test_raw_notifier_chain, &nb2);
    raw_notifier_chain_unregister(&test_raw_notifier_chain, &nb3);

    pr_info("Unregistered notifier callbacks\n");
}

module_init(test_raw_notifier_init);
module_exit(test_raw_notifier_exit);

