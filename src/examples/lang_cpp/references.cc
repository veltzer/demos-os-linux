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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example explores the use of references.
 *
 * Things to notice:
 * - the reference is not even stored in it's own place on the stack, or anywhere.
 *	It is a compile time figment of the imagination.
 * - when changing the reference you change the actual value, much like you would do
 *	with a pointer. The syntax is different though. Instead of doing *p=7 you simply
 *	do p=7.
 * - Changing the reference DOES NOT change what value of the reference (it s a pointer
 *	after all) but rather what the reference is pointing to.
 *
 */

int main(int argc, char** argv, char** envp) {
	int i=7;
	int u=20;
	int& j=i;
	int* k=&i;
	printf("i is %d\n", i);
	printf("j is %d\n", j);
	printf("k is %p\n", (void*)k);
	printf("&j is %p\n", (void*)&j);
	printf("&j is %p\n", (void*)&i);
	printf("&k is %p\n", (void*)&k);
	printf("*k is %d\n", *k);
	j=8;
	printf("i is %d\n", i);
	printf("j is %d\n", j);
	printf("k is %p\n", (void*)k);
	printf("&j is %p\n", (void*)&j);
	printf("&j is %p\n", (void*)&i);
	printf("&k is %p\n", (void*)&k);
	printf("*k is %d\n", *k);
	*k=9;
	printf("i is %d\n", i);
	printf("j is %d\n", j);
	printf("k is %p\n", (void*)k);
	printf("&j is %p\n", (void*)&j);
	printf("&j is %p\n", (void*)&i);
	printf("&k is %p\n", (void*)&k);
	printf("*k is %d\n", *k);
	j=u;
	printf("i is %d\n", i);
	printf("j is %d\n", j);
	printf("k is %p\n", (void*)k);
	printf("&j is %p\n", (void*)&j);
	printf("&j is %p\n", (void*)&i);
	printf("&k is %p\n", (void*)&k);
	printf("*k is %d\n", *k);
	return EXIT_SUCCESS;
}
