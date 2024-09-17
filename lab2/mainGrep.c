#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char redColor[] = "\x1b[31m";
const char resetColor[] = "\x1b[0m";

size_t getPatternCount(char* str, char* pattern) {
	size_t count = 0;
	char* curPtr = strstr(str, pattern);
	while(curPtr != NULL) {
		count++;
		curPtr = strstr(curPtr + 1, pattern);
	}
	return count;
}

void highlightPattern(char* line, char* pattern) {
	char* patternPtr = strstr(line, pattern);
	size_t prevN = 0;
	size_t patternCount = 0;
	char* resStr = NULL;
	if(patternPtr != NULL) {
		patternCount = getPatternCount(line, pattern);
		resStr = calloc(1, strlen(line) + patternCount * (sizeof(redColor) + sizeof(resetColor)));
	}
	while(patternPtr != NULL) {
		size_t n = strlen(line) - strlen(patternPtr);
		strncat(resStr, line + prevN, n - prevN);
		strcat(resStr, redColor);
		strcat(resStr, pattern);
		strcat(resStr, resetColor);
		patternPtr = strstr(patternPtr + 1, pattern);
		prevN = n + strlen(pattern);
	}
	if(resStr != NULL) {
		strcat(resStr, line + prevN);
		printf("%s", resStr);
		free(resStr);
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "No pattern provided\n");
		return -1;
	}
	char* pattern = argv[1];
	if(argc == 2) {
		char* line = NULL;
		size_t len = 0;
		while(true) {
			getline(&line, &len, stdin);
			highlightPattern(line, pattern);
		}
	
	}
	char* filename = argv[2];
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "Can't open file\n");
		return -1;
	}
	char* line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	while((read = getline(&line, &len, fp)) != -1) {
		highlightPattern(line, pattern);
	}	
	if(line != NULL) {
		free(line);
	}
	fclose(fp);
	return 0;
}
