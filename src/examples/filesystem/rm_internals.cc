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
#include <stdio.h>	// for fopen(3), fgets(3), printf(3)
#include <unistd.h>	// for sleep(3)
#include <err_utils.h>	// for CHECK_NOT_NULL()
#include <stdlib.h>	// for EXIT_SUCCESS

int main(int argc, char** argv, char** envp) {
	FILE* f=fopen("myfile", "r");
	while(true) {
		char buf[1024];
		CHECK_NOT_NULL(fgets(buf, 1024, f));
		printf("yes!!! got [%s]\n", buf);
		sleep(1);
	}
	return EXIT_SUCCESS;
}
