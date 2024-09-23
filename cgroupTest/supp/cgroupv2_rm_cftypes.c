#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/blk-cgroup.h>
#include <linux/seq_file.h>
#include </home/kanghong.chen/workspace/linux-rt-6.1.96/block/blk-cgroup.h>

// 定义一个简单的cftype结构体
static u64 example_read_u64(struct cgroup_subsys_state *css, struct cftype *cft)
{
    // 返回一个简单的常数，实际可以返回cgroup相关的统计信息
    return 42;
}

static struct cftype legacy_cftypes[] = {
    {
        .name = "example_file",  // 控制文件名
        .flags = CFTYPE_ONLY_ON_ROOT,  // 文件仅存在于cgroup根目录中
        .read_u64 = example_read_u64,  // 读取回调函数
    },
    { }  // 以空的cftype结束
};

// 定义一个全局的 blkcg_policy 结构体
static struct blkcg_policy test_blkcg_policy = {
    .plid = 1,  // 假设使用策略 ID 1，这通常在真实场景中需要确保唯一性
    .dfl_cftypes = NULL,  // 默认没有控制文件类型
    .legacy_cftypes = legacy_cftypes,  // 默认没有遗留控制文件类型
    .cpd_free_fn = NULL,  // 没有特殊的释放函数
};

// 初始化模块
static int __init test_module_init(void)
{
    int ret;

    printk(KERN_INFO "Loading test module...\n");

    ret = blkcg_policy_register(&test_blkcg_policy);
    if (ret) {
        pr_err("Failed to register blkcg policy, error: %d\n", ret);
        return ret;
    }
    pr_info("blkcg policy registered successfully\n");

    blkcg_policy_unregister(&test_blkcg_policy);

    pr_info("unregistered successfully\n");

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
