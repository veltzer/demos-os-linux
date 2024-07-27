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
#include <assert.h>	// for assert(3)

/*
 * This is an example showing C99 initialisation syntax and horrible
 * effect this has on the size of the object file generated from this file.
 * Just look at the size of the executable for this example and you will
 * see what I mean.
 *
 * Notes:
 * - If you switch the initialisation from {1} to {0} then suddenly the size
 * of the object file drops dramatically. This is because initialisation to 0
 * is built in to the toolchain and is done programatically rather than storing
 * the entire data in the data segment.
 * - when you initialize the array using={1} only the first element is initialized.
 * You can initialize more elements of the array by adding more initialization values.
 * The other values of the array will be zero as this example clearly shows.
 */

int myarray[100000]={1, 1};

int main() {
	int i;
	for(i=0; i<2; i++) {
		assert(myarray[i]==1);
	}
	for(i=2; i<100000; i++) {
		assert(myarray[i]==0);
	}
	return 0;
}
