/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to use C99 designated initializers.
 *
 * References:
 * https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html
 * http://www.drdobbs.com/the-new-c-x-macros/184401387
 */

enum COLOR {
	red,
	green,
	blue,
};

/*
notice we initialize the cells according to the enum
values thus not assuming anything about enum order.
*/
char *color_name[] = {
	[red]="red",
	[green]="green",
	[blue]="blue",
};

int main(int argc, char** argv, char** envp) {
	enum COLOR c = red;
	printf("c=%s\n", color_name[c]);
	return EXIT_SUCCESS;
}
