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
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for sleep(3)
#include <papi.h>	// for PAPI_* functions
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO()

/*
 * Demo for using the PAPI performance counters library.
 *
 * References:
 * http://icl.cs.utk.edu/papi/index.html
 *
 * EXTRA_LINK_FLAGS=-lpapi
 */

void print_counters(long long* arr, int num) {
	for(int i=0; i<num; i++) {
		printf("%d: %llu\n", i, arr[i]);
	}
}

int main(int argc, char** argv, char** envp) {
	printf("starting up...\n");
	int counters=PAPI_num_hwctrs();
	printf("PAPI_num_counters() returned %d\n", counters);
	int components=PAPI_num_components();
	printf("PAPI_num_components() returned %d\n", components);

	// array for counters
	//long long* arr=new long long[counters];
	float rtime, ptime, ipc;
	//float mflops;
	long long ins;
	//long long flpops;

	int ret=PAPI_ipc(&rtime, &ptime, &ins, &ipc);
	printf("PAPI_ipc() returned:\n");
	printf("\trtime=%f\n", rtime);
	printf("\tptime=%f\n", ptime);
	printf("\tins=%lld\n", ins);
	printf("\tipc=%f\n", ipc);
	printf("\tret=%d\n", ret);

	/* need to adjust to new API
	ret=PAPI_flops(&rtime, &ptime, &flpops, &mflops);
	printf("PAPI_flops() returned:\n");
	printf("\trtime=%f\n", rtime);
	printf("\tptime=%f\n", ptime);
	printf("\tflpops=%lld\n", flpops);
	printf("\tmflops=%f\n", mflops);
	printf("\tret=%d\n", ret);
	*/

	/* need to adjust to new API
	PAPI_read_ts(arr, counters);
	printf("ret is %d\n", ret);
	print_counters(arr, counters);
	CHECK_ZERO(sleep(1));
	ret=PAPI_read_counters(arr, counters);
	printf("ret is %d\n", ret);
	print_counters(arr, counters);
	*/
	return EXIT_SUCCESS;
}
