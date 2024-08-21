#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
	printf("I'm here and I'm alive...\n");
	for(int i=0;i<10;i++) {
		// this is the bug, this line will cause the operating system to kill me...:)
		printf("i is %d\n", i);
		if(i==5) {
			char* p=(char*)0;
			*p=0;
		}
		sleep(1);
	}
	printf("You can't see this becuase the process is already dead...\n");
	return EXIT_SUCCESS;
}
