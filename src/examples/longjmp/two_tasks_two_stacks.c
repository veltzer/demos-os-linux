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
#include <stdio.h>
#include <setjmp.h> // Include this header
#include <unistd.h> // For sleep()

// Global "jump buffers" to store the context of each task
jmp_buf env_task_a;
jmp_buf env_task_b;

// Task A: A "never-ending" function
void task_a(void) {
    // CRITICAL: Any local variable that you need to persist
    // across a longjmp MUST be declared volatile.
    volatile int i = 0;

    printf("Task A: Started\n");

    while (1) {
        printf("Task A: loop %d\n", i++);
        fflush(stdout); // Flush output before we jump
        sleep(1);

        // 1. Save our current context
        if (setjmp(env_task_a) == 0) {
            // setjmp() returned 0, meaning we just saved.
            // 2. Now, jump to Task B
            longjmp(env_task_b, 1);
        }
        // If we are here, setjmp() returned non-zero,
        // meaning Task B jumped back to us. We just continue our loop.
    }
}

// Task B: Another "never-ending" function
void task_b(void) {
    volatile int j = 0;

    printf("Task B: Started\n");

    while (1) {
        printf("Task B: loop %d\n", j++);
        fflush(stdout);
        sleep(1);

        // 1. Save our current context
        if (setjmp(env_task_b) == 0) {
            // setjmp() returned 0, meaning we just saved.
            // 2. Now, jump back to Task A
            longjmp(env_task_a, 1);
        }
        // If we are here, setjmp() returned non-zero,
        // meaning Task A jumped back to us. We continue our loop.
    }
}

// The "Scheduler"
int main() {
    printf("Starting cooperative multitasking...\n");

    // This is the "scheduler" or "trampoline".
    // We first save a "start context" for Task B.
    if (setjmp(env_task_b) == 0) {
        // setjmp() returned 0.
        // We have saved Task B's "pre-start" location.
        // Now, we will manually start Task A.
        printf("Main: Starting Task A\n");
        task_a(); // This function will never return
    } else {
        // setjmp() returned non-zero.
        // This means Task A performed a longjmp(env_task_b).
        // We are now "in" Task B's context, so we call it.
        printf("Main: Context switching to Task B\n");
        task_b(); // This function will also never return
    }
    
    // This part of the code is never reached.
    return 0;
}
