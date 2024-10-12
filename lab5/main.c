#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void addToArchive(const char* archivePath, const char* filePath) {
	int fd = open(archivePath, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	bool isArchExist = (fd == -1);
	fd = open(archivePath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
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
		const char* archName = "SomeArchiveName\n";
		//ssize_t res = write(...); if(res == -1) {...}
		write(fd, archName, strlen(archName));
	}
	int addfd = open(filePath, O_RDONLY);
	if(addfd == -1) {
		fprintf(stderr, "Can't open file to input\n");
		return;
	}
	int fileSize = lseek(addfd, 0, SEEK_END);
	lseek(addfd, 0, 0);
	char* buf = calloc(1, fileSize);
	read(addfd, buf, fileSize);
	close(addfd);
	write(fd, "[", sizeof("["));
	write(fd, filePath, strlen(filePath)); // write fileName instead of filePath ?
	write(fd, ";", 1);
	//st_mode; uid; gid; st_blocks; st_size...
	write(fd, "]\n{\n", sizeof("]\n{\n"));
	write(fd, buf, fileSize);
	write(fd, "}\n", sizeof("}\n"));
	close(fd);
	free(buf);
}

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
	// argc >= 4
	if(strcmp(argv[2], "-i") == 0 || strcmp(argv[2], "--input") == 0) {
		addToArchive(argv[1], argv[3]);
	}
	return 0;
}
