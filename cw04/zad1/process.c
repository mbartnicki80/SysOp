#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Inaccurate number of arguments\n");
        exit(EXIT_FAILURE);
    }

    int num = atoi(argv[1]);

    for (int i = 0; i < num; i++) {
        pid_t child_pid = fork();
        if (child_pid < 0) { //error
            perror("fork error");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) { //child
            printf("Child process: PID=%d, Parent PID=%d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        } else { //parent
            wait(NULL);
        }
    }

    printf("%d\n", num);
}