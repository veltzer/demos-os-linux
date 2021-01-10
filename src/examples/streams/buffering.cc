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
#include <stdio.h>	// for printf(3), __flbf(3), __fbufsize(3), fclose(3), setbuffer(3), fwrite(3), fprintf(3), stderr, fflush(3), stdin, stdout, stderr, scanf(3), fread(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio_ext.h>	// for __flbf(3), __fbufsize(3)
#include <unistd.h>	// for unlink(2)
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_ZERO(), CHECK_NOT_M1(), CHECK_INT_NOERRNO()

/*
 * This example shows the different buffering schemes of the standard
 * io files.
 * It shows the buffering schemes for the standard streams: stdin,
 * stdout and stderr, as well as the buffering schemes for a newly
 * opened file for writing and reading.
 *
 * NOTES:
 * - note that output streams allocate their buffers of size 4096
 * ONLY WHEN FIRST WRITTEN TOO. That is why when you query then
 * without first writing to them you get 0 as size of buffer.
 * - same for input streams.
 * - stdin and stdout are line buffered.
 * - stderr is not buffered (size of buffer is 1).
 */
void printBuffDetails(FILE *s, const char *name) {
	printf("stats for the [%s] stream\n", name);
	printf("===============================\n");
	bool isLineBuf=(__flbf(s) > 0);
	if (isLineBuf) {
		printf("stream is line buffered\n");
	} else {
		printf("stream is not line buffered\n");
		size_t bufSize=__fbufsize(s);
		printf("size of buffer is %zd\n", bufSize);
	}
	printf("\n");
	fflush(stdout);
}

int main(int argc, char** argv, char** envp) {
	const char *writeFileName="/tmp/newfile.txt";
	const char *readFileName="/etc/passwd";

	printBuffDetails(stdin, "stdin");
	printBuffDetails(stdout, "stdout");
	printBuffDetails(stderr, "stderr");

	// lets print something to stdout
	// lets read something from stdin
	// lets print something to stderr
	fprintf(stdout, "give me a number: ");
	fflush(stdout);
	fprintf(stderr, "this is a message to stderr");
	int d;
	CHECK_INT_NOERRNO(scanf("%d", &d), 1);

	printBuffDetails(stdin, "stdin");
	printBuffDetails(stdout, "stdout");
	printBuffDetails(stderr, "stderr");

	FILE *f1=CHECK_NOT_NULL_FILEP(fopen(writeFileName, "w"));
	printBuffDetails(f1, "newly created file for writing without write(3)");
	CHECK_ZERO_ERRNO(fclose(f1));
	CHECK_NOT_M1(unlink(writeFileName));

	FILE *f2=CHECK_NOT_NULL_FILEP(fopen(readFileName, "r"));
	printBuffDetails(f2, "newly created file for reading without read(3)");
	CHECK_ZERO_ERRNO(fclose(f2));

	FILE *f3=CHECK_NOT_NULL_FILEP(fopen(writeFileName, "w"));
	const char* hello="hello";
	CHECK_INT_NOERRNO(fwrite(hello, strlen(hello), 1, f3), 1);
	char buf[1024];
	// no error return code for setbuffer(3)
	setbuffer(f3, buf, sizeof(buf));
	printBuffDetails(f3, "newly created file for writing with setbuffer(3)");
	CHECK_ZERO_ERRNO(fclose(f3));
	CHECK_NOT_M1(unlink(writeFileName));

	FILE *f4=CHECK_NOT_NULL_FILEP(fopen(writeFileName, "w"));
	CHECK_INT_NOERRNO(fwrite(hello, strlen(hello), 1, f4), 1);
	printBuffDetails(f4, "newly created file for writing with write(3)");
	CHECK_ZERO_ERRNO(fclose(f4));
	CHECK_NOT_M1(unlink(writeFileName));

	FILE *f5=CHECK_NOT_NULL_FILEP(fopen(readFileName, "r"));
	// lets read something
	CHECK_INT_NOERRNO(fread(buf, sizeof(buf), 1, f5), 1);
	printBuffDetails(f5, "newly created file for reading with read(3)");
	CHECK_ZERO_ERRNO(fclose(f5));
	return EXIT_SUCCESS;
}
