#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile int mode_changes = 0;

void signal_handler(int signal, siginfo_t *info, void *context) {
    if (signal == SIGUSR1) {
        printf("Received %d from sender %d\n", info->si_value.sival_int, info->si_pid);
        kill(info->si_pid, SIGUSR1);
        int mode = info->si_value.sival_int;
        mode_changes++;
        if(mode == 1) {
            for (int i = 1; i <= 100; i++) {
                printf("%d ", i);
            }
            printf("\n");
        } else if(mode == 2) {
            printf("Number of change requests received: %d\n", mode_changes);
        } else if(mode == 3) {
            exit(EXIT_SUCCESS);
        }
    }
}

int main() {    
    printf("Catcher PID: %d\n", getpid());

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = signal_handler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);

    while(1) {
        pause();
    }
}