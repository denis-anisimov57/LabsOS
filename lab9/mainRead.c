#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sem.h>

const size_t size = 64;
const char* shmemFilename = "semLab";
char* shStr = NULL;
char* addr = NULL;
struct sembuf sem_lock = {0, -1, 0}, sem_open = {0, 1, 0};

char* getStrTimePid() {
	time_t ts = time(NULL);
	struct tm curTime = *localtime(&ts);
	char* timeStr = (char*)calloc(1, size);
	strftime(timeStr, size + 1, "%H:%M:%S;", &curTime);
	char* pidStr = (char*)calloc(1, size);
	sprintf(pidStr, "Process: %d; ", getpid());
	strcat(pidStr, timeStr);
	free(timeStr);
	return pidStr;
}

void printError(const char* preStr) {
	int err = errno;
	fprintf(stderr, "%s: %s (%d)\n", preStr, strerror(err), err);
}

void signalHandler(int sig) {
	printf("Stopping the process reading from the shared memory (signal %d)\n", sig);
	if(shStr != NULL) free(shStr);
	if(addr != NULL) {
		int res = shmdt(addr);
		if(res < 0) {
			printError("Shmdt error");
			exit(1);
		}	
	}
	exit(0);	
}

int main(int argc, char** argv) {
	(void)argc, (void)argv;

	key_t key = ftok(shmemFilename, 1);
	if(key < 0) {
		printError("Ftok error");
		return -1;
	}

	int shmid = shmget(key, size, 0666);
	if(shmid < 0) {
		printError("Shmid error");
		return -1;
	}
	int semid = semget(key, 1, 0666);
	if(semid == -1) {
		printError("Semget error");
		return -1;
	}
	addr = shmat(shmid, NULL, SHM_RDONLY);
	if(addr == (void*)-1) {
		printError("Shmat error");
		return -1;
	}

	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);
	shStr = (char*)calloc(1, size);
		
	while(1) {
		semop(semid, &sem_lock, 1);
		strcpy(shStr, addr);
		char* infoStr = getStrTimePid();
		printf("[Current process info]: %s [Shared memory info]: %s\n", infoStr, shStr);
		free(infoStr);
		semop(semid, &sem_open, 1);
		sleep(1);
	}
	return 0;
}
