#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char** argv, char** envp) {
	int fd=open("/dev/ioctl", O_RDWR);
	assert(fd!=-1);
	int ret=ioctl(fd, 0);
	assert(ret!=-1);
	ret=close(fd);
	assert(ret!=-1);
	return 0;
}
