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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for vprintf(3)
#include <assert.h>	// for assert(3)
#include <time.h>	// for clock(3), CLOCKS_PER_SEC

/**
 * This is an example of how to do min() and max() macros without branches.
 * Remember why branching is bad for performance? Because it hinders prediction.
 *
 * References:
 * - https://www.reddit.com/r/learnprogramming/comments/u6iq0/minmax_without_branching_c/
 * - https://stackoverflow.com/questions/37636192/find-max-among-32-bit-integers
 * - https://stackoverflow.com/questions/1969809/x86-max-min-asm-instructions
 */

// Using functions

int f_min_a(int a, int b) {
	return (a < b ? a:b);
}
int f_max_a(int a, int b) {
	return (a > b ? a:b);
}
int f_min_b(int a, int b) {
	return b + ((a-b) & (a-b)>>31);
}
int f_max_b(int a, int b) {
	return b + ((a-b) & (a-b)>>31);
}

// Using macro

#define min_a(a, b) ((a) < (b) ? (a):(b))
#define max_a(a, b) ((a) > (b) ? (a):(b))
#define min_b(a, b) ((b) + (((a)-(b)) & ((a)-(b))>>31))
#define max_b(a, b) ((a) - (((a)-(b)) & ((a)-(b))>>31))

int main() {
	clock_t start_a=clock();
	long sum_a=0;
	for(int i=0; i<10000; i++) {
		for(int j=0; j<10000; j++) {
			sum_a+=min_a(i, j);
		}
	}
	clock_t end_a=clock();
	clock_t start_b=clock();
	long sum_b=0;
	for(int i=0; i<10000; i++) {
		for(int j=0; j<10000; j++) {
			sum_b+=min_b(i, j);
		}
	}
	clock_t end_b=clock();
	assert(sum_a==sum_b);
	double elapsed_a = ((double)(end_a - start_a))/CLOCKS_PER_SEC;
	double elapsed_b = ((double)(end_b - start_b))/CLOCKS_PER_SEC;
	printf("Time measured a: %.3f seconds.\n", elapsed_a);
	printf("Time measured b: %.3f seconds.\n", elapsed_b);
	return 0;
}
