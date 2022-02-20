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
#include <unistd.h>	// for _exit(2)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>	// for fprintf(3), stderr:object
#include <string.h>	// for strcmp(3)
#include <multiproc_utils.h>	// for my_system()

/*
 * This example just calls the glibc wrapper for _exit(2). If you
 * strace this one then you will see that glibc actually calls
 * the exit_group(2) system call and not _exit(2) since the real
 * _exit(2) (the one provided by the kernel, not the wrapper)
 * will terminate just the calling thread and not other threads
 * in it's thread group.
 *
 * Notes:
 * - you can echo $? after this program is done to see
 * the exit code.
 * - note that we protect from accidentaly running this program
 * from the command line with the wrong arguments
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=1 && !(argc==2 && strcmp(argv[1], "secret")==0)) {
		fprintf(stderr, "%s: usage: %s\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	if(argc==2 && strcmp(argv[1], "secret")==0) {
		_exit(5);
	} else {
		my_system("strace %s secret", argv[0]);
	}
	return EXIT_SUCCESS;
}
