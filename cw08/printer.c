#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "semaphore_def.h"

Printers* queue;

volatile bool running = 1;

void cleanup() {
    for (int i = 0; i < queue->number_of_printers; i++) {
        sem_destroy(&queue->printers[i].printer_semaphore);
    }
    munmap(queue, sizeof(Printers));
    shm_unlink(SEMAPHORE_NAME);
    printf("Printers closed and removed\n");
}

void sigint_handler(int signum) {
    running = 0;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: <number_of_printers>\n");
        exit(EXIT_FAILURE);
    } 
    int number_of_printers = atoi(argv[1]);

    int shm_fd;
    if ((shm_fd = shm_open(SEMAPHORE_NAME, O_CREAT | O_RDWR, 0666)) < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(Printers)) < 0) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    queue = mmap(NULL, sizeof(Printers), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    queue->number_of_printers = number_of_printers;

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_printers; i++) {
        sem_init(&queue->printers[i].printer_semaphore, 1, 1); //sem, shared, val
        pid_t printer_pid;
        if ((printer_pid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (printer_pid == 0) {
            while (running) {
                int valp;
                sem_getvalue(&queue->printers[i].printer_semaphore, &valp);
                if (valp == 0) {
                    for (int j = 0; j < QUEUE_SIZE; j++) {
                        printf("printer %d: ", i);
                        for (int k = 0; k <= j; k++)
                            printf("%c", queue->printers[i].text[k]);   
                        printf("\n");
                        fflush(stdout);
                        sleep(1);
                    }
                    sem_post(&queue->printers[i].printer_semaphore);
                }
            }
            exit(EXIT_SUCCESS);
        }
    }
    while (wait(NULL) > 0);
    cleanup();
    exit(EXIT_SUCCESS);
}