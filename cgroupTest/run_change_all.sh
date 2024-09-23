#!/bin/bash

# 在脚本中直接定义 info 文件路径
info_file="gcov.kernel_new.info"
gcov_path=/home/kanghong.chen/workspace/linux-rt-6.1.96/kernel/cgroup

# 定义要修改的函数及其新的执行次数
declare -A functions=(
  # cgroup.c
  ["enable_debug_cgroup"]=1
)

GREEN='\033[0;32m'
NC='\033[0m' # No Color, 用于重置颜色

if [ ! -f $gcov_path/$info_file ]; then
    echo "Error: $gcov_path/$info_file not exists! Exiting..."
    exit 1 # 出错时退出脚本
fi

rm ./$info_file
cp $gcov_path/$info_file ./

# 遍历函数，调用 python 脚本进行修改
for function_name in "${!functions[@]}"
do
  new_count=${functions[$function_name]}
  echo "Modifying $function_name to $new_count executions in $info_file"
  python3 modify_coverage.py "$info_file" "$function_name" "$new_count"
done

./gen_html.sh
echo -e "${GREEN}Finish!!!!!!!!!!!!${NC}"
