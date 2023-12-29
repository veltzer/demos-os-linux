// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <linux/moduleparam.h> /* for module_param, MODULE_PARM_DESC */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for class_create */
#include <linux/slab.h> /* for kmalloc, kfree */
#include <linux/cdev.h> /* for cdev_* */
#include <linux/sched.h> /* for TASK_INTERRUPTIBLE and more constants */
#include <linux/spinlock.h> /* for spinlock_t and ops on it */
#include <linux/wait.h> /* for wait_queue_head_t and ops on it */
#include <linux/uaccess.h> /* for copy_to_user, access_ok */
#include <linux/mutex.h> /* for mutex_* */
#include <linux/version.h> /* for LINUX_VERSION_CODE, KERNEL_VERSION */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A named pipe exercise");
MODULE_VERSION("1.12.43b");

/* yes, actually activate copy_to_user, copy_from_user */
#define DO_COPY
/* should we use spinlocks to synchronize readers and writes */
/* #define DO_SPINLOCK */
/* should we use mutexes to synchronize readers and writes */
#define DO_MUTEX
/* do we want to use the waitqueue API? */
#define DO_WAITQUEUE
/* #define DO_WAITQUEUE_RISQUE */
/* do we want to wakeup processes at the appropriate time? */
#define DO_WAKEUP
/* #define DO_SCHEDULE */
/* #define DO_NOTHING */

/*
 * See exercise.txt for the description of this exercise.
 *
 * Notes:
 * - we protect the pipe here using a mutex. A spinlock option is also
 * present here but it is NOT good. We call 'copy_to_user' and
 * 'copy_from_user' while holding these functions may sleep so strictly
 * speaking this is prohibited. It will work on lots of kernels but has
 * failed for me on a PREEMPT kernel.
 * - Even though the pipe size is PAGE_SIZE (4096 on a i386) we can only
 * store PAGE_SIZE-1 bytes in the pipe. This is because if we stored 4096
 * bytes in the pipe we would not be able to distinguish between a full pipe
 * and an empty one. This is the reason for the weird -1 in the 'pipe_room'
 * function.
 * - the performace of this pipe as can be ascertained using the 'pipemeter'
 * is much lower than the kernels own pipe as can be checked
 * via 'cat /dev/zero | pipemeter > /dev/null' (I prepared a script for this).
 * - there is no need to allocated the memory for the pipe using kzalloc.
 * kmalloc is good enough assuming that we have no bugs in the driver and do
 * not supply unwritten data to user space by mistake...:)
 *
 * TODO:
 * - there is a bug in that closing the last writer does NOT make the readers
 * wake up and return EOF. Find the problem and fix it.
 * - explain the difference in performance mentioned above and improve this
 * example to give similar performance.
 * - hold the readers and writers as atomic variables.
 * This will enable us to remove the spinlock protection that we currently have
 * in the 'open' and 'release' fops.
 *
 * TOOLS: pipemeter
 */

static int pipes_count = 8;
module_param(pipes_count, int, 0644);
MODULE_PARM_DESC(pipes_count, "How many pipes to create ?");
/* this size of pipe performs very well! (3.0 G/s) */
/* static int pipe_size = PAGE_SIZE*10; */
/* this one doesnt... */
static int pipe_size = PAGE_SIZE;
module_param(pipe_size, int, 0644);
MODULE_PARM_DESC(pipe_size, "What is the pipe size ?");

/* this is the first dev_t allocated to us... */
static dev_t first_dev;
/* struct for each pipe */
struct my_pipe_t {
	char *data;
	size_t size;
	size_t read_pos;
	size_t write_pos;
	wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;
	#ifdef DO_SPINLOCK
	spinlock_t lock;
	#endif /* DO_SPINLOCK */
	struct device *pipe_device;
	#ifdef DO_MUTEX
	struct mutex mutex;
	#endif /* DO_MUTEX */
	int readers;
	int writers;
};

/* automatically initialised to NULL... */
static struct my_pipe_t *pipes;

/* pipe constructur */
static inline void pipe_ctor(struct my_pipe_t *pipe)
{
	pipe->data = kmalloc(pipe_size, GFP_KERNEL);
	pipe->size = pipe_size;
	pipe->read_pos = 0;
	pipe->write_pos = 0;
#ifdef DO_SPINLOCK
	spin_lock_init(&pipe->lock);
#endif /* DO_SPINLOCK */
#ifdef DO_MUTEX
	mutex_init(&pipe->mutex);
#endif /* DO_MUTEX */
	pipe->pipe_device = NULL;
	init_waitqueue_head(&pipe->read_queue);
	init_waitqueue_head(&pipe->write_queue);
	pipe->readers = 0;
	pipe->writers = 0;
}

/* pipe destructor */
static inline void pipe_dtor(const struct my_pipe_t *pipe)
{
#ifdef DO_SPINLOCK
	/* spin_lock_destroy(&pipe->lock); */
#endif /* DO_SPINLOCK */
	kfree(pipe->data);
}

/* do we have data in the pipe ? */
static inline bool pipe_have_data(const struct my_pipe_t *pipe)
{
	return pipe->read_pos != pipe->write_pos;
}

/* return the empty room of a pipe */
static inline size_t pipe_room(const struct my_pipe_t *pipe)
{
	if (pipe->read_pos <= pipe->write_pos)
		return pipe->size-pipe->write_pos+pipe->read_pos-1;
	else
		return pipe->read_pos-pipe->write_pos-1;
}
/* return the occupied room of a pipe */
static inline size_t pipe_data(const struct my_pipe_t *pipe)
{
	if (pipe->read_pos <= pipe->write_pos)
		return pipe->write_pos-pipe->read_pos;
	else
		return pipe->size-pipe->read_pos+pipe->write_pos;
}

/* lock the pipe spinlock */
static inline void pipe_lock(struct my_pipe_t *pipe)
{
	#ifdef DO_SPINLOCK
	spin_lock(&pipe->lock);
	#endif /* DO_SPINLOCK */
	#ifdef DO_MUTEX
	mutex_lock(&pipe->mutex);
	#endif /* DO_MUTEX */
}

/* unlock the pipe spinlock */
static inline void pipe_unlock(struct my_pipe_t *pipe)
{
	#ifdef DO_SPINLOCK
	spin_unlock(&pipe->lock);
	#endif /* DO_SPINLOCK */
	#ifdef DO_MUTEX
	mutex_unlock(&pipe->mutex);
	#endif /* DO_MUTEX */
}

static inline int pipe_reader_should_i_wake_up(struct my_pipe_t *pipe)
{
	int ret;

	pipe_lock(pipe);
	ret = pipe_data(pipe) > 0 || pipe->writers == 0;
	if (!ret)
		pipe_unlock(pipe);
	return ret;
}

/* wait on the pipes readers queue */
static inline int pipe_wait_read(struct my_pipe_t *pipe)
{
	#ifdef DO_WAITQUEUE
	pipe_unlock(pipe);
	return wait_event_interruptible(pipe->read_queue,
			pipe_reader_should_i_wake_up(pipe));
	#endif /* DO_WAITQUEUE */
	#ifdef DO_WAITQUEUE_RISQUE
	int ret;

	pipe_unlock(pipe);
	ret = wait_event_interruptible(pipe->read_queue,
			pipe_data(pipe) > 0 || pipe->writers == 0);
	pipe_lock(pipe);
	return ret;
	#endif /* DO_WAITQUEUE_RISQUE */
	#ifdef DO_SCHEDULE
	int ret;
	DEFINE_WAIT(wait);

	prepare_to_wait(&pipe->read_queue, &wait, TASK_INTERRUPTIBLE);
	pipe_unlock(pipe);
	schedule();
	if (signal_pending(current))
		ret = -ERESTARTSYS;
	else
		ret = 0;
	finish_wait(&pipe->read_queue, &wait);
	pipe_lock(pipe);
	return ret;
	#endif /* DO_SHCEDULE */
	#ifdef DO_NOTHING
	return 0;
	#endif /* DO_NOTHING */
}

static inline int pipe_writer_should_i_wake_up(struct my_pipe_t *pipe)
{
	int ret;

	pipe_lock(pipe);
	ret = pipe_room(pipe) > 0;
	if (!ret)
		pipe_unlock(pipe);
	return ret;
}

/* wait on the pipes writers queue */
static inline int pipe_wait_write(struct my_pipe_t *pipe)
{
	#ifdef DO_WAITQUEUE
	pipe_unlock(pipe);
	return wait_event_interruptible(pipe->write_queue,
			pipe_writer_should_i_wake_up(pipe));
	#endif /* DO_WAITQUEUE */
	#ifdef DO_WAITQUEUE_RISQUE
	int ret;

	pipe_unlock(pipe);
	ret = wait_event_interruptible(pipe->write_queue, pipe_room(pipe) > 0);
	pipe_lock(pipe);
	return ret;
	#endif /* DO_WAITQUEUE_RISQUE */
	#ifdef DO_SCHEDULE
	int ret;
	DEFINE_WAIT(wait);

	prepare_to_wait(&pipe->write_queue, &wait, TASK_INTERRUPTIBLE);
	pipe_unlock(pipe);
	schedule();
	if (signal_pending(current))
		ret = -ERESTARTSYS;
	else
		ret = 0;
	finish_wait(&pipe->write_queue, &wait);
	pipe_lock(pipe);
	return ret;
	#endif /* DO_SHCEDULE */
	#ifdef DO_NOTHING
	return 0;
	#endif /* DO_NOTHING */
}

/* wake up the readers */
static inline void pipe_wake_readers(struct my_pipe_t *pipe)
{
	#ifdef DO_WAKEUP
	wake_up_all(&pipe->read_queue);
	#endif /* DO_WAKEUP */
}

/* wake up the writers */
static inline void pipe_wake_writers(struct my_pipe_t *pipe)
{
	#ifdef DO_WAKEUP
	wake_up_all(&pipe->write_queue);
	#endif /* DO_WAKEUP */
}

/* read into the pipe */
static inline int pipe_copy_from_user(struct my_pipe_t *pipe, int count,
		const char __user **ubuf) {
	int ret;

	pr_debug("copy_from_user: count is %d, read_pos is %zd, write_pos is %zd, size is %zd\n",
			count, pipe->read_pos, pipe->write_pos, pipe->size);
	#ifdef DO_COPY
	ret = copy_from_user(pipe->data+pipe->write_pos, *ubuf, count);
	#else /* DO_COPY */
	ret = 0;
	#endif /* DO_COPY */
	if (ret == 0) {
		pr_debug("advancing buffer\n");
		*ubuf += count;
		pipe->write_pos += count;
		BUG_ON(pipe->write_pos > pipe->size);
		if (pipe->write_pos == pipe->size)
			pipe->write_pos = 0;
	} else {
		pr_err("error on copy_from_user, count is %d, return is %d\n",
				count, ret);
	}
	return ret;
}

/* read from the pipe */
static inline int pipe_copy_to_user(struct my_pipe_t *pipe, int count,
		char __user **ubuf)
{
	unsigned long ret;

	pr_debug("copy_to_user: count is %d, read_pos is %zd, write_pos is %zd, size is %zd\n",
			count, pipe->read_pos, pipe->write_pos, pipe->size);
	#ifdef DO_COPY
	pr_debug("copying to %p\n", *ubuf);
	ret = copy_to_user(*ubuf, pipe->data+pipe->read_pos, count);
	#else /* DO_COPY */
	ret = 0;
	#endif /* DO_COPY */
	if (ret == 0) {
		pr_debug("advancing buffer\n");
		*ubuf += count;
		pipe->read_pos += count;
		BUG_ON(pipe->read_pos > pipe->size);
		if (pipe->read_pos == pipe->size)
			pipe->read_pos = 0;
	} else {
		pr_err("error on copy_to_user, count is %d, return is %lu\n",
				count, ret);
	}
	return ret;
}

/* these are the actual operations */

static int pipe_open(struct inode *inode, struct file *filp)
{
	/* hide the pipe in the private_data of the struct file... */
	int the_pipe_number = iminor(inode)-MINOR(first_dev);
	struct my_pipe_t *pipe = pipes+the_pipe_number;

	filp->private_data = pipe;
	pipe_lock(pipe);
	if (filp->f_mode & FMODE_READ)
		pipe->readers++;
	if (filp->f_mode & FMODE_WRITE)
		pipe->writers++;
	pipe_unlock(pipe);
	return 0;
}

static int pipe_release(struct inode *inode, struct file *filp)
{
	struct my_pipe_t *pipe;

	pipe = (struct my_pipe_t *)(filp->private_data);
	pipe_lock(pipe);
	if (filp->f_mode & FMODE_READ)
		pipe->readers--;
	if (filp->f_mode & FMODE_WRITE)
		pipe->writers--;
	/*
	 * wake up readers since they may want to end if there
	 * are no more writers...
	 */
	if (filp->f_mode & FMODE_WRITE) {
		if (pipe->writers == 0) {
			pr_debug("%s: no more writers, waking up readers...\n", __func__);
			pipe_wake_readers(pipe);
		}
	}
	pipe_unlock(pipe);
	return 0;
}

static ssize_t pipe_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct my_pipe_t *pipe;
	size_t data, work_size, first_chunk, second_chunk, ret;

	pr_debug("%s: start with buf %p\n", __func__, buf);
	if (!access_ok(buf, count))
		return -EFAULT;
	pipe = (struct my_pipe_t *)(file->private_data);
	/*
	 * lets sleep while there is no data in the pipe
	 * why do we not just use the waitqueue condition?
	 * because we want to get the pipe LOCKED with data
	 */
	pipe_lock(pipe);
	data = pipe_data(pipe);
	while (data == 0 && pipe->writers > 0) {
		ret = pipe_wait_read(pipe);
		if (ret) {
			pipe_unlock(pipe);
			return ret;
		}
		data = pipe_data(pipe);
	}
	pr_debug("data is %zd\n", data);
	/* EOF handling */
	if (data == 0 && pipe->writers == 0) {
		pr_debug("signaling EOF\n");
		pipe_unlock(pipe);
		return 0;
	}
	/* now data > 0 */
	work_size = min(data, count);
	pr_debug("work_size is %zd\n", work_size);
	/* copy_to_user data from the pipe */
	if (pipe->read_pos <= pipe->write_pos) {
		ret = pipe_copy_to_user(pipe, work_size, &buf);
		if (ret) {
			pipe_unlock(pipe);
			return ret;
		}
	} else {
		first_chunk = min(work_size, pipe->size-pipe->read_pos);
		ret = pipe_copy_to_user(pipe, first_chunk, &buf);
		if (ret) {
			pipe_unlock(pipe);
			return ret;
		}
		if (first_chunk < work_size) {
			second_chunk = work_size-first_chunk;
			ret = pipe_copy_to_user(pipe, second_chunk, &buf);
			if (ret) {
				pipe_unlock(pipe);
				return ret;
			}
		}
	}
	*ppos += work_size;
	pipe_unlock(pipe);
	/* wake up the writers */
	pipe_wake_writers(pipe);
	return work_size;
}

static ssize_t pipe_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	struct my_pipe_t *pipe;
	size_t work_size, room, first_chunk, second_chunk, ret;

	if (!access_ok(buf, count))
		return -EFAULT;
	pipe = (struct my_pipe_t *)(file->private_data);
	/* lets check if we have room in the pipe
	 * why do we not just use the waitqueue condition?
	 * because we want to get the pipe LOCKED with data
	 */
	pipe_lock(pipe);
	room = pipe_room(pipe);
	while (room == 0) {
		ret = pipe_wait_write(pipe);
		if (ret) {
			pipe_unlock(pipe);
			return ret;
		}
		room = pipe_room(pipe);
	}
	pr_debug("room is %zd\n", room);
	/* now room > 0 */
	work_size = min(room, count);
	pr_debug("work_size is %zd\n", work_size);
	/* copy_from_user data from the pipe */
	if (pipe->read_pos <= pipe->write_pos) {
		first_chunk = min(work_size, pipe->size-pipe->write_pos);
		ret = pipe_copy_from_user(pipe, first_chunk, &buf);
		if (ret) {
			pipe_unlock(pipe);
			return ret;
		}
		if (first_chunk < work_size) {
			second_chunk = work_size-first_chunk;
			ret = pipe_copy_from_user(pipe, second_chunk, &buf);
			if (ret) {
				pipe_unlock(pipe);
				return ret;
			}
		}
	} else {
		ret = pipe_copy_from_user(pipe, work_size, &buf);
		if (ret) {
			pipe_unlock(pipe);
			return ret;
		}
	}
	*ppos += work_size;
	pipe_unlock(pipe);
	/* wake up the readers */
	pipe_wake_readers(pipe);
	return work_size;
}

/* this is the operations table */
static const struct file_operations pipe_fops = {
	.owner = THIS_MODULE,
	.open = pipe_open,
	.release = pipe_release,
	.read = pipe_read,
	.write = pipe_write,
};

/* this variable will store the class */
static struct class *my_class;
/* this variable will hold our cdev struct */
static struct cdev cdev;
/* this is our first minor (0 by default)*/
static int first_minor;

static int __init pipe_init(void)
{
	int ret;
	int i;
	/* allocate all pipes */
	pipes = kmalloc_array(pipes_count, sizeof(struct my_pipe_t), GFP_KERNEL);
	if (IS_ERR(pipes)) {
		pr_err("kmalloc\n");
		ret = PTR_ERR(pipes);
		goto err_return;
	}
	/* initialize all pipes */
	for (i = 0; i < pipes_count; i++)
		pipe_ctor(pipes+i);
	/* allocate our own range of devices */
	ret = alloc_chrdev_region(&first_dev, first_minor, pipes_count,
			THIS_MODULE->name);
	if (ret) {
		pr_err("cannot alloc_chrdev_region\n");
		goto err_final;
	}
	pr_debug("allocated the region\n");
	/* add the cdev structure */
	cdev_init(&cdev, &pipe_fops);
	ret = cdev_add(&cdev, first_dev, pipes_count);
	if (ret) {
		pr_err("cannot cdev_add\n");
		goto err_dealloc;
	}
	pr_debug("added the cdev\n");
	/* this is creating a new class (/proc/devices) */
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 5, 0)
	my_class = class_create(THIS_MODULE, THIS_MODULE->name);
#else
	my_class = class_create(THIS_MODULE->name);
#endif
	if (IS_ERR(my_class)) {
		pr_err("class_create\n");
		ret = PTR_ERR(my_class);
		goto err_cdev_del;
	}
	pr_debug("created the class\n");
	for (i = 0; i < pipes_count; i++) {
		/* and now lets auto-create a /dev/ node */
		pipes[i].pipe_device = device_create(my_class, NULL,
			MKDEV(MAJOR(first_dev), MINOR(first_dev)+i),
			NULL, "%s%d", THIS_MODULE->name, i);
		if (IS_ERR(pipes[i].pipe_device)) {
			pr_err("device_create\n");
			ret = PTR_ERR(pipes[i].pipe_device);
			goto err_class;
		}
	}
	pr_info(KBUILD_MODNAME " loaded successfully\n");
	return 0;
/*
 * err_device:
 *	for (i = 0; i < pipes_count; i++) {
 *		device_destroy(my_class, MKDEV(MAJOR(first_dev),
 *			MINOR(first_dev)+i));
 *	}
 */
err_class:
	class_destroy(my_class);
err_cdev_del:
	cdev_del(&cdev);
err_dealloc:
	unregister_chrdev_region(first_dev, pipes_count);
err_final:
	for (i = 0; i < pipes_count; i++)
		pipe_dtor(pipes+i);
	kfree(pipes);
err_return:
	return ret;
}

static void __exit pipe_exit(void)
{
	int i;

	for (i = 0; i < pipes_count; i++)
		device_destroy(my_class, MKDEV(MAJOR(first_dev),
			MINOR(first_dev)+i));
	class_destroy(my_class);
	cdev_del(&cdev);
	unregister_chrdev_region(first_dev, pipes_count);
	for (i = 0; i < pipes_count; i++)
		pipe_dtor(pipes+i);
	kfree(pipes);
	pr_info(KBUILD_MODNAME " unloaded successfully\n");
}

module_init(pipe_init);
module_exit(pipe_exit);
