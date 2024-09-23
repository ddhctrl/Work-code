#!/bin/bash

insmod perf_event_sysfs_show_tst.ko

lsmod | grep perf_event_sysfs_show_tst.ko

# 读取设备属性以触发 perf_event_sysfs_show
cat /sys/class/test_class/test_device/test

dmesg | grep "perf_event_sysfs_show"

rmmod perf_event_sysfs_show_tst.ko
