/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for atoi(3)
#include <string.h>	// for memset(3)
#include <trace_utils.h>// for TRACE()
#include <multiproc_utils.h>	// for my_system()

/*
 * This example explores how static data (data segement) affects binary size
 * and speed
 *
 * Notes:
 * - if you initialize the array to 0 (first example) then the binary is small.
 * - if you do not initialize the array (second example) then the binary is small.
 * - if you initialize the array to anything which is not 0 then the binary is large.
 */

const unsigned int size=1000000;
// first example
int myarray[size]={ 0 };
// second example
// int myarray[size];
// third example
// int myarray[size]={ 3 };

int main(int argc, char** argv, char** envp) {
	unsigned int sum=0;
	for(int i=0; i<argc; i++) {
		sum+=atoi(argv[i]);
	}
	memset(myarray, sum, sizeof(int)*size);
	sum=0;
	for(unsigned int l=0; l<size; l++) {
		sum+=myarray[l];
	}
	TRACE("sum is %u\n", sum);
	my_system("ls -l %s", argv[0]);
	return EXIT_SUCCESS;
}
