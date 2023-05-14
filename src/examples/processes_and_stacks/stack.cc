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
#include <string.h>	// for memset(3)
#include <stdio.h>	// for printf(3)
#include <sys/resource.h>	// for getrlimit(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows the stack limitations of a regular program.
 *
 * Notes:
 * - this is why it's a bad idea to store stuff on the stack.
 * - note that without the memset there is no problem since the
 *	memory is not really allocated.
 */

// print the current limit for stack usage for the process
void print_limit(void) {
	struct rlimit rlim;
	CHECK_NOT_M1(getrlimit(RLIMIT_STACK, &rlim));
	printf("rlim_cur is %ld\n", rlim.rlim_cur);
	printf("rlim_max is %ld\n", rlim.rlim_max);
}

// a single function using lots of stack. Either fake (without
// memset) or real (with memset)
void func(int m, bool mems) {
	const unsigned int size=m*1024*1024;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
	char data[size];
#pragma GCC diagnostic pop
	if(mems) {
		memset(data, 0, size);
	}
}

// a loop trying to use more and more stack
void loop(bool mems) {
	printf("mems is %d\n", mems);
	for(unsigned int m=1; m<50; m++) {
		printf("m is %d\n", m);
		func(m, mems);
	}
}

int main(int argc, char** argv, char** envp) {
	print_limit();
	// example of non dumping the stack...
	loop(false);
	// this will dump core...
	loop(true);
	return EXIT_SUCCESS;
}
