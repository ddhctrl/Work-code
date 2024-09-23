#!/bin/bash
#1
work_path=$(pwd)
unshare -C -m bash -c "ls -l /proc/self/ns/cgroup;exit"

#2
echo "/**************old*************/"
source ./test_cgroupns_fork_process.sh

#3
echo "/**************new***********$PID**/"
unshare -C -m bash -c "./test_cgroupns_fork_process.sh; nsenter --target $PID --all bash -c 'echo \"Final cgroup ns : \"; ls -l /proc/self/ns/cgroup'"
echo "last, kill..."
ps -ef | grep './cgroup_fork_process' | grep -v grep | awk '{print $2}' | xargs kill -9
#nsenter --target $PID --all
# echo "Final cgroup ns : "
# ls -l /proc/self/ns/cgroup
# cd $work_path