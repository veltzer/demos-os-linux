/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MATRIX_SIZE 256 // Smaller matrices to fit in cache
#define BLOCK_SIZE 32 // Block size for cache efficiency
#define NUM_RUNS 500 // Fewer runs as matrix mult takes longer
#define WARMUP_RUNS 1

void scalar_matrix_multiply(float* A, float* B, float* C, int n) {
	// Initialize result matrix to zero
	memset(C, 0, n * n * sizeof(float));

	// Basic matrix multiplication with blocking for cache efficiency
	for(int ii = 0; ii < n; ii += BLOCK_SIZE) {
		for(int jj = 0; jj < n; jj += BLOCK_SIZE) {
			for(int kk = 0; kk < n; kk += BLOCK_SIZE) {
				// For each block
				for(int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {
					for(int j = jj; j < jj + BLOCK_SIZE && j < n; j++) {
						float sum = C[i * n + j];
						for(int k = kk; k < kk + BLOCK_SIZE && k < n; k++) {
							sum += A[i * n + k] * B[k * n + j];
						}
						C[i * n + j] = sum;
					}
				}
			}
		}
	}
}

void simd_matrix_multiply(float* A, float* B, float* C, int n) {
	// Initialize result matrix to zero
	memset(C, 0, n * n * sizeof(float));

	// Blocked matrix multiplication with SIMD
	for(int ii = 0; ii < n; ii += BLOCK_SIZE) {
		for(int jj = 0; jj < n; jj += BLOCK_SIZE) {
			for(int kk = 0; kk < n; kk += BLOCK_SIZE) {
				// For each block
				for(int i = ii; i < ii + BLOCK_SIZE && i < n; i++) {
					for(int j = jj; j < jj + BLOCK_SIZE && j < n; j += 8) {
						__m256 sum = _mm256_load_ps(&C[i * n + j]);

						for(int k = kk; k < kk + BLOCK_SIZE && k < n; k++) {
							// Broadcast single value from A to all elements
							__m256 a = _mm256_set1_ps(A[i * n + k]);
							// Load 8 elements from B
							__m256 b = _mm256_load_ps(&B[k * n + j]);
							// Multiply and accumulate
							sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
						}

						_mm256_store_ps(&C[i * n + j], sum);
					}
				}
			}
		}
	}
}

static inline double get_time() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void init_matrix(float* mat, int n) {
	for(int i = 0; i < n * n; i++) {
		mat[i] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
	}
}

int main() {
	float *A = NULL, *B = NULL, *C_scalar = NULL, *C_simd = NULL;
	double scalar_time = 0.0, simd_time = 0.0;
	int ret = 0;
	const int n = MATRIX_SIZE;

	// CPU feature detection
	if (!__builtin_cpu_supports("avx")) {
		fprintf(stderr, "Error: This CPU does not support AVX instructions!\n");
		return 1;
	}

	// Allocate aligned memory for matrices
	ret = posix_memalign((void**)&A, 32, n * n * sizeof(float));
	if (ret != 0) {
		fprintf(stderr, "Error allocating matrix A: %s\n", strerror(ret));
		goto cleanup;
	}

	ret = posix_memalign((void**)&B, 32, n * n * sizeof(float));
	if (ret != 0) {
		fprintf(stderr, "Error allocating matrix B: %s\n", strerror(ret));
		goto cleanup;
	}

	ret = posix_memalign((void**)&C_scalar, 32, n * n * sizeof(float));
	if (ret != 0) {
		fprintf(stderr, "Error allocating matrix C_scalar: %s\n", strerror(ret));
		goto cleanup;
	}

	ret = posix_memalign((void**)&C_simd, 32, n * n * sizeof(float));
	if (ret != 0) {
		fprintf(stderr, "Error allocating matrix C_simd: %s\n", strerror(ret));
		goto cleanup;
	}

	// Initialize matrices
	printf("Initializing matrices (%dx%d)...\n", n, n);
	init_matrix(A, n);
	init_matrix(B, n);

	// Warmup runs
	printf("Warming up...\n");
	for(int i = 0; i < WARMUP_RUNS; i++) {
		scalar_matrix_multiply(A, B, C_scalar, n);
		simd_matrix_multiply(A, B, C_simd, n);
	}

	printf("Running benchmark...\n");

	// Timing runs
	for(int run = 0; run < NUM_RUNS; run++) {
		double start, end;

		printf("Run %d/%d:\n", run + 1, NUM_RUNS);

		// Time scalar version
		printf("Running scalar version...\n");
		start = get_time();
		scalar_matrix_multiply(A, B, C_scalar, n);
		end = get_time();
		scalar_time += (end - start);

		// Time SIMD version
		printf("Running SIMD version...\n");
		start = get_time();
		simd_matrix_multiply(A, B, C_simd, n);
		end = get_time();
		simd_time += (end - start);
	}

	// Calculate average times
	scalar_time /= NUM_RUNS;
	simd_time /= NUM_RUNS;

	// Verify results
	int mismatch = 0;
	float max_diff = 0.0f;
	int mismatch_count = 0;
	for(int i = 0; i < n * n; i++) {
		float diff = fabsf(C_scalar[i] - C_simd[i]);
		if (diff > max_diff) max_diff = diff;
		if (diff > 1e-4) { // Slightly relaxed tolerance for matrix multiplication
			mismatch_count++;
			if (mismatch_count <= 5) { // Show first few mismatches
				printf("Mismatch at index %d: scalar=%.7f, simd=%.7f, diff=%.7f\n",
					i, C_scalar[i], C_simd[i], diff);
			}
			mismatch = 1;
		}
	}

	printf("\nResults:\n");
	printf("Matrix size: %dx%d\n", n, n);
	printf("Scalar time: %.6f seconds\n", scalar_time);
	printf("SIMD time: %.6f seconds\n", simd_time);
	printf("Speedup: %.2fx\n", scalar_time / simd_time);
	printf("Max diff: %.9f\n", max_diff);
	printf("Mismatches: %d\n", mismatch_count);
	printf("Results %s\n", mismatch ? "MISMATCH!" : "match within tolerance");

	// Calculate and print GFLOPS
	double ops = 2.0 * n * n * n; // multiply-add for each element
	printf("Scalar Performance: %.2f GFLOPS\n", (ops / scalar_time) / 1e9);
	printf("SIMD Performance: %.2f GFLOPS\n", (ops / simd_time) / 1e9);

cleanup:
	free(A);
	free(B);
	free(C_scalar);
	free(C_simd);

	return ret;
}
