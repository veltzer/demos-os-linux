#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** envp) {
	printf("I'm here...\n");
	*(char*)0=0;
	printf("Where did this go?...\n");
	return EXIT_SUCCESS;
}
