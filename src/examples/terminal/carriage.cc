/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3), fflush(3), setvbuf(3)
#include <unistd.h>	// for usleep(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an example of how to use the carriage return character
 * to do some nice output progress reporting without scrolling with standard
 * output...
 *
 * Notes:
 * - we print space after printing to show the general way to use this since our
 * number is going down and not up. In the up case you would not need it.
 * - the 'fflush' is actually needed. Otherwise you would still get updates but
 * only when stdout is flushed naturally which happens only when it's buffer
 * is full or when "\n" is printed.
 *	- you can set stdout for no buffering, use stderr instead, or use direct write(2)
 *	to avoid the extra call to flush.
 *	- ofcouse, you can also use the curses library to do all of this which has a much
 *	richer API but does require some learning.
 */

int main(int argc, char** argv, char** envp) {
	printf("stdout, no flush (bad updates)\n");
	for(unsigned int i=1000; i>0; i--) {
		printf("i is %10d\r", i);
		CHECK_NOT_M1(usleep(10000));
	}
	printf("\n\n");
	printf("stdout, with flush (good updates, overhead on calling flush)\n");
	for(unsigned int i=1000; i>0; i--) {
		printf("i is %10d\r", i);
		fflush(stdout);
		CHECK_NOT_M1(usleep(10000));
	}
	printf("\n\n");
	printf("stdout, no buffer (good updates, no call to flush so good performance)\n");
	CHECK_NOT_M1(setvbuf(stdout, NULL, _IONBF, 0));
	for(unsigned int i=1000; i>0; i--) {
		printf("i is %10d\r", i);
		CHECK_NOT_M1(usleep(10000));
	}
	printf("\n\n");
	printf("stderr, unbuffered naturally so no flush\n");
	for(unsigned int i=1000; i>0; i--) {
		fprintf(stderr, "i is %10d\r", i);
		CHECK_NOT_M1(usleep(10000));
	}
	printf("\n\n");
	return EXIT_SUCCESS;
}
