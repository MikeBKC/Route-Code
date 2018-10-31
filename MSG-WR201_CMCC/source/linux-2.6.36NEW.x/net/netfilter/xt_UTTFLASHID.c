/*
 * This is a module which is used for setting the MSS option in TCP packets.
 *
 * Copyright (C) 2000 Marc Boucher <marc@mbsi.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
/* TODO: 
 * 统计是否需要加信号量?
 *
 */
#include <linux/types.h>
#include <net/sock.h>
#include <linux/autoconf.h>
#include <net/netlink.h>

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <net/netfilter/nf_conntrack.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter_ipv4/utt_netfilter.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("GET UTT FLASH ID module");
MODULE_ALIAS("UTTFLASHID");


extern int raspi_flashid_utt(char *buf_jedec,unsigned int len,unsigned int flag);

static struct xt_target xt_uttflashid_reg[] = {
	{
		.name		= "UTTFLASHID",
		.me		= THIS_MODULE,
	},
};

/*
 * /proc/uttFlashId读取函数
 */
int uttFlashIdRead( char *page, char **start, off_t off,
	int count, int *eof, void *data) {
    int len = 0;
    /*输入到page中就可以得到
     *调用读取函数
     * */
    unsigned char buf[8];
    unsigned char buf_flash[4];
    memset(buf,0,sizeof(buf));
    memset(buf_flash,0,sizeof(buf_flash));
    raspi_flashid_utt(buf,8,1);
    raspi_flashid_utt(buf_flash,4,0);
    len = sprintf(page,"uttflashFactory: %02x%02x%02x%02x\nuttFlashId: %02x%02x%02x%02x %02x%02x%02x%02x\n",buf_flash[0],buf_flash[1],buf_flash[2],buf_flash[3],buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
     return len;
}

static int __init xt_uttflashid_init(void)
{
    static struct proc_dir_entry *entry;

    entry = create_proc_entry( "uttFlashId", 0644, NULL );
    if (!entry) {
	printk("uttFlashId: create failed\n");
    } else {
	entry->read_proc = uttFlashIdRead;
    }

    return xt_register_targets(xt_uttflashid_reg, ARRAY_SIZE(xt_uttflashid_reg));
}

static void __exit xt_uttflashid_fini(void)
{
    remove_proc_entry("uttFlashId", NULL);
    xt_unregister_targets(xt_uttflashid_reg, ARRAY_SIZE(xt_uttflashid_reg));
}

module_init(xt_uttflashid_init);
module_exit(xt_uttflashid_fini);
