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
#include <sys/time.h>	// for getrusage(2)
#include <sys/resource.h>	// for getrusage(2)

/*
 * This is a program that measures it's own cpu usage using the
 * getrusage(2), times(2), clock(3), clock_gettime(2) functions.
 *
 * TODO:
 * - write down this example.
 */

int main(int argc, char** argv, char** envp) {
	return EXIT_SUCCESS;
}
