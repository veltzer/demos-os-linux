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
#include <ace/OS_NS_time.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Scheduling timers with the Reactor
 *
 * This example demostrates the scheduling of timers on a reactor and the ability
 * to handle signals at the same time with the same singleton reactor.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

// This is our timer handler which handles some periodic job in the system
class MyTimerHandler:public ACE_Event_Handler {
public:
	int handle_timeout(const ACE_Time_Value& current_time, const void* =0) {
		time_t epoch=((timespec_t)current_time).tv_sec;
		ACE_DEBUG((LM_INFO, "handle_timeout: %s\n", ACE_OS::ctime(&epoch)));
		return 0;
	}
};

// Create a SIGINT handler so that we can exit the program politely
class SigintHandler:public ACE_Event_Handler {
public:
	int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0) {
		static int counter=0;
		counter++;
		if (signum==SIGINT) {
			if (counter > 2) {
				ACE_DEBUG((LM_INFO, "Got SIGINIT exiting\n"));
				ACE_Reactor::instance()->end_reactor_event_loop();
			} else {
				ACE_DEBUG((LM_INFO, "Got SIGINIT ignoring\n"));
			}
		}
		return 0;
	}
};

// This is our main, it registers two event handlers with the singleton reactor
// the first is the timer handler
// the second is the signal handler
// then it simply goes into the reactor main loop.
// it relies on the someone (in this case the signal hanlder) to stop it...
int main() {
	MyTimerHandler *timer=new MyTimerHandler();
	ACE_Time_Value initialDelay(3);
	ACE_Time_Value interval(5);
	ACE_Reactor::instance()->schedule_timer(timer, 0, initialDelay, interval);
	SigintHandler *handleExit=new SigintHandler();
	ACE_Reactor::instance()->register_handler(SIGINT, handleExit);
	ACE_DEBUG((LM_INFO, "Pressing 3 times on ^c will stop execution\n"));
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
