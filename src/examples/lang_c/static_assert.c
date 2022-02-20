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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to do static assert in pure C.
 * What are static assertions? assertions that are validated/checked at
 * compile time and will cause a compilation time error if violated.
 * Take note that in the C++ language GCC already supports static assertion
 * via the c++0x standard. You need to pass the -std=c++0x flag and use
 * 'static_assert' for that. See an example for that in the C++ section.
 * The example here demonstrates implementing a static assert without
 * support for it from the compiler and the use of the _Static_assert feature
 * of GCC in the C language.
 * Also note that boost has a static assert feature (C++ only).
 *
 * References:
 * http://www.pixelbeat.org/programming/gcc/static_assert.html
 * http://stackoverflow.com/questions/987684/does-gcc-have-a-built-in-compile-time-assert
 * http://stackoverflow.com/questions/3385515/static-assert-in-c
 *
 * EXTRA_COMPILE_FLAGS=-Wno-unused-local-typedefs
 */

// this works by creating a "duplicate case for switch" error at compile time...
// and can only be used inside code.
// A problem with this is that it actually creates code that may not be eliminatable by
// the compiler.
#define COMPILE_TIME_ASSERT(cond) do { switch(0) { case 0: case cond:; } } while (0)
// this works by creating an error for size of array being negative at compile time.
// can be used outside/inside of code and therefore if placed outside of your functions
// will not make your functions any less efficient.
#define PR_STATIC_ASSERT(condition) extern void pr_static_assert(int arg[(condition) ? 1 : -1])
// this is similar to what the kernel is using
// cannot be used outside of functions
#define ct_assert(e) ((void)sizeof(char[1 - 2*!(e)]))
// another option (can be used outside functions, mesage is not nice)
#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_ ## MSG[(COND) ? 1 : -1]
// using error (does not work, need to fix it...)
#define CTC(X) { extern int __attribute__((error("assertion failure: '" # X "' not true"))) compile_time_check(); ((X) ? 0 : compile_time_check()), 0; };

PR_STATIC_ASSERT(1==1);
_Static_assert(1==1, "fail message");
STATIC_ASSERT(1==1, fail);

int main(int argc, char** argv, char** envp) {
	COMPILE_TIME_ASSERT(1==1);
	PR_STATIC_ASSERT(1==1);
	// this is a built in gcc static assertion, no flags needed
	_Static_assert(1==1, "fail message");
	ct_assert(1==1);
	STATIC_ASSERT(1==1, fail);
	// CTC(1==1);
	return EXIT_SUCCESS;
}
