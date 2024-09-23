#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

typedef uint32_t compat_size_t;
typedef struct {
    uint32_t val[2];
} compat_sigset_t;

int main() {
    compat_sigset_t pending_set;
    compat_size_t sigsetsize = sizeof(compat_sigset_t);

    // 初始化信号集
    pending_set.val[0] = 0;
    pending_set.val[1] = 0;

    // 调用兼容模式的 rt_sigpending 系统调用
    int result = syscall(SYS_rt_sigpending, &pending_set, sigsetsize);
    if (result == -1) {
        perror("compat_sys_rt_sigpending");
        return 1;
    }

    printf("Pending signals:\n");
    for (int i = 1; i <= 32; i++) {
        if (pending_set.val[0] & (1U << (i - 1))) {
            printf("Signal %d is pending\n", i);
        }
    }
    for (int i = 33; i <= 64; i++) {
        if (pending_set.val[1] & (1U << (i - 33))) {
            printf("Signal %d is pending\n", i);
        }
    }

    return 0;
}
