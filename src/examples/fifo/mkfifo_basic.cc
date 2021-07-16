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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for mkfifo(2)
#include <sys/stat.h>	// for mkfifo(2)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <stdio.h>	// for printf(3)

/*
 * This example shows basic usage of mkfifo(2)
 */

int main(int argc, char** argv, char** envp) {
	const char* filename="/tmp/myfifo";
	CHECK_NOT_M1(mkfifo(filename, 0666));
	int fd=CHECK_NOT_M1(open(filename, O_RDONLY));
	while(true) {
		char buf[256];
		ssize_t ret=CHECK_NOT_M1(read(fd, buf, sizeof(buf)));
		printf("got %zd bytes...\n", ret);
	}
	return EXIT_SUCCESS;
}
