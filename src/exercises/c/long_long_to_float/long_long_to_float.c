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
#include <stdbool.h>	// for true
#include <stdlib.h>	// for EXIT_SUCCESS

int main(int argc, char** argv, char** envp) {
	long long ll=0;
	// the volatile is needed because otherwise gcc will assume that
	// it knows that back==ll and will not do the actual convesion...
	volatile float f=0;
	long long back=0;
	unsigned long long iterations=0;
	while(true) {
		f=(float)ll;
		back=(long long)f;
		if(ll!=back) {
			break;
		}
		ll++;
		iterations++;
	}
	printf("ll is %lld\n", ll);
	printf("f is %f\n", f);
	printf("back is %lld\n", back);
	printf("number of iterations is %llu\n", iterations);
	return EXIT_SUCCESS;
}
