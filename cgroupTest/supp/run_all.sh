#!/bin/bash

#当前脚本名字run_all.sh
current_script="$(basename "$0")"
#不想执行的脚本
nowant_script=()
#特殊处理的脚本
special_sh=()
#不想执行的可执行文件
nowant_exe=("cgroup_fork_process" "ctest_cgroupv2_clone_into")
# 需要带参数执行的脚本及其参数
declare -A script_args=(
  #["perf_events_perf.sh"]="2 3 4 8 16 23 25 26 28 35 50 70"
)

# 要加载的内核模块列表
modules_to_load=("cgroupv2_rm_cftypes.ko") # 替换为你需要的内核模块名称

# 蓝色文本的 ANSI 转义序列
BLUE='\033[0;34m'
NC='\033[0m' # No Color, 用于重置颜色

# 检查cgroup v1 or v2
is_cgroup_v2=false
if [ -f /sys/fs/cgroup/cgroup.controllers ]; then
    is_cgroup_v2=true
fi
# 根据 cgroup v2 检查结果，往数组中添加脚本
if [ "$is_cgroup_v2" = true ]; then
    # 排除cgroup v1的脚本
    nowant_script+=("test_cgroupv1_tasks_write.sh" "test_debug_inforead.sh"
                    "test_legacy_freezing_read.sh"
                    )
    nowant_exe+=("ctest_cgroupstat_build_netlink")
else
    # 排除cgroup v2的脚本
    nowant_script+=("test_cgroupv2_e_css.sh" "test_cgroupv2_reconfigure.sh" "test_cgroupv2_cpuset_initfs.sh"
                    "test_task_cgroup_path.sh" "test_cgroupv2_finalize_control.sh" "test_cgroupv2_irq_pressure_rw.sh"
                    "test_cgroupv2_kill.sh" "cgroupv2_rm_cftypes.ko" "test_cgroupv2_threads_rw.sh"
                    "test_cgroup_get_from_id.sh" "test_cgroup_get_from_file.sh" "test_ns_cgroupns_owner.sh"
                    "test_pids_cancel_attach.sh" "rstat_root_cputime.sh"
                    )
fi

# 执行当前目录下所有的.sh脚本
for script in *.sh; do
  if [ -f "$script" ] && [ "$script" != "$current_script" ] && [[ ! " ${nowant_script[@]} " =~ " ${script} " ]]; then
    if [ -n "${script_args[$script]}" ]; then  # 检查该脚本是否有参数列表
      for arg in ${script_args[$script]}; do
        echo -e "${BLUE}Executing $script with argument $arg...${NC}"  # 输出蓝色文本
        chmod +x "$script"  # 确保脚本是可执行的
        ./"$script" "$arg"  # 带参数执行脚本
        wait  # 等待脚本执行完成
      done
    else
      echo -e "${BLUE}Executing $script...${NC}"  # 输出蓝色文本
      chmod +x "$script"  # 确保脚本是可执行的
      ./"$script"
      wait  # 等待脚本执行完成
    fi
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
    sudo insmod "$module"                                  # 加载内核模块
    wait                                                   # 等待加载完成

    # 在此处执行其他需要在模块加载后进行的操作

    echo -e "${BLUE}Unloading kernel module $module...${NC}"
    if ! sudo rmmod "$module"; then                          # 卸载内核模块并检测是否出错
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
      echo -e "${BLUE}Please Executing manualy!!! $script...${NC}"
      # chmod +x "$script"
      # ./"$script"
  fi
done
