#!/bin/bash
#1
MOD="ctest_cgroup_misc.ko"
CPath=/sys/fs/cgroup
insmod $MOD
lsmod
rmmod $MOD

#2
cd $CPath
mkdir Example
echo "+misc" >> $CPath/cgroup.subtree_control

echo "$CPath/misc.capacity:"
cat  $CPath/misc.capacity
echo "$CPath/Example/misc.current:"
cat $CPath/Example/misc.current
echo "$CPath/Example/misc.events:"
cat $CPath/Example/misc.events
echo "$CPath/Example/misc.max:"
cat $CPath/Example/misc.max

#3设置misc.max(目前无法设置, misc没有资源可用, cat /sys/fs/cgroup/misc.capacity)
echo sev 100 > $CPath/Example/misc.max

#4
rmdir $CPath/Example