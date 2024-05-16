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
#include <linux/mman.h> /* for remap_pfn_range */
#include <linux/pagemap.h> /* for vma structures */
#include <linux/sched.h> /* for current */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */
#include "shared.h" /* for the ioctl numbers */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Mmapping kernel allocated memory to user space");

/*
 *	This is a driver that maps memory allocated by the kernel into
 *	user space.
 *	The method is a user driven ioctl.
 *	The device does not support the 'mmap' operation (as you can see
 *	from its ops table below) but rather does the equivalent of mmap
 *	from it's ioctl	context.
 */

/* static data */
static struct device *my_device;
static bool do_kmalloc = true;
/* This is the kernel space pointer, this is all the kernel needs to know */
static void *kptr;
/* for the size we get from user space */
static unsigned int size = -1;
/* for the pointer we return to user space */
static unsigned long uptr = -1;

/* do we want to use kmalloc or get_free_pages ? */
static void *alloc_mem(unsigned int size) {
	/* kernel address to be returned */
	void *kaddr;
	/* order of allocation in case we use get_free_pages */
	int order;

	PR_DEBUG("start");
	if (do_kmalloc) {
		kaddr = kmalloc(size, GFP_KERNEL);
	} else {
		order = get_order(size);
		kaddr = (void *)__get_free_pages(GFP_KERNEL, order);
	}
	if (((unsigned long)kaddr) % PAGE_SIZE != 0)
		return NULL;
	return kaddr;
}

static void free_mem(void *kptr, unsigned int size)
{
	/* order of allocation in case we use get_free_pages */
	int order;

	PR_DEBUG("start");
	if (do_kmalloc) {
		/* kfree does not return error code */
		kfree(kptr);
	} else {
		order = get_order(size);
		/* free pages does not return error code */
		free_pages((unsigned long)kptr, order);
	}
}

static unsigned long map_to_user(struct file *filp, void *kptr,
	unsigned int size)
{
	/* the user space address to be returned */
	unsigned long uptr;
	/* the mmap struct to hold the semaphore of */
	/* struct mm_struct *mm; */
	/* flags to pass to do_mmap_pgoff */
	unsigned long flags;
	/* old value in private field */
	void *oldval;

	/* print some debug info... */
	PR_DEBUG("size is (d) %d", size);

	/* mm = current->mm; */
	/* must NOT add MAP_LOCKED to the flags (it causes a hang) */
	flags = MAP_POPULATE | MAP_SHARED;
	/* flags=MAP_POPULATE|MAP_PRIVATE; */
	flags &= ~(MAP_EXECUTABLE | MAP_DENYWRITE);
	/*
	 * vm_mmap does not need the semaphore to be held
	 * down_write(&mm->mmap_sem);
	 */
	oldval = filp->private_data;
	filp->private_data = kptr;
	uptr = vm_mmap(
		filp, /* file pointer for which filp->mmap will be called */
		0, /* address - this is the address we recommend for user space - best not to ... */
		size, /* size */
		PROT_READ | PROT_WRITE, /* protection */
		flags, /* flags */
		0 /* pg offset */
	);
	filp->private_data = oldval;
	/*
	 * vm_mmap does not need the semaphore to be held
	 * up_write(&mm->mmap_sem);
	 */
	if (IS_ERR_VALUE(uptr))
		PR_ERROR("ERROR: problem calling do_mmap_pgoff");
	else {
		PR_DEBUG("addr for user space is (lu) %lu / (p) %p", uptr,
			(void *)uptr);
	}
	return uptr;
}

/*
 * static int unmap(unsigned long uadr,unsigned int size) {
 *	int res = do_munmap(current->mm,uadr,size);
 *	if(res)
 *		PR_ERROR("PR_ERROR: unable to do_munmap");
 *	return res;
 * }
 */

/* fops */

/*
 * This is the ioctl implementation.
 */
static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	/* for results from functions */
	int res;

	PR_DEBUG("start with cmd %d", cmd);
	switch (cmd) {
	/*
	 *	Asking the kernel to mmap into user space.
	 *	Only argument is size.
	 */
	case IOCTL_DEMO_MAP:
		PR_DEBUG("trying to mmap");
		size = arg;
		kptr = alloc_mem(size);
		if (kptr == NULL) {
			PR_ERROR("ERROR: could not allocate memory");
			return -EFAULT;
		}
		PR_DEBUG("After alloc_mem with kptr=%p", kptr);
		uptr = map_to_user(filp, kptr, size);
		if (IS_ERR_VALUE(uptr)) {
			PR_ERROR("ERROR: quiting on process of mmaping");
			return -EFAULT;
		}
		PR_DEBUG("After map_to_user");
		PR_DEBUG("Successful exit");
		return uptr;

	/*
	 *	Asking the kernel to munmap user space.
	 *	No arguments are required.
	 */
	case IOCTL_DEMO_UNMAP:
		PR_DEBUG("trying to munmap");
		res = vm_munmap(uptr, size);
		if (res)
			return res;
		PR_DEBUG("After unmap");
		free_mem(kptr, size);
		PR_DEBUG("Successful exit");
		/* so we won't accidentaly use these pointers */
		kptr = NULL;
		size = -1;
		uptr = -1;
		return 0;

	/*
	 *	Asking the kernel to write to the buffer.
	 *	One argument which is the value to write.
	 */
	case IOCTL_DEMO_WRITE:
		if (kptr == NULL) {
			PR_ERROR("ERROR: kptr is NULL?!?");
			return -EFAULT;
		}
		memset(kptr, arg, size);
		return 0;

	/*
	 *	Asking the kernel to check that the buffer is a certain value.
	 *	One argument which is the value to check.
	 */
	case IOCTL_DEMO_READ:
		if (kptr == NULL) {
			PR_ERROR("ERROR: kptr is NULL?!?");
			return -EFAULT;
		}
		return memcheck(kptr, arg, size);

	/*
	 *	Asking the kernel to copy the in kernel buffer to user space.
	 *	One argument which is the pointer to the user space buffer.
	 */
	case IOCTL_DEMO_COPY:
		if (kptr == NULL) {
			PR_ERROR("ERROR: kptr is NULL?!?");
			return -EFAULT;
		}
		return copy_to_user((void *)arg, kptr, size);
	}
	return -EINVAL;
}


/*
 *	VMA ops
 */
static void kern_vma_open(struct vm_area_struct *vma)
{
	PR_DEBUG("start");
	PR_DEBUG("vma->start is %lu , %lx", vma->vm_start, vma->vm_start);
	PR_DEBUG("vma->end-vma->start is %lu", vma->vm_end - vma->vm_start);
}


static void kern_vma_close(struct vm_area_struct *vma)
{
	PR_DEBUG("start");
	PR_DEBUG("vma->start is %lu , %lx", vma->vm_start, vma->vm_start);
	PR_DEBUG("vma->end-vma->start is %lu", vma->vm_end - vma->vm_start);
}

/* on error should return VM_FAULT_SIGBUS */
static vm_fault_t kern_vma_fault(struct vm_fault *vmf)
{
	struct page *page = NULL;
	/* kernel side address */
	void *kaddr;
	/* offset at which user wants the page... */
	unsigned long offset;

	PR_DEBUG("start");
	/* for old kernels */
	/* offset = (unsigned long)vmf->virtual_address - vma->vm_start; */
	/* for new kernels */
	offset = (unsigned long)vmf->address - vmf->vma->vm_start;
	kaddr = vmf->vma->vm_private_data;
	kaddr += offset;
	page = virt_to_page(kaddr);
	if (page == NULL) {
		PR_ERROR("couldnt find page");
		return VM_FAULT_SIGBUS;
	}
	get_page(page);
	vmf->page = page;
	return 0;
}

static const struct vm_operations_struct kern_vm_ops = {
	.open = kern_vma_open,
	.close = kern_vma_close,
	.fault = kern_vma_fault,
};

/*
 * The mmap implementation.
 */
static int kern_mmap(struct file *filp, struct vm_area_struct *vma)
{
	/*
	 *	// size of memory to map
	 *	unsigned int size;
	 *	// for the physical address
	 *	unsigned long phys;
	 *	// for the starting page number
	 *	unsigned int pg_num;
	 *	// for return values
	 *	int ret;
	 *
	 *	size=vma->vm_end-vma->vm_start;
	 *	phys=virt_to_phys(kadr);
	 *	pg_num=phys >> PAGE_SHIFT;
	 *	PR_DEBUG("size is %d",size);
	 *	PR_DEBUG("kadr is %p",kadr);
	 *	PR_DEBUG("phys is %lx",phys);
	 *	PR_DEBUG("pg_num is %d",pg_num);
	 *	PR_DEBUG("vm_start is %lx",vma->vm_start);
	 *	PR_DEBUG("vm_end is %lx",vma->vm_end);
	 *	PR_DEBUG("vm_pgoff is %lx",vma->vm_pgoff);
	 *	ret=remap_pfn_range(
	 *		vma, // into which vma
	 *		vma->vm_start, // where in the vma
	 *		pg_num, // which starting physical page
	 *		size, // how much to map (in bytes)
	 *		vma->vm_page_prot // what protection to give
	 *	);
	 *	if(ret) {
	 *		PR_ERROR("ERROR: could not remap_pfn_range");
	 *		return ret;
	 *	}
	 */
	/* pointer for already allocated memory */
	void *kadr;

	PR_DEBUG("start");
	kadr = filp->private_data;
	/* does not work on 3.8.0 */
	/* vma->vm_flags |= VM_RESERVED; */

	vma->vm_private_data = kadr;
	vma->vm_ops = &kern_vm_ops;
	kern_vma_open(vma);
	PR_DEBUG("all ok from mmap. returning");
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
