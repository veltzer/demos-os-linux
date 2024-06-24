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

#pragma once

#include <firstinclude.h>
#include <pthread.h>	// for pthread_mutex_init(3), pthread_mutex_destroy(3), pthread_mutex_lock(3), pthread_mutex_unlock(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()

class MT_SYNCH{
};
class MT_NOSYNCH{
};

template<typename T> class MyMutex{
};

template<> class MyMutex<MT_SYNCH> {
private:
	pthread_mutex_t mylock;

public:
	MyMutex() {
		CHECK_ZERO_ERRNO(pthread_mutex_init(&mylock, NULL));
	}
	~MyMutex() {
		CHECK_ZERO_ERRNO(pthread_mutex_destroy(&mylock));
	}
	void lock() {
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&mylock));
	}
	void unlock() {
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&mylock));
	}
};

template<> class MyMutex<MT_NOSYNCH> {
private:

public:
	MyMutex() {
	}
	~MyMutex() {
	}
	void lock() {
	}
	void unlock() {
	}
};
