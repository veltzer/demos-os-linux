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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <err_utils.h>

int main() {
	const char *name = "example_memfd";
	const char *data = "Hello, memfd!";
	char buf[20];

	// Create a memfd
	int fd =CHECK_NOT_M1(memfd_create(name, 0));

	// Write data to the memfd
	int ret = CHECK_NOT_M1(write(fd, data, strlen(data)));

	// Seek to the beginning of the file
	CHECK_NOT_M1(lseek(fd, 0, SEEK_SET));

	// Read data from the memfd
	ret = CHECK_NOT_M1(read(fd, buf, sizeof(buf) - 1));
	buf[ret] = '\0';

	printf("Read from memfd: %s\n", buf);

	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
