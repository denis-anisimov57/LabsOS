#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

//kill -l // list of signals
// kill -15 <pid> // kill pid with SIGTERM
//sigaction

void func() {
	printf("I'm at exit 1 for process %d\n", getpid());
}

void func2() {
	printf("I'm at exit 2 for process %d\n", getpid());
}

void handler(int sig) {
	printf("Signal %d recieved\n", sig);
//	signal(SIGINT, handler);
}

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	int res = 0;
	atexit(func);
	//signal(SIGINT, handler);

	switch(res = fork()) {
		case -1: {
			int err = errno;
			fprintf(stderr, "Fork error: %s (%d)\n", strerror(err), err);
			break;
		}
		case 0:
			//sleep(1000);
			sleep(3);
			atexit(func2);
			printf("[CHILD] I'm child of %d, my pid is %d\n", getppid(), getpid());
			//return 1;
			break;
		default: {
//			signal(SIGINT, handler);
			signal(SIGINT, SIG_IGN);
			int cres;
			wait(&cres);
			printf("[PARENT] I'm parent of %d, my pid is %d, my parent pid is %d\n", res, getpid(), getppid());
			printf("[PARENT] Child exit code %d\n", WEXITSTATUS(cres));
			break;
		}
	}
	return 0;
}
