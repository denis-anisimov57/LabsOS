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
char* addr = NULL;
int shmid = -1;
int semid = -1;
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
	printf("Stopping the process writing to the shared memory (signal %d)\n", sig);
	if(addr != NULL) {
		int res = shmdt(addr);
		if(res < 0) {
			printError("Shmdt error");
			exit(1);
		}
	}
	if(shmid != -1) {
		int rmRes = shmctl(shmid, IPC_RMID, NULL);
		if(rmRes < 0) {
			printError("Shmctl remove error");
			exit(1);
		}
	}
	if(semid != -1) {
		int rmRes = semctl(semid, 0, IPC_RMID);
		if(rmRes < 0) {
			printError("Semctl remove error");
			exit(1);
		}
	}
	if((remove(shmemFilename)) == -1) {
		printError("Can't remove shmem file");
		exit(1);
	}
	exit(0);	
}

int main(int argc, char** argv) {
	(void)argc, (void)argv;

	int fd = open(shmemFilename, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printError("Create file error");
		return 1;
	}
	close(fd);

	key_t key = ftok(shmemFilename, 1);
	if(key < 0) {
		printError("Ftok error");
		return -1;
	}

	shmid = shmget(key, size, 0666 | IPC_CREAT);
	if(shmid < 0) {
		printError("Shmid error");
		return -1;
	}
	semid = semget(key, 1, 0666 | IPC_CREAT);
		semop(semid, &sem_open, 1);
	if(semid == -1) {
		printError("Semget error");
		return -1;
	}
	addr = shmat(shmid, NULL, 0);
	if(addr == (void*)-1) {
		printError("Shmat error");
		return -1;
	}

	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);
	
	while(1) {
		semop(semid, &sem_lock, 1);
		char* shStr = getStrTimePid();
		strcpy(addr, shStr);
		sleep(3);
		printf("-----Writing-----\n");
		free(shStr);
		semop(semid, &sem_open, 1);
	}		
	return 0;
}
