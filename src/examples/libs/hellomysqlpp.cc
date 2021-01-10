/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <iostream>	// for std::cout, std::cerr, std::endl
#include <mysql++.h>	// for mysqlpp::*
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a mysql++ demo program
 * You need all of these folders added with -I because of how the h files
 * for mysql++ are written.
 *
 * The mysql++ and mysql libraries are stupid and require -I to them
 * EXTRA_COMPILE_FLAGS=-I/usr/include/mysql++ -I/usr/include/mysql -Wno-deprecated -Wno-deprecated-declarations
 * EXTRA_LINK_FLAGS=-lmysqlpp
 */

int main(int argc, char** argv, char** envp) {
	// parameters are: schema, host, user, pass, use exceptions
	mysqlpp::Connection con("myworld", "localhost", "mark", "", true);
	mysqlpp::Query query=con.query("select id,name from TbOrganization order by id");
	mysqlpp::StoreQueryResult res=query.store();
	unsigned int j=0;
	std::cout << "Records Found: " << res.size() << std::endl;
	for(mysqlpp::StoreQueryResult::iterator i=res.begin(); i!=res.end(); i++) {
		mysqlpp::Row row=*i;
		std::cout << j << ":" << row["name"] << std::endl;
		j++;
	}
	return EXIT_SUCCESS;
}
