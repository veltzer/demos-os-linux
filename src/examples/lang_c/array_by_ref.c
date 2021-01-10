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
#include <stdio.h>	// for printf(3)

/*
 * This example shows how to pass an array by reference in C
 */

void print_array(double* ar, int size) {
	int i;
	for(i=0; i<size; i++)
		printf("ar[%d]=%f\n", i, ar[i]);
}

void manipulate_array(double* ar, int size) {
	int i;
	for(i=0; i<size; i++) {
		// nicest
		// ar[i]=(double)i;
		// not nice but works
		// *(ar+i)=(double)i;
		*ar=(double)i;
		ar++;
	}
}

int main(int argc, char** argv, char** envp) {
	const int SIZE=10;
	double d[10]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	print_array(d, SIZE);
	manipulate_array(d, SIZE);
	print_array(d, SIZE);
	return 0;
}
