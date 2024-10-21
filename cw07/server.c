#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>

#define MAX_CLIENTS 10
#define SERVER_QUEUE_NAME "/server_queue"
#define MESSAGE_SIZE 4096

struct client_info {
    int client_id;
    mqd_t queue_descriptor;
};

struct client_info clients[MAX_CLIENTS];
mqd_t server_queue;

void cleanup() {
    if (mq_close(server_queue) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }
    if (mq_unlink(SERVER_QUEUE_NAME) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    printf("Server queue closed and removed\n");
}

void sigint_handler(int signum) {
    printf("Received SIGINT, cleaning up and exiting...\n");
    cleanup();
    exit(EXIT_SUCCESS);
}

int main() {
    int client_count = 0;
    char buffer[MESSAGE_SIZE];
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MESSAGE_SIZE;

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (server_queue == -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Server started, waiting for messages...\n");

    while (1) {
        if (mq_receive(server_queue, buffer, MESSAGE_SIZE, NULL) == -1) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }

        if (strncmp(buffer, "INIT", 4) == 0) {
            char client_queue_name[50];
            int client_id;

            sscanf(buffer, "INIT %s", client_queue_name); //przypisanie %s do client_queue_name z buffera

            mqd_t client_queue = mq_open(client_queue_name, O_WRONLY);
            if (client_queue == -1) {
                perror("mq_open");
                exit(EXIT_FAILURE);
            }

            client_id = client_count + 1;
            clients[client_count].client_id = client_id;
            clients[client_count].queue_descriptor = client_queue;
            client_count++;

            sprintf(buffer, "%d", client_id); //wpisanie client_id do buffera
            if (mq_send(client_queue, buffer, strlen(buffer) + 1, 0) == -1) {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }

            printf("Client connected with ID: %d\n", client_id);
        }
        else {
            char* client_id_str = strstr(buffer, "from client: "); //wyszukanie wzorca w bufferze
            unsigned int client_id_rcv;
            if (client_id_str != NULL) {
                client_id_str += strlen("from client: "); // wskaznik na poczatek id clienta
                client_id_rcv = atoi(client_id_str);
            } else printf("Invalid message format\n");

            for (int i = 0; i < client_count; i++) {
                if (clients[i].client_id == client_id_rcv) continue; //zeby nie wyslac do samego siebie
                
                if (mq_send(clients[i].queue_descriptor, buffer, strlen(buffer) + 1, 0) == -1) {
                    perror("mq_send");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
