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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This explorer issues with accessing structures which are in themselves
 * inside unions...
 *
 * This clearly shows that you can indeed access either fields within structures
 * or fields which are not in structures...
 *
 * It also explores how to get offsets of fields in structures...
 */

// this next macro calculates the offset of a field from the structure
// or union that contains it...
#define OffsetOf(StructName, FieldName) ((char *)(&(((StructName *)1)->FieldName)) - (char *)1)

typedef union _LARGE_INTEGER {
	struct {
		unsigned int LowPart;
		unsigned int HighPart;
	};
	long long QuadPart;
} LARGE_INTEGER;

int main() {
	// this is how you would use such a union...
	LARGE_INTEGER li;
	li.LowPart=15;
	li.HighPart=16;
	printf("li.LowPart is %u\n", li.LowPart);
	printf("li.HighPart is %u\n", li.HighPart);
	printf("li.QuadPart is %lli\n", li.QuadPart);
	// trampling the quad part tramples both low and high parts
	li.QuadPart=17;
	printf("li.LowPart is %u\n", li.LowPart);
	printf("li.HighPart is %u\n", li.HighPart);
	printf("li.QuadPart is %lli\n", li.QuadPart);
	// this is the offsets of it's relevant parts...
	printf("offset of LowPart is %zd\n", OffsetOf(LARGE_INTEGER, LowPart));
	printf("offset of HighPart is %zd\n", OffsetOf(LARGE_INTEGER, HighPart));
	printf("offset of QuadPart is %zd\n", OffsetOf(LARGE_INTEGER, QuadPart));
	// or we could use the built in...
	printf("offset of LowPart is %zd\n", __builtin_offsetof(LARGE_INTEGER, LowPart));
	printf("offset of HighPart is %zd\n", __builtin_offsetof(LARGE_INTEGER, HighPart));
	printf("offset of QuadPart is %zd\n", __builtin_offsetof(LARGE_INTEGER, QuadPart));
	return EXIT_SUCCESS;
}
