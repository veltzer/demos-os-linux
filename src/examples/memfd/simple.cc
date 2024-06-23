#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

int main() {
    const char *name = "example_memfd";
    const char *data = "Hello, memfd!";
    char buf[20];
    int fd, ret;

    // Create a memfd
    fd = memfd_create(name, 0);
    if (fd == -1) {
        perror("memfd_create");
        exit(1);
    }

    // Write data to the memfd
    ret = write(fd, data, strlen(data));
    if (ret == -1) {
        perror("write");
        close(fd);
        exit(1);
    }

    // Seek to the beginning of the file
    lseek(fd, 0, SEEK_SET);

    // Read data from the memfd
    ret = read(fd, buf, sizeof(buf) - 1);
    if (ret == -1) {
        perror("read");
        close(fd);
        exit(1);
    }
    buf[ret] = '\0';

    printf("Read from memfd: %s\n", buf);

    close(fd);
    return 0;
}
