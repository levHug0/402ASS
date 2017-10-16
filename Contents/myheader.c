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


void clientMenu() {
	printf("Welcome to the Hangman Gaming System\n\n\n");
	printf("Please enter a selection\n");
	printf("<1> Play Hangman\n");
	printf("<2> Show Leaderboard\n");
	printf("<3> Quit\n\n");
	printf("Selection option 1 - 3 ->\n\n");
	
	return;
};



