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
#include <err_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/epoll.h>
#include <time.h>
#include <errno.h>
#include <string.h>

/*
 * EXTRA_COMPILE_FLAGS=-std=c++20
 */


#ifndef PIDFD_THREAD
#define PIDFD_THREAD 0x1
#endif

#define NUM_THREADS 5
#define MAX_SLEEP_SECS 5

struct thread_info {
    int id;
    pid_t tid;
    int pidfd;
    int sleep_time;
};

static void *worker(void *arg)
{
    struct thread_info *info = (thread_info*)arg;
    
    // Store our TID so main thread can create pidfd
    info->tid = syscall(SYS_gettid);
    printf("[Thread %d] Started (tid=%d), sleeping for %d seconds\n", info->id, info->tid, info->sleep_time);
    sleep(info->sleep_time);
    printf("[Thread %d] Exiting\n", info->id);
    return NULL;
}

int main(void)
{
    struct thread_info threads[NUM_THREADS];
    pthread_t pthreads[NUM_THREADS];
    int epfd;
    int alive = NUM_THREADS;
    srand(time(NULL));
    epfd=CHECK_NOT_M1(epoll_create1(0));
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i;
        threads[i].sleep_time = (rand() % MAX_SLEEP_SECS) + 1;
        threads[i].tid = 0;
        CHECK_ZERO_ERRNO(pthread_create(&pthreads[i], NULL, worker, &threads[i]));
    }
    
    // Wait for all threads to have stored their TID, then create pidfds
    for (int i = 0; i < NUM_THREADS; i++) {
        // Spin until tid is set (crude but simple)
        while (threads[i].tid == 0)
            usleep(1000);
        
        threads[i].pidfd = CHECK_NOT_M1(syscall(SYS_pidfd_open, threads[i].tid, PIDFD_THREAD));
   
        struct epoll_event ev = {
            .events = EPOLLIN,
            .data = &threads[i]
        };
        
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, threads[i].pidfd, &ev) < 0) {
            perror("epoll_ctl");
            return 1;
        }
    }
    
    printf("[Main] All threads spawned, waiting for exits...\n\n");
    
    // Wait for threads to die
    while (alive > 0) {
        struct epoll_event events[NUM_THREADS];
        int n = CHECK_NOT_M1(epoll_wait(epfd, events, NUM_THREADS, -1));
        for (int i = 0; i < n; i++) {
            struct thread_info *info = (thread_info*)events[i].data.ptr;
            
            printf("[Main] Detected thread %d (tid=%d) died! "
                   "(slept %d secs, %d remaining)\n",
                   info->id, info->tid, info->sleep_time, alive - 1);
            
            // Clean up
            close(info->pidfd);
            pthread_join(pthreads[info->id], NULL);
            alive--;
        }
    }
    
    printf("[Main] All threads finished!\n");
    
    CHECK_NOT_M1(close(epfd));
    return EXIT_SUCCESS;
}
