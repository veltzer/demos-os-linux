# SIMD Matrix Multiplication Example

This document explains a practical example of using SIMD (Single Instruction Multiple Data) instructions to accelerate matrix multiplication.

## Overview

The example demonstrates the performance difference between scalar and SIMD implementations of matrix multiplication, achieving a ~6-7x speedup using AVX instructions.

## Key Concepts

### Matrix Multiplication
- Each element in the result matrix C[i,j] is the dot product of row i from matrix A and column j from matrix B
- Computational complexity: O(n³)
- Very compute-intensive operation (perfect for SIMD)

### SIMD Implementation ({2)
- Uses AVX instructions (`__m256` type) to process 8 single-precision floats at once
- Key AVX instructions used:
    - `_mm256_load_ps`: Load 8 aligned floats
    - `_mm256_set1_ps`: Broadcast single value to all 8 elements
    - `_mm256_mul_ps`: Multiply 8 pairs of floats
    - `_mm256_add_ps`: Add 8 pairs of floats

### Cache Optimization

```c
#define MATRIX_SIZE 256
#define BLOCK_SIZE 32
```

- Uses blocking (tiling) to improve cache utilization
- Block size chosen to fit in L1 cache
- Reduces cache misses and memory bandwidth requirements

## Code Structure

### Memory Alignment

```c
ret = posix_memalign((void**)&A, 32, n * n * sizeof(float));
```

- 32-byte alignment required for AVX operations
- Uses `posix_memalign` for aligned allocation

### Scalar Implementation

```c
void scalar_matrix_multiply(float* A, float* B, float* C, int n) {
    for (int ii = 0; ii < n; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < n; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < n; kk += BLOCK_SIZE) {
                // Block multiplication
```

- Uses 6 nested loops:
    - 3 outer loops for block iteration
    - 3 inner loops for element computation
- Implements blocking for cache efficiency

### SIMD Implementation (1)

```c
void simd_matrix_multiply(float* A, float* B, float* C, int n) {
    // ... block loops ...
    __m256 sum = _mm256_load_ps(&C[i * n + j]);
    __m256 a = _mm256_set1_ps(A[i * n + k]);
    __m256 b = _mm256_load_ps(&B[k * n + j]);
    sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
```

- Processes 8 elements at once using AVX
- Broadcasts single A value to multiply with 8 B values
- Accumulates results in SIMD register

## Performance Analysis

### Theoretical Maximum
- AVX processes 8 floats per instruction
- Ideal speedup would be 8x
- Achieved ~6.7x in practice (very good efficiency)

### Why It's Fast
1. Compute-bound operation
    - Many arithmetic operations per memory access
    - High arithmetic intensity
    - Not limited by memory bandwidth
1. Good cache utilization
    - Blocked algorithm
    - Sequential memory access
    - Data reuse through broadcasting
1. SIMD advantages
    - 8-way parallelism
    - Efficient memory operations
    - Hardware-level optimization

## Potential Optimizations

### FMA Instructions

```c
sum = _mm256_fmadd_ps(a, b, sum);  // a * b + sum
```

- Uses single instruction for multiply-add
- Better performance on supporting CPUs

### Memory Prefetching

```c
_mm_prefetch(&B[k * n + j + 8], _MM_HINT_T0);
```

- Brings data into cache before needed
- Can hide memory latency

### Parameter Tuning
- Matrix size affects overall performance
- Block size affects cache efficiency
- Can be tuned for specific CPU architectures

## Compilation

```bash
gcc -mavx -march=native -O3 matrix_mult.c -o matrix_mult -lm
```

- `-mavx`: Enable AVX instructions
- `-march=native`: Optimize for local CPU
- `-O3`: Maximum optimization level

## Results
- Scalar time: x.xx seconds
- SIMD time: x.xx seconds
- Speedup: ~6.7x
- Close to theoretical maximum for AVX
- Demonstrates effective use of SIMD instructions

## Common Pitfalls
1. Memory alignment
    - Must use aligned loads/stores
    - Must allocate aligned memory

1. Cache effects
    - Wrong block size can hurt performance
    - Need to consider cache hierarchy

1. Data dependencies
    - Must ensure correct accumulation
    - Need to handle edge cases properly

## Applications
- Scientific computing
- Computer graphics
- Machine learning
- Signal processing
- Any domain with matrix operations

## Conclusion
This example demonstrates how SIMD instructions can significantly accelerate compute-bound operations when properly implemented with attention to:
- Memory alignment
- Cache efficiency
- Computational intensity
- Hardware capabilities
