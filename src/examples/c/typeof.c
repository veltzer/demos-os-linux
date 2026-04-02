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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for stringify()

/*
 * Demo of the __typeof__ operator in C.
 *
 * References:
 * http://stackoverflow.com/questions/4436889/what-is-__typeof__c-1-in-c
 * http://gcc.gnu.org/onlinedocs/gcc/Typeof.html
 */

static int __attribute__((unused)) myfunction(double x) {
	return (int)x+1;
}

int main(void) {
	/* The next attempt to stringify __typeof__ does not work */
	/*
	 * int __attribute__((unused)) c=6;
	 * printf("__typeof__(c) returned %s\n",stringify(__typeof__(c)));
	 */

	/*
	 * Example of using __typeof__() to avoid writing the type yourself
	 */
	int c=6;
	/* d is the same type of c */
	__typeof__(c)d=c;
	printf("d is %d\n", d);

	/* e's type is a pointer to whatever c type is... */
	__typeof__(__typeof__(c) *)e=(__typeof__(__typeof__(c) *)) 100;
	printf("e is %p\n", (void*)e);

	/* f's type is the return type of myfunction... */
	__typeof__(myfunction(1))f=3;
	printf("f is %d\n", f);

	/* g's type is a promoted to at least int...*/
	char a=4;
	__typeof__(a+1)g=(__typeof__(g))a;
	printf("g is %d\n", g);

	/* same as above but with float (remains float)... */
	float b=4;
	__typeof__(b+1)h=(__typeof__(h))b;
	printf("h is %f\n", h);
	return EXIT_SUCCESS;
}
