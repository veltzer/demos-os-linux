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
#include <syslog.h>	// for openlog(3), syslog(3), closelog(3)
#include <stdio.h>	// for printf(3), fopen(3), fclose(3), fflush(3)
#include <sys/time.h>	// for gettimeofday(2)
#include <pthread.h>	// for pthread_mutex_t, pthread_mutex_lock, pthread_mutex_unlock
#include <stdarg.h>	// for va_list, va_start, va_end
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const, sched_print_table()
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_NULL_FILEP()
#include <measure.h>	// for measure:struct, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * This example explores syslog speed as compared to writing to a simple file.
 *
 * Notes:
 * - every messsage that I'm sending to syslog or to file has a number in it. This
 *	has two reasons: the most important is for syslog to actually log the message
 *	since if syslog gets two identical messages it does not log the second. Instead
 *	it waits for sometime and then says that the previous message repeated so and so
 *	times. The second reason is to include the printf like formatting code in the
 *	measurements.
 * - the tested code runs in a high priority thread to make sure that we measure times
 *	correctly.
 * - the fwrite implementation is fast because it does buffering. Maybe you are ok with
 *	that (you may lose data if you crash) and in that case you can use it.
 *
 * Results:
 * - by default you will find that syslog is much much slower than write.
 *	Output for example from this program:
 *	=====================================
 *	doing 1000 syslogs
 *	time in micro of one syslog: 55.897000
 *	doing 1000 writes
 *	time in micro of one write: 0.330000
 *	=====================================
 *	This is because rsyslogd in ubuntu is synchroneous. Most syslogd implementations are
 *	like that.
 *
 * TODO:
 * - add three more test cases: open(2), write(2), close(2) with
 *	- standard flags.
 *	- O_ASYNC
 *	- O_SYNC
 * - add another test with syslog which writes to a sysfs file instead.
 * - add another test case of asynchroneous syslog (damn it! how do I configure that?!?).
 * - explain the results in the text above.
 * - do better stats (min, max, variance and more - max is the most important).
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

// this emulates an async implementation
// pthread_mutex_t fastmutex=PTHREAD_MUTEX_INITIALIZER;
const unsigned int buffer_size=1024;
char buffer[buffer_size];
inline void my_syslog(const char* fmt, ...) {
	// pthread_mutex_lock(&fastmutex);
	va_list args;
	va_start(args, fmt);
	int ret=vsnprintf(buffer, buffer_size, fmt, args);
	va_end(args);
	memcpy(buffer, fmt, ret);
	// pthread_mutex_unlock(&fastmutex);
}
inline void my_syslog(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

void* func(void*) {
	sched_print_info();
	// number of messages to measure
	const unsigned int number=10000;
	// measurement structure
	measure m;

	openlog("syslog_speed", LOG_PID, LOG_USER);
	measure_init(&m, "standard syslog", number);
	measure_start(&m);
	for(unsigned int i=0; i < number; i++) {
		syslog(LOG_ERR, "this is a message %d", i);
	}
	// end timing...
	measure_end(&m);
	closelog();
	measure_print(&m);
	// let io buffers be flushed...
	CHECK_ZERO(sleep(1));

	FILE* f=CHECK_NOT_NULL_FILEP(fopen("/tmp/syslog_test", "w+"));
	measure_init(&m, "regular file operations (nonbuffreed, flushed, synchroneous)", number);
	measure_start(&m);
	for(unsigned int i=0; i < number; i++) {
		fprintf(f, "this is a message %d", i);
		fflush(f);
	}
	measure_end(&m);
	CHECK_ZERO_ERRNO(fclose(f));
	measure_print(&m);
	// let io buffers be flushed...
	CHECK_ZERO(sleep(1));

	f=CHECK_NOT_NULL_FILEP(fopen("/tmp/syslog_test", "w+"));
	measure_init(&m, "regular file operations (nonbuffreed, flushed, synchroneous)", number);
	measure_start(&m);
	for(unsigned int i=0; i < number; i++) {
		fprintf(f, "this is a message %d", i);
	}
	measure_end(&m);
	CHECK_ZERO_ERRNO(fclose(f));
	measure_print(&m);
	// let io buffers be flushed...
	CHECK_ZERO(sleep(1));

	// now lets measure how long it would take to memcpy...
	measure_init(&m, "fastlog", number);
	measure_start(&m);
	for(unsigned int i=0; i < number; i++) {
		my_syslog("this is a message %d", i);
	}
	measure_end(&m);
	measure_print(&m);
	// let io buffers be flushed...
	CHECK_ZERO(sleep(1));

	return NULL;
}

int main() {
	sched_print_table();
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	return EXIT_SUCCESS;
}
