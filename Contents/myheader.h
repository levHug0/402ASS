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


/*	This will be used by the client to view the leaderboard	*/
typedef struct {
	char *player;
	int won;
	int played;

}leaderboard;

int randomNumber();

void wordCreator(hangmanWord *arr);

void userCreator(person *arr);

void createLeaderboard(leaderboard *arr);

void clientMenu();	// Prints client menu

void playHangman(int sock_id, hangmanWord *arr, char *user, person *people);

int guesses(int a, int b);

#endif
