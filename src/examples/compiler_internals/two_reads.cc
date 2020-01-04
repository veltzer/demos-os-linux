/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that two consecutive reads of the same value
 * are optimized away by the compiler.
 *
 * You can see this by building the disassembly file for this
 * example.
 *
 * Notice that the reads to the volatile are not optimized away
 * because the compiler treats the volatile with suspicion and
 * fears it may change value.
 */

void __attribute__((noinline)) print_address_of(int* ptr) {
	printf("ptr is %p\n", ptr);
}

void __attribute__((noinline)) printit(int val) {
	printf("val is %d\n", val);
}

int main(int argc, char** argv, char** envp) {
	volatile int i=6;
	int j=8;
	int a;
	// to make the compiler actually instantiate a and j
	print_address_of(&a);
	print_address_of(&j);
	a=i;
	a=i;
	printit(a);
	a=j;
	a=j;
	printit(a);
	return EXIT_SUCCESS;
}
