/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3), fopen(3), fgets(3), fclose(3), snprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <string.h>	// for strchr(3), strrchr(3)
#include <ctype.h>	// for isdigit(3)
#include <dirent.h>	// for opendir(3), readdir(3), closedir(3)
#include <map>	// for std::map
#include <vector>	// for std::vector
#include <string>	// for std::string
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_ZERO(), CHECK_ASSERT()

/*
 * This is a primitive implementation of pstree(1).
 *
 * It scans the /proc filesystem: every directory whose name is a number
 * is a process and the pid of the process is the name of the directory.
 * For each process we read /proc/[pid]/stat to find out its name (comm)
 * and its parent pid (ppid). Then we print the resulting tree starting
 * from the processes which have no parent (init(1) which has ppid 0 and
 * kernel threads like kthreadd(2)).
 *
 * Note the parsing of /proc/[pid]/stat: the process name is surrounded
 * by parenthesis and may itself contain spaces and even parenthesis so
 * we look for the LAST closing parenthesis in the line.
 *
 * Note also that processes may come and go while we are scanning /proc
 * so failure to open a stat file of a process we have just seen is not
 * an error - the process simply died - and we skip it.
 */

// map from pid to process name
static std::map<int, std::string> names;
// map from pid to parent pid
static std::map<int, int> parents;
// map from pid to the list of its children
static std::map<int, std::vector<int> > children;

static void scan_proc(void) {
	DIR* d=(DIR*)CHECK_NOT_NULL(opendir("/proc"));
	const struct dirent* ent;
	while((ent=readdir(d))!=NULL) {
		if(!isdigit(ent->d_name[0])) {
			continue;
		}
		int pid=atoi(ent->d_name);
		char filename[256];
		snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
		FILE* f=fopen(filename, "r");
		if(f==NULL) {
			// the process died while we were scanning...
			continue;
		}
		char line[1024];
		const char* res=fgets(line, sizeof(line), f);
		CHECK_ZERO(fclose(f));
		if(res==NULL) {
			continue;
		}
		// the name is between the first '(' and the LAST ')'
		char* open_paren=strchr(line, '(');
		char* close_paren=strrchr(line, ')');
		CHECK_ASSERT(open_paren!=NULL && close_paren!=NULL && open_paren<close_paren);
		std::string name(open_paren+1, close_paren);
		// after the ')' comes a space, the state character, a space and the ppid
		char state;
		int ppid;
		CHECK_ASSERT(sscanf(close_paren+1, " %c %d", &state, &ppid)==2);
		names[pid]=name;
		parents[pid]=ppid;
		children[ppid].push_back(pid);
	}
	CHECK_ZERO(closedir(d));
}

static void print_tree(int pid, int depth) {
	printf("%*s%s(%d)\n", depth*4, "", names[pid].c_str(), pid);
	const std::vector<int>& kids=children[pid];
	for(unsigned int i=0; i<kids.size(); i++) {
		print_tree(kids[i], depth+1);
	}
}

int main() {
	scan_proc();
	// print all the processes which have no living parent as roots
	// (init(1) with ppid 0 and the kernel threads under kthreadd(2))
	for(std::map<int, int>::iterator it=parents.begin(); it!=parents.end(); ++it) {
		if(names.find(it->second)==names.end()) {
			print_tree(it->first, 0);
		}
	}
	return EXIT_SUCCESS;
}
