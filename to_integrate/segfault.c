#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv, char** envp) {
	char* p=(char*)malloc(17);
	while(1) {
		printf("trying to access address [%p]\n", p);
		*p=0;
		p++;
	}
	return EXIT_SUCCESS;
}
