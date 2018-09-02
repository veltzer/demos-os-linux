/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __MESSAGE_RECEIVER_H_
#define __MESSAGE_RECEIVER_H_

#include <firstinclude.h>
#include <ace/Log_Msg.h>
#include <ace/Message_Block.h>
#include <ace/SOCK_Stream.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch.h>
#include <ace/Task.h>

struct DeviceCommandHeader {
	int length_;
	int deviceId_;
};

class HA_Device_Repository {
public:
	HA_Device_Repository();
	int update_device(int device_id, char *commands);

private:
	ACE_Task_Base *owner_;
};

HA_Device_Repository::HA_Device_Repository() {
}

int HA_Device_Repository::update_device(int, char *) {
	return(0);
}

class HA_CommandHandler : public ACE_Task<ACE_MT_SYNCH> {
public:
	HA_CommandHandler(HA_Device_Repository & rep) : rep_(rep) {
	}
	virtual int svc();

private:
	HA_Device_Repository& rep_;
};

class Message_Receiver : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> {
public:
	Message_Receiver() : handler_(0) {
		ACE_ASSERT(0);
	}
	Message_Receiver(HA_CommandHandler* ch) : handler_(ch) {
	}
	ACE_Message_Block *shut_down_message(void);
	virtual int handle_input(ACE_HANDLE fd);
	virtual int handle_close(ACE_HANDLE=ACE_INVALID_HANDLE, ACE_Reactor_Mask=ACE_Event_Handler::ALL_EVENTS_MASK) {
		this->peer().close();
		delete this;
		return(0);
	}

private:
	int read_header(DeviceCommandHeader *dch);
	int copy_payload(ACE_Message_Block *mb, int payload_length);

private:
	HA_CommandHandler *handler_;
};
#endif	/* __MESSAGE_RECEIVER_H */
