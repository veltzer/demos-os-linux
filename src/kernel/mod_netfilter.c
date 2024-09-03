// SPDX-License-Identifier: GPL-2.0
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
#include <linux/uaccess.h> /* For copy_from_user */

/*
 * This is a pretty simple example of a netfilter module.
 *
 * References:
 * http://www.lainoox.com/netfilter-hook-packet-filter/
 */

MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Basic netfilter module");
MODULE_LICENSE("GPL");

static struct nf_hook_ops nfho; /* net filter hook option struct */

static unsigned int hook_func(
	void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state
)
{
	struct iphdr *ip_header; /* IP header struct */

	if (!skb)
		return NF_ACCEPT;
	ip_header = ip_hdr(skb);
	if (ip_header) {
		pr_info(
			"Packet intercepted: Source IP = %pI4, Dest IP = %pI4\n",
			&ip_header->saddr,
			&ip_header->daddr
		);
	}
	return NF_ACCEPT;
}

static int __init mod_init(void)
{
	int ret;
	/* Netfilter hook information, specify where and when we get the SKB */
	nfho.hook = hook_func;
	nfho.pf = PF_INET;
	nfho.hooknum = NF_INET_PRE_ROUTING;
	nfho.priority = NF_IP_PRI_LAST;
	ret = nf_register_net_hook(&init_net, &nfho);
	if (ret) {
		pr_err("could not register netfilter hook\n");
		goto error;
	}

	pr_info("Netfilter module loaded.\n");

error:
	return ret;
}

static void __exit mod_exit(void)
{
	nf_unregister_net_hook(&init_net, &nfho);
	pr_info("Netfilter module unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);
