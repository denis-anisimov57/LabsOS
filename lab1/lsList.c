#include "lsList.h"

const char greenColor[] = "\x1b[32m";
const char blueColor[] = "\x1b[34m";
const char cyanColor[] = "\x1b[36m";
const char resetColor[] = "\x1b[0m";

void printSpaces(unsigned number) {
	for(int i = 0; i < number; i++) {
		printf(" ");
	}
}

void printColor(int color) {
	switch(color) {
		case GREEN:
			printf("%s", greenColor);
			break;
		case CYAN:
			printf("%s", cyanColor);
			break;
		case BLUE:
			printf("%s", blueColor);
			break;
		default:
			break;
	}	
}

unsigned getNumberLength(int number) {
	if(number == 0) {
		return 1;
	}
	unsigned length = 0;
	while(number != 0) {
		length++;
		number /= 10;
	}
	return length;
}

unsigned long getBlockSize(unsigned long size) {
	if(size == 0) {
		return 0;
	}
	if((size % 1024) == 0) {
		return size / 1024;
	}
	return (size / 4096) * 4 + 4;
}

void writeMax(unsigned* a, unsigned b) {
	*a = *a > b ? *a : b;
}

struct lsList* lsListInit() {
	struct lsList* list = malloc(sizeof(struct lsList));
	list->files = NULL;
	list->size = 0;
	list->memSize = 0;
	list->maxNumLinksLength = 0;
	list->maxNameLength = 0;
	list->maxSizeLength = 0;
	list->maxGroupLength = 0;
	list->maxUserLength = 0;
	list->totalFilesSize = 0;
	list->lFlag = false;
}

void addLsFile(struct lsList* list, struct lsFile* file) {
	if(list->files == NULL) {
		list->files = malloc(2 * sizeof(struct lsFile*));
		list->memSize = 2;
	}
	list->size++;
	if(list->size > list->memSize) {
		list->memSize *= 2;
		list->files = realloc(list->files, list->memSize * sizeof(struct lsFile*));
	}
	list->files[list->size - 1] = file;
	if(list->lFlag) {
		writeMax(&list->maxNumLinksLength, getNumberLength(file->numlink));
		writeMax(&list->maxSizeLength, getNumberLength(file->size));
		writeMax(&list->maxNameLength, strlen(file->name));
		writeMax(&list->maxUserLength, strlen(file->user));
		writeMax(&list->maxGroupLength, strlen(file->group));
		if(file->permissions[0] != 'l') {
			list->totalFilesSize += getBlockSize(file->size);
		}
	}
	int i = list->size - 2;
	while(i >= 0 && strcmp(list->files[i + 1]->name, list->files[i]->name) < 0) {
		list->files[i + 1] = list->files[i];
		list->files[i] = file;
		i--;
	}	
}

void printLsList(struct lsList* list) {
	if(list->lFlag) {
		printf("total %lu\n", list->totalFilesSize);
		for(int i = 0; i < list->size; i++) {
			struct lsFile* file = list->files[i];
			printf("%s ", file->permissions);
			printSpaces(list->maxNumLinksLength - getNumberLength(file->numlink));
			printf("%lu %s", file->numlink, file->user);
			printSpaces(list->maxUserLength - strlen(file->user));
			printf(" %s", file->group);
			printSpaces(list->maxGroupLength - strlen(file->group) + list->maxSizeLength - getNumberLength(file->size));
			printf(" %lu %s ", file->size, file->mtime);
			printColor(file->color);
			printf("%s%s", file->name, resetColor);
			if(file->permissions[0] == 'l') {
				printf(" -> %s", file->linkTo);
			}
			printf("\n");
		}
	}
	else {
		for(int i = 0; i < list->size; i++) {
			struct lsFile* file = list->files[i];
			printColor(file->color);
			printf("%s%s  ", file->name, resetColor);
		}
		printf("\n");
	}
}

void deleteLsList(struct lsList* list) {
	if(list->files != NULL) {
		for(int i = 0; i < list->size; i++) {
			deleteLsFile(list->files[i]);
		}
		free(list->files);
	}
	free(list);
}
