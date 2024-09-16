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

#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

#define NUM_CALLS 1000000000

extern int static_function(int x);

int main() {
	void *handle;
	int (*dso_function)(int);
	struct timespec start, end;
	long long int dso_time, static_time;

	// Load the shared library
	handle = dlopen("./libshared.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "Error loading shared library: %s\n", dlerror());
		return 1;
	}

	// Get the DSO function
	dso_function = dlsym(handle, "dso_function");
	if (!dso_function) {
		fprintf(stderr, "Error getting DSO function: %s\n", dlerror());
		dlclose(handle);
		return 1;
	}

	// Benchmark DSO function
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(int i = 0; i < NUM_CALLS; i++) {
		dso_function(i);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	dso_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

	// Benchmark static function
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(int i = 0; i < NUM_CALLS; i++) {
		static_function(i);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	static_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

	printf("Time for %d DSO function calls: %lld ns\n", NUM_CALLS, dso_time);
	printf("Time for %d static function calls: %lld ns\n", NUM_CALLS, static_time);
	printf("Difference per call: %.2f ns\n", (double)(dso_time - static_time) / NUM_CALLS);

	dlclose(handle);
	return 0;
}
