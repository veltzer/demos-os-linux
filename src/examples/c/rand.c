/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3), scanf(3)
#include <stdlib.h>	// for rand(3), srand(3), EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_INT()

int main() {
	int seed;
	int i;
	printf("give me a seed: ");
	CHECK_INT(scanf("%d", &seed), 1);
	// no error code for srand
	srand(seed);
	for(i=0; i<10; i++) {
		// no error code for rand
		printf("rand gave me %d\n", rand());
	}
	return EXIT_SUCCESS;
}
