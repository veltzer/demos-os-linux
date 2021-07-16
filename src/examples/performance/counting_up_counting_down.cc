/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
 * This example examines the question of whether it is more efficient to loop
 * from a number down to 0 rather than from 0 to the number. In theory this
 * could have some advantage because comparing to 0 is supposed to be more
 * efficient either in execution or in size of machine instruction when compared
 * to comparison with an arbitrary value.
 *
 * Compile this code and disassemble it using objdump in order to test this
 * question.
 *
 * The disassembly actually proves that counting down is more efficient than
 * counting up in this specific compiler (g++ 4.4.3) since it saves one machine
 * instruction inside the loop.
 *
 * This is for being able to see source code intermingled with assembly code...
 * EXTRA_COMPILE_FLAGS=-g3
 *
 * TODO:
 * - actually measure the time with a performance counter and see it.
 */

int main(int argc, char** argv, char** envp) {
	int sum=0;
	for(int i=0; i<100; i++) {
		sum+=i*i;
	}
	for(int j=99; j>0; j--) {
		sum+=j*j;
	}
	printf("sum is %d\n", sum);
	return EXIT_SUCCESS;
}
