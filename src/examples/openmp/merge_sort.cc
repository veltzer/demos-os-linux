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
#include <omp.h>// for openmp pragmas and functions
#include <stdlib.h>	// for random(3)
#include <string.h>	// for memcpy(3)

/*
 * An example of writing a merge sort algorithm using open mp.
 *
 * EXTRA_COMPILE_FLAGS=-fopenmp
 * EXTRA_LINK_FLAGS_AFTER=-fopenmp
 */

void mergesort(int* arr, unsigned int from, unsigned int to, int* scratch) {
	if(from+1==to) {
		return;
	}
	unsigned int length=to-from;
	unsigned int mid=(from+to)/2;
	// if you put the number of threads here omp will not exceed them
	// if you don't then omp will use the number of your cores...
	// see this via ps -eLf...
	// omp_set_num_threads(2);
	#pragma omp parallel sections
	{
		#pragma omp section
		mergesort(arr, from, mid, scratch);
		#pragma omp section
		mergesort(arr, mid, to, scratch);
	}
	memcpy(scratch+from, arr+from, length*sizeof(int));
	unsigned int i=mid;
	unsigned int j=from;
	unsigned int target=from;
	while(target<to) {
		if(i<to && j<mid) {
			if(scratch[i]<scratch[j]) {
				arr[target]=scratch[i];
				i++;
			} else {
				arr[target]=scratch[j];
				j++;
			}
		} else {
			if(i<to) {
				arr[target]=scratch[i];
				i++;
			} else {
				arr[target]=scratch[j];
				j++;
			}
		}
		target++;
	}
}

void mergesort(int* arr, unsigned int size) {
	int* scratch=new int[size];
	mergesort(arr, 0, size, scratch);
	delete[] scratch;
}

int main() {
	const unsigned int size=100000000;
	int* arr=new int[size];
	for(unsigned int i=0; i<size; i++) {
		arr[i]=random();
	}
	// for debugging...
	// const unsigned int size=4;
	// int arr[size]={ 4,5,7,6 };
	mergesort(arr, size);
	for(unsigned int i=0; i<size; i++) {
		printf("%d: %d\n", i, arr[i]);
	}
	return EXIT_SUCCESS;
}
