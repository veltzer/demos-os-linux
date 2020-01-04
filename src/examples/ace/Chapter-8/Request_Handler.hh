/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __REQUEST_HANDLER_H_
#define __REQUEST_HANDLER_H_

#include <firstinclude.h>
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/Thread.h>

class Request_Handler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> {
	// This class is the Svc_Handler used by <Acceptor>.

public:
	Request_Handler(ACE_Thread_Manager * tm=0);
	// The default constructor makes sure the right reactor is used.

protected:
	virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);

	virtual int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask=0);

private:
	size_t nr_msgs_rcvd_;
};

#endif	/* __REQUEST_HANDLER_H_ */
