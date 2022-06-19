#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	if(!fork()) {
		// this is the child
		printf("I'm the child and I'm dying...%d\n", getpid());
		exit(7);
	} else {
		// this is the parent
		while(1) {
			printf("hello again, I'm the parent...%d\n", getpid());
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}
