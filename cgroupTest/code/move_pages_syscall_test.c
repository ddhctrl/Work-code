#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/mempolicy.h>
#include <numaif.h>

// 通过 syscall 调用 move_pages
long move_pages_wrapper(pid_t pid, unsigned long nr_pages,
                        void **pages, const int *nodes,
                        int *status, int flags) {
    return syscall(__NR_move_pages, pid, nr_pages, pages, nodes, status, flags);
}

int main() {
    pid_t pid = getpid(); // 当前进程的 PID
    unsigned long nr_pages = 2; // 要移动的页面数量
    void *pages[2]; // 页面地址数组
    int nodes[2]; // 目标节点数组
    int status[2]; // 存储页面移动状态的数组
    int flags = 0; // 标志位

    // 分配并初始化页面
    pages[0] = malloc(4096); // 分配 4KB 页面
    pages[1] = malloc(4096);
    if (!pages[0] || !pages[1]) {
        perror("malloc");
        return 1;
    }

    // 假设目标节点为 0 和 1
    nodes[0] = 0;
    nodes[1] = 1;

    // 调用 move_pages
    long result = move_pages_wrapper(pid, nr_pages, pages, nodes, status, flags);
    if (result == 0) {
        printf("Pages moved successfully\n");
    } else {
        perror("move_pages");
        printf("move_pages failed with error code %ld\n", result);
    }

    // 打印页面移动状态
    for (unsigned long i = 0; i < nr_pages; i++) {
        if (status[i] == 0) {
            printf("Page %lu moved successfully to node %d\n", i, nodes[i]);
        } else {
            printf("Page %lu move failed with error %d\n", i, status[i]);
        }
    }

    // 释放分配的页面
    free(pages[0]);
    free(pages[1]);

    return 0;
}
