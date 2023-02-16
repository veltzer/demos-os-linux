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
#include <unistd.h>	// for pathconf(2)
#include <stdio.h>	// for printf(3), fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <us_helper.h>	// for stringify()
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows the various values returned by path conf for the folder you give it...
 */

#define PRINT_PATHCONF(path, name) printf("pathconf(\"%s\",%s)=%d\n", path, stringify(name), CHECK_NOT_M1(pathconf(path, name)))

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [folder]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* path=argv[1];
	PRINT_PATHCONF(path, _PC_LINK_MAX);
	PRINT_PATHCONF(path, _PC_MAX_CANON);
	PRINT_PATHCONF(path, _PC_MAX_INPUT);
	PRINT_PATHCONF(path, _PC_NAME_MAX);
	PRINT_PATHCONF(path, _PC_PATH_MAX);
	PRINT_PATHCONF(path, _PC_PIPE_BUF);
	PRINT_PATHCONF(path, _PC_CHOWN_RESTRICTED);
	PRINT_PATHCONF(path, _PC_NO_TRUNC);
	PRINT_PATHCONF(path, _PC_VDISABLE);
	return EXIT_SUCCESS;
}
