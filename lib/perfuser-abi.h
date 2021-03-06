/*
 * perfuser-abi.h
 *
 *  Created on: Dec 9, 2014
 *      Author: francis
 */

#ifndef PERFUSER_ABI_H_
#define PERFUSER_ABI_H_

#include <linux/types.h>

#define PERFUSER_PROC "perfuser"
#define PERFUSER_PATH "/proc/" PERFUSER_PROC

enum perfuser_cmd {
	PERFUSER_REGISTER = 0,
	PERFUSER_UNREGISTER = 1,
	PERFUSER_STATUS = 2,
	PERFUSER_DEBUG = 3,
	PERFUSER_SENDSIG = 4, /* benchmark purpose */
	PERFUSER_SUSPEND = 5,
	PERFUSER_RESUME = 6,
};

/*
 * Structure to exchange data from and to kernel module.
 */
struct perfuser_info {
	int cmd;
	int signum;
} __attribute__((packed));

/*
 * Forward statistics to user-space.
 */
struct perfuser_stats {
	int nmi;
	int irq;
	int sig;
	int blk;
	int err;
	unsigned long ts;
} __attribute__((packed));

/* Borrow some unused range of LTTng ioctl ;-) */
#define PERFUSER_IOCTL 		_IO(0xF6, 0x90)

#endif /* PERFUSER_ABI_H_ */
