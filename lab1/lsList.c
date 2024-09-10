#include "lsList.h"

unsigned getNumberLength(int number) {
	unsigned length = 0;
	while(number != 0) {
		length++;
		number /= 10;
	}
	return length;
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
}

void printSpaces(unsigned number) {
	for(int i = 0; i < number; i++) {
		printf(" ");
	}
}

void printLsList(struct lsList* list) {
	for(int i = 0; i < list->size; i++) {
		struct lsFile* file = list->files[i];
		//printf("%s %lu %s %s %lu %s %s\n", file->permissions, file->numlink, file->user, file->group, file->size, file->mtime, file->name);
		printf("%s ", file->permissions);
		printSpaces(list->maxNumLinksLength - getNumberLength(file->numlink));
		printf("%lu", file->numlink);
		printf(" %s", file->user);
		printSpaces(list->maxUserLength - strlen(file->user));
		printf(" %s", file->group);
		printSpaces(list->maxGroupLength - strlen(file->group));
		printSpaces(list->maxSizeLength - getNumberLength(file->size));
		printf(" %lu", file->size);
		printf(" %s %s\n", file->mtime, file->name);
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
