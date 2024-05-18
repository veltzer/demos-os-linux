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

#ifndef __Client_h
#define __Client_h

#include <firstinclude.h>
#include <ace/Synch_Traits.h>
#include <ace/Null_Condition.h>
#include <ace/Null_Mutex.h>
#include <ace/Reactor.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Connector.h>
#include <ace/Svc_Handler.h>
#include <ace/Reactor_Notification_Strategy.h>

class Client: public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> {
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

public:
	Client() : notifier_(0, this, ACE_Event_Handler::WRITE_MASK) {
	}
	// Called when the connection is established.
	virtual int open(void* =0);

	// Called when input is available from the server.
	virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);

	// Called when output is possible.
	virtual int handle_output(ACE_HANDLE fd=ACE_INVALID_HANDLE);

	// Called when a timer expires.
	virtual int handle_timeout(const ACE_Time_Value& current_time, const void* act=0);

private:
	enum {
		ITERATIONS=5
	};
	int iterations_;
	ACE_Reactor_Notification_Strategy notifier_;
};

#endif	/* __Client_h */
