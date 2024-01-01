/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h> // for printf(3)
#include <stdlib.h> // for EXIT_SUCCESS
#include <sys/mman.h> // for mprotect(2), PROT_READ, PROT_WRITE, PROT_EXEC
#include <err_utils.h> // for CHECK_NOT_M1()
#include <proc_utils.h> // for proc_get_stack_info(), proc_print_mmap_self()

/*
 * This example shows how to make the stack executable.
 * We basically need the stack size and address and call mprotect(2).
 * For a discussion on how to get the stack information see the
 * "find_stack_start" example.
 */

int main(int argc, char** argv, char** envp) {
	unsigned long adr_start, adr_end;
	proc_get_stack_info(&adr_start, &adr_end);
	printf("before...\n");
	proc_print_mmap_self_filter("[stack]");
	CHECK_NOT_M1(mprotect((void*)adr_start, adr_end-adr_start, PROT_READ | PROT_WRITE | PROT_EXEC));
	printf("after...\n");
	proc_print_mmap_self_filter("[stack]");
	return EXIT_SUCCESS;
}
