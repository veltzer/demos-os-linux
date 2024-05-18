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
#include <pthread.h>	// for pthread_mutex_init(3), pthread_mutex_t(o)
#include <stdlib.h>	// for EXIT_SUCCESS, NULL
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <pthread_utils.h>	// for pthread_print_mutexattr()

/*
 * This example explores what attributes linux provides for mutexes.
 * Attributes include: type, pshared, protocol, prioceiling and robustness which is not documented at all!
 *
 * Why do you need this example? For instance, the default protocol that a mutex
 * uses is not is not explicitly stated by:
 * pthread_mutexattr_getprotocol(3)
 * pthread_mutexattr_setprotocol(3)
 *
 * References:
 * http://stackoverflow.com/questions/4252005/what-is-the-attribute-of-a-pthread-mutex
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

int main() {
	pthread_mutexattr_t myattr;
	CHECK_ZERO_ERRNO(pthread_mutexattr_init(&myattr));
	pthread_print_mutexattr(&myattr);
	CHECK_ZERO_ERRNO(pthread_mutexattr_destroy(&myattr));
	return EXIT_SUCCESS;
}
