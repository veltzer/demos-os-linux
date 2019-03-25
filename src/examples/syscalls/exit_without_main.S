/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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

/*
 * this is an example of doing the exit system call in assembly...
 * you can see this is running ok if you use 'echo $?' after the run
 * and see the return code coming up ok...
 *
 * Notes:
 * - -nostdlib is needed to avoid duplicate symbol for _start and avoid
 * excess size of the executable.
 * - -Xlinker --strip-all is to reduce size of the executable
 * - -Xlinker --build-id=none is to remove the linkers build-id stamp and
 * so reduce the size of the executable even more.
 * - use 'ls -l' to see just how small the executable file is.
 * - use 'ldd' to see that this executable does not link with anything.
 * - use 'objdump -D [exefile]' to see how little content this executable has.
 *
 * EXTRA_LINK_FLAGS=-nostdlib -Xlinker --strip-all -Xlinker --build-id=none
 */

#include <asm/unistd.h>
#include <syscall.h>

.text
.global _start
.type _start, @function
_start:
	## the system call we want (from syscall.h)
	movl $SYS_exit, %eax
	## the value we want to return
	movl $7, %ebx
	## call the system call itself
	int $0x80
