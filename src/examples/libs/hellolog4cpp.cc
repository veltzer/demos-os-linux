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
#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/SyslogAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/NDC.hh>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_LINK_FLAGS_AFTER=-llog4cpp
 */

int main(int, char** argv) {
	log4cpp::SyslogAppender *syslogAppender=new log4cpp::SyslogAppender("syslog", argv[0], LOG_USER);

	log4cpp::Category& root=log4cpp::Category::getRoot();

	root.setAppender(syslogAppender);

	root.error("root error");
	root.warn("root warn");

	return EXIT_SUCCESS;
}
