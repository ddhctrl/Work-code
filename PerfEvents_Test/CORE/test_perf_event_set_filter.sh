#!/bin/bash

# 设置运行时间为10秒
timeout 10 perf record -e syscalls:sys_enter_openat --filter 'common_pid == 1234'

# 检查命令是否正常退出
if [ $? -eq 124 ]; then
    echo "perf record stopped after 10 seconds."
else
    echo "perf record finished before timeout."
fi
