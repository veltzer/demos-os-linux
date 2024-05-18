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

/*
 * This application just prints malloc diffs for various malloc allocation.
 * The idea is to use this application as a way to analyze how malloc allocates
 * memory...
 */

#include <firstinclude.h>
#include <stdlib.h>	// for malloc(3)
#include <assert.h>	// for assert(3)

/*
 * This function guesses the next address that malloc(3) will allocate to your
 * code.
 *
 * This is the basic stuff:
 * - malloc needs at least 16 bytes per chunk.
 * - it also needs at least 3 bytes for it's own internal data.
 * - it will allocate on an 8 byte boundary.
 */

void* guess_next_address(void* prev, unsigned int size) {
	if(size<8) {
		size+=8;
	} else {
		if(size%8>5) {
			size+=8;
		}
	}
	size+=8-size%8;
	char* cprev=(char*)prev;
	cprev+=size;
	return cprev;
}

int main() {
	void* prev=NULL;
	for(unsigned int sizeToAlloc=1; sizeToAlloc<100; sizeToAlloc++) {
		void* guess=NULL;
		if(prev!=NULL) {
			guess=guess_next_address(prev, sizeToAlloc);
		}
		void* p=malloc(sizeToAlloc);
		if(prev!=NULL) {
			// TRACE("guess is %p, p is %p, sizeToAlloc is %d",guess,p,sizeToAlloc);
			assert(guess==p);
			// this is debug code aimed at understanding how the malloc
			// engine works...
			// unsigned int diff=(char*)p-(char*)prev;
			// TRACE("allocated %d, real diff is %d, diff is %d, diff mod 8 is %d\n",sizeToAlloc,diff, diff-sizeToAlloc , diff%8);
		}
		prev=p;
	}
	return EXIT_SUCCESS;
}
