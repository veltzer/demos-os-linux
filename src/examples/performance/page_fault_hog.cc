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
#include <iostream>	// for std::cout, std::endl
#include <sys/mman.h>	// for mmap(2), munmap(2)
#include <string.h>	// for memset(3)
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_ZERO()

/*
 * This example creates lots of minor page faults...
 */

void minor_fault_hog_function() {
	for(unsigned int i=0; i<1000000; i++) {
		int flags=MAP_PRIVATE | MAP_ANONYMOUS;
		const unsigned int size=1024*1024;
		void* p=CHECK_NOT_NULL(mmap(NULL, size, PROT_READ | PROT_WRITE, flags, -1, 0));
		// memset(p, 0, size);
		for(unsigned int j=0; j<size; j+=2) {
			((char*)p)[j]=j;
		}
		CHECK_ZERO(munmap(p, size));
	}
}

int main(int argc, char** argv, char** envp) {
	minor_fault_hog_function();
	return EXIT_SUCCESS;
}
