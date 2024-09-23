#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cgroup-defs.h>
#include <linux/misc_cgroup.h>

MODULE_LICENSE("GPL");

static int __init test_cgroup_misc_init(void)
{
    unsigned long usage_res = 0;
    int set_ret, charge_ret;
    struct misc_cg cg;

    usage_res = misc_cg_res_total_usage(MISC_CG_RES_TYPES);
    set_ret = misc_cg_set_capacity(MISC_CG_RES_TYPES, 1);
    charge_ret = misc_cg_try_charge(MISC_CG_RES_TYPES, &cg, 0);
    misc_cg_uncharge(MISC_CG_RES_TYPES, &cg, 1);

    printk(KERN_EMERG"usage_res: %lu, set_ret: %d, charge_ret: %d", usage_res, set_ret, charge_ret);
    return 0;
}

static void __exit test_cgroup_misc_exit(void)
{
    printk(KERN_EMERG"EXIT! \n");
}

module_init(test_cgroup_misc_init);
module_exit(test_cgroup_misc_exit);
MODULE_AUTHOR("CKH");
MODULE_VERSION("1.1");
