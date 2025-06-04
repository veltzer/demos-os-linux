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
#include <sys/types.h>	// for ftok(3), shmat(2)
#include <sys/ipc.h>	// for ftok(3), shmget(2)
#include <sys/shm.h>	// for shmget(2), shmat(2)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_CHARP(), CHECK_NOT_M1(), CHECK_NOT_VOIDP()

const int PROJID=50;
#define PROJID 50

int main(int argc, char**) {
	key_t key=CHECK_NOT_M1(ftok("/etc/passwd", PROJID));
	if(argc==1) {
		printf("running server...\n");
		int shmid=CHECK_NOT_M1(shmget(key, getpagesize(), IPC_CREAT | 0666));
		int* p_int=(int*)CHECK_NOT_VOIDP(shmat(shmid, NULL, 0), (void*)-1);
		printf("the addres of the shared memory is at %p\n", (void*)p_int);
		for(int i=0;i<100;i++) {
			*p_int=i;
			printf("server wrote %d\n", i); 
			sleep(1);
		}
		CHECK_NOT_M1(shmctl(shmid, IPC_RMID, 0));
	} else {
		printf("running client...\n");
		int shmid=CHECK_NOT_M1(shmget(key, getpagesize(), 0));
		int* p_int=(int*)CHECK_NOT_VOIDP(shmat(shmid, NULL, 0), (void*)-1);
		printf("the addres of the shared memory is at %p\n", (void*)p_int);
		for(int i=0;i<100;i++) {
			int data=*p_int;
			printf("client got number %d\n", data); 
			sleep(1);
		}
		CHECK_NOT_M1(shmctl(shmid, IPC_RMID, 0));
	}
	return EXIT_SUCCESS;
}
