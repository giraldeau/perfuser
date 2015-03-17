/*
 * test_siginfo.c
 *
 *  Created on: Mar 10, 2015
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libperfuser.h>

FILE *f = NULL;
int once = 1;

static void handler(int nr, siginfo_t *info, void *void_context)
{
	struct perfuser_stats pstats;
	int ret = read(f->_fileno, &pstats, sizeof(pstats));
	if (ret < 0) {
		printf("read failed %s\n", strerror(ret));
		return;
	}
	if (once) {
		printf("%5s %5s %5s %5s %5s %5s %5s %10s\n", "pid", "ret", "nmi", "irq",
			"sig", "blk", "err", "ts");
		once = 0;
	}
	printf("%5d %5d %5d %5d %5d %5d %5d %10lu\n", getpid(), ret, pstats.nmi,
			pstats.irq, pstats.sig, pstats.blk, pstats.err,
			pstats.ts);
}

static int install_handler(void)
{
	struct sigaction act;
	sigset_t mask;
	memset(&act, 0, sizeof(act));
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);

	act.sa_sigaction = &handler;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror("sigaction");
		return -1;
	}
	if (sigprocmask(SIG_UNBLOCK, &mask, NULL)) {
		perror("sigprocmask");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	sigset_t mask;
	struct perfuser_info info = {
		.signum = SIGUSR1,
	};
	install_handler();
	f = fopen(PERFUSER_PATH, "rw");
	if (f == NULL) {
		printf("fopen failed %s\n", strerror(errno));
		return -1;
	}

	info.cmd = PERFUSER_REGISTER;
	ioctl(f->_fileno, PERFUSER_IOCTL, &info);

	printf("ioctl\n");
	info.cmd = PERFUSER_SENDSIG;
	ioctl(f->_fileno, PERFUSER_IOCTL, &info);

	sigaddset(&mask, SIGUSR1);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	ioctl(f->_fileno, PERFUSER_IOCTL, &info);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	printf("done\n");
	return 0;
}

