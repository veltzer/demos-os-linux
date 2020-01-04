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
#include <linux/pagemap.h> /* for vma structures */
#include <linux/sched.h> /* for current */
#include <linux/vmalloc.h> /* for vmap */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("get_user_pages demo");

/*
*	This driver demostrates how to map user space (virtual space)
*	to kernel space.
*	You can either get it fragmented in pages, or if you really need to,
*	use the MMU to map it to a single kernel side linear address...
*/

#include "shared.h"

/* static data */
static struct device *my_device;
/* this is the size of the buffer */
static unsigned int size;
/* this is the kernel space pointer (matches user) */
static void *ptr;
/* this is the kernel space pointer (we got from vmap) */
static void *vptr;
/* will store page data as they are mapped... */
static struct page **pages;
/* will store the numer of pages required... */
static unsigned int nr_pages;

/* fops */

/*
* This is the ioctl implementation.
*/

static inline void pages_unlock(void)
{
	unsigned int i;
	/* unlock the pages */
	for (i = 0; i < nr_pages; i++)
		unlock_page(pages[i]);
}

static inline void pages_lock(void)
{
	unsigned int i;
	/* unlock the pages */
	for (i = 0; i < nr_pages; i++)
		lock_page(pages[i]);
}

static inline void pages_dirty(void)
{
	unsigned int i;

	/* set the pages as dirty */
	for (i = 0; i < nr_pages; i++)
		SetPageDirty(pages[i]);
}

static inline void pages_unmap(void)
{
	unsigned int i;

	/* set the pages as dirty */
	for (i = 0; i < nr_pages; i++) {
		if (!PageReserved(pages[i]))
			SetPageDirty(pages[i]);
		put_page(pages[i]);
	}
}

static inline void pages_reserve(void)
{
	unsigned int i;
	/* set the pages as reserved */
	for (i = 0; i < nr_pages; i++)
		SetPageReserved(pages[i]);
}

static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg) {
	/* this is the buffer which will hold the data of the buffer from user
	 * space... */
	BufferStruct b;
	/* for results from calls */
	int res;
	/* for printing buffers */
	char *cptr;
	/* for loop length */
	unsigned int sloop;
	/* for loops */
	unsigned int i;
	/* some unsigned ints for address manipulation... */
	unsigned long bpointer, offset, aligned, newsize;
	/* the newly created vma */
	struct vm_area_struct *vma;

	PR_DEBUG("start with ioctl %u", cmd);
	switch (cmd) {
	/*
	*	This is asking the kernel to map the memory to kernel space.
	*/
	case IOCTL_DEMO_MAP:
		/* get the data from the user */
		res = copy_from_user(&b, (void *)arg, sizeof(b));
		if (res) {
			PR_ERROR("problem with copy_from_user");
			return res;
		}
		PR_DEBUG("after copy");
		bpointer = (unsigned long)b.pointer;
		offset = bpointer % PAGE_SIZE;
		aligned = bpointer - offset;
		newsize = b.size + offset;
		PR_DEBUG("bpointer is %x", bpointer);
		PR_DEBUG("offset is %u", offset);
		PR_DEBUG("aligned is %x", aligned);
		PR_DEBUG("newsize is %u", newsize);

		/*
		* // make sure that the user data is page aligned...
		* if(((unsigned int)b.pointer)%PAGE_SIZE!=0) {
		*	PR_ERROR("pointer is not page aligned");
		*	return -EFAULT;
		* }
		* PR_DEBUG("after modulu check");
		*/
		/* find the number of pages */
		nr_pages = (newsize - 1) / PAGE_SIZE + 1;
		PR_DEBUG("nr_pages is %d", nr_pages);
		/* alocate page structures... */
		pages = kmalloc(nr_pages * sizeof(struct page *),
				GFP_KERNEL);
		if (IS_ERR(pages)) {
			PR_ERROR("could not allocate page structs");
			return PTR_ERR(pages);
		}
		PR_DEBUG("after pages allocation");
		/* get user pages and fault them in */
		down_write(&current->mm->mmap_sem);
		/* rw==READ means read from drive, write into memory area */
		res = get_user_pages(
			/* current->mm,
			current, */
			aligned,
			nr_pages,
			1,/* write */
			pages,
			NULL
		);
		vma = find_vma(current->mm, bpointer);
		vma->vm_flags |= VM_DONTCOPY;
		up_write(&current->mm->mmap_sem);
		PR_DEBUG("after get_user_pages res is %d", res);
		/* Errors and no page mapped should return here */
		if (res != nr_pages) {
			PR_ERROR("could not get_user_pages. res was %d", res);
			kfree(pages);
			return -EFAULT;
		}
		/* pages_lock();
		pages_reserve();
		pages_unlock(); */
		/* map the pages to kernel space... */
		vptr = vmap(pages, nr_pages, VM_MAP, PAGE_KERNEL);
		if (vptr == NULL) {
			PR_ERROR("could not get_user_pages. res was %d", res);
			kfree(pages);
			return -EFAULT;
		}
		ptr = vptr + offset;
		size = b.size;
		PR_DEBUG("after vmap - vptr is %p", vptr);
		/* free the pages */
		kfree(pages);
		pages = NULL;
		PR_DEBUG("after freeing the pages");
		/* were dont! return with success */
		PR_DEBUG("success - on the way out");
		return 0;

	/*
	*	This is asking the kernel to unmap the data
	*	No arguments are passed
	*/
	case IOCTL_DEMO_UNMAP:
		/* this function does NOT return an error code. Strange...:) */
		vunmap(vptr);
		vptr = NULL;
		ptr = NULL;
		size = 0;
		nr_pages = 0;
		pages_unmap();
		return 0;

	/*
	*	This is asking the kernel to read the data.
	*	No arguments are passed
	*/
	case IOCTL_DEMO_READ:
		cptr = (char *)ptr;
		sloop = min_t(unsigned int, size, (unsigned int)10);
		PR_DEBUG("sloop is %d", sloop);
		for (i = 0; i < sloop; i++)
			PR_INFO("value of %d is %c", i, cptr[i]);
		return 0;
	/*
	*	This is asking the kernel to write on our data
	*	argument is the constant which will be used...
	*/
	case IOCTL_DEMO_WRITE:
		memset(ptr, arg, size);
		/* pages_dirty(); */
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
