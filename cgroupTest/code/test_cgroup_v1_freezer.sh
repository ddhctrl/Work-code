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

if [ ! -d $CPath/freezer ]; then
    print_message "ERROR" "$CPath/freezer does not exits!"
    exit 1
fi

if [ ! -d "$CPath/freezer/Example" ]; then
    # 如果不存在，则创建该文件夹
    mkdir -p $CPath/freezer/Example
else
    print_message "INFO" "$CPath/freezer/Example already exits."
fi
#冻结进程
./cgroup_fork_process &
PID=$!
CPID=$(pgrep -P $PID)

#统计5s内的cpu占用
get_cpu(){
    COUNTER=0
    while [ $COUNTER -lt 5 ]; do
    CPU_USAGE=$(top -b -n 1 | grep " $CPID " | awk '{print $9}')  
    echo "CPU at second $COUNTER: $CPU_USAGE"
    COUNTER=$((COUNTER + 1))
    sleep 1
done    
}

print_message "INFO" "//////////////////CPU Usage for PID $CPID///////////////////"
get_cpu

echo $CPID > $CPath/freezer/Example/cgroup.procs
echo "FROZEN" > $CPath/freezer/Example/freezer.state
print_message "INFO" "$CPath/freezer/Example/freezer.state: "
cat $CPath/freezer/Example/freezer.state
echo "////////////////freeze PID:$CPID////////////////////"

print_message "INFO" "//////////////////CPU Usage for PID $CPID///////////////////"
get_cpu

#解冻进程
echo "THAWED" > $CPath/freezer/Example/freezer.state
print_message "INFO" "$CPath/freezer/Example/freezer.state: "
cat $CPath/freezer/Example/freezer.state
sleep 3
echo "////////////////unfreeze PID:$CPID////////////////////"
get_cpu

kill -9 $CPID
rmdir $CPath/freezer/Example