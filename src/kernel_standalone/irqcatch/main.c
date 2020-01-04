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
* An irq catcher module
*/
#include <linux/module.h>		/* for MODULE_* */
#include <linux/moduleparam.h>		/* for pr_info() */
#include <linux/kernel.h>		/* for printk() */
#include <linux/interrupt.h>		/* for request_irq(), free_irq() */
#include <linux/proc_fs.h>		/* for proc_create(),
						remove_proc_entry() */

MODULE_AUTHOR("Mark Veltzer");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("An irq catcher module");

/*
* TODO:
*/

/* the /proc entry with which you can view the number of irqs intercepted */
static const char *proc_filename = "irqcatch";
/* usb2 irq number on my desktop is 23, on dell laptop is 17 */
static unsigned int irq_num = 17;
module_param(irq_num, uint, S_IRWXU);
MODULE_PARM_DESC(irq_num, "irq_num is the irq number to catch");
/* the number if irqs got (initialised to 0 by default) */
static unsigned int counter;

/*
* proc file callback
*/
static ssize_t proc_read(struct file *file, char __user *buffer,
		size_t len, loff_t *offset)
{
	char buf[256];
	int mylen;
	int ret;
	/* EOF */
	if (file->private_data)
		return 0;
	mylen = snprintf(buf, 256, "counter=%d\n", counter);
	ret = copy_to_user(buffer, buf, mylen);
	if (ret) {
		return ret;
	} else {
		file->private_data = &proc_read;
		*offset += mylen;
		return mylen;
	}
}

/*
* Interrupt callback
* We say that this is not our interrupt.
*/
static irqreturn_t irqcatch_int_handler(int irq, void *dev)
{
	/* increase the counter. No need for atomics here
	 * since we are guaranteed by the kernel that this
	 * function will not be run concurrently... */
	counter++;
	/* this wasn't our interrupt...:) */
	return IRQ_NONE;
}

/*
* This structure gathers "functions" that manage the /proc file
*/
static const struct file_operations my_file_ops = {
	.owner = THIS_MODULE,
	/* .read = irqcatch_proc_reader, */
	.read = proc_read,
};

struct proc_dir_entry *my_entry;
/*
* Module housekeeping.
*/
static int irqcatch_init(void)
{
	int ret;
	my_entry = proc_create(proc_filename, S_IRUGO, NULL, &my_file_ops);
	if (IS_ERR(my_entry)) {
		pr_err("error in create_proc_entry");
		ret = PTR_ERR(my_entry);
		goto error_start;
	}

	ret = request_irq(irq_num, irqcatch_int_handler, IRQF_SHARED,
			THIS_MODULE->name, &counter);
	if (ret)
		goto error_after_proc;

	pr_info("irqcatch loaded sucessfuly\n");
	return 0;

/*
error_after_irq:
	free_irq(irq_num, &counter);
*/
error_after_proc:
	proc_remove(my_entry);
error_start:
	return ret;
}

static void irqcatch_cleanup(void)
{
	free_irq(irq_num, &counter);
	proc_remove(my_entry);
	pr_info("irqcatch unloaded succefully\n");
}

module_init(irqcatch_init);
module_exit(irqcatch_cleanup);
