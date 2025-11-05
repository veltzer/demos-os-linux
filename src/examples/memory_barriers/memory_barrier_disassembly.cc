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
#include <atomic>	// for atomic_thread_fence, memory_order
#include <disassembly_utils.h>	// for disassemble_main()

using namespace std;

/*
 * This is a demo to show what machine instructions are really when
 * you emit a memory barrier.
 *
 * Take note that a memory barrier is a also a compiler barrier.
 * It prevents the compiler from reordering around this code and carrying
 * assumptions across the barrier.
 * You don't see any assembly emitted for that since a compiler barrier is only
 * a compile time instruction about future code emittion.
 *
 * The file now includes five types of memory barriers:
 * 1. GCC intrinsics (__sync_synchronize, __atomic_thread_fence)
 * 2. Inline assembly (lock instructions, mfence, lfence, sfence)
 * 3. Additional hardware barriers (CPUID, RDTSCP, CLFLUSH, PAUSE, I/O)
 * 4. C-style barriers (volatile, compiler barriers)
 * 5. C++ standard library (atomic_thread_fence with different orderings)
 *
 * this is so disassembly will show interleaved code
 * EXTRA_COMPILE_FLAGS=-g3 -std=c++20
 */

void gcc_intrinsics() __attribute__((unused, noinline));
void gcc_intrinsics() {
	__sync_synchronize();
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	__atomic_thread_fence(__ATOMIC_ACQUIRE);
	__atomic_thread_fence(__ATOMIC_RELEASE);
	__atomic_thread_fence(__ATOMIC_ACQ_REL);
	__atomic_thread_fence(__ATOMIC_CONSUME);
	__atomic_thread_fence(__ATOMIC_RELAXED);
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

void additional_hw_barriers() __attribute__((unused, noinline));
void additional_hw_barriers() {
	// Serializing instructions (full memory barriers)
	asm ("cpuid" : : : "eax", "ebx", "ecx", "edx");
	asm ("rdtscp" : : : "eax", "edx", "ecx");

	// XCHG with implicit lock (full memory barrier)
	asm ("xchg %eax, %ebx");

	// Cache control with memory barrier semantics
	// Using stack pointer for valid address
	asm ("clflush (%%rsp)" : : : "memory"); // Flush cache line + memory barrier

	// Memory ordering hint for spin loops
	asm ("pause");

	// I/O instructions (serialize execution)
	asm ("outb %%al, $0x80" : : "a"(0)); // I/O write with serialization
	unsigned char dummy;
	asm ("inb $0x80, %%al" : "=a"(dummy) : : ); // I/O read with serialization
	(void)dummy; // Suppress unused warning

	// Memory barrier through memory operand constraints
	int barrier_var = 0;
	asm ("" : "+m"(barrier_var) : : "memory"); // Memory barrier via operand constraint
}

void c_style_barriers() __attribute__((unused, noinline));
void c_style_barriers() {
	// C-style compiler barriers using volatile
	volatile int dummy = 0;
	(void)dummy; // prevent unused variable warning

	// GCC-specific compiler barrier (prevents reordering but no hw fence)
	asm volatile ("" ::: "memory");

	// Traditional C-style barriers using volatile reads/writes
	volatile int barrier_var = 0;
	barrier_var = 1; // volatile write
	dummy = barrier_var; // volatile read
}

void cpp_std_fences() __attribute__((unused, noinline));
void cpp_std_fences() {
	// C++11 standard memory fences with different orderings
	atomic_thread_fence(memory_order_seq_cst);
	atomic_thread_fence(memory_order_acquire);
	atomic_thread_fence(memory_order_release);
	atomic_thread_fence(memory_order_acq_rel);
	atomic_thread_fence(memory_order_consume);
	atomic_thread_fence(memory_order_relaxed);
}

int main() {
	disassemble_function("gcc_intrinsics");
	disassemble_function("asm_locks");
	disassemble_function("additional_hw_barriers");
	disassemble_function("c_style_barriers");
	disassemble_function("cpp_std_fences");
	return EXIT_SUCCESS;
}
