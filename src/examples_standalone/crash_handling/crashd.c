/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for exit(3), EXIT_SUCCESS, EXIT_FAILURE
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>

#include "crash_msg.h"

/* The buffer holds the message + ancillary data, such as symbol stack traced
* The pointer is used to cash the buffer to the header */
static char crash_msg_buf[CRASH_MAX_MSG_SIZE];
static struct crash_message_struct * crash_msg=(struct crash_message_struct *)&crash_msg_buf;

/* A simple compiler only memory barrier, both read and write */
#define mb(x) asm volatile ("":::"memory")

/* When this is set from SIGTERM signal handler it's
* time to terminate.
*
* NOTE: It's a very good idea to kill crashd before any process
* relying on it for exception handling.
*/
static char terminate_flag=0;

/* When this is set from the SIGALRM signal handler
* it means our time to read crash details is out
*/

static char timeout_flag=0;

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

/* Call this to reboot. Production version must be async-signal safe */
static void inline do_reboot(void) {

#ifdef NDEBUG

	char * reboot_argv[]={ "reboot", NULL};
	char * reboot_env[]={NULL};

	execve("/sbin/reboot", reboot_argv, reboot_env);

#else /* NDEBUG */

	fprintf(stderr, "Boo!!! would have rebooted but running in debug mode. Have a nice day.\n");
	exit(EXIT_FAILURE);

#endif /* NDEBUG */

	/* NOT REACHED */
	return;
}

/* Handle the crash data
* This is just an example: it speqs the entire message to stderr in human readable form
*/
static void handle_crash(void) {

	int i;

	assert(crash_msg!=NULL);
	assert(sizeof(crash_msg->assert_buffer[0])==sizeof(unsigned char));


	fprintf (stderr,
			"\n********************"
			"\n* EXCEPTION CAUGHT *"
			"\n********************\n"
			"Process name: %s\n"
			"Process ID: %d\n"
			"Thread ID: %d\n"
			"Exception: %s\n"
			"Reason: %s\n"
			"Fault Address: %p\n"
			"Signal error: %s\n"
			"Last error: %s\n"
			"Time stamp: %s"
			"Assert buffer: %s",
			crash_msg->process_name,
			crash_msg->process_id,
			crash_msg->thread_id,
			strsignal(crash_msg->signal_number),
			code2str(crash_msg->signal_code, crash_msg->signal_number),
			crash_msg->fault_address,
			strerror(crash_msg->signal_errno),
			strerror(crash_msg->handler_errno),
			ctime(&(crash_msg->timestamp.tv_sec)),
			crash_msg->assert_buffer

	);

	fprintf(stderr, "\nStack trace addresses:\n");
	for(i=0; i< crash_msg->num_backtrace_frames; ++i) {
			fprintf(stderr, "[%d] %p\n", i, crash_msg->backtrace[i]);
	}

	fprintf(stderr, "\nAncillary data follows:\n");
	fprintf(stderr, "%s\n",crash_msg->ancillary_data);
	fflush(NULL);

	return;
}

static void term_sig_handler(int signal) {

	terminate_flag=1;
	mb();
	return;
}

/* Timeout reading crash data */
static void alarm_sig_handler(int signal) {

	timeout_flag=1;
	mb();
	return;
}

/* Our very own fault handler.
* If we ever got it it means something is very very wrong.
* Trying to save debug info is useless. We probably got here
* because of a fault when processing some crash. The chances
* are very slim that we'll be able to save any meaningfull
* data and we risk getting stuck instead of resetting the system,
* so we just reboot
*/
static void fault_sig_handler(int signal) {

	do_reboot();
	return;
}


/* Utility function to register a simple signal handler with no flags
* (as opposed to signal(2))
*/
static int register_signal(int signo, sighandler_t handler) {

	struct sigaction act;

	memset(&act, 0, sizeof (act));
	act.sa_handler=handler;
	sigemptyset (&act.sa_mask);
	act.sa_flags=0;

	return sigaction (signo, &act, NULL);
}

/* The main deal */
void crashd_main(char daemonise_flag, const char * progname, int pfd[])
{
	int ret, fd;
	char * p=crash_msg_buf;
	int remaining_bytes=CRASH_MAX_MSG_SIZE;
	fd_set rfds;

	ret=fork();

	if(ret) {
		return;
	} else {
		close(pfd[1]);
		fd=pfd[0];
	}

	/* This forks again, closing stdin/out/err and loose our TTY, if asked to. */
	if(daemonise_flag) {
		ret=daemon(0, 1);
		if(-1==ret) goto bail_out;
	}

	/* Register all signal handlers for timeout, kill and fault */
	ret=register_signal(SIGTERM, term_sig_handler);
	if(-1==ret) goto bail_out;

	ret=register_signal(SIGALRM, alarm_sig_handler);
	if(-1==ret) goto bail_out;

	ret=register_signal(SIGSEGV, fault_sig_handler);
	if(-1==ret) goto bail_out;

	ret=register_signal(SIGILL, fault_sig_handler);
	if(-1==ret) goto bail_out;

	ret=register_signal(SIGFPE, fault_sig_handler);
	if(-1==ret) goto bail_out;

	ret=register_signal(SIGBUS, fault_sig_handler);
	if(-1==ret) goto bail_out;


	/* OK, wait for someone to tickle us */

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	ret=select(fd+1, &rfds, NULL, NULL, NULL);

	/* Deal correctly with random harmless signals
	* Especially useful for when we run under debugger */
	while(-1==ret && EINTR==errno) {

		mb();
		if(terminate_flag) exit(EXIT_SUCCESS);

		ret=select(fd+1, &rfds, NULL, NULL, NULL);
	}

	if(-1==ret) goto bail_out;

	/* OK, we have action. First thing arm the timer */
	(void)alarm(READ_TIMEOUT);

	/* This crazy loop reads the message in, possbly in several parts.
	* We continue when we're done or when it's time to leave.
	*/

	do {

		ret=read(fd, p, remaining_bytes);

		/* We need to exit if the end closed the pipe or if we asked to terminate */
		if((0==ret) || terminate_flag) {
			break;
		}

		/* Oh oh... we're late. Time out. */
		if(timeout_flag) break;

		/* Handle random signals nicely */
		if(-1==ret && EINTR==errno) continue;

		/* Read errors make us nervous. log and bail out */
		if(-1==ret) break;

		p+=ret;
		remaining_bytes-=ret;

	} while(ret && (remaining_bytes>0));

	assert(CRASH_MSG_MAGIC==crash_msg->magic);

	/* Make sure the process name has an ending NULL */
	crash_msg->process_name[CRASH_MAX_PROCESS_NAME_SIZE-1]='\0';

	close(fd);

	/* Go process the crash */
	handle_crash();

	exit(EXIT_SUCCESS);

bail_out:
	/* Oy very... if we got here it means that the crash daemon has itself
	* encountered some error. We simply record it in the usuall format and
	* initaite a crash as normal.
	*
	* We don't bother with the backtrace symbols since there is only main here
	* but we do put a meaningful error message as ancillary data.
	*
	* Hope this never happens...
	*/

	strncpy(crash_msg->process_name, progname, CRASH_MAX_PROCESS_NAME_SIZE-1);
	crash_msg->process_id=getpid();
	crash_msg->thread_id=getpid();
	crash_msg->handler_errno=errno;
	clock_gettime(CLOCK_REALTIME, &crash_msg->timestamp);
	crash_msg->num_backtrace_frames=backtrace(crash_msg->backtrace, CRASH_MAX_BACKTRACE_DEPTH);
	snprintf(crash_msg->ancillary_data, CRASH_ANCILLARY_DATA_SIZE-1,
			"crashd bailing out due to %s\n",strerror(errno));

	handle_crash();


	/* THE END */
	return;
}



