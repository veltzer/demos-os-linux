void sum(int* array, int size, int* result) {
	int temp=0;
	for (int i=0;i<size;i++) {
		temp+=array[i];
	}
	*result=temp;
}
