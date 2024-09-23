#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void print_sigset(const sigset_t *set) {
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(set, i)) {
            printf("Signal %d is blocked\n", i);
        }
    }
}

int main() {
    sigset_t new_set, old_set;
    int result;

    // 初始化信号集，将 SIGINT 添加到新信号集中
    sigemptyset(&new_set);
    sigaddset(&new_set, SIGINT);

    // 打印新设置的信号集
    printf("New signal set to be blocked:\n");
    print_sigset(&new_set);

    // 调用 sigprocmask，设置信号屏蔽字
    result = sigprocmask(SIG_BLOCK, &new_set, &old_set);
    if (result == -1) {
        perror("sigprocmask");
        return 1;
    }

    printf("Successfully blocked SIGINT\n");

    // 打印之前的信号屏蔽字
    printf("Previous signal mask:\n");
    print_sigset(&old_set);

    // 检查当前信号屏蔽字
    sigset_t current_set;
    sigprocmask(SIG_SETMASK, NULL, &current_set);
    printf("Current signal mask after blocking SIGINT:\n");
    print_sigset(&current_set);

    // 恢复之前的信号屏蔽字
    result = sigprocmask(SIG_SETMASK, &old_set, NULL);
    if (result == -1) {
        perror("sigprocmask");
        return 1;
    }

    printf("Restored previous signal mask\n");

    // 检查恢复后的信号屏蔽字
    sigprocmask(SIG_SETMASK, NULL, &current_set);
    printf("Current signal mask after restoring:\n");
    print_sigset(&current_set);

    return 0;
}
