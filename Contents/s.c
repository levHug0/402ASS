#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include "myheader.h"

/*	THIS IS THE SERVER	*/


sig_atomic_t volatile running = 1;
void signalHandler(int signal);

int main(int argc, char *argv[]) {
	srand(time(NULL));
	/*	Signal handler Ctrl + C	*/
	struct sigaction handler;
	handler.sa_handler = signalHandler;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);		

	person *users = malloc(USERSIZE * sizeof(person));
	hangmanWord *words = malloc(WORDSIZE * sizeof(hangmanWord));
	userCreator(users);
	wordCreator(words);

	struct sockaddr_in server, destination;
	int sockfd, newsockfd, num, port, yes = 1;
	socklen_t size;
	char buffer[MAXSIZE];

	memset(buffer, 0, sizeof(buffer));

	port = (argc == 2) ? atoi(argv[1]) : PORT;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket Failure!\n");
		return EXIT_FAILURE;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		printf("setsockopt\n");
		return EXIT_FAILURE;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
		printf("Binding error\n");
		return EXIT_FAILURE;
	}

	listen(sockfd, 5);
	printf("\nServer is listening...\n");

	while(running == 1) {
		size = sizeof(destination);

		if ((newsockfd = accept(sockfd, (struct sockaddr*)&destination, &size)) < 0) {
			printf("Did not accept any connection\n");
			continue;
		}

		printf("\nVerifying a client: %s\n\n", inet_ntoa(destination.sin_addr));
		char usr[MAXSIZE], pass[MAXSIZE];
		char *userLoggedIn;
		int a,b, correct = 0;

		a = recv(newsockfd, usr, MAXSIZE, 0);
		b = recv(newsockfd, pass, MAXSIZE, 0);

		usr[a] = '\0';
		pass[b] = '\0';		

		/***	Verify User	***/

		for (int i = 0; i < USERSIZE; i++) {
			if (memcmp(usr, users[i].username, strlen(users[i].username)) == 0) {
				if (memcmp(pass, users[i].password, 6) == 0) {
					printf("%sLogged in successfully\n\n", usr);
					userLoggedIn = users[i].username;
					correct = 1;
				} 
			}
		}

		if (correct == 0) {
			char approve[MAXSIZE] = "no";
			send(newsockfd, approve, strlen(approve), 0);
			printf("Denying user ... Incorrect username OR password\n");
			close(newsockfd);

		} else if (correct == 1) {
			correct = 0;
			char approve[MAXSIZE] = "yes";
			
			/*	Sends 'yes' to accept the user	*/
			send(newsockfd, approve, strlen(approve), 0);
		
			while(running == 1) {
				int ans;
				uint16_t myans;
				
				/*	Recieve Number of 1, 2 or 3	*/
				if ((recv(newsockfd, &myans, sizeof(uint16_t), 0)) <= 0) {
					printf("Client disconnected\n");
					break;
				}

				ans = ntohs(myans);

				/*	If '1' play hangman, If '2' show leaderboard	*/
				if (ans == 1) {
					printf("%s is playing Hangman\n", userLoggedIn);
					playHangman(newsockfd, words, userLoggedIn, users);
					printf("Game is finished\n");
					printf("%s - Games played: %d | Games won: %d\n", userLoggedIn, users[0].gamesPlayed, users[0].gamesWon);

				} else if (ans == 2) {
					printf("You typed 2\n");
				}

			} // END of while (running == 1)
			close(newsockfd);
		}
	}

	free(users);
	free(words);
	close(sockfd);
	printf("Server disconnected...\n");
	return 0;
}

void signalHandler(int signal) {
	if (signal == SIGINT) {
		printf("\nServer is disconnecting Ctrl + C is pressed\n");
		running = 0;
	}
	return;
};

