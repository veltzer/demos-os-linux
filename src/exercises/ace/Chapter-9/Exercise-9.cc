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
// We must replace Acceptor with Connector
// #define ACE_NTRACE 0
#include <ace/config-lite.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <ace/Process_Manager.h>
#include <ace/OS_NS_time.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

#define NCHILDREN 2
int MyIndex=-1;
long ListenPort;

class SignalableTask:public ACE_Task<ACE_MT_SYNCH> {
public:
	virtual int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0) {
		if (signum==SIGUSR1) {
			// ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("(%t) received a %S signal\n"), signum));
			MyIndex=0;
			handle_alert();
		}
		if (signum==SIGUSR2) {
			// ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("(%t) received a %S signal\n"), signum));
			MyIndex=1;
			handle_alert();
		}
		return 0;
	}

	void handle_alert();
};

void SignalableTask::handle_alert() {
}

int DoAccept(long ReceivePort, ACE_SOCK_Stream *peer, ACE_INET_Addr *peer_addr, ACE_SOCK_Acceptor *acceptor) {
	ACE_Time_Value timeout(10, 0);

	ACE_INET_Addr address_to_listen=ACE_INET_Addr(ReceivePort, ACE_LOCALHOST);
	if (acceptor->open(address_to_listen, 1)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("acceptor.open")), 100);
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Trying to accept %d.\n"), ReceivePort));
	if (acceptor->accept(*peer, peer_addr, &timeout, 0)==-1) {
		if (ACE_OS::last_error()==EINTR) {
			ACE_DEBUG((LM_DEBUG, "waiting for connection"));
		} else if (ACE_OS::last_error()==ETIMEDOUT) {
			ACE_DEBUG((LM_DEBUG, "Timeout while waiting for connection"));
		}
	} else {
		ACE_TCHAR peer_name[MAXHOSTNAMELEN];
		peer_addr->addr_to_string(peer_name, MAXHOSTNAMELEN);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection from %s\n"), peer_name));
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) accept Done %d.\n"), ReceivePort));
	return 0;
}

int ReceiveMessage(ACE_SOCK_Stream *peer) {
	char buffer[4096];

	peer->recv(buffer, sizeof(buffer));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) R E C E I V I N G: <%s>\n"), buffer));
	// Send Acknowledge
	peer->send_n("OK", 3, 0);
	ACE_OS::sleep(1);
	peer->close();
	return 0;
}

int main(int argc, char** argv) {
	SignalableTask handler;
	if (argc < 3) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s: usage: %s Port1 Port2\n"), argv[0], argv[0]));
		return 1;
	}
	if (argc > 3) {
		// Running as a child.
		ACE_Sig_Handler sh;
		ACE_SOCK_Stream peer;
		ACE_SOCK_Connector connector;
		char buffer[4096];
		ACE_Time_Value timeout(10, 0);

		// Use SIGUSR1
		sh.register_handler(SIGUSR1, &handler);
		// Use SIGUSR2
		sh.register_handler(SIGUSR2, &handler);
		// wit untill MyIndex is modified
		while(MyIndex==-1) {
			ACE_OS::sleep(1);
		}
		ListenPort=atoi(argv[MyIndex + 1]);

		ACE_INET_Addr address_to_listen=ACE_INET_Addr(ListenPort, ACE_LOCALHOST);
		ACE_OS::sleep(1);
		if (-1==connector.connect(peer, address_to_listen, &timeout)) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("%p\n"), ACE_TEXT("(%P|%t) Client %d connect"), MyIndex + 1));
			return 1;
		}
		sprintf(buffer, "Im a child %d This is My Message.", MyIndex + 1);
		size_t size=ACE_OS::strlen(buffer);
		ACE_DEBUG((LM_DEBUG, "Child %d sending: <%s>\n", MyIndex + 1, buffer));
		peer.send_n(buffer, size, 0);
		// Get Acknowledge
		peer.recv(buffer, sizeof(buffer));
		peer.close();
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Child %d is terminating.\n"), MyIndex + 1));
	} else {
		// Running as a parent.
		ACE_Process_Manager *pm=ACE_Process_Manager::instance();
		// Get the processwide process manager.
		ACE_SOCK_Stream peer[2];
		ACE_INET_Addr peer_addr[2];
		ACE_SOCK_Acceptor acceptor[2];

		// Specify the options for the new processes to be spawned.
		ACE_Process_Options options;
		options.command_line(ACE_TEXT("%s %s %s Extra"), argv[0], argv[1], argv[2]);

		// Spawn two child processes.
		pid_t pids[NCHILDREN];
		pm->spawn_n(NCHILDREN, options, pids);

		// 1st process will use the 1st port
		ACE_OS::kill(pids[0], SIGUSR1);
		// 2nd process will use the 2nd port
		ACE_OS::kill(pids[1], SIGUSR2);

		// Wait for the child we just terminated.
		long ListenPort1=atoi(argv[1]);
		long ListenPort2=atoi(argv[2]);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Parent Ports are: %d %d\n"), ListenPort1, ListenPort2));
		// Do the messaging with 1st Process
		DoAccept(ListenPort1, &peer[0], &peer_addr[0], &acceptor[0]);
		DoAccept(ListenPort2, &peer[1], &peer_addr[1], &acceptor[1]);
		// Do the messaging with 1st Process
		ReceiveMessage(&peer[0]);
		// Do the messaging with 2nd Process
		ReceiveMessage(&peer[1]);
		// Wait for all the children to exit.
		pm->wait(0);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Parent is terminating.\n")));
	}
	return EXIT_SUCCESS;
}
