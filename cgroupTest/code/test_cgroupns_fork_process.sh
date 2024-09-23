 #!/bin/bash
 ./cgroup_fork_process &
PID=$!

if [ -z "$PID" ]; then
     echo "PID get failed!"
     exit 1
fi

ls -l /proc/$PID/ns/cgroup

