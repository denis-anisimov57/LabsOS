#pragma once
#include <sys/types.h>
#include <stdbool.h>

struct arFile {
	char* name;
	mode_t mode;
	int userId;
	int groupId;
	int size;
	time_t atime;
	time_t mtime;
	char* content;
};

struct Archiver {
	char* name;
	unsigned long size;
	unsigned long capacity;
	struct arFile** files;	
};

struct Archiver* initArchiver(const char* arPath);

void addToArchiver(struct Archiver* ar, const char* filePath);

void pushArFile(struct Archiver* ar, struct arFile* file);

struct arFile* parseHeader(const char* header);

bool createArFile(struct arFile* file);

void removeFromArchiver(struct Archiver* ar, const char* filename, bool isExtract);

void printArFile(struct arFile* file);

void printArchiver(struct Archiver* ar);

void saveArchive(struct Archiver* ar);

void delArFile(struct arFile* file);

void delArchiver(struct Archiver* ar);
