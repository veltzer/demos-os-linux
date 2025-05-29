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
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <dirent.h>	// for DT_* constants
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <us_helper.h>	// for syscall_getdents(), struct linux_dirent
#include <trace_utils.h>// for INFO()
#include <dir_utils.h>	// for dir_get_by_val()

/*
 * This example shows how to read the content of a directory using
 * the getdents(2) system call.
 *
 * A great part of this example was shamelessly stolen from the getdents(2)
 * manual page.
 */

int main(int argc, char** argv) {
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [dir]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /etc\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* dirname=argv[1];
	// code
	int fd=CHECK_NOT_M1(open(dirname, O_RDONLY | O_DIRECTORY));
	char buf[1024];
	while(true) {
		int nread=CHECK_NOT_M1(syscall_getdents(fd, (struct linux_dirent*)buf, sizeof(buf)));
		if(nread==0) {
			break;
		}
		printf("--------------- nread=%d ---------------\n", nread);
		printf("%-8s %-10s %-8s %-10s %s\n",
			"i-node#",
			"file type",
			"d_reclen",
			"d_off",
			"d_name"
			);
		int bpos=0;
		while(bpos < nread) {
			struct linux_dirent * d = (struct linux_dirent *) (buf + bpos);
			char d_type = *(buf + bpos + d->d_reclen - 1);
			printf("%8ld %-10s %8d %10lu %s\n",
				d->d_ino,
				dir_get_by_val(d_type),
				d->d_reclen,
				d->d_off,
				d->d_name
				);
			bpos += d->d_reclen;
		}
	}
	return EXIT_SUCCESS;
}
