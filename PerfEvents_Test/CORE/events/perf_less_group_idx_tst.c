#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/perf_event.h>

static bool (*test_func)(const void *, const void *);

static int __init test_init(void)
{
    struct perf_event *event1, *event2;
    struct perf_event **events;
    
    event1 = kzalloc(sizeof(*event1), GFP_KERNEL);
    event2 = kzalloc(sizeof(*event2), GFP_KERNEL);
    
    if (!event1 || !event2) {
        pr_err("Memory allocation failed\n");
        kfree(event1);
        kfree(event2);
        return -ENOMEM;
    }

    // Initialize group_index
    event1->group_index = 1;
    event2->group_index = 2;

    // Assign the function pointer
    test_func = 0xffff80000823e0c0;

    // Create an array of pointers to perf_event
    events = kmalloc(2 * sizeof(*events), GFP_KERNEL);
    if (!events) {
        pr_err("Memory allocation for events array failed\n");
        kfree(event1);
        kfree(event2);
        return -ENOMEM;
    }

    events[0] = event1;
    events[1] = event2;

    // Test the function
    if (test_func(&events[0], &events[1])) {
        pr_info("event1 has a smaller group_index than event2\n");
    } else {
        pr_info("event1 does not have a smaller group_index than event2\n");
    }

    kfree(events);
    kfree(event1);
    kfree(event2);

    return 0;
}

static void __exit test_exit(void)
{
    pr_info("Test module exit\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
