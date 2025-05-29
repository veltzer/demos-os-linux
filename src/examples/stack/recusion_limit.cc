/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
 * This program finds the recusion limit.
 *
 * NOTES:
 * - if this is a tail recursion then gcc may eliminate the recursion.
 * that is why we have a double call below.
 * - you can set the stack limit via 'ulimit -s'.
 * - 'ulimit -s unlimited' seems to offer a stack which can grow up to the
 * whole systems RAM.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-infinite-recursion
 */

static int i;

void recurse_onek() {
	// char __attribute__((unused)) data[1024];
	printf("i is %d\n", i++);
	recurse_onek();
	recurse_onek();
}

int main() {
	recurse_onek();
	return EXIT_SUCCESS;
}
