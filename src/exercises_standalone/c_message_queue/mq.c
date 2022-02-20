/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <err_utils.h> // for CHECK_ZERO_ERRNO
#include <stdlib.h> // for malloc(3)
#include "mq.h"	// for mq_init(), mq_destroy(), mq_put(), mq_get(), struct mq
#include <pthread.h>	// for pthread_mutex_t(3), pthread_cond_t(3)

/*
 * This is the solution to the mq exercise.
 */

typedef struct _mq {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	void** messages;
	int maxsize;
	int size;
} *mq;

int mq_init(mq* mqp, const int maxsize) {
	mq m=(mq)malloc(sizeof(struct _mq));
	CHECK_ZERO_ERRNO(pthread_mutex_init(&m->mutex, NULL));
	CHECK_ZERO_ERRNO(pthread_cond_init(&m->cond, NULL));
	m->messages=(void**)malloc(maxsize*sizeof(void*));
	m->maxsize=maxsize;
	m->size=0;
	*mqp=m;
	return 0;
}

int mq_destroy(mq m) {
	CHECK_ZERO_ERRNO(pthread_cond_destroy(&m->cond));
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&m->mutex));
	free(m->messages);
	free(m);
	return 0;
}

int mq_put(mq m, void* message) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&m->mutex));
	while(m->size==m->maxsize) {
		CHECK_ZERO_ERRNO(pthread_cond_wait(&m->cond, &m->mutex));
	}
	// here we have the mutex, and we have room in the mq
	m->messages[m->size]=message;
	m->size++;
	CHECK_ZERO_ERRNO(pthread_cond_broadcast(&m->cond));
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&m->mutex));
	return 0;
}

void* mq_get(mq m) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&m->mutex));
	while(m->size==0) {
		CHECK_ZERO_ERRNO(pthread_cond_wait(&m->cond, &m->mutex));
	}
	// here we have the mutex, and we data in the mq
	m->size--;
	void* ret=m->messages[m->size];
	CHECK_ZERO_ERRNO(pthread_cond_broadcast(&m->cond));
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&m->mutex));
	return ret;
}
