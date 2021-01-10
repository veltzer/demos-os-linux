/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <disassembly_utils.h>	// for disassemble_main()

/*
 * This is a simple example causing a floating point error.
 * The truth is that this code doesn't even use floating point
 * operations. The compiler understands at compile time that
 * a division by 0 will cause infinity...
 * You can see it in the disassembly...
 *
 * this is so disassembly will show interleaved code
 * EXTRA_COMPILE_FLAGS=-g3
 */

int main(int argc, char** argv, char** envp) {
	volatile float a=40;
	volatile float b=0;
	printf("a/b is %f\n", a/b);
	disassemble_main();
	return EXIT_SUCCESS;
}
