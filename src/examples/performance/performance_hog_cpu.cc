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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)

/*
 * This is a simple example that spends lots of time in a function.
 * You can watch this with 'sudo perf top' and enter the function with 's'.
 * make sure you compile this with debug info if you want to see
 * perf annotate the machine code with your code (see below)
 *
 * Notes:
 * - you need the sudo since perf works by hardware events and support for them is in the kernel.
 * - you can avoid root/sudo with:
 *	Consider tweaking /proc/sys/kernel/perf_event_paranoid:
 *	-1 - Not paranoid at all
 *	0 - Disallow raw tracepoint access for unpriv
 *	1 - Disallow cpu events for unpriv
 *	2 - Disallow kernel profiling for unpriv
 * - if you strip the executable you will not get the function names.
 * - if you turn off optimisation you should see completely different assembly code.
 *
 * This is to make sure I can see annotation (adding the debug info).
 * EXTRA_COMPILE_FLAGS=-g3 -O0
 */

class A {
public:
	// the function is not inlined on purpose so it would show up in profilers
	// like 'perf(1)'...
	// static void performance_hog_function2() {
	static float performance_hog_function_cpu() __attribute__((noinline)) {
		float sum=0;
		for(unsigned int i=0; i<1000000; i++) {
			for(unsigned int j=0; j<500000; j++) {
				sum+=i*j;
			}
			A::inner_loop(&sum, i);
		}
		return sum;
	}
	static void inner_loop(float* sum, int i) __attribute__((noinline)) {
		for(unsigned int j=0; j<1000000; j++) {
			*sum+=i*j;
		}
	}
};

int main(int argc, char** argv, char** envp) {
	float s=A::performance_hog_function_cpu();
	printf("s is %lf\n", s);
	return EXIT_SUCCESS;
}
