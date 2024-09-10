#include "lsList.h"

char greenColor[] = "\x1b[32m";
char blueColor[] = "\x1b[34m";
char cyanColor[] = "\x1b[36m";
char resetColor[] = "\x1b[0m";

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
	unsigned length = getNumberLength(file->numlink);
	list->maxNumLinksLength = length > list->maxNumLinksLength ? length : list->maxNumLinksLength;
	length = getNumberLength(file->size);
	list->maxSizeLength = length > list->maxSizeLength ? length : list->maxSizeLength;
	length = strlen(file->name);
	list->maxNameLength = length > list->maxNameLength ? length : list->maxNameLength;
	length = strlen(file->user);
	list->maxUserLength = length > list->maxUserLength ? length : list->maxUserLength;
	length = strlen(file->group);
	list->maxGroupLength = length > list->maxGroupLength ? length : list->maxGroupLength;
	int i = list->size - 2;
	while(i >= 0 && strcmp(list->files[i + 1]->name, list->files[i]->name) < 0) {
		list->files[i + 1] = list->files[i];
		list->files[i] = file;
		i--;
	}
	if(file->permissions[0] != 'l') {
		list->totalFilesSize += getBlockSize(file->size);
	}
}

void printSpaces(unsigned number) {
	for(int i = 0; i < number; i++) {
		printf(" ");
	}
}

void printLsList(struct lsList* list) {
	if(list->lFlag) {
		printf("total %lu\n", list->totalFilesSize);
	}
	for(int i = 0; i < list->size; i++) {
		struct lsFile* file = list->files[i];
		//printf("%s %lu %s %s %lu %s %s\n", file->permissions, file->numlink, file->user, file->group, file->size, file->mtime, file->name);
		if(list->lFlag) {
			printf("%s ", file->permissions);
			printSpaces(list->maxNumLinksLength - getNumberLength(file->numlink));
			printf("%lu", file->numlink);
			printf(" %s", file->user);
			printSpaces(list->maxUserLength - strlen(file->user));
			printf(" %s", file->group);
			printSpaces(list->maxGroupLength - strlen(file->group));
			printSpaces(list->maxSizeLength - getNumberLength(file->size));
			printf(" %lu", file->size);
			printf(" %s ", file->mtime);
		}
		switch(file->color) {
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
		printf("%s%s", file->name, resetColor);
		if(!list->lFlag) {
			printf("  ");
		}
		else {
			if(file->permissions[0] == 'l') {
				printf(" -> %s", file->linkTo);
			}
			printf("\n");
		}
	}
	if(!list->lFlag) {
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
