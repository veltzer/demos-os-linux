#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

int main() {
	FILE* f=fopen("data.dat", "w");
	assert(f!=NULL);
	char buf[100]="this is a buffer";
	int number_of_items=fwrite(buf, sizeof(buf), 1, f);
	assert(number_of_items==1);
	fflush(f);
	int y=5/0;
	int ret=fclose(f);
	assert(ret==0);
}
