#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mempolicy.h>
#include <linux/sched.h>
#include <linux/numa.h>
#include <linux/mm_types.h>
#include <linux/gfp.h>
#include <linux/mmu_context.h>
#include <linux/init.h>
#include <linux/slab.h>

static int pid = -1;
module_param(pid, int, 0444);
MODULE_PARM_DESC(pid, "PID of the task");

static struct mempolicy *create_preferred_policy(void)
{
    struct mempolicy *pol;
    int preferred_node = numa_node_id();

    pol = kzalloc(sizeof(struct mempolicy), GFP_KERNEL);
    if (!pol)
        return NULL;

    pol->mode = MPOL_BIND;
    pol->flags = 0;
    pol->nodes = NODE_MASK_NONE;
    node_set(preferred_node, pol->nodes);

    return pol;
}

static void free_mempolicy(struct mempolicy *pol)
{
    kfree(pol);
}

static int __init vma_alloc_folio_init(void)
{
    struct task_struct *task;
    struct folio *folio;
    struct vm_area_struct *vma;
    struct mempolicy *pol;
    unsigned long addr;
    int order = 0;  // Order of the folio (0 means 1 page)
    gfp_t gfp = GFP_KERNEL;
    bool hugepage = true;
    struct mempolicy *orig_policy;

    if (pid < 0) {
        printk(KERN_ERR "Invalid PID\n");
        return -EINVAL;
    }

    // 获取进程的task_struct
    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    rcu_read_unlock();

    if (!task) {
        printk(KERN_ERR "task with PID %d not found\n", pid);
        return -ESRCH;
    }

    // 获取当前进程的一个 VMA 和地址
    down_read(&task->mm->mmap_lock);
    vma = find_vma(task->mm, task->mm->start_code);
    if (!vma) {
        up_read(&task->mm->mmap_lock);
        printk(KERN_ERR "Failed to find VMA\n");
        return -1;
    }

    addr = vma->vm_start;

    pol = create_preferred_policy();
    if (!pol) {
        up_read(&task->mm->mmap_lock);
        printk(KERN_ERR "Failed to create new memory policy\n");
        return -ENOMEM;
    }

    orig_policy = vma->vm_policy;
    vma->vm_policy = pol;

    // 调用 vma_alloc_folio 函数
    folio = vma_alloc_folio(gfp, order, vma, addr, hugepage);
    if (!folio) {
        up_read(&task->mm->mmap_lock);
        printk(KERN_ERR "Failed to allocate folio\n");
        vma->vm_policy = orig_policy;
        free_mempolicy(pol);
        return -ENOMEM;
    }

    printk(KERN_INFO "Successfully allocated folio at address: %p\n", folio);

    // 释放分配的页面
    __free_pages(&folio->page, order);
    up_read(&task->mm->mmap_lock);
    vma->vm_policy = orig_policy;
    free_mempolicy(pol);

    return 0;
}

static void __exit vma_alloc_folio_exit(void)
{
    printk(KERN_INFO "vma_alloc_folio module exited\n");
}

module_init(vma_alloc_folio_init);
module_exit(vma_alloc_folio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_DESCRIPTION("test vma_alloc_folio");
