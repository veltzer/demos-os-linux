/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <bfd.h>
#include <iostream>
#include <cstdlib>

using namespace std;

/*
 * This program demos the libbfd library which enabled you
 * to scan and manipulate object files in various formats
 *
 * EXTRA_LINK_FLAGS_AFTER=-lbfd
 */

int main() {
	bfd_init();
	bfd *b=bfd_openr("/bin/ls", NULL);
	if (!b) {
		cerr << "problem with open" << endl;
		return EXIT_FAILURE;
	}
	bfd_format format=bfd_object;
	bool ok_format=bfd_check_format(b, format);
	if (!ok_format) {
		cerr << "problem with bfd_check_format\n";
		return EXIT_FAILURE;
	}
	const char *name=bfd_format_string(format);
	cout << "format is " << name << endl;
	bool res=bfd_close(b);
	if (!res) {
		cerr << "problem with close" << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
