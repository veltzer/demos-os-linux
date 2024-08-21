#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	char* p=(char*)malloc(1024*1024*1024);
	int i;
	while(1) {
		printf("touching 20 pages at address [%p]\n", p);
		for(i=0; i<20; i++) {
			*p=0;
			p+=4096;
		}
		sleep(1);
	}
	return EXIT_SUCCESS;
}
