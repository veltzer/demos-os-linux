/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>	// for open(2), fstat(2), lseek(2)
#include <sys/stat.h>	// for open(2), fstat(2)
#include <fcntl.h>	// for fcntl(2), open(2)
#include <unistd.h>	// for write(2), fcntl(2), fstat(2), read(2), lseek(2)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/param.h>	// for MAXPATHLEN:const
#include <err_utils.h>	// for CHECK_NOT_M1()

const int MAXINDEXSIZE=128;

struct index {
	unsigned int ID;
	char path[MAXPATHLEN];
};

int main(int argc, char** argv, char** envp) {
	struct index buffer;
	int bufsize;
	int currid;
	struct flock lplock;
	struct stat buff;

	int fdindex=CHECK_NOT_M1(open("lpindex", O_RDWR|O_CREAT, 0666));
	lplock.l_type=F_WRLCK;
	lplock.l_whence=SEEK_SET;
	lplock.l_start=0;
	lplock.l_len=0;
	CHECK_NOT_M1(fcntl(fdindex, F_SETLKW, &lplock));
	CHECK_NOT_M1(fstat(fdindex, &buff));
	if (buff.st_size==0) {
		currid=0;
		CHECK_NOT_M1(write(fdindex, &currid, sizeof(currid)));
		buffer.ID=0;
		buffer.path[0]='\0';
		CHECK_NOT_M1(write(fdindex, &buffer, sizeof(buffer)));
	}
	CHECK_NOT_M1(read(fdindex, &currid, sizeof(int)));
	bufsize=CHECK_NOT_M1(read(fdindex, &buffer, sizeof(buffer)));
	while(buffer.ID==0 && bufsize > 0) {
		CHECK_NOT_M1(read(fdindex, &buffer, sizeof(buffer)));
	}
	if (buffer.ID > 0) {
		printf("Now printing job: %d file: %s\n", buffer.ID, buffer.path);
	}
	CHECK_NOT_M1(lseek(fdindex, -1 * sizeof(buffer), SEEK_CUR));
	buffer.ID=0;
	CHECK_NOT_M1(write(fdindex, &buffer, sizeof(buffer)));
	lplock.l_type=F_UNLCK;
	CHECK_NOT_M1(fcntl(fdindex, F_SETLK, &lplock));
	return EXIT_SUCCESS;
}
