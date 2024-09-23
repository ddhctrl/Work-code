#!/bin/bash
#1 创建进程
CPath=/sys/fs/cgroup
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

echo "CPID:$CPID"
ps -p $CPID -o %cpu

#2 cgroup添加进程
cd $CPath
if [ ! -d "Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir Example
else
    echo "$CPath/Example already exits."
fi
echo $CPID > $CPath/Example/cgroup.procs

echo "Example cgroup.procs: "
cat $CPath/Example/cgroup.procs

#3 冻结进程
echo 1 > $CPath/Example/cgroup.freeze
echo "freeze CPID:$CPID"

# awk 提取实时CPU占用率（假设它是第二列，可能需要调整）  
CPU_USAGE=$(top -b -n 1 | grep " $CPID " | awk '{print $9 + $10}')  
echo "CPU Usage for PID $CPID: $CPU_USAGE%"

#4 解冻进程
echo 0 > $CPath/Example/cgroup.freeze
echo "unfreeze CPID:$CPID"
ps -p $CPID -o %cpu

kill -9 $CPID
rmdir $CPath/Example