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
#include <stdlib.h>	// for malloc(3)
#include <assert.h>	// for assert(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <string.h>	// for memset(3)

/*
 * This application tries to guess where the next malloc allocation would be...
 *
 * In order to analyze malloc performance take a look at the "mguess_allocated"
 * function.
 * - It seems that malloc always needs at least 4 bytes for internal
 * book keeping. This is where it keeps the size of the block which is allocated
 * (probably - well check it later...).
 * - It is also deducible that malloc always allocated on an 8 byte boundry.
 *
 * TODO:
 * - improve the guessing system to also handle free (get reports about them
 * and be able to guess even though there are calls to free(3)).
 */

static char* p;
static bool debug=false;

/* convert number of bytes to be allocated to real number of bytes allocated... */
unsigned int round_it(unsigned int size) {
	if(size%8>4) {
		size=size-size%8+16;
	} else {
		size=size-size%8+8;
	}
	if(size==8) size+=8;
	return size;
}

/* let the guessing system know that you have allocated memory */
void mguess_allocated(unsigned int size) {
	unsigned int real=round_it(size);
	if(debug) {
		printf("real is %d,diff is %d, p is %p\n", real, real-size, p);
	}
	p+=real;
}

unsigned int get_block_size(void* p) {
	unsigned int* u=(unsigned int*)p;
	u-=1;
	if(debug) {
		printf("*u is %d (char ptr is %d)\n", *u, *((char*)u));
	}
	return (*u)-1;
}

/* initialize the guessing system */
void mguess_init(void) {
	p=(char*)malloc(1);
	mguess_allocated(1);
}

/* get a guess about the next allocation from the guessing system */
void* mguess_guess(void) {
	return (void*)p;
}
/* shut down the guessing system */
void mguess_fini(void) {
	// do nothing...
}

int main(int argc, char** argv, char** envp) {
	mguess_init();
	// lets randomize things a bit so everything will be different on each run...
	srandom(getpid());
	// lets do some random allocatinons...
	for(unsigned int i=0; i<10000; i++) {
		unsigned int size=random()%10000;
		void* g=mguess_guess();
		void* p=malloc(size);
		unsigned int blk_size=get_block_size(p);
		unsigned int real_size=round_it(size);
		mguess_allocated(size);
		if(debug) {
			printf("g is %p, p is %p, size is %d, blk_size is %d, real_size is %d\n", g, p, size, blk_size, real_size);
		}
		assert(g==p);
		assert(blk_size==real_size);
	}
	mguess_fini();
	return EXIT_SUCCESS;
}
