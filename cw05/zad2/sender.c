#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig_no) {
    printf("Received signal %d.\n", sig_no);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <catcher_pid> <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR1, &action, NULL); //rozszerzenie signal, co robic gdy otrzyma sygnal

    union sigval value;
    value.sival_int = mode;
    sigqueue(catcher_pid, SIGUSR1, value); //wysyla sygnal z modem

    sigsuspend(&action.sa_mask); //zawiesza proces az otrzyma sygnal zwrotny
    
    exit(EXIT_SUCCESS);
}