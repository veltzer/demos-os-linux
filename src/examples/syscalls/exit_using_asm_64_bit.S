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

/*
 * this is an example of doing the exit system call in assembly...
 * you can see this is running ok if you use 'echo $?' after the run
 * and see the return code coming up ok...
 *
 * The next flags are needed:
 * EXTRA_COMPILE_FLAGS=-static -m64
 * EXTRA_LINK_FLAGS=-static -m64 -nostdlib
 */

#include <asm/unistd.h>
#include <syscall.h>

.text
.global _start
_start:
## _exit(7)
movq $60, %rax # use the `_exit` [fast] syscall
movq $7, %rdi # error code 0
syscall # make syscall
