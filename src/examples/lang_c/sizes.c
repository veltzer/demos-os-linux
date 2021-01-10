/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <sys/utsname.h>// for uname(2)
#include <stdbool.h>	// for bool
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for stringify(), PRINT_SIZEOF()
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <stdint.h>	// for uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t

/*
 * This is a demo showing the sizes of basic types on the architecture
 * it is running on (it also prints the name of the architecture).
 *
 * This is intended for a GNU/linux system.
 *
 * Notes:
 * - bool is just a macro for _Bool as the print clearly shows.
 *
 * TODO:
 * - Show the results inside this source code for 32bit and 64bit linux.
 */

typedef struct _struct_empty {
} struct_empty;
typedef struct _struct_emptyarray {
	char a[0];
} struct_emptyarray;
typedef struct _struct_char {
	char c;
} struct_char;
typedef struct _struct_charintchar {
	char c1;
	int i;
	char c2;
} struct_charintchar;
typedef struct _struct_char5 {
	char c1;
	char c2;
	char c3;
	char c4;
	char c5;
} struct_char5;

int main(int argc, char** argv, char** envp) {
	struct utsname buf;
	CHECK_NOT_M1(uname(&buf));
	printf("architecture is [%s]\n", buf.machine);

	PRINT_SIZEOF(bool);
	PRINT_SIZEOF(char);
	PRINT_SIZEOF(unsigned char);
	PRINT_SIZEOF(short);
	PRINT_SIZEOF(unsigned short);
	PRINT_SIZEOF(int);
	PRINT_SIZEOF(unsigned int);
	PRINT_SIZEOF(long);
	PRINT_SIZEOF(unsigned long);
	PRINT_SIZEOF(long long);
	PRINT_SIZEOF(unsigned long long);
	PRINT_SIZEOF(float);
	PRINT_SIZEOF(double);
	// this is not allowed in C
	// float* fourfloatarray={ 5, 3, 2, 1 };
	// but this is...
	float fourfloatarray[]={ 5, 3, 2, 1 };
	printf("size of fourfloatarray is %zd\n", sizeof(fourfloatarray));
	float zerofloatarray[]={};
	printf("size of zerofloatarray is %zd\n", sizeof(zerofloatarray));
	PRINT_SIZEOF(struct_empty);
	PRINT_SIZEOF(struct_emptyarray);
	PRINT_SIZEOF(struct_char);
	PRINT_SIZEOF(struct_charintchar);
	PRINT_SIZEOF(struct_char5);
	PRINT_SIZEOF(uint8_t);
	PRINT_SIZEOF(uint16_t);
	PRINT_SIZEOF(uint32_t);
	PRINT_SIZEOF(uint64_t);
	PRINT_SIZEOF(int8_t);
	PRINT_SIZEOF(int16_t);
	PRINT_SIZEOF(int32_t);
	PRINT_SIZEOF(int64_t);
	return EXIT_SUCCESS;
}
