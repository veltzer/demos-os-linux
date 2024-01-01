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
#include <stdio.h>	// for printf(3), scanf(3), fflush(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3)
#include <err_utils.h>	// for CHECK_INT()

float get_average(float* p, int size) {
	int i;
	float sum=0;
	for(i=0; i<size; i++) {
		sum+=p[i];
		/* sum+=*(p+i); */
	}
	return sum/size;
}

int main(int argc, char** argv, char** envp) {
	int size;
	float* p;
	int i;

	printf("How many elements do you have? ");
	// there is not need to flush since scanf will flush
	// as well
	/* fflush(stdout); */
	CHECK_INT(scanf("%d", &size), 1);
	p=(float*)malloc(sizeof(float)*size);
	for(i=0; i<size; i++) {
		printf("please give me the %d'th element: ", i);
		CHECK_INT(scanf("%f", &p[i]), 1);
		/* scanf("%f",p+i); */
	}
	printf("the average is %f\n", get_average(p, size));
	free(p);
	return EXIT_SUCCESS;
}
