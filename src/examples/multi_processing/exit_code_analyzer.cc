#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void analyze_status(int status) {
    printf("Status value: %d (0x%x)\n\n", status, status);
    
    if (WIFEXITED(status)) {
        printf("WIFEXITED:   true\n");
        printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
    } else {
        printf("WIFEXITED:   false\n");
    }
    
    if (WIFSIGNALED(status)) {
        printf("WIFSIGNALED: true\n");
        printf("WTERMSIG:    %d\n", WTERMSIG(status));
        #ifdef WCOREDUMP
        printf("WCOREDUMP:   %s\n", WCOREDUMP(status) ? "true" : "false");
        #endif
    } else {
        printf("WIFSIGNALED: false\n");
    }
    
    if (WIFSTOPPED(status)) {
        printf("WIFSTOPPED:  true\n");
        printf("WSTOPSIG:    %d\n", WSTOPSIG(status));
    } else {
        printf("WIFSTOPPED:  false\n");
    }
    
    #ifdef WIFCONTINUED
    printf("WIFCONTINUED: %s\n", WIFCONTINUED(status) ? "true" : "false");
    #endif
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <status_value>\n", argv[0]);
        fprintf(stderr, "Analyzes the status value returned by wait() using standard macros\n");
        return 1;
    }
    
    int status;
    char *endptr;
    
    // Allow both decimal and hexadecimal input
    if (argv[1][0] == '0' && (argv[1][1] == 'x' || argv[1][1] == 'X')) {
        status = (int)strtol(argv[1], &endptr, 16);
    } else {
        status = (int)strtol(argv[1], &endptr, 10);
    }
    
    if (*endptr != '\0') {
        fprintf(stderr, "Error: Invalid status value\n");
        return 1;
    }
    
    analyze_status(status);
    return 0;
}