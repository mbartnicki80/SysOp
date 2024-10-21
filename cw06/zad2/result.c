#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd[2];

int main(void) {
    double step_width;
    scanf("%lf", &step_width);


    if ((fd[1] = open("pipe_wr", O_WRONLY)) == -1) {
        perror("open pipe_wr error");
        exit(EXIT_FAILURE);
    }

    if (write(fd[1], &step_width, sizeof(step_width)) == -1) {
        perror("write pipe_rd error");
        exit(EXIT_FAILURE);
    }

    if ((fd[0] = open("pipe_rd", O_RDONLY)) == -1) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    double result;
    if (read(fd[0], &result, sizeof(result)) == -1) {
        perror("read error");
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    close(fd[1]);

    printf("Estimated value of the integral: %.6f\n", result);
    exit(EXIT_SUCCESS);
}