#!/bin/bash

# 定义捕捉到信号时执行的命令
trap 'echo "Caught SIGINT signal (Ctrl+C). Exiting..."' SIGINT
count=1
while [ $count -le 5 ]
do
        echo "sleep $count"
        sleep $count
        count=$[ $count + 1 ]
done
echo "end"
