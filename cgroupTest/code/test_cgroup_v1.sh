#!/bin/bash
CPath=/sys/fs/cgroup
print_message() {
    local message_type=$1
    shift
    local message="$@"
    case "$message_type" in
        "INFO")
            echo -e "\033[1;32m[INFO]\033[0m $message"
            ;;
        "WARNING")
            echo -e "\033[1;33m[WARNING]\033[0m $message"
            ;;
        "ERROR")
            echo -e "\033[1;31m[ERROR]\033[0m $message"
            ;;
        *)
            echo "[UNKNOWN] $message"
            ;;
    esac
}

#1 判断是否挂载cgroupv1
check_cgroup_v1() {
    local subsystems=("cpu" "memory" "pids")
    local cgroup_v1_used=false

    for subsystem in "${subsystems[@]}"; do
        if [ -d "$CPath/$subsystem" ]; then
            cgroup_v1_used=true
            break
        fi
    done
}
check_cgroup_v1
if $cgroup_v1_used; then
    print_message "INFO" "cgroupv1 is in use."
else
    print_message "ERROR" "cgroupv1 is not in use."
    exit 1
fi

#2 创建新的子cgroup
if [ ! -d "$CPath/cpu/Example1" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p $CPath/cpu/Example1
else
    print_message "INFO" "$CPath/cpu/Example1 already exits."
fi

if [ ! -d "$CPath/memory/Example2" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p $CPath/memory/Example2
else
    print_message "INFO" "$CPath/memory/Example2 already exits."
fi

#3 添加进程
./cgroup_fork_process &
PID1=$!
CPID1=$(pgrep -P $PID1)

if [ -z "$PID1" ]; then
     print_message "ERROR" "PPID1 get failed!"
     exit 1
fi

if [ -z "$CPID1" ]; then
     print_message "ERROR""CPID1 get failed!"
     exit 1
fi

echo $CPID1 >> $CPath/cpu/Example1/cgroup.procs

./cgroup_fork_process &
PID2=$!
CPID2=$(pgrep -P $PID2)

if [ -z "$PID2" ]; then
     print_message "ERROR" "PPID2 get failed!"
     exit 1
fi

if [ -z "$CPID2" ]; then
     print_message "ERROR""CPID2 get failed!"
     exit 1
fi

echo $CPID2 >> $CPath/memory/Example2/cgroup.procs

print_message "INFO" "$CPath/cpu/Example1/cgroup.procs: "
cat $CPath/cpu/Example1/cgroup.procs
print_message "INFO" "$CPath/memory/Example2/cgroup.procs: "
cat $CPath/memory/Example2/cgroup.procs

echo $CPID2 >> $CPath/cpu/Example1/cgroup.procs
print_message "INFO" "pid:$CPID2 add cpu"
print_message "INFO" "$CPath/cpu/Example1/cgroup.procs: "
cat $CPath/cpu/Example1/cgroup.procs
print_message "INFO" "$CPath/memory/Example2/cgroup.procs: "
cat $CPath/memory/Example2/cgroup.procs

kill -9 $CPID1 $CPID2
rmdir $CPath/cpu/Example1 $CPath/memory/Example2

#4 创建新的非root cgroup挂载v1
if [ ! -d "/tmp/cgroup" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p /tmp/cgroup
else
    print_message "INFO" "/tmp/cgroup already exits."
fi

mount -t cgroup -o none,name=cgroup cgroup /tmp/cgroup
if [ $? -eq 0 ]; then
    print_message "INFO" "/tmp/cgroup 挂载成功"
    mount | grep cgroup
else
    print_message "INFO" "/tmp/cgroup 挂载失败"
    exit 1
fi

#开启memory子系统
MEMPath=/tmp/cgroup/memory
if [ ! -d "$MEMPath" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p $MEMPath
else
    print_message "INFO" "$MEMPath already exits."
fi
mount -t cgroup -o memory cgroup $MEMPath
if [ $? -eq 0 ]; then
    echo "$MEMPath 成功挂载"
    print_message "INFO" "$MEMPath 挂载成功"
    mount | grep cgroup
else
    print_message "ERROR" "$MEMPath 挂载失败"
    exit 1
fi

#5 cgroup清理工作
#设置release agent
if [ -f /usr/local/bin/cgroup_release.sh ]; then
   rm -f /usr/local/bin/cgroup_release.sh   
fi
mkdir -p $MEMPath/Example

echo '#!/bin/bash' >> /usr/local/bin/cgroup_release.sh
echo 'echo "Cgroup released: $1" >> /var/log/cgroup_release.log' >> /usr/local/bin/cgroup_release.sh
echo "/usr/local/bin/cgroup_release.sh" >> $MEMPath/release_agent
chmod +x /usr/local/bin/cgroup_release.sh

print_message "INFO" "$MEMPath/release_agent: "
cat $MEMPath/release_agent

#cgroup中无进程则自动清理
echo 1 > $MEMPath/notify_on_release
echo 1 > $MEMPath/Example/notify_on_release
./cgroup_fork_process &
PID3=$!
CPID3=$(pgrep -P $PID3)

if [ -z "$PID3" ]; then
     print_message "ERROR" "PPID3 get failed!"
     exit 1
fi

if [ -z "$CPID3" ]; then
     print_message "ERROR""CPID3 get failed!"
     exit 1
fi

echo $CPID3 >> $MEMPath/Example/cgroup.procs
print_message "INFO" "$MEMPath/Example/cgroup.procs: "
cat $MEMPath/Example/cgroup.procs

echo $CPID3 >> $MEMPath/cgroup.procs
print_message "INFO" "after move, $MEMPath/Example/cgroup.procs: "
cat $MEMPath/Example/cgroup.procs

#6 读写信息
print_message "INFO" "$MEMPath/cgroup.clone_children: "
cat $MEMPath/cgroup.clone_children
echo 1 > $MEMPath/cgroup.clone_children
print_message "INFO" "echo 1 to $MEMPath/cgroup.clone_children: "
cat $MEMPath/cgroup.clone_children
print_message "INFO" "$MEMPath/cgroup.sane_behavior: "
cat $MEMPath/cgroup.sane_behavior
print_message "INFO" "$MEMPath/notify_on_release: "
cat $MEMPath/notify_on_release
print_message "INFO" "$MEMPath/release_agent: "
cat $MEMPath/release_agent
print_message "INFO" "//////////release log output/////////////"
cat /var/log/cgroup_release.log

# rename cgroup
mv $MEMPath/Example $MEMPath/Example2
print_message "INFO" "$MEMPath: "
ls -l $MEMPath | grep Example

kill -9 $CPID3
umount $MEMPath
rmdir $MEMPath

#重新挂载
mount -o remount,nosuid,nodev,noexec,none,name=cgroup cgroup /tmp/cgroup
print_message "INFO" "remount: "
mount | grep cgroup
umount /tmp/cgroup
rmdir /tmp/cgroup