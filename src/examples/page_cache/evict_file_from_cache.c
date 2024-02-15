#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>


int main() {
	const char* filename = "/etc/passwd";
	// evict the file from the cache
	int fd = open(filename, O_RDONLY); 
	posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED); 
	close(fd);
	// check that the evict worked...
	//fd = open(filename, O_RDONLY); 
	//mmap();
	//mincore();
	return EXIT_SUCCESS;
}
