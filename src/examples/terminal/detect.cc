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
#include <stdio.h>	// for stdout, fileno(3), fprintf(3), stderr, fopen(3), fclose(3), fwrite(3)
#include <unistd.h>	// for isatty(3), ttyname(3), close(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <string.h>	// for strdup(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL_CHARP(), CHECK_INT_NOERRNO(), CHECK_NOT_NULL_FILEP()

/*
 * This is an example of how to detect whether a file you are using is
 * a terminal or not and print it's name in case it is.
 * This is similar to the tty(1) command line utility.
 *
 * NOTES:
 * - The documentation for ttyname states the pointer returned by ttyname(3)
 * may be overwritten by subsequent calls which means that you should duplicate
 * the return value if you want to use it. See below.
 * - It is better to use the ttyname_r(3) function so as not to need to duplicate
 * the resulting string. Also ttyname_r(3) is thread safe.
 * - You can use the name returned to reopen the terminal after it has been closed. See below.
 */

int main(int argc, char** argv, char** envp) {
	// lets take the file descriptor number from stdout which is usually
	// a terminal (unless you redirect it...)
	int filedes=fileno(stdout);
	const char* myttyname;
	// if the file descriptor is a terminal then lets get it's name
	// we duplicate the string since it may be overwritten.
	if (isatty(filedes)) {
		myttyname=CHECK_NOT_NULL_CHARP(ttyname(filedes));
		myttyname=strdup(myttyname);
	} else {
		myttyname=NULL;
	}
	if (myttyname) {
		fprintf(stderr, "stdout is a terminal with name [%s]\n", myttyname);
	} else {
		fprintf(stderr, "stdout is not a terminal\n");
	}
	CHECK_NOT_M1(close(fileno(stdin)));
	CHECK_NOT_M1(close(fileno(stdout)));
	CHECK_NOT_M1(close(fileno(stderr)));
	FILE* newout=CHECK_NOT_NULL_FILEP(fopen(myttyname,"w"));
	const char* hello="you can still write to this terminal after you reopen it\n";
	CHECK_INT_NOERRNO(fwrite(hello, strlen(hello), 1, newout), 1);
	CHECK_ZERO_ERRNO(fclose(newout));
	return EXIT_SUCCESS;
}
