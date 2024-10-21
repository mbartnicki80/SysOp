#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>

#define BUFFER_SIZE 10

int main(int argc, char const *argv[])
{
    int fd[2];
    pid_t pid;
    char sendBuffer[BUFFER_SIZE] = "welcome!";
    char receiveBuff[BUFFER_SIZE];
    
    // Wyzeruj bufor receiveBuff
    memset(receiveBuff, 0, BUFFER_SIZE);

    // Utwórz anonimowy potok
    if (pipe(fd) == -1)
    {
        fprintf(stderr, "pipe error: %s\n", strerror(errno));
        exit(1);
    }

    // Utwórz proces potomny
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "fork error: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid > 0)  // proces nadrzędny
    {
        close(fd[0]);
        write(fd[1], sendBuffer, BUFFER_SIZE);
        close(fd[1]);
        printf("parent process %d send: %s\n", getpid(), sendBuffer);
    }
    else  // proces potomny
    {
        close(fd[1]);
        read(fd[0], receiveBuff, BUFFER_SIZE);
        close(fd[0]);
        printf("child process %d receive: %s\n", getpid(), receiveBuff);
    }

    return 0;
}

/*
gcc -o pipe pipe.c 
./pipe 
parent process send:welcome!
child  process receive:welcome
*/
