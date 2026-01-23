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
#include <err_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/sched.h>
#include <asm/prctl.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

/*
 * Thread creation and joining without pthreads library.
 * Uses clone3() with PIDFD for joining (requires Linux 5.10+).
 *
 * Key challenges solved:
 * 1. After clone, child has different RSP - can't use stack-relative addressing
 * 2. Child has no TLS - must set up %fs manually before using glibc
 * 3. glibc's syscall() wrapper may access TLS - use raw syscall instructions
 * 4. Stack protector reads %fs:0x28 - must disable or set up TLS first
 *
 * EXTRA_COMPILE_FLAGS=-std=c++20
 */

#define STACK_SIZE (64 * 1024)

// Minimal TCB structure that glibc expects at %fs:0
struct minimal_tcb {
	void *self; // %fs:0x00 - must point to itself
	void *dtv; // %fs:0x08 - dynamic thread vector
	void *thread_self; // %fs:0x10
	int multiple_threads; // %fs:0x18
	int gscope_flag; // %fs:0x1c
	uintptr_t sysinfo; // %fs:0x20
	uintptr_t stack_guard; // %fs:0x28 - for -fstack-protector
	uintptr_t pointer_guard; // %fs:0x30
	char padding[4096 - 64];
};

// Globals - child can't access parent's stack variables
static const char *g_msg;
static uintptr_t g_stack_guard;

// Raw syscalls - completely bypass glibc to avoid TLS access
__attribute__((no_stack_protector, always_inline))
static inline long raw_syscall1(long n, long a1)
{
	long ret;
	asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1) : "rcx", "r11", "memory");
	return ret;
}

__attribute__((no_stack_protector, always_inline))
static inline long raw_syscall2(long n, long a1, long a2)
{
	long ret;
	asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2) : "rcx", "r11", "memory");
	return ret;
}

__attribute__((no_stack_protector, always_inline))
static inline long raw_syscall3(long n, long a1, long a2, long a3)
{
	long ret;
	asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2), "d"(a3) : "rcx", "r11", "memory");
	return ret;
}

__attribute__((no_stack_protector, always_inline))
static inline long raw_syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	long ret;
	register long r10 asm("r10") = a4;
	register long r8 asm("r8") = a5;
	register long r9 asm("r9") = a6;
	asm volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8), "r"(r9) : "rcx", "r11", "memory");
	return ret;
}

__attribute__((no_stack_protector, noinline))
static void setup_tls(uintptr_t stack_guard)
{
	long ret = raw_syscall6(SYS_mmap, 0, sizeof(struct minimal_tcb),
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ret < 0) {
		raw_syscall3(SYS_write, 2, (long)"mmap failed\n", 12);
		raw_syscall1(SYS_exit, 1);
	}
	struct minimal_tcb *tcb = (struct minimal_tcb *)ret;

	// Zero manually - memset might use TLS
	volatile char *p = (volatile char *)tcb;
	for(size_t i = 0; i < sizeof(*tcb); i++)
		p[i] = 0;

	tcb->self = tcb;
	tcb->thread_self = tcb;
	tcb->multiple_threads = 1;
	tcb->stack_guard = stack_guard;

	if (raw_syscall2(SYS_arch_prctl, ARCH_SET_FS, (long)tcb) < 0) {
		raw_syscall3(SYS_write, 2, (long)"arch_prctl failed\n", 18);
		raw_syscall1(SYS_exit, 1);
	}
}

// Child entry - use raw syscalls only
extern "C" __attribute__((no_stack_protector, noinline, noreturn, used))
void child_entry(void)
{
	raw_syscall3(SYS_write, 1, (long)"[Thread] Setting up TLS...\n", 27);
	setup_tls(g_stack_guard);

	char buf[64];
	int len = 0;
	const char *prefix = "[Thread] Hello! Arg: ";
	for(const char *s = prefix; *s; s++) buf[len++] = *s;
	for(const char *s = g_msg; *s; s++) buf[len++] = *s;
	buf[len++] = '\n';
	raw_syscall3(SYS_write, 1, (long)buf, len);

	struct timespec ts = {2, 0};
	raw_syscall2(SYS_nanosleep, (long)&ts, 0);

	raw_syscall3(SYS_write, 1, (long)"[Thread] Goodbye!\n", 18);
	raw_syscall1(SYS_exit, 42);
	__builtin_unreachable();
}

__attribute__((no_stack_protector))
int main(void)
{
	int pidfd = -1;
	void *stack;

	stack = (int*)CHECK_NOT_VOIDP(mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0), MAP_FAILED);
	g_msg = "no pthreads!";
	asm volatile("mov %%fs:0x28, %0" : "=r"(g_stack_guard));

	pid_t tid;
	register void (*entry)(void) asm("r12") = child_entry;

	struct clone_args ca = {};
	ca.flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_SYSVSEM | CLONE_PIDFD;
	ca.pidfd = (uint64_t)(uintptr_t)&pidfd;
	ca.stack = (uint64_t)(uintptr_t)stack;
	ca.stack_size = STACK_SIZE;

	asm volatile(
		"syscall\n"
		"test %%rax, %%rax\n"
		"jnz 1f\n"
		// Use call to push return addr and fix stack alignment
		"call *%%r12\n"
		"1:\n"
		: "=a"(tid), "+r"(entry)
		: "a"(SYS_clone3), "D"(&ca), "S"(sizeof(ca))
		: "rcx", "r11", "memory"
	);
	CHECK_NOT_M1(tid);
	printf("[Main tid=%d] Spawned thread %d\n", gettid(), tid);

	assert(pidfd >= 0);
	// Wait via pidfd (proper join)
	struct pollfd pfd = {.fd = pidfd, .events = POLLIN, .revents = 0};
	CHECK_NOT_M1(poll(&pfd, 1, -1));
	close(pidfd);
	printf("[Main] Thread exited (via pidfd).\n");
	CHECK_NOT_M1(munmap(stack, STACK_SIZE));
	return EXIT_SUCCESS;
}
