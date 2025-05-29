/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
 * Reactor examples
 * Timers & state data
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class TimerHandler: public ACE_Event_Handler {
private:
	int myid;

public:
	TimerHandler(int imyid) : myid(imyid) {
	}
	int handle_timeout(const ACE_Time_Value& current_time __attribute__((unused)), const void* arg) {
		printf("%d in here %p\n", myid, arg);
		return 0;
	}
};

int main() {
	TimerHandler *handler=new TimerHandler(1);
	TimerHandler *handler2=new TimerHandler(2);
	ACE_Reactor::instance()->schedule_timer(
		handler,
		handler,
		ACE_Time_Value(0),
		ACE_Time_Value(10)
		);
	ACE_Reactor::instance()->schedule_timer(
		handler2,
		handler2,
		ACE_Time_Value(0),
		ACE_Time_Value(7)
		);
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
