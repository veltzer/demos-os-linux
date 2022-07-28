/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for malloc(3), EXIT_SUCCESS, EXIT_FAILURE, rand(3), atoi(3)
#include <stdio.h>	// for printf(3), fprintf(3), stderr

/*
 * This is a sample which misses the cache on purpose...
 *
 * test this with:
 * perf stat -e cache-misses ./src/examples/performance/cache_misser.elf [size] [times]
 * try 104857600 as the value (100MB)
 * note that when you pass times=0 you will still get lots of cache misses.
 * Those are the cache misses to materialize the memory. Do it one time with times=0
 * and then whenever you increase times you will get the extra cache misses you are
 * generating.
 * make the value bigger to see more misses...
 *
 * TODO:
 * - allocate the memory using mmap(2) and MAP_POPULATE before starting the loop in * order to get number of cache misses lower.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [size] [times]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: 104857600 100000000\n", argv[0]);
		fprintf(stderr, "%s: measure with: perf stat -e cache-misses ./src/examples/performance/cache_misser.elf [size] [times]\n", argv[0]);
		return EXIT_FAILURE;
	}
	printf("RAND_MAX is %d\n", RAND_MAX);
	unsigned int size=atoi(argv[1]);
	unsigned int times=atoi(argv[2]);
	char* p=(char*)malloc(size);
	for(unsigned int i=0; i<size; i++) {
		p[i]=i%256;
	}
	long long sum=0;
	for(unsigned int i=0; i<times; i++) {
		int pos=rand()%size;
		sum+=p[pos];
	}
	printf("sum is %lld\n", sum);
	return EXIT_SUCCESS;
}
