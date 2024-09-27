#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void atExitHandler() {
	printf("[atexit] Called atexit for process %d\n", getpid());
}

void onExitHandler() {
	printf("[on_exit] Called on_exit for process %d\n", getpid());
}

void signalHandler(int sig) {
	printf("[signal] Signal %d recieved\n", sig);
}

void sigactionHandler(int sig, siginfo_t* siginfo, void* context) {	
	(void)context;
	printf("[sigactionHandler] Signal %d recieved\n", sig);
	printf("----------\n");
	printf("[sigactionHandler] INFO:\nSignal number: %d\nErrno value: %d\nSending pid: %u\nSending uid: %u\nExit value: %d\n", siginfo->si_signo, siginfo->si_errno, siginfo->si_pid, siginfo->si_uid, siginfo->si_status);
	printf("----------\n");
}

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	atexit(atExitHandler);
	on_exit(onExitHandler, NULL);

	signal(SIGINT, signalHandler);
	
	struct sigaction newAct;
	newAct.sa_flags = SA_SIGINFO;
	newAct.sa_sigaction = sigactionHandler;
	sigaction(SIGTERM, &newAct, NULL);
	
	int res = 0;
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
	sleep(10);
	return 0;
}
