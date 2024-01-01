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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()

struct index {
	unsigned int ID;
	char path[MAXPATHLEN];
};

struct index buffer;
int bufsize;
int fdindex;
int cri1done;
int cri2done;
int currid;

void critical1() {
	char lpindexFname[MAXNAMLEN];
	struct stat buff;
	sprintf(lpindexFname, "/tmp/lpindex.%d", getpid());
	fdindex=CHECK_NOT_M1(open(lpindexFname, O_RDWR|O_CREAT, 0666));
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
		bufsize=CHECK_NOT_M1(read(fdindex, &buffer, sizeof(buffer)));
	}
	if (buffer.ID > 0) {
		printf("Now printing job: %d file: %s\n", buffer.ID, buffer.path);
	}
}

void critical2() {
	CHECK_NOT_M1(lseek(fdindex, -1 * sizeof(buffer), SEEK_CUR));
	buffer.ID=0;
	CHECK_NOT_M1(write(fdindex, &buffer, sizeof(buffer)));
}

void sigint(int gotsig) {
	struct dirent* dircontent;
	char pathname[MAXPATHLEN];
	struct stat statbuf;
	char strPID[10];
	DIR* sdir=(DIR*)CHECK_NOT_NULL(opendir("/tmp"));
	if (cri1done && !cri2done) {
		critical2();
	}
	while((dircontent=readdir(sdir))) {
		if ((strcmp(dircontent->d_name, "." )==0) || strcmp(dircontent->d_name, "..")==0)
			continue;
		sprintf(strPID, "%d", getpid());
		if (strstr(dircontent->d_name, strPID)) {
			sprintf(pathname, "/tmp/%s", dircontent->d_name);
			CHECK_NOT_M1(lstat(pathname, &statbuf));
			if (S_ISREG(statbuf.st_mode)) {
				if (statbuf.st_uid==getuid()) {
					CHECK_NOT_M1(unlink(pathname));
				}
			}
		}
	}
}

int main(int argc, char** argv, char** envp) {
	sigset_t currentset, settoblock;
	struct sigaction act;
	sigemptyset(&settoblock);
	sigemptyset(&currentset);
	sigaddset(&settoblock, SIGINT);
	sigaddset(&settoblock, SIGQUIT);
	sigaddset(&settoblock, SIGABRT);
	sigaddset(&settoblock, SIGTERM);
	act.sa_handler=sigint;
	act.sa_mask=settoblock;
	CHECK_NOT_M1(sigaction(SIGINT, &act, NULL));
	CHECK_NOT_M1(sigaction(SIGQUIT, &act, NULL));
	CHECK_NOT_M1(sigaction(SIGABRT, &act, NULL));
	CHECK_NOT_M1(sigaction(SIGTERM, &act, NULL));

	cri1done=cri2done=0;
	printf("blocking signals\n");
	CHECK_NOT_M1(sigprocmask(SIG_BLOCK, &settoblock, &currentset));
	printf("doing critical 1\n");
	critical1();
	cri1done=1;
	CHECK_ZERO(sleep(15));
	printf("doing critical 2\n");
	critical2();
	cri2done=1;
	CHECK_NOT_M1(sigprocmask(SIG_SETMASK, &currentset, NULL));
	fprintf(stderr, "enabling signals\n");
	return EXIT_SUCCESS;
}
