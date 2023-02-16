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

#include <firstinclude.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for usleep(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an uncontrolled program that we want to arrange logging for...
 */
int main(int argc, char** argv, char** envp) {
	unsigned int counter=0;
	while(true) {
		printf("%d: This is yet another log message\n", counter);
		CHECK_NOT_M1(usleep(100));
		counter++;
	}
	return EXIT_SUCCESS;
}
