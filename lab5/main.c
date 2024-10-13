#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "archiver.h"

#define HEADER_SIZE 512

unsigned numLen(long int num) {
	if(num == 0) {
		return 0;
	}
	unsigned len = 0;
	while(num != 0) {
		num /= 10;
		len++;
	}
	return len;
}

void addToAr(const char* arPath, const char* filePath) {
	int fd = open(arPath, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	bool isArchExist = (fd == -1);
	fd = open(arPath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("Can't open archive\n");
		return;
	}
	struct stat st;
	int statRes = fstat(fd, &st);
	if(statRes == -1) {
		fprintf(stderr, "Stat error\n");
		return;
	}
	if(isArchExist) {
		int seekRes = lseek(fd, 0, SEEK_END);
		if(seekRes == -1) {
			fprintf(stderr, "lseek error\n");
			return;
		}
	}
	else {
//		const char* archName = "SomeArchiveName\n";
		//ssize_t res = write(...); if(res == -1) {...}
//		write(fd, archName, strlen(archName));
	}
	int addfd = open(filePath, O_RDONLY);
	if(addfd == -1) {
		fprintf(stderr, "Can't open file to input\n");
		return;
	}
	int fileSize = lseek(addfd, 0, SEEK_END) + 1;
	lseek(addfd, 0, 0);
	char* fileContest = (char*)calloc(1, fileSize);
	read(addfd, fileContest, fileSize);
	close(addfd);
	char fileHeader[HEADER_SIZE] = {0};
	int headerLen = sprintf(fileHeader, "%s;%u;%d;%d;%d", filePath, st.st_mode, st.st_uid, st.st_gid, fileSize);	
	for(int i = headerLen; i < HEADER_SIZE; i++) {
		fileHeader[i] = '-';
	}
	write(fd, fileHeader, sizeof(fileHeader));
	write(fd, fileContest, fileSize - 1);
	free(fileContest);
	close(fd);
}

void removeFromAr(const char* arPath, const char* filename) {
	int fd = open(arPath, O_RDWR);
	if(fd == -1) {
		fprintf(stderr, "Can't open archive\n");
		return;
	}
	int arSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, 0);
	bool isRemoved = false;
	while(!isRemoved) {
		char* fileHeader = (char*)calloc(1, HEADER_SIZE + 1);
		read(fd, fileHeader, HEADER_SIZE);
		char* curIndex = strchr(fileHeader, ';');
		int statLen = strlen(fileHeader) - strlen(curIndex);
		char* curFilename = (char*)calloc(1, statLen + 1);
		strncpy(curFilename, fileHeader, statLen);
		curIndex = strchr(curIndex, '[');
		char* prevIndex = curIndex + 1;
		curIndex = strchr(curIndex, ']');
		int fileSizeLen = strlen(prevIndex) - strlen(curIndex);
		char* fileSizeStr = (char*)calloc(1, fileSizeLen + 1);
		strncpy(fileSizeStr, prevIndex, fileSizeLen);
		int fileSize = atoi(fileSizeStr);
		if(strcmp(curFilename, filename) != 0) {
			lseek(fd, fileSize - 1, SEEK_CUR);
		}
		else {
			int curPos = lseek(fd, 0, SEEK_CUR) - HEADER_SIZE;
			lseek(fd, fileSize - 1, SEEK_CUR);
			char* arPart = (char*)calloc(1, arSize + 1);
			read(fd, arPart, arSize);
			printf("ArPart: %s\n", arPart);
			lseek(fd, curPos, 0);
			write(fd, arPart, strlen(arPart) - 1);
			isRemoved = true;
			free(arPart);
		}
		free(fileSizeStr);
		free(curFilename);
		free(fileHeader);
	}
	close(fd);
}
// make class archiver
int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		printf("Try ./archiver.out --help or ./archiver.out -h\n");
		return -1;
	}
	if(argc == 2 ) {
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			printf("Manual: ...\n"); // print manual to archiver
			return 0;
		}
		else {
			fprintf(stderr, "Incorrect argument\n");
			printf("Try ./archiver.out -h or ./archiver.out --help\n");
			return -1;
		}
	}
	if(argc == 3) {
		if(strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "--stat") == 0) {
			printf("Archive status: ...\n"); // print archive status
			return 0;
		}
		else {
			fprintf(stderr, "Incorrect argument\n");
			printf("Try ./archiver.out -h or ./archiver.out --help\n");
			return -1;
		}
	}
	srand(time(0));
	// argc >= 4
	if(strcmp(argv[2], "-i") == 0 || strcmp(argv[2], "--input") == 0) {
//		addToAr(argv[1], argv[3]);
		struct Archiver* ar = initArchiver(argv[1]);
		addToArchiver(ar, argv[3]);
		delArchiver(ar);
	}
	else if(strcmp(argv[2], "-e") == 0 || strcmp(argv[2], "--extract") == 0) {
		removeFromAr(argv[1], argv[3]);
	}
	else {
		fprintf(stderr, "Incorrect argumvent\n");
		printf("Try ./archiver.out -h or ./archiver.out --help\n");
		return -1;
	}
	return 0;
}
