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
#include <sys/types.h>	// for ftok(3), key_t
#include <sys/ipc.h>	// for ftok(3), key_t
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that you actually need to point to a real existing file
 * name in order for ftok to succeed...
 * Notes
 * - ftok(3) is a userspace function. if you strace this program
 * you will not see any ftok(2) system call or the like.
 * - ftok(3) is deteministic. You can see this if you run this program several times.
 * - the 'proj_id' is just an arbitrary integer that the client and server have to
 * agree upon.
 */

int main(int argc, char** argv, char** envp) {
	const int proj_id=17;
	key_t k1=ftok("/tmp/no_such_file", proj_id);
	printf("key with no file is %d\n", k1);
	key_t k2=ftok("/etc/passwd", proj_id);
	printf("key with file is %d\n", k2);
	return EXIT_SUCCESS;
}
