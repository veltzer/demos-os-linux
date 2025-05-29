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

/*
 * This is the process which is monitored by the watchdog in the watchdog
 * exercise.
 * It just sleeps for a variable amount of time and the dies with a signal.
 */

#include <firstinclude.h>
#include <stdlib.h>	// for EXIT_SUCCCESS, abort(3), srand(3), rand(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for sleep(3), getpid(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <trace_utils.h>// for TRACE()

int main() {
	const unsigned int min_sleep=5;
	const unsigned int max_sleep=10;
	srand(getpid());
	int sleep_time=min_sleep+rand()%(max_sleep-min_sleep);
	TRACE("sleeping for %d seconds", sleep_time);
	CHECK_ZERO(sleep(sleep_time));
	// no return value for abort(3)
	TRACE("dying");
	// cppcheck-suppress nullPointer
	*(char*)0=0;
	// abort();
	return EXIT_SUCCESS;
}
