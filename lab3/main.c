#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void func() {
	printf("I'm at exit 1 for process %d\n", getpid());
}

void func2() {
	printf("I'm at exit 2 for process %d\n", getpid());
}

void handler(int sig) {
	printf("Signal %d recieved\n", sig);
}

void newActHandler(int sig) {
	printf("Signal %d recieved with sigaction\n", sig);
}

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	int res = 0;
	atexit(func);
	atexit(func2);
	
	switch(res = fork()) {
		case -1: {
			int err = errno;
			fprintf(stderr, "Fork error: %s (%d)\n", strerror(err), err);
			break;
		}
		case 0: {
			printf("[CHILD] I'm child of %d, my pid is %d\n", getppid(), getpid());
			break;
		}
		default: {
			int cres;
			wait(&cres);
			printf("[PARENT] I'm parent of %d, my pid is %d, my parent pid is %d\n", res, getpid(), getppid());
			printf("[PARENT] Child exit code %d\n", WEXITSTATUS(cres));
			break;
		}
	}
	
	signal(SIGINT, handler);
	
	struct sigaction newAct;
	newAct.sa_handler = newActHandler;
	sigaction(SIGTERM, &newAct, NULL);
	sleep(10);
	
	return 0;
}
