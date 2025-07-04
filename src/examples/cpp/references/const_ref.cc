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
 * This example demonstrates const references, pointers, and variables in C++.
 * It shows that when variables, pointers, or references are declared const,
 * any attempt to modify them will result in compilation errors. The commented
 * code shows the types of modifications that would fail to compile, illustrating
 * how const provides compile-time protection against unwanted modifications.
 */

int main() {
	// const int i=5;
	// const int* pi=&i;
	// const int& ri=i;
	// next lines produce compilation errors...
	// i=8;
	// *pi=7;
	// ri=8;
	return EXIT_SUCCESS;
}
