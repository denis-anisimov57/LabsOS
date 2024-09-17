#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

void printSpaces(unsigned number) {
	for(int i = 0; i < number; i++) {
		printf(" ");
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

int main(int argc, char** argv) {
	char c;
	bool nFlag = false, bFlag = false, EFlag = false;
	while((c = getopt(argc, argv, "nbE")) != -1) {
		switch(c) {
			case 'n': {
				nFlag = true;
				break;
			}
			case 'b':
				bFlag = true;
				break;
			case 'E':
				EFlag = true;
				break;
			default:
				fprintf(stderr, "Unknown op\n");
				break;
		}
	}
	char* filename;
	if(optind >= argc) {
		char* line = NULL;
		size_t len = 0;
		while(true) {
			getline(&line, &len, stdin);
			printf("%s", line);
		}
		return 0;
	}
	else {
		filename = argv[optind];
	}
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "Can't open file\n");
		return -1;
	}
	char* line = NULL;
	size_t len = 0;
	unsigned lineCount = 0;
	while(getline(&line, &len, fp) != -1) {
		lineCount++;
	}
	rewind(fp);
	unsigned lineNumberLength = getNumberLength(lineCount);
	unsigned lineNumber = 0;
	ssize_t read = 0;
	while((read = getline(&line, &len, fp)) != -1) {
		if(bFlag) {
			if(read != 1) {
				printf("    ");
				lineNumber++;
				printSpaces(lineNumberLength - getNumberLength(lineNumber));
				printf("%u ", lineNumber);
			}
		}
		else if(nFlag){
			lineNumber++;
			printf("    ");
			printSpaces(lineNumberLength - getNumberLength(lineNumber));
			printf("%u ", lineNumber);
		}
		if(EFlag) {
			line[read - 1] = '$';
			printf("%s\n", line);
		}
		else {
			printf("%s", line);
		}
	}
	if(line != NULL) {
		free(line);
	}
	fclose(fp);
	return 0;
}
