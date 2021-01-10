/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>
#include <string.h>

/*
 * This is to check some assumptions on bit operations
 */

#define COMB(a, b) (a | b)
const unsigned int cone=0x0000f000;
const unsigned int ctwo=0x0000f001;

#define one 0x00010000
#define two 0x706

int main(int argc, char** argv, char** envp) {
	printf("COMB(0x800,%d) is %d\n", cone, COMB(0x800, cone));
	printf("COMB(0x801,%d) is %d\n", cone, COMB(0x801, cone));
	printf("COMB(%d,%d) is %d\n", one, two, COMB(one, two));
	return(0);
}
