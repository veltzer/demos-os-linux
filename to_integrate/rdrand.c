#include <stdio.h>
#include <immintrin.h>

/*
 * This example shows how to use the gcc _rdrand64_step intrinsic function
 *
 * References:
 * - https://stackoverflow.com/questions/31214457/how-to-use-rdrand-intrinsics
 */

int main() {
	unsigned long long result = 0ULL;
	int rc = _rdrand64_step (&result);
	printf("%i %llu", rc, result);
	return (rc != 1);
}
