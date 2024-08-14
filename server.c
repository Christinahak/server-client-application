#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

void error(const char* msg) {
    perror(msg);
    exit(1);
}

const int maxClients = 5;

bool handle_client(int newsockfd, int* client_count) {
    char buffer[1024];
    int n;

    while (1) {
        bzero(buffer, 1024);
        n = read(newsockfd, buffer, 1023);
        if (n < 0) {
            error("ERROR reading from socket");
        }

        printf("Client: %s\n", buffer);

        if (strncmp("disconnect", buffer, 10) == 0) {
            --(*client_count);
            printf("%d Client disconnected\n", *client_count);
            close(newsockfd);
            exit(0);
        } else if (strncmp("connect", buffer, 7) == 0) {
            if (maxClients <= *client_count) {
                send(newsockfd, "Server full\n", 12, 0);
                close(newsockfd);
                return false;
            }
            send(newsockfd, "connected\n", 10, 0);
            ++(*client_count);
            printf("Connected %d\n", *client_count);
        } else if (strncmp("shell ", buffer, 6) == 0) {
            FILE *fp;
            char result[1024];

            // Execute the shell command
            fp = popen(buffer + 6, "r");
            if (fp == NULL) {
                error("ERROR executing shell command");
            }

            // Read the output a line at a time - output it
            while (fgets(result, sizeof(result) - 1, fp) != NULL) {
                n = write(newsockfd, result, strlen(result));
                if (n < 0) {
                    error("ERROR writing to socket");
                }
            }

            pclose(fp);
        } else {
            n = write(newsockfd, "Unknown command\n", 16);
            if (n < 0) {
                error("ERROR writing to socket");
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Port number not provided. Program terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    key_t key = IPC_PRIVATE;
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        error("ERROR creating shared memory");
    }

    int* client_count = (int*)shmat(shmid, NULL, 0);
    if (client_count == (int*)-1) {
        error("ERROR attaching shared memory");
    }

    *client_count = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    int pid = 1;
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
	if (newsockfd < 0) {
            error("ERROR on accept");
        }

        if (pid > 0) {
	       pid = fork();
	}
        if (pid < 0) {
            error("ERROR on fork");
        }

        if (pid == 0) {
            close(sockfd);
            handle_client(newsockfd, client_count);
            exit(0);
        }
        close(newsockfd); 
        // Clean up any terminated child processes
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
    close(sockfd);
    return 0;
}

