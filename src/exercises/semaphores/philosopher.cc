/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3), EXIT_FAILURE, rand(3), srand(3), atoi()
#include <time.h>	// for time(2)
#include <unistd.h>	// for sleep(3)
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for semop(2), ftok(3), semget(2)
#include <sys/ipc.h>	// for semop(2), ftok(3), semget(2)
#include <sys/sem.h>	// for semop(2), semget(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO()
#include "common.hh"

void activity(int semid, int id, const char* activity) {
	int stime;
	printf("Philosopher %d is busy %s\n", id, activity);
	stime=MIN_SLEEP+rand()%(MAX_SLEEP-MIN_SLEEP);
	CHECK_ZERO(sleep(stime));
	printf("Philosopher %d finished %s\n", id, activity);
}

void forks(int semid, int id, int amount) {
	int next;
	if(id==NPHIL-1)
		next=0;
	else
		next=id+1;
	printf("Philosopher %d is starting handling forks with amount %d\n", id, amount);
	struct sembuf sops[2];
	sops[0].sem_num=id;
	sops[0].sem_op=amount;
	sops[0].sem_flg=0;
	sops[1].sem_num=next;
	sops[1].sem_op=amount;
	sops[1].sem_flg=0;
	CHECK_NOT_M1(semop(semid, sops, 2));
	printf("Philosopher %d is ending handling forks with amount %d\n", id, amount);
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [0-%d)\n", argv[0], argv[0], NPHIL);
		exit(EXIT_FAILURE);
	}
	// parameters
	int id=atoi(argv[1]);
	if(id<(int)0 || id>=(int)NPHIL) {
		fprintf(stderr, "%s: bad argument %s, argument must be numeric value between 0 and %d\n", argv[0], argv[1], NPHIL);
		exit(EXIT_FAILURE);
	}
	// so things will not be boring..
	srand(time(NULL));
	// create the key
	key_t key=CHECK_NOT_M1(ftok(FTOK_PATHNAME, FTOK_PROJID));

	int semid=CHECK_NOT_M1(semget(key, 0, 0));
	while(true) {
		forks(semid, id, -1);
		activity(semid, id, "eating");
		forks(semid, id, 1);
		activity(semid, id, "sleeping");
	}
	return EXIT_SUCCESS;
}
