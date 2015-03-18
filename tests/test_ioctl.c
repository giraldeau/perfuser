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
#include <perfuser-abi.h>

#include <libperfuser.h>
#include <sys/syscall.h>
#include <unistd.h>

#define PAGE_SIZE 4096
static int th = 4;
static int count[4];

static __thread int id;
static int rank;
static int suspend_insig = 0;
static int suspend_even = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_sample(void *arg) {
	if (suspend_insig)
		perfuser_suspend();
	count[id]++;
	if (suspend_insig)
		perfuser_resume();
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

	printf("%20s %ld pid=%d tid=%d\n", "self", pthread_self(), getpid(), (int) syscall(SYS_gettid));
	for (i = 0; i < nb; i++) {
		if (id % 2 && suspend_even)
			perfuser_suspend();
		char *buf = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		memset(buf, 0x42, PAGE_SIZE);
		munmap(buf, PAGE_SIZE);
		madvise(buf, PAGE_SIZE, MADV_DONTNEED);
		if (id % 2 && suspend_even)
			perfuser_resume();
	}

	return NULL;
}

int main(int argc, char **argv)
{
	int i, ret;
	int nb = 10;
	pthread_t pth[th];

	/*
	 * FIXME: at some point, we will need opts
	 */
	if (argc >= 2)
		suspend_insig = atoi(argv[1]);
	if (argc >= 3)
		suspend_even = atoi(argv[2]);

	printf("test_ioctl begin suspend_insig=%d suspend_even=%d\n", suspend_insig, suspend_even);
	ret = perfuser_register(&conf);
	struct perfuser_info info = { .cmd = PERFUSER_DEBUG };
	ioctl(perfuser_get_fd(), PERFUSER_IOCTL, &info);
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
