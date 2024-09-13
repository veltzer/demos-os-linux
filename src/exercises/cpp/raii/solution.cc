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

#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

class File {
private:
	FILE* m_file;
	string m_filename;

public:
	File(const string& filename, const string& mode) : m_filename(filename) {
		m_file = fopen(filename.c_str(), mode.c_str());
		if (m_file == nullptr) {
			throw runtime_error("Failed to open file: " + filename);
		}
		cout << "File opened: " << m_filename << endl;
	}

	~File() {
		if (m_file) {
			fclose(m_file);
			cout << "File closed: " << m_filename << endl;
		}
	}

	// Prevent copying
	File(const File&) = delete;
	File& operator=(const File&) = delete;

	void write(const string& data) {
		if (fputs(data.c_str(), m_file) == EOF) {
			throw runtime_error("Failed to write to file: " + m_filename);
		}
	}

	// Bonus: Additional file operations
	string read(size_t size) {
		char* buffer = new char[size + 1];
		size_t bytesRead = fread(buffer, 1, size, m_file);
		buffer[bytesRead] = '\0';
		string result(buffer);
		delete[] buffer;
		return result;
	}

	void seek(long offset, int origin) {
		if (fseek(m_file, offset, origin) != 0) {
			throw runtime_error("Failed to seek in file: " + m_filename);
		}
	}
};

void useFile(const string& filename) {
	try {
		File file(filename, "w");
		file.write("Hello, RAII!");
		// File is automatically closed when 'file' goes out of scope,
		// even if an exception is thrown
	} catch (const exception& e) {
		cerr << "Error: " << e.what() << endl;
	}
}

int main() {
	useFile("example.txt");

	// Bonus: Demonstrate additional operations
	try {
		File file("example.txt", "r+");
		cout << "File content: " << file.read(100) << endl;
		file.seek(0, SEEK_SET);
		file.write("RAII is awesome!");
	} catch (const exception& e) {
		cerr << "Error: " << e.what() << endl;
	}

	return EXIT_SUCCESS;
}
