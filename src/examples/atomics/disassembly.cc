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
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * This example compares the adding of integers and the adding of atomics
 * This next line is to avoid optimization which will make
 * the second loop go away all together...
 * this is not really needed since we use a compiler barrier instead (see below).
 * EXTRA_COMPILE_FLAGS_AFTER_DUMMY=-O0
 */

int main(int argc, char** argv, char** envp) {
	int number = 0;
	printf("number is %d\n", number);
	int result = __sync_add_and_fetch(&number, 1);
	// number = number + 1;
	// int result = number;
	printf("number is %d\n", number);
	printf("result is %d\n", result);
	return EXIT_SUCCESS;
}
