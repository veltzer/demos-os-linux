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
#include <stdlib.h>	// EXIT_SUCCESS
#include <stdio.h>	// printf(3), scanf(3)
#include <err_utils.h>	// for CHECK_INT()

/*
 * This example shows how to allocate memory using what's called a VLA - Variable Length Array.
 * This is available via GCC, ISO C99, C90 or C++14
 * This memory is allocated on the stack, and this example demonstrates this althrough the gcc documentation
 * does not state so explicitly.
 * You can just run this example and see that the address of the variable length array is the same as the stack.
 *
 * This feature is therefore similar to alloca(3) but more elegant.
 * This also means that this feature is subject to the same constraints as alloc(3). For instance, you
 * should not keep a pointer to such an a variable sized array and try to use it after the function or
 * scope in which it was defined goes out of scope.
 *
 * Jumping or breaking out of the scope of the array name deallocates the storage.
 * Jumping into the scope is not allowed; you get an error message for it.
 * This is epecially important for longjmp(3) and exception handling in C++.
 *
 * Note that "pedantic" gcc mode does not allow the "vla" features and so we need to supply the
 * "-Wno-vla" flag below.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-vla
 *
 * References:
 * - https://gcc.gnu.org/onlinedocs/gcc/Variable-Length.html
 * - https://www.geeksforgeeks.org/variable-length-arrays-in-c-and-c/
 * - https://stackoverflow.com/questions/2863347/declaring-the-array-size-with-a-non-constant-variable
 */

void function(int len) {
	int array[len];
	for(int i=0; i<len; i++) {
		array[i]=i;
	}
	int sum=0;
	for(int i=0; i<len; i++) {
		sum+=array[i];
	}
	printf("sum is %d\n", sum);
	printf("array location is %p\n", (void*)array);
}

void print_stack_adr() {
	int a;
	printf("stack location is %p\n", (void*)&a);
}

int main() {
	print_stack_adr();
	int len;
	printf("Enter the size of the array: ");
	CHECK_INT(scanf("%d", &len), 1);
	function(len);
	return EXIT_SUCCESS;
}
