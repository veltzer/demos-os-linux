/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <pcap.h>	// for pcap_open_live(3), pcap_setdirection(3), pcap_compile(3), pcap_setfilter(3), pcap_loop(3), pcap_freecode(3), pcap_close(3), bpf_program, pcap_t, pcap_pkthdr
#include <stdio.h>	// for fprintf(3), printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_NULL()

/*
 * This is a simple example showing how to use the pcap user space library to capture packets according
 * to defined filters.
 * Remmember that you have to install the development package and link with the pcap library.
 * In addition you have to run it as root.
 * Example: sudo ./src/examples/networking/pcap_simple.elf any ip -1
 *
 * EXTRA_LINK_FLAGS=-lpcap
 */

/*
 * This callback is called for every packet
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	/* packet counter */
	static int count=1;
	printf("Packet number %d recevied on %ld seconds and %ld usec\n", count, header->ts.tv_sec, header->ts.tv_usec);
	/* packet is a pointer to the packet. */
	count++;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [device] [filter expression] [numpackets]\n", argv[0], argv[0]);
		fprintf(stderr, "use 'any' for any device\n");
		fprintf(stderr, "use 'ether proto 17 and ether[15] & 1!=0' for the filter expression\n");
		fprintf(stderr, "use '-1' for numpackets to capture forever\n");
		return EXIT_FAILURE;
	}
	// get the user parameters
	/* capture device name */
	char* dev=argv[1];
	/* filter expression */
	char* filter_exp=argv[2];
	/* number of packets to capture */
	int num_packets=atoi(argv[3]);
	/* Snap length (maximum bytes per packet to capture) */
	const unsigned int SNAP_LEN=1518;

	/* print capture info */
	printf("Device: %s\n", dev);
	printf("Number of packets: %d\n", num_packets);
	printf("Filter expression: %s\n", filter_exp);

	/* error buffer */
	char errbuf[PCAP_ERRBUF_SIZE];
	/* Open capture device */
	pcap_t* handle=(pcap_t*)CHECK_NOT_NULL(pcap_open_live(dev, SNAP_LEN, 0, 1000, errbuf));

	/* Set the direction - we only want incoming packets */
	CHECK_NOT_M1(pcap_setdirection(handle, PCAP_D_IN));

	/* Compile the filter expression
	 * compiled filter program (expression) */
	struct bpf_program fp;
	CHECK_NOT_M1(pcap_compile(handle, &fp, filter_exp, 1, 0));

	/* Apply the compiled filter */
	CHECK_NOT_M1(pcap_setfilter(handle, &fp));

	/* now we can set our callback function */
	pcap_loop(handle, num_packets, got_packet, NULL);

	/* cleanup */
	pcap_freecode(&fp);
	pcap_close(handle);
	return EXIT_SUCCESS;
}
