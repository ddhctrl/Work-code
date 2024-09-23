#!/bin/bash



# 设置更高的采样率
MAX_SAMPLE_RATE=100000
echo $MAX_SAMPLE_RATE > /proc/sys/kernel/perf_event_max_sample_rate

perf record -e syscalls:sys_enter_clone --namespaces -a -m 64  -- bash -c "for i in {1..100}; do sleep 0.1 & done; sleep 5"

perf report

