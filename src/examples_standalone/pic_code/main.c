#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	void (*func_ptr)(int) = _exit;
	while(1) {
	    func_ptr(7);  // Might use absolute addressing
	}
}
