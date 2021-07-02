#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
	int fd=open("/proc/uptime", O_RDONLY);
	assert(fd!=-1);
	while(1) {
		size_t bufsize=256;
		char buf[bufsize];
		ssize_t count_read=read(fd, buf, bufsize);
		printf("count_read is %ld\n", count_read);
		assert(count_read>0);
		printf("we got [%s]\n", buf);
		int ret=lseek(fd, SEEK_SET, 0);
		assert(ret!=-1);
	}
}
