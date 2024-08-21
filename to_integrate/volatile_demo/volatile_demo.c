#include <stdio.h>
#include <stdlib.h>
#include "callme.h"
#include <unistd.h>

int main() {
	long long int volatile sum=0;
	long long tmp=0;
	for(long int i=0;i<1000000000;i++) {
		tmp+=i;
	}
	sum=tmp;
	printf("sum is %lld\n", sum);
	return EXIT_SUCCESS;
}
