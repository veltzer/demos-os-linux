/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <unistd.h>	// for mincore(2), getpagesize(2)
#include <sys/mman.h>	// for mincore(2)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for posix_memalign(3), malloc(3), free(3), EXIT_SUCCESS
#include <string.h>	// for memset(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_NOT_M1(), CHECK_ZERO()

/*
 * This is a demo of the mincore(2) syscall.
 * Using this system call you can determine whether or not a pointer
 * points to a page which is already realized as a physical page attached
 * to your process or not. This will allow you to determine whether or not
 * you will create a minor or major page fault when accessing the page.
 *
 * NOTES:
 * - the address passed to mincore(2) should be on a page boundary. That
 * is why posix_memalign(3) is used.
 * - if you remove the memset the pages are not there. If you add the memset
 * then the pages are there.
 */

enum Method {
	METHOD_POSIX_MEMALIGN,
	METHOD_MALLOC,
	METHOD_MMAP
};

int main(int argc, char** argv, char** envp) {
	const unsigned int pagenum=100;
	const unsigned int pagesize=getpagesize();
	const unsigned int size=pagenum*pagesize;
	// const Method method=METHOD_POSIX_MEMALIGN;
	const Method method=METHOD_MALLOC;
	unsigned char vec[pagenum];
	// TRACE("pagenum is %u",pagenum);
	// TRACE("pagesize is %u",pagesize);
	// TRACE("size is %u",size);
	void* mincore_pointer;
	unsigned int memset_size;
	void* free_pointer;
	char* mp;
	switch(method) {
	case METHOD_POSIX_MEMALIGN:
		void* p;
		CHECK_ZERO(posix_memalign(&p, pagesize, size));
		mincore_pointer=p;
		memset_size=size;
		free_pointer=p;
		break;
	case METHOD_MALLOC:
		mp=(char*)CHECK_NOT_NULL(malloc(size));
		CHECK_NOT_NULL(malloc(10));
		// mincore_pointer=mp+pagesize-(unsigned long)mp%pagesize;
		mincore_pointer=mp-(unsigned long)mp%pagesize;
		// memset_size=size-(unsigned long)mp%pagesize;
		memset_size=size+pagesize;
		free_pointer=(void*)mp;
		break;
	case METHOD_MMAP:
		break;
	}
	// memset(mincore_pointer,0,memset_size);
	CHECK_NOT_M1(mincore(mincore_pointer, memset_size, vec));
	free(free_pointer);
	// print the vector
	for(unsigned int i=0; i<pagenum; i++) {
		char c;
		if(vec[i] & 1) {
			c='1';
		} else {
			c='0';
		}
		printf("%c", c);
	}
	printf("\n");
	return EXIT_SUCCESS;
}
