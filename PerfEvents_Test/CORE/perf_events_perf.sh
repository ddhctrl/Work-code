#!/bin/bash

# 检查是否传入参数
if [ $# -eq 0 ]; then
    echo "Usage: $0 <test_case_number>"
    exit 1
fi

# 获取测试用例编号
test_case_number=$1

# 执行测试用例
echo "Running test case $test_case_number"
perf test --verbose $test_case_number
