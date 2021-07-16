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
#include <stdlib.h>	// for atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>	// for printf(3), fprintf(3)
#include <multiproc_utils.h>	// for print_status()

/*
 * This executable receives the status code (exit code) of some process and prints
 * an analysis of what the code means. Useful in case where you are not sure what
 * the status code means or as a demo of how to analyze such a code.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [status code]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: 0 - the process ended with success\n", argv[0]);
		fprintf(stderr, "%s: n*256 - the process ended with error n\n", argv[0]);
		fprintf(stderr, "%s: n<256 - the process was killed by OS signal n\n", argv[0]);
		return EXIT_FAILURE;
	}
	print_status(atoi(argv[1]));
	return EXIT_SUCCESS;
}
