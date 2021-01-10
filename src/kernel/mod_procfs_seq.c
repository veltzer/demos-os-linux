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

/*
 * procfs4.c - create a "file" in /proc
 * This program uses the seq_file library to manage the /proc file.
 */

/* #define DEBUG */
#include <linux/module.h> /* for the MODULE_* */
#include <linux/printk.h> /* for printk and the pr_* API */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/seq_file.h> /* for seq_file */
#include <linux/slab.h> /* for kmalloc */
/* #define DO_DEBUG */
#include "kernel_helper.h"

#define PROC_NAME KBUILD_MODNAME

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("a module showing how to use the _seq /proc API");

/*
 * This function is called at the beginning of a sequence.
 * ie, when:
 * - the /proc file is read (first time)
 * - after the function stop (end of sequence)
 *
 */
static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	pr_debug("%s %p, %lld\n", __func__, s, *pos);
	/* beginning a new sequence ? */
	if (*pos == 0) {
		/* yes, return a non null value to begin the sequence */
		int *p = kmalloc(sizeof(int), GFP_KERNEL);
		*p = 0;
		return p;
	}
	/*
	 * no, it's the end of the sequence,
	 * return NULL to stop reading
	 */
	return NULL;
}

/*
 * This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	int *p;

	pr_debug("%s %p, %p, %lld\n", __func__, s, v, *pos);
	p = (int *)v;
	if (*p < 10) {
		(*p)++;
		(*pos)++;
		return v;
	} else
		return NULL;
}

/*
 * This function is called at the end of a sequence
 */
static void my_seq_stop(struct seq_file *s, void *v)
{
	pr_debug("%s %p, %p\n", __func__, s, v);
	kfree(v);
}

/*
 * This function is called for each "step" of a sequence
 */
static int my_seq_show(struct seq_file *s, void *v)
{
	int *p;

	pr_debug("%s %p, %p\n", __func__, s, v);
	p = (int *)v;
	seq_printf(s, "%d\n", *p);
	return 0;
}

/*
 * This structure gather "function" to manage the sequence
 */
static const struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next = my_seq_next,
	.stop = my_seq_stop,
	.show = my_seq_show
};

/*
 * This function is called when the /proc file is open.
 */
static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_seq_ops);
};

/*
 * This structure gathers "functions" that manage the /proc file
 */
static const struct file_operations my_file_ops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};
struct proc_dir_entry *my_entry;

static int __init mod_init(void)
{
	my_entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);
	if (IS_ERR(my_entry)) {
		PR_ERROR("error in create_proc_entry");
		return PTR_ERR(my_entry);
	}
	pr_info(KBUILD_MODNAME " loaded successfully\n");
	return 0;
}

static void __exit mod_exit(void)
{
	proc_remove(my_entry);
	pr_info(KBUILD_MODNAME " unloaded successfully\n");
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
