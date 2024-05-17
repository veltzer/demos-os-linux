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
#include <stdio.h>	// for fprintf(3)
#include <setjmp.h>	// for longjmp(3), setjmp(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a demo of how to use the setjmp/longjmp in order to simulate exception
 * handling in C code.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-clobbered
 */

// This buffer saves the context of the jump. We need one for each unique
// point we wish to jump to
static jmp_buf env;

/* this simulates a function which sometimes encounters errors */
void func() {
	static int counter=0;

	counter++;
	fprintf(stderr, "setting counter to %d\n", counter);
	if (counter % 3==0) {
		longjmp(env, counter);
	}
	fprintf(stderr, "this is the continuation of the function %d\n", counter);
}

int main() {
	for(int c=0; c<10; c++) {
		int ret;
		ret=setjmp(env);
		if(!ret) {
			// This is the regular code. We get here when doing the
			// setjmp for the first couple of times
			fprintf(stderr, "c is %d\n", c);
			func();
		} else {
			// we got here from longjmp, this means that some
			// error occured
			fprintf(stderr, "oops, got an error from func %d\n", ret);
		}
	}
	return EXIT_SUCCESS;
}
