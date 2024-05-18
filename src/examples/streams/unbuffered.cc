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
#include <stdio.h>	// for printf(3), fopen(3), fseek(3), fread(3), fgets(3), setvbuf(3), _IONBF
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_NOT_M1(), CHECK_ZERO_ERRNO(), CHECK_INT_NOERRNO(), CHECK_ZERO(), CHECK_NOT_NEGATIVE()
#include <unistd.h>	// for sleep(3)

/*
 * This example shows how to prevent buffering.
 * the weird thing is that fread(3) does not do caching
 * while fgets(3) does.
 * another way to prevent the caching is to call setvbuf(3)
 * and then it fgets(3) will work too.
 */

int main() {
	FILE* fp=CHECK_NOT_NULL_FILEP(fopen("/proc/uptime", "r"));
	CHECK_NOT_M1(setvbuf(fp, NULL, _IONBF, 0));
	while(true) {
		const unsigned int SIZE=256;
		char line[SIZE];
		// CHECK_NOT_NEGATIVE(fread(line, SIZE, 1, fp));
		CHECK_NOT_NULL(fgets(line, SIZE, fp));
		printf("got value [%s]\n", line);
		CHECK_ZERO(sleep(1));
		// no return value
		rewind(fp);
	}
	CHECK_ZERO_ERRNO(fclose(fp));
	return EXIT_SUCCESS;
}
