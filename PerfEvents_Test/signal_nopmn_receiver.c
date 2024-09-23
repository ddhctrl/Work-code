#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_signal(int sig) {
    printf("Received signal %d\n", sig);
    exit(0);
}

int main() {
    printf("Receiver PID: %d\n", getpid());

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // 等待信号
    printf("Waiting for signal...\n");
    while (1) {
        pause();
    }

    return 0;
}
