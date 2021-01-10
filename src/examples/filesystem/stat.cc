/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for stderr, fprintf(3), printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for stat(2)
#include <sys/stat.h>	// for stat(2)
#include <unistd.h>	// for stat(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <time.h>	// for ctime(3)
#include <timespec_utils.h>	// for timespec_snprintf()

/*
 * This example is similar to the command line stat(1) one and
 * is similar in essence to the example in the stat(2) manual page.
 */

int main(int argc, char** argv, char** envp) {
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [file]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /etc/passwd\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* filename=argv[1];
	// code
	struct stat sb;
	CHECK_NOT_M1(stat(filename, &sb));
	printf("File type: ");
	switch (sb.st_mode & S_IFMT) {
	case S_IFBLK: printf("block device\n"); break;
	case S_IFCHR: printf("character device\n"); break;
	case S_IFDIR: printf("directory\n"); break;
	case S_IFIFO: printf("FIFO/pipe\n"); break;
	case S_IFLNK: printf("symlink\n"); break;
	case S_IFREG: printf("regular file\n"); break;
	case S_IFSOCK: printf("socket\n"); break;
	default: printf("unknown?\n"); break;
	}
	printf("I-node number: %ld\n", (long) sb.st_ino);
	printf("Mode: %lo (octal)\n", (unsigned long) sb.st_mode);
	printf("Link count: %ld\n", (long) sb.st_nlink);
	printf("Ownership: UID=%ld GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
	printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
	printf("File size: %lld bytes\n", (long long) sb.st_size);
	printf("Blocks allocated: %lld\n", (long long) sb.st_blocks);
	printf("Low resolution times (seconds since the epoch):\n");
	printf("Last status change: %s", ctime(&sb.st_ctime));
	printf("Last file access: %s", ctime(&sb.st_atime));
	printf("Last file modification: %s", ctime(&sb.st_mtime));
	printf("High resolution times (nanosecond level):\n");
	char buf[300];
	timespec_snprintf(buf, sizeof(buf), &sb.st_ctim, 1);
	printf("Last status change: %s\n", buf);
	timespec_snprintf(buf, sizeof(buf), &sb.st_atim, 1);
	printf("Last file access: %s\n", buf);
	timespec_snprintf(buf, sizeof(buf), &sb.st_mtim, 1);
	printf("Last file modification: %s\n", buf);
	return EXIT_SUCCESS;
}
