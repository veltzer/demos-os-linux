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
 * A simple speed test app to show the difference between software and
 * hardware breakpoints.
 *
 * You use this example by running it in gdb once under "break" and once
 * under "hbreak" and show the difference in speed.
 *
 * Compare the following:
 * 1) break /home/mark/git/demos-linux/src/examples/debugging/speed_test.cc:43
 * if sum=8000
 * 2) break /home/mark/git/demos-linux/src/examples/debugging/speed_test.cc:43
 * 3) hbreak /home/mark/git/demos-linux/src/examples/debugging/speed_test.cc:43
 *
 * EXTRA_COMPILE_FLAGS=-g3
 */

int main() {
	volatile long sum=0;
	for(long i=0; i<1000000; i++) {
		for(long j=0; j<1000000000; j++) {
			sum+=i+j;
		}
		printf("%ld: one step\n", i);
	}
	printf("sum is %ld\n", sum);
	return EXIT_SUCCESS;
}
