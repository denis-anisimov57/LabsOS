#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {
	(void)argc, (void)argv;
	const char* fifoName = "fifoLab";
	int fifoRes = mkfifo(fifoName, S_IRUSR | S_IWUSR);	
	if(fifoRes != 0) {
		int err = errno;
		fprintf(stderr, "Fifo error: %s (%d)\n", strerror(err), err);
		return 1;
	}
	pid_t res;
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

			int fd = open(fifoName, O_RDONLY);
			if(fd == -1) {
				fprintf(stderr, "Open fifo error\n");
				return 1;
			}
			int len = 0;
			while((len = read(fd, buf, sizeof(buf))) != 0) {
				write(2, buf, len);
			}
			close(fd);

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
			free(pidStr);

			int fd = open(fifoName, O_WRONLY);
			if(fd == -1) {
				fprintf(stderr, "Open fifo error\n");
				free(timeStr);
				return 1;
			}
			write(fd, timeStr, strlen(timeStr) + 1);
			free(timeStr);
			close(fd);
					
			break;
		}
	}
	unlink(fifoName);
	return 0;
}
