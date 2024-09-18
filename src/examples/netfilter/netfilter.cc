#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

/*
 * This is a userspace netfilter example
 * EXTRA_LINK_FLAGS_AFTER=-lnetfilter_queue
 */

static int callback(struct nfq_q_handle *qh,
	struct nfgenmsg __attribute__((unused)) *nfmsg,
	struct nfq_data *nfa,
	void __attribute__((unused))  *data
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

    h = nfq_open();
    if (!h) {
        fprintf(stderr, "Error during nfq_open()\n");
        exit(1);
    }

    if (nfq_unbind_pf(h, AF_INET) < 0) {
        fprintf(stderr, "Error during nfq_unbind_pf()\n");
        exit(1);
    }

    if (nfq_bind_pf(h, AF_INET) < 0) {
        fprintf(stderr, "Error during nfq_bind_pf()\n");
        exit(1);
    }

    qh = nfq_create_queue(h, 0, &callback, NULL);
    if (!qh) {
        fprintf(stderr, "Error during nfq_create_queue()\n");
        exit(1);
    }

    if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        fprintf(stderr, "Can't set packet_copy mode\n");
        exit(1);
    }

    fd = nfq_fd(h);

    while ((rv = recv(fd, buf, sizeof(buf), 0)) && rv >= 0) {
        nfq_handle_packet(h, buf, rv);
    }

    nfq_destroy_queue(qh);
    nfq_close(h);

    return EXIT_SUCCESS;
}
