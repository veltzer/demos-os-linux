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
 * An example of how to write a classic "Hello, World!\n" program in assembly
 * see: https://en.wikibooks.org/wiki/X86_Assembly/Interfacing_with_Linux
 *
 * The next flags are needed:
 * EXTRA_COMPILE_FLAGS=-static -m64
 * EXTRA_LINK_FLAGS=-static -m64 -nostdlib
 */

#include <asm/unistd.h>
#include <syscall.h>

.section .rodata
hello:
.ascii "Hello, world!\n"

.section .text
.global _start
_start:
## write(1,hello,14)
movq $1, %rax # use the `write` [fast] syscall
movq $1, %rdi # write to stdout
movq $hello, %rsi # use string "Hello World"
movq $14, %rdx # write 14 characters
syscall # make syscall

## _exit(7)
movq $60, %rax # use the `_exit` [fast] syscall
movq $0, %rdi # error code 0
syscall # make syscall
