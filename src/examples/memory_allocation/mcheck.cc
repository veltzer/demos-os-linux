/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for abort(3)
#include <stdio.h>	// for printf(3)
#include <mcheck.h>	// for mcheck(3), states, mprobe(3)
#include <string.h>	// for strcpy(3)

/*
 * This is a demo of using the mcheck feature of the GNU C standard library
 */

void myhandler(enum mcheck_status status) {
	printf("inside my handler, careful not to do something complicated here since the system is in a bad state as it is...\n");
	char state[256];
	switch(status) {
	case MCHECK_DISABLED:
		strcpy(state, "disabled");
		break;
	case MCHECK_OK:
		strcpy(state, "ok");
		break;
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
	printf("state is %s\n", state);
	// abort();
}

int main(int argc, char** argv, char** envp) {
	// I am using my own handler but the default one is ok (prints an error
	// message and aborts).
	// This call is neccessary if you havent linked with -lmcheck or if you really
	// want to use your own handler. I haven't linked with -lmcheck so I will
	// use my own handler...
	mcheck(myhandler);
	const int size_of_buffer=10;
	char* buf=(char*)malloc(size_of_buffer);
	char* buf2=(char*)malloc(size_of_buffer);
	const int to_overrun=1;
	int i;
	for(i=size_of_buffer; i<size_of_buffer+to_overrun; i++) {
		buf[i]=i%26+'a';
	}
	for(i=-to_overrun; i<0; i++) {
		buf2[i]=i%26+'a';
	}
	buf[size_of_buffer+to_overrun]='\0';
	printf("buf is %s\n", buf);
	// check the status of our allocated chunk...
	int status=mprobe(buf);
	int status2=mprobe(buf2);
	printf("You will not reach this line if the probe fails");
	printf("status is %d\n", status);
	printf("status is %d\n", status2);
	return EXIT_SUCCESS;
}
