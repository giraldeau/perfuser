/*
 * libperfuser.h
 *
 *  Created on: Jan 13, 2015
 *      Author: francis
 */

#ifndef LIBPERFUSER_H_
#define LIBPERFUSER_H_

#include <signal.h>
#include <sys/ioctl.h>
#include <perfuser-abi.h>

/*
 * Perfuser configuration
 */
struct perfuser {
	int signum;					/* signal number   */
	void (*sample)(void *data);	/* sample callback */
	void *data; 				/* custom argument */
};

/*
 * Test if perfuser is enabled.
 *
 * Return: 1 if initialized, 0 otherwise
 */
int perfuser_registered();

/*
 * Register the current process (and all its threads) to perfuser. If already
 * registered, then it resets the configuration. Open the required file
 * descriptor and install the signal handler.
 *
 * The signal handler may be called before this function returns. Therefore,
 * any required setup must be performed prior to registration.
 *
 * Supported signals are SIGUSR1 and SIGUSR2.
 *
 * The registration is process-wide, and signal is sent per-thread.
 *
 * @pu: pointer to struct perfuser configuration
 *
 * Return: 0 in case of success, error code otherwise
 */
int perfuser_register(struct perfuser *pu);

/*
 * Unregister the calling process from perfuser. The previous signal handler
 * is restored.
 *
 * Return: 0 in case of success, error code otherwise
 */
int perfuser_unregister();

/*
 * Return the file descriptor for direct ioctl().
 */
int perfuser_get_fd();

#endif /* LIBPERFUSER_H_ */
