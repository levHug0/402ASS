#define _GNU_SOURCE
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
#include <pthread.h>
#include "myheader.h"

/*	THIS IS THE SERVER

	**** Thread pool Implementation as well as the process synchronization
	are VERY similar to prac 5 ****

*/

#define MAX_THREAD 10

pthread_mutex_t request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

/* global condition variable for our program. assignment initializes it. */
pthread_cond_t  got_request   = PTHREAD_COND_INITIALIZER;

struct request {
	int number;
	struct request* next;
};


struct request* requests = NULL;
struct request* last_request = NULL;
int num_requests = 0;

sig_atomic_t volatile running = 1;
void signalHandler(int signal);


struct sockaddr_in server, destination;
int sockfd, newsockfd, sockfdlist[MAX_THREAD], num, port, yes = 1;
socklen_t size;

person *users;
hangmanWord *words;


void add_request(int request_num, pthread_mutex_t* p_mutex, pthread_cond_t*  p_cond_var) {
    int rc;
    struct request* a_request;

    a_request = (struct request*)malloc(sizeof(struct request));
    if (!a_request) { 
        fprintf(stderr, "add_request: out of memory\n");
        exit(1);
    }
    a_request->number = request_num;
    a_request->next = NULL;

    rc = pthread_mutex_lock(p_mutex);

    if (num_requests == 0) {
        requests = a_request;
        last_request = a_request;
    }
    else {
        last_request->next = a_request;
        last_request = a_request;
    }

    num_requests++;

    rc = pthread_mutex_unlock(p_mutex);

    rc = pthread_cond_signal(p_cond_var);
}

struct request* get_request(pthread_mutex_t* p_mutex) {
    int rc; 
    struct request* a_request;

    rc = pthread_mutex_lock(p_mutex);

    if (num_requests > 0) {
        a_request = requests;
        requests = a_request->next;
        if (requests == NULL) {
            last_request = NULL;
        }
        num_requests--;
    }
    else { 
        a_request = NULL;
    }

    rc = pthread_mutex_unlock(p_mutex);

    return a_request;
}


void handle_request(struct request* a_request, int thread_id) {
	if (a_request) {
		printf("Thread '%d' handled request '%d'\n", thread_id, a_request->number);

		char usr[MAXSIZE], pass[MAXSIZE];
		char *userLoggedIn;
		int a,b, correct = 0;

		/*	index is the filedescriptor number of the specific thread	*/
		int  index = a_request->number;

		a = recv(sockfdlist[index], usr, MAXSIZE, 0);
		b = recv(sockfdlist[index], pass, MAXSIZE, 0);

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
			send(sockfdlist[index], approve, strlen(approve), 0);
			printf("Denying user ... Incorrect username OR password\n");
			close(newsockfd);

		} else if (correct == 1) {
			correct = 0;
			char approve[MAXSIZE] = "yes";
			
			/*	Sends 'yes' to the client, so they can proceed, if it's not a 'yes' the client will disconnect	*/
			send(sockfdlist[index], approve, strlen(approve), 0);
		
			while(running == 1) {
				int ans;
				uint16_t myans;
				
				/*	Recieve Number of 1, 2 or 3	*/
				if ((recv(sockfdlist[index], &myans, sizeof(uint16_t), 0)) <= 0) {
					printf("%s disconnected\n", userLoggedIn);
					break;
				}

				ans = ntohs(myans);

				/*	If '1' play hangman, If '2' show leaderboard	*/
				if (ans == 1) {
					printf("%s is playing Hangman\n", userLoggedIn);
					playHangman(sockfdlist[index], words, userLoggedIn, users);
					printf("%s finished Playing Hangman\n", userLoggedIn);
					
				} else if (ans == 2) {
					sendLeaderboard(sockfdlist[index], users);
				}

			} // END of while (running == 1)

			close(sockfdlist[index]);
		}			
		fflush(stdout);
	}

};


void *handle_requests_loop(void* data) {
	int rc;

	struct request* a_request;
	int thread_id = *((int*) data);

	rc = pthread_mutex_lock(&request_mutex);

	while(1) {
		if (num_requests > 0) {
			a_request = get_request(&request_mutex);			
			
			if (a_request) {
				rc = pthread_mutex_unlock(&request_mutex);
				handle_request(a_request, thread_id);
				free(a_request);
				rc = pthread_mutex_lock(&request_mutex);
			}
		} else {
			rc = pthread_cond_wait(&got_request, &request_mutex);
		}

	} // END of while
};

int main(int argc, char *argv[]) {
	srand(time(NULL));
	/*	Signal handler Ctrl + C	*/
	struct sigaction handler;
	handler.sa_handler = signalHandler;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);

	users = malloc(USERSIZE * sizeof(person));
	words = malloc(WORDSIZE * sizeof(hangmanWord));

	userCreator(users);
	wordCreator(words);

	struct timespec delay; 

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

	listen(sockfd, 10);

	/*	Create Thread pool	*/
	int thread_id[MAX_THREAD];
	pthread_t pthr[MAX_THREAD];
	int sockCounter = 0;

	for (int i = 0; i < MAX_THREAD; i++) {
		thread_id[i] = i;
		pthread_create(&pthr[i], NULL, handle_requests_loop, (void*)&thread_id[i]);
	}

	while(running == 1) {
		printf("\nServer is listening...\n");
		size = sizeof(destination);

		if ((newsockfd = accept(sockfd, (struct sockaddr*)&destination, &size)) < 0) {
			printf("Did not accept any connection\n");
			continue;
		} else {
			printf("\nVerifying a client: %s\n\n", inet_ntoa(destination.sin_addr));
			sockfdlist[sockCounter] = newsockfd;

			add_request(sockCounter, &request_mutex, &got_request);

			usleep(5 * 100000);

			if (rand() > 3* (RAND_MAX/4)) {
				delay.tv_sec = 0;
				delay.tv_nsec = 10;
				nanosleep(&delay, NULL);
			}
				
			sockCounter++; // Increment counter to store the new client sock
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

