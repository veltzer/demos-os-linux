/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for srand(3), rand(3), EXIT_SUCCESS, EXIT_FAILURE, fprintf(3), stderr:symbol
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)

/*
 * This example abuses the CPU as far as branch prediction goes...
 *
 * Test this application with:
 * perf stat -e branch-misses ./src/examples/performance/branch_prediction_mispredict.elf 1|0
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [miss]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	int miss=atoi(argv[1]);
	srand(getpid());
	long long sum=0;
	for(unsigned int i=0; i<100000000; i++) {
		if(miss) {
			if(rand()%2==0) {
				sum+=i*i;
			} else {
				sum-=i*i;
			}
		} else {
			rand();
			if(i<50000000) {
				sum+=i*i;
			} else {
				sum-=i*i;
			}
		}
	}
	printf("the sum is %lld\n", sum);
	return EXIT_SUCCESS;
}
