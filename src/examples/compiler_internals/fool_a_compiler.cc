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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that you can fool a compiler which is using a register for a loop
 * variable and store something else in the memory location for that variable and have
 * the compiler be very ambiguous about what the value of the variable is.
 *
 * NOTES:
 * - The reason why the second value of i is different from the first is that the compiler,
 * forgets about registers whenever calling a remote function (in our case, printf(3)).
 * this means that the compiler is using the register for the first call but the memory
 * location for the second which accounts for the different values.
 * - the reason you need the two print statements at the begining is that otherwise pi or
 * i will get optimized away completely either which will make the trick not work.
 * - notice that on modern Linux systems whenever you run this program you will get different
 * addresses for the variables unlike in older systems where the addresses are constant. This
 * is ASLR (address space randomization) which is a security measure of the kernel that puts
 * the different segments of memory in slightly different locations on each run to protect
 * against hackers. You can turn that off (look at my interesting command lines).
 */

int main(int argc, char** argv, char** envp) {
	int i;
	//printf("&i-&argc=%ld\n", &i-&argc); // this is 2 on x64 and 5 and i386
	// pi points to i but the compiler does not know it...
#pragma GCC diagnostic ignored "-Warray-bounds"
	int* pi=&argc+2;
	//int* pi=&i;
	// the next TWO print statements are neccessary to make the magic at the end happen...
	printf("&i is %p\n", (void*)&i);
	printf("&pi is %p\n", (void*)pi);
	printf("pi is %p\n", (void*)pi);
	unsigned long sum=0;
	for(i=0; i<100; i++) {
		sum+=i*i*i;
	}
	*pi=700;
	// watch this magic trick! i has two different values...
	printf("i is %d\n", i);
	printf("i is %d\n", i);
	printf("i is %d\n", i);
	return EXIT_SUCCESS;
}
