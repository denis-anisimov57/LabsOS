#pragma once
#include <sys/types.h>

struct arFile {
	char* name;
	mode_t mode;
	int userId;
	int groupId;
	int size;
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

void removeFromArchiver(struct Archiver* ar, const char* filename);

void printArchiver(struct Archiver* ar);

void saveArchive(struct Archiver* ar);

void delArFile(struct arFile* file);

void delArchiver(struct Archiver* ar);
