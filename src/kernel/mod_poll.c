// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for device support */
#include <linux/slab.h> /* for kmalloc, kfree */
#include <linux/sched.h> /* for TASK_INTERRUPTIBLE and more constants */
#include <linux/wait.h> /* for wait_queue_head_t and ops on it */
#include <linux/poll.h> /* for poll_wait, POLLIN */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */
#include "shared.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("device whose file descriptors can participate in select/poll/epoll");

/* static data */
static struct device *my_device;

/* our device structure */
struct polldev {
	wait_queue_head_t wq;
	unsigned long state;
};

/* fops */

/*
 * This is the open implementation
 */
static int kern_open(struct inode *inode, struct file *filp)
{
	struct polldev *pd;

	PR_INFO("start");
	pd = kmalloc(sizeof(struct polldev), GFP_KERNEL);
	if (IS_ERR(pd))
		return PTR_ERR(pd);
	init_waitqueue_head(&pd->wq);
	pd->state = 0;
	filp->private_data = pd;
	return 0;
}

/*
 * This is the release implementation
 */
static int kern_release(struct inode *inode, struct file *filp)
{
	struct polldev *pd;

	PR_INFO("start");
	pd = (struct polldev *)(filp->private_data);
	kfree(pd);
	return 0;
}


/*
 * This is the ioctl implementation.
 */
static long kern_unlocked_ioctl(struct file *fp, unsigned int cmd,
		unsigned long arg)
{
	struct polldev *pd;

	pd = (struct polldev *)fp->private_data;
	PR_INFO("start %p", pd);
	switch (cmd) {
	case IOCTL_EPOLL_WAKE:
		PR_INFO("in WAKE");
		pd->state = POLLIN;
		wmb(); /* comment */
		wake_up_all(&pd->wq);
		return 0;
	case IOCTL_EPOLL_RESET:
		PR_INFO("in RESET");
		pd->state = 0;
		wmb(); /* comment */
		wake_up_all(&pd->wq);
		return 0;
	}
	return -ENOTTY;
}
/*
 * This is the poll implementation
 */
static unsigned int kern_poll(struct file *fp, poll_table *wait)
{
	struct polldev *pd;

	pd = (struct polldev *)fp->private_data;
	PR_INFO("start %p", pd);
	poll_wait(fp, &pd->wq, wait);
	unsigned int mask = pd->state;
	/* no need to reset the state
	 * pd->state = 0;
	 */
	PR_INFO("return with %u", mask);
	return mask;
}

/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = kern_open,
	.release = kern_release,
	.unlocked_ioctl = kern_unlocked_ioctl,
	.poll = kern_poll,
};

#include "device.inc"
