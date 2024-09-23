#!/bin/bash
mount -t tmpfs cgroup_root /sys/fs/cgroup
mkdir /sys/fs/cgroup/cpuset
mount -t cgroup cpuset -o cpuset /sys/fs/cgroup/cpuset
cd /sys/fs/cgroup/cpuset
mkdir CKH
cd CKH
echo 2-3 > cpuset.cpus
echo 1 > cpuset.mems
echo $$ > tasks
sh
cat /proc/self/cgroup