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

/*
 * The reason we need the "__attribute__((aligned (8)))" tag below is to avoid
 * a link warning which actually raises the fact the the alignment between
 * the two object files of that particular symbol is different.
 * This tag makes the warning go away but in real life taking the warning seriously
 * could make this bug go away.
 */

#include <firstinclude.h>
#include <stdio.h> // for printf(3)
#include <stdlib.h> // for EXIT_SUCCESS
#include "file_with_weak_symbol.h"

int mysym=56;

int y=70;
int __attribute__((aligned (8))) x=58;
//int x=58;

int main(int argc,char** argv,char** envp) {
	print_mysym();
	printf("mysym is [%d]\n", mysym);
	mysym=57;
	print_mysym();
	printf("mysym is [%d]\n", mysym);
	// now lets show how problems happen
	// lets see that x and y are close together...
	printf("&x is [%p]\n", &x);
	printf("&y is [%p]\n", &y);
	// lets see what the other file thinks...
	print_details_of_x();
	printf("x is [%d]\n", x);
	printf("y is [%d]\n", y);
	touch_x();
	printf("x is [%d]\n", x);
	printf("y is [%d]\n", y);
	return EXIT_SUCCESS;
}
