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
#include <sys/mman.h>	// for mmap(2), munmap(2)
#include <sys/types.h>	// for open(2), fstat(2)
#include <sys/stat.h>	// for open(2), fstat(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for fstat(2), close(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_VOIDP()
#include <proc_utils.h>	// for proc_print_mmap_self_filter()

/*
 * This example tried to map a read/write file to an executable space.
 * This actually works. Which means that mmap(2) in linux allows
 * you to map a file which is rw- as rwx.
 * Note that this will fail if you try to do the mapping as SHARED
 * unless ofcourse you change the permissions to read only...
 */

/*
 * A file which is kind of guaranteed to exist...
 */
const char* file_to_map="/etc/passwd";

int main(int argc, char** argv, char** envp) {
	int fd=CHECK_NOT_M1(open(file_to_map, O_RDONLY));
	struct stat stat_buf;
	CHECK_NOT_M1(fstat(fd, &stat_buf));
	void *res=CHECK_NOT_VOIDP(mmap(
			NULL,	/* addr: dont recommend address */
			stat_buf.st_size,	/* size: the size of the file */
			PROT_READ | PROT_WRITE | PROT_EXEC,	/* prot: we just want read */
			MAP_PRIVATE,	/* flags: PRIVATE or SHARED ** MUST** be specified */
			//PROT_READ,	/* prot: we just want read */
			//MAP_SHARED,	/* flags: PRIVATE or SHARED ** MUST** be specified */
			fd,	/* fd: our file descriptor */
			0	/* offset: from the begining of the file */
			), MAP_FAILED);
	// we have the mmap address, we don't need the file anymore...
	CHECK_NOT_M1(close(fd));
	proc_print_mmap_self_filter(file_to_map);
	CHECK_NOT_M1(munmap(res, stat_buf.st_size));
	return EXIT_SUCCESS;
}
