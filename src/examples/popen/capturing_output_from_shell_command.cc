/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3), popen(3), pclose(3), fread(3), feof(3), fwrite(3), stdout:object
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_INT(), CHECK_ZERO(), CHECK_NOT_M1()

/*
 * This is an example showing how to capture the output stream comming out of a child
 * process running a shell command of your choosing.
 *
 * Remember that popen(3) wraps fork(2)/wait(2)/pipe(2)/read(2) shell and more.
 * It this example we run 'ls /etc' and read the results from it.
 *
 * You need to do the following things:
 * - call popen with "r" (as opposed to "w")
 * - read the resulting FILE stream until eof of file.
 * - do something with the output (in this example we simply print it out to the screen).
 * - pclose(3) the stream.
 */

int main() {
	FILE* input=CHECK_NOT_NULL_FILEP(popen("ls /etc", "r"));
	while(!feof(input)) {
		const unsigned int size=256;
		char buf[size];
		size_t num=fread(buf, 1, size, input);
		if(num>0) {
			CHECK_INT(fwrite(buf, 1, num, stdout), num);
		} else {
			CHECK_ZERO(ferror(input));
		}
	}
	CHECK_NOT_M1(pclose(input));
	return EXIT_SUCCESS;
}
