#!/bin/bash

#当前脚本名字run_all.sh
current_script="$(basename "$0")"
#不想执行的脚本
nowant_script=()
#特殊处理的脚本
special_sh=()
#不想执行的可执行文件
nowant_exe=("bpf_ioctl" "bpf_fd" "bpf_link" "perf_fasync_tst")

# 要加载的内核模块列表
modules_to_load=("perf_less_group_idx_tst.ko" "perf_event_read_value_tst.ko"
    "perf_register_guest_info_callbacks_tst.ko" "perf_sigtrap.ko"
    "perf_pmu_unregister_tst.ko" "perf_event_create_kernel_counter_tst.ko") # 替换为你需要的内核模块名称

# 蓝色文本的 ANSI 转义序列
BLUE='\033[0;34m'
NC='\033[0m' # No Color, 用于重置颜色

# 先手动执行下面的测试用例,放在后面会出问题
echo -e "${BLUE}Executing bpf_ioctl...${NC}"
./bpf_ioctl
echo -e "${BLUE}Executing bpf_fd...${NC}"
./bpf_fd
echo -e "${BLUE}Executing bpf_link...${NC}"
./bpf_link
echo -e "${BLUE}Loading module perf_mmap_open_tst.ko...${NC}"
insmod perf_mmap_open_tst.ko
echo -e "${BLUE}Unloading module perf_mmap_open_tst.ko...${NC}"
rmmod perf_mmap_open_tst.ko

# 执行当前目录下所有的.sh脚本
for script in *.sh; do
    if [ -f "$script" ] && [ "$script" != "$current_script" ] && [[ ! " ${nowant_script[@]} " =~ " ${script} " ]]; then
        echo -e "${BLUE}Executing $script...${NC}" # 输出蓝色文本
        chmod +x "$script"                         # 确保脚本是可执行的
        ./"$script"
        wait # 等待脚本执行完成
    fi
done

# 执行当前目录下所有的可执行文件
for file in *; do
    if [ -f "$file" ] && [ -x "$file" ] && [ "${file##*.}" != "sh" ] && [ "${file##*.}" != "ko" ] && [[ ! " ${nowant_exe[@]} " =~ " ${file} " ]]; then
        echo -e "${BLUE}Executing $file...${NC}"
        ./"$file"
        wait # 等待文件执行完成
    fi
done

# 加载并卸载指定的 .ko 内核模块
for module in "${modules_to_load[@]}"; do
    if [ -f "$module" ]; then
        echo -e "${BLUE}Loading kernel module $module...${NC}"
        sudo insmod "$module" # 加载内核模块
        wait                  # 等待加载完成

        # 在此处执行其他需要在模块加载后进行的操作

        echo -e "${BLUE}Unloading kernel module $module...${NC}"
        if ! sudo rmmod "$module"; then # 卸载内核模块并检测是否出错
            echo "Error: Failed to unload $module. Exiting..."
            exit 1 # 出错时退出脚本
        fi
        wait # 等待卸载完成
    else
        echo "$module not found in the current directory."
    fi
done

# 特殊的测试用例.放在中间执行会出问题, 故放在最后单独处理(问题原因暂不清楚)
for script in "${special_sh[@]}"; do
    if [ -f "$script" ]; then
        echo -e "${BLUE}Executing delayed $script...${NC}"
        chmod +x "$script"
        ./"$script"
    fi
done
