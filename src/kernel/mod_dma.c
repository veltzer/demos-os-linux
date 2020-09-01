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

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_* */
#include <linux/slab.h> /* for the kmalloc API */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
 * This example demos how to allocate and deallocate coherent memory for DMA
 * When runnig this on intel/ubuntu 10.04 these are the conclusions:
 * - dma_alloc_coherent limit is 2Mb.
 * - kmalloc(GFP_DMA) limit is 2Mb.
 * - kmalloc(GFP_KERNEL) limit is 4Mb (which is still contiguous but ok
 *	to use for most hardware that does DMA).
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("DMA demo module");

/* our own functions */

/* static dma_addr_t device_addr; */

/* just one test (allocate and deallocate) */
/* #define DO_ONE */
/* allocate and free in a loop */
/* #define DO_LOOP */
/* slowly increase the size of the memory we are allocating until we bust... */
#define DO_INC

static int __init mod_init(void)
{
	void *vptr;
	/* unsigned int device_addr; */
#ifdef DO_LOOP
	int i;
#endif /* DO_LOOP */
#ifdef DO_INC
	unsigned int size;
	unsigned int inc;
	bool stop;
#endif /* DO_INC */
#ifdef DO_ONE
	const unsigned int size = 1024 * 1024 * 24;
#endif /* DO_ONE */
	PR_DEBUG("start");
#ifdef DO_LOOP
	for (i = 0; i < 1000; i++) {
		vptr = dma_alloc_coherent(NULL, size, &device_addr,
				GFP_KERNEL | GFP_DMA);
		if (IS_ERR(vptr)) {
			pr_err("ERROR! could not allocate memory");
			return PTR_ERR(vptr);
		dma_free_coherent(NULL, size, vptr, device_addr);
	}
#endif /* DO_LOOP */
#ifdef DO_ONE
	vptr = dma_alloc_coherent(NULL, size, &device_addr,
			GFP_KERNEL | GFP_DMA);
	if (IS_ERR(vptr)) {
		pr_err("ERROR! could not allocate memory");
		return PTR_ERR(vptr);
	}
	pr_info("vptr is %p\n", vptr);
	pr_info("size is %d\n", size);
	pr_info("device_addr is %d\n", device_addr);
	dma_free_coherent(NULL, size, vptr, device_addr);
#endif /* DO_ONE */
#ifdef DO_INC
	inc = 1024*512;
	size = 1024*512; /* half a meg */
	stop = false;
	while (!stop) {
		/*
		 * vptr = dma_alloc_coherent(NULL, size, &device_addr, GFP_DMA);
		 * vptr = kmalloc(size,GFP_DMA);
		 */
		vptr = kmalloc(size, GFP_KERNEL);
		if (IS_ERR(vptr)) {
			pr_err("ERROR! could not allocate memory for size %d",
				size);
			stop = true;
		}
		pr_info("vptr is %p\n", vptr);
		pr_info("size is %d\n", size);
		/*
		 * printk("device_addr is %d\n", device_addr);
		 * dma_free_coherent(NULL, size, vptr, device_addr);
		 */
		kfree(vptr);
		size += inc;
	}
#endif /* DO_INC */
	PR_DEBUG("end");
	return 0;
}

static void __exit mod_exit(void)
{
	PR_DEBUG("start");
	PR_DEBUG("end");
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
