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
#include <iostream>	// for std::cout, std::endl
#include <string.h>	// for memcpy(3)
#include <stdlib.h>	// for malloc(3)

/*
 * This is a simple example that spends lots of time in a function.
 * You can watch this with 'sudo perf top' and enter the function with 's'.
 * If you want annotated sources then stand at the root where you are
 * running.
 *
 * make sure you compile this with debug info if you want to see
 * perf annotate the machine code with your code.
 *
 * The reason that the function whose performance we want to measure is
 * wrapped in a C++ class is to see how well perf handles C++ demangling.
 *
 * Notes:
 * - you need the sudo since perf works by hardware events and support for them in the kernel.
 * - you can avoid root with:
 *	Consider tweaking /proc/sys/kernel/perf_event_paranoid:
 *	-1 - Not paranoid at all
 *	0 - Disallow raw tracepoint access for unpriv
 *	1 - Disallow cpu events for unpriv
 *	2 - Disallow kernel profiling for unpriv
 * - if you strip the executable you will not get the function names.
 *
 * TODO:
 * - enable to ensure, from the source code of this example, that this example
 * will be compiled with debug info regardless of how the other examples
 * are compiled.
 *
 * This is to make sure I can see annotation.
 * EXTRA_COMPILE_FLAGS=-g3
 */

class A {
public:
	// the function is not inlined on purpose so it would show up in profilers
	// use this example to see that the actual memcpy(3) function
	// is tuned to the cpu you are running on...
	static void performance_hog_function(char* buf1, char* buf2, unsigned int size) __attribute__((noinline)) {
		for(unsigned int i=0; i<1000000; i++) {
			for(unsigned int j=0; j<1000000; j++) {
				memcpy(buf1, buf2, size);
			}
		}
	}
};

int main() {
	const unsigned int size=1024*1024;
	char* buf1=(char*)malloc(size);
	char* buf2=(char*)malloc(size);
	A::performance_hog_function(buf1, buf2, size);
	return EXIT_SUCCESS;
}
