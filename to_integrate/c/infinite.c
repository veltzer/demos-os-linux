#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	while(1) {
		printf("Im still here...\n");
		sleep(1);
	}
	return EXIT_SUCCESS;
}
