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
#include <stdio.h>	// for popen(3), pclose(3), fwrite(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_INT(), CHECK_NOT_M1()

/*
 * This is an example showing how to use popen(3) to send data to a shell
 * command.
 *
 * Remember that popen(3) wraps fork(2)/wait(2)/pipe(2)/read(2) shell and more.
 * It this example we run 'bc -l' and send questions to it.
 *
 * You need to do the following things:
 * - call open with "w" (as opposed to "r").
 * - write data to the FILE stream returned from popen.
 * - remember to pclose(3) the FILE stream.
 */

int main(int argc, char** argv, char** envp) {
	FILE* input=CHECK_NOT_NULL_FILEP(popen("bc -l", "w"));
	CHECK_INT(fwrite("2+2\n", 1, 4, input), 4);
	CHECK_INT(fwrite("3+3\n", 1, 4, input), 4);
	CHECK_NOT_M1(pclose(input));
	return EXIT_SUCCESS;
}
