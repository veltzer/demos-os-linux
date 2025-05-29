/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void analyze_status(int status) {
	printf("Status value: %d (0x%x)\n\n", status, status);

	if (WIFEXITED(status)) {
		printf("WIFEXITED: true\n");
		printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
	} else {
		printf("WIFEXITED: false\n");
	}

	if (WIFSIGNALED(status)) {
		printf("WIFSIGNALED: true\n");
		printf("WTERMSIG: %d\n", WTERMSIG(status));
		#ifdef WCOREDUMP
		printf("WCOREDUMP: %s\n", WCOREDUMP(status) ? "true" : "false");
		#endif
	} else {
		printf("WIFSIGNALED: false\n");
	}

	if (WIFSTOPPED(status)) {
		printf("WIFSTOPPED: true\n");
		printf("WSTOPSIG: %d\n", WSTOPSIG(status));
	} else {
		printf("WIFSTOPPED: false\n");
	}

	#ifdef WIFCONTINUED
	printf("WIFCONTINUED: %s\n", WIFCONTINUED(status) ? "true" : "false");
	#endif
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "%s: usage: %s <status_value>\n", argv[0], argv[0]);
		fprintf(stderr, "%s: Analyzes the status value returned by wait() using standard macros\n", argv[0]);
		return EXIT_FAILURE;
	}

	int status;
	char *endptr;

	// Allow both decimal and hexadecimal input
	if (argv[1][0] == '0' && (argv[1][1] == 'x' || argv[1][1] == 'X')) {
		status = (int)strtol(argv[1], &endptr, 16);
	} else {
		status = (int)strtol(argv[1], &endptr, 10);
	}

	if (*endptr != '\0') {
		fprintf(stderr, "Error: Invalid status value\n");
		return 1;
	}

	analyze_status(status);
	return EXIT_SUCCESS;
}
