#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <unistd.h>	// for sleep(3), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <sched.h>	// for sched_getcpu(2)
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>

/*
 * This is a standard pthread demo.
 * This shows the various attributes of each of the threads:
 * pthread_self, tid, tid cached, pid, core it is running
 * on.
 *
 * EXTRA_COMPILE_FLAGS=-g
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

int fd;

static void* closer(void* p) {
	/* This is the thread that closes the file descritor */
	sleep(10);
	int ret=close(3);
	assert(ret!=-1);
	printf("\n\n\n\n\nclose was ok!\n\n\n\n");
	sleep(3600);
	return NULL;
}

static void* reader(void* p) {
	char buf[10];
	while(1) {
		int count=read(3, buf, 10);
		assert(count!=-1);
		write(1, "got data!\n", 10);
		write(1, buf, count);
	}
	return NULL;
}

int main() {
	const char* file="/dev/input/mouse5";
	fd=open(file, O_RDWR);
	pthread_t threads[2];
	pthread_create(threads, NULL, closer, NULL);
	pthread_create(threads+1, NULL, reader, NULL);
	pthread_join(*threads, NULL);
	pthread_join(threads[1], NULL);
	return EXIT_SUCCESS;
}
