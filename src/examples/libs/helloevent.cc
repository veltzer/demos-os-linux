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

/*
 * This exmple program provides a trivial server program that listens for TCP
 * connections on port 9995. When they arrive, it writes a short message to
 * each client connection, and closes each connection once it is flushed.
 * Where possible, it exits cleanly in response to a SIGINT (ctrl-c).
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags libevent
 * EXTRA_LINK_CMD=pkg-config --libs libevent
 */

#include <firstinclude.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#ifndef WIN32
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#include <sys/socket.h>
#endif

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

static const char MESSAGE[] = "Hello, World!\n";

static const int PORT = 9995;

static void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void*);
static void conn_writecb(struct bufferevent *, void*);
static void conn_eventcb(struct bufferevent *, short, void*);
static void signal_cb(evutil_socket_t, short, void*);

int main() {
	struct event_base *base;
	struct evconnlistener *listener;
	struct event *signal_event;

	struct sockaddr_in sin;
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void*)base,
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));
	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}
	signal_event = evsignal_new(base, SIGINT, signal_cb, (void*)base);
	if (!signal_event || event_add(signal_event, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return 1;
	}
	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");
	return 0;
}

static void listener_cb(struct evconnlistener *listener __attribute__((unused)),
	evutil_socket_t fd,
	struct sockaddr *sa __attribute__((unused)),
	int socklen __attribute__((unused)),
	void* user_data)
{
	struct event_base *base = (struct event_base*) user_data;
	struct bufferevent *bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, NULL, conn_writecb, conn_eventcb, NULL);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_disable(bev, EV_READ);
	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void conn_writecb(struct bufferevent *bev, void* user_data __attribute__((unused))) {
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}

static void conn_eventcb(struct bufferevent *bev __attribute__((unused)), short events, void* user_data __attribute__((unused))) {
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
			strerror(errno));	/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

static void signal_cb(evutil_socket_t sig __attribute__((unused)), short events __attribute__((unused)), void* user_data __attribute__((unused))) {
	struct event_base *base = (struct event_base*)user_data;
	struct timeval delay = { 2, 0 };
	printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");
	event_base_loopexit(base, &delay);
}
