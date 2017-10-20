#include "myheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

int randomNumber() {
	return rand() % 288;
};

void wordCreator(hangmanWord *arr) {
	FILE *file = fopen("hangman_text.txt", "r");
	int index = 0;
	char str[100];

	while(fgets(str, 100, file)) {
		char *dup = strdup(str);
		char *token = strtok(dup, ",");
		arr[index].object = token;

		token = strtok(NULL, ",'\n'");
		arr[index].type = token;
		index++;
	}
	fclose(file);
	return;

};

void userCreator(person *arr) {
	FILE *file = fopen("Authentication.txt", "r");
	char str[MAXSIZE];
	int lines = 0, index = 0;

	while(fgets(str, MAXSIZE, file)) {
		lines++;
		if (lines > 1) {
			char *dup = strdup(str);
			char *token = strtok(dup, " '\t''\n'");
			arr[index].username = token;

			token = strtok(NULL, " '\t''\n'");
			arr[index].password = token;
			
			arr[index].gamesPlayed = 0;
			arr[index].gamesWon = 0;
			index++;
		}
	}	
	fclose(file);
	return;
};

void createLeaderboard(leaderboard *arr) {
	FILE *file = fopen("Authentication.txt", "r");
	char str[MAXSIZE];
	int lines = 0, index = 0;

	while(fgets(str, MAXSIZE, file)) {
		lines++;

		if(lines > 1) {
			char *dup = strdup(str);
			char *token = strtok(dup, " '\t''\n'");
			arr[index].player = token;
			arr[index].won = 0;
			arr[index].played = 0;
			index++;
		}
	}
	fclose(file);
	return;
};


void clientMenu() {
	printf("\n\nWelcome to the Hangman Gaming System\n\n\n");
	printf("Please enter a selection\n");
	printf("<1> Play Hangman\n");
	printf("<2> Show Leaderboard\n");
	printf("<3> Quit\n\n");
	printf("Selection option 1 - 3 -> ");
	return;
};


void playHangman(int sock_id, hangmanWord *arr, char *user, person *people) {
	char type[MAXSIZE], object[MAXSIZE], fromuser[MAXSIZE], checker[MAXSIZE], checker2[MAXSIZE];
	char right[MAXSIZE] = "right", wrong[MAXSIZE] = "wrong", winner[MAXSIZE] = "win", loser[MAXSIZE] = "lose";

	int r = randomNumber(), tlen, olen, lives, found, position, position2, win, win2, z, both = 0, both2 = 0;
	uint16_t sendLives;
	
	tlen = strlen(arr[r].type);
	olen = strlen(arr[r].object);

	char *arrtype = arr[r].type;
	char *arrobj = arr[r].object;

	lives = guesses(tlen, olen);

	/*	Create '_' based on the length of both type and object	*/
	for (int i = 0; i < tlen; i++) {
		type[i] = '_';
		type[tlen] = '\0';
	}

	for (int i = 0; i < olen; i++) {
		object[i] = '_';
		object[olen] = '\0';
	}

	sendLives = htons(lives);

	/*	Send both type and object '_' and lives	*/
	send(sock_id, type, MAXSIZE, 0);
	send(sock_id, object, MAXSIZE, 0);
	send(sock_id, &sendLives, sizeof(uint16_t), 0);

	while(1) {

		z = 0;
		found = 0;
		/*	Recieve letter	*/
		memset(fromuser, 0, sizeof(fromuser));

		recv(sock_id, fromuser, MAXSIZE, 0);
		
		/*	Check type	*/
		for (int i = 0; i < tlen; i++) {
			if (fromuser[0] == arrtype[i]) {
				found = 1;
				z = 1;
			}
		}

		/*	Check object	*/
		for (int i = 0; i < olen; i++) {
			if(fromuser[0] == arrobj[i]) {
				found = 1;
				z = 1;
			}
		}

		if (found == 0) {
			lives--;
			z = 0;

			if (lives == 0) {
				break;
			}

		} else if (found == 1) {

			for (int i = 0; i < tlen; i++) {
				found = 0;

				if (fromuser[0] == arrtype[i]) {
					found = 1;
					position = i;
				}

				if (found == 1) {
					for (int i = 0; i < tlen; i++) {
						
						if (i == position) {
							type[i] = fromuser[0];

						} else if (type[i] >= 'a' && type[i] <= 'z') {
							continue;
						} else {
							type[i] = '_';
						}

						checker[position] = fromuser[0];
						checker[tlen] = '\0';
						//win = strcmp(arrtype, checker);	
						win = memcmp(checker, arrtype, strlen(arrtype));

						both = 1;		

					}
				} // END of if (found == 1)
			
			}// END of for loop for type



			for (int i = 0; i < olen; i++) {
				found = 0;

				if (fromuser[0] == arrobj[i]) {
					position2 = i;
					found = 1;
				}

				if (found == 1) {
					for (int i = 0; i < olen; i++) {
						if (i == position2) {
							object[i] = fromuser[0];

						} else if (object[i] >= 'a' && object[i] <= 'z') {
							continue;

						} else {
							object[i] = '_';
						}

						checker2[position2] = fromuser[0];
						checker2[olen] = '\0';
						//win2 = strcmp(arrobj, checker2);
						win2 = memcmp(checker2, arrobj, strlen(arrobj));

						both2 = 1;

					}
				}

			}

			if ((win == 0 && win2 == 0) && (both == 1 && both2 == 1)) {

				for (int i = 0; i < 10;i ++) {
					if (strcmp(people[i].username, user) == 0) {
						/*	Once it finds the user increment games won by 1	*/
						people[i].gamesWon++;
					}
				}
				break;
			}	
			
		}// END of else if (found == 1)

		/*	Send the strings and lives */
		send(sock_id, type, strlen(type), 0);
		send(sock_id, object, strlen(object), 0);
		
		/*	z == 0 means the letter recieved is wrong, z == 1 the letter recieved is correct	*/
		if (z == 0) {
			send(sock_id, wrong, strlen(wrong), 0);

		} else if (z == 1) {
			send(sock_id, right, strlen(right), 0);
		}
		

	}// END of while

	if (lives == 0) {
		send(sock_id, loser, strlen(loser), 0);
	} else {
		send(sock_id, winner, strlen(winner), 0);
	}

	/*	Increment games played	*/
	for (int i = 0; i < 10; i++) {
		if (strcmp(people[i].username, user) == 0) {
			people[i].gamesPlayed++;
		}
	
	}
};


void sendLeaderboard(int sock_id, person *people) {
	int totalGames = 0;
	uint16_t tg;

	for (int i = 0; i < USERSIZE; i++) {
		if (people[i].gamesPlayed > 0) {
			totalGames++;
		}
	}

	tg = htons(totalGames);

	/*	Send the length	*/
	send(sock_id, &tg, sizeof(uint16_t), 0);

	/*	Send the user with games played and the name	*/
	if (totalGames > 0) {
		for (int i = 0; i < USERSIZE; i++) {
			if (people[i].gamesPlayed > 0) {
				/*	First send the index of the person	*/
				
				uint16_t num;
				num = htons(people[i].gamesPlayed);

				// Games played
				send(sock_id, &num, sizeof(uint16_t), 0);

				num = htons(people[i].gamesWon);
					
				// games lost
				send(sock_id, &num, sizeof(uint16_t), 0);

				num = htons(i);

				// Index
				send(sock_id, &num, sizeof(uint16_t), 0);
			}
		}
	}
	return;
};


int cmp_gamesWon(const void *a, const void *b) {
	leaderboard *playerA = (leaderboard *) a;
	leaderboard *playerB = (leaderboard *) b;

	return (playerB->won - playerA->won);
};

int cmp_percentage(const void *a, const void *b) {
	leaderboard *playerA = (leaderboard *)a;
	leaderboard *playerB = (leaderboard *)b;

	return(playerB->won/playerB->played - playerA->won/playerA->played);
};

int cmp_names(const void *a, const void *b) {
	leaderboard *playerA = (leaderboard *)a;
	leaderboard *playerB = (leaderboard *)b;

	return strcmp(playerA->player,playerB->player);
};


void showLeaderboard(leaderboard *arr, int length) {
	leaderboard *withGamesPlayed = malloc(USERSIZE * sizeof(leaderboard));

	char *t;
	int j = 0, k;

	for (int i = 0; i < USERSIZE; i++) {
		if (arr[i].played > 0) {
			withGamesPlayed[j] = arr[i];
			j++;
		}
	}

	qsort(withGamesPlayed, j, sizeof(leaderboard), cmp_names);
	qsort(withGamesPlayed, j, sizeof(leaderboard), cmp_percentage);
	qsort(withGamesPlayed, j, sizeof(leaderboard), cmp_gamesWon);


	for (int k = j - 1; k >= 0; k--) {
		printf("\n======================================\n\n");
		printf("Player - %s\n", withGamesPlayed[k].player);
		printf("Number of games won - %d\n", withGamesPlayed[k].won);
		printf("Number of games played - %d\n", withGamesPlayed[k].played);
		printf("\n======================================\n\n");
	}
	free(withGamesPlayed);
};


int guesses(int a, int b) {
	
	return (a + b + 10);
};


