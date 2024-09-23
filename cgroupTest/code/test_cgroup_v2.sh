#!/bin/bash
#1 挂载cgroupv2文件系统
cd /tmp
if [ ! -d "Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir Example
else
    echo "/tmp/Example already exits."
fi

mount -t cgroup2 cgroup2 /tmp/Example
if [ $? -eq 0 ]; then
    echo "成功挂载"
    mount | grep cgroup
else
    echo "挂载失败"
    exit 1
fi

cat /tmp/Example/cgroup.subtree_control
echo "+cpuset" >> /tmp/Example/cgroup.subtree_control
echo "+a +b +c" >> /tmp/Example/cgroup.subtree_control
echo "添加子系统, result: "
cat /tmp/Example/cgroup.subtree_control
echo "-cpuset" >> /tmp/Example/cgroup.subtree_control
echo "删除子系统, result: "
cat /tmp/Example/cgroup.subtree_control

#2 卸载
umount /tmp/Example
if [ $? -eq 0 ]; then
    echo "成功卸载 /tmp/Example"
else
    echo "卸载 /tmp/Example 失败"
    exit 1
fi

rmdir /tmp/Example

#3 进程迁移
cd - 
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)

if [ -z "$PID" ]; then
     echo "PPID get failed!"
     exit 1
fi

if [ -z "$CPID" ]; then
     echo "CPID get failed!"
     exit 1
fi

if [ ! -d "/sys/fs/cgroup/Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir /sys/fs/cgroup/Example
else
    echo "/sys/fs/cgroup/Example already exits."
fi

echo "cgroup controllers: "
cat /sys/fs/cgroup/cgroup.controllers
echo "Example controllers: "
cat /sys/fs/cgroup/Example/cgroup.controllers

echo $CPID >> /sys/fs/cgroup/cgroup.procs
echo "源: "
cat /sys/fs/cgroup/cgroup.procs | tail -n 5
echo "目的地: "
cat /sys/fs/cgroup/Example/cgroup.procs

echo $CPID >> /sys/fs/cgroup/Example/cgroup.procs
echo "源: "
cat /sys/fs/cgroup/cgroup.procs | tail -n 5
echo "目的地: "
cat /sys/fs/cgroup/Example/cgroup.procs

kill -9 $CPID
rmdir /sys/fs/cgroup/Example

#4 查看可委派性和特性
CFILE=/sys/kernel/cgroup
if [ -f $CFILE/delegate ]; then
    echo "$CFILE/delegate: "
    cat $CFILE/delegate
else
    echo "$CFILE/delegate does not exist!"
fi

if [ -f $CFILE/features ]; then
    echo "$CFILE/features: "
    cat $CFILE/features
else
    echo "$CFILE/features does not exist!"
fi