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

#include <firstinclude.h>
#include <ace/Reactor.h>
#include <ace/Sig_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Signal.h>
#include <ace/Log_Msg.h>

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

// Specify that termination has not occured yet
bool Termination=false;

class Net_Handler:public ACE_Event_Handler {
public:
	Net_Handler(ACE_SOCK_Stream & s);
	virtual int handle_input(ACE_HANDLE handle);
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	virtual void terminate();
	virtual ACE_HANDLE get_handle(void) const;

protected:
	ACE_SOCK_Stream stream;
};

Net_Handler::Net_Handler(ACE_SOCK_Stream& s):stream(s) {
	this->reactor(ACE_Reactor::instance());
	int result=this->reactor()->register_handler(this, READ_MASK);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
}

ACE_HANDLE Net_Handler::get_handle(void) const {
	return this->stream.get_handle();
}

void Net_Handler::terminate() {
	ACE_Reactor::end_event_loop();
}

int Net_Handler::handle_input(ACE_HANDLE handle) {
	// if Quit(and only this string) is detected in the message the close everything
	char message[BUFSIZ];
	if (Termination) {
		ACE_Reactor::end_event_loop();
		return -1;
	}
	int result=this->stream.recv(message, sizeof message);
	if (result > 0) {
		message[result]=0;
		// Trim CR and LF(in case of telnet)
		while(true) {
			char value=message[result - 1];
			if ((value==10) || (value==13)) {
				message[--result]=0;
			} else {
				break;
			}
		}
		if (!ACE_OS::strcmp(message, "Quit")) {
			ACE_Reactor::end_event_loop();
			return -1;
		}
		ACE_DEBUG((LM_DEBUG, "handle: %d - Remote message: %s\n", handle, message));
		// Wait to allow kill from other terminal
		// ACE_OS::sleep(1);
	} else if (result==0) {
		ACE_DEBUG((LM_DEBUG, "(1) - Connection closed\n"));
		return -1;
	} else if (errno==EWOULDBLOCK) {
		return 0;
	} else {
		ACE_DEBUG((LM_DEBUG, "Problems in receiving data, result=%d", result));
		return -1;
	}
	return 0;
}

int Net_Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask) {
	ACE_DEBUG((LM_DEBUG, "Net_Handler::handle_close handle=%d\n", handle));
	this->stream.close();
	delete this;
	ACE_Reactor::end_event_loop();
	return 0;
}

class Net_Listener:public ACE_Event_Handler {
public:
	Net_Listener(int local_address);
	~Net_Listener(void);
	virtual int handle_input(ACE_HANDLE handle);
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
	ACE_HANDLE get_handle(void) const;

protected:

public:
	ACE_SOCK_Acceptor acceptor;
	Net_Handler* Save_handler;
};

Net_Listener::Net_Listener(int local_address) {
	acceptor=ACE_INET_Addr(local_address, ACE_LOCALHOST);
	this->reactor(ACE_Reactor::instance());
	int result=this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
	Save_handler=0;
}

Net_Listener::~Net_Listener(void) {
}

ACE_HANDLE Net_Listener::get_handle(void) const {
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
	int reset_new_hndl=this->reactor()->uses_event_associations();
	// reset new handler
	int result=this->acceptor.accept(stream,
		&remote_address,
		0,	// timeout
		1,	// restart
		reset_new_hndl
		);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
	remote_address.dump();
	Net_Handler *handler;
	ACE_NEW_RETURN(handler, Net_Handler(stream), -1);
	Save_handler=handler;
	return 0;
}

int Net_Listener::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask) {
	ACE_DEBUG((LM_DEBUG, "Net_Listener::handle_close handle=%d\n", handle));
	this->acceptor.close();
	delete this;
	return 0;
}

class CatchSignal:public ACE_Event_Handler {
protected:

public:
	Net_Listener *listener;
	int signum;

public:
	CatchSignal(int isignum):listener(NULL), signum(isignum) {
	}
	virtual ~CatchSignal() {
	}
	virtual int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0) {
		ACE_TRACE("CatchSignal::handle_signal");
		// Make sure the right handler was called back.
		ACE_ASSERT(signum==this->signum);
		ACE_DEBUG((LM_DEBUG, "%S occured. Terminating the program\n", signum));
		if (listener->Save_handler) {
			ACE_DEBUG((LM_DEBUG, "Pointer exists. Terminating the program\n"));
			listener->Save_handler->terminate();
		} else {
			// Since the Save_handler does not
			// have a pointer we will set a flag
			Termination=true;
		}
		return 0;
	}
};

int main(int argc, char** argv) {
	Net_Listener* listener1=0;
	Net_Listener* listener2=0;
	int port=ACE_DEFAULT_SERVER_PORT;
	ACE_Sig_Handler handler;

	// SIGTRAP is 5
	CatchSignal terminate(SIGTRAP);
	if (argc > 1) {
		port=atoi(argv[1]);
	}
	ACE_DEBUG((LM_DEBUG, "Listening to ports(%d %d)\n", port, port + 1));
	handler.register_handler(SIGTRAP, &terminate);
	listener1=new Net_Listener(port);

	port++;
	listener2=new Net_Listener(port);

	// Store Net_Listener for Reactor method
	terminate.listener=listener1;
	ACE_DEBUG((LM_DEBUG, "my ppid is: %P\n"));
	ACE_Reactor::run_event_loop();
	listener1->acceptor.close();
	listener2->acceptor.close();
	ACE_OS::sleep(4);
	ACE_DEBUG((LM_DEBUG, "Program end\n"));
	return 0;
}
