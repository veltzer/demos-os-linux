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
#include <stdio.h> // for printf(3)
#include <stdlib.h> // for EXIT_SUCCESS, exit(3)

/*
 * Must implement the add function, as main actually calls it...
 */
int add(int a,int b) {
	return a+b;
}

// References:
// http://gcc.gnu.org/ml/gcc-help/2003-07/msg00232.html

// this is a must since we need dynamic linking to be able to call printf and the like
const char my_interp[] __attribute__((section(".interp"))) = "/lib/ld-linux.so.2";

// our main function should not return as it has noone to return to
// (remember that libc is not there)
// if you actually return you will cause a segfault
void mymain(int argc,char** argv,char** envp) __attribute__ ((noreturn));
void mymain(int argc,char** argv,char** envp) {
	int i;
	printf("This is the add library.\n");
	printf("argc is %d\n",argc);
	printf("Called as:\n");
	for(i=0;i<argc;i++) {
		printf(" %s",argv[i]);
	}
	printf("\n");
	exit(EXIT_SUCCESS);
}
