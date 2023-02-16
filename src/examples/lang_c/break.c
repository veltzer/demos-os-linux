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

/*
 * Compare the version that uses the 'break' keyword with the version that
 * does not. Which one is shorter? more maintanable? easier to read? easier
 * to change? Sometimes break is just the right answer.
 */

int main(int argc, char** argv, char** envp) {
	/* without break */
	int early_break=0;
	int i;
	for(i=0; i<100 && !early_break; i++) {
		if(i==5)
			early_break=1;
		else
			printf("i is %d\n", i);
	}
	/* with break */
	for(i=0; i<100; i++) {
		if(i==5) break;
		printf("i is %d\n", i);
	}
	return 0;
}
