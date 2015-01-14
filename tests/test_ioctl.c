/*
 * test_ioctl.c
 *
 *  Created on: Dec 9, 2014
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <pthread.h>

#include <libperfuser.h>

#define PAGE_SIZE 4096
static int th = 4;
static int count[4];

static __thread int id;
static int rank;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_sample(void *arg) {
	count[id]++;
}

static struct perfuser conf = {
		.signum = SIGUSR1,
		.sample = handle_sample,
		.data = NULL,
};

void *do_work(void *args) {
	int nb = *((int *) args);
	int i, j, ret;

	pthread_mutex_lock(&mutex);
	id = rank++;
	pthread_mutex_unlock(&mutex);

	printf("%20s %ld\n", "self", pthread_self());
	for (i = 0; i < nb; i++) {
		char *buf = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		memset(buf, 0x42, PAGE_SIZE);
		munmap(buf, PAGE_SIZE);
		madvise(buf, PAGE_SIZE, MADV_DONTNEED);
	}

	return NULL;
}

int main(int argc, char **argv)
{
	int i, ret;
	int nb = 1000;
	pthread_t pth[th];

	printf("test_ioctl begin\n");
	ret = perfuser_register(&conf);
	printf("perfuser register ret=%d\n", ret);

	for (i = 0; i < th; i++) {
		pthread_create(&pth[i], NULL, do_work, &nb);
	}
	for (i = 0; i < th; i++) {
		pthread_join(pth[i], NULL);
		printf("count=%d\n", count[i]);
	}

	ret = perfuser_unregister();
	printf("perfuser unregister ret=%d\n", ret);

	printf("test_ioctl done\n");
error:
	return 0;
}
