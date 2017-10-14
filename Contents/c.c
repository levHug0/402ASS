#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 3490
#define MAXSIZE 10241

int main(int argc, char **argv) {
	struct sockaddr_in server_info;
	struct hostent *host;
	int sockfd, num;

	char buffer[MAXSIZE];
	char buff[MAXSIZE];

	if (argc != 3) {
		printf("Hostname OR Port number missing\n");
		return EXIT_FAILURE;
	}	

	host = gethostbyname(argv[1]);

	if (host == NULL) {
		printf("Cannot get host name\n");
		return EXIT_FAILURE;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		printf("Socket error\n");
		return EXIT_FAILURE;
	}

	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(atoi(argv[2]));
	memcpy(&server_info.sin_addr, host->h_addr, host->h_length);

	if (connect(sockfd, (struct sockaddr*)&server_info, sizeof(struct sockaddr)) < 0) {
		printf("Connection failure\n");
		return EXIT_FAILURE;
	}

	printf("\n============================================\n\n");
	printf("Welcome to the Online Hangman Gaming System\n\n");
	printf("============================================\n\n");
	printf("You are required to logon with your registered Username and Password\n\n");

	while(1) {
		printf("Client: Enter data for Server:\n");
		fgets(buffer, MAXSIZE, stdin);
		
		if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
			printf("Sending message error\n");
			return EXIT_FAILURE;

		} else {
			printf("Client: message being sent: %s\n", buffer);

			// int num value returns the strings length
			num = recv(sockfd, buffer, sizeof(buffer), 0);	

			if (num <= 0) {
				printf("Server terminated OR error\n");
				break;
			}
			
			buffer[num] = '\0';		
			printf("Message recieved from server: %s\n", buffer);
		}
	}
	close(sockfd);
	return 0;
}
