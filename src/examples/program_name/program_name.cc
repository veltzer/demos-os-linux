/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <us_helper.h>	// for get_program_name()

/*
 * This example shows the various ways with which to retrieve the current
 * applications name. Some GNU only (program_invocation*), some Linux
 * only (/proc), some standard (argv[0]).
 */

extern const char* __progname;
extern char* program_invocation_short_name;
extern char* program_invocation_name;

int main(int argc, char** argv, char** envp) {
	printf("argv[0] is %s\n", argv[0]);
	printf("__progname is %s\n", __progname);
	printf("program_invocation_short_name is %s\n", program_invocation_short_name);
	printf("program_invocation_name is %s\n", program_invocation_name);
	char mybuf[1024];
	get_program_name(mybuf, sizeof(mybuf));
	printf("program name is %s\n", mybuf);
	return EXIT_SUCCESS;
}
