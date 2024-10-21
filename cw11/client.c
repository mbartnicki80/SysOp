#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int sockfd;
char id[32];

void handle_signal(int sig_no) {
    send(sockfd, "STOP", 4, 0);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ID> <Server IP> <Server Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(id, argv[1]);
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    send(sockfd, id, strlen(id), 0);

    signal(SIGINT, handle_signal);

    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        if (select(sockfd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            int len = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (len > 0) {
                buffer[len] = '\0';
                printf("%s\n", buffer);
                
                if (strcmp(buffer, "PING") == 0) {
                    send(sockfd, "PONG", 4, 0);
                }
            } else {
                perror("Server closed connection");
                shutdown(sockfd, SHUT_RDWR);
                close(sockfd);
                exit(EXIT_FAILURE);
            }
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            send(sockfd, buffer, strlen(buffer), 0);
        }
    }
}
