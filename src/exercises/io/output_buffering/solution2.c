#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

int main() {
	int fd=open("data.dat", O_WRONLY | O_CREAT, 0666);
	assert(fd!=-1);
	char buf[100]="this is a buffer";
	int res=write(fd, buf, sizeof(buf));
	assert(res==100);
}
