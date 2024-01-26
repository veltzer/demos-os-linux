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
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3), atoi(3)
#include <stdio.h>	// for printf(3), fprintf(3), stderr
#include <ucontext.h>	// for ucontext_t:type, REG_EIP
#include <setjmp.h>	// for sigjmp_buf:type, sigsetjmp(3), siglongjmp(3)
#include <signal_utils.h>	// for signal_register_handler_sigaction()
#include <trace_utils.h>// for TRACE()
#include <disassembly_utils.h>	// for disassemble_main()

/*
 * This demonstrates how you can recover from a segmentation fault.
 * Several ways are demonstrated:
 * - jumping to a different (safe?) function.
 * - relative jump.
 * - absolute jump via a label.
 * - siglongjmp(3)
 *
 * References:
 * http://stackoverflow.com/questions/3291382/coming-back-to-life-after-segmentation-violation
 *
 * this is to make sure that source code interleaving in the disassembly works
 * out well...
 * EXTRA_COMPILE_FLAGS=-g3
 *
 * TODO:
 * - remove stuff from this example which was incorporated into signal_utils.h
 */

static void safe_func(void) {
	printf("Safe now ?\n");
	exit(EXIT_SUCCESS);
}

static void handler_safe(int sig, siginfo_t *si, void *uap) {
	TRACE("start");
	ucontext_t *context = (ucontext_t*)uap;
	/*
	 * alternativly, try to jump to a "safe place"
	 */
#if __i386__
	context->uc_mcontext.gregs[REG_EIP] = (unsigned int)safe_func;
#endif	/* __i386__ */
#if __x86_64__
	context->uc_mcontext.gregs[REG_RIP] = (unsigned long)safe_func;
#endif	/* __x86_64__ */
	TRACE("end");
}

static int jmp_rel;
static void handler_jmp_rel(int sig, siginfo_t *si, void *uap) {
	TRACE("start");
#if __i386__
	ucontext_t *context = (ucontext_t*)uap;
	TRACE("REG_EIP is %x", context->uc_mcontext.gregs[REG_EIP]);
	/*
	 * On my particular system, compiled with gcc -O2, the offending instruction
	 * generated for "*f = 16;" is 6 bytes. Lets try to set the instruction
	 * pointer to the next instruction (general register 14 is EIP, on linux x86)
	 */
	context->uc_mcontext.gregs[REG_EIP] += jmp_rel;
#endif	/* __i386__ */
	TRACE("end");
}

static int jmp_abs;
static void handler_jmp_abs(int sig, siginfo_t *si, void *uap) {
	TRACE("start");
#if __i386__
	ucontext_t *context = (ucontext_t*)uap;
	/*
	 * We will jump to some address in this handler.
	 * You better set the address right before hand...
	 */
	context->uc_mcontext.gregs[REG_EIP] = jmp_abs;
#endif	/* __i386__ */
	TRACE("end");
}

static sigjmp_buf env;
static void handler_sigjmp(int sig, siginfo_t *si, void *uap) {
	TRACE("start");
	TRACE("end");
	siglongjmp(env, 1);
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [type]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: type can be 0-3\n", argv[0]);
		return EXIT_FAILURE;
	}
	int choice=atoi(argv[1]);
	if(choice<0 || choice>3) {
		fprintf(stderr, "%s: what kind of choice is %d ?!?\n", argv[0], choice);
		return EXIT_FAILURE;
	}
	if(choice==0) {
		signal_register_handler_sigaction(SIGSEGV, handler_safe, 0);
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=100;
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=101;
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=102;
	}
	if(choice==1) {
		// 10 is the size of the instruction
		jmp_rel=10;
		signal_register_handler_sigaction(SIGSEGV, handler_jmp_rel, 0);
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=200;
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=201;
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=202;
	}
	if(choice==2) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
		jmp_abs=(unsigned long)&&mylabel;
#pragma GCC diagnostic pop
		signal_register_handler_sigaction(SIGSEGV, handler_jmp_abs, 0);
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=300;
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=301;
		// cppcheck-suppress nullPointer
		*(volatile int*)NULL=302;
	}
	if(choice==3) {
		signal_register_handler_sigaction(SIGSEGV, handler_sigjmp, 0);
		int ret=sigsetjmp(env, 0);
		if(ret==0) {
			// cppcheck-suppress nullPointer
			*(volatile int*)NULL=400;
			// cppcheck-suppress nullPointer
			*(volatile int*)NULL=401;
			// cppcheck-suppress nullPointer
			*(volatile int*)NULL=402;
		} else {
			printf("after...\n");
		}
	}
mylabel:
	printf("after fault...\n");
	return EXIT_SUCCESS;
}
