#include <stdio.h>
#include <stdlib.h>

// restricted pointers assumption
int sum(int *a, int *b) {
	*a = 4;
	return *b;
}

int main(int argc, char **argv) {
	int src = 6;
	int *a, *b;
	a = b = &src;
	printf("%d", foo(a, b)); // Prints 4
	return EXIT_SUCCESS;
}
