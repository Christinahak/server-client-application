#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

void error(const char* msg) {
    perror(msg);
    exit(1);
}

void prompt() {
    printf("Client> ");
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[1024];
    int connected = 0; // flag to check connection status

    fd_set readfds;

    while (1) {
        prompt();
        bzero(buffer, 1024);
        fgets(buffer, 1023, stdin);

        // Remove newline character from fgets
        buffer[strcspn(buffer, "\n")] = 0;

        if (strncmp("connect", buffer, 7) == 0) {
            if (connected) {
                printf("Already connected to a server. Please disconnect first.\n");
                continue;
            }
            char* token = strtok(buffer, " ");
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Usage: connect <IP_ADDRESS> <PORT>\n");
                continue;
            }
            char ip_address[1000];
	    strncpy(ip_address, token, sizeof(ip_address) - 1);
	    ip_address[sizeof(ip_address) - 1] = '\0';
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Usage: connect <IP_ADDRESS> <PORT>\n");
                continue;
            }
            portno = atoi(token);

            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                error("ERROR opening socket");
            }

            server = gethostbyname(ip_address);
            if (server == NULL) {
                fprintf(stderr, "ERROR, no such host\n");
                exit(0);
            }

            bzero((char*)&serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
            serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    		error("ERROR connecting");
	}

	// Send the connect command to the server
	sprintf(buffer, "connect %s %d", ip_address, portno);
	write(sockfd, buffer, strlen(buffer));

	// Loop until a response is received
	bzero(buffer, 1024);
	while ((n = recv(sockfd, buffer, 1024, 0)) <= 0) {
    		if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        		error("ERROR reading from socket");
    		}
	}	

	printf("Server: %s\n", buffer);

	if (strncmp(buffer, "Server full", 11) == 0) {
    		close(sockfd);
    		connected = 0;
	} else {
    		printf("Connected to %s:%d\n", ip_address, portno);
    		connected = 1;
	}

        } else if (strncmp("disconnect", buffer, 10) == 0) {
            if (!connected) {
                printf("Not connected to any server.\n");
                continue;
            }
            write(sockfd, buffer, strlen(buffer));
            close(sockfd);
            printf("Disconnected from server\n");
            connected = 0;
        } else if (strncmp("shell ", buffer, 6) == 0) {
            if (!connected) {
                printf("Not connected to any server.\n");
                continue;
            }
            write(sockfd, buffer, strlen(buffer));

            // Use select to wait for data from server
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            struct timeval timeout;
            timeout.tv_sec = 2; // Wait for 2 seconds
            timeout.tv_usec = 0;

            int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
            if (activity < 0) {
                error("ERROR in select");
            }

            if (FD_ISSET(sockfd, &readfds)) {
                    bzero(buffer, 1024);
                    n = read(sockfd, buffer, 1023);
                    if (n < 0) {
                        error("ERROR reading from socket");
                    }
                    if (n == 0) {
                        break;
                    }
                    printf("%s", buffer);
            }
        } else {
            printf("Unknown command\n");
        }

    }

    return 0;
}

