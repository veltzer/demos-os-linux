#include <firstinclude.h>
#include <err_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/sched.h>
#include <poll.h>

#define STACK_SIZE (64 * 1024)

/*
 * What pthreads does that we're skipping:
 *
 * TLS (Thread-Local Storage) — pthreads sets up %fs/%gs segment registers pointing to a TLS block
 * Stack guard pages — protection against overflow
 * Thread ID caching — pthread_self() reads from TLS instead of syscall
 * Cleanup handlers — pthread_cleanup_push/pop
 * Return value retrieval — we can't get the 42 back easily without extra plumbing
 * Futex-based join — pthreads uses a futex for pthread_join, which is more efficient than a pidfd for single waits
 *
 * If you want the exit code, you'd either need to write it to shared memory before exiting,
 * or use waitid() with P_PIDFD (but that only works for processes, not CLONE_THREAD threads).
 * The pidfd approach is really best for multiplexed waiting in an event loop rather than
 * simple join semantics.
 *
 * EXTRA_COMPILE_FLAGS=-std=c++20
 */

static int thread_func(void *arg)
{
	const char *msg = (const char*)arg;
	printf("[Thread tid=%d] Hello! Arg: %s\n", gettid(), msg);
	sleep(2);
	printf("[Thread tid=%d] Goodbye!\n", gettid());
	return 42; // exit code
}

int main(void)
{
    int pidfd;
    void *stack;

    // Allocate stack (grows down on most architectures)
    stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

	struct clone_args ca = {};  // Zero-init everything first
	ca.flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
		   CLONE_THREAD | CLONE_SYSVSEM | CLONE_PIDFD;
	ca.pidfd = (uint64_t)(uintptr_t)&pidfd;
	ca.stack = (uint64_t)(uintptr_t)stack;
	ca.stack_size = STACK_SIZE;

    pid_t tid = CHECK_NOT_M1(syscall(SYS_clone3, &ca, sizeof(ca)));

    if (tid == 0) {
        // Child thread
        int ret = thread_func((void*)"no pthreads!");
        syscall(SYS_exit, ret);
        __builtin_unreachable();
    }

    // Parent
    printf("[Main tid=%d] Spawned thread %d, waiting...\n", gettid(), tid);

    // "Join" - wait for pidfd to become readable
    struct pollfd pfd = {};
    pfd.fd = pidfd;
    pfd.events = POLLIN;
    CHECK_NOT_M1(poll(&pfd, 1, -1));
    printf("[Main] Thread exited!\n");
    // Cleanup
    CHECK_NOT_M1(close(pidfd));
    CHECK_NOT_M1(munmap(stack, STACK_SIZE));
    return EXIT_SUCCESS;
}
