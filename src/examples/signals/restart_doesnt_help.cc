#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <assert.h>

/*
 * This is an example of syscall restarting after signal handling
 * This shows that SA_RESTART doesn't help every system call.
 */

static void handler(int sig_num,  siginfo_t *, void *) {
	printf("Inside the signal handler %d in process %d\n", sig_num, getpid());
	printf("signal handler is done...\n");
}

int main(int argc, char** argv, char** envp) {
	// signal registration
	struct sigaction sa;
        sa.sa_flags=SA_RESTART;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction=handler;
        struct sigaction old;
        int ret=sigaction(SIGUSR1, &sa, &old);
	assert(ret!=-1);

	// main code
	printf("going to sleep...%d\n", getpid());
	sleep(3600);
	printf("waking up after the sleep...\n");
}
