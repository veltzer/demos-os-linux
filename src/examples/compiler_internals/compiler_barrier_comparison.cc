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
#include <stdio.h>	// for printf(3), fprintf(3)
#include <stdlib.h>	// for srandom(3), EXIT_SUCCESS
#include <unistd.h>	// for isatty(3), STDOUT_FILENO
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP()
#include <us_helper.h>	// for stringify()

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
 * How the "someone else changed the variable" is simulated
 * ========================================================
 * Every test first writes CORRECT_VAL into 'a' with an inline asm store that
 * declares no "memory" clobber and does not list 'a' as an output. The
 * compiler trusts the declared constraints of an asm, so it believes memory is
 * untouched and keeps using the copy of 'a' it has in a register. This is
 * exactly what another thread or a device writing to 'a' looks like from the
 * compiler's point of view.
 * The asm is deliberately *not* volatile: clang (LLVM) assumes that any
 * asm volatile may read and write any memory whose address has escaped (and
 * &a escapes via fprintf below), so a volatile asm would not hide the store
 * from clang. To keep the non-volatile asm from being deleted it produces a
 * dummy output which is written to a volatile sink.
 * Older versions of this demo wrote through a pointer that was walked from
 * a neighbouring stack variable into 'a' (p=&u; p++). That is undefined
 * behaviour and newer gcc (15) no longer falls for it, so the demo stopped
 * showing anything.
 *
 * gcc vs clang
 * ============
 * The two compilers disagree about what an asm volatile without a "memory"
 * clobber may do:
 * - gcc believes the declared constraints: an empty asm volatile("") or one
 *   that only names another variable ('u') is not a barrier for 'a'.
 * - clang treats every asm volatile as possibly reading and writing any
 *   memory whose address has escaped, so all of those are barriers for 'a'.
 * Tests whose outcome differs between the compilers give their expectation
 * with BY_COMPILER(gcc_expectation, clang_expectation).
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
 * EXTRA_COMPILE_FLAGS_BEFORE=-O3
 */

FILE* outfile;

/*
 * Receives the dummy output of HIDDEN_STORE so that the (non volatile) asm
 * is not removed as dead code.
 */
volatile int hidden_store_sink;

/*
 * Store 'val' into '*p' behind the compiler's back: no "memory" clobber and
 * '*p' is not an output, so the optimizer does not know memory changed.
 * Not volatile on purpose, see "How the ... is simulated" above.
 * '*p' is passed as an "m" *input*: that tells the compiler the asm reads
 * '*p', so the pending store of 3200 to 'a' must happen before the asm (without
 * it the asm has no dependencies and gcc may hoist it above that store, making
 * every real barrier look broken). Since '*p' is not an output, the compiler
 * still believes the asm left it unchanged.
 * The dummy output is 0, not 'val', and on x86 'val' is passed as an
 * immediate when possible: no register should be left holding CORRECT_VAL,
 * otherwise a test like asm volatile("" : "=r" (a)) (which leaves in 'a'
 * whatever the chosen register happened to hold) "works" by pure accident.
 */
#if defined(__x86_64__) || defined(__i386__)
#define HIDDEN_STORE(p, val) do { \
		int hidden_store_out; \
		asm("movl %2, %1\n\txorl %0, %0" : "=r" (hidden_store_out) : "m" (*(p)), "ir" (val)); \
		hidden_store_sink=hidden_store_out; \
	} while(0)
#elif defined(__aarch64__)
#define HIDDEN_STORE(p, val) do { \
		int hidden_store_out; \
		asm("str %w2, %1\n\tmov %w0, wzr" : "=r" (hidden_store_out) : "m" (*(p)), "r" (val)); \
		hidden_store_sink=hidden_store_out; \
	} while(0)
#else
#error "HIDDEN_STORE is not implemented for this architecture"
#endif

/*
 * Color the verdict line green (as expected) or red (not as expected),
 * but only when stdout is a terminal so redirected output stays clean.
 */
static bool use_color() {
	static const bool tty=isatty(STDOUT_FILENO);
	return tty;
}

static const char* color_start(bool as_expected) {
	if(!use_color()) {
		return "";
	}
	return as_expected ? "\033[32m" : "\033[31m";
}

static const char* color_end() {
	return use_color() ? "\033[0m" : "";
}

/* expectation for tests where gcc and clang behave differently */
#ifdef __clang__
#define BY_COMPILER(gcc_expected, clang_expected) (clang_expected)
#else
#define BY_COMPILER(gcc_expected, clang_expected) (gcc_expected)
#endif

#define TEST(shortname, desc, expected, code) \
	void test_ ## shortname() __attribute__((noinline)); \
	void test_ ## shortname(volatile int& val_before, volatile int& val_after) { \
		int a=0; \
		/* only used by the "wrong variable" tests */ \
		[[maybe_unused]] int u=0; \
		const int CORRECT_VAL=2000; \
		const int WRONG_VAL=3200; \
		/* p points to a; the store through it below is done in asm \
		 * so the compiler does not see it. */ \
		int* p=&a; \
		/* this taking of the address of a to have the compiler actually store
		 * a on the stack at all! If we don't do this the compiler will treat
		 * a as a register for the entire scope of this function! */ \
		const int *pa=&a; \
		/* this printing is essential to keep the compiler from telling us
		 * that 'pa' is an unused variable. */ \
		fprintf(outfile, "pa is %p\n", static_cast<const void*>(pa)); \
		fprintf(outfile, "p is %p\n", static_cast<void*>(p)); \
		fprintf(outfile, "&a is %p\n", static_cast<void*>(&a)); \
		fprintf(outfile, "now starting\n"); \
		/* this loop will force the compiler to load a into a register */ \
		a=100; \
		while(a < 3000) { \
			a+=a; \
		} \
		/* at this point the compiler emits an instruction to store the \
		 * register back into the memory address of a. Then we change a \
		 * in memory without the compiler knowing about it. */ \
		HIDDEN_STORE(p, CORRECT_VAL); \
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
		const bool worked=(val_after==CORRECT_VAL); \
		const bool as_expected=(worked==static_cast<bool>(expected)); \
		printf("%scompiler barrier %s %s%s\n", \
			color_start(as_expected), \
			worked ? "worked" : "did not work", \
			as_expected ? "as expected" : "*** NOT AS EXPECTED ***", \
			color_end()); \
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
// cppcheck-suppress syntaxError
TEST(
	emptasm,
	"Empty assembly block which works on some compilers but not on others.\n"
	"On gcc 4.5.2 it did work but newer gcc see that you are not really doing anything in the block.\n"
	"clang treats any asm volatile as touching escaped memory so there it is a barrier.\n",
	BY_COMPILER(false, true),
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
	"A single variable output in a register.\n"
	"This tells the compiler that the asm wrote some value into a register which is now 'a'.\n"
	"The asm does not actually write anything so the value of 'a' is whatever was in that register.\n"
	"Whether you get the right value is pure luck - do not rely on this.\n",
	false,
	asm volatile("" : "=r" (a)::)
	);
TEST(
	singvarbar3,
	"A single variable barrier on the wrong variable1 (write)\n"
	"Not a barrier for 'a' on gcc. On clang any asm volatile is a barrier for escaped memory.\n",
	BY_COMPILER(false, true),
	asm volatile("" : "=g" (u)::)
	);
TEST(
	singvarbar4,
	"A single variable barrier on the wrong variable1 (read)\n"
	"This actually worked on some gcc versions because of a bug.\n"
	"On clang any asm volatile is a barrier for escaped memory.\n",
	BY_COMPILER(false, true),
	asm volatile("" : "=r" (u)::)
	);
// The C-style volatile casts below are the very technique these tests
// demonstrate, so they are deliberate and must not be rewritten.
// cppcheck-suppress cstyleCast
TEST(
	singvarvol,
	"Attempt to use volatile on left side to barrier the compiler.\n"
	"The compiler sees through this\n",
	false,
	*(volatile int *)&a=a;
	);
// cppcheck-suppress cstyleCast
TEST(
	singvarvolright,
	"Use a volatile read to force re-reading the variable from memory.\n"
	"A volatile access cannot be optimized away so the compiler must load 'a' from memory.\n"
	"This is a legitimate single variable re-read (it is what READ_ONCE() does in the kernel).\n",
	true,
	a=*(volatile int*)&a;
	);
// cppcheck-suppress cstyleCast
TEST(
	singvarvol2,
	"Attempt to use volatile to barrier the compiler\n"
	"The compiler sees through this\n",
	false,
	{ int y=a; *(volatile int*)&a=y; }
	);
// cppcheck-suppress cstyleCast
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
	"doing the same on the wrong barrier is wrong.\n"
	"On clang any asm volatile is a barrier for escaped memory so there it works.\n",
	BY_COMPILER(false, true),
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
