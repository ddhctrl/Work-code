#!/bin/bash

MAX_SAMPLE_RATE=100000
echo $MAX_SAMPLE_RATE > /proc/sys/kernel/perf_event_max_sample_rate
echo 262144 > /proc/sys/vm/max_map_count

# 打印开始信息
echo "开始记录 cgroup:cgroup_mkdir 事件..."

# 使用 perf 记录事件，并创建 cgroup
perf record  -e cgroup:cgroup_mkdir --all-cgroups -m 16 -- bash -c '
sleep 2
for i in {1..10}; do
  CGROUP_PATH="/sys/fs/cgroup/perf_event/perf_test_$i"
  mkdir -p $CGROUP_PATH
done
sleep 5
'

# 检查 perf.data 文件是否成功生成
if [ -f "perf.data" ]; then
  echo "记录完成，数据保存在 perf.data 中。"
else
  echo "记录失败，没有生成 perf.data 文件。"
  exit 1
fi

# 使用 perf report 查看数据
perf report 

