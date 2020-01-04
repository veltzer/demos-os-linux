/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/stat.h>	// for mkdir(2), stat(2), open(2)
#include <sys/types.h>	// for mkdir(2), stat(2), open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for rmdir(2), stat(2), unlink(2), sleep(3)
#include <stdio.h>	// for snprintf(3), printf(3)
#include <time.h>	// for time(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO()
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <limits.h>	// for PATH_MAX

/*
 * This example shows that when you change a file in a directory then
 * the directories mtime changes.
 *
 * Notes:
 * - regular stat(2) time resolution is 1 second although modern linux
 * systems provide nano second support but via more fields in the stat
 * structure.
 * - that is why we sleep between the creation of the dir and the creation
 * of the file.
 * - note that if we add a file in a sub sub sub folder the times which are
 * affected are only those of the directory containting the change
 * (and not it's parent folders).
 * - this has ramifications for how to know if something changed in a folder.
 * if the folder has no subfolders then you can just look at it's time.
 * if it has subfolders you have to scan all the files and directories
 * (directories will do).
 */

// the directory we will be creating...
const char* dirname="tmpdir";
const char* filename="tmpfile";

void show_mtime(const char* filename) {
	struct stat buf;
	CHECK_NOT_M1(stat(filename, &buf));
	// printf("Data for %s\n",filename);
	// printf("Last status change: %s", ctime(&buf.st_ctime));
	printf("%s ctime: %s", filename, ctime(&buf.st_ctime));
	printf("%s atime: %s", filename, ctime(&buf.st_atime));
	printf("%s mtime: %s", filename, ctime(&buf.st_mtime));
}

int main(int argc, char** argv, char** envp) {
	// lets prepare some variables...
	mode_t mode_all=S_IRWXU | S_IRWXG | S_IRWXO;
	char dirname2[PATH_MAX];
	snprintf(dirname2, PATH_MAX, "%s/%s", dirname, dirname);
	char dirname3[PATH_MAX];
	snprintf(dirname3, PATH_MAX, "%s/%s/%s", dirname, dirname, dirname);
	char fullname[PATH_MAX];
	snprintf(fullname, PATH_MAX, "%s/%s/%s/%s", dirname, dirname, dirname, filename);
	// lets make the directories...
	CHECK_NOT_M1(mkdir(dirname, mode_all));
	CHECK_NOT_M1(mkdir(dirname2, mode_all));
	CHECK_NOT_M1(mkdir(dirname3, mode_all));
	// lets check the mtime of the various dirs...
	show_mtime(dirname);
	show_mtime(dirname2);
	show_mtime(dirname3);
	// lets sleep before we create the file to allow at least
	// one second (the regular stat(2) time resolution) to elapse...
	CHECK_ZERO(sleep(2));
	// now lets create a file inside the directory...
	int d=CHECK_NOT_M1(open(fullname, O_CREAT|O_EXCL, mode_all));
	// lets close the file
	CHECK_NOT_M1(close(d));
	// lets check the mtime of the various dirs...
	show_mtime(dirname);
	show_mtime(dirname2);
	show_mtime(dirname3);
	// now lets delete the file
	CHECK_NOT_M1(unlink(fullname));
	// lets delete the directories (reverse order)...
	CHECK_NOT_M1(rmdir(dirname3));
	CHECK_NOT_M1(rmdir(dirname2));
	CHECK_NOT_M1(rmdir(dirname));
	return EXIT_SUCCESS;
}
