#!/bin/bash
path=$(pwd)
work_path=/home/kanghong.chen/workspace
srcode_path=$work_path/linux-rt-6.1.96
kernel_path=$work_path/linux-rt-6.1.96/kernel

genhtml -o /home/kanghong.chen/gcov/test_coverage_cgroup -t "test coverage" --num-spaces 4 gcov.kernel_new.info
echo "genhtml success"
