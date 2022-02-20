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
#include <stdio.h>	// for scanf(3), printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_INT()

void print_array(int* ar, int size) {
	int i;
	for(i=0; i<size; i++)
		printf("ar[%d]=%d\n", i, ar[i]);
}

int remove_zeros(int* ar, int size) {
	int pos=0;
	int i;
	for(i=0; i<size; i++) {
		if(ar[i]!=0)
			ar[pos++]=ar[i];
	}
	return pos;
}

int main(int argc, char** argv, char** envp) {
	// read the size of the array from the user...
	int size;
	printf("give me the size of the array: ");
	CHECK_INT(scanf("%d", &size), 1);

	// read the array from the user...
	int ar[size];
	int i;
	for(i=0; i<size; i++) {
		printf("give me the %d element: ", i);
		// scanf("%d",ar+i);
		CHECK_INT(scanf("%d", &ar[i]), 1);
	}
	// print the array before...
	print_array(ar, size);
	// remove the zeros...
	int newsize=remove_zeros(ar, size);
	// print the array after...
	printf("*************** results after *********************\n");
	print_array(ar, newsize);
	return EXIT_SUCCESS;
}
