/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <ucontext.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <string.h>


/* gettid in non offical so not in glibc headers. This works though */
pid_t gettid (void) {
	return syscall(__NR_gettid);
}

/* This translates a signal code into a readable string */
static inline char * code2str(int code, int signal) {

	switch(code) {
		case SI_USER:
			return "kill, sigsend or raise ";
		case SI_KERNEL:
			return "kernel";
		case SI_QUEUE:
			return "sigqueue";
	}

	if(SIGILL==signal) switch(code) {
		case ILL_ILLOPC:
			return "illegal opcode";
		case ILL_ILLOPN:
			return "illegal operand";
		case ILL_ILLADR:
			return "illegal addressing mode";
		case ILL_ILLTRP:
			return "illegal trap";
		case ILL_PRVOPC:
			return "privileged register";
		case ILL_COPROC:
			return "coprocessor error";
		case ILL_BADSTK:
			return "internal stack error";
	}

	if(SIGFPE==signal) switch(code) {
		case FPE_INTDIV:
			return "integer divide by zero";
		case FPE_INTOVF:
			return "integer overflow";
		case FPE_FLTDIV:
			return "floating point divide by zero";
		case FPE_FLTOVF:
			return "floating point overflow";
		case FPE_FLTUND:
			return "floating point underflow";
		case FPE_FLTRES:
			return "floating point inexact result";
		case FPE_FLTINV:
			return "floating point invalid operation";
		case FPE_FLTSUB:
			return "subscript out of range";
	}

	if(SIGSEGV==signal) switch(code) {
		case SEGV_MAPERR:
			return "address not mapped to object";
		case SEGV_ACCERR:
			return "invalid permissions for mapped object";
	}

	if(SIGBUS==signal) switch(code) {
		case BUS_ADRALN:
			return "invalid address alignment";
		case BUS_ADRERR:
			return "non-existent physical address";
		case BUS_OBJERR:
			return "object specific hardware error";
	}

	if(SIGTRAP==signal) switch(code) {
		case TRAP_BRKPT:
			return "process breakpoint";
		case TRAP_TRACE:
			return "process trace trap";
	}

	return "Unhandled signal handler";
}

void fault_handler (int signal, siginfo_t * siginfo, void *context)
{
	#define MAX_FRAMES 25

	void * frames[MAX_FRAMES];
	int num_frames;
	struct timespec timestamp;
	char ** symbols;
	int i;

	/* Grab time stamp */
	clock_gettime(CLOCK_REALTIME, &timestamp);

	fprintf (stderr,
				"\n********************"
				"\n* EXCEPTION CAUGHT *"
				"\n********************\n"
				"Process ID: %d\n"
				"Thread ID: %d\n"
				"Exception: %s\n"
				"Reason: %s\n"
				"Fault Address: %p\n"
				"Signal error: %s\n"
				"Last error: %s\n"
				"Time stamp: %s",
				getpid(),
				gettid(),
				strsignal(signal),
				code2str(siginfo->si_code, signal),
				siginfo->si_addr,
				strerror(siginfo->si_errno),
				strerror(errno),
				ctime(&timestamp.tv_sec)
		);

	/* Get the backtrace. */
	num_frames=backtrace(frames, MAX_FRAMES);

	symbols=backtrace_symbols(frames, num_frames);

	if(symbols) {
		for(i=0; i< num_frames; i++) {
			fprintf(stderr, "%s\n", symbols[i]);
		}
	}

	fflush(NULL);

	free(symbols);

	/* Produce a core dump for in depth debugging */
	abort();

	return;
}




void print_message_function (void *ptr);
unsigned char buf[128];

int main(int argc,char** argv,char** envp) {
	pthread_t thread1;
	pthread_t thread2;
	int ret;
	struct sigaction act;	/* Signal handler register struct */

	/* Prepare a sigaction struct for exception handler registrations */
	memset(&act, 0, sizeof (act));
	act.sa_sigaction=fault_handler;
	/* No signals during handler run, please */
	sigfillset (&act.sa_mask);
	/* We want the 3 parameter form of the handler with the siginfo_t addtional data */
	act.sa_flags=SA_SIGINFO;

	/* Register the handler for all exception signals. */
	ret=sigaction (SIGSEGV, &act, NULL);
	ret|=sigaction (SIGILL, &act, NULL);
	ret|=sigaction (SIGFPE, &act, NULL);
	ret|=sigaction (SIGBUS, &act, NULL);
	ret|=sigaction (SIGQUIT, &act, NULL);

	printf("Starting first run\n");
	fflush(NULL);

	/* create two threads and let them race */
	pthread_create (&thread1, NULL, (void *) &print_message_function, NULL);
	pthread_create (&thread2, NULL, (void *) &print_message_function, NULL);
	pthread_join (thread2, NULL);

	/* Not reached */
	printf("This should never happen!\n");
	fflush(NULL);
	assert(0);
	pthread_join (thread1, NULL);

	return 0;
}

/* This function generaters a fault.
* We try to REALLY be nasty and screw things up bad.
*/

void croak(void) {
	int *ip=(int *) 17;
	/* Do a simple system that fails so that errno has some interesting
	* value to check
	*/
	write(3000, "xxx", 3);
	/* Try to put 7 in address 17. This is an illegal memory access.
	* Sit back and watch the fire works...
	*/
	*ip=7;
}

/* A filler function so that we'll have a meanigful stack.
* The volatile int is used to keep the compiler from optimizing
* this function away
*/

void die(void) {
	volatile int i=12;
	croak();
	i++;
	return;
}

/* The test thread function */
void print_message_function(void *dummy) {
	/* Latin: "those who about to die sallute you". */
	printf ("Morituri te salutant!\n");
	fflush(NULL);

	/* Call the crasher functions */
	die ();

	pthread_exit (0);
}
