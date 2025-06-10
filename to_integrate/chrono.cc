#include <chrono>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main() {
	cout << "starting" << endl;
	auto start_time = chrono::high_resolution_clock::now();
	auto end_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	cout << "STL unordered_map lookup time: " << duration << "ms" << endl;
	return EXIT_SUCCESS;
}
