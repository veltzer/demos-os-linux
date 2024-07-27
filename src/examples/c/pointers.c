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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is showing pointer pointer stuff
 * The idea is to show that the compiler does NOT allow to convert
 * Foobar** to char**.
 */

void func(char **p __attribute__((unused))) {
}

typedef struct _Foobar{
	int i;
	int j;
} Foobar;

int main() {
	// releasing the next two lines will trigger a compilation error!!!
	// Foobar* f=(Foobar*)malloc(sizeof(Foobar));
	// func(&f);
	return EXIT_SUCCESS;
}
