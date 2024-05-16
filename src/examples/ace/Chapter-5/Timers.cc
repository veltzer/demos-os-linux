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
#include <ace/config-all.h>
#include <ace/OS_main.h>
#include <ace/streams.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_time.h>
#include <ace/OS_NS_signal.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

typedef void (*timerTask_t)(void);

pid_t timerTask(int initialDelay, int interval, timerTask_t task) {
	if((initialDelay < 1) && (interval < 1)) {
		return(-1);
	}
	pid_t pid=ACE_OS::fork();
	if(pid < 0) {
		return(-1);
	}
	if(pid > 0) {
		return(pid);
	}
	if(initialDelay > 0) {
		ACE_OS::sleep(initialDelay);
	}
	if(interval < 1) {
		return(0);
	}
	while(true) {
		(*task)();
		ACE_OS::sleep(interval);
	}
	ACE_NOTREACHED(return 0);
}

void foo() {
	time_t now=ACE_OS::time(0);
	cerr << "The time is " << ACE_OS::ctime(&now) << endl;
}

void programMainLoop(void) {
	ACE_OS::sleep(30);
}

int main() {
	pid_t timerId=timerTask(3, 5, foo);
	programMainLoop();
	ACE_OS::kill(timerId, SIGINT);
	return EXIT_SUCCESS;
}
