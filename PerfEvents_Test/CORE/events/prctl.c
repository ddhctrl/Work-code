#include <sys/prctl.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // 使用 prctl 启用 PR_TASK_PERF_EVENTS_ENABLE
    if (prctl(PR_TASK_PERF_EVENTS_ENABLE) == -1) {
        perror("prctl enable");
        exit(EXIT_FAILURE);
    }

    printf("PR_TASK_PERF_EVENTS_ENABLE set successfully\n");

    // 在启用性能事件时执行一些操作
    // 模拟一些工作负载
    for (volatile int i = 0; i < 1000000; i++);

    // 使用 prctl 禁用 PR_TASK_PERF_EVENTS_DISABLE
    if (prctl(PR_TASK_PERF_EVENTS_DISABLE) == -1) {
        perror("prctl disable");
        exit(EXIT_FAILURE);
    }

    printf("PR_TASK_PERF_EVENTS_DISABLE set successfully\n");

    return 0;
}

