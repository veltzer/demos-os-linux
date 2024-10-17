// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <linux/module.h> /* for MODULE_* */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for struct device */
#include <linux/slab.h> /* for the kmalloc API */
#include <linux/sched.h> /* for current */
#include <linux/mman.h> /* for remap_pfn_range */
#include <linux/pagemap.h> /* for vma structures */
#include <linux/io.h> /* for virt_to_phys */
#include "shared.h" /* for the ioctl numbers */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
 * This is a driver that maps memory allocated by the kernel into user space.
 * The method is the regular mmap system call.
 *
 * This driver also shows you how to implement mmap via a regular ioctl call.
 *
 * TODO:
 * - show how to kfree my pointer when munmap is made
 * - separate the part of the driver that does the mmap via the ioctl call.
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("module that maps memory allocated by the kernel to user space");

#define DO_FREE
#define DO_RESERVE

/* static data */
static void *vaddr;
static unsigned int size;
static unsigned int pg_num;
static unsigned long phys;
static unsigned int pages;
static bool do_kmalloc = true;
static struct device *my_device;
static unsigned long addr;
static int ioctl_size;
static void *kaddr;

/* fops */

/*
 * This is the ioctl implementation.
 */
static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	char str[256];
	void *ptr;
	unsigned int order;
	unsigned long private;
	unsigned long adjusted;
	unsigned int diff;
	int ret;
	struct vm_area_struct *vma;
	// struct mm_struct *mm;
	void *kernel_addr;
	unsigned long flags;

	pr_debug("start");
	switch (cmd) {
	/*
	 * Exploring VMA issues
	 */
	case IOCTL_MMAP_PRINT:
		ptr = (void *)arg;
		pr_info("ptr is %p", ptr);
		vma = find_vma(current->mm, arg);
		pr_info("vma is %p", vma);
		diff = arg - vma->vm_start;
		pr_info("diff is %d", diff);
		private = (unsigned long)vma->vm_private_data;
		pr_info("private (ul) is %lu", private);
		pr_info("private (p) is %p", (void *)private);
		adjusted = private + diff;
		pr_info("adjusted (ul) is %lu", adjusted);
		pr_info("adjusted (p) is %p", (void *)adjusted);
		return 0;

	/*
	 * This is asking the kernel to read the memory
	 */
	case IOCTL_MMAP_READ:
		pr_debug("starting to read");
		memcpy(str, vaddr, 256);
		str[255] = '\0';
		pr_debug("data is %s", str);
		return 0;

	/*
	 * This is asking the kernel to write the memory
	 */
	case IOCTL_MMAP_WRITE:
		pr_debug("starting to write");
		memset(vaddr, arg, size);
		return 0;

	/*
	 * This demos how to take the user space pointer and turn it
	 * into a kernel space pointer
	 */
	case IOCTL_MMAP_WRITE_USER:
		pr_debug("starting to write using us pointer");
		ptr = (void *)arg;
		pr_debug("ptr is %p", ptr);
		return 0;

	/*
	 * mmap a region
	 */
	case IOCTL_MMAP_MMAP:
		pr_debug("trying to mmap");
		if (do_kmalloc)
			kaddr = kmalloc(ioctl_size, GFP_KERNEL);
		else {
			order = get_order(ioctl_size);
			kaddr = (void *)__get_free_pages(GFP_KERNEL, order);
		}
		// mm = current->mm;
		flags = MAP_POPULATE | MAP_SHARED | MAP_LOCKED;
		flags &= ~(MAP_EXECUTABLE | MAP_DENYWRITE);
		/*
		 * vm_mmap does not need the semaphore to be held
		 * down_write(&mm->mmap_sem);
		 */
		addr = vm_mmap(
			/* file pointer */
			filp,
			/* address - this is the buffer we kmalloc'ed */
			(unsigned long)kaddr,
			/* size */
			ioctl_size,
			/* protection */
			PROT_READ | PROT_WRITE,
			/* flags */
			flags,
			/* pg offset */
			0
		);
		/*
		 * vm_mmap does not need the semaphore to be held
		 * up_write(&mm->mmap_sem);
		 */
		pr_debug("kaddr is (p) %p", kaddr);
		pr_debug("real size is (d) %d", ioctl_size);
		pr_debug("addr for user space is (lu) %lu / (p) %p", addr, (void *)addr);
		return addr;

	/*
	 * unmap a region
	 */
	case IOCTL_MMAP_UNMAP:
		pr_debug("trying to unmap");
		vma = find_vma(current->mm, addr);
		kernel_addr = vma->vm_private_data;
		size = vma->vm_end - vma->vm_start;
		pr_debug("deduced kernel_addr is %p", kernel_addr);
		pr_debug("deduced size is (d) %d", size);
		pr_debug("real size is (d) %d", ioctl_size);
		pr_debug("real kaddr is (p) %p", kaddr);
		ret = vm_munmap(addr, ioctl_size);
		if (do_kmalloc)
			kfree(kernel_addr);
		else {
			order = get_order(size);
			free_pages((unsigned long)kernel_addr, order);
		}
		return ret;

	/*
	 * The size of the region
	 */
	case IOCTL_MMAP_SETSIZE:
		pr_debug("setting the size");
		ioctl_size = arg;
		pr_debug("size is %d", ioctl_size);
		return 0;
	}
	return -EINVAL;
}

/*
 * VMA ops
 */
static void kern_vma_open(struct vm_area_struct *vma)
{
	pr_debug("start");
}

static void kern_vma_close(struct vm_area_struct *vma)
{
#ifdef DO_FREE
	unsigned int order;
#endif /* DO_FREE */
	unsigned int size = vma->vm_end - vma->vm_start;
	void *addr = vma->vm_private_data;

	pr_debug("start");
	pr_debug("pointer as long is %lu", vma->vm_start);
	pr_debug("pointer as pointer is %p", (void *)(vma->vm_start));
	pr_debug("addr is %p", addr);
	pr_debug("size is %d", size);
#ifdef DO_FREE
	if (do_kmalloc)
		kfree(addr);
	else {
		order = get_order(size);
		free_pages((unsigned long)addr, order);
	}
#endif /* DO_FREE */
}

static const struct vm_operations_struct kern_remap_vm_ops = {
	.open = kern_vma_open,
	.close = kern_vma_close,
};

/*
 * This is the most basic mmap implementation. It does NOT work because
 * you don't really state WHAT memory kernel side you are mapping to user
 * space...
 */

/*
 * static int kern_mmap_simple(struct file *filp, struct vm_area_struct *vma) {
 *	return remap_pfn_range(
 *		vma, // vma
 *		vma->vm_start, // start
 *		vma->vm_pgoff, // page number
 *		vma->vm_end-vma->vm_start, // size
 *		vma->vm_page_prot // protection
 *	);
 * }
 */

/*
 * The mmap implementation.
 */
static int kern_mmap(struct file *filp, struct vm_area_struct *vma)
{
#ifdef DO_RESERVE
	/* for the reserve loop */
	int i;
#endif /* DO_RESERVE */
	/* for return values */
	int ret;
	/* for __get_user_pages */
	int order = 0;

	pr_debug("start");
	size = vma->vm_end - vma->vm_start;
	pr_debug("size is %d", size);

	/*
	 * This code uses kmalloc
	 */
	if (do_kmalloc) {
		/* calculate number of pages needed */
		pages = size / PAGE_SIZE;
		if (size % PAGE_SIZE > 0)
			pages++;
		vaddr = kmalloc(size, GFP_KERNEL);
#ifdef DO_RESERVE
		/* reserve the pages */
		for (i = 0; i < pages * PAGE_SIZE; i += PAGE_SIZE) {
			SetPageReserved(
				virt_to_page(((unsigned long)vaddr) + i));
		}
#endif /* DO_RESERVE */
	} else {
		/*
		 * This code used __get_free_pages
		 */
		order = get_order(size);
		addr = __get_free_pages(
			GFP_KERNEL,
			order
		);
		vaddr = (void *)addr;
		pr_debug("addr is %lx", addr);
		pr_debug("order is %d", order);
	}
	pr_debug("vaddr is %p", vaddr);
	/* memset(vaddr,'a',size); */
	phys = virt_to_phys(vaddr);
	pg_num = phys >> PAGE_SHIFT;
	pr_debug("phys is %lx", phys);
	pr_debug("pg_num is %d", pg_num);
	/* pr_debug("pp is %p",pp); */
	/* pr_debug("start_addr is %p",start_addr); */
	pr_debug("vm_start is %lx", vma->vm_start);
	pr_debug("vm_end is %lx", vma->vm_end);
	pr_debug("vm_pgoff is %lx", vma->vm_pgoff);
	ret = remap_pfn_range(
		vma,
		vma->vm_start,
		pg_num,
		size,
		vma->vm_page_prot
	);
	if (ret) {
		pr_debug("in error path");
		if (do_kmalloc)
			kfree(vaddr);
		else
			free_pages((unsigned long)addr, order);
		return ret;
	}
	/* memset(vaddr,'b',size); */
	vma->vm_private_data = vaddr;
	vma->vm_ops = &kern_remap_vm_ops;
	kern_vma_open(vma);
	return 0;
}


/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = kern_unlocked_ioctl,
	.mmap = kern_mmap,
/*	.mmap = kern_mmap_simple, */
};

#include "device.inc"
