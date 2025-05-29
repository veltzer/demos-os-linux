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
 * This example shows the pointer values in various segments...
 */

int var_data=4;
const char* buf_data="this is the data buffer";

int main() {
	int var_stack=5;
	const char* buf_stack="this is a buffer";
	printf("var_stack is %p\n", (void*)&var_stack);
	printf("buf_stack is %p\n", buf_stack);
	printf("var_data is %p\n", (void*)&var_data);
	printf("buf_data is %p\n", buf_data);
	return EXIT_SUCCESS;
}
