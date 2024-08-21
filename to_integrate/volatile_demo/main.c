#include <stdio.h>
#include <stdlib.h>
#include "callme.h"
#include <unistd.h>

int main() {
	for(int i=0;i<100;i++) {
		callme();
		sleep(1);
		printf("i is %d, g is %d\n", i, get_g());
	}
	return EXIT_SUCCESS;
}
