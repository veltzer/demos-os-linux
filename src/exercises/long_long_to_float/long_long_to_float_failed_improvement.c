/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdbool.h>	// for true
#include <stdlib.h>	// for EXIT_SUCCESS

int main(int argc, char** argv, char** envp) {
	long long prev=0;
	long long ll=0;
	// the volatile is needed because otherwise gcc will assume that
	// it knows that back==ll and will not do the actual convesion...
	volatile float f=0;
	long long back=0;
	unsigned long long iterations=0;
	unsigned long long jmp=1;
	while(true) {
		f=(float)ll;
		printf("ll is %lld\n", ll);
		back=(long long)f;
		if(ll!=back) {
			if(jmp==2) {
				break;
			} else {
				jmp=1;
				ll=prev;
			}
		}
		prev=ll;
		ll+=jmp;
		jmp*=2;
		iterations++;
	}
	printf("ll is %lld\n", ll);
	printf("f is %f\n", f);
	printf("back is %lld\n", back);
	printf("number of iterations is %llu\n", iterations);
	return EXIT_SUCCESS;
}
