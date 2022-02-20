/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3), fopen(3), getline(3), feof(3)
#include <stdlib.h>	// for EXIT_SUCCESS, free(3)
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_NOT_M1(), CHECK_ZERO_ERRNO()

/*
 * This example demonstrates the use of the getline(3) function
 *
 * NOTES
 * - at the end you need to free the memory yourself.
 */

int main(int argc, char** argv, char** envp) {
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	FILE* fp=CHECK_NOT_NULL_FILEP(fopen("/etc/motd", "r"));
	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line of length %zu :\n", read);
		printf("len is %zd :\n", len);
		printf("%s", line);
	}
	if(!feof(fp)) {
		CHECK_NOT_M1(read);
	}
	free(line);
	CHECK_ZERO_ERRNO(fclose(fp));
	return EXIT_SUCCESS;
}
