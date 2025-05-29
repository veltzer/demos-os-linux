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
#include <ucontext.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#ifdef USE_THREADS
#include <pthread.h>
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#include <sys/select.h>
#endif /* USE_THREADS */

#include "crash.h"
#include "crash_msg.h"

/* Global static crash message buffer */
static struct crash_message_struct g_crash_msg;

/* Pipe file descriptor to crashd */
static int g_logfd=-1;

/* Pointer to global assert info, supplied during registration */
static unsigned char * g_assert_buf_ptr=NULL;

#ifdef USE_THREADS
/* Spinlock protecting access to the fault handler in multi-threaded setups */
static pthread_spinlock_t g_thread_lock;
#endif /* USE_THREADS */

/* gettid in non offical so not in glibc headers. This works though */
pid_t gettid(void) {
	return syscall(__NR_gettid);
}

/* Get a backtrace from a signal handler.
* array is place to put array
* size is it's size
* context is a pointer to the mysterious signal ahndler 3rd parameter with the registers
* distance is the distance is calls from the signal handler
*
*/
unsigned int signal_backtrace(void** array, unsigned int size, ucontext_t * context, unsigned int distance) {

	/* WARNING: If you ever remove the inline from the function prototype,
	* adjust this to match!!!
	*/
#define IP_STACK_FRAME_NUMBER (3)

	unsigned int ret=backtrace(array, size);
	distance+=IP_STACK_FRAME_NUMBER;

	assert(distance<=size);

	/* OK, here is the tricky part:
	*
	* Linux signal handling on some archs works by the kernel replacing, in situ, the
	* return address of the faulting function on the faulting thread user space stack with
	* that of the Glibc signal unwind handling routine and coercing user space to just to
	* glibc signal handler preamble. Later the signal unwind handling routine undo this.
	*
	* What this means for us is that the backtrace we get is missing the single most important
	* bit of information: the addres of the faulting function.
	*
	* We get it back using the undocumented 3rs parameter to the signal handler call back
	* with used in it's SA_SIGINFO form which contains access to the registers kept during
	* the fault. We grab the IP from there and 'fix' the backtrace.
	*
	* This needs to be different per arch, of course.
	*/

#ifdef __i386__
	array[distance]=(void *)(context->uc_mcontext.gregs[REG_EIP]);
#endif /* __i386__ */

#ifdef __PPC__
	array[distance]=(void *)(context->uc_mcontext.regs->nip);
#endif /* __PPC__ */
	return ret;
}


/* The fault handler function.
*
* OK. The rules of the battle are those:
*
* 1. Can't use any function that relies on malloc and friends working as the malloc arena may be corrupt.
* 2. Can only use a the POSIX.1-2003 list of async-safe functions.
* 3. Some of the functions on the list are not always safe (like fork when atfork() is used),
* so need to avoid these also.
* 4. No locking allowed. We don't know in what state the process/thread was when the exception
* occured.
*/
void fault_handler (int signal, siginfo_t * siginfo, void *context) {
	int i, ret;

#ifdef USE_THREADS

	ret=pthread_spin_trylock(&g_thread_lock);
	if (EBUSY==ret) {
		/* Think of the following as an async-signal safe super sched_yield that
		* yields even to threads with lower real-time priority */
		sigset_t smask;
		sigemptyset(&smask);
		pselect(0, NULL, NULL, NULL, NULL, &smask);
	}

#endif /* USE_THREADS */

	/* Get the backtrace. See signal_backtrace for the parameters */

	g_crash_msg.num_backtrace_frames=signal_backtrace(g_crash_msg.backtrace,
			CRASH_MAX_BACKTRACE_DEPTH, context, 0);

	/* Grab the kernel thread id. Because signal handler are shared between all
	* threads of the same process, this can only be doen in fault time. */

	g_crash_msg.thread_id=gettid();

	/* Grab the signal number */
	g_crash_msg.signal_number=signal;

	/* Grab time stamp */
	clock_gettime(CLOCK_REALTIME, &g_crash_msg.timestamp);

	/* Copy the assert buffer without using strings.h fucntions. */
	for(i=0; i< CRASH_ASSERT_BUFFER_SIZE; ++i) {
		g_crash_msg.assert_buffer[i]=*(g_assert_buf_ptr++);
	}

	if (siginfo) /* No reasons for this to be NULL, but still... */
	{
		/* See description of these in crash_msg.h */
		g_crash_msg.signal_code=siginfo->si_code;
		g_crash_msg.fault_address=siginfo->si_addr;
		g_crash_msg.signal_errno=siginfo->si_errno;
		g_crash_msg.handler_errno=errno;
	}

retry_write:

	ret=write(g_logfd, &g_crash_msg, sizeof(g_crash_msg));

	/* If we got interrupt by a signal, retry the write.
	* This shouldn't really happen since we mask all signals
	* during the handler run via sigaction sa_mask field but
	* it can't hurt to test.
	*
	* It's useless to test for any other condition since we
	* can't do anything if we fail
	*/
	if(ret && EINTR==errno) goto retry_write;

	/* We use backtrace_symbols_fd rather then backtrace_symbols since
	* the latter uses malloc to allocate memory and if we got here
	* because of malloc arena curroption we'll double fault.
	*/
	backtrace_symbols_fd(g_crash_msg.backtrace, g_crash_msg.num_backtrace_frames, g_logfd);

	close(g_logfd);

	/* Produce a core dump for post morteum debugging */
	abort();

	assert(0 /* Not Reached */);

	return;
}

/* Set the FD_CLOEXEC flag of desc if value is nonzero,
	or clear the flag if value is 0.
	Return 0 on success, or -1 on error with errno set. */

int set_cloexec_flag (int desc, int value) {
	int oldflags=fcntl(desc, F_GETFD, 0);

	/* If reading the flags failed, return error indication now. */
	if (oldflags < 0) {
		return oldflags;
	}

	/* Set just the flag we want to set. */
	if (value!=0) {
		oldflags|=FD_CLOEXEC;
	} else {
		oldflags&=~FD_CLOEXEC;
	}

	/* Store modified flag word in the descriptor. */
	return fcntl(desc, F_SETFD, oldflags);
}

/* Registration function. Needs to be called once by each process (not thread)
* process_name is argv[0] or whatever you'd like.
* assert_buf_ptr needs to point to the 128 byte assert buffer.
*/
int register_crash_handler(const char* process_name,unsigned char* assert_buf_ptr) {
	struct sigaction act; /* Signal handler register struct */
	int ret; /* Return value for various calls */
	int pfd[2]; /* Pipe file descriptor array */

	/* See ahead about these two: */
	void * dummy_trace_array[1];
	unsigned int dummy_trace_size;

	assert(sizeof(g_crash_msg)<=CRASH_MAX_MSG_SIZE);

	if(!process_name || !assert_buf_ptr) {
		return EINVAL;
	}
	#ifdef USE_THREADS
	ret=pthread_spin_init(&g_thread_lock, PTHREAD_PROCESS_PRIVATE);
	if(ret) {
		return ret;
	}
	#endif /* USE_THREADS */
	/* If we're called again (perhaps after a fork() ), the pipe is already open.
	* That's just fine with us */
	if(-1==g_logfd) {
		/* Grab us a pipe to communicate with our crash daemon */
		ret=pipe(pfd);
		if(-1==ret) {
			return errno;
		}
		g_logfd=pfd[1]; /* Grab the write end of the pipe */

		/* If the caller program execs, we want the pipe to close,
		* because it's not likely a random program will have the
		* right signal handler set to use the crash daemon. */
		ret=set_cloexec_flag(g_logfd,1);
		if(-1==ret) {
			return errno;
		}
		/* Set our daemon up */
		crashd_main(1,process_name,pfd);
		close(pfd[0]);
	}
	/* This requires some explaining:
	* In theory, neither backtrace nor backtrace_symbold_fd call malloc and friends so
	* we are able to use them in a an exception handler safely.
	*
	* In practice recent glibc versions put these function in a seperate shared library
	* called libgcc_s.so when gets loaded automagically by the dynamic linker when any these
	* of these functions are first used and, you guessed it, the dynamic linker uses malloc
	* in the process to get some internal buffer.
	*
	* We therefore give these a dummy call here during registration to assure that the library
	* gets loaded where it's safe to malloc.
	*/
	dummy_trace_size=backtrace(dummy_trace_array, 1);
	backtrace_symbols_fd(dummy_trace_array, dummy_trace_size,-1);

	/* This data we can already grab during registration, not need to wait for crash */
	g_crash_msg.magic=CRASH_MSG_MAGIC;
	memcpy(g_crash_msg.process_name, process_name, strnlen(process_name, CRASH_MAX_PROCESS_NAME_SIZE)+1);
	g_crash_msg.process_id=getpid();

	g_assert_buf_ptr=assert_buf_ptr;

	/* Prepare a sigaction struct for exception handler registrations */
	memset(&act,0,sizeof(act));
	act.sa_sigaction=fault_handler;
	/* No signals during handler run, please */
	sigfillset(&act.sa_mask);
	/* We want the 3 parameter form of the handler with the siginfo_t addtional data */
	act.sa_flags=SA_SIGINFO;

	/* Register the handler for all exception signals. */
	ret=sigaction(SIGSEGV,&act,NULL);
	ret|=sigaction(SIGILL,&act,NULL);
	ret|=sigaction(SIGFPE,&act,NULL);
	ret|=sigaction(SIGBUS,&act,NULL);
	ret|=sigaction(SIGQUIT,&act,NULL);

	return ret;
}
