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
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char** argv, char** envp) {
	int shm_fd = shm_open("myData", O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1)
	{
		printf("error in creating shm_fd\n");
	}

	if (ftruncate(shm_fd, sizeof(int)) == -1) {
		printf("ftruncate error\n");
	}

	int* pData =(int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (pData == MAP_FAILED) {
		printf("MAP_FAILED\n");
	}


	shm_fd = shm_open("myMutex", O_CREAT | O_RDWR, 0777);
	if (shm_fd == -1)
	{
		printf("error in creating shm_fd\n");
	}
	if (ftruncate(shm_fd, sizeof(pthread_mutex_t)) == -1)
		printf("ftruncate error\n");

	pthread_mutex_t* pLock = (pthread_mutex_t*)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (pLock == MAP_FAILED)
		printf("MAP_FAILED\n");

	//init mutex
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
	pthread_mutex_init(pLock, &attr);
	pthread_mutexattr_destroy(&attr);

	int counter = 0;

	while(true) {
		pthread_mutex_lock(pLock);
		printf("writer lock\n");
		sleep(10);
		counter++;
		memcpy(pData, &counter, sizeof(int));
		printf("writer unlock %d\n", counter);
		pthread_mutex_unlock(pLock);
		sleep(1);
	}
	return EXIT_SUCCESS;
}
