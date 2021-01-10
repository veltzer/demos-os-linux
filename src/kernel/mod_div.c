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
#include <linux/module.h> /* for MODULE_* stuff */
#include <linux/fs.h> /* for struct device */
#include <linux/device.h> /* to register our device */
#include <linux/uaccess.h> /* copy_to_user, copy_from_user */
/* define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Driver that adds 64 bit integer arithmetic operations to the kernel");

#include "shared.h" /* for ioctl numbers */

/*
 * Why do you need this module?
 * Because on a 32 bit intel system you do not have long long operations
 * for division.
 * Other operations work fine btw.
 */

/* static data */
static struct device *my_device;

/* fops */

/*
 * This is the ioctl implementation.
 */
static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	/* the buffer which will be used for the transaction */
	buffer b;
	int err;

	PR_DEBUG("start");
	switch (cmd) {
	case IOCTL_DIV_DOOPS:
		/* get the data from the user */
		err = copy_from_user(&b, (void *)arg, sizeof(b));
		if (err) {
			PR_ERROR("problem with copy_from_user");
			return err;
		}
		PR_DEBUG("after copy");
		PR_INFO("b.u1 is %llu", b.u1);
		PR_INFO("b.u2 is %llu", b.u2);
		PR_INFO("b.d1 is %lld", b.d1);
		PR_INFO("b.d2 is %lld", b.d2);
		b.udiv = b.u1 / b.u2;
		b.umul = b.u1 * b.u2;
		b.uadd = b.u1 + b.u2;
		b.usub = b.u1 - b.u2;
		b.ddiv = b.d1 / b.d2;
		b.dmul = b.d1 * b.d2;
		b.dadd = b.d1 + b.d2;
		b.dsub = b.d1 - b.d2;
		/* copy the data back to the user */
		err = copy_to_user((void *)arg, &b, sizeof(b));
		if (err) {
			PR_ERROR("problem with copy_to_user");
			return err;
		}
		/* everything is ok */
		return 0;
	}
	return -EINVAL;
}

/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = kern_unlocked_ioctl,
};

#include "device.inc"

/* this is what gives us the division...
 */
#include <asm/div64.h>

/*
 * The header above gave us functions as div_u64_rem. We can either use them
 * directly or we could define the functions under the names __udivdi3
 * __divdi3 which are the names that gcc plants to be searched in case we
 * want regular code like:
 * long long x=
 * long long y=
 * long long z=x/y
 * to work.
 * See the following code...
 */

static unsigned long long __udivdi3(unsigned long long divided,
		unsigned long long divisor)
{
	unsigned int reminder;

	PR_DEBUG("divided is %llu", divided);
	PR_DEBUG("divisor is %llu", divisor);
	return div_u64_rem(divided, divisor, &reminder);
}

static long long __divdi3(long long divided, long long divisor)
{
	unsigned int reminder;

	PR_DEBUG("divided is %lld", divided);
	PR_DEBUG("divisor is %lld", divisor);
	return div_u64_rem(divided, divisor, &reminder);
}

/* disregard the next section. It is documentation for myself since it took
 * me quite a long time to find the above and I want to preserve the road I
 * got there by...
 */

/* this is where the 64 bit division magic starts... */

/* and here is the division function: */

/*
 * 64bit division - for sync stuff..
 */

/*
 * #define udiv_qrnnd(q, r, n1, n0, d) \
 * __asm__ ("divl %4" \
 * :"=a" ((u32)(q)), \
 * "=d" ((u32)(r)) \
 * :"0" ((u32)(n0)), \
 * "1" ((u32)(n1)), \
 * "rm" ((u32)(d)))
 *
 * #define u64_div(x,y,q) do {u32 __tmp; udiv_qrnnd(q, __tmp, (x)>>32, x, y);}\
 *	while(0)
 * #define u64_mod(x,y,r) do {u32 __tmp; udiv_qrnnd(__tmp, q, (x)>>32, x, y);}\
 *	while(0)
 * #define u64_divmod(x,y,q,r) udiv_qrnnd(q, r, (x)>>32, x, y)
 */

/*
 * #define _FP_W_TYPE_SIZE 32
 * #define _FP_W_TYPE unsigned int
 * #define _FP_WS_TYPE signed int
 * #define _FP_I_TYPE int
 *
 * #include <math-emu/op-1.h>
 * #include <math-emu/op-2.h>
 * #include <math-emu/op-4.h>
 * #include <math-emu/op-common.h>
 */
/* there is no such file for x86 */
/*#include <asm/sfp-machine.h> */
/* creates compilation issues... */
/*#include <math-emu/soft-fp.h> */
/* there is no such file for x86 */
/* #include <math-emu/sfp-util.h> */

/*
 * - If you compile on kernel 2.6.31-14-generic (ubuntu 9.10)
 * - If you compile on kernel 2.6.28-15-generic (ubuntu 9.04)
 *	you will find that the symbol __udivdi3 is missing.
 *	You see that in two place:
 *	- when you compile the module you get the warning:
 *	WARNING: "__udivdi3" [/home/mark/bla/kernel_arithmetic/demo.ko]
 *	undefined!
 *	- when you try to insmod the module you get the error:
 *	[12697.177574] demo: Unknown symbol __udivdi3
 *	This means that you need to link with libgcc.
 *	just run 'make relink'.
 */
