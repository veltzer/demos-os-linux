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

#include <firstinclude.h>
#include <stdio.h>	// for printf(3), getchar(3)
#include <stdlib.h>	// for EXIT_SUCCESS

int main(int argc, char** argv, char** envp) {
	char c=getchar();
	while(c!=EOF) {
		printf("got a single character whose code is %d, and value is %c\n", c, c);
		c=getchar();
	}
	printf("Got EOF, you probably hit CTRL+D\n");
	return EXIT_SUCCESS;
}
