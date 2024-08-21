void sum(int* restrict array, int size, int* restrict result) {
	*result=0;
	for (int i=0;i<size;i++) {
		*result+=array[i];
	}
}
