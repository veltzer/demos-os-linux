/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <err_utils.h>	// for CHECK_NOT_M1()
#include "shared.h"

/*
 * This test checks memory mapping where the memory is dictated from
 * user space.
 * 1. Allocate memory in user space.
 * 2. Ask kernel to map it to kernel space.
 * 3. Ask kernel to manipulate that memory address.
 * 4. Check (from user space) that the memory was actually changed.
 * 5. Ask kernel to unmap the memory.
 * 6. Check (from user space) that the memory was actually changed.
 */

// file descriptor to access the device with...
int d;

const bool do_stack_test=true;
const bool do_valloc_test=true;
const bool do_dataseg_test=true;
const bool do_malloc_test=true;
const bool do_stacks_test=true;

static void show_data(char *data, unsigned int size) {
	printf("data is [");
	// int psize=min(size,10);
// fwrite(data,1,psize,stdout);
	printf("%c", *data);
	printf("]\n");
}

static void do_test(char *data, unsigned int size, const char *name) {
	printf("starting test named [%s]\n", name);
	int PAGE_SIZE=getpagesize();
	unsigned long adr=(unsigned long)data;
	bool aligned=(adr % PAGE_SIZE==0);
	if (aligned) {
		printf("data %p is page aligned\n", data);
	} else {
		printf("data %p is NOT page aligned\n", data);
	}
	/* the -1 here is so the kernel will not write on my newline */
	// data[size-1]='\0';

	printf("writing a into the buffer from userspace\n");
	memset(data, 'a', size);
	show_data(data, size);
	BufferStruct b;
	b.size=size;
	b.pointer=data;
	// klog_clear();
	CHECK_NOT_M1(ioctl(d, IOCTL_DEMO_MAP, &b));
	// klog_show();
	show_data(data, size);
	// klog_clear();
	CHECK_NOT_M1(ioctl(d, IOCTL_DEMO_READ));
	// klog_show();
	show_data(data, size);
	CHECK_NOT_M1(ioctl(d, IOCTL_DEMO_WRITE, 'z'));
	show_data(data, size);
	printf("writing b into the buffer from userspace\n");
	memset(data, 'b', size);
	// klog_clear();
	CHECK_NOT_M1(ioctl(d, IOCTL_DEMO_READ));
	// klog_show();
	show_data(data, size);
	CHECK_NOT_M1(ioctl(d, IOCTL_DEMO_UNMAP));
	show_data(data, size);
}

const int csize=64000;
static char data_ds[csize];
int main(int argc, char** argv, char** envp) {
	// file name of the device
	const char *filename="/dev/demo";

	d=CHECK_NOT_M1(open(filename, O_RDWR));
	if (do_stack_test) {
		char data[csize + getpagesize() * 10];
		do_test(data, csize, "stack");
	}
	if (do_valloc_test) {
		char *data=(char *)valloc(csize);
		do_test(data, csize, "valloc");
	}
	if (do_malloc_test) {
		char *data=(char *)malloc(csize);
		do_test(data, csize, "malloc");
	}
	if (do_dataseg_test) {
		do_test(data_ds, csize, "dataset");
	}
	if (do_stacks_test) {
		static char data[csize];
		do_test(data, csize, "stacks");
	}
	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
