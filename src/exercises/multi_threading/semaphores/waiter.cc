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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for semctl(2), ftok(3), semget(2)
#include <sys/ipc.h>	// for semctl(2), ftok(3), semget(2)
#include <sys/sem.h>	// for semctl(2), semget(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()
#include <signal_utils.h>	// for signal_register_handler_signal()
#include "common.hh"

static bool over=false;

void clean(int sig __attribute__((unused))) {
	over=true;
}

int main() {
	signal_register_handler_signal(SIGINT, clean);
	key_t key=CHECK_NOT_M1(ftok(FTOK_PATHNAME, FTOK_PROJID));
	int semid=CHECK_NOT_M1(semget(key, NPHIL, IPC_CREAT | 0666));
	for(unsigned int i=0; i<NPHIL; i++) {
		CHECK_NOT_M1(semctl(semid, i, SETVAL, 1));
	}
	// a non busy busy wait
	while(!over) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
	}
	// destroy the semaphore
	CHECK_NOT_M1(semctl(semid, 0, IPC_RMID, 0));
	return EXIT_SUCCESS;
}
