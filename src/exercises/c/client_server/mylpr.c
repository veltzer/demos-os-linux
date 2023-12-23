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
#include <sys/param.h>	// for MAXPATHLEN
#include <sys/types.h>	// for lseek(2)
#include <fcntl.h>	// for fcntl(2)
#include <unistd.h>	// for lseek(2), write(2), read(2), fcntl(2)
#include <stdio.h>	// for fprintf(3)
#include <string.h>	// for strcpy(3)
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3), EXIT_FAILURE
#include <err_utils.h>	// for CHECK_NOT_M1()

const int MAXINDEXSIZE=128;

struct index {
	unsigned int ID;
	char path[MAXPATHLEN];
};

int main(int argc, char** argv, char** envp) {
	if(argc < 2) {
		fprintf(stderr, "%s: usage: %s file\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}
	int fdindex=CHECK_NOT_M1(open("lpindex", O_RDWR|O_CREAT, 0666));
	struct flock lplock;
	lplock.l_type=F_WRLCK;
	lplock.l_whence=SEEK_SET;
	lplock.l_start=0;
	lplock.l_len=0;
	CHECK_NOT_M1(fcntl(fdindex, F_SETLKW, &lplock));
	int currid;
	int buffersize=CHECK_NOT_M1(read(fdindex, &currid, sizeof(int)));
	if (buffersize==0 || currid > MAXINDEXSIZE) {
		currid=0;
	}
	struct index buffer;
	CHECK_NOT_M1(lseek(fdindex, sizeof(int) + currid * sizeof(buffer), SEEK_SET));
	CHECK_NOT_M1(read(fdindex, &buffer, sizeof(buffer)));
	if(buffer.ID>0) {
		fprintf(stderr, "Index table full. Retry print later");
		exit(EXIT_FAILURE);
	}
	CHECK_NOT_M1(lseek(fdindex, sizeof(int) + currid * sizeof(buffer), SEEK_SET));
	buffer.ID=currid+1;
	strcpy(buffer.path, argv[1]);
	CHECK_NOT_M1(write(fdindex, &buffer, sizeof(buffer)));
	printf("file %s spooled, JobID: %d\n", buffer.path, buffer.ID);
	CHECK_NOT_M1(lseek(fdindex, 0, SEEK_SET));
	currid++;
	CHECK_NOT_M1(write(fdindex, &currid, sizeof(int)));
	lplock.l_type=F_UNLCK;
	CHECK_NOT_M1(fcntl(fdindex, F_SETLK, &lplock));
	return EXIT_SUCCESS;
}
