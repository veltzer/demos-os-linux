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

#include <firstinclude.h>
#include <mysql/mysql.h>
#include <iostream>
#include <cstdlib>

using namespace std;

/*
 * This is a demo program for regular C mysql access
 *
 * TODO:
 * - In this file the three parameters: database name,
 * database user and the password are hardcoded. Get
 * them out of here to some outside xml options file
 *
 * EXTRA_LINK_FLAGS_AFTER=-lmysqlclient
 */

int main() {
	MYSQL mysql;
	MYSQL *ret0=mysql_init(&mysql);
	if (!ret0) {
		cerr << "init failed" << endl;
		cerr << mysql_error(&mysql) << endl;
		return EXIT_FAILURE;
	}
	cout << "after init" << endl;
	MYSQL *ret1=mysql_real_connect(&mysql, "database", "master", "master", "mysql", 0, NULL, 0);
	if (!ret1) {
		cerr << "connect failed" << endl;
		cerr << mysql_error(&mysql) << endl;
		return EXIT_FAILURE;
	}
	cout << "after connect" << endl;
	const char *info=mysql_get_server_info(&mysql);
	if (!info) {
		cerr << "get_server_info failed" << endl;
		cerr << mysql_error(&mysql) << endl;
		return EXIT_FAILURE;
	}
	cout << "info is " << info << endl;
	mysql_close(&mysql);	// there are no error codes for this one
	return EXIT_SUCCESS;
}
