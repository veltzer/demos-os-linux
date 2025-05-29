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
#include <ace/Reactor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

/*
 * This is the handler which follows the ACE_Event_Hanlder
 * pattern. Notice the overriding of handle_input and handle_close.
 * There are other handle_* events that you can implement...
 */

class Net_Handler: public ACE_Event_Handler {
protected:
	ACE_SOCK_Stream stream;

public:
	Net_Handler(ACE_SOCK_Stream & s);
	virtual int handle_input(ACE_HANDLE handle);
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	virtual ACE_HANDLE get_handle(void) const;
};

Net_Handler::Net_Handler(ACE_SOCK_Stream& s) : stream(s) {
	// set the reactor that we are connected to
	this->reactor(ACE_Reactor::instance());
	// register outselves as handlers for that reactor
	int result=this->reactor()->register_handler(this, READ_MASK);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
}

ACE_HANDLE Net_Handler::get_handle(void) const{
	return this->stream.get_handle();
}

int Net_Handler::handle_input(ACE_HANDLE handle) {
	// if Quit (and only this string) is detected in the message the close everything
	char message[BUFSIZ];
	int result=this->stream.recv(message, sizeof(message));
	if (result > 0) {
		message[result]=0;
		// Trim CR and LF (in case of telnet)
		while(true) {
			char value=message[result - 1];
			if ((value==10) || (value==13)) {
				message[--result]=0;
			} else {
				break;
			}
		}
		// If we got "Quit" then tell the reactor
		// to end it all
		if (!ACE_OS::strcmp(message, "Quit")) {
			ACE_Reactor::end_event_loop();
			// returning -1
			return -1;
		}
		ACE_DEBUG((LM_DEBUG, "handle: %d - Remote message: %s\n", handle, message));
		// return 0 to say that we handled the message ok
		return 0;
	} else if (result==0) {
		// we did not get input so we
		ACE_DEBUG((LM_DEBUG, "Net_Handler: Connection closed\n"));
		return -1;
	} else if (errno==EWOULDBLOCK) {
		// async io state
		return 0;
	} else {
		// what state is this ?!?
		ACE_DEBUG((LM_DEBUG, "Problems in receiving data, result=%d", result));
		return -1;
	}
}

int Net_Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask) {
	ACE_DEBUG((LM_DEBUG, "Net_Handler::handle_close handle=%d\n", handle));
	// close the stream, kill the object and tell the reactor to
	// end the loop
	this->stream.close();
	delete this;
	ACE_Reactor::end_event_loop();
	return 0;
}

class Net_Listener: public ACE_Event_Handler {
protected:
	ACE_SOCK_Acceptor acceptor;

public:
	Net_Listener(int local_address);
	~Net_Listener(void);
	virtual int handle_input(ACE_HANDLE handle);
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	ACE_HANDLE get_handle(void) const;

	inline void close() {
		this->acceptor.close();
	}
};

Net_Listener::Net_Listener(int local_address) {
	acceptor=ACE_INET_Addr(local_address, ACE_LOCALHOST);
	this->reactor(ACE_Reactor::instance());
	int result=this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
}

Net_Listener::~Net_Listener(void) {
}

ACE_HANDLE Net_Listener::get_handle(void) const{
	return this->acceptor.get_handle();
}

int Net_Listener::handle_input(ACE_HANDLE handle) {
	ACE_DEBUG((LM_DEBUG, "Net_Listener::handle_input handle=%d\n", handle));
	ACE_INET_Addr remote_address;
	ACE_SOCK_Stream stream;
	// Try to find out if the implementation of the reactor that we are
	// using requires us to reset the event association for the newly
	// created handle. This is because the newly created handle will
	// inherit the properties of the listen handle, including its event
	// associations.
	int reset_new_handle=this->reactor()->uses_event_associations();
	// reset new handler
	int result=this->acceptor.accept(stream,
		&remote_address,
		0,	// timeout
		1,	// restart
		reset_new_handle
		);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
	remote_address.dump();
	Net_Handler *handler;
	ACE_NEW_RETURN(handler, Net_Handler(stream), -1);
	return 0;
}

int Net_Listener::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask) {
	ACE_DEBUG((LM_DEBUG, "Net_Listener::handle_close handle=%d\n", handle));
	this->acceptor.close();
	delete this;
	return 0;
}

int main(int argc, char** argv) {
	int port=ACE_DEFAULT_SERVER_PORT;
	if (argc > 1) {
		port=atoi(argv[1]);
	}
	ACE_DEBUG((LM_DEBUG, "Listening to ports (%d %d)\n", port, port + 1));
	Net_Listener *listener1=new Net_Listener(port);
	ACE_DEBUG((LM_DEBUG, "Have opened port (%d)\n", port));
	port++;
	Net_Listener *listener2=new Net_Listener(port);
	ACE_DEBUG((LM_DEBUG, "Have opened port (%d)\n", port));
	ACE_Reactor::run_event_loop();
	listener1->close();
	listener2->close();
	ACE_DEBUG((LM_DEBUG, "Program end\n"));
	return EXIT_SUCCESS;
}
