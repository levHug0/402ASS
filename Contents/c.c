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

/*	THIS IS THE CLIENT	*/
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

	if ((n = recv(sockfd, loggedIn, sizeof(loggedIn), 0)) < 0) {
		printf("Server disconnected OR recv error\n\n");
		close(sockfd);
	}

	loggedIn[n] = '\0';

	if (strcmp(loggedIn, "no") == 0) {
		printf("You entered either an incorrect username or a password - disconnecting\n");
		close(sockfd);

	} else if (strcmp(loggedIn, "yes") == 0) {

		while(1) {
			int ans;
			uint16_t myans;
			clientMenu();
			
			/*	Gets number from user 1,2 or 3	*/
			scanf("%d", &ans);
			myans = htons(ans);
			printf("\n\n");
		
			/*	Sends the typed number to server for processing	*/
			if((send(sockfd, &myans, sizeof(uint16_t), 0)) < 0) {
				printf("Server disconnected\n");
				break;
			}
			
			/*	If '1' Play Hangman	*/
			if (ans == 1) {
				int lives, lent, leno, counter, inLetters = 0, storing = 0;
				char type[MAXSIZE], object[MAXSIZE], car[MAXSIZE], catcher[MAXSIZE], guess[MAXSIZE], usedLetters[MAXSIZE];

				uint16_t tlen, olen, recvLives;	

				memset(type, 0, sizeof(type));
				memset(object, 0, sizeof(object));
				memset(usedLetters, 0, sizeof(usedLetters));

				/*	Recieve the type string of '_'	*/
				if((recv(sockfd, type, MAXSIZE, 0)) <= 0) {
					printf("Server disconnected\n");
					close(sockfd);
				}
					
				/*	Recieve the object string of '_'	*/
				if ((recv(sockfd, object, MAXSIZE, 0)) <= 0) {
					printf("Server disconnected\n");
					close(sockfd);
				}

				/*	Recieve the number of guesses(lives)	*/
				if((recv(sockfd, &recvLives, sizeof(uint16_t), 0)) <= 0) {
					printf("Server disconnected\n");
					close(sockfd);	
				}

				lives = ntohs(recvLives);

				lent = strlen(type);
				leno = strlen(object);

				while(1) {
					counter = 0;
					
					printf("Guessed letters: %s\n\n", usedLetters);
					printf("Number of guesses left: %d\n\n", lives);
					printf("Word: ");

					for (int i = 0; i < strlen(type); i++) {
						printf("%c ", type[i]);
					}
					printf(" ");

					for (int i = 0; i < strlen(object); i++) {
						printf("%c ", object[i]);
					}
					printf("\n\n");
					printf("Enter your guess - ");
					scanf("%s", car);
					car[1] = '\0';
					send(sockfd, car, strlen(car), 0);
					
					/*	Recieve the word	*/
					memset(type, 0, strlen(type));
					memset(object, 0, strlen(object));
					memset(catcher, 0, strlen(catcher));
					memset(guess, 0, strlen(guess));
					recv(sockfd, catcher, MAXSIZE, 0);

					/*	If the game hasn't concluded continue playing the game	*/
					if (strcmp(catcher, "win") != 0 || strcmp(catcher, "lose") != 0) {

						for (int i = 0; i < lent; i++) {
							type[i] = catcher[i];
						}

						type[strlen(type)] = '\0';
	
						for (int i = lent; i < (lent + leno); i++) {
							object[counter] = catcher[i];
							counter++;
						}
						object[strlen(object)] = '\0';
						counter = 0;

						guess[0] = catcher[(lent + leno)];
						guess[1] = '\0';
					
						/*	Decrease lives if guess is 'w' = wrong	*/
						if (strcmp(guess, "w") == 0) {
							lives--;
						} 

						/*	Store the letter inside usedLetters and check if letter is already in the array	*/
						if (storing == 1) {
							int finder = 0;

							for (int i = 0; i < strlen(usedLetters); i++) {
								if (usedLetters[i] == car[0]) {
									finder++;
								} else {
									continue;
								}
							}

							/*	This means if it doesn't find any match, then add the letter at the end of the char array	*/
							if (finder == 0) {
								usedLetters[strlen(usedLetters)] = car[0];
								usedLetters[strlen(usedLetters) + 1] = '\0';
							}
	
						} else if (storing == 0) {
							usedLetters[0] = car[0];
							usedLetters[strlen(usedLetters)] = '\0';
							storing = 1;

						}

					}

					if (strcmp(catcher, "win") == 0 || strcmp(catcher, "lose") == 0) {
						break;
					}
			
					usleep(5 * 100000);

				}// END of while

				if (lives == 1) {
					printf("\n    You have run out of guesses. You Lost! The Hangman got you..\n");

				} else if (lives > 1) {
					printf("\n\n|	***	***	***	***	***	***	***	|");
					printf("\n\n               Congratulations You Won a game of Hangman!\n\n");
					printf("|	***	***	***	***	***	***	***	|\n\n");
				}

			} else if (ans == 2) {
				printf("You pressed 2\n");

			} else if (ans == 3) {
				break;
			}

		}// END of while
		close(sockfd);
	}
	printf("Disconnected\n");
	return 0;
}





