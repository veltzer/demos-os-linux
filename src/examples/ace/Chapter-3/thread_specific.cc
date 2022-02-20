/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_main.h>
#include <ace/Service_Config.h>
#include <ace/Thread_Manager.h>
#include <ace/Signal.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include "thread_specific.hh"

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lpthread
 */

// Static variables.
ACE_MT(ACE_Thread_Mutex Errno::lock_);
int Errno::flags_;

// This is our thread-specific error handler...
static ACE_TSS<Errno> tss_error;

// Serializes output via cout.
static ACE_SYNCH_MUTEX printf_lock;

typedef ACE_TSS_Guard<ACE_Thread_Mutex> GUARD;

extern "C" void cleanup(void *ptr) {
	ACE_DEBUG((LM_DEBUG, "(%t) in cleanup, ptr=%x\n", ptr));

	delete reinterpret_cast<char *>(ptr);
}

// This worker function is the entry point for each thread.
static void* worker(void* c) {
	intptr_t count=reinterpret_cast<intptr_t>(c);
	ACE_thread_key_t key=ACE_OS::NULL_key;
	int* ip=0;
	// Make one key that will be available when the thread exits so that
	// we'll have something to cleanup!
	if(ACE_Thread::keycreate(&key, cleanup)==-1) {
		ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::keycreate"));
	}
	ACE_NEW_RETURN(ip, int, 0);
	if(ACE_Thread::setspecific(key, (void *)ip)==-1) {
		ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
	}
	for(intptr_t i=0; i<count; i++) {
		if(ACE_Thread::keycreate(&key, cleanup)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::keycreate"));
		}
		ACE_NEW_RETURN(ip, int, 0);
		ACE_DEBUG((LM_DEBUG, "(%t) in worker 1, key=%d, ip=%x\n", key, ip));
		{
			// tmp is workaround for gcc strict aliasing warning.
			void *tmp=reinterpret_cast<void *>(ip);
			if(ACE_Thread::setspecific(key, tmp)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
			}
			if(ACE_Thread::getspecific(key, &tmp)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
			}
			if(ACE_Thread::setspecific(key, (void *)0)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
			}
		}
		delete ip;
		if(ACE_Thread::keyfree(key)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::keyfree"));
		}
		// Cause an error.
		ACE_OS::read(ACE_INVALID_HANDLE, 0, 0);
		// The following two lines set the thread-specific state.
		tss_error->error(errno);
		tss_error->line(__LINE__);
		// This sets the static state (note how C++ makes it easy to do
		// both).
		tss_error->flags(count);
		{
			ACE_hthread_t handle;
			ACE_Thread::self(handle);
			// Use the guard to serialize access to printf...
			ACE_GUARD_RETURN(ACE_Thread_Mutex, ace_mon, printf_lock, 0);
			// Print the thread id portably.
			ACE_OS::printf("(%p)", &handle);
			ACE_OS::printf(" errno=%d, lineno=%d, flags=%d\n",
				tss_error->error(),
				tss_error->line(),
				tss_error->flags()
				);
		}
		key=ACE_OS::NULL_key;
		if(ACE_Thread::keycreate(&key, cleanup)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::keycreate"));
		}
		ACE_NEW_RETURN(ip, int, 0);
		ACE_DEBUG((LM_DEBUG, "(%t) in worker 2, key=%d, ip=%x\n", key, ip));
		{
			// Tmp is workaround for GCC strict aliasing warning.
			void *tmp(reinterpret_cast<void *>(ip));
			if(ACE_Thread::setspecific(key, tmp)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
			}
			if(ACE_Thread::getspecific(key, &tmp)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
			}
			if(ACE_Thread::setspecific(key, (void *)0)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::setspecific"));
			}
		}
		delete ip;
		if(ACE_Thread::keyfree(key)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "ACE_Thread::keyfree"));
		}
	}
	ACE_DEBUG((LM_DEBUG, "(%t) exiting\n"));
	return NULL;
}

extern "C" void handler(int signum) {
	ACE_DEBUG((LM_DEBUG, "signal=%S\n", signum));
	ACE_Thread_Manager::instance()->exit(EXIT_SUCCESS);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// The Service_Config must be the first object defined in main...
	ACE_Service_Config daemon(argv[0]);
	int threads=argc>1 ? ACE_OS::atoi(argv[1]) : 4;
	intptr_t count=argc>2 ? ACE_OS::atoi(argv[2]) : 10;
	// Register a signal handler.
	ACE_Sig_Action sa((ACE_SignalHandler)(handler), SIGINT);
	ACE_UNUSED_ARG(sa);
	if(ACE_Thread_Manager::instance()->spawn_n(threads, ACE_THR_FUNC(&worker), reinterpret_cast<void *>(count), THR_BOUND | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "ACE_Thread_Manager::spawn_n"), -1);
	}
	ACE_Thread_Manager::instance()->wait();
	return EXIT_SUCCESS;
}
