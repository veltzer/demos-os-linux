// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <linux/page-flags.h> /* for PG_* constants */
#include <linux/mm.h> /* for page_* functions */
#include <linux/ioport.h> /* for page_* functions */
#include <linux/io.h> /* for */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("module which prints out the ram map");

/* static data */
static unsigned int physaddr = 0x32000000;
static unsigned int size = 170 * 1024 * 1024;
static void *logical;

/* our own functions */
static void api_print_addressinfo(void *logical_adr)
{
	struct page *page = virt_to_page(logical_adr);

	if (page == NULL) {
		pr_info("unable to translate address %p to page", logical_adr);
		return;
	}
	pr_info("address %p, page:%p flags:0x%0*lx mapping:%p count:%d\n",
		logical_adr,
		page,
		(int)(2 * sizeof(unsigned long)),
		page->flags,
		page->mapping,
		page_count(page)
	);

	pr_info("PG_lru is %lu", page->flags & (1 << PG_lru));
	pr_info("PG_private is %lu", page->flags & (1 << PG_private));
	pr_info("PG_locked is %lu", page->flags & (1 << PG_locked));
	/* Missing in newer kernels and so is remarked... */
	/* pr_info("PG_buddy is %lu", page->flags & (1 << PG_buddy)); */
	pr_info("PG_writeback is %lu", page->flags & (1 << PG_writeback));
	pr_info("PG_slab is %lu", page->flags & (1 << PG_slab));
	pr_info("PG_swapcache is %lu", page->flags & (1 << PG_swapcache));
	pr_info("PG_active is %lu", page->flags & (1 << PG_active));
	pr_info("PG_reserved is %lu", page->flags & (1 << PG_reserved));
}

static void api_debug_address(unsigned int phys)
{
	void *logical = __va(phys);
	void *logical2 = phys_to_virt(phys);
	unsigned int phys2 = __pa(logical);

	pr_info("phys is %u", phys);
	pr_info("logical is %p", logical);
	pr_info("phys2 is %u", phys2);
	pr_info("logical2 is %p", logical2);
	api_print_addressinfo(logical);
}

static int __init mod_init(void)
{
	pr_debug("start");
	api_debug_address(physaddr);

	/*
	 * if (!request_mem_region(physaddr,size,)) {
	 *	pr_err("could not get the memory");
	 *	return 1;
	 * }
	 */
	logical = ioremap(physaddr, size);
	if (IS_ERR(logical)) {
		pr_err("could not ioremap");
		release_mem_region(physaddr, size);
		return PTR_ERR(logical);
	}
	pr_info("got logical address %p", logical);
	/*
	 * memset(logical,0,size);
	 * logical=5;
	 * pr_info("read %c",*logical);
	 * logical=phys_to_virt(physaddr);
	 * for(i=0;i<170*1024*1024;i++)
	 *	logical[i]=0;
	 * api_print_addressinfo((void*)(1024*1024*700));
	 * api_print_addressinfo((void*)(1024*1024*695));
	 * api_print_addressinfo((void*)(1024*1024*720));
	 */
	return 0;
}

static void __exit mod_exit(void)
{
	pr_debug("start");
	iounmap(logical);
	release_mem_region(physaddr, size);
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
