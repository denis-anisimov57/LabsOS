#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ARR_SIZE 10
#define THREADS_NUM 10

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int count = 0;
int mas[ARR_SIZE];
int strSize = 64;
int numSize = 4;

void* thread_read(void* arg) {
	(void)arg;
	char* outStr = (char*)calloc(1, strSize);
	char* strNum = (char*)calloc(1, numSize);
	while(count < ARR_SIZE) {
		usleep(10000);
		pthread_rwlock_rdlock(&rwlock);
		sprintf(outStr, "%li: ", pthread_self());
		for(int i = 0; i < ARR_SIZE; i++) {
			sprintf(strNum, "%d ", mas[i]);
			strcat(outStr, strNum);
		}
		strcat(outStr, "\n");
		printf("%s", outStr);
		pthread_rwlock_unlock(&rwlock);
	}
	free(outStr);
	free(strNum);
	pthread_exit(NULL);
}

void* thread_write(void* arg) {
	(void)arg;
	for(int i = 0; i < ARR_SIZE; i++) {
		pthread_rwlock_wrlock(&rwlock);
		usleep(100000);
		printf("---Writing---\n");	
		count = i + 1;
		mas[i] = count;
		pthread_rwlock_unlock(&rwlock);
	}
	pthread_exit(NULL);
}

int main() {
	pthread_t threads[THREADS_NUM];
	pthread_t twrite;
	pthread_create(&twrite, NULL, thread_write, NULL);
	for(int i = 0; i < THREADS_NUM; i++) {
		int create_res = pthread_create(&threads[i], NULL, thread_read, NULL);
		if(create_res != 0) {
			int err = errno;
			printf("Thread create error: %s(%d)\n", strerror(err), err);
		}
	}
	pthread_join(twrite, NULL);
	for(int i = 0; i < THREADS_NUM; i++) {
		void* res = NULL;
		int join_res = pthread_join(threads[i], &res);
		if(join_res != 0) {
			int err = errno;
			printf("Thread join error: %s(%d)\n", strerror(err), err);		
		}
	}
	return 0;
}
