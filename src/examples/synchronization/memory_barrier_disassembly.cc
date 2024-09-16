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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <disassembly_utils.h>	// for disassemble_main()

/*
 * This is a demo to show what machine instructions are really when
 * you emit a memory barrier.
 * We give several example of memory barriers:
 * sync_synchronize() - "lock orl"
 *
 * Take note that this is a also a compiler barrier. It prevents the compiler
 * from reordering around this code and carrying assumptions across the barrier.
 * You don't see any assembly emitted for that since a compiler barrier is only
 * a compile time instruction about future code emittion.
 *
 * There are other examples of other ways to achieve memory fences on x86
 * but the differences between all of these various ways are not explained.
 *
 * this is so disassembly will show interleaved code
 * EXTRA_COMPILE_FLAGS=-g3
 */

void gcc_intrinsics() __attribute__((unused, noinline));
void gcc_intrinsics() {
	__sync_synchronize();
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	__atomic_thread_fence(__ATOMIC_ACQUIRE);
	__atomic_thread_fence(__ATOMIC_RELEASE);
	__atomic_thread_fence(__ATOMIC_ACQ_REL);
	__atomic_thread_fence(__ATOMIC_CONSUME);
}

void asm_locks() __attribute__((unused, noinline));
void asm_locks() {
	asm ("lock orq $0x0, (%esp)");
	asm ("lock orl $0x0, (%esp)");
	asm ("lock addl $0x0, (%esp)");
	asm ("lock xchg (%esp), %esp");
	asm ("mfence");
	asm ("lfence");
	asm ("sfence");
}

int main() {
	disassemble_function("gcc_intrinsics");
	disassemble_function("asm_locks");
	return EXIT_SUCCESS;
}
