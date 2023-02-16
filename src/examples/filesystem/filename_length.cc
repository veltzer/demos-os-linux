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
#include <stdio.h>	// for printf(3)
#include <limits.h>	// for PATH_MAX, _POSIX_PATH_MAX
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for pathconf(3), _PC_PATH_MAX
#include <sys/param.h>	// for MAXPATHLEN
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows all the different ways of accessing the maximum file length in Linux.
 * As you can see different APIs return different values. Actually, the limit in Linux is
 * much higher that 4096 (you can create folders that have an absolute path of more than 4096
 * bytes) but the macro is good enough to use in most cases.
 */

int main(int argc, char** argv, char** envp) {
	printf("PATH_MAX is %d\n", PATH_MAX);
	printf("_POSIX_PATH_MAX is %d\n", _POSIX_PATH_MAX);
	printf("MAXPATHLEN is %d\n", MAXPATHLEN);
	printf("pathconf(\"/\",_PC_PATH_MAX) is %d\n", CHECK_NOT_M1(pathconf("/", _PC_PATH_MAX)));
	return EXIT_SUCCESS;
}
