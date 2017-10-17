#include "myheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int randomNumber() {
	return rand() % 289;
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
			index++;
		}
	}	
	fclose(file);
	return;
};


void clientMenu(int sockfd) {
	printf("Welcome to the Hangman Gaming System\n\n\n");
	printf("Please enter a selection\n");
	printf("<1> Play Hangman\n");
	printf("<2> Show Leaderboard\n");
	printf("<3> Quit\n\n");
	printf("Selection option 1 - 3 ->\n\n");
	while(1) {
		int option;
		scanf("%d", &option);

		switch(option) {
			case 1:
				printf("You pressed number 1\n");
				break;
			case 2:
				printf("You pressed number 2\n");

				char two[sizeof(char)] = "2";
				if (send(sockfd, two, sizeof(char), 0) < 0) {
					printf("Sending password error!\n");
					return EXIT_FAILURE;
				}

				break;

			case 3:
				printf("You pressed number 3\n");
				break;
			default:
				printf("That is not a valid option, please try again\n\n");
				clientMenu(sockfd);
		}
	}
	
	return;
};

// comparison function for ascending games won
int cmp_gamesWon(const void *a, const void *b){
	person *playerA = (person *)a;
	person *playerB = (person *)b;
	return (playerB->gamesWon - playerA->gamesWon);
}

// comparison function for ascending percentage of games won
int cmp_percentage(const void *a, const void *b){
	person *playerA = (person *)a;
	person *playerB = (person *)b;	
	return (playerB->gamesWon/playerB->gamesPlayed - playerA->gamesWon/playerA->gamesPlayed);
}

void leaderboard(person *users, int sockfd) {
	person *withGamesPlayed = malloc(USERSIZE * sizeof(person));
	int j = 0;
	for (int i; i < USERSIZE; i++){
		if (users[i].gamesPlayed > 0){
			withGamesPlayed[j] = users[i];
			j++;
		}
		// if no users have played a game
		if (j == 0){
			printf("=============================================================================\n\n");
			printf("There is no information currently stored in the Leader Board. Try again later\n\n");
			printf("=============================================================================\n\n\n\n");
			clientMenu(sockfd);
		}
	}
	// sort array ascending gamesWon
	//qsort(withGamesPlayed, j, sizeof(person), cmp_gamesWon);
	// sort array ascending gamesWon / gamesPlayed
	//qsort(withgamesPlayed, j, sizeof(person), cmp_percentage);
	// sort array by username
	for (int k = 0; k < j; k++){
		printf("========================================\n\n");
		printf("Player  - %s\n", withGamesPlayed[k].username);
		printf("Number of games won - %d\n", withGamesPlayed[k].gamesWon);
		printf("Number of games played - %d\n\n", withGamesPlayed[k].gamesPlayed);
		printf("========================================\n\n");		
	}
}
