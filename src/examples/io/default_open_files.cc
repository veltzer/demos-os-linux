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
#include <stdio.h>	// for printf(3), fileno(3), stdin, stdout, stderr
#include <stdlib.h>	// EXIT_SUCCESS
#include <unistd.h>	// getdtablesize(2), sysconf(3), fcntl(2)
#include <sys/resource.h>	// for getrlimit(2)
#include <sys/time.h>	// for getrlimit(2)
#include <fcntl.h>	// for fcntl(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This shows the default open files in linux.
 * Many ways to find this out...
 *
 * NOTES:
 * - getdtablesize() returns the MAXIMUM number of open files, not the current
 * number of open files.
 *
 * TODO:
 * - where do I get the _POSIX_OPEN_MAX and FOPEN_MAX macros
 */

/*
 *      A function to find the number of *really* open files...
 */
int get_openfds() {
	int count = 0;
	for(int fd = 0; fd < getdtablesize(); fd++) {
		int res=fcntl(fd, F_GETFD, 0);
		if(res==-1) {
			continue;
		}
		count++;
	}
	return count;
}

int main() {
	printf("getdtablesize() returned %d\n", CHECK_NOT_M1(getdtablesize()));
	printf("sysconf(_SC_OPEN_MAX) returned %d\n", CHECK_NOT_M1(sysconf(_SC_OPEN_MAX)));
	struct rlimit rlim;
	CHECK_NOT_M1(getrlimit(RLIMIT_NOFILE, &rlim));
	printf("rlim_cur is %ld\n", rlim.rlim_cur);
	printf("rlim_max is %ld\n", rlim.rlim_max);
	printf("FOPEN_MAX is %d\n", FOPEN_MAX);
	printf("_POSIX_OPEN_MAX is %d\n", _POSIX_OPEN_MAX);
	printf("fileno(stdin) is %d\n", fileno(stdin));
	printf("fileno(stdout) is %d\n", fileno(stdout));
	printf("fileno(stderr) is %d\n", fileno(stderr));
	printf("get_openfds() is %d\n", get_openfds());
	return EXIT_SUCCESS;
}
