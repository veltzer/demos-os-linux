/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for scanf(3), putchar(3), fflush(3)
#include <unistd.h>	// for usleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_INT(), CHECK_NOT_M1()

int main(int argc, char** argv, char** envp) {
	int num;
	int direction=1;
	printf("Give me a number of stars: ");
	fflush(stdout);
	CHECK_INT(scanf("%d", &num), 1);
	while(true) {
		int i;
		for(i=(num-1)*(direction==-1); direction==1 ? i<num-1 : i>=0; i+=direction) {
			int j;
			for(j=0; j<num-i-1; j++) {
				putchar(' ');
			}
			for(j=0; j<i+1; j++) {
				putchar('*');
				putchar(' ');
			}
			for(j=0; j<num-i-1; j++) {
				putchar(' ');
			}
			putchar('\r');
			fflush(stdout);
			CHECK_NOT_M1(usleep(1000000));
		}
		// flip the direction
		direction*=-1;
	}
	return EXIT_SUCCESS;
}
