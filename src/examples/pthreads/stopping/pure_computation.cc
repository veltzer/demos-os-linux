/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <pthread.h>	// for pthread_setcancelstate(3), pthread_setcanceltype(3)
#include <unistd.h>	// for sleep(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <trace_utils.h>// for TRACE()

/*
 * This is an example showing how you can cancel a thread that is doing pure computation.
 * The idea is to set the pthread canceltype to PTHREAD_CANCEL_ASYNCHRONOUS.
 *
 * NOTES:
 * - it seems that pure computation is one of the few scenarios where PTHREAD_CANCEL_ASYNCHRONOUS
 * is effective as otherwise it would the state of malloc(3) and possibly other data structures
 * and locks in a bad state.
 * - cancelling a thread which is set to PTHREAD_CANCEL_ASYNCHRONOUS happens *immediately* which
 * is good in some cases.
 * - once you cancel the thread it just stops running, you dont get to react to the cancellation.
 * - a cancelled thread gets PTHREAD_CANCELED back as a result from pthread_join(3). This also
 * means that PTHREAD_CANCELED should not be used as a valid return value for a pthread thread.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static void * thread_func(void *ignored_argument) {
	/* By default threads are cancelleble but their canceltype is PTHREAD_CANCEL_DEFERRED */
	CHECK_ZERO_ERRNO(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));
	TRACE("at start");
	while(true) {
	}
	TRACE("at end");
}

int main(int argc, char** argv, char** envp) {
	TRACE("PTHREAD_CANCELED is %p", PTHREAD_CANCELED);
	/* Start a thread and then send it a cancellation request */
	pthread_t thr;
	CHECK_ZERO_ERRNO(pthread_create(&thr, NULL, &thread_func, NULL));
	CHECK_ZERO(sleep(2));	/* Give thread a chance to get started */
	TRACE("sending cancellation request");
	CHECK_ZERO_ERRNO(pthread_cancel(thr));
	/* Join with thread to see what its exit status was */
	void* res;
	CHECK_ZERO_ERRNO(pthread_join(thr, &res));
	if (res==PTHREAD_CANCELED)
		TRACE("thread was canceled");
	else
		TRACE("thread wasn't canceled (shouldn't happen!)");
	return EXIT_SUCCESS;
}
