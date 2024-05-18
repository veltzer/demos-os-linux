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

#ifndef APG_CALLBACK2_H
#define APG_CALLBACK2_H

#include <firstinclude.h>
#include <ace/OS_NS_time.h>
#include <ace/streams.h>
#include <ace/Log_Msg_Callback.h>
#include <ace/Log_Record.h>
#include <ace/SString.h>
#include <ace/Time_Value.h>

class Callback: public ACE_Log_Msg_Callback {
public:
	void log(ACE_Log_Record& log_record) {
		cerr << "Log Message Received:" << endl;
		unsigned long msg_severity=log_record.type();
		ACE_Log_Priority prio=static_cast<ACE_Log_Priority>(msg_severity);
		const char* prio_name=ACE_Log_Record::priority_name(prio);
		const time_t epoch=log_record.time_stamp().sec();
		cerr	<< "\tType: "
			<< prio_name
			<< endl
			<< "\tLength: " << log_record.length()
			<< endl
			<< "\tTime_Stamp: "
			<< ACE_OS::ctime(&epoch)
			<< endl
			<< "\tPid: " << log_record.pid()
			<< endl;
		ACE_CString data(">> ");

		data+=log_record.msg_data();
		cerr << "\tMsgData: " << data.c_str() << endl;
	}
};

#endif	/* APG_CALLBACK2_H */
