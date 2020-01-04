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

/*
 * this is an example of doing the exit system call in assembly...
 * you can see this is running ok if you use 'echo $?' after the run
 * and see the return code coming up ok...
 */

#include <asm/unistd.h>
#include <syscall.h>

.text
.global main
.type main, @function
main:
	## the system call we want (from syscall.h)
	movl $SYS_exit, %eax
	## the value we want to return
	movl $7, %ebx
	## call the system call itself
	int $0x80
