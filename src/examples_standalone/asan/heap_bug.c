#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t pick_size(int n) {
    return (size_t)n;
}

int main(int argc, char **argv) {
    (void)argv;
    char *buf = malloc(pick_size(8));
    const char *src = "AAAAAAAAAAAAAAAA";
    strcpy(buf, src + (argc - 1));
    printf("%s\n", buf);
    free(buf);
    return 0;
}
