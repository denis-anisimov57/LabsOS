#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ARR_SIZE 10
#define THREADS_NUM 10

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int count = 0;
int mas[ARR_SIZE];

void* thread_read(void* arg) {
	(void)arg;
	while(count < ARR_SIZE) {
		pthread_mutex_lock(&mtx);
		pthread_cond_wait(&cond, &mtx);
		printf("%li: ", pthread_self());
		for(int i = 0; i < ARR_SIZE; i++) {
			printf("%d ", mas[i]);
		}
		printf("\n");
		pthread_mutex_unlock(&mtx);
	}
	pthread_exit(NULL);
}

void* thread_write(void* arg) {
	(void)arg;
	for(int i = 0; i < ARR_SIZE; i++) {
		usleep(100000);
		pthread_mutex_lock(&mtx);
		printf("---Writing---\n");	
		count = i + 1;
		mas[i] = count;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mtx);
	}
	pthread_exit(NULL);
}

int main() {
	pthread_t threads[THREADS_NUM];
	pthread_t twrite;
	for(int i = 0; i < THREADS_NUM; i++) {
		int create_res = pthread_create(&threads[i], NULL, thread_read, NULL);
		if(create_res != 0) {
			int err = errno;
			printf("Thread create error: %s(%d)\n", strerror(err), err);
		}
	}
	pthread_create(&twrite, NULL, thread_write, NULL);
	for(int i = 0; i < THREADS_NUM; i++) {
		void* res = NULL;
		int join_res = pthread_join(threads[i], &res);
		if(join_res != 0) {
			int err = errno;
			printf("Thread join error: %s(%d)\n", strerror(err), err);		
		}
	}
	pthread_join(twrite, NULL);
	return 0;
}
