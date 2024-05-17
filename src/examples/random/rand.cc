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
#include <stdlib.h>	// for srandom(3), rand(3), EXIT_SUCCESS, EXIT_FAILURE, atoi(3), srand(3), rand_r(3)
#include <stdio.h>	// for fprintf(3), printf(3), stderr:object

/*
 * This example demos the pseudo random number generator
 * implemented by the glibc functions srand(3), rand(3) etc.
 *
 * Notes:
 * - since this is a pseudo random number generator
 * and it is always initialized using the same seed, then
 * it will always produce the EXACT series of "random"
 * numbers.
 * - This is great especially if you want determinism and
 * reproducibility of your code.
 * - This is bad if you adopt a security standpoint.
 * - the default seed is 1 (that is if you don't initialize the seed).
 * - both srandom(3) and srand(3) can be used intechangebly to seed this generator.
 */

int main(int argc, char** argv) {
	if(argc!=6) {
		fprintf(stderr, "%s: usage %s [num] [be_thread_safe] [use_srandom] [use_seed] [seed]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: seed=1 is the default seed\n", argv[0]);
		return EXIT_FAILURE;
	}
	printf("RAND_MAX is %20d\n", RAND_MAX);
	printf("2^31-1 is %22lld\n", (2ll<<30)-1);
	unsigned int num=atoi(argv[1]);
	int be_thread_safe=atoi(argv[2]);
	int use_srandom=atoi(argv[3]);
	int use_seed=atoi(argv[4]);
	unsigned int seed=(unsigned int)atoi(argv[5]);
	if(use_seed) {
		if(!be_thread_safe) {
			if(use_srandom) {
				printf("initializing seed to %u using srandom\n", seed);
				srandom(seed);
			} else {
				printf("initializing seed to %u using srand\n", seed);
				srand(seed);
			}
		}
	}
	for(unsigned int i=0; i<num; i++) {
		if(be_thread_safe) {
			printf("i=%d, rand_r(&seed)=%d\n", i, rand_r(&seed));
		} else {
			printf("i=%d, rand()=%d\n", i, rand());
		}
	}
	return EXIT_SUCCESS;
}
