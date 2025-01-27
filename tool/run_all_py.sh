#!/bin/bash

# 获取当前脚本所在的目录
SCRIPT_DIR=$(dirname "$0")

# 在当前目录搜索带 .env 后缀的文件，获取第一个文件名
ENV_FILE=$(find . -type f -name "*.env" | head -n 1)

# 如果找到 .env 文件
if [ -n "$ENV_FILE" ]; then
    # 从文件名中提取环境名（去掉 .env 后缀）
    ENV_NAME="${ENV_FILE%.env}"

    # 根据环境名构建参数路径
    PARAM="${ENV_NAME}/${ENV_NAME}_linker_output.txt"

	# step1
    echo -e "\033[33m 执行命令: python3 ./extract_undef.py $PARAM \033[0m"
    python3 "$SCRIPT_DIR/extract_undef.py" "$PARAM"
	if [ $? -ne 0 ]; then
        exit 1
    fi

	# step2
	echo -e "\033[33m 执行命令: python3 Insert_stub.py \033[0m"
    python3 "$SCRIPT_DIR/Insert_stub.py"
	if [ $? -ne 0 ]; then
        exit 1
    fi

else
    # 如果没有找到 .env 文件，输出错误信息
    echo "环境错误, 未找到.env, 请检查"
    exit 1
fi

