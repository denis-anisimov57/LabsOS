#include "archiver.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

const unsigned HEADER_SIZE = 512;

char* pathToName(const char* filePath) {
	char* cur = strchr(filePath, '/');
	char* prev = (char*)filePath;
	while(cur != NULL) {
		prev = cur + 1;
		cur = strchr(prev, '/');
	}
	char* filename = (char*)calloc(1, strlen(prev) + 1);
	strcpy(filename, prev);
	return filename;
}

struct arFile* parseHeader(const char* header) {
	struct arFile* file = (struct arFile*)calloc(1, sizeof(struct arFile));
	char* curIndex = strchr(header, '/');
	int statLen = strlen(header) - strlen(curIndex);
	file->name = (char*)calloc(1, statLen + 1);
	strncpy(file->name, header, statLen);
	sscanf(curIndex + 1, "%u", &file->mode); 
	curIndex = strchr(curIndex + 1, '/');
	sscanf(curIndex + 1, "%d", &file->userId);
	curIndex = strchr(curIndex + 1, '/');
	sscanf(curIndex + 1, "%d", &file->groupId);
	curIndex = strchr(curIndex + 1, '/');
	sscanf(curIndex + 1, "%d", &file->size);
	file->content = (char*)calloc(1, file->size + 1);
	return file;
}

void pushArFile(struct Archiver* ar, struct arFile* file) {
	if(ar->files == NULL) {
		ar->files = (struct arFile**)calloc(2, sizeof(struct arFile*));
		ar->capacity = 2;
	}
	ar->size++;
	if(ar->size > ar->capacity) {
		ar->capacity *= 2;
		ar->files = (struct arFile**)realloc(ar->files, ar->capacity * sizeof(struct arFile*));
	}
	ar->files[ar->size - 1] = file;
}

struct Archiver* initArchiver(const char* arPath) {
	int fd = open(arPath, O_RDWR);
	struct Archiver* ar = (struct Archiver*)calloc(1, sizeof(struct Archiver));
	ar->name = (char*)calloc(1, strlen(arPath) + 1);
	strcpy(ar->name, arPath);
	ar->files = NULL;
	ar->capacity = 0;
	ar->size = 0;
	if(fd == -1) {
		return ar;
	}
	int arSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, 0);
	char* fileHeader = (char*)calloc(1, HEADER_SIZE + 1);
	while(lseek(fd, 0, SEEK_CUR) != arSize) {
		read(fd, fileHeader, HEADER_SIZE);
		struct arFile* file = parseHeader(fileHeader);
		read(fd, file->content, file->size);
		pushArFile(ar, file);
	}
	free(fileHeader);
	close(fd);
	return ar;
}

void addToArchiver(struct Archiver* ar, const char* filePath) {
	int addfd = open(filePath, O_RDONLY);
	if(addfd == -1) {
		perror("Can't open file\n");
		return;
	}
	struct stat st;
	int statRes = stat(filePath, &st);
	if(statRes == -1) {
		close(addfd);
		fprintf(stderr, "Stat error\n");
		return;
	}
	struct arFile* file = (struct arFile*)calloc(1, sizeof(struct arFile));
	int fileSize = lseek(addfd, 0, SEEK_END) + 1;
	lseek(addfd, 0, 0);
	file->content = (char*)calloc(1, fileSize);
	read(addfd, file->content, fileSize);
	close(addfd);

	file->name = pathToName(filePath);
	removeFromArchiver(ar, file->name, false);
	file->mode = st.st_mode;
	file->userId = st.st_uid;
	file->groupId = st.st_gid;
	file->size = fileSize;
	pushArFile(ar, file);
}

bool createArFile(struct arFile* file) {
	int fd = open(file->name, O_RDWR | O_CREAT | O_EXCL, file->mode);
	if(fd == -1) {
		perror("Can't create file\n");
		return false;
	}
	fchmod(fd, file->mode);
	write(fd, file->content, file->size);
	close(fd);
	return true;
}

void removeFromArchiver(struct Archiver* ar, const char* filename, bool isExtract) {	
	for(unsigned long i = 0; i < ar->size; i++) {
		if(strcmp(ar->files[i]->name, filename) == 0) {
			if(isExtract) {
				if(!createArFile(ar->files[i])) return;
			}
			delArFile(ar->files[i]);
			if(i != ar->size - 1) {
				ar->files[i] = ar->files[ar->size - 1];
			}
			ar->size--;
			break;
		}
	}
}

void printArFile(struct arFile* file) {
	printf("Filename: %s; mode: %u; uid: %d; gid: %d; size: %d;\n", file->name, file->mode, file->userId, file->groupId, file->size);
}

void printArchiver(struct Archiver* ar) {
	for(unsigned long i = 0; i < ar->size; i++) {
		printArFile(ar->files[i]);
	}
}

void saveArchive(struct Archiver* ar) {
	int fd = open(ar->name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("Can't open archive\n");
		return;
	}
	for(unsigned long i = 0; i < ar->size; i++) {
		char* fileHeader = (char*)calloc(1, HEADER_SIZE + 1);
		struct arFile* file = ar->files[i];
		int headerLen = sprintf(fileHeader, "%s/%u/%d/%d/%d", file->name, file->mode, file->userId, file->groupId, file->size);	
		for(unsigned i = headerLen; i < HEADER_SIZE; i++) {
			fileHeader[i] = '-';
		}
		write(fd, fileHeader, HEADER_SIZE);
		write(fd, file->content, file->size);	
		free(fileHeader);
	}
	close(fd);

}

void delArFile(struct arFile* file) {
	if(file != NULL) {
		if(file->name != NULL) free(file->name);
		if(file->content != NULL) free(file->content);
		free(file);
	}
}

void delArchiver(struct Archiver* ar) {
	if(ar != NULL) {
		if(ar->name != NULL) free(ar->name);
		if(ar->files != NULL) {
			for(unsigned long i = 0; i < ar->size; i++) {
				if(ar->files[i] != NULL) delArFile(ar->files[i]);
			}
			free(ar->files);
		}
		free(ar);
	}
}
