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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int array[6]={5,3,8,2,9,1};

int main() {
	int arr[6];
	int vals=scanf("%d %d %d %d %d %d", arr, arr+1, arr+2, arr+3, arr+4, arr+5);
	assert(vals==6);
	int prev=-1000;
	for(int i=0;i<6;i++) {
		int val=array[arr[i]];
		if(val<=prev) {
			fprintf(stderr, "Boom!\n");
			exit(1);
		}
		prev=val;
	}
	return 0;
}
