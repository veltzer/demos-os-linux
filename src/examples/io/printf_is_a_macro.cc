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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that printf(3) is really a marco in the standard glibc library.
 *
 * How does it work?
 * It checks the number of arguments passed to it and if it is 1 then it converts into
 * a call to puts(3), if the number of arguments is greater than 1 then it really calls
 * printf(3).
 *
 * This is all based on the feature that allows macros to ask with how many arguments they
 * are called. This happens at compile time, not run time.
 *
 * How can you see that?
 * $ nm [this executable] | grep puts
 */

int main(int argc, char** argv, char** envp) {
	printf("Hello, World!\n");
	return EXIT_SUCCESS;
}
