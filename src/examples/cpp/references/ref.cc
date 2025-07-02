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
 * This example demonstrates basic C++ reference usage and behavior.
 * It shows how references create aliases to variables, meaning that
 * modifications to the reference affect the original variable and
 * vice versa. References must be initialized when declared and
 * cannot be reassigned to refer to different variables.
 */

int main() {
	int i=5;
	int& ri=i;
	printf("i is %d, ri is %d\n", i, ri);
	ri=6;
	printf("i is %d, ri is %d\n", i, ri);
	i=7;
	printf("i is %d, ri is %d\n", i, ri);
	return EXIT_SUCCESS;
}
