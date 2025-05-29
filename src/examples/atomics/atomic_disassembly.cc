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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <disassembly_utils.h>	// for disassemble_function()

/*
 * This example is written so that you could disassemble the machine
 * code for volatiles and atomics. That is also why debug info is enabled
 * for it (so that you could see the source code interleaved with the
 * assembly).
 *
 * EXTRA_COMPILE_FLAGS=-g3
 */

int main() {
	disassemble_function("do_volatile");
	disassemble_function("do_regular");
	disassemble_function("do_atomic");
	return EXIT_SUCCESS;
}

/*
 * When looking at the disassembly we see that the compiler moves the variable
 * from the stack into a register, does the add in the register and then returns
 * it to the stack
 */
int do_volatile(volatile int i) {
	i++;
	return i;
}

/*
 */
int do_regular(int i) {
	i++;
	return i;
}

/*
 * this one is the atomic case
 */
int do_atomic(int i) {
	__sync_add_and_fetch(&i, 1);
	return i;
}
