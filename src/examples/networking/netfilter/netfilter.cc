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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <err_utils.h>

/*
 * This is a userspace netfilter example
 * EXTRA_LINK_FLAGS_AFTER=-lnetfilter_queue
 */

static int callback(struct nfq_q_handle *qh,
	struct nfgenmsg __attribute__((unused)) *nfmsg,
	struct nfq_data *nfa,
	void __attribute__((unused)) *data
)
{
	u_int32_t id = 0;
	struct nfqnl_msg_packet_hdr *ph;

	ph = nfq_get_msg_packet_hdr(nfa);
	if (ph) {
		id = ntohl(ph->packet_id);
	}

	printf("Processing packet with id: %u\n", id);

	// Here you can analyze the packet and make a decision
	// For this example, we'll always accept the packet
	return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
}

int main()
{
	struct nfq_handle *h;
	struct nfq_q_handle *qh;
	int fd;
	int rv;
	char buf[4096] __attribute__ ((aligned));

	CHECK_NOT_NULL(h=nfq_open());
	CHECK_NOT_M1(nfq_unbind_pf(h, AF_INET));
	CHECK_NOT_M1(nfq_bind_pf(h, AF_INET));

	CHECK_NOT_NULL(qh = nfq_create_queue(h, 0, &callback, NULL));
	CHECK_NOT_M1(nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0);
	fd = nfq_fd(h);

	while ((rv = recv(fd, buf, sizeof(buf), 0)) && rv >= 0) {
		nfq_handle_packet(h, buf, rv);
	}

	nfq_destroy_queue(qh);
	nfq_close(h);
	return EXIT_SUCCESS;
}
