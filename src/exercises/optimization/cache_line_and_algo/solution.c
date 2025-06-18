#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

// 100MB / sizeof(int) = ~26.2M integers
#define ARRAY_SIZE (100 * 1024 * 1024 / sizeof(int))

// Function to get high-resolution time
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Initialize array with random data
void init_array(int *arr, size_t size) {
    srand(42); // Fixed seed for reproducible results
    for (size_t i = 0; i < size; i++) {
        arr[i] = rand() % 1000;
    }
}

// Method 1: Process entire array twice (cache-unfriendly)
void process_cache_unfriendly(int *arr, size_t size) {
    // First pass: add 1 to all elements
    for (size_t i = 0; i < size; i++) {
        arr[i] += 1;
    }
    
    // Second pass: multiply by 2 if previous element is odd
    for (size_t i = 1; i < size; i++) {
        if (arr[i-1] % 2 == 1) {
            arr[i] *= 2;
        }
    }
}

// Method 2: Process in chunks to be cache-friendly
void process_cache_friendly(int *arr, size_t size) {
    // Process in chunks that fit in cache
    // Typical L3 cache is 8-32MB, we'll use 1MB chunks to be safe
    const size_t chunk_size = 64 * 1024 / sizeof(int); // ~64K integers per chunk
    
    for (size_t start = 0; start < size; start += chunk_size) {
        size_t end = start + chunk_size;
        if (end > size) end = size;
        
        // First operation on this chunk: add 1 to all elements
        for (size_t i = start; i < end; i++) {
            arr[i] += 1;
        }
        
        // Second operation on same chunk: multiply by 2 if previous element is odd
        size_t loop_start = (start == 0) ? 1 : start;
        for (size_t i = loop_start; i < end; i++) {
            if (arr[i-1] % 2 == 1) {
                arr[i] *= 2;
            }
        }
    }
}

// Function to verify results are identical
int verify_results(int *arr1, int *arr2, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            printf("Mismatch at index %zu: %d vs %d\n", i, arr1[i], arr2[i]);
            return 0;
        }
    }
    return 1;
}

int main() {
    printf("Cache Performance Comparison\n");
    printf("Array size: %zu integers (%.2f MB)\n\n", 
           ARRAY_SIZE, ARRAY_SIZE * sizeof(int) / (1024.0 * 1024.0));
    
    // Allocate arrays
    int *arr1 = malloc(ARRAY_SIZE * sizeof(int));
    int *arr2 = malloc(ARRAY_SIZE * sizeof(int));
    
    if (!arr1 || !arr2) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }
    
    // Initialize both arrays with identical data
    printf("Initializing arrays with random data...\n");
    init_array(arr1, ARRAY_SIZE);
    memcpy(arr2, arr1, ARRAY_SIZE * sizeof(int));
    
    printf("Processing arrays...\n\n");
    
    // Method 1: Cache-unfriendly approach
    printf("Method 1: Cache-unfriendly (two separate passes)\n");
    double start_time = get_time();
    process_cache_unfriendly(arr1, ARRAY_SIZE);
    double unfriendly_time = get_time() - start_time;
    printf("Time taken: %.4f seconds\n\n", unfriendly_time);
    
    // Method 2: Cache-friendly approach
    printf("Method 2: Cache-friendly (chunked processing)\n");
    start_time = get_time();
    process_cache_friendly(arr2, ARRAY_SIZE);
    double friendly_time = get_time() - start_time;
    printf("Time taken: %.4f seconds\n\n", friendly_time);
    
    // Verify results are identical
    printf("Verifying results are identical... ");
    if (verify_results(arr1, arr2, ARRAY_SIZE)) {
        printf("✓ Results match\n\n");
    } else {
        printf("✗ Results don't match!\n\n");
    }
    
    // Performance analysis
    printf("Performance Analysis:\n");
    printf("Cache-unfriendly time: %.4f seconds\n", unfriendly_time);
    printf("Cache-friendly time:   %.4f seconds\n", friendly_time);
    
    if (friendly_time < unfriendly_time) {
        double speedup = unfriendly_time / friendly_time;
        printf("Speedup: %.2fx faster with cache-friendly approach\n", speedup);
        printf("Time saved: %.4f seconds (%.1f%% reduction)\n", 
               unfriendly_time - friendly_time,
               (unfriendly_time - friendly_time) / unfriendly_time * 100);
    } else {
        printf("Cache-friendly approach was %.2fx slower\n", friendly_time / unfriendly_time);
    }
    
    printf("\nExplanation:\n");
    printf("- Cache-unfriendly: Processes entire array twice, causing cache misses\n");
    printf("- Cache-friendly: Processes data in chunks that fit in cache\n");
    printf("- Performance difference depends on your CPU's cache hierarchy\n");
    
    // Memory usage info
    printf("\nMemory Info:\n");
    printf("Total memory allocated: %.2f MB\n", 
           2 * ARRAY_SIZE * sizeof(int) / (1024.0 * 1024.0));
    printf("Chunk size used: %zu integers (%.2f KB)\n",
           256 * 1024 / sizeof(int), 256.0);
    
    free(arr1);
    free(arr2);
    
    return 0;
}
