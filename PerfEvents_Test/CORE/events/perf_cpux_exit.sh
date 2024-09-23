#!/bin/bash

# 函数：禁用指定的 CPU
disable_cpu() {
    local cpu=$1
    echo "Disabling CPU${cpu}..."
    echo 0 > /sys/devices/system/cpu/cpu${cpu}/online
}

# 函数：重新启用指定的 CPU
enable_cpu() {
    local cpu=$1
    echo "Enabling CPU${cpu}..."
    echo 1 > /sys/devices/system/cpu/cpu${cpu}/online
}

# 函数：检查性能事件
check_perf_event() {
    echo "Checking performance events..."
    perf stat -e cpu-clock sleep 1
}

# 主程序
CPU_TO_DISABLE=1  # 要禁用的 CPU 编号
CPU_TO_ENABLE=1   # 要重新启用的 CPU 编号

# 禁用指定的 CPU
disable_cpu $CPU_TO_DISABLE

# 检查性能事件
check_perf_event

# 重新启用指定的 CPU
enable_cpu $CPU_TO_ENABLE

# 再次检查性能事件
check_perf_event

echo "Done!"

