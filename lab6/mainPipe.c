#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	(void)argc, (void)argv;
	pid_t res = 0;

	int pipedesc[2];
	int pipeRes = pipe(pipedesc);
	if(pipeRes != 0) {
		int err = errno;
		fprintf(stderr, "Pipe error: %s (%d)\n", strerror(err), err);
		return 1;
	}
	switch(res = fork()) {
		case -1: {
			int err = errno;
			fprintf(stderr, "Fork error: %s (%d)\n", strerror(err), err);
			break;
		}
		case 0: {
			size_t size = 64;
			char buf[size];

			time_t ts = time(NULL);
			struct tm curTime = *localtime(&ts);
			char* timeStr = (char*)calloc(1, size);
			strftime(timeStr, size + 1, "Child time: %H:%M:%S;\n", &curTime);	
			printf("%s", timeStr);
			free(timeStr);
			
			int len = 0;
			close(pipedesc[1]);
			while((len = read(pipedesc[0], buf, sizeof(buf))) != 0) {
				write(2, buf, len);
			}
			close(pipedesc[0]);

			break;	
		}
		default: {
			sleep(6);
			time_t ts = time(NULL);
			size_t size = 64;
			struct tm curTime = *localtime(&ts);
			char* timeStr = (char*)calloc(1, size);
			strftime(timeStr, size + 1, "Parent time: %H:%M:%S; ", &curTime);
			
			char* pidStr = (char*)calloc(1, size);
			sprintf(pidStr, "Parent pid: %d\n", getpid());
			strcat(timeStr, pidStr);
			
			close(pipedesc[0]);
			write(pipedesc[1], timeStr, strlen(timeStr) + 1);
			free(timeStr);
			free(pidStr);
			close(pipedesc[1]);
					
			break;
		}
	}
	return 0;
}
