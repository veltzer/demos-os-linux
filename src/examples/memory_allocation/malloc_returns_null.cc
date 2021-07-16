/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for malloc(3), EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_NULL()

/*
 * This example explores when it is exactly that malloc(3) returns NULL.
 * The answer is when virtual memory is over.
 * On a 64bit box this seems to be around 100,000 gigs.
 * Need to see how this works on a 32 bit box.
 * Another note is that malloc, even on a 64bit box, does not allocate more than 4gigs in one allocation.
 */

int main(int argc, char** argv, char** envp) {
	printf("sizeof(size_t) is [%zd]...\n", sizeof(size_t));
	unsigned int num_gigs=4;
	size_t size=num_gigs*1024L*1024L*1024L;
	unsigned int counter=0;
	while(true) {
		CHECK_NOT_NULL(malloc(size));
		counter+=num_gigs;
		printf("allocted %d gigs..\n", counter);
	}
	return EXIT_SUCCESS;
}
