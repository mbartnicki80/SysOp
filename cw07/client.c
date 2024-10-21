#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MESSAGE_SIZE 4096

mqd_t client_queue;
char client_queue_name[50];

void cleanup() {
    if (mq_close(client_queue) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }
    if (mq_unlink(client_queue_name) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    printf("Client queue closed and removed\n");
}

void sigint_handler(int signum) {
    printf("Received SIGINT, cleaning up and exiting...\n");
    cleanup();
    exit(EXIT_SUCCESS);
}

int main() {
    char buffer[MESSAGE_SIZE];
    char msg[MESSAGE_SIZE - 50];
    unsigned int client_id;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MESSAGE_SIZE;

    sprintf(client_queue_name, "/client_queue_%d", getpid());

    client_queue = mq_open(client_queue_name, O_CREAT | O_RDONLY, 0644, &attr);
    if (client_queue == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY, 0644, NULL);
    if (server_queue == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    sprintf(buffer, "INIT %s", client_queue_name);
    if (mq_send(server_queue, buffer, strlen(buffer) + 1, 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    if (mq_receive(client_queue, buffer, MESSAGE_SIZE, NULL) == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }
    client_id = atoi(buffer);
    printf("Connected to server with ID: %u\n", client_id);

    if (fork() == 0) {
        while (1) {
            if (mq_receive(client_queue, buffer, MESSAGE_SIZE, NULL) == -1) {
                perror("mq_receive");
                exit(EXIT_FAILURE);
            }
            printf("Received message: %s\n", buffer);
        }
    }
    //w tym miejscu, zeby tylko proces macierzysty zamykal kolejke
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    
    printf("Enter messages to send to other clients:\n");
    while (1) {
        fgets(msg, MESSAGE_SIZE - 50, stdin);
        char* newline_pos = strchr(msg, '\n'); // wyszukuje pierwsze \n bo wyswietlanie bylo zepsute
        if (newline_pos != NULL) {
            *newline_pos = '\0'; // zamiana na null char
        }
        sprintf(buffer, "Message %s from client: %u", msg, client_id);
        if (mq_send(server_queue, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
    }
}
