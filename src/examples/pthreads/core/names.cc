/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <pthread.h>	// for pthread_create(3), pthread_join(3), pthread_t:struct, pthread_setname_np(3), pthread_getname_np(3)
	// pthread_mutex_init(3), pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_destroy(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <string.h>	// for strncpy(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <multiproc_utils.h>	// for my_system(), gettid()
#include <pthread_utils.h>	// for set_thread_name(), get_thread_name()
#include <proc_utils.h>	// for print_thread_name_proc()

/*
 * This exapmle shows how to set thread names in Linux.
 * There are 3 ways to do that:
 * - prctl(2) with PR_SET_NAME/PR_GET_NAME.
 * If you look at the documentation of prctl(2), it states that it only sets PROCESS
 * names, but this is actually wrong since it sets the name of the current schedualable
 * entity which may be a thread or a process. The reason that it says what is says is
 * because in Linux threads are really processes.
 * - pthread_setname_np/pthread_getname_np.
 * This is the new non-portable/non-posix API for setting and getting thread names.
 * - Reading and writing from /proc/self/task/[tid]/comm.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

const int name_length=16;

typedef enum _method {
	PRCTL,
	PTHREAD_SET_NAME,
	PROC,
} method;

typedef struct _todo_item {
	method m;
	char name[name_length];
} todo_item;

todo_item todo[]={
	{ PRCTL, "hello" },
	{ PTHREAD_SET_NAME, "hello" },
	{ PROC, "hello" },
	{ PRCTL, "תהליכון" },
	{ PTHREAD_SET_NAME, "תהליכון" },
	{ PROC, "תהליכון" },
};

typedef struct _thread_data {
	pthread_mutex_t start_mutex;
	pthread_mutex_t end_mutex;
	char name[name_length];
	method m;
} thread_data;

void* worker(void* arg) {
	thread_data* td=(thread_data*)arg;

	char name[name_length];

	get_thread_name(name, name_length);
	TRACE("get_thread_name is [%s]", name);
	get_thread_name_proc(name, name_length);
	TRACE("get_thread_name_proc is [%s]", name);
	CHECK_ZERO_ERRNO(pthread_getname_np(pthread_self(), name, name_length));
	TRACE("pthread_getname_np is [%s]...\n", name);

	switch(td->m) {
		case PRCTL:
			set_thread_name(td->name);
			break;
		case PTHREAD_SET_NAME:
			CHECK_ZERO_ERRNO(pthread_setname_np(pthread_self(), td->name));
			break;
		case PROC:
			set_thread_name_proc(td->name);
			break;
	}

	get_thread_name(name, name_length);
	TRACE("get_thread_name is [%s]", name);
	get_thread_name_proc(name, name_length);
	TRACE("get_thread_name_proc is [%s]", name);
	CHECK_ZERO_ERRNO(pthread_getname_np(pthread_self(), name, name_length));
	TRACE("pthread_getname_np is [%s]...\n", name);

	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&(td->start_mutex)));
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&(td->end_mutex)));
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	for(unsigned int i=0;i<ARRAY_SIZEOF(todo);i++) {
		pthread_t t;
		thread_data td;
		strncpy(td.name, todo[i].name, name_length-1);
		td.m=todo[i].m;
		CHECK_ZERO_ERRNO(pthread_mutex_init(&td.start_mutex, NULL));
		CHECK_ZERO_ERRNO(pthread_mutex_init(&td.end_mutex, NULL));
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&td.start_mutex));
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&td.end_mutex));
		CHECK_ZERO_ERRNO(pthread_create(&t, NULL, worker, &td));

		// wait for the threads to be initialized, if we got the lock then they are...
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&td.start_mutex));

		// now that both threads have set their name, show the threads with their names...
		my_system("ps -p %d -L", getpid());
		// let the threads die (if we do not unlock they will wait forever...)
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&td.end_mutex));
		// join the theads so that everything will be clean...
		CHECK_ZERO_ERRNO(pthread_join(t, NULL));
		// destroy all the mutexes
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&td.start_mutex));
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&td.end_mutex));
		CHECK_ZERO_ERRNO(pthread_mutex_destroy(&td.start_mutex));
		CHECK_ZERO_ERRNO(pthread_mutex_destroy(&td.end_mutex));
	}
	return EXIT_SUCCESS;
}
