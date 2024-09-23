#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/pid.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CKH");
MODULE_VERSION("1.0");

static int pid = -1;
module_param(pid, int, 0444);
MODULE_PARM_DESC(pid, "PID of the process to send signal");

static unsigned long addr = 0;
module_param(addr, ulong, 0444);
MODULE_PARM_DESC(addr, "Address of the signal_setup_done function");

typedef void (*signal_setup_done_t)(int failed, struct ksignal *ksig, int stepping);
static signal_setup_done_t setup_done_callback = NULL;

static int __init test_mod_init(void)
{
    struct task_struct *task;
    struct ksignal ksig;
    sigset_t oldset;

    if (pid == -1) {
        pr_err("Please provide a valid PID using pid parameter\n");
        return -EINVAL;
    }

    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        pr_err("Invalid PID: %d\n", pid);
        return -ESRCH;
    }

    memset(&ksig, 0, sizeof(ksig));
    ksig.sig = SIGUSR1; // Example signal
    ksig.ka.sa.sa_flags = 0;

    // Save the current signal mask
    sigprocmask(SIG_BLOCK, NULL, &oldset);

    // Call the signal_delivered function
    setup_done_callback = (signal_setup_done_t)addr;
    setup_done_callback(0, &ksig, 1);

    // Restore the original signal mask
    sigprocmask(SIG_SETMASK, &oldset, NULL);

    put_task_struct(task);

    pr_info("Signal delivered to process %d\n", pid);
    return 0;
}

static void __exit test_mod_exit(void)
{
    pr_info("Module unloaded\n");
}

module_init(test_mod_init);
module_exit(test_mod_exit);
