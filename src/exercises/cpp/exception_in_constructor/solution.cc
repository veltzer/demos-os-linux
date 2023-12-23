/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

class Resource {
public:
	Resource(const char* filename) {
		if(!is_valid_file(filename)) {
			throw invalid_argument("file name is empty or non-existent");
		}

		file_handle = fopen(filename, "r");

		if(file_handle == nullptr) {
			throw runtime_error("failed to open file: " + string(strerror(errno)));
		}
	}

	~Resource() {
		if(file_handle) fclose(file_handle);
	}

	size_t read(char buf[], int len) {
		// wrap fread in try/catch
		return fread(buf, 1, len, file_handle);
	}

private:
	bool is_valid_file(const char* filename) {
		return filename != nullptr && strlen(filename) > 0;
	}

	FILE* file_handle = nullptr;
};

int main(int argc, char** argv, char** envp) {
	try {
		Resource file("testfile.dat");
		char data[100];
		file.read(data, 100);
	}
	catch(const exception& e) {
		cerr << e.what() << endl;
	}
}
