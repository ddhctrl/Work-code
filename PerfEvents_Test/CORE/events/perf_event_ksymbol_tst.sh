#!/bin/bash



# 设置更高的采样率
MAX_SAMPLE_RATE=100000
echo $MAX_SAMPLE_RATE > /proc/sys/kernel/perf_event_max_sample_rate
perf record -e syscalls:sys_enter_* -e syscalls:sys_exit_* -a -- ls

perf report

