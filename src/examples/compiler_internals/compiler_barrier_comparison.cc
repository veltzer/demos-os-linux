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
#include <stdio.h>	// for printf(3), fprintf(3)
#include <stdlib.h>	// for srandom(3), EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP()
#include <lowlevel_utils.h>	// for stack_vars_direction_up()

/*
 * This is an example of a compiler barrier
 * This example needs to be compiled with optimizations to see it in action.
 * View the resulting assembly code with:
 * objdump --disassemble --line-numbers --source --demangle barrier
 *
 * The gcc compiler barrier
 * ========================
 * is the asm(memory) one. It is not a CPU reordering barrier - just a compiler
 * one.
 * It seems that gcc does not have the notion of a *read* vs *write*
 * barrier which could come in handly instead of this dominating
 * *FULL* barrier.
 *
 * Function call barriers
 * ======================
 * On some platforms/compilers these are also memory barriers but not so in gcc
 * that assumes that functions that are called do not change the content of registers
 * (and if they do, they put back everything the way they found it). In any case
 * you cannot really rely on them since you are not sure which functions are macros,
 * inlines etc. Better to use an official compiler barrier. I picked a function that
 * is definately not an inlined one and still this it does not perform well as
 * a compiler barrier (srandom on gcc 4.5.2).
 *
 * Machine memory barriers
 * =======================
 * is the __sync_synchronize() one. There doesn't seem to be a read vs write
 * one. This does not serve as a compiler barrier as is evident as a result
 * of this program.
 *
 * Empty assembly block
 * ====================
 * Used to work in some versions of gcc but stopped working. Better not to use.
 *
 * Notes:
 * - the loop that you see in the code forces the compiler to use a register
 * to hold the 'a' variable (if you compile with optimisation ofcourse).
 * - if you disassemble the code you see that the compiler writes the loop variable
 * (a in our case) back to it's natural place on the stack right after the loop.
 * - after the loop suprisingly, the compiler is certain that the register is still
 * holding the right value for a and so uses it.
 * - without a good compiler barrier the compiler is certain that the value in the
 * register is synchronized with the memory location.
 *
 * References:
 * http://ridiculousfish.com/blog/archives/2007/02/17/barrier/
 * http://stackoverflow.com/questions/8896092/is-there-a-compiler-memory-barrier-for-a-single-variable
 *
 * TODO:
 * - make this program show the instructions that are emitted for main so
 * that people could see the assembly code generated.
 * - stop using the printing in this example and use the __attribute__((used))
 * gcc feature instead. then I can get ridd of the outfile.
 * this is actually wrong, the documentation of __attribute__((used)) explicitly
 * states that I cannot use it for non static variables so we would need
 * some other mechanism (__attribute__((unused)) ?!?) to force the compiler
 * for forgo warnings on these variables.
 *
 * EXTRA_COMPILE_FLAGS=-O3
 */

FILE* outfile;

#define TEST(shortname, desc, expected, code) \
	void test_ ## shortname() __attribute__((noinline)); \
	void test_ ## shortname(volatile int& val_before, volatile int& val_after) { \
		int a=0; \
		int u=0; \
		const int CORRECT_VAL=2000; \
		const int WRONG_VAL=3200; \
		/* p will point to a but the compiler does not know it.*/ \
		int* p=&u; \
		if(stack_vars_direction_up()) { \
			p--; \
		} else { \
			p++; \
		} \
		/* this taking of the address of a to have the compiler actually store
		 * a on the stack at all! If we don't do this the compiler will treat
		 * a as a register for the entire scope of this function! */ \
		int *pa=&a; \
		/* this printing is essential to keep the compiler from telling us
		 * that 'pa' is an unused variable. */ \
		fprintf(outfile, "pa is %p\n", (void*)pa); \
		fprintf(outfile, "p is %p\n", (void*)p); \
		fprintf(outfile, "&a is %p\n", (void*)&a); \
		fprintf(outfile, "now starting\n"); \
		/* this loop will force the compiler to load a into a register */ \
		a=100; \
		while(a < 3000) { \
			a+=a; \
		} \
		/* at this point the compiler emits an instruction to store the \
		 * register back into the memory address of a */ \
		*p=CORRECT_VAL; \
		val_before=a; \
		code; \
		val_after=a; \
		printf("results for [%s]\n", stringify(code)); \
		/* printf("description [%s]\n", desc); */ \
		printf("%s", desc); \
		/* printf("expectation [%d]\n", expected); */ \
		printf("val_before is %d, val_after is %d\n", val_before, val_after); \
		if(val_after==WRONG_VAL) { \
			printf("compiler used register for value\n"); \
		} else { \
			printf("compiler did not use register for value\n"); \
		} \
		if(val_after==CORRECT_VAL) { \
			printf("barrier worked\n"); \
			if(!expected) \
			printf("*** NOT AS EXPECTED ***\n"); \
		} else { \
			printf("barrier did not work\n"); \
			if(expected) \
			printf("*** NOT AS EXPECTED ***\n"); \
		} \
		printf("===========================================\n"); \
	}

TEST(
	nothing,
	"Doing nothing. This should give you the same values, which are the WRONG values.\n",
	false,
	);
TEST(
	machbar,
	"This is a full barrier which is both a MACHINE memory barrier and a compiler barrier.\n"
	"The macro actually inlines a machine instruction.\n",
	true,
	__sync_synchronize()
	);
TEST(
	emptasm,
	"Empty assembly block which works on some compilers but not on others.\n"
	"On gcc 4.5.2 it did work but newer compilers see that you are not really doing anything in the block.\n",
	false,
	asm volatile("")
	);
TEST(
	funccall,
	"Calling extern function works on some compilers and some functions.\n"
	"But watch out for functions annotated with certain annotations or inlined functions.\n"
	"Compiler ABI may also state that functions should restore register values which means that even\n"
	"non-inline non specially annotated functions will not serve as compiler barrier.\n"
	"This varies across compiler versions.",
	true,
	srandom(5)
	);
TEST(
	funccall2,
	"printf is always external and should work as a compiler barrier\n",
	true,
	printf("hello")
	);
TEST(
	fullcompbar,
	"Official full compiler barrier for gcc for all variables.\n"
	"Too strong if all you want is to force just one variable to be read from memory instead of register.\n"
	"The 'volatile' tells the compiler not to reorder around it.\n",
	true,
	asm volatile("" ::: "memory")
	);
TEST(
	singvarbar,
	"A single variable barrier -> this is what you should use instead of volatile\n",
	true,
	asm volatile("" : "=g" (a)::)
	);
TEST(
	singvarbar2,
	"A single variable read.\n"
	"This tells the compiler that someone read the variable, which means nothing since there is no reason to re-read it.\n",
	false,
	asm volatile("" : "=r" (a)::)
	);
TEST(
	singvarbar3,
	"A single variable barrier on the wrong variable1 (write)\n",
	false,
	asm volatile("" : "=g" (u)::)
	);
TEST(
	singvarbar4,
	"A single variable barrier on the wrong variable1 (read)\n"
	"This actually worked on some gcc versions because of a bug.\n",
	false,
	asm volatile("" : "=r" (u)::)
	);
TEST(
	singvarvol,
	"Attempt to use volatile on left side to barrier the compiler.\n"
	"The compiler sees through this\n",
	false,
	*(volatile int *)&a=a;
	);
TEST(
	singvarvolright,
	"Attempt to use volatile on right side to barrier the compiler\n"
	"This worked once but no more\n",
	false,
	a=*(volatile int*)&a;
	);
TEST(
	singvarvol2,
	"Attempt to use volatile to barrier the compiler\n"
	"The compiler sees through this\n",
	false,
	{ int y=a; *(volatile int*)&a=y; }
	);
TEST(
	singvarvolright2,
	"Attempt to use volatile on right side to barrier the compiler\n"
	"This one has bad logic\n",
	false,
	{ int y=a; a=*(volatile int*)&y; }
	);
TEST(
	atomicop,
	"Attempt to do an atomic operation on the var in question.\n"
	"This works because every atomic op is a memory barrier which is also a compiler barrier.\n",
	true,
	__sync_bool_compare_and_swap(&a, 0, 0)
	);
TEST(
	atomicop2,
	"Attempt to do an atomic operation on some other variable.\n"
	"This works because every atomic op is a memory barrier which is also a compiler barrier.\n",
	true,
	__sync_bool_compare_and_swap(&u, 0, 0)
	);
TEST(
	lkmla,
	"A solution suggested on lkml.\n"
	"this means that someone touched m. This works well.\n",
	true,
	asm volatile("" : "=m" (a) : "m" (a))
	);
TEST(
	lkmlu,
	"A solution suggested on lkml.\n"
	"doing the same on the wrong barrier is wrong.\n",
	false,
	asm volatile("" : "=m" (u) : "m" (u))
	);

int main() {
	outfile=CHECK_NOT_NULL_FILEP(fopen("/dev/null", "w"));
	int val_before, val_after;
	test_nothing(val_before, val_after);
	test_machbar(val_before, val_after);
	test_emptasm(val_before, val_after);
	test_funccall(val_before, val_after);
	test_funccall2(val_before, val_after);
	test_fullcompbar(val_before, val_after);
	test_singvarbar(val_before, val_after);
	test_singvarbar2(val_before, val_after);
	test_singvarbar3(val_before, val_after);
	test_singvarbar4(val_before, val_after);
	test_singvarvol(val_before, val_after);
	test_singvarvolright(val_before, val_after);
	test_singvarvol2(val_before, val_after);
	test_singvarvolright2(val_before, val_after);
	test_atomicop(val_before, val_after);
	test_atomicop2(val_before, val_after);
	test_lkmla(val_before, val_after);
	test_lkmlu(val_before, val_after);
	return EXIT_SUCCESS;
}
