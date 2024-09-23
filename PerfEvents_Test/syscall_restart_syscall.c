#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

void handle_signal(int sig) {
    printf("Signal %d received, calling restart_syscall\n", sig);
    syscall(SYS_restart_syscall);
}

void send_sigusr1(int sig) {
    printf("Sending SIGUSR1 to self after 5 seconds.\n");
    kill(getpid(), SIGUSR1);
}

int main() {
    struct sigaction sa;

    // Set up the signal handler for SIGUSR1
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    // Set up a timer to send SIGUSR1 after 5 seconds
    sa.sa_handler = send_sigusr1;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    alarm(5);  // Set an alarm to go off after 5 seconds

    struct timespec ts;
    ts.tv_sec = 10;
    ts.tv_nsec = 0;

    printf("Going to sleep for 10 seconds. SIGUSR1 will be sent after 5 seconds.\n");

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        printf("nanosleep interrupted by signal, remaining time: %ld.%09ld seconds\n", ts.tv_sec, ts.tv_nsec);
    }

    printf("Sleep finished.\n");

    return 0;
}
