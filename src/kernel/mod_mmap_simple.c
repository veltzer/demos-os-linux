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
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for struct device */
#include <linux/slab.h> /* for the kmalloc API */
#include <linux/mman.h> /* for remap_pfn_range */
#include <linux/pagemap.h> /* for vma structures */
#include <linux/sched.h> /* for current */
#include "shared.h" /* for the ioctl numbers */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Showing how to implement mmap");

/*
 * Mmap is way by which your module may map memory into the address space
 * of the process that is using it. This is an example of how to do it.
 */
static unsigned long addr;
static void *vaddr;
static unsigned int size;
/* this is initialised automatically to false (0) ... */
static bool do_kmalloc;
static struct device *my_device;
static unsigned long addr;
static int ioctl_size;

/* fops */

/*
 * This is the ioctl implementation.
 */
static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	/* int i; */
	char str[256];
	void *ptr;
	unsigned int order;

	unsigned long private;
	unsigned long adjusted;
	unsigned int diff;
	int ret;
	struct vm_area_struct *vma;
	struct mm_struct *mm;
	void *kernel_addr;
	unsigned long flags;

	PR_DEBUG("start");
	switch (cmd) {
	/*
	 *	Exploring VMA issues
	 */
	case IOCTL_MMAP_PRINT:
		ptr = (void *)arg;
		PR_DEBUG("ptr is %p", ptr);
		vma = find_vma(current->mm, arg);
		PR_DEBUG("vma is %p", vma);
		diff = arg - vma->vm_start;
		PR_DEBUG("diff is %d", diff);
		private = (unsigned long)vma->vm_private_data;
		PR_DEBUG("private (ul) is %lu", private);
		PR_DEBUG("private (p) is %p", (void *)private);
		adjusted = private + diff;
		PR_DEBUG("adjusted (ul) is %lu", adjusted);
		PR_DEBUG("adjusted (p) is %p", (void *)adjusted);
		return 0;

	/*
	 *	This is asking the kernel to read the memory
	 */
	case IOCTL_MMAP_READ:
		PR_DEBUG("starting to read");
		memcpy(str, vaddr, 256);
		str[255] = '\0';
		PR_DEBUG("data is %s", str);
		return 0;

	/*
	 *	This is asking the kernel to write the memory
	 */
	case IOCTL_MMAP_WRITE:
		PR_DEBUG("starting to write");
		memset(vaddr, arg, size);
		return 0;

	/*
	 *	This demos how to take the user space pointer and turn it
	 *	into a kernel space pointer
	 */
	case IOCTL_MMAP_WRITE_USER:
		PR_DEBUG("starting to write using us pointer");
		ptr = (void *)arg;
		PR_DEBUG("ptr is %p", ptr);
		return 0;

	/*
	 *	mmap a region from an ioctl
	 */
	case IOCTL_MMAP_MMAP:
		PR_DEBUG("trying to mmap");

		/*
		 * if(do_kmalloc) {
		 *	kaddr=kmalloc(ioctl_size,GFP_KERNEL);
		 * } else {
		 *	order=get_order(ioctl_size);
		 *	kaddr=(void*)__get_free_pages(GFP_KERNEL,order);
		 * }
		 */
		mm = current->mm;
		flags = MAP_POPULATE | MAP_SHARED;
		flags &= ~(MAP_EXECUTABLE | MAP_DENYWRITE);
		/*
		 * must hold process memory map semaphore because next
		 * function will change memory layout for the process.
		 * This also means that this code must be in a path that can
		 * sleep.
		 */
		/*
		 * vm_mmap does not need the semaphore to be held
		 * down_write(&mm->mmap_sem);
		 */
		addr = vm_mmap(
			filp,/* file pointer */
			0,/* recommended use space address */
			ioctl_size,/* size */
			PROT_READ | PROT_WRITE,/* protection */
			flags,/* flags */
			0/* pg offset */
		);
		/* remmember to release the semaphore! */
		/*
		 * vm_mmap does not need the semaphore to be held
		 * up_write(&mm->mmap_sem);
		 */
		/*
		 * PR_DEBUG("kaddr is (p) %p",kaddr);
		 * PR_DEBUG("real size is (d) %d",ioctl_size);
		 */
		PR_DEBUG(
			"addr for user space is (lu) %lu / (p) %p",
			addr, (void *)addr);
		return addr;

	/*
	 *	unmap a region
	 */
	case IOCTL_MMAP_UNMAP:
		PR_DEBUG("trying to unmap");
		vma = find_vma(current->mm, addr);
		kernel_addr = vma->vm_private_data;
		size = vma->vm_end - vma->vm_start;
		PR_DEBUG("deduced kernel_addr is %p", kernel_addr);
		PR_DEBUG("deduced size is (d) %d", size);
		PR_DEBUG("real size is (d) %d", ioctl_size);
		PR_DEBUG("real kaddr is (p) %p", kaddr);
		ret = vm_munmap(addr, ioctl_size);
		if (ret) {
			PR_ERROR("error from do_munmap");
			return ret;
		}
		if (do_kmalloc)
			kfree(kernel_addr);
		else {
			order = get_order(size);
			free_pages((unsigned long)kernel_addr, order);
		}
		return ret;

	/*
	 *	The the size of the region
	 */
	case IOCTL_MMAP_SETSIZE:
		PR_DEBUG("setting the size");
		ioctl_size = arg;
		PR_DEBUG("size is %d", ioctl_size);
		return 0;
	}
	return -EINVAL;
}

/*
 *	VMA ops
 */
void kern_vma_open(struct vm_area_struct *vma)
{
	PR_DEBUG("start");
}

void kern_vma_close(struct vm_area_struct *vma)
{
	unsigned int size = vma->vm_end - vma->vm_start;
	unsigned int order;
	void *addr = vma->vm_private_data;

	PR_DEBUG("start");
	PR_DEBUG("pointer as long is %lu", vma->vm_start);
	PR_DEBUG("pointer as pointer is %p", (void *)(vma->vm_start));
	PR_DEBUG("addr is %p", addr);
	PR_DEBUG("size is %d", size);
	if (do_kmalloc)
		kfree(addr);
	else {
		order = get_order(size);
		free_pages((unsigned int)addr, order);
	}
}


static const struct vm_operations_struct kern_remap_vm_ops = {
	.open = kern_vma_open,
	.close = kern_vma_close,
};

/*
 *	This is the most basic mmap implementation. It does NOT work because
 *	you don't really state WHAT memory kernel side you are mapping to user
 *	space...
 */
static int kern_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned int size, order, pg_num;
	unsigned long addr, phys;
	void *kaddr;
	int res;

	PR_DEBUG("start");
	size = vma->vm_end - vma->vm_start;
	order = get_order(size);
	addr = __get_free_pages(GFP_KERNEL, order);
	kaddr = (void *)addr;
	phys = virt_to_phys(vaddr);
	pg_num = phys >> PAGE_SHIFT;
	res = remap_pfn_range(
		/* vma */
		vma,
		/* start */
		vma->vm_start,
		/* how many pages */
		pg_num,
		/* size (derived from the vma) */
		size,
		/* protection */
		vma->vm_page_prot
	);
	if (res) {
		PR_DEBUG("error path");
		return res;
	}
	vma->vm_ops = &kern_remap_vm_ops;
	vma->vm_private_data = kaddr;
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
};

#include "device.inc"
