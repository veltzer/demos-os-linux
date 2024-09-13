#include <unistd.h>

int main(int argc, char** argv,char** envp) {
	execve("./hello.sh", argv, envp);
}
