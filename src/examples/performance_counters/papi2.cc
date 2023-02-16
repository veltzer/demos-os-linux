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
#include <papi.h>	// for PAPI_* functions
#include <unistd.h>	// for sleep(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for exit(3), atoi(3), EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_INT(), CHECK_ZERO()

/*
 * Demo for using the PAPI performance counters library.
 *
 * References:
 * http://icl.cs.utk.edu/papi/index.html
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpapi
 */

long long dummy_work(int limit) {
	int count=0;
	for(int j=0; j<limit; j++) {
		for(int i=0; i<limit; i++) {
			count+=dummy_work(limit-1);
			// count+=i;
		}
	}
	return count;
}

int main(int argc, char** argv, char** envp) {
	//needs to be adjusted for new API
	//const unsigned int NUM_EVENTS=2;
	//int Events[NUM_EVENTS]={PAPI_TOT_INS, PAPI_TOT_CYC};
	//long_long values[NUM_EVENTS];
	/* Start counting events */
	//CHECK_INT(PAPI_start_counters(Events, NUM_EVENTS), PAPI_OK);
	/* Do some computation here */
	// CHECK_ZERO(sleep(atoi(argv[1])));
	//dummy_work(atoi(argv[1]));
	/* Stop counting events */
	//CHECK_INT(PAPI_stop_counters(values, NUM_EVENTS), PAPI_OK);
	//printf("value 0 is %lld\n", values[0]);
	//printf("value 1 is %lld\n", values[1]);
	return EXIT_SUCCESS;
}
