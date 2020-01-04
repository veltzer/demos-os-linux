/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/OS_NS_unistd.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lACE -ldl -lrt -lpthread
 */

class CanceledTask : public ACE_Task<ACE_MT_SYNCH> {
public:
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Starting thread\n")));
		if (this->set_cancel_mode() < 0) {
			return(-1);
		}
		while(true) {
			// Put this thread in a compute loop.. no
			// cancellation points are available.
		}
		// This is only to workaround a warning of the compiler.
		return(0);
	}
	int set_cancel_mode(void) {
		cancel_state new_state;

		// Set the cancel state to asynchronous and enabled.
		new_state.cancelstate=PTHREAD_CANCEL_ENABLE;
		new_state.canceltype=PTHREAD_CANCEL_ASYNCHRONOUS;
		if (ACE_Thread::setcancelstate(new_state, 0)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("cancelstate")), -1);
		}
		return(0);
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	CanceledTask task;
	task.activate();
	ACE_OS::sleep(1);
	ACE_Thread_Manager::instance()->cancel_task(&task, 1);
	task.wait();
	return EXIT_SUCCESS;
}
