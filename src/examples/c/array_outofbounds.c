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

/*
 * This example shows how to step out of the array bounds, in this case
 * in order to step over some other variable. Note that if you do not
 * make the other number (in this case 'num') materialize by taking a reference
 * to it then it will not materialize and a bug will not be seen (although a bug
 * will occur!).
 */

int main() {
	int a[10];
	int num=1995;
	printf("%p %p\n", (void*)a, (void*)&num);
	int i;
	for(i=0; i<10; i++) {
		a[i]=i;
	}
	for(i=0; i<10; i++) {
		printf("a[%d]=%d\n", i, a[i]);
	}
	printf("num is %d\n", num);
	// next line will not compile if warnings in gcc
	// are turned on since gcc is smart
	// a[10]=2011;
	for(i=0; i<10; i++) {
		printf("a[%d]=%d\n", i, a[i]);
	}
	printf("num is %d\n", num);
	return 0;
}
