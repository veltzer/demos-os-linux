#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int array[6]={5,3,8,2,9,1};

int main() {
	int arr[6];
	int vals=scanf("%d %d %d %d %d %d", arr, arr+1, arr+2, arr+3, arr+4, arr+5);
	assert(vals==6);
	int prev=-1000;
	for(int i=0;i<6;i++) {
		int val=array[arr[i]];
		if(val<=prev) {
			fprintf(stderr, "Boom!\n");
			exit(1);
		}
		prev=val;
	}
	return 0;
}
