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
#include <strings.h>	// for bzero(3)
#include <alloca.h>	// for alloca(3)
#include <stdio.h>	// for printf(3)
#include <sys/time.h>	// for setrlimit(2)
#include <sys/resource.h>	// for setrlimit(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows how to allocate space on the stack using the alloca(3) function call.
 * This function is actually inlined assembly that never fails and only increments the stack
 * pointer and so is VERY VERY VERY fast. On the other hand it has it's own issues.
 *
 * On a standard linux machine this test will fail when reaching about 10M of ram. If you want
 * more then you can use setrlimit with the stack size you want to get more or use ulimit(1)
 * on the command line before running your program.
 *
 * Notice that once you set a limit using ulimit(1) on the command line then using this program
 * would fail since the limit was already set and so cannot be made higher (ulimit security).
 *
 * Notes:
 * - even if you did not zero the memory you got from alloca you would still get a stack
 * violation exception since in ubuntu 6.10 and onwards a stack protector is
 * automatically enabled for any thread calling alloca.
 */

void do_something() {
	printf("something...\n");
}

void my_func(size_t size) {
	char* p=(char*)alloca(size);
	// this will show that we can use the memory
	p[0]=0;
	// this will REALLY allocate and will cause a segfault...
	// bzero(p,size);
	// this will actually work, creating a "hole" in the stack...
	do_something();
}

int main(int argc, char** argv, char** envp) {
	unsigned int stack_size_to_set=50*1024*1024;
	struct rlimit rlim;
	rlim.rlim_max=stack_size_to_set;
	rlim.rlim_cur=stack_size_to_set;
	CHECK_NOT_M1(setrlimit(RLIMIT_STACK, &rlim));
	const size_t size=1024*1024;
	for(unsigned int i=0; i<100; i++) {
		printf("trying to allocate %zd\n", i*size);
		my_func(size*i);
	}
	return EXIT_SUCCESS;
}
