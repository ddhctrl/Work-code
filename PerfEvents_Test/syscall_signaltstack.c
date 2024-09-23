#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <errno.h>

int main() {
    stack_t ss, old_ss;
    int ret;

    // 为替代信号堆栈分配内存
    ss.ss_sp = malloc(SIGSTKSZ);
    if (ss.ss_sp == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;

    // 设置替代信号堆栈
    ret = syscall(__NR_sigaltstack, &ss, &old_ss);
    if (ret == -1) {
        perror("sigaltstack");
        free(ss.ss_sp);
        return EXIT_FAILURE;
    }

    printf("Alternative signal stack is set.\n");
    printf("Old stack: ss_sp=%p, ss_size=%zu, ss_flags=%d\n",
           old_ss.ss_sp, old_ss.ss_size, old_ss.ss_flags);

    // 恢复原来的信号堆栈
    ret = syscall(__NR_sigaltstack, NULL, &old_ss);
    if (ret == -1) {
        perror("sigaltstack");
        free(ss.ss_sp);
        return EXIT_FAILURE;
    }

    printf("Restored old signal stack: ss_sp=%p, ss_size=%zu, ss_flags=%d\n",
           old_ss.ss_sp, old_ss.ss_size, old_ss.ss_flags);

    // 清理分配的内存
    free(ss.ss_sp);

    return EXIT_SUCCESS;
}
