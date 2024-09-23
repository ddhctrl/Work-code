#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/perf_event.h>



static struct device *test_device;
static struct class *test_class;

static ssize_t test_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t ret;
    ret = perf_event_sysfs_show(dev, attr, buf);
    pr_info("perf_event_sysfs_show returned: %zd\n", ret);
    return ret;
}


static struct perf_pmu_events_attr test_attr = {
    .attr = __ATTR(test, 0444, test_show, NULL),
    .event_str = "test_event_string",
};

static int __init test_module_init(void)
{
    int ret;
    
    // Create a class
    test_class = class_create(THIS_MODULE, "test_class");
    if (IS_ERR(test_class)) {
        pr_err("Failed to create class\n");
        return PTR_ERR(test_class);
    }

    // Create a device
    test_device = device_create(test_class, NULL, MKDEV(0, 0), NULL, "test_device");
    if (IS_ERR(test_device)) {
        pr_err("Failed to create device\n");
        class_destroy(test_class);
        return PTR_ERR(test_device);
    }
    // Create the device attribute
    ret = device_create_file(test_device, &test_attr.attr);
    if (ret) {
        pr_err("Failed to create device attribute\n");
        device_destroy(test_class, MKDEV(0, 0));
        class_destroy(test_class);
        return ret;
    }

    pr_info("Test module initialized\n");
    return 0;
}

static void __exit test_module_exit(void)
{
    device_remove_file(test_device, &test_attr.attr);
    device_destroy(test_class, MKDEV(0, 0));
    class_destroy(test_class);
    pr_info("Test module exited\n");
}

module_init(test_module_init);
module_exit(test_module_exit);
MODULE_LICENSE("GPL");