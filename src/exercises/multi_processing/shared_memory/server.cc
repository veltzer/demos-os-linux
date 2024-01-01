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
#include <sys/types.h>	// for ftok(3), semget(2), semctl(2), shmat(2)
#include <sys/ipc.h>	// for ftok(3), semget(2), semctl(2), shmget(2)
#include <sys/shm.h>	// for shmget(2), shmat(2)
#include <sys/sem.h>	// for semget(2), semctl(2)
#include <stdio.h>	// for printf(3), fgets(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_CHARP(), CHECK_NOT_M1(), CHECK_NOT_VOIDP()

const int MAXCLIENTS=10;
const int CLIENTMESSAGESIZE=4096;

struct data {
	int readOffset;
	int writeOffset;
	char message[CLIENTMESSAGESIZE];
};

int main(int argc, char** argv, char** envp) {
	key_t key=CHECK_NOT_M1(ftok("/etc/passwd", 'x'));
	int semid=CHECK_NOT_M1(semget(key, MAXCLIENTS, IPC_CREAT | 0666));
	for(int i=0; i<MAXCLIENTS; i++) {
		CHECK_NOT_M1(semctl(semid, i, SETVAL, 0));
	}
	printf("asking for %zd bytes\n", sizeof(struct data) * MAXCLIENTS);
	int shmid=CHECK_NOT_M1(shmget(key, sizeof(struct data) * MAXCLIENTS, IPC_CREAT | 0666));
	struct data* smdata=(struct data*)CHECK_NOT_VOIDP(shmat(shmid, NULL, 0), (void*)-1);
	for(int i=0; i<MAXCLIENTS; i++) {
		smdata[i].readOffset=0;
		smdata[i].writeOffset=0;
	}
	printf("Hit <Enter> to finish\n");
	char ans[10];
	CHECK_CHARP(fgets(ans, sizeof(ans), stdin), ans);
	CHECK_NOT_M1(shmctl(shmid, IPC_RMID, 0));
	CHECK_NOT_M1(semctl(semid, 0, IPC_RMID, 0));
	return EXIT_SUCCESS;
}
