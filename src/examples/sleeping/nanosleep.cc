#include <stdio.h>
#include <time.h>
#include <errno.h>

int main() {
    // Define a timespec structure for sleeping
    struct timespec ts;
    
    // Set sleep time: 2 seconds and 500 million nanoseconds (2.5 seconds total)
    ts.tv_sec = 600;   // 10 minutes
    ts.tv_nsec = 0;  // Nanoseconds (500 million = 0.5 seconds)

    printf("About to sleep for 10 minutes...\n");

    // Perform the nanosleep
    int result = nanosleep(&ts, NULL);

    // Check if sleep was successful
    if (result == 0) {
        printf("Slept successfully!\n");
    } else {
        // If sleep failed, print error
        perror("nanosleep failed");
        return errno;
    }

    return 0;
}
