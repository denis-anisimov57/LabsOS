#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

const size_t size = 64;

char* getStrTime() {
	time_t ts = time(NULL);
	struct tm curTime = *localtime(&ts);
	char* timeStr = (char*)calloc(1, size);
	strftime(timeStr, size + 1, "%H:%M:%S;", &curTime);
	return timeStr;
}

int main(int argc, char** argv) {
	(void)argc, (void)argv;
	const char* fifoName = "fifoLab";
	int fifoD = open(fifoName, O_RDONLY);
	if(fifoD == -1) {
		int fifoRes = mkfifo(fifoName, S_IRUSR | S_IWUSR);	
 		if(fifoRes != 0) {
			int err = errno;
			fprintf(stderr, "Fifo error: %s (%d)\n", strerror(err), err);
			return 1;
		}
		fifoD = open(fifoName, O_WRONLY);
		for(int i = 0; i < 2; i++) {
			char* pidStr = (char*)calloc(1, 2 * size);
			sprintf(pidStr, "Process %d; Time: ", getpid());
			
			char* timeStr = getStrTime();
			sleep(5);
			strcat(pidStr, timeStr);
				
			write(fifoD, pidStr, size + 1);
			free(timeStr);
			free(pidStr);
		}
		close(fifoD);
	}
	else {
		int len;
		char* buf = (char*)calloc(1, size);
		while((len = read(fifoD, buf, size)) != 0) {
			char* timeStr = getStrTime();
			printf("Current process %d, time %s ------ String from: \"%s\"\n", getpid(), timeStr, buf);	
			free(timeStr);
		}
		free(buf);
		close(fifoD);
	}
	unlink(fifoName);		
	return 0;
}
