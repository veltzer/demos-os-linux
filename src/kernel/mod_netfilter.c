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

/* #define DEBUG */
#include <linux/kernel.h>
#include <linux/module.h> /* for MODULE_*. module_* */
#include <linux/printk.h> /* for pr_* */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <linux/skbuff.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <net/ip.h>
#include <linux/printk.h> /* for pr_* */
#include <linux/proc_fs.h> /* Necessary because we use the proc fs */
#include <asm/uaccess.h> /* For copy_from_user */

/*
 * This is a pretty simple example of a netfilter module.
 *
 * References:
 * http://www.lainoox.com/netfilter-hook-packet-filter/
 */

MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Basic netfilter module");
MODULE_LICENSE("GPL");

/* #define DO_PROC */

static struct nf_hook_ops nfho; /* net filter hook option struct */
static int filter_value = 1; /* initialized automatically to NULL */
#ifdef DO_PROC
static const char *skb_filter_name = KBUILD_MODNAME;
static struct proc_dir_entry *skb_filter;
#endif /* DO_PROC */

/*
old API
static unsigned int hook_func(
	unsigned int hooknum,
	struct sk_buff *skb,
	const struct net_device *in,
	const struct net_device *out,
	int (*okfn)(struct sk_buff *)) {
*/
static unsigned int hook_func(
	void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state
) {
	struct iphdr *ip_header; /* IP header struct */
	struct udphdr *udp_header; /* UDP header struct */
	struct icmphdr *icmp_header; /* ICMP Header */
	if (!skb)
		return NF_ACCEPT;
	ip_header = (struct iphdr *)skb_network_header(skb);
	if (ip_header->protocol == IPPROTO_UDP) {
		udp_header = (struct udphdr *)(skb_transport_header(skb) +
				ip_hdrlen(skb));
		if (udp_header)
			pr_info("SRC: (%pI4):%d --> DST: (%pI4):%d\n",
					&ip_header->saddr,
					ntohs(udp_header->source),
					&ip_header->daddr,
					ntohs(udp_header->dest)
			);
		else
			return NF_DROP;
	}
	if (ip_header->protocol == IPPROTO_ICMP) {
		pr_info("---------- ICMP -------------\n");
		icmp_header = (struct icmphdr *)(skb_transport_header(skb) +
				ip_hdrlen(skb));
		if (icmp_header) {
			pr_info("SRC: (%pI4) --> DST: (%pI4)\n",
					&ip_header->saddr,
					&ip_header->daddr
			);
			pr_info("ICMP type: %d - ICMP code: %d\n",
					icmp_header->type,
					icmp_header->code
			);
		} else
			return NF_DROP;
	}
	return filter_value == 0 ? NF_ACCEPT : NF_DROP;
}

#ifdef DO_PROC
static int skb_read(char *page, char **start, off_t off, int count, int *eof,
		void *data)
{
	int len;
	if (off > 0) {
		*eof = 1;
		return 0;
	}
	if (count < sizeof(int)) {
		*eof = 1;
		return -ENOSPC;
	}
	/* copy to userspace */
	memcpy(page, &filter_value, sizeof(int));
	len = sizeof(int);
	return len;
}

static int skb_write(struct file *file, const char *buffer, unsigned long len,
		void *data)
{
	int ret;
	unsigned char userData;
	if (len > PAGE_SIZE || len < 0) {
		pr_err("SKB System: cannot allow space for data\n");
		return -ENOSPC;
	}
	/* write data to the buffer */
	ret = copy_from_user(&userData, buffer, 1);
	if (ret) {
		pr_err("SKB System: cannot copy data from userspace\n");
		return ret;
	}
	ret = kstrtol(&userData, 10, &filter_value);
	if (ret) {
		pr_err("bad value from user buffer\n");
		return ret;
	}
	return len;
}
#endif /* DO_PROC */

static int __init mod_init(void)
{
	int ret;
	#ifdef DO_PROC
	skb_filter = create_proc_entry(skb_filter_name, 0644, NULL);
	if (IS_ERR(skb_filter)) {
		pr_err("could not create proc entry\n");
		ret = PTR_ERR(skb_filter);
		goto error;
	}
	skb_filter->read_proc = skb_read;
	skb_filter->write_proc = skb_write;
	/* skb_filter->owner = THIS_MODULE; */
	#endif /* DO_PROC */

	/* Netfilter hook information, specify where and when we get the SKB */
	nfho.hook = hook_func;
	nfho.hooknum = NF_INET_PRE_ROUTING;
	nfho.pf = PF_INET;
	nfho.priority = NF_IP_PRI_LAST;
	ret = nf_register_net_hook(NULL, &nfho);
	if (ret) {
		pr_err("could not register netfilter hook\n");
		goto error_proc;
	}

	pr_info("Registering SK Parse Module\n");

error_proc:
	#ifdef DO_PROC
	remove_proc_entry(skb_filter_name, NULL);
	#endif /* DO_PROC */
/* error: */
	return ret;
}

static void __exit mod_exit(void)
{
	nf_unregister_net_hook(NULL, &nfho);
	#ifdef DO_PROC
	remove_proc_entry(skb_filter_name, NULL);
	#endif /* DO_PROC */
	pr_info("Unregistered the SK Parse Module\n");
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
