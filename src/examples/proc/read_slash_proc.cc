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
#include <string.h>	// for memset(3)
#include <stdio.h>	// for printf(3)
#include <sys/resource.h>	// for getrlimit(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows how you you can read /proc/self/stack
 * It turns out that you can't by default on ubuntu
 * You need to either:
 * - add capabities to your user
 * - remove some security features from the kernel
 * either in compilation or at runtime at /proc
 * - run this as sudo.
 */

int main(int argc, char** argv, char** envp) {
	char *line = NULL;
	size_t len = 0;
	ssize_t read_size;
	FILE* fp=CHECK_NOT_NULL_FILEP(fopen("/proc/self/stack", "r"));
	while (CHECK_NOT_M1(read_size=getline(&line, &len, fp))) {
		puts(line);
	}
	CHECK_ZERO_ERRNO(fclose(fp));
	return EXIT_SUCCESS;
}
