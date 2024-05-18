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
#include <stdbool.h>	// for bool

/*
 * This example shows one way to find if the stack goes up or down
 */

bool called(int* b) __attribute__((noinline));
bool called(int* b) {
	int a;
	return &a < b;
}

bool stack_is_up() __attribute__((noinline));
bool stack_is_up() {
	int b;
	return called(&b);
}

bool stack_is_up_2() __attribute__((noinline));
bool stack_is_up_2() {
	int a, b;
	return &a+1==&b;
}

int main() {
	printf("stack_is_up is %d\n", stack_is_up());
	printf("stack_is_up_2 is %d\n", stack_is_up_2());
	return EXIT_SUCCESS;
}
