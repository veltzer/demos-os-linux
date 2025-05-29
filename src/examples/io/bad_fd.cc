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
#include <unistd.h>	// for read(2), getdtablesize(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an example of what happens when you read from a bad fd.
 * The important thing to note is that you do not SEGFAULT. Technically
 * you called a syscall with the wrong parameter and what you get back
 * is an error.
 * Note yet that read(2) return type is ssize_t which is SIGNED as opposed to size_t
 * which is UNSIGNED. The idea here is the ability to return -1 and report errors
 * this way.
 * Note that if you pass a bad file descriptor it does not matter if you passed
 * NULL as the pointer to be read to...
 */

int main() {
	const int bad_fd=getdtablesize()+1;
	CHECK_NOT_M1(read(bad_fd, NULL, 0));
	char buf[5];
	CHECK_NOT_M1(read(bad_fd, buf, sizeof(buf)));
	return EXIT_SUCCESS;
}
