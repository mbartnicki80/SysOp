#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PING_INTERVAL 20 // seconds
#define PING_TIMEOUT 5 // seconds

int server_sock;
typedef struct {
    struct sockaddr_in addr;
    char id[32];
    int active;
    time_t last_ping_time;
    int waiting_for_pong;
} client_t;

client_t clients[MAX_CLIENTS];

void broadcast_message(char *message, char *sender_id, struct sockaddr_in *sender_addr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, sender_id) != 0) {
            sendto(server_sock, message, strlen(message), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
}

void private_message(char *message, char *receiver_id, char *sender_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, receiver_id) == 0) {
            sendto(server_sock, message, strlen(message), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
            break;
        }
    }
}

void remove_client(struct sockaddr_in *client_addr) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].addr.sin_addr.s_addr == client_addr->sin_addr.s_addr && clients[i].addr.sin_port == client_addr->sin_port) {
            clients[i].active = 0;
            break;
        }
    }
}

void list_clients(struct sockaddr_in *client_addr) {
    char message[BUFFER_SIZE] = "Active clients:\n";
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            strcat(message, clients[i].id);
            strcat(message, "\n");
        }
    }
    sendto(server_sock, message, strlen(message), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
}

void signal_handler(int sig_no) {
    close(server_sock);
    exit(EXIT_SUCCESS);
}

void ping_clients() {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            if (clients[i].waiting_for_pong && difftime(now, clients[i].last_ping_time) > PING_TIMEOUT) {
                printf("Client %s timed out\n", clients[i].id);
                remove_client(&clients[i].addr);
            } else if (!clients[i].waiting_for_pong && difftime(now, clients[i].last_ping_time) >= PING_INTERVAL) {
                sendto(server_sock, "PING", 4, 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
                clients[i].last_ping_time = now;
                clients[i].waiting_for_pong = 1;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    fd_set read_fds;
    struct timeval timeout;
    char buffer[BUFFER_SIZE];
    int len;

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s\n", argv[1]);

    signal(SIGINT, signal_handler);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].waiting_for_pong = 0;
    }

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        if (select(server_sock + 1, &read_fds, NULL, NULL, &timeout) < 0) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_sock, &read_fds)) {
            len = recvfrom(server_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_size);
            if (len > 0) {
                buffer[len] = '\0';

                int is_new_client = 1;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].active && clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && clients[i].addr.sin_port == client_addr.sin_port) {
                        is_new_client = 0;
                        break;
                    }
                }

                if (is_new_client) {
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (!clients[i].active) {
                            clients[i].addr = client_addr;
                            strncpy(clients[i].id, buffer, 31);
                            clients[i].id[31] = '\0';
                            clients[i].active = 1;
                            clients[i].last_ping_time = time(NULL);
                            clients[i].waiting_for_pong = 0;
                            printf("New client connected: %s\n", clients[i].id);
                            break;
                        }
                    }
                } else {
                    char command[16];
                    sscanf(buffer, "%s", command);

                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients[i].active && clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && clients[i].addr.sin_port == client_addr.sin_port) {
                            if (strcmp(command, "LIST") == 0) {
                                list_clients(&client_addr);
                            } else if (strcmp(command, "2ALL") == 0) {
                                char message[BUFFER_SIZE];
                                snprintf(message, BUFFER_SIZE, "[%s]: %s", clients[i].id, buffer + 5);
                                broadcast_message(message, clients[i].id, &client_addr);
                            } else if (strcmp(command, "2ONE") == 0) {
                                char receiver_id[32], message[BUFFER_SIZE - 96];
                                sscanf(buffer + 5, "%s %[^\n]", receiver_id, message);
                                char full_message[BUFFER_SIZE];
                                snprintf(full_message, BUFFER_SIZE, "[%s to %s]: %s", clients[i].id, receiver_id, message);
                                private_message(full_message, receiver_id, clients[i].id);
                            } else if (strcmp(command, "STOP") == 0) {
                                remove_client(&clients[i].addr);
                            } else if (strcmp(command, "PONG") == 0) {
                                clients[i].waiting_for_pong = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }

        ping_clients();
    }
}