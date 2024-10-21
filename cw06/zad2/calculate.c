#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd[2];

double f(double x) {
    return 4.0 / (x * x + 1);
}

double calculate_integral(double start, double end, double step) {
    double sum = 0;

    for (double x = start; x < end; x += step) {
        sum += f(x) * step;
    }
    return sum;
}

int main(void) {


    if (mkfifo("pipe_rd", 0777) == -1) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }
    if (mkfifo("pipe_wr", 0777) == -1) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }

    if ((fd[1] = open("pipe_wr", O_RDONLY)) == -1) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    double step_width;
    if (read(fd[1], &step_width, sizeof(step_width)) == -1) {
        perror("read error");
        exit(EXIT_FAILURE);
    }

    double start = 0.0;
    double end = 1.0;
    double result = calculate_integral(start, end, step_width);

    if ((fd[0] = open("pipe_rd", O_WRONLY)) == -1) {
        perror("open error");
        exit(EXIT_FAILURE);
    }
    if (write(fd[0], &result, sizeof(result)) == -1) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
    close(fd[1]);
    close(fd[0]);
    if (unlink("pipe_wr") == -1) {
        perror("unlink pipe_wr error");
        exit(EXIT_FAILURE);
    }

    if (unlink("pipe_rd") == -1) {
        perror("unlink pipe_rd error");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}