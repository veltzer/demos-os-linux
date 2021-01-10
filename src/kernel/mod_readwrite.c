// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_* */
#include <linux/fcntl.h> /* for O_ flags */
#include <linux/fs.h> /* for vfs_write, vfs_read */
#include <linux/uaccess.h> /* get_fs */
#include <linux/sched.h> /* current */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("module that demos how to read and write files inside the kernel");

static int __init read_file(char *filename)
{
	struct file *filp;
	char buf[1];
	loff_t pos;
	int err;

	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);

	filp = filp_open(filename, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		PR_ERROR("could not read file %s", filename);
		return PTR_ERR(filp);
	}
	/* PR_DEBUG("debug message"); */
	pos = 0;
	while (vfs_read(filp, buf, 1, &pos) == 1)
		pr_info("%c", buf[0]);
	pr_info("\n");
	err = filp_close(filp, current->files);
	if (err) {
		PR_ERROR("could not close file %s", filename);
		return err;
	}
	set_fs(old_fs);
	return 0;
}

static int __init write_file(char *filename, char *data)
{
	struct file *filp;
	loff_t pos = 0;
	unsigned int len;

	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);

	filp = filp_open(filename, O_WRONLY | O_CREAT, 0644);
	if (IS_ERR(filp)) {
		PR_ERROR("cannot open file %s for writing", filename);
		return -EFAULT;
	}
	len = strlen(data);
	if (vfs_write(filp, data, len, &pos) != len) {
		PR_ERROR("could not write");
		return -EFAULT;
	}
	if (filp_close(filp, current->files)) {
		PR_ERROR("cannot close file %s after writing", filename);
		return -EFAULT;
	}
	set_fs(old_fs);
	return 0;
}

static int __init mod_init(void)
{
	if (read_file("/etc/shadow")) {
		PR_ERROR("unable to read file");
		return -EFAULT;
	}
	if (write_file("/tmp/test", "This is a line.\n")) {
		PR_ERROR("unable to read file");
		return -EFAULT;
	}
	return 0;
}

static void __exit mod_exit(void)
{
}

module_init(mod_init);
module_exit(mod_exit);
