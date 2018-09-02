/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for fwrite(3), fflush(3), stdout
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for read(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_INT(), CHECK_ZERO_ERRNO()
#include <security_utils.h>	// for check_root()

int main(int argc, char** argv, char** envp) {
	// lets check that we are root
	check_root();
	int fd=CHECK_NOT_M1(open("/dev/input/mouse0", O_RDONLY));
	int res;
	do {
		char buf[1024];
		res=CHECK_NOT_M1(read(fd, buf, sizeof(buf)));
		if(res>0) {
			CHECK_INT(fwrite(buf, 1, res, stdout), res);
			CHECK_ZERO_ERRNO(fflush(stdout));
		}
	} while(res!=0);
	return EXIT_SUCCESS;
}
