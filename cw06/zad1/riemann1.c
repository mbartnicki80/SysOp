#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

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

int main(int argc, char *argv[]) {
    struct timespec time_start, time_finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &time_start);

    if (argc != 3) {
        printf("Usage: %s <step_width> <num_processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    double step_width = atof(argv[1]); //na double
    int num_processes = atoi(argv[2]);

    double start = 0.0;
    double end = 1.0;

    if (ceil((end - start) / step_width) < num_processes) {
        printf("Too many processes\n");
        exit(EXIT_FAILURE);
    }

    int pipes[num_processes][2];
    for (int i = 0; i < num_processes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe error");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid, wpid;

    for (int i = 0; i < num_processes; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            close(pipes[i][0]);
            double interval_start = start + (i * (end - start)) / num_processes;
            double interval_end = (start + ((i + 1) * (end - start)) / num_processes);
            double result = calculate_integral(interval_start, interval_end, step_width);
            if (write(pipes[i][1], &result, sizeof(result)) == -1) {
                close(pipes[i][1]);
                perror("write error");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][1]);
            exit(EXIT_SUCCESS);
        }
    }
    while ((wpid = wait(NULL)) > 0);
    
    double total_sum = 0;
    for (int i = 0; i < num_processes; i++) {
        close(pipes[i][1]);
        double result;
        if (read(pipes[i][0], &result, sizeof(result)) <= 0) {
            close(pipes[i][0]);
            perror("read error");
            exit(EXIT_FAILURE);
        }
        total_sum += result;
        close(pipes[i][0]);
    }

    printf("Estimated value of the integral: %.6f\n", total_sum);
    
    clock_gettime(CLOCK_MONOTONIC, &time_finish);
    elapsed = (time_finish.tv_sec - time_start.tv_sec); //sekundy
    elapsed += (time_finish.tv_nsec - time_start.tv_nsec) / 1000000000.0; //+ nanosekundy
    printf("Time taken: %f seconds\n", elapsed);
    exit(EXIT_SUCCESS);
}