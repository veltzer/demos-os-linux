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
#include <err_utils.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for malloc(3)
#include <stdio.h>	// for printf(3)
#include <string.h>	// for memset(3)

/*
 * This example proves that glibc does not protect against
 * accidentaly passing a non const char* string as format
 * argument to printf(3)
 */

int main() {
	char* malicious_string=(char*)CHECK_NOT_NULL(malloc(20));
	memset(malicious_string, 0, 20);
	printf(malicious_string, 45);
	free(malicious_string);
	return EXIT_SUCCESS;
}
