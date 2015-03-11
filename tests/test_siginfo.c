/*
 * test_siginfo.c
 *
 *  Created on: Mar 10, 2015
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libperfuser.h>

static void handler(int nr, siginfo_t *info, void *void_context)
{
	perfuser_siginfo_t *psi = (void *) info;
	printf("psi->_info.si_signo=%d\n", psi->_info.si_signo);
	printf("psi->_info.si_errno=%d\n", psi->_info.si_errno);
	printf("psi->_info.si_code=%d\n", psi->_info.si_code);
	printf("psi->_perf.bidon=%d\n", psi->_perf.bidon);
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

	install_handler();
	FILE *f = fopen(PERFUSER_PATH, "rw");
	if (f == NULL)
		return -1;
	struct perfuser_info info = {
		.cmd = PERFUSER_SENDSIG,
		.signum = SIGUSR1,
	};
	printf("ioctl\n");
	ioctl(f->_fileno, PERFUSER_IOCTL, &info);
	printf("done\n");
	return 0;
}

