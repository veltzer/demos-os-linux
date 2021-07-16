/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for sprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, rand(3), srand(3), malloc(3), free(3)
#include <unistd.h>	// for usleep(2), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <trace_utils.h>	// for TRACE()
#include "mq.h"	// for mq_init(), mq_destroy(), mq_get(), mq_put(), mq struct

typedef struct _thread_data {
	mq m;
	int mynum;
	bool consumer;
} thread_data;

static volatile bool over=false;

void *worker(void *p) {
	thread_data* td=(thread_data *)p;
	mq m=td->m;
	int mynum=td->mynum;
	bool consumer=td->consumer;
	if(consumer) {
		char* msg=mq_get(m);
		while(msg!=NULL) {
			TRACE("consumer %d got message %s, working on it...", mynum, msg);
			usleep(rand()%1000);
			free(msg);
			msg=mq_get(m);
		}
	} else {
		int counter=0;
		while(!over) {
			// generate a fake message
			char* msg=(char*)malloc(256);
			sprintf(msg, "message %d from producer %d", counter, mynum);
			mq_put(m, msg);
			TRACE("producer %d sent message %s...", mynum, msg);
			counter++;
			usleep(rand()%1000);
		}
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const unsigned int producers=1;
	const unsigned int consumers=1;
	const unsigned int thread_num=producers+consumers;
	unsigned int i;
	pthread_t* threads=(pthread_t*)malloc(thread_num*sizeof(pthread_t));
	thread_data* tds=(thread_data*)malloc(thread_num*sizeof(thread_data));

	srand(getpid());
	mq m;
	mq_init(&m, 10);
	for(i=0; i<thread_num; i++) {
		tds[i].m=m;
		tds[i].mynum=i<consumers ? i: i-consumers;
		tds[i].consumer=i<consumers;
		CHECK_ZERO_ERRNO(pthread_create(threads+i, NULL, worker, tds+i));
	}
	for(i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}

	mq_destroy(m);
	free(threads);
	free(tds);
	return EXIT_SUCCESS;
}
