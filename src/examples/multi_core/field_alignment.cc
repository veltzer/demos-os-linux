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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3)
#include <err_utils.h>	// for CHECK_ASSERT()

/*
 * This example shows that the compiler aligns fields to 4/8
 * bytes according to whether you are on a on a 32/64 bit CPU.
 * It also shows that malloc always returns aligned memory.
 * It also shows that parameters to functions are 4/8 aligned.
 *
 * The result is that all of your fields are aligned on a 4/8
 * alignment which results in better performance.
 *
 * References:
 * http://www.geeksforgeeks.org/structure-member-alignment-padding-and-data-packing/
 * http://en.wikipedia.org/wiki/Data_structure_alignment
 */

struct small {
	char c1;
	char c2;
};

struct medium {
	char c1;
	int i1;
	char c2;
};

struct medium2 {
	char c1;
	char c2;
	int i1;
};

void myfunction(char c1, int i1, char c2) {
	CHECK_ASSERT((unsigned long)&c1%4==0);
	CHECK_ASSERT((unsigned long)&i1%4==0);
	CHECK_ASSERT((unsigned long)&c2%4==0);
}

int main(int argc, char** argv, char** envp) {
	printf("sizeof(small)=%zd\n", sizeof(small));
	printf("sizeof(medium)=%zd\n", sizeof(medium));
	printf("sizeof(medium2)=%zd\n", sizeof(medium2));
	for(int i=1; i<200; i++) {
		char* p=(char*)malloc(i);
		CHECK_ASSERT(p!=NULL);
		CHECK_ASSERT((unsigned long)p%4==0);
	}
	return EXIT_SUCCESS;
}
