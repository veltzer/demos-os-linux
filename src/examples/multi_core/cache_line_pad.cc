/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <err_utils.h>	// for CHECK_ASSERT()

/*
 * Example of padding a structure to L2 cache line size bytes via union...
 * Notice how to avoid the cache line size being hardcoded in your code...
 *
 * The various ways in which this could be done and are demonstrated:
 * 1. using a union.
 * 2. explicit padding.
 * 3. __attribute__((aligned (alignment)))
 *
 * This also shows that the __attribute__((aligned (LEVEL2_CACHE_LINESIZE)))
 * will cause the structure to always be aligned by the compiler when passing
 * it around and creating it on the stack.
 * Both ways DO NOT force malloc to allocate these structures aligned. You will
 * still need something like posix_memalign(3) for that...
 *
 * EXTRA_COMPILE_CMD=echo -DLEVEL2_CACHE_LINESIZE=`getconf LEVEL2_CACHE_LINESIZE`
 * required for the static_assert below to work...
 * EXTRA_COMPILE_FLAGS=-std=c++0x
 *
 * References:
 * http://stackoverflow.com/questions/1215612/structure-padding-in-c
 *
 * TODO:
 * - the union could be made transparent using a transparent_union attribute. check
 * out the gcc documentation and put this in.
 */

struct foo{
	union _un{
		struct _pad_struct{
			char irrelevant[LEVEL2_CACHE_LINESIZE];
		} pad_struct;
		struct _real{
			char c1;
			int i1;
			char c2;
		} real;
	} un;
};
static_assert(sizeof(foo)==LEVEL2_CACHE_LINESIZE, "size of foo is wrong");

// this guarantess that the size of the structure will be exactly 64 bytes
struct bar{
	char c1;
	int i1;
	char c2;
} __attribute__((aligned (LEVEL2_CACHE_LINESIZE)));
static_assert(sizeof(bar)==LEVEL2_CACHE_LINESIZE, "size of bar is wrong");

// same as before, applied to a Class instead...
class MyClass{
private:
	char c1;
	int i1;
	char c2;
} __attribute__((aligned (LEVEL2_CACHE_LINESIZE)));
static_assert(sizeof(MyClass)==LEVEL2_CACHE_LINESIZE, "size of MyClass is wrong");

typedef struct _barestruct{
	int field1;
	char foo;
} barestruct;

typedef struct _mystruct{
	int field1;
	char foo;
	char padding[LEVEL2_CACHE_LINESIZE-sizeof(barestruct)];
} mystruct;

static_assert(sizeof(mystruct)==LEVEL2_CACHE_LINESIZE, "size of mystruct is wrong");

/*
 * This does not work...
 * #include <stddef.h> // for offsetof(3)
 * typedef struct _mystruct2 {
 * int field1;
 * char foo;
 * char padding[64-__builtin_offsetof(_mystruct2,foo)];
 * } mystruct2;
 */
/*
 * We show that struct bar is passed aligned to functions while myfoo isn't
 * x64 complains about this
 */
#ifndef __x86_64__
void myfunction(char c, struct bar mybar, char l, struct foo myfoo) {
	CHECK_ASSERT((unsigned long)&mybar%LEVEL2_CACHE_LINESIZE==0);
	CHECK_ASSERT((unsigned long)&myfoo%LEVEL2_CACHE_LINESIZE!=0);
}
#endif	/* __x86_64__ */

int main() {
	printf("LEVEL2_CACHE_LINESIZE=%d\n", LEVEL2_CACHE_LINESIZE);
	printf("sizeof(foo)=%zd\n", sizeof(foo));
	printf("sizeof(bar)=%zd\n", sizeof(bar));
	printf("sizeof(MyClass)=%zd\n", sizeof(MyClass));
	struct bar mybar;
	struct foo myfoo;
	CHECK_ASSERT((unsigned long)&mybar%LEVEL2_CACHE_LINESIZE==0);
	CHECK_ASSERT((unsigned long)&myfoo%LEVEL2_CACHE_LINESIZE!=0);
#ifndef __x86_64__
	// cppcheck-suppress uninitvar
	myfunction('4', mybar, '5', myfoo);
#endif	/* __x86_64__ */
	printf("sizeof(barestruct)=%zd\n", sizeof(barestruct));
	printf("sizeof(mystruct)=%zd\n", sizeof(mystruct));
	return EXIT_SUCCESS;
}
