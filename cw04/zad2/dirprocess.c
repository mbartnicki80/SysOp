#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int global = 0;

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Inaccurate number of arguments\n");
        exit(EXIT_FAILURE);
    }

    int local = 0;

    printf("Program name: %s\n", argv[0]);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { //child
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        
        execl("/bin/ls", argv[1], NULL);
        
        perror("execl error");
        exit(EXIT_FAILURE);
    } else { //parent
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), pid);

        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status))
            printf("Child exit code: %d\n", WEXITSTATUS(status));
        else
            printf("Child process finished incorrectly\n");
        
        printf("Parent's local = %d, parent's global = %d\n", local, global);
        exit(EXIT_SUCCESS);
    }
}