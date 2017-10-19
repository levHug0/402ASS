#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "myheader.h"

int main(int argc, char **argv) {
	struct sockaddr_in server_info;
	struct hostent *host;
	int sockfd, num, n;

	char buffer[MAXSIZE];
	char username[MAXSIZE];
	char password[MAXSIZE];
	char loggedIn[MAXSIZE];

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
	printf("Please enter your username--> ");
	fgets(username, MAXSIZE, stdin);
	if (send(sockfd, username, strlen(username), 0) < 0) {
		printf("Sending username error!\n");
		return EXIT_FAILURE;
	}
	printf("\n");
	printf("Please enter your password--> ");
	fgets(password, MAXSIZE, stdin);
	if (send(sockfd, password, strlen(password), 0) < 0) {
		printf("Sending password error!\n");
		return EXIT_FAILURE;
	}
	printf("\n");	

	n = recv(sockfd, loggedIn, sizeof(loggedIn), 0);
	loggedIn[n] = '\0';

	
	if (strcmp(loggedIn, "no") == 0) {
		printf("You entered either an incorrect username or a password - disconnecting\n");
		close(sockfd);

	} else if (strcmp(loggedIn, "yes") == 0) {
		printf("Welcome to the Hangman Gaming System\n\n\n");
		clientMenu(sockfd);
		n = recv(sockfd, loggedIn, sizeof(loggedIn), 0);
		close(sockfd);
	}
	
	return 0;
}






