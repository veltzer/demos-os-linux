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
#include <sys/types.h>	// for stat(2)
#include <sys/stat.h>	// for stat(2)
#include <unistd.h>	// for stat(2)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>	// for stderr, fprintf(3), printf(3)
#include <time.h>	// for ctime_r(3), localtime_r(3), strftime(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_CHARP(), CHECK_NOT_NULL(), CHECK_NOT_ZERO()

/*
 * This example explores the time_t type.
 *
 * Notes:
 * - This shows that on Intel 32 bit time_t is an unsigned integer
 * which is 32 bites wide that counts the seconds from 1/1/1970
 * which means it will run out on:
 * 1970+2^32/(60*60*24*365)=2106
 * - it shows how to print the time using ctime(3)/ctime_r(3)
 * functions.
 * - it shows how to convert the time from 'time_t' to
 * 'struct tm'
 * - it shows formatting 'struct tm' to any representation
 * using the strftime(3) function.
 *
 * TODO:
 * - run this on 64 bit and see the results.
 * - do my own ctime_r function which is more like snprintf(3)
 */

int main(int argc, char** argv, char** envp) {
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [file]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /etc/passwd\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* filename=argv[1];
	// code
	printf("sizeof(time_t) is %zd\n", sizeof(time_t));
	struct stat sb;
	CHECK_NOT_M1(stat(filename, &sb));
	unsigned int myint=(unsigned int)sb.st_mtime;
	printf("myint is %u\n", myint);
	// lets guess the year according to myint
	printf("the year is %d\n", 1970+myint/(3600*24*365));
	// lets print the time using ctime_r(3)
	// buffer must be at least 26 bytes long (see documentation of ctime(3))
	char buf[30];
	CHECK_CHARP(ctime_r(&sb.st_mtime, buf), buf);
	// no newline on the next line since ctime(3)/ctime_r(3) already have
	// a newline
	printf("ctime_r(&sb.st_mtime, buf) is %s", buf);
	// convert 'time_t' to 'struct tm' using localtime_r(3)
	struct tm mytm;
	CHECK_NOT_NULL(localtime_r(&sb.st_mtime, &mytm));
	// print the 'struct tm' using the strftime(3) functions
	char mybuf[256];
	const char* format="%b %e %R";
	CHECK_NOT_ZERO(strftime(mybuf, sizeof(mybuf), format, &mytm));
	printf("time after formatting is [%s]\n", mybuf);
	return EXIT_SUCCESS;
}
