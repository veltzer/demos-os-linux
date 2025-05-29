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

#include <firstinclude.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>
#include <sys/ioctl.h>
#include <err_utils.h>	// for CHECK_NOT_M1()
#include "shared.h"

/*
 * This test explores how kmalloc behaves when given various
 * sizes to allocate
 */

int main() {
	// file to be used
	const char *filename="/dev/demo";
	// klog_clear();
	int d=CHECK_NOT_M1(open(filename, O_RDWR));
	printf("starting out\n");
	for (unsigned int i=1; i < 1000000; i+=100) {
		// kmalloc does not allocate on a page boundry...
		// CHECK_NOT_M1(ioctl(d,IOCTL_DEMO_KMALLOC,i),"kmalloc");
		// printf("kmalloc: i is %d\n",i);
		CHECK_NOT_M1(ioctl(d, IOCTL_DEMO_GET_FREE_PAGES, i));
		printf("__get_free_pages: i is %d\n", i);
		// I don't seem to be able to call these unless I have a real
		// device (like PCI etc...).
		// CHECK_NOT_M1(ioctl(d,IOCTL_DEMO_DMA_ALLOC_COHERENT,i),"dma_alloc_coherent");
		// printf("dma_alloc_coherent: i is %d",i);
	}
	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
