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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>	// for fprintf(3), stderr
#include <disassembly_utils.h>	// for disassemble_function()

/*
 * This example shows the disassembly of a single system call.
 *
 * We must link static to have the system call code in our own executable
 * EXTRA_LINK_FLAGS=-static
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=1) {
		fprintf(stderr, "%s: usage: %s\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	disassemble_function("_exit");
	return EXIT_SUCCESS;
}
