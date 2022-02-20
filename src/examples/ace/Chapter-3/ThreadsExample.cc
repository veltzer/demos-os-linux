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
#include <ace/Thread.h>
#include <ace/Synch.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lpthread
 */

static int number=0;
static int seed=0;
static void* worker(void *arg) {
	ACE_UNUSED_ARG(arg);
	ACE_DEBUG((LM_DEBUG, "Thread (%t) Created to do some work"));
	::number++;
	ACE_DEBUG((LM_DEBUG, " and number is %d\n", ::number));
// Let the other guy go while I fall asleep for a random period of time
	ACE_OS::sleep(ACE_OS::rand() % 2);
// Exiting now
	ACE_DEBUG((LM_DEBUG, "\t\t Thread (%t) Done! \t The number is now: %d\n", number));
	return(0);
}

/*
 * class M {
 * public:
 * void run() {
 * ACE_DEBUG((LM_DEBUG, "Thread (%t) Created to do some work"));
 * ::number++;
 * ACE_DEBUG((LM_DEBUG, " and number is %d\n", ::number));
 * //Let the other guy go while I fall asleep for a random period of time
 * ACE_OS::sleep(ACE_OS::rand() % 2);
 * //Exiting now
 * ACE_DEBUG((LM_DEBUG, "\t\t Thread (%t) Done! \t The number is now: %d\n", number));
 * }
 * };
 *
 * static void* run_method(void* arg) {
 * M* p=(M*)arg;
 * p->run();
 * return NULL;
 * }
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	if(argc<2) {
		ACE_DEBUG((LM_DEBUG, "%s: usage: %s <number of threads>\n", argv[0], argv[0]));
		ACE_OS::exit(EXIT_FAILURE);
	}
	// setup the random number generator
	ACE_OS::srand(::seed);
	// number of threads to spawn
	unsigned int n_threads=ACE_OS::atoi(argv[1]);
	ACE_thread_t* threadID=new ACE_thread_t[n_threads + 1];
	ACE_hthread_t* threadHandles=new ACE_hthread_t[n_threads + 1];
	// spawn n_threads
	if(ACE_Thread::spawn_n(threadID, n_threads, (ACE_THR_FUNC)worker, NULL, THR_JOINABLE | THR_NEW_LWP, ACE_DEFAULT_THREAD_PRIORITY, NULL, 0, threadHandles)!=n_threads) {
		ACE_DEBUG((LM_DEBUG, "Error in spawning thread\n"));
	}
	// Wait for all the threads to exit before you let the main fall
	// through and have the process exit.
	for(unsigned int i=0; i<n_threads; i++) {
		ACE_Thread::join(threadHandles[i]);
	}
	return EXIT_SUCCESS;
}
