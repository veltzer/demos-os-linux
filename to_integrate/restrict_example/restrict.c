#include <stdio.h>
#include <stdlib.h>

extern void sum(int*, int, int*);

int main(int argc, char **argv) {
	int a[10];
	for(int i=0;i<10;i++) {
		a[i]=i;
	}
	int t=0;
	sum(a,10,a+5);
	printf("result is %d\n", *(a+5));
	return EXIT_SUCCESS;
}
