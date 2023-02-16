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
#include <unistd.h>	// for fork(2), sleep(3), lseek(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/types.h>	// for waitpid(2), open(2), lseek(2)
#include <sys/wait.h>	// for waitpid(2)
#include <limits.h>	// for PATH_MAX
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)

/*
 * This example shows how one process can inspect the memory of another
 * using the /proc file: /proc/pid/mem.
 * In this case the parent inspects the memory of the child.
 *
 * NOTES:
 * - you have to have the right permissions to read the file.
 * - in this case we just read the value, we could also write to it if we wanted.
 */

int global=0;

int main(int argc, char** argv, char** envp) {
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid) {
		char filename[PATH_MAX];
		snprintf(filename, PATH_MAX, "/proc/%d/mem", child_pid);
		int fd=CHECK_NOT_M1(open(filename, O_RDONLY));
		for(int i=0;i<5;i++) {
			CHECK_NOT_M1(lseek(fd, (unsigned long long)&global, SEEK_SET));
			int val;
			CHECK_INT(read(fd, &val, sizeof(int)), sizeof(int));
			printf("val is [%d]\n", val);
			sleep(1);
		}
		CHECK_NOT_M1(close(fd));
		int status;
		CHECK_NOT_M1(waitpid(child_pid, &status, 0));
	} else {
		for(int i=0;i<10;i++) {
			global++;
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}
