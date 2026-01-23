/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <err_utils.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct _pthread_mq_t {
	int size;
	int* data;
	int min;
	int max;
	pthread_mutex_t m;
	pthread_cond_t cond_empty;
	pthread_cond_t cond_full;
} pthread_mq_t;

int pthread_mq_init(pthread_mq_t* p, int size) {
	p->size=size;
	p->data=(int*)CHECK_NOT_NULL(malloc(sizeof(int)*size));
	p->min=0;
	p->max=0;
	CHECK_ZERO_ERRNO(pthread_mutex_init(&(p->m), NULL));
	CHECK_ZERO_ERRNO(pthread_cond_init(&(p->cond_empty), NULL));
	CHECK_ZERO_ERRNO(pthread_cond_init(&(p->cond_full), NULL));
	return 0;
}

int pthread_mq_get(pthread_mq_t* mq, int* value) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&(mq->m)));
	// while the queue is empty go to sleep
	while(mq->max==mq->min) {
		CHECK_ZERO_ERRNO(pthread_cond_wait(&(mq->cond_empty), &(mq->m)));
	}
	*value=mq->data[mq->min];
	// if we turned the queue from full -> non full, wake up whoever is waiting
	if((mq->max+1)%mq->size==mq->min) {
		CHECK_ZERO_ERRNO(pthread_cond_broadcast(&(mq->cond_empty)));
	}
	mq->min++;
	mq->min%=mq->size;
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&(mq->m)));
	return 0;
}

int pthread_mq_put(pthread_mq_t* mq, int value) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&(mq->m)));
	// while the queue is full go to sleep
	while((mq->max+1)%mq->size==mq->min) {
		CHECK_ZERO_ERRNO(pthread_cond_wait(&(mq->cond_full), &(mq->m)));
	}
	mq->data[mq->max]=value;
	// if we turned the queue from empty -> non empty, wake up whoever is waiting
	if(mq->max==mq->min) {
		CHECK_ZERO_ERRNO(pthread_cond_broadcast(&(mq->cond_empty)));
	}
	mq->max++;
	mq->max%=mq->size;
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&(mq->m)));
	return 0;
}

int pthread_mq_destroy(pthread_mq_t* p) {
	free(p->data);
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&(p->m)));
	CHECK_ZERO_ERRNO(pthread_cond_destroy(&(p->cond_empty)));
	CHECK_ZERO_ERRNO(pthread_cond_destroy(&(p->cond_full)));
	return 0;
}

void* func_producer(void* arg) {
	pthread_mq_t* mq=(pthread_mq_t*)arg;
	while(1) {
		int value;
		printf("give me a number: ");
		fflush(stdout);
		int res=scanf("%d", &value);
		assert(res==1);
		CHECK_ZERO_ERRNO(pthread_mq_put(mq, value));
		if(value==666) {
			break;
		}
	}
	return NULL;
}

void* func_consumer(void* arg) {
	pthread_mq_t* mq=(pthread_mq_t*)arg;
	while(1) {
		int value;
		CHECK_ZERO_ERRNO(pthread_mq_get(mq, &value));
		printf("consumer: got value %d\n", value);
		if(value==666) {
			break;
		}
	}
	return NULL;
}

int main() {
	pthread_t consumer, producer;
	pthread_mq_t mq;
	CHECK_ZERO_ERRNO(pthread_mq_init(&mq, 10));
	CHECK_ZERO_ERRNO(pthread_create(&producer, NULL, func_producer, &mq));
	CHECK_ZERO_ERRNO(pthread_create(&consumer, NULL, func_consumer, &mq));
	CHECK_ZERO_ERRNO(pthread_join(producer, NULL));
	CHECK_ZERO_ERRNO(pthread_join(consumer, NULL));
	CHECK_ZERO_ERRNO(pthread_mq_destroy(&mq));
	return EXIT_SUCCESS;
}
