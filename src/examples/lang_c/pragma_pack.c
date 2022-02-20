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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for stringify(), PRINT_SIZEOF()

/*
 * This is an example of controlling the packing of structures
 *
 * Notes:
 * - Where do you get documentation on how to instruct the compiler
 * on pragma packing? 'info gcc-4.7' and search for 'Structure-Packing Pragmas'.
 * - we use gcc style static asserts here to make sure that what we say
 * about the sizes and offsets of the structures is really true...
 */

// structs under 4 bytes are tight
typedef struct _s1 {
	char c1;
} s1;
_Static_assert(sizeof(s1)==1, "msg");
_Static_assert(__builtin_offsetof(s1, c1)==0, "msg");

// and again...
typedef struct _s2 {
	char c1;
	char c2;
} s2;
_Static_assert(sizeof(s2)==2, "msg");
_Static_assert(__builtin_offsetof(s2, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s2, c2)==1, "msg");

// this one will be 6 bytes because the short will be on a 2 byte boundary
// and so the first char is padded and so is the second...
typedef struct _s3 {
	char c1;
	short s1;
	char c2;
} s3;
_Static_assert(sizeof(s3)==6, "msg");
_Static_assert(__builtin_offsetof(s3, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s3, s1)==2, "msg");
_Static_assert(__builtin_offsetof(s3, c2)==4, "msg");

// lets pack it tighter
#pragma pack(push,1)
typedef struct _s4 {
	char c1;
	short s1;
	char c2;
} s4;
#pragma pack(pop)
_Static_assert(sizeof(s4)==4, "msg");
_Static_assert(__builtin_offsetof(s4, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s4, s1)==1, "msg");
_Static_assert(__builtin_offsetof(s4, c2)==3, "msg");

// ints will be on their own 4 byte boundary
typedef struct _s5 {
	char c1;
	int i1;
	char c2;
} s5;
_Static_assert(sizeof(s5)==12, "msg");
_Static_assert(__builtin_offsetof(s5, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s5, i1)==4, "msg");
_Static_assert(__builtin_offsetof(s5, c2)==8, "msg");

// lets pack the most (1)
#pragma pack(push,1)
typedef struct _s6 {
	char c1;
	int i1;
	char c2;
} s6;
#pragma pack(pop)
_Static_assert(sizeof(s6)==6, "msg");
_Static_assert(__builtin_offsetof(s6, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s6, i1)==1, "msg");
_Static_assert(__builtin_offsetof(s6, c2)==5, "msg");

// lets pack it less (2)
#pragma pack(push,2)
typedef struct _s7 {
	char c1;
	int i1;
	char c2;
} s7;
#pragma pack(pop)
_Static_assert(sizeof(s7)==8, "msg");
_Static_assert(__builtin_offsetof(s7, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s7, i1)==2, "msg");
_Static_assert(__builtin_offsetof(s7, c2)==6, "msg");

// lets specify the alignment of the whole structure
// note that alignment does not make the structure size a multiplication
// of the alignment but also makes sure that any structure like this passed
// around by the compiler on the stack or data segment will be on a
// properly aligned addres.
typedef struct _s8 {
	char c1;
	int i1;
	char c2;
} __attribute__((aligned (16))) s8;
_Static_assert(sizeof(s8)==16, "msg");
_Static_assert(__builtin_offsetof(s8, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s8, i1)==4, "msg");
_Static_assert(__builtin_offsetof(s8, c2)==8, "msg");

// another way to say that the structure is packed...
typedef struct _s9 {
	char c1;
	int i1;
	char c2;
} __attribute__ ((__packed__)) s9;
_Static_assert(sizeof(s9)==6, "msg");
_Static_assert(__builtin_offsetof(s9, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s9, i1)==1, "msg");
_Static_assert(__builtin_offsetof(s9, c2)==5, "msg");

// lets specify alignment of individual fields in our structure
typedef struct _s10 {
	char c1;
	int i1 __attribute__ ((aligned (8)));
	char c2;
} s10;
_Static_assert(sizeof(s10)==16, "msg");
_Static_assert(__builtin_offsetof(s10, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s10, i1)==8, "msg");
_Static_assert(__builtin_offsetof(s10, c2)==12, "msg");

// this is really unclear and does not match up with gcc
// documentation (http://gcc.gnu.org/onlinedocs/gcc/Type-Attributes.html)
typedef struct _s11 {
	short s1;
	short s2;
	short s3;
} __attribute__ ((aligned)) s11;
_Static_assert(sizeof(s11)==16, "msg");
_Static_assert(__builtin_offsetof(s11, s1)==0, "msg");
_Static_assert(__builtin_offsetof(s11, s2)==2, "msg");
_Static_assert(__builtin_offsetof(s11, s3)==4, "msg");

// you can pack just individual fields
typedef struct _s12 {
	char c1;
	int i1 __attribute__ ((packed));
	char c2;
	int i2;
} s12;
_Static_assert(sizeof(s12)==12, "msg");
_Static_assert(__builtin_offsetof(s12, c1)==0, "msg");
_Static_assert(__builtin_offsetof(s12, i1)==1, "msg");
_Static_assert(__builtin_offsetof(s12, c2)==5, "msg");
_Static_assert(__builtin_offsetof(s12, i2)==8, "msg");

int main(int argc, char** argv, char** envp) {
	PRINT_SIZEOF(s1);
	PRINT_SIZEOF(s2);
	PRINT_SIZEOF(s3);
	PRINT_SIZEOF(s4);
	PRINT_SIZEOF(s5);
	PRINT_SIZEOF(s6);
	PRINT_SIZEOF(s7);
	PRINT_SIZEOF(s8);
	PRINT_SIZEOF(s9);
	PRINT_SIZEOF(s10);
	PRINT_SIZEOF(s11);
	PRINT_SIZEOF(s12);
	// you can use __BIGGEST_ALIGNMENT__ for the largest alignment of any
	// basic type on the system you are compiling to...
	printf("__BIGGEST_ALIGNMENT__ is %d\n", __BIGGEST_ALIGNMENT__);
	return EXIT_SUCCESS;
}
