#pragma once
#include "lsFile.h"

struct lsList {
	struct lsFile** files;
	unsigned memSize;
	unsigned size;
	unsigned maxNumLinksLength;
	unsigned maxNameLength;
	unsigned maxGroupLength;
	unsigned maxSizeLength;
	unsigned maxUserLength;
};

unsigned getNumberLength(int number);

struct lsList* lsListInit();

void addLsFile(struct lsList* list, struct lsFile* file);

void printSpaces(unsigned number);

void printLsList(struct lsList* list);

void deleteLsList(struct lsList* list);
