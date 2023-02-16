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
#include <stdio.h>	// for fopen(3), fprintf(3), fclose(3), getline(3), popen(3), pclose(3)
#include <stdlib.h>	// for system(3), abort(3), mkstemp(3), malloc(3), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>	// for close(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_INT(), CHECK_NOT_NULL_FILEP(), CHECK_ZERO_ERRNO()
#include <multiproc_utils.h>	// for my_system()

/*
 * Example for using the shbang line to run C code
 */

// #define DO_DEBUG

/*
 * cut all but the first line in the file and return the temp file holding the result
 */
void cut_first_line(const char* filename, FILE* output) {
	FILE* input=CHECK_NOT_NULL_FILEP(fopen(filename, "r"));
	size_t n=1024;
	char* lineptr=(char*)malloc(n);
	int active=0;
	int ret=CHECK_NOT_M1(getline(&lineptr, &n, input));
	while(ret>0) {
		if(active) {
#ifdef DO_DEBUG
			fprintf(stderr, "writing out [%s]\n", lineptr);
#endif	// DO_DEBUG
			CHECK_NOT_INT(fputs(lineptr, output), EOF);
		} else {
			active=1;
		}
		ret=getline(&lineptr, &n, input);
	}
	CHECK_ZERO_ERRNO(fclose(input));
}

int main(int argc, char** argv, char** envp) {
	// handle command line
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [cshell file]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* filename=argv[1];
	char tmpl[]="/tmp/crun.XXXXXX";
	// create a filename for the executable
	int fd=CHECK_NOT_M1(mkstemp(tmpl));
#ifdef DO_DEBUG
	fprintf(stderr, "tmpl is [%s]\n", tmpl);
#endif	// DO_DEBUG
	// run the compiler until eof...
	const int max=1024;
	char cmd[max];
	sprintf(cmd, "gcc -x c -o %s -O2 -", tmpl);
#ifdef DO_DEBUG
	fprintf(stderr, "cmd is [%s]\n", cmd);
#endif	// DO_DEBUG
	FILE* handle=CHECK_NOT_NULL_FILEP(popen(cmd, "w"));
	cut_first_line(filename, handle);
	CHECK_NOT_M1(pclose(handle));
	// close the tmpl
	CHECK_NOT_M1(close(fd));
	// now run the resulting executable
	my_system("%s", tmpl);
	// finally unlink the tmpl
	CHECK_NOT_M1(unlink(tmpl));
	return EXIT_SUCCESS;
}
