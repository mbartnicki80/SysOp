#define _POSIX_C_SOURCE 200809L // Ensure POSIX feature availability
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void signal_handler(int sig_no) {
    printf("Received signal %d.\n", sig_no);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <action>\n", argv[0]);
        printf("Actions: none | ignore | handler | mask\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "none") == 0) {
        // Do nothing, = signal(SIGUSR1, SIG_DFL);
    } else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    } else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, signal_handler);
    } else if (strcmp(argv[1], "mask") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
            perror("Failed to block signal");
    } else {
        printf("Invalid argument. Possible arguments: none | ignore | handler | mask\n");
        exit(EXIT_FAILURE);
    }

    if (raise(SIGUSR1) != 0) {
        perror("Error raising signal");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}