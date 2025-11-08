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
 * kill_via_pthread_cancel.cc
 *
 * Demonstrates how to use pthread_cancel to terminate another thread.
 * This example shows two threads:
 * 1. A "target" thread that runs in a loop until canceled.
 * 2. A "canceler" thread that waits a few seconds and then cancels the target.
 *
 * How to compile:
 * gcc -o pthread_cancel_example pthread_cancel_example.c -pthread
 *
 * How to run:
 * ./pthread_cancel_example
 */

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err_utils.h>

/**
 * @brief A cleanup handler function.
 * This function is registered by pthread_cleanup_push() and is
 * automatically executed when the thread is canceled.
 */
void cleanup_handler(void *) {
	// This message will print when the thread is canceled
	printf("cleanup...\n");
}

/**
 * @brief The "target" thread function (the one to be canceled).
 *
 * This thread runs an infinite loop, printing a message.
 * It establishes a cleanup handler to run upon cancellation.
 * The sleep() function acts as a "cancellation point," which is
 * where the thread checks for and acts on pending cancel requests.
 */
void* target_thread_func(void *) {
	// Register the cleanup handler. It will be called when this thread
	// is canceled.
	// pthread_cleanup_push and pthread_cleanup_pop must be paired
	// in the same function scope.
	pthread_cleanup_push(cleanup_handler, NULL);
	printf("Target thread (TID: %lu): Started. Running in a loop...\n", (unsigned long)pthread_self());
	// Infinite loop to simulate ongoing work
	while (true) {
		printf("Target thread: ...alive...\n");
		// sleep() is a cancellation point.
		// If a cancellation request is pending, it will be
		// processed here, the cleanup handler will run,
		// and the thread will terminate.
		sleep(1);
	}
	// This part is normally unreachable due to the infinite loop,
	// but pthread_cleanup_pop is required to match pthread_cleanup_push.
	// The '1' means "execute the cleanup handler" if we pop it normally.
	// When canceled, the handler runs regardless of this value.
	pthread_cleanup_pop(1);
	return NULL;
}

/**
 * @brief The "canceler" thread function.
 *
 * This thread waits for a few seconds, then sends a cancellation
 * request to the target thread.
 */
void* canceler_thread_func(void *arg) {
    // The argument is a pointer to the target thread's ID
    pthread_t target_tid = *(pthread_t *)arg;

    printf("Canceler thread (TID: %lu): Started.\n", (unsigned long)pthread_self());

    // Wait for 3 seconds to let the target thread run for a bit
    sleep(3);

    printf("Canceler thread: Sending cancellation request to target thread %lu...\n", (unsigned long)target_tid);

    // Send the cancellation request
    int s = pthread_cancel(target_tid);
    if (s != 0) {
        // pthread functions return the error number directly
        fprintf(stderr, "Canceler thread: Error in pthread_cancel: %s\n", strerror(s));
    } else {
        printf("Canceler thread: Cancellation request sent.\n");
    }

    return NULL;
}

/**
 * @brief Main function
 * Creates and manages the two threads.
 */
int main() {
	pthread_t tid_target, tid_canceler;
	void *res_target;
	void *res_canceler;
	printf("Main (PID: %d): Starting program.\n", getpid());
	// --- Create the target thread ---
	printf("Main: Creating target thread...\n");
	CHECK_ZERO(pthread_create(&tid_target, NULL, target_thread_func, NULL));
	printf("Main: Target thread created with TID %lu.\n", (unsigned long)tid_target);
	// Give the target a moment to start (optional, but good for demo order)
	sleep(1);
	// --- Create the canceler thread ---
	// We pass the target's thread ID as the argument
	printf("Main: Creating canceler thread...\n");
	CHECK_ZERO(pthread_create(&tid_canceler, NULL, canceler_thread_func, &tid_target));
	printf("Main: Canceler thread created with TID %lu.\n", (unsigned long)tid_canceler);
	// --- Wait for threads to finish ---
	// Wait for the canceler thread to finish its job
	printf("Main: Joining canceler thread...\n");
	CHECK_ZERO(pthread_join(tid_canceler, &res_canceler));
	printf("Main: Canceler thread joined.\n");
	// Wait for the target thread to finish
	// We expect this to return immediately after the canceler runs,
	// as the target will have been terminated.
	printf("Main: Joining target thread...\n");
	CHECK_ZERO(pthread_join(tid_target, &res_target));
	// --- Check the result ---
	// When a thread is successfully canceled, pthread_join stores
	// the special value PTHREAD_CANCELED in its result pointer.
	assert(res_target == PTHREAD_CANCELED);
	printf("Main: Program finished.\n");
	return EXIT_SUCCESS;
}
