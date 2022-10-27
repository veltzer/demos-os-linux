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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for malloc(3), free(3), atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#ifdef DMALLOC
#include <dmalloc.h>
#endif	// DMALLOC

/*
 * This test program simply simulates various memory errors
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [number of error to perform]\n", argv[0], argv[0]);
		fprintf(stderr, "\t0 - write before the buffer and free\n");
		fprintf(stderr, "\t1 - write after the buffer and free\n");
		fprintf(stderr, "\t2 - write before the buffer and dont free\n");
		fprintf(stderr, "\t3 - write after the buffer and dont free\n");
		fprintf(stderr, "\t4 - read before the buffer\n");
		fprintf(stderr, "\t5 - read after the buffer\n");
		fprintf(stderr, "\t6 - forget to release the buffer\n");
		fprintf(stderr, "\t7 - release the buffer twice\n");
		return EXIT_FAILURE;
	}
	unsigned int type=atoi(argv[1]);
	const int size=100;
	fprintf(stderr, "starting\n");
	char* buffer=(char*)malloc(size);
	switch(type) {
	case 0:
		for(int i=0; i>-50; i--) {
			buffer[i]=i%256;
		}
		free(buffer);
		break;
	case 1:
		for(int i=0; i<size*2; i++) {
			fprintf(stderr, "i is %d\n", i);
			buffer[i]=i%256;
		}
		free(buffer);
		break;
	case 2:
		for(int i=0; i>-50; i--) {
			buffer[i]=i%256;
		}
		break;
	case 3:
		for(int i=0; i<size*2; i++) {
			fprintf(stderr, "i is %d\n", i);
			buffer[i]=i%256;
		}
		break;
	case 4:
		for(int i=0; i>-50; i--) {
			printf("value at %d is %c\n", i, buffer[i]);
		}
		break;
	case 5:
		for(int i=0; i<size*2; i++) {
			printf("value at %d is %c\n", i, buffer[i]);
		}
		break;
	case 6:
		// no need to do anything, just don't free the buffer
		break;
	case 7:
		free(buffer);
#pragma GCC diagnostic ignored "-Wuse-after-free"
		free(buffer);
		break;
	}
	fprintf(stderr, "ending\n");
	return EXIT_SUCCESS;
}
