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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This demos how to use compiler attributes to make sure we don't mix pointers that
 * refer to different memory spaces...
 *
 * Regular software developers usually do not use these features...
 */

// this is taken from the kernel sources (include/linux/compiler.h)
// currently this does not seem to work

// #define DO_IT

#ifdef DO_IT
#define __space1 __attribute__((noderef, address_space(1)))
#define __space2 __attribute__((noderef, address_space(2)))
#else	// DO_IT
#define __space1
#define __space2
#endif	// DO_IT
void func1(void *__space1 pointer) {
}

void *__space1 malloc1(unsigned int size) {
	return NULL;
}

void func2(void *__space2 pointer) {
}

void *__space2 malloc2(unsigned int size) {
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	return EXIT_SUCCESS;
}
