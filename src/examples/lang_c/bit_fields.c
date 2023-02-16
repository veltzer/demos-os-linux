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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <assert.h>	// for assert(3)
#include <us_helper.h>	// for PRINT_SIZEOF()
#include <endian.h>	// for __BYTE_ORDER

/*
 * This example shows bit fields as they are implemented by the gcc
 * compiler.
 *
 * Notes:
 * - if you pack such a structure the size of the structure will be
 * the number of bits divided by 8 rounded up.
 * - if you don't pack the size will just be sum of the sizes of
 * each field which is, for each field, number of bits divided by
 * 8 rounded up. And all will be rounded up to 4 bytes.
 *
 * References:
 * http://www.coranac.com/documents/working-with-bits-and-bitfields/
 */

typedef struct _s1 {
	unsigned int f1 : 4;
	unsigned int f2 : 3;
	unsigned int f3 : 2;
} s1;

typedef struct _s2 {
	unsigned int f1 : 4;
	unsigned int f2 : 4;
	unsigned int f3 : 8;
	unsigned int f4 : 16;
} __attribute__ ((__packed__)) s2;

int main(int argc, char** argv, char** envp) {
	unsigned char data[]={0x12, 0x34, 0x56, 0x78};
	s2* v2=(s2*)data;
	PRINT_SIZEOF(s1);
	PRINT_SIZEOF(s2);
#if __BYTE_ORDER == __LITTLE_ENDIAN
	assert(v2->f1==0x2);
	assert(v2->f2==0x1);
	assert(v2->f3==0x34);
	assert(v2->f4==0x7856);
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
	assert(v2->f1==0x1);
	assert(v2->f2==0x2);
	assert(v2->f3==0x34);
	assert(v2->f4==0x5678);
#endif
	return EXIT_SUCCESS;
}
