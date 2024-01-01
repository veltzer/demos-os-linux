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

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for malloc(3), EXIT_SUCCESS
#include <sys/mman.h>	// for mlock(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()

/*
 * This example shows that when the OS finally catches
 * you on an illegal access then the pointer is a page
 * bounded pointer.
 *
 * Remember that it is actually the CPU/MMU that catches
 * you for accessing the illegal address. The CPU jumps
 * from your program to the OS and the OS responds by
 * sending you a SIGSEGV (at least on UNIX).
 */

int main(int argc, char** argv, char** envp) {
	CHECK_NOT_M1(mlockall(MCL_CURRENT|MCL_FUTURE));
	char* p=(char*)CHECK_NOT_NULL(malloc(20));
	//CHECK_NOT_M1(mlock(p,128*1024));
	int i=0;
	while(true) {
		printf("i is %i, p+i is %p\n", i, p+i);
		p[i]=0;
		i+=1;
	}
	return EXIT_SUCCESS;
}
