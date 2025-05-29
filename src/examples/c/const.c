/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
 * This example explores the "const" keyword in C.
 */

void print_it(const int* p) __attribute__((noinline));
void print_it(const int* p) {
	printf("*p=%d\n", *p);
}

int main() {
	const int a=5;
	printf("a=%d\n", a);
	print_it(&a);
	// this next line if, enabled will cause a compilation error
	//a=6;
	// you can still change the value inside the cell if you really want to, this is C after all
	int* p=(int*)&a;
	*p=6;
	// Can you understand why the two following prints print different things?
	printf("a=%d\n", a);
	print_it(&a);
	return EXIT_SUCCESS;
}
