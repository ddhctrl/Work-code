#!/bin/bash
CPath=/sys/fs/cgroup
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)

#1 开启cpuset子系统
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
echo "0" > $CPath/Example/cpuset.mems

#3 读写调度域
echo "Example cpuset.cpus.partition: "
cat Example/cpuset.cpus.partition
echo "root" > $CPath/Example/cpuset.cpus.partition
echo "1. Example write cpuset.cpus.partition: "
cat Example/cpuset.cpus.partition

echo "isolated" > $CPath/Example/cpuset.cpus.partition
echo "2. Example write cpuset.cpus.partition: "
cat Example/cpuset.cpus.partition

echo "member" > $CPath/Example/cpuset.cpus.partition
echo "3. Example write cpuset.cpus.partition: "
cat Example/cpuset.cpus.partition

echo $CPID > $CPath/Example/cgroup.procs
echo "Example cgroup.procs: "
cat $CPath/Example/cgroup.procs

echo "/proc/$CPID/cpuset: "
cat /proc/$CPID/cpuset

echo "查看进程<$CPID>绑定cpu: "
taskset -cp $CPID

echo "alloc cpumask, cpuset.cpus: "
cat Example/cpuset.cpus
echo "alloc cpumask, cpuset.mems: "
cat Example/cpuset.mems
echo "alloc cpumask, cpuset.cpus.effective: "
cat Example/cpuset.cpus.effective
echo "alloc cpumask, cpuset.mems.effective: "
cat Example/cpuset.mems.effective

#4 热插拔CPU
echo 0 > /sys/devices/system/cpu/cpu1/online
lscpu | grep CPU
echo "########CPU Hotplug########"
echo 1 > /sys/devices/system/cpu/cpu1/online
lscpu | grep CPU

kill -9 $CPID
echo "" > $CPath/Example/cpuset.cpus

#5 读写cpus,mems掩码
echo "free cpumask, cpuset.cpus: "
cat Example/cpuset.cpus
echo "free cpumask, cpuset.mems: "
cat Example/cpuset.mems

if [ ! -d "$CPath/Example/g1" ]; then
    # 如果不存在，则创建该文件夹
    mkdir $CPath/Example/g1
else
    echo "$CPath/Example/g1 already exits."
fi
echo "1" > $CPath/Example/g1/cpuset.cpus
echo "0" > $CPath/Example/g1/cpuset.mems
echo "g1 cpuset.cpus: "
cat Example/g1/cpuset.cpus

echo "g1 cpuset.cpus.effective: "
cat Example/g1/cpuset.cpus.effective
echo "g1 cpuset.mems.effective: "
cat Example/g1/cpuset.mems.effective

rmdir $CPath/Example/g1
rmdir $CPath/Example