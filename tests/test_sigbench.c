/*
 * test_sigbench.c
 *
 *  Created on: Feb 13, 2015
 *      Author: francis
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <libperfuser.h>

/* FIXME: pack these time and benchmark function into a library */

struct profile {
	int (*func)(void *args);
	void *args;
	int repeat;
	float mean;
	float sd;
	char *name;
};

/*
 * computes time difference (x - y)
 */
struct timespec time_sub(struct timespec *x, struct timespec *y)
{
    struct timespec res;
    res.tv_sec  = x->tv_sec  - y->tv_sec;
    res.tv_nsec = x->tv_nsec - y->tv_nsec;
    if(x->tv_nsec < y->tv_nsec) {
        res.tv_sec--;
        res.tv_nsec += 1000000000;
    }
    return res;
}

void time_add(struct timespec *x, struct timespec *y)
{
    x->tv_sec  = x->tv_sec + y->tv_sec;
    x->tv_nsec = x->tv_nsec + y->tv_nsec;
    if(x->tv_nsec >= 1000000000) {
        x->tv_sec++;
        x->tv_nsec -= 1000000000;
    }
}

void profile_func(struct profile *prof)
{
	int i;
	struct timespec *data;
	/* allocate memory ahead and writes to it to avoid page fault */
	data = malloc(prof->repeat * sizeof(struct timespec));
	memset(data, 0, prof->repeat * sizeof(struct timespec));

	for (i = 0; i < prof->repeat; i++) {
		clock_gettime(CLOCK_MONOTONIC, &data[i]);
		prof->func(prof->args);
	}

	struct timespec total = { .tv_sec = 0, .tv_nsec = 0 };
	for (i = 0; i < prof->repeat - 1; i++) {
		struct timespec delta = time_sub(&data[i + 1], &data[i]);
		time_add(&total, &delta);
	}
	prof->mean = (double)(total.tv_sec * 1000000000 + total.tv_nsec) / prof->repeat;
	printf("%s mean=%f\n", prof->name, prof->mean);
}


static int count = 0;

void handle_signal(void *arg)
{
	/*
	static char *msg = "signal\n";
	write(1, msg, strlen(msg));
	*/
	count++;
}

static struct perfuser conf = {
	.signum = SIGUSR1,
	.sample = handle_signal,
	.data = NULL,
};

int profile_wrapper_calibrate(void *arg)
{
	return 0;
}

int profile_wrapper_ioctl(void *arg)
{
	struct perfuser_info info = { .cmd = PERFUSER_NONE };
	int fd = perfuser_get_fd();
	return ioctl(fd, PERFUSER_IOCTL, &info);
}

int profile_wrapper_sendsig(void *arg)
{
	struct perfuser_info info = { .cmd = PERFUSER_SENDSIG, .sig = SIGUSR1 };
	int fd = perfuser_get_fd();
	//printf("sendsig\n");
	return ioctl(fd, PERFUSER_IOCTL, &info);
}

int profile_wrapper_kill(void *arg)
{
	//printf("kill\n");
	return kill(0, SIGUSR1);
}

static struct profile prof[] = {
  { .name = "calibrate", .func = profile_wrapper_calibrate, },
  { .name = "ioctl", .func = profile_wrapper_ioctl, },
  { .name = "sendsig", .func = profile_wrapper_sendsig, },
  { .name = "kill", .func = profile_wrapper_kill, },
};

static int nrprof = sizeof(prof) / sizeof(prof[0]);

//#define REPEAT 1000000;
#define REPEAT 1000000;

int main(int argc, char **argv)
{
	int i, ret;

	printf("test_sigbench begin\n");
	ret = perfuser_register(&conf);
	printf("perfuser register ret=%d\n", ret);

	for (i = 0; i < nrprof; i++) {
		prof[i].repeat = REPEAT;
		profile_func(&prof[i]);
	}

	ret = perfuser_unregister();
	printf("perfuser unregister ret=%d\n", ret);

	printf("test_ioctl done\n");
error:
	return 0;
}
