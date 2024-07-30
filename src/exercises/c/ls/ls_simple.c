#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <sys/stat.h>

int main() {
	DIR* p=opendir(".");
	assert(p!=NULL);
	struct dirent* current;
	while((current=readdir(p))) {
		printf("got file [%s]\n", current->d_name);
		struct stat statbuf;
		int res=stat(current->d_name, &statbuf);
		assert(res!=-1);
		printf("uid is %d\n", statbuf.st_uid);
		printf("gid is %d\n", statbuf.st_gid);

	}
	return 0;
}
