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
#include <ace/Thread_Hook.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include "SecurityContext.hh"

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

class HA_ThreadHook : public ACE_Thread_Hook {
public:
	virtual ACE_THR_FUNC_RETURN start(ACE_THR_FUNC func, void *arg) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) New Thread Spawned\n")));
		// Create the context on the thread's own stack.
		ACE_TSS<SecurityContext> secCtx;
		// Special initialization.
		add_sec_context_thr(secCtx);
		return((*func)(arg));
	}
	void add_sec_context_thr(ACE_TSS<SecurityContext>& secCtx);
};

void HA_ThreadHook::add_sec_context_thr(ACE_TSS<SecurityContext>& secCtx) {
	secCtx->user=0;
}

class HA_CommandHandler : public ACE_Task_Base {
public:
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) starting up \n")));
		// Do something.
		return(0);
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	HA_ThreadHook hook;
	ACE_Thread_Hook::thread_hook(&hook);
	HA_CommandHandler handler;
	handler.activate();
	handler.wait();
	return EXIT_SUCCESS;
}
