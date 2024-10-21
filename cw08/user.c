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
#include <string.h>
#include "semaphore_def.h"

Printers* queue;
volatile bool running = 1;

void cleanup() {
    munmap(queue, sizeof(Printers));
    printf("Queue unmapped\n");
}

void sigint_handler(int signum) {
    running = 0;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: <number_of_printers>\n");
        exit(EXIT_FAILURE);
    } 
    int number_of_users = atoi(argv[1]);

    int shm_fd;
    if ((shm_fd = shm_open(SEMAPHORE_NAME, O_RDWR, 0666)) < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    queue = mmap(NULL, sizeof(Printers), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_users; i++) {
        pid_t user_pid;
        if ((user_pid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (user_pid == 0) {
            while (running) {
                int printer_idx = -1;
                for (int j = 0; j < queue->number_of_printers; j++) {
                    int valp;
                    sem_getvalue(&queue->printers[j].printer_semaphore, &valp);
                    if (valp > 0) {
                        printer_idx = j;
                        break;
                    }
                } 
                if (printer_idx == -1)
                    printer_idx = rand() % queue->number_of_printers;
                

                char buffer[11];
                for (int i = 0; i < 10; i++) {
                    buffer[i] = 'a' + rand() % 26;
                }
                buffer[10] = '\0';
                
                strncpy(queue->printers[printer_idx].text, buffer, 11);
                if(sem_wait(&queue->printers[printer_idx].printer_semaphore) < 0)
                        perror("sem_wait");
                sleep(rand() % 10 + 1);
            }
            exit(EXIT_SUCCESS);
        }
    }
    while (wait(NULL) > 0);
    cleanup();
    exit(EXIT_SUCCESS);
}