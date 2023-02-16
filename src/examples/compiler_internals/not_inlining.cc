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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <disassembly_utils.h>	// for disassemble_main()

/*
 * This is an example showing how to force the compiler not to inline the function.
 * How can you check that this works? compare the two disassembled version
 * resulting from putting the "noinline" line into a comment
 *
 * Note that if the function that you want to avoid inlineing has no side effects
 * (a prime example of this is an empty function) then the compiler could still
 * avoid the entire call to the function. So if you want to avoid inlining an
 * empty function or a function with no side effects insert "asm ("");" as
 * a side effect generating statement into it.
 */

int add1(int a, int b) __attribute__((noinline));
int add1(int a, int b) {
	return a+b;
}

int __attribute__((noinline)) add2(int a, int b) {
	return a+b;
}

void empty1() __attribute__((noinline));
void empty1() {
}

void __attribute__((noinline)) empty2() {
}

void empty3() {
	asm ("");
}

void __attribute__((noinline)) empty4() {
	asm ("");
}

int main(int argc, char** argv, char** envp) {
	disassemble_main();
	printf("2+2=%d\n", add1(2, 2));
	printf("2+3=%d\n", add2(2, 3));

	const unsigned int attempts=1000000000;
	measure m;

	measure_init(&m, "just __attribute__((noinline))", attempts);
	measure_start(&m);
	for(unsigned int i=0; i<attempts; i++) {
		empty1();
	}
	measure_end(&m);
	measure_print(&m);

	measure_init(&m, "another type of __attribute__((noinline))", attempts);
	measure_start(&m);
	for(unsigned int i=0; i<attempts; i++) {
		empty2();
	}
	measure_end(&m);
	measure_print(&m);

	measure_init(&m, "empty function with asm side effect", attempts);
	measure_start(&m);
	for(unsigned int i=0; i<attempts; i++) {
		empty3();
	}
	measure_end(&m);
	measure_print(&m);

	measure_init(&m, "empty function with __attribute__((noinline)) and asm side effect", attempts);
	measure_start(&m);
	for(unsigned int i=0; i<attempts; i++) {
		empty4();
	}
	measure_end(&m);
	measure_print(&m);
	return EXIT_SUCCESS;
}
