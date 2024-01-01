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
#include <pthread_utils.h>	// for gettid()
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an example of how to use the random_r(3) family of functions.
 *
 * NOTES:
 * - the random_data given to initstate_r must be zeroed in some versions of the libc library
 * although I could find no reference to this in the documentation.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage %s [num]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	unsigned int num=atoi(argv[1]);

	printf("RAND_MAX is %20d\n", RAND_MAX);
	printf("2^31-1 is %22lld\n", (2ll<<30)-1);

	const unsigned int state_len=32;
	char state[state_len];
	struct random_data data={0};
	CHECK_NOT_M1(initstate_r(gettid(), state, state_len, &data));

	for(unsigned int i=0; i<num; i++) {
		int32_t result;
		CHECK_NOT_M1(random_r(&data, &result));
		printf("i=%d, random_r(&data, &result)=%d\n", i, result);
	}
	return EXIT_SUCCESS;
}
