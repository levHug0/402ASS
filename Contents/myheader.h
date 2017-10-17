#ifndef MY_FAVORITE_HEADER
#define MY_FAVORITE_HEADER
#define PORT 12345	
#define MAXSIZE 1024
#define USERSIZE 10
#define WORDSIZE 288

typedef struct {
	char *username;
	char *password;
	int gamesPlayed;
	int gamesWon;
}person; 

typedef struct {
	char *type;
	char *object;

}hangmanWord;

int randomNumber();

void wordCreator(hangmanWord *arr);

void userCreator(person *arr);

void clientMenu();

int cmp_gamesWon(const void *a, const void *b);

int cmp_percentage(const void *a, const void *b);

void leaderboard();

#endif
