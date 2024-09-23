#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <receiver-pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t receiver_pid = atoi(argv[1]);

    // 更改发送者的有效用户ID
    uid_t new_uid = 65534; // 使用一个普通用户ID来模拟权限不足
    if (seteuid(new_uid) != 0) {
        perror("seteuid");
        exit(EXIT_FAILURE);
    }

    // 尝试发送信号
    printf("Attempting to send signal to PID %d\n", receiver_pid);
    if (kill(receiver_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("Signal sent successfully\n");

    return 0;
}
