int add(int a, int b) __attribute__ ((weak));
int add(int a, int b) {
	return a+b;
}
