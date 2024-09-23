#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>

void setup_core_dump() {
    struct rlimit core_limit;
    core_limit.rlim_cur = core_limit.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_CORE, &core_limit) == -1) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // 设置允许生成 core dump 文件
    setup_core_dump();

    printf("PID: %d\n", getpid());
    printf("This process will now generate a segmentation fault and a core dump...\n");

    // 强制触发段错误（致命信号）
    int *p = NULL;
    *p = 42; // 这里会触发 SIGSEGV

    // 如果没有触发段错误，这行代码不会被执行
    printf("This line will not be printed.\n");

    return 0;
}
