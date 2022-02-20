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
#include <ace/OS_NS_time.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Changing the interval
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

class MyTimerHandler : public ACE_Event_Handler {
public:
	int handle_timeout(const ACE_Time_Value& current_time, const void* =0) {
		time_t epoch=((timespec_t)current_time).tv_sec;
		ACE_DEBUG((LM_INFO, ACE_TEXT("handle_timeout: %s"), ACE_OS::ctime(&epoch)));
		return(0);
	}
};

class SigintHandler : public ACE_Event_Handler {
public:
	SigintHandler(long timerId, int currentInterval) : ACE_Event_Handler(), timerId_(timerId), currentInterval_(currentInterval) {
	}
	int handle_signal(int, siginfo_t* =0, ucontext_t* =0) {
		ACE_DEBUG((LM_INFO, ACE_TEXT("Resetting interval of timer %d to %d\n"), this->timerId_, ++this->currentInterval_));
		ACE_Time_Value newInterval(this->currentInterval_);
		ACE_Reactor::instance()->reset_timer_interval(this->timerId_, newInterval);
		return(0);
	}

private:
	long timerId_;
	int currentInterval_;
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_Time_Value initialDelay(3);
	ACE_Time_Value interval(5);
	MyTimerHandler *handler=new MyTimerHandler();
	long timerId=ACE_Reactor::instance()->schedule_timer(handler, 0, initialDelay, interval);
	SigintHandler *handleSigint=new SigintHandler(timerId, 5);
	ACE_Reactor::instance()->register_handler(SIGINT, handleSigint);
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
