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
#include "one.h"
#include "two.h"

int main(int argc,char** argv,char** envp) {
	printf("did you know that 2+2=%d\n", add(2,2));
	return EXIT_SUCCESS;
}
