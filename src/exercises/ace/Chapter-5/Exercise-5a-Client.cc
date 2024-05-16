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
#include <ace/Read_Buffer.h>
#include <ace/Log_Msg.h>
#include <ace/SOCK_Connector.h>
#include <stdlib.h>	// for EXIT_FAILURE

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

const int SIZE_BUF=128;
const int NO_ITERATION=5;

class Client {
public:

	ACE_SOCK_Stream client_stream_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
	Client(char *hostname, int port):remote_addr_(port, hostname) {
	}

	// Uses a connector component `connector_ to connect to a
	// remote machine and pass the connection into a stream
	// component client_stream_
	int connect_to_server() {
		// Initiate blocking connection with server.
		ACE_DEBUG((LM_DEBUG, "(%P|%t) Starting connect to %s:%d\n", remote_addr_.get_host_name(), remote_addr_.get_port_number()));
		if (connector_.connect(client_stream_, remote_addr_)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "connection failed"), -1);
		} else {
			ACE_DEBUG((LM_DEBUG, "(%P|%t) connected to %s\n", remote_addr_.get_host_name()));
		}
		return 0;
	}

	// Close down the connection properly.
	int close() {
		if (client_stream_.close()==-1) {
			ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "close"), -1);
			return -1;
		} else {
			return 0;
		}
	}
};

// Global Read_Buffer that reads from STDIN
static ACE_Read_Buffer rb(ACE_STDIN);

int GetMessageType(char *data) {
	// read a single line from stdin
	// Allocate a new buffer.
	char *buffer=rb.read('\n');
	if (buffer==0) {
		// return message type zero when EOF is reached
		return 0;
	} else {
		int type;
		sscanf(buffer, "%d", &type);
		// Remove the type from the buffer
		ACE_OS::sprintf(data, "%s", buffer + 2);
		return type;
	}
}

int main() {
	int FinalDelay=4;
	if (argc < 2) {
		ACE_DEBUG((LM_DEBUG, "%s: usage %s <port_number> [Final delay(sec)]\n", argv[0], argv[0]));
		ACE_DEBUG((LM_DEBUG, "%s: Final delay is useful for Exercise 5b\n", argv[0]));
		ACE_OS::exit(EXIT_FAILURE);
	}
	int port=ACE_OS::atoi(argv[1]);
	if (argc > 1) {
		int value=ACE_OS::atoi(argv[2]);
		if (value > FinalDelay) {
			FinalDelay=value;
		}
	}
	Client client1((char *)(ACE_LOCALHOST), port);

	port++;
	Client client2((char *)(ACE_LOCALHOST), port);

	client1.connect_to_server();
	client2.connect_to_server();
	int type=1;
	char buffer[100];
	while(type) {
		type=GetMessageType(buffer);
		// ACE_DEBUG((LM_DEBUG,"%s\n", buffer));
		if ((type==1) || (type==2)) {
			if (client1.client_stream_.send_n(buffer, ACE_OS::strlen(buffer) + 1, 0)==-1) {
				ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "send_n"), 0);
			}
		} else {
			if (client2.client_stream_.send_n(buffer, ACE_OS::strlen(buffer) + 1, 0)==-1) {
				ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "send_n"), 0);
			}
		}
	}
	if (client1.client_stream_.send_n("Quit", 5, 0)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "send_n"), 0);
	}
	// Close down the connections
	// Let the other program complete the processing
	ACE_OS::sleep(FinalDelay);
	client1.close();
	client2.close();
	return 0;
}
