/*
 * libperfuser.c
 *
 *  Created on: Dec 9, 2014
 *      Author: francis
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include "memory.h"
#include "libperfuser.h"
#include "perfuser-abi.h"

struct perfuser_state {
	int registered;
	FILE *fd;
	struct sigaction sigact;
	struct sigaction sigact_old;
	struct perfuser pu;
};

static struct perfuser_state *state = NULL;

/*
 * signal handler
 */
static void perfuser_sigaction(int signum, siginfo_t *info, void *arg)
{
	if (!perfuser_registered())
		return;
	state->pu.sample(state->pu.data);
}

static int perfuser_ioctl(struct perfuser_state *state, int cmd)
{
	struct perfuser_info info;

	if (!(state && state->fd))
		return -1;
	info.cmd = cmd;
	info.signum = state->pu.signum;
	return ioctl(state->fd->_fileno, PERFUSER_IOCTL, &info);
}

/*
 * API functions
 */

int perfuser_registered()
{
	return (state && state->registered);
}

int perfuser_register(struct perfuser *pu)
{
	int ret = 0;

	/* if initialized, then reset configuration */
	if (perfuser_registered()) {
		perfuser_unregister();
	}
	state = calloc(1, sizeof(*state));
	if (!state) {
		return -ENOMEM;
	}
	state->fd = fopen(PERFUSER_PATH, "rw");
	if (!state->fd) {
		ret = -ENOENT;
		goto error_fd;
	}
	state->pu = *pu;

	/* install signal handler before registration */
	state->sigact.sa_sigaction = perfuser_sigaction;
	state->sigact.sa_flags = SA_SIGINFO;
	ret = sigaction(state->pu.signum, &state->sigact, &state->sigact_old);
	if (ret != 0)
		goto error_sig;
	ret = perfuser_ioctl(state, PERFUSER_REGISTER);
	if (ret != 0)
		goto error_ioctl;
	state->registered = 1;
	return ret;

error_ioctl:
	sigaction(state->pu.signum, &state->sigact_old, NULL);
error_sig:
	fclose(state->fd);
error_fd:
	FREE(state);
	return ret;
}

int perfuser_unregister()
{
	int ret = 0;
	if (perfuser_registered()) {
		ret = perfuser_ioctl(state, PERFUSER_UNREGISTER);
		sigaction(state->pu.signum, &state->sigact_old, NULL);
		fclose(state->fd);
		FREE(state);
	}
	return ret;
}

int perfuser_get_fd()
{
	return perfuser_registered() ? state->fd->_fileno : -1;
}
