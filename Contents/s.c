#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

typedef struct {
	char *username;
	char *password;
	int gamesPlayed;
	int gamesWon;
}person; 

int main(int argc, char *argv[]) {
	person *users = malloc(10 * sizeof(person));
	FILE *file = fopen("Authentication.txt", "r");
	char str[100];
	int lines = 0, index = 0, i;

	if (file == NULL) {
		printf("Error opening Authentication.txt\n");
		return EXIT_FAILURE;
	}
	
	while(fgets(str, 100, file)) {
		lines++;
		if (lines > 1) {
			char *dup = strdup(str);
			char *token = strtok(dup, " '\t'");
			users[index].username = token;
			
			token = strtok(NULL, " '\t'");
			users[index].password = token;
			index++;
		}
	}
	fclose(file);

	for (i = 0; i < 10; i++) {
		printf("Usr: %s Pass: %s\n", users[i].username, users[i].password);
	}
	
	/**************************************************************/
	/******			Socket	Below			*******/
	/**************************************************************/

	struct sockaddr_in server, destination;
	int status, sockfd, newsockfd, num, port;
	socklen_t size;
	char buffer[10241];

	memset(buffer, 0, sizeof(buffer));

	/*	port 	*/
	port = (argc == 2) ? atoi(argv[1]) : PORT;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket Failure!\n");
		return EXIT_FAILURE;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
	
	/*	Bind	*/
	if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("Binding error\n");
		return EXIT_FAILURE;
	}

	listen(sockfd, 5);
	printf("Server is listening...\n");

	while(1) {
		size = sizeof(destination);
		newsockfd = accept(sockfd, (struct sockaddr*)&destination, &size);
		if (newsockfd < 0) {
			printf("Accept failure\n");
			return EXIT_FAILURE;
		}
		
		/*	Implement a way to authenticate 2 messages from the user	*/
		/*	Implement a way to authenticate 2 messages from the user	*/


		printf("A client has successfully connected: %s\n", inet_ntoa(destination.sin_addr));

		while(1) {
			if ((num = recv(newsockfd, buffer, 10241, 0)) < 0) {
				printf("recv error\n");
				return EXIT_FAILURE;

			} else if (num == 0) {
				printf("Connection %s closed\n", inet_ntoa(destination.sin_addr));
				break;
			}

			buffer[num] = '\0';
			printf("Server recieved message: %s\n", buffer);

			/*	Send what the client said	*/
			if ((send(newsockfd, buffer, strlen(buffer), 0)) < 0) {
				printf("Sending message back to client error\n");
				close(newsockfd);
				break;
			}
			
			printf("Server: msg %s\nNumber of bytes: %d\n", buffer, strlen(buffer));
		}
		
		close(newsockfd);		
	}
	
	close(sockfd);
	return 0;
}
