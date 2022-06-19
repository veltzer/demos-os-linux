#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	if(fork()) {
		// this is the parent
		printf("I'm the parent and I'm dying...%d\n", getpid());
		exit(0);
	} else {
		// this is the child
		while(1) {
			printf("hello again, I'm the child...%d\n", getpid());
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}
