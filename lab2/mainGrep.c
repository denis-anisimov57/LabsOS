#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

const char redColor[] = "\x1b[31m";
const char resetColor[] = "\x1b[0m";

void highlightPattern(char* line, char* pattern) {
    char *s = line;
	regex_t regex;
    regmatch_t pmatch[1];
    regoff_t off = 0, len = 0;
    regcomp(&regex, pattern, REG_NEWLINE);
    while(regexec(&regex, s, sizeof(pmatch) / sizeof(pmatch[0]), pmatch, 0) == 0) { 
        off = pmatch[0].rm_so + strlen(line) - strlen(s);
        len = pmatch[0].rm_eo - pmatch[0].rm_so;
		if(len == 0) {
			if(s == line) {
				printf("%s", s);
			}
			break;
		}
		char* beforePattern = calloc(1, pmatch[0].rm_so + 1);
		char* foundPattern = calloc(1, len + 1);
		strncpy(beforePattern, s, pmatch[0].rm_so);
		strncpy(foundPattern, s + pmatch[0].rm_so, len);
		printf("%s%s%s%s", beforePattern, redColor, foundPattern, resetColor);
		free(beforePattern);
		free(foundPattern);
		s += pmatch[0].rm_eo;
    }
	if(s != line) {
		printf("%s", s);
	}
	regfree(&regex);
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "No pattern provided\n");
		return -1;
	}
	regex_t regex;
	if (regcomp(&regex, argv[1], REG_NEWLINE)) {
		fprintf(stderr, "Invalid pattern\n");
		regfree(&regex);
		return -1;
	}
	regfree(&regex);
	if(argc == 2) {
		char* line = NULL;
		size_t len = 0;
		while(getline(&line, &len, stdin) != -1) {
			highlightPattern(line, argv[1]);
		}
		if(line != NULL) {
			free(line);
		}
		return 0;
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
	while(getline(&line, &len, fp) != -1) {
		highlightPattern(line, argv[1]);
	}
	if(line != NULL) {
		free(line);
	}
	fclose(fp);
	return 0;
}
