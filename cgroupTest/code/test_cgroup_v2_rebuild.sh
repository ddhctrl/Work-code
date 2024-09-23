#!/bin/bash
CPath=/sys/fs/cgroup
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)

cd $CPath
if [ ! -d "Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir Example
else
    echo "$CPath/Example already exits."
fi
echo "+cpuset" >> $CPath/cgroup.subtree_control
echo "+cpuset" >> $CPath/Example/cgroup.subtree_control

#2 分配,释放cpu掩码
echo "cpuset.cpus: "
cat Example/cpuset.cpus
echo "1" > $CPath/Example/cpuset.cpus
echo "alloc cpu to Example, cpuset.cpus: "
cat Example/cpuset.cpus

echo "Example cpuset.cpus.partition: "
cat $CPath/Example/cpuset.cpus.partition
echo "root" > $CPath/Example/cpuset.cpus.partition
echo "write root to Example cpuset.cpus.partition: "
cat $CPath/Example/cpuset.cpus.partition

echo $CPID > $CPath/Example/cgroup.procs
echo "$CPath/Example/cgroup.procs: "
cat $CPath/Example/cgroup.procs

echo "/proc/$CPID/cpuset: "
cat /proc/$CPID/cpuset

echo "查看进程<$CPID>绑定cpu: "
taskset -cp $CPID

#4 热插拔CPU
echo 0 > /sys/devices/system/cpu/cpu1/online
lscpu | grep CPU
echo "########CPU Hotplug########"
echo 1 > /sys/devices/system/cpu/cpu1/online
lscpu | grep CPU

kill -9 $CPID
rmdir $CPath/Example