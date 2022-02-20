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

#include "rectangle.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

int main(int argc, char** argv, char** envp) {
	rectangle* r=create_rectangle(5,6);
	printf("r is at %p\n", r);
	printf("the area of the rectangle is %d\n", rectangle_area(r));
	rectangle_destroy(r);

	rectangle* r2=(rectangle*)alloca(sizeof_rectangle());
	construct_rectangle(r2, 7, 8);
	printf("r2 is at %p\n", r2);
	printf("the area of the rectangle is %d\n", rectangle_area(r2));

	int i;
	rectangle* myrectangles=(rectangle*)malloc(100*sizeof_rectangle());
	char* p=(char*)myrectangles;
	for(i=0;i<100;i++) {
		construct_rectangle((rectangle*)p, 7, 8);
		p+=sizeof_rectangle();
	}
	p=(char*)myrectangles;
	for(i=0;i<100;i++) {
		rectangle_destroy((rectangle*)p);
		p+=sizeof_rectangle();
	}

	return EXIT_SUCCESS;
}
