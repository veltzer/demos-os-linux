/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
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
