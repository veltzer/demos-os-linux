#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <cpuid.h>

int main() {
    uint64_t random_number;

    // Check if RDRAND is supported
    if (!__get_cpuid_count(0x07, 0, NULL, NULL, NULL, NULL) & (1 << 30)) {
        printf("RDRAND instruction is not supported on this CPU.\n");
        return 1;
    }

    // Generate a random 64-bit number
    if (_rdrand64_step(&random_number) == 0) {
        printf("Failed to generate a random number.\n");
        return 1;
    }

    printf("Random number: %llu\n", random_number);

    return 0;
}
