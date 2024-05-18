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
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <lowlevel_utils.h>	// for ticks_t, getticks(), getrdtsc(), getrdtscp()

/*
 * Demo of working with my low level getticks(), getrdtsc(), getrdtscp() functions.
 */

int main() {
	ticks_t t1=getticks();
	ticks_t t1_rd=getrdtsc();
	ticks_t t1_p=getrdtscp();
	CHECK_ZERO(sleep(1));
	ticks_t t2=getticks();
	ticks_t t2_rd=getrdtsc();
	ticks_t t2_p=getrdtscp();
	printf("t1 is %lu\n", t1);
	printf("t1_rd is %lu\n", t1_rd);
	printf("t1_p is %lu\n", t1_p);
	printf("t2 is %lu\n", t2);
	printf("t2_rd is %lu\n", t2_rd);
	printf("t2_p is %lu\n", t2_p);
	printf("t2-t1 is %lu\n", t2-t1);
	return EXIT_SUCCESS;
}
