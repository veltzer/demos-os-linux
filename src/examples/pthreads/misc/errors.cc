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
#include <stdio.h>	// for fprintf(3)
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <errno.h>	// for errno
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()

/*
 * This is an example exploring handling errors returned from the pthread library.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

int main() {
	pthread_t thread=0;
	CHECK_ZERO_ERRNO(pthread_join(thread, NULL));
	return EXIT_SUCCESS;
}
