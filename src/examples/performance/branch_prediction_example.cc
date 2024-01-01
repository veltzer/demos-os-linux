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
#include <stdlib.h>	// for EXIT_SUCCESS, rand(3)
#include <iostream>	// for std::cout, std::endl
#include <algorithm>	// for std::rand(), std::sort()
#include <time.h>	// for clock(3), clock_t:type, CLOCKS_PER_SEC

/*
 * This is an example which shows a real difference in performance when
 * having and not having correct branch prediction by the CPU.
 *
 * Notes:
 * - no amount of optimization hints to GCC (see below) could remove
 * the huge differences seen by this example.
 * - the Intel compiler will do much better here since it will exchange
 * the two loop, reduce cache misses and vectorize everything. so with
 * the intel compiler you should see no difference in performance.
 *
 * References:
 * http://stackoverflow.com/questions/11227809/why-is-processing-a-sorted-array-faster-than-an-unsorted-array
 *
 * EXTRA_COMPILE_FLAGS=-mtune=corei7 -O3 -ftree-vectorize
 * EXTRA_COMPILE_FLAGS=-O3
 */

void do_work(int* data, unsigned int arraySize, const char* msg) {
	clock_t start = clock();

	long long sum = 0;
	for(unsigned i = 0; i < 100000; ++i) {
		// Primary loop
		for(unsigned c = 0; c < arraySize; ++c) {
			if (data[c] >= 128)
				sum += data[c];
		}
	}
	clock_t end = clock();
	double elapsedTime = static_cast<double>(end - start) / CLOCKS_PER_SEC;

	std::cout << msg << std::endl;
	std::cout << elapsedTime << std::endl;
	std::cout << "sum = " << sum << std::endl;
}

void do_work_no_if(int* data, unsigned int arraySize, const char* msg) {
	clock_t start = clock();

	long long sum = 0;
	for(unsigned i = 0; i < 100000; ++i) {
		// Primary loop
		for(unsigned c = 0; c < arraySize; ++c) {
			int t = (data[c] - 128) >> 31;
			sum += ~t & data[c];
		}
	}
	clock_t end = clock();
	double elapsedTime = static_cast<double>(end - start) / CLOCKS_PER_SEC;

	std::cout << msg << std::endl;
	std::cout << elapsedTime << std::endl;
	std::cout << "sum = " << sum << std::endl;
}

int main(int argc, char** argv, char** envp) {
	// Generate data
	const unsigned int arraySize = 32768;
	int data[arraySize];
	for(unsigned c = 0; c < arraySize; ++c)
		data[c] = std::rand() % 256;
	// run unsorted and then sorted
	do_work(data, arraySize, "unsorted with if");
	do_work_no_if(data, arraySize, "unsorted no if");
	std::sort(data, data + arraySize);
	do_work(data, arraySize, "sorted with if");
	do_work_no_if(data, arraySize, "sorted no if");
	return EXIT_SUCCESS;
}
