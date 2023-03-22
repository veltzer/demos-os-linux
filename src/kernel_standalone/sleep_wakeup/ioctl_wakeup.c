#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>

#include "ioctl.h"

int main(int argc, char** argv, char** envp) {
	int fd=open("/dev/ioctl", O_RDWR);
	assert(fd!=-1);
	int ret=ioctl(fd, IOCTL_WAKEUP);
	assert(ret!=-1);
	ret=close(fd);
	assert(ret!=-1);
	return 0;
}
