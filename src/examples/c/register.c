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
 * An example of doing a loop and telling the compiler that a register should be
 * involved.
 * We will also print the register.
 * How did we know what register the compiler used as the iterator of the loop? We
 * disassembled the code.
 * Note that removing the 'volatile' will cause the code to stop running correctly.
 * (need to look at the disassembly to find out exactly why...).
 * Here is a funny side note: if you remove the printf for i then the compiler
 * will not optimize the loop away because of the printing of the register. But
 * you will see that the value of the register does go down from 100 to 1 (not
 * from 99 to 0 mind you). The compiler thinks that its better to count down
 * than up because of the shorter "compare to zero" at the end of the loop.
 */

int main() {
	register unsigned int i;
	for(i=0; i<100; i++) {
		printf("i is %d\n", i);
		int reg;
		asm volatile("movl %%ebx, %0;" : "=r" (reg));
		printf("reg is %d\n", reg);
	}
	return EXIT_SUCCESS;
}
