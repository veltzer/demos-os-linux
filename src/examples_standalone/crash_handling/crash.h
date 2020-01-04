/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CRASH_H
#define CRASH_H

#include <firstinclude.h>
#include <signal.h>

/* Main library registration function.
* Call this once for each process (not thread). Recall after fork() if you want
* process name and ID to be updated
*
*/
int register_crash_handler(
	const char * process_name, /* Name of Process, such as argv[0] */
	unsigned char * assert_buf_ptr /* Pointer to assert buffer */
);

/* Asks this thread to dump. You can use this for asserts. */
int static inline crash_dump(void) {
	asm volatile ("":::"memory");
	return raise(SIGQUIT);
}

#ifdef USE_THREADS

/* Ask some other thread to dump. You can use this for asserts. */
int static inline crash_dump_thread(pthread_t thread) {
	asm volatile ("":::"memory");
	return pthread_kill(thread, SIGUSR1);
}
#endif /* USE_THREADS */


#endif /* CRASH_H */

