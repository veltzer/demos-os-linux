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
#include <ace/OS_main.h>
#include <ace/Thread.h>
#include <ace/Thread_Manager.h>
#include <ace/Get_Opt.h>
#include <ace/Atomic_Op.h>
#include <ace/Guard_T.h>
#include <ace/RW_Mutex.h>
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE

/*
 * This demonstration program verifies the functionality of the ACE_OS
 * implementation of readers/writer locks on Win32 and Posix pthreads.
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lpthread
 */

// Default number of iterations.
static int n_iterations=1000;

// Default number of loops.
static int n_loops=100;

// Default number of readers.
static int n_readers=6;

// Default number of writers.
static int n_writers=2;

// Thread id of last writer.
static ACE_thread_t shared_thr_id;

// Lock for shared_thr_id.
static ACE_RW_Mutex rw_mutex;

// Count of the number of readers and writers.
ACE_Atomic_Op<ACE_Thread_Mutex, int> current_readers, current_writers;

// Thread manager
static ACE_Thread_Manager thr_mgr;

// Explain usage and exit.
static void print_usage_and_die(void) {
	ACE_DEBUG((LM_DEBUG, "usage: %n [-r n_readers] [-w n_writers] [-n iteration_count]\n"));
	ACE_OS::exit(EXIT_FAILURE);
}

// Parse the command-line arguments and set options.
static void parse_args(int argc, ACE_TCHAR** argv) {
	ACE_Get_Opt get_opt(argc, argv, ACE_TEXT("r:w:n:l:"));
	int c;
	while((c=get_opt())!=-1) {
		switch (c) {
		case 'r':
			n_readers=ACE_OS::atoi(get_opt.opt_arg());
			break;
		case 'w':
			n_writers=ACE_OS::atoi(get_opt.opt_arg());
			break;
		case 'n':
			n_iterations=ACE_OS::atoi(get_opt.opt_arg());
			break;
		case 'l':
			n_loops=ACE_OS::atoi(get_opt.opt_arg());
			break;
		default:
			print_usage_and_die();
			break;
		}
	}
}

// Iterate <n_iterations> each time checking that nobody modifies the
// data while we have a read lock.
static void *reader(void *) {
	ACE_DEBUG((LM_DEBUG, "(%t) reader starting\n"));
	for(int iterations=1; iterations<=n_iterations; iterations++) {
		ACE_Read_Guard<ACE_RW_Mutex> g(rw_mutex);

		++current_readers;
		if(current_writers > 0) {
			ACE_DEBUG((LM_DEBUG, "(%t) writers found!!!\n"));
		}
		ACE_thread_t thr_id=shared_thr_id;
		for(int loop=1; loop<=n_loops; loop++) {
			ACE_Thread::yield();
			if(ACE_OS::thr_equal(shared_thr_id, thr_id)==0) {
				ACE_DEBUG((LM_DEBUG, "(%t) somebody changed %d to %d\n", thr_id, shared_thr_id));
			}
		}
		--current_readers;

		ACE_Thread::yield();
	}
	return(0);
}

// Iterate <n_iterations> each time modifying the global data
// and checking that nobody steps on it while we can write it.
static void *writer(void *) {
	ACE_DEBUG((LM_DEBUG, "(%t) writer starting\n"));
	for(int iterations=1; iterations<=n_iterations; iterations++) {
		ACE_Write_Guard<ACE_RW_Mutex> g(rw_mutex);
		++current_writers;
		if(current_writers > 1) {
			ACE_DEBUG((LM_DEBUG, "(%t) other writers found!!!\n"));
		}
		if(current_readers > 0) {
			ACE_DEBUG((LM_DEBUG, "(%t) readers found!!!\n"));
		}
		ACE_thread_t self=ACE_Thread::self();
		shared_thr_id=self;
		for(int loop=1; loop<=n_loops; loop++) {
			ACE_Thread::yield();
			if(ACE_OS::thr_equal(shared_thr_id, self)==0) {
				ACE_DEBUG((LM_DEBUG, "(%t) somebody wrote on my data %d\n", shared_thr_id));
			}
		}
		--current_writers;

		ACE_Thread::yield();
	}
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_LOG_MSG->open(argv[0]);
	parse_args(argc, argv);
	current_readers=0;
	// Possibly already done
	current_writers=0;
	// Possibly already done
	ACE_DEBUG((LM_DEBUG, "(%t) main thread starting\n"));
	// Spawn off threads.
	if((thr_mgr.spawn_n(n_readers, (ACE_THR_FUNC)reader, 0, THR_NEW_LWP)==-1) || (thr_mgr.spawn_n(n_writers, (ACE_THR_FUNC)writer, 0, THR_NEW_LWP)==-1)) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn_n"), 1);
	}
	thr_mgr.wait();
	ACE_DEBUG((LM_DEBUG, "(%t) exiting main thread\n"));
	return EXIT_SUCCESS;
}
