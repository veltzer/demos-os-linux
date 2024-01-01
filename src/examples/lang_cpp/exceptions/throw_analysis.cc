/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for sleep(3)
#include <trace_utils.h>// for TRACE()
#include <signal_utils.h>	// for signal_register_handler_signal()
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()

/*
 * A simple demo of throwing an exception and not catching it
 *
 * This example shows that throwing an exception will terminate any running threads
 * and the program as a whole.
 * The same applied for exit(3).
 *
 * We use a thread here to show that it stops...
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

static void handler(int sig) {
	TRACE("in signal handler for signal [%d],[%s]", sig, signal_get_by_val(sig));
}

static void *worker(void *p) {
	unsigned int counter=0;
	while(true) {
		TRACE("counter is %d", counter);
		CHECK_ZERO(sleep(1));
		counter++;
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	// to catch when the signal is thrown
	signal_register_handler_signal(SIGABRT, handler);
	pthread_t thread;
	CHECK_ZERO_ERRNO(pthread_create(&thread, NULL, worker, NULL));
	CHECK_ZERO(sleep(5));
	throw 20;
	std::cout << "Where did this go?" << std::endl;
	CHECK_ZERO_ERRNO(pthread_join(thread, NULL));
	return EXIT_SUCCESS;
}
