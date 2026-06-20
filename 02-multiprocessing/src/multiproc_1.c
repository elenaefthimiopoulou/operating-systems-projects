#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "util.h"
 
#define SEM_NAME "/mp1_display"

int main(int argc, char *argv[])
{
	if (argc < 3){
		fprintf(stderr, "Usage: %s <repetitions> <string1> [string2 ...]\n", argv[0]);
		exit(1);
	}

	int rep = atoi(argv[1]);
	if (rep <= 0){
		fprintf(stderr,"Error:repetition must be a positive integer\n");
		exit(1);
	}

	int num_strings = argc - 2;

	sem_unlink(SEM_NAME);
	sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
	if (sem == SEM_FAILED){
		perror("sem_open");
		exit(1);
	}

	for (int i =0; i<num_strings; i++){
		pid_t pid = fork();
		if (pid < 0){
			perror("fork");
			exit(1);
		}

		if (pid == 0){
			char *str = argv[i+2];
			for (int j = 0; j < rep; j++){
				sem_wait(sem);
				display(str);
				fflush(stdout);
				sem_post(sem);
			}

			sem_close(sem);
			exit(0);
		}
	}

	for (int i=0; i < num_strings; i++){
		wait(NULL);
	}

	sem_close(sem);
	sem_unlink(SEM_NAME);

	return 0;
}
