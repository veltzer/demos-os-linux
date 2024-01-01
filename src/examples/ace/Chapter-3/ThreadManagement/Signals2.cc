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
#include <ace/config-lite.h>
#include <ace/OS_NS_time.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

class SignalableTask : public ACE_Task<ACE_MT_SYNCH> {
public:
	virtual int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0) {
		if(signum==SIGUSR1) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) received a %S signal\n"), signum));
			handle_alert();
		}
		return(0);
	}
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Starting thread\n")));
		while(true) {
			ACE_Message_Block *mb=0;
			ACE_Time_Value tv(0, 1000);
			tv+=ACE_OS::time(0);
			int result=this->getq(mb, &tv);
			if((result==-1) && (errno==EWOULDBLOCK)) {
				continue;
			} else {
				process_message(mb);
			}
		}
		ACE_NOTREACHED(return 0);
	}
	void handle_alert();
	void process_message(ACE_Message_Block *mb);
};

void SignalableTask::process_message(ACE_Message_Block *) {
}

void SignalableTask::handle_alert(void) {
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Main thread \n")));
	SignalableTask handler;
	handler.activate(THR_NEW_LWP | THR_JOINABLE, 5);
	ACE_Sig_Handler sh;
	sh.register_handler(SIGUSR1, &handler);
	// Allow threads to start
	ACE_OS::sleep(1);
	for(int i=0; i<5; i++) {
		ACE_OS::kill(ACE_OS::getpid(), SIGUSR1);
	}
	handler.wait();
	return EXIT_SUCCESS;
}
