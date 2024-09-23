#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/srcu.h>
#include <linux/reboot.h>
#include <linux/kdebug.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/buffer_head.h> 

#define EVENT_A 0x01    
#define EVENT_B 0x02
#define PROC_FILENAME "notify_die_address"

 static char address_str[20] = {0};
module_param_string(addr, address_str, 20, 0444);
MODULE_PARM_DESC(addr, "Address string");

typedef int (*notify_die_fn_t)(enum die_val val, const char *str,
                               struct pt_regs *regs, long err, int trap, int sig);

static notify_die_fn_t notify_die_fn = NULL;
unsigned long notify_die_address = 0;

static RAW_NOTIFIER_HEAD(raw_chain_list);   // 定义通知链列表
static BLOCKING_NOTIFIER_HEAD(blocking_chain_list);
static ATOMIC_NOTIFIER_HEAD(atomic_chain_list);

static struct srcu_notifier_head srcu_chain_list;

// 定义回调函数
static int raw_notifier_callback(struct notifier_block *nb, unsigned long event, void *v)
{
    printk(KERN_INFO "raw_notifier_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

static int blocking_notifier_callback(struct notifier_block *nb, unsigned long event, void *v)
{
    printk(KERN_INFO "blocking_notifier_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

static int blocking_notifier_high_prio_callback(struct notifier_block *nb, unsigned long event, void *v) {
    printk(KERN_INFO "blocking_notifier_high_prio_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

static int atomic_notifier_callback(struct notifier_block *nb, unsigned long event, void *v)
{
    printk(KERN_INFO "atomic_notifier_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

static int atomic_notifier_high_prio_callback(struct notifier_block *nb, unsigned long event, void *v) {
    printk(KERN_INFO "atomic_notifier_high_prio_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

static int srcu_notifier_callback(struct notifier_block *nb, unsigned long event, void *v)
{
    printk(KERN_INFO "srcu_notifier_callback: event %lu\n", event);

    return NOTIFY_DONE;
}

static int die_notifier_callback(struct notifier_block *nb, unsigned long event, void *v) {
    printk(KERN_ALERT "Die notifier callback: event %lu\n", event);
    return NOTIFY_DONE;
}

static int reboot_notifier_callback(struct notifier_block *nb, unsigned long event, void *v) {
    printk(KERN_ALERT "reboot notifier callback: event %lu\n", event);
    return NOTIFY_DONE;
}

// 定义通知块
// 指定通知回调函数
static struct notifier_block raw_notif = { .notifier_call = raw_notifier_callback };
static struct notifier_block blocking_notif = { .notifier_call = blocking_notifier_callback, .priority = 1 };
static struct notifier_block blocking_notif_high_prio = { .notifier_call = blocking_notifier_high_prio_callback, .priority = 2 };
static struct notifier_block atomic_notif = { .notifier_call = atomic_notifier_callback, .priority = 1 };
static struct notifier_block atomic_notif_high_prio = { .notifier_call = atomic_notifier_high_prio_callback, .priority = 2 };
static struct notifier_block srcu_notif = { .notifier_call = srcu_notifier_callback };
static struct notifier_block die_notif = { .notifier_call = die_notifier_callback };
static struct notifier_block reboot_notif = { .notifier_call = reboot_notifier_callback, .priority = 0 };

void set_notify_die_callback(notify_die_fn_t fn) {
    notify_die_fn = fn;
}

static int __init notifier_chains_init(void)
{
    int ret;
    srcu_init_notifier_head(&srcu_chain_list);

    printk(KERN_INFO "address str:%s\n", address_str);
    ret = kstrtoul(address_str, 16, &notify_die_address);
    if (ret) {
        printk(KERN_ERR "kstrtoul error, ret:%d\n", ret);
        return ret;
    }
    printk(KERN_INFO "notify die address:%lx\n", notify_die_address);
    notify_die_fn = (notify_die_fn_t)notify_die_address;

    //注册
    printk(KERN_INFO "Registering notifier chains\n");
    raw_notifier_chain_register(&raw_chain_list, &raw_notif);
    
    ret = blocking_notifier_chain_register_unique_prio(&blocking_chain_list, &blocking_notif);
    if (ret) {
        printk(KERN_ERR "Failed to register blocking notifier with priority 1\n");
    }
    ret = blocking_notifier_chain_register_unique_prio(&blocking_chain_list, &blocking_notif_high_prio);
    if (ret) {
        printk(KERN_ERR "Failed to register blocking notifier with priority 2\n");
    }
    
    ret = atomic_notifier_chain_register(&atomic_chain_list, &atomic_notif);
    if (ret) {
        printk(KERN_ERR "Failed to register atomic notifier with priority 1\n");
    }

    // 使用 atomic_notifier_chain_register_unique_prio 注册具有唯一优先级的通知块到原子通知链
    ret = atomic_notifier_chain_register_unique_prio(&atomic_chain_list, &atomic_notif_high_prio);
    if (ret) {
        printk(KERN_ERR "Failed to register atomic notifier with priority 2\n");
    }
    srcu_notifier_chain_register(&srcu_chain_list, &srcu_notif);

    ret = register_die_notifier(&die_notif);
    if (ret) {
        printk(KERN_ERR "Failed to register die notifier\n");
    }

    ret = register_reboot_notifier(&reboot_notif);
    if (ret) {
        printk(KERN_ERR "Failed to register reboot notifier\n");
    }

    //触发事件通知
    printk(KERN_INFO "Calling raw notifier chain\n");
    raw_notifier_call_chain(&raw_chain_list, EVENT_A, NULL);
    printk(KERN_INFO "Calling blocking notifier chain\n");
    blocking_notifier_call_chain(&blocking_chain_list, EVENT_B, NULL);
    printk(KERN_INFO "Calling atomic notifier chain\n");
    atomic_notifier_call_chain(&atomic_chain_list, EVENT_B, NULL);
    printk(KERN_INFO "Calling srcu notifier chain\n");
    srcu_notifier_call_chain(&srcu_chain_list, EVENT_A, NULL);

    if (notify_die_fn) {
        struct pt_regs regs = { 0 };
        printk(KERN_ALERT "Simulating a die event\n");
        notify_die_fn(DIE_OOPS, "Simulated die event", &regs, 0, 0, SIGSEGV);
    } else {
        printk(KERN_ERR "notify_die_fn is not set\n");
    }

    if(kernel_can_power_off()) {
        printk(KERN_INFO "power-off handler is registered and system can be powered off\n");
    }

    return 0;
}

static void __exit notifier_chains_exit(void)
{
    printk(KERN_INFO "Unregistering notifier chains\n");
    raw_notifier_chain_unregister(&raw_chain_list, &raw_notif);
    blocking_notifier_chain_unregister(&blocking_chain_list, &blocking_notif);
    blocking_notifier_chain_unregister(&blocking_chain_list, &blocking_notif_high_prio);
    atomic_notifier_chain_unregister(&atomic_chain_list, &atomic_notif);
    atomic_notifier_chain_unregister(&atomic_chain_list, &atomic_notif_high_prio);
    srcu_notifier_chain_unregister(&srcu_chain_list, &srcu_notif);
    unregister_die_notifier(&die_notif);
    unregister_reboot_notifier(&reboot_notif);

    synchronize_srcu(&srcu_chain_list.srcu);
}

module_init(notifier_chains_init);
module_exit(notifier_chains_exit);

MODULE_AUTHOR("CKH");
MODULE_LICENSE("GPL");
