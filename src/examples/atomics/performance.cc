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
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * This example compares the adding of integers and the adding of atomics
 * This next line is to avoid optimization which will make
 * the second loop go away all together...
 * this is not really needed since we use a compiler barrier instead (see below).
 * EXTRA_COMPILE_FLAGS_AFTER_DUMMY=-O0
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [attempts]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	int attempts=atoi(argv[1]);
	int counter;
	volatile int vcounter;
	measure m;

	counter=0;
	measure_init(&m, "atomic adds", attempts);
	measure_start(&m);
	for(int i=0; i<attempts; i++) {
		__sync_add_and_fetch(&counter, 1);
	}
	measure_end(&m);
	measure_print(&m);

	counter=0;
	measure_init(&m, "regular adds (big barrier)", attempts);
	measure_start(&m);
	for(int i=0; i<attempts; i++) {
		counter++;
		// this is a compiler barrier that forces the compiler
		// to actually instantiate a loop here...
		asm volatile ("" ::: "memory");
	}
	measure_end(&m);
	measure_print(&m);

	counter=0;
	//print the address of counter to force the compiler
	//to allocate it on the stack
	printf("address of counter is %p\n", &counter);
	measure_init(&m, "regular adds (best barrier)", attempts);
	measure_start(&m);
	for(int i=0; i<attempts; i++) {
		counter++;
		// this is to make the compiler actually do the loop
		asm volatile ("" : "=g" (counter) ::);
	}
	measure_end(&m);
	measure_print(&m);

	counter=0;
	measure_init(&m, "regular adds (no barrier-loop probably goes away here)", attempts);
	measure_start(&m);
	for(int i=0; i<attempts; i++) {
		counter++;
	}
	measure_end(&m);
	measure_print(&m);

	vcounter=0;
	measure_init(&m, "volatile adds (volatile is close to a barrier)", attempts);
	measure_start(&m);
	for(int i=0; i<attempts; i++) {
		vcounter++;
	}
	measure_end(&m);
	measure_print(&m);

	return EXIT_SUCCESS;
}
