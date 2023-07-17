/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <err_utils.h>	// for CHECK_NOT_VOIDP
#include <unistd.h>	// for sleep(3), getpagesize(2)
#include <sys/mman.h>	// for mmap(2), MAP_FAILED
#include <stdlib.h>	// for EXIT_SUCCESS
#include <multiproc_utils.h>	// for my_system()
#include <unistd.h>	// for getpid(2)

/*
 * This example show two to allocate memory using the 'mmap(2)' system call.
 * We will then show how when we slowly use more and more of this newly allocated
 * memory, we create minor page faults.
 */

int main(int argc, char** argv, char** envp) {
	const unsigned long length = 4*1024*1024*1024L;
	const int pagesize = getpagesize();
	// allocate the memory
	char* p=(char*)CHECK_NOT_VOIDP(mmap(
		NULL,
		length,
		PROT_READ|PROT_WRITE,
		MAP_ANONYMOUS|MAP_PRIVATE,
		-1,
		0
	), MAP_FAILED);
	// slowly use the memory
	while(true) {
		for(int i=0;i<1000;i++) {
			*p=0;
			p+=pagesize;
		}
		printf("oops, I did it again! (britney spears style)...");
		sleep(1);
	}
	return EXIT_SUCCESS;
}
