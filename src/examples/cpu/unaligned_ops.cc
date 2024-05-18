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
#include <stdio.h>	// for snprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3)
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <err_utils.h>	// for CHECK_NOT_NEGATIVE()
#include <sched_utils.h>// for sched_run_priority()

/*
 * This example tries to abuse the cpu by doing lots of integral
 * operations on addresses which are not 4 byte aligned and showing
 * the difference in performace between these and properly aligned
 * operations.
 *
 * Results:
 * It seems that on modern Intel platforms data alignment is not an issue.
 *
 * References:
 * - http://lemire.me/blog/archives/2012/05/31/data-alignment-for-speed-myth-or-reality/
 *
 * TODO:
 * - in the referenced blog there is a note saying that some patterns
 * will cause delays. Reproduce these patterns (the blog has some code
 * attached that will help in that effort).
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

static int diff;
static char* buf;
const unsigned int num_of_ints=1000;

void* func(void*) {
	char name[256];
	CHECK_NOT_NEGATIVE(snprintf(name, 256, "ops with alignment %d", diff));
	measure m;
	measure_init(&m, name, 1);
	measure_start(&m);
	int* pi=(int*)(buf+diff);
	for(int j=0; j<1000000; j++) {
		for(unsigned int i=0; i<num_of_ints; i++) {
			pi[i]+=i*i;
		}
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

int main() {
	const unsigned int size_to_alloc=num_of_ints*sizeof(int)+500;
	buf=(char*)malloc(size_to_alloc);
	for(unsigned int i=0; i<size_to_alloc; i++) {
		buf[i]=rand()%256;
	}
	diff=0;
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	diff=1;
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	diff=2;
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	diff=3;
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	free(buf);
	return EXIT_SUCCESS;
}
