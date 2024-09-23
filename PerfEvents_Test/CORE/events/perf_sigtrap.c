#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <linux/ptrace.h>
#include <linux/irq_work.h>
#include <linux/sched.h>

// 函数指针声明
typedef int (*perf_event_overflow_func)(struct perf_event *, int, struct perf_sample_data *, struct pt_regs *);

// 声明函数指针
static perf_event_overflow_func overflow_func;

static struct perf_event *test_event;
static struct pt_regs test_regs;
static struct perf_sample_data test_data;
static struct irq_work test_irq_work;

// 自定义 irq_work 回调函数
static void test_irq_work_func(struct irq_work *work)
{
    printk(KERN_INFO "test_irq_work_func called\n");
}

// 自定义溢出处理函数
static void test_overflow_handler(struct perf_event *event, struct perf_sample_data *data, struct pt_regs *regs)
{
    printk(KERN_INFO "test_overflow_handler called\n");
}

// 自定义 __perf_event_account_interrupt 函数，以确保返回 0
static inline int __perf_event_account_interrupt(struct perf_event *event, int throttle)
{
    return 0;
}

static int __init my_module_init(void)
{
    struct perf_event_attr attr;

    printk(KERN_INFO "Loading test module\n");

    // 初始化函数指针为 __perf_event_overflow 的内核地址
    overflow_func = (perf_event_overflow_func)0xffff80000824ef10;

    // 初始化 perf_event_attr 结构体
    memset(&attr, 0, sizeof(struct perf_event_attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.exclude_kernel = 1;
    attr.sigtrap = 1;
    attr.sample_period = 100;  // 确保 sample_period 不为 0

    // 分配和初始化 perf_event 结构体
    test_event = kzalloc(sizeof(struct perf_event), GFP_KERNEL);
    if (!test_event) {
        printk(KERN_ERR "Failed to allocate perf_event\n");
        return -ENOMEM;
    }
    test_event->attr = attr;
    atomic_set(&test_event->event_limit, 1);
    test_event->pending_sigtrap = 0;
    test_event->pending_kill = 0;
    test_event->pending_wakeup = 0;

    // 初始化 irq_work 结构体
    init_irq_work(&test_irq_work, test_irq_work_func);
    test_event->pending_irq = test_irq_work;

    // 初始化 pt_regs 结构体
    memset(&test_regs, 0, sizeof(struct pt_regs));
#if defined(CONFIG_ARM64)
    // 设置用户模式标志以确保 sample_is_allowed 返回 true
    test_regs.pstate = PSR_MODE_EL0t;  // ARM64 架构
#elif defined(CONFIG_X86)
    // x86 架构中 pt_regs 结构体包含 cs 成员
    test_regs.cs = USER_DS;  // x86 架构
#else
    #error "Unsupported architecture"
#endif

    // 初始化 perf_sample_data 结构体
    memset(&test_data, 0, sizeof(struct perf_sample_data));
    test_data.sample_flags = PERF_SAMPLE_ADDR;
    test_data.addr = 0x1234;

    // 设置事件上下文
    test_event->ctx = kzalloc(sizeof(struct perf_event_context), GFP_KERNEL);
    if (!test_event->ctx) {
        printk(KERN_ERR "Failed to allocate perf_event_context\n");
        kfree(test_event);
        return -ENOMEM;
    }
    local_set(&test_event->ctx->nr_pending, 0);

    // 设置溢出处理程序
    test_event->overflow_handler = test_overflow_handler;

    // 调用函数指针指向的 __perf_event_overflow 函数
    if (overflow_func) {
        int ret = overflow_func(test_event, 0, &test_data, &test_regs);
        printk(KERN_INFO "__perf_event_overflow called successfully, return value: %d\n", ret);
    } else {
        printk(KERN_ERR "Failed to set function pointer for __perf_event_overflow\n");
        kfree(test_event->ctx);
        kfree(test_event);
        return -EINVAL;
    }

    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Unloading test module\n");
    kfree(test_event->ctx);
    kfree(test_event);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
