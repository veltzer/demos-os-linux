// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * clipboard.c
 * Originally from Alessandro Rubini Linux Device Driver book.
 * Much messed with by Gilad Ben-Yossef.
 * Fixed up by Mark Veltzer for newer kernels and added features.
 */
#include <linux/module.h>		/* Module API */
#include <linux/moduleparam.h>		/* Module parameters API */
#include <linux/kernel.h>		/* printk() */
#include <linux/slab.h>			/* Memory alloc functions */
#include <linux/fs.h>			/* File operations */
#include <linux/errno.h>			/* Error codes */
#include <linux/interrupt.h>		/* Interrupts */
#include <linux/proc_fs.h>		/* Proc file */
#include <linux/cdev.h>			/* Character device */
#include <linux/sched.h>			/* TASK_* definitions */
#include <linux/uaccess.h>		/* User space access */
#include <linux/proc_fs.h>		/* for create_proc_entry, remove_proc_entry */

MODULE_AUTHOR("Mark Veltzer");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A proc/interrupt/clipboard example");

/*
 * TODO:
 * We have only one wait queue which is bad if we have multiple clipboards.
 * We also have just one in-kernel buffer which is shared by all clipboard
 * /dev/ devices.
 * store the clipboard itself in the device itself.
 * work with an ioctl that can trigger an interrupt.
 * write user space demos of how to work with this (do I need this ? won't
 * cat and echo do ?).
 * add makefile targets for insmod/rmmod etc.
 * improve the write method to write as much as it can and return what is
 * written.
 * if two or more readers or writers are involved we will have race conditions.
 * protect the clipboard or allow just one reader/writer (but even then some
 * protection would be required).
 */

static dev_t clipboard_dev;
static struct cdev *clipboard_cdev;
/* automatically initialised to NULL... */
static char *clipboard;
/* automatically initialised to 0... */
static unsigned int flag;
/* automatically initialised to 0... */
static unsigned int debug_param;
static const unsigned int CLIPBOARD_SIZE = 4096;
/* how many clipboards do we have ? */
static const unsigned int CLIPBOARD_COUNT = 1;
/*
 * this is the interrupt we register to. How did we get the number?
 * 'watch /proc/interrupts' helps.
 */
static const unsigned int MOUSE_INT = 12;
/* the /proc entry with which you can view the size of the clipboard. */
static const char *proc_filename = "driver/clipboard";
/*
 * the wait queue we will use to put processes to sleep on and wake
 * them up...
 */
DECLARE_WAIT_QUEUE_HEAD(clipboard_read_wq);
DECLARE_WAIT_QUEUE_HEAD(clipboard_write_wq);

/* module paramters */
module_param(debug_param, uint, 0700);

/*
 * Open the device. Optional.
 */
static int clipboard_open(struct inode *inode, struct file *filp)
{
	pr_debug("device is open\n");
	return 0;
}

/*
 * Release the device. Optional as well.
 */
static int clipboard_release(struct inode *inode, struct file *filp)
{
	pr_debug("device is released\n");
	return 0;
}

/*
 * Read from the clipboard. This does the real work. This method always goes
 * to sleep until woken up by the interrupt handler. When it is woken up it
 * copies the data from the clip board. There is no real locking on the
 * clipboard status which is a problem. Fix this sometime.
 */
static ssize_t clipboard_read(struct file *filp, __user char *user_buf,
		size_t count, loff_t *offset)
{
	int ret;
	int remaining_bytes;
	/* Number of bytes left to read in the open file */
	remaining_bytes = min(CLIPBOARD_SIZE-(*offset), (loff_t)count);
	if (remaining_bytes == 0) {
		/* All read and returning 0 for End Of File */
		return 0;
	}
	/*
	 * lets go to sleep until an intterupt occurs in which case we will
	 * have data
	 */
	flag = 0;
	wait_event_interruptible(clipboard_read_wq, flag);
	ret = copy_to_user(user_buf, *offset+clipboard, remaining_bytes);
	if (ret)
		return ret;
	/* Increase the position in the open file */
	*offset += remaining_bytes;
	wake_up_interruptible(&clipboard_write_wq);
	return remaining_bytes;
}

/*
 * Writing to the clipboard. This simply copied the data from user space into
 * the in-kernel buffer using copy_from_user. If the clipboard is full we do
 * not return 0 since this will cause the user process to think that it had
 * reached end of file. Instead, we put it to sleep and wait to be woken up by
 * a reader that has read from the clipboard. Then we check the size again
 * since the clipboard has chanced. Once we can write any positive amount we
 * do the write and return the number of bytes written to the user. Why don't
 * we loop until we write the entire buffer of the user (count bytes)? Well,
 * we could do it but then we would not comply with the standard UNIX
 * semantics for write that state that we should write what we can NOW and
 * return the number of bytes written.
 *
 * A simpler approach would have been to just see if we have at least count
 * bytes free in the buffer and return an error if that is not the case.
 * This is too simplistic and would break lots of prefectly legal user space
 * programs.
 */
static ssize_t clipboard_write(struct file *filp, const char *user_buf,
		size_t count, loff_t *offset)
{
	int ret;
	int remaining_bytes = min(CLIPBOARD_SIZE-(*offset), (loff_t)count);
	/* clipboard is full, put the process to sleep */
	while (remaining_bytes == 0) {
		wait_event_interruptible(clipboard_write_wq, flag);
		remaining_bytes = min(CLIPBOARD_SIZE-(*offset), (loff_t)count);
	}
	/* now we have room in the clipboard */
	/*
	 * old code - we can write less than count
	 * if (count > remaining_bytes)
	 *	Can't write beyond the end of the device
	 *	return -EIO;
	 * }
	 */
	ret = copy_from_user(*offset+clipboard, user_buf, remaining_bytes);
	if (ret)
		return ret;
	/* Increase the position in the open file */
	*offset += remaining_bytes;
	return remaining_bytes;
}

/*
 * proc file callback
 */
/*
 * static int clipboard_proc_reader(char *page, char **start, off_t off, int count,
 *		int *eof, void *data)
 * {
 *	int len = 0;
 *	len += sprintf(page+len, "Hello world! debug param=%d\n", debug_param);
 *	if (len <= off+count)
 *		*eof = 1;
 *	*start = page+off;
 *	len -= off;
 *	if (len > count)
 *		len = count;
 *	if (len < 0)
 *		len = 0;
 *	return len;
 * }
 */

/*
 * Interrupt callback
 * We say that it was our interrupt but really shouldn't.
 */
static irqreturn_t clipboard_int_handler(int irq, void *dev)
{
	flag = 1;
	mb(); /* comment */
	wake_up_interruptible(&clipboard_read_wq);
	return IRQ_HANDLED;
}

/* our file operations structure that gathers all the ops */
static const struct file_operations clipboard_fops = {
	.owner = THIS_MODULE,
	.open = clipboard_open,
	.release = clipboard_release,
	.read = clipboard_read,
	.write = clipboard_write
};

static const struct file_operations clipboard_proc_ops = {
	.owner = THIS_MODULE,
/*	.read = clipboard_proc_reader, */
};

static struct proc_dir_entry *clipboard_proc_entry;


/*
 * Module housekeeping.
 */
static int clipboard_init(void)
{
	int ret = 0;

	clipboard = kmalloc(CLIPBOARD_SIZE, GFP_KERNEL);
	if (!clipboard) {
		ret = -ENOMEM;
		goto any_error;
	}
	memset(clipboard, 0, CLIPBOARD_SIZE);

	clipboard_proc_entry = proc_create(proc_filename, 0, NULL,
			&clipboard_proc_ops);
	if (IS_ERR(clipboard_proc_entry)) {
		ret = PTR_ERR(clipboard_proc_entry);
		goto error_after_alloc;
	}

	if (alloc_chrdev_region(&clipboard_dev, 0, CLIPBOARD_COUNT,
				"clipboard")) {
		ret = -ENODEV;
		goto error_after_proc;
	}

	clipboard_cdev = cdev_alloc();
	if (!clipboard_cdev) {
		ret = -ENOMEM;
		goto error_after_region;
	}
	clipboard_cdev->ops = &clipboard_fops;
	clipboard_cdev->owner = THIS_MODULE;

	if (cdev_add(clipboard_cdev, clipboard_dev, CLIPBOARD_COUNT)) {
		ret = -ENODEV;
		/*
		 * Only if we allocated a cdev but did not register do we
		 * we need to kfree it. In any other case cdev_del is enough
		 */
		kfree(clipboard_cdev);
		goto error_after_region;
	}

	ret = request_irq(MOUSE_INT, clipboard_int_handler, IRQF_SHARED,
			THIS_MODULE->name, &clipboard);
	if (ret)
		goto error_after_register;

	pr_info("clipboard loaded sucessfuly.\n");
	return 0;

error_after_register:
	cdev_del(clipboard_cdev);
error_after_region:
	unregister_chrdev_region(clipboard_dev, CLIPBOARD_COUNT);
error_after_proc:
	proc_remove(clipboard_proc_entry);
error_after_alloc:
	kfree(clipboard);
any_error:
	return ret;
}

static void clipboard_cleanup(void)
{
	proc_remove(clipboard_proc_entry);
	free_irq(MOUSE_INT, &clipboard);
	cdev_del(clipboard_cdev);
	unregister_chrdev_region(clipboard_dev, CLIPBOARD_COUNT);
	kfree(clipboard);
	pr_info("clipboard unloaded succefully.\n");
}

module_init(clipboard_init);
module_exit(clipboard_cleanup);
