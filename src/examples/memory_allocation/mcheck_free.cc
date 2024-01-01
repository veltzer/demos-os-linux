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
#include <mcheck.h>	// for mcheck(3), states, mprobe(3)
#include <string.h>	// for strcpy(3)
#include <stdlib.h>	// for malloc(3), free(3)

/*
 * This example shows how mcheck helps to catch errors in free...
 * This means that if you activated mcheck your free(3) calls will run
 * some consistency check and you will catch the errors.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-use-after-free
 */


void myhandler(enum mcheck_status status) {
	// inside my handler, careful not to do something complicated here since the system is in a bad state as it is...
	char state[256];
	switch(status) {
	case MCHECK_DISABLED:
		strcpy(state, "disabled");
		break;
	case MCHECK_OK:
		return;
	// strcpy(state,"ok");
	// break;
	case MCHECK_HEAD:
		strcpy(state, "head");
		break;
	case MCHECK_TAIL:
		strcpy(state, "tail");
		break;
	case MCHECK_FREE:
		strcpy(state, "free");
		break;
	}
	printf("mcheck caught error: status is %s\n", state);
	// abort();
}

int main(int argc, char** argv, char** envp) {
	printf("Starting\n");
	// I am using my own handler but the default one is ok (prints an error
	// message and aborts).
	// This call is neccessary if you havent linked with -lmcheck or if you really
	// want to use your own handler. I haven't linked with -lmcheck so I will
	// use my own handler...
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
	mcheck(myhandler);
	const int size_of_buffer=10;
	char* buf=(char*)malloc(size_of_buffer);
	char* buf2=(char*)malloc(size_of_buffer);
	printf("After allocation\n");
	const int to_overrun=1;
	int i;
	for(i=size_of_buffer; i<size_of_buffer+to_overrun; i++) {
		buf[i]=i%26+'a';
	}
	for(i=-to_overrun; i<0; i++) {
		buf2[i]=i%26+'a';
	}
	buf[size_of_buffer+to_overrun]='\0';
	printf("After overruns\n");
	printf("Before first free\n");
	free(buf);
	printf("After first free\n");
	printf("Before second free\n");
	free(buf2);
	printf("After second free\n");
	// check the status of our allocated chunk...
	int status=mprobe(buf);
	int status2=mprobe(buf2);
	printf("status is %d\n",status);
	printf("status is %d\n",status2);
	return EXIT_SUCCESS;
}
