/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/percpu.h>
#include <linux/netdevice.h>
#include <net/net_namespace.h>
#ifdef CONFIG_SYSCTL
#include <linux/sysctl.h>
#endif

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_l3proto.h>
#include <net/netfilter/nf_conntrack_l4proto.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_acct.h>
#include <net/netfilter/nf_conntrack_zones.h>

MODULE_LICENSE("GPL");

/* lan¿ÚipµØÖ· */
__be32 utt_local_address __read_mostly;
char utt_local_ifname[IFNAMSIZ] = "eth0";
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
__be32 utt_wifi_online __read_mostly = 1U;
#endif
#ifdef CONFIG_PROC_FS
int
print_tuple(struct seq_file *s, const struct nf_conntrack_tuple *tuple,
            const struct nf_conntrack_l3proto *l3proto,
            const struct nf_conntrack_l4proto *l4proto)
{
	return l3proto->print_tuple(s, tuple) || l4proto->print_tuple(s, tuple);
}
EXPORT_SYMBOL_GPL(print_tuple);

struct ct_iter_state {
	struct seq_net_private p;
	unsigned int bucket;
};

static struct hlist_nulls_node *ct_get_first(struct seq_file *seq)
{
	struct net *net = seq_file_net(seq);
	struct ct_iter_state *st = seq->private;
	struct hlist_nulls_node *n;

	for (st->bucket = 0;
	     st->bucket < net->ct.htable_size;
	     st->bucket++) {
		n = rcu_dereference(net->ct.hash[st->bucket].first);
		if (!is_a_nulls(n))
			return n;
	}
	return NULL;
}

static struct hlist_nulls_node *ct_get_next(struct seq_file *seq,
				      struct hlist_nulls_node *head)
{
	struct net *net = seq_file_net(seq);
	struct ct_iter_state *st = seq->private;

	head = rcu_dereference(head->next);
	while (is_a_nulls(head)) {
		if (likely(get_nulls_value(head) == st->bucket)) {
			if (++st->bucket >= net->ct.htable_size)
				return NULL;
		}
		head = rcu_dereference(net->ct.hash[st->bucket].first);
	}
	return head;
}

static struct hlist_nulls_node *ct_get_idx(struct seq_file *seq, loff_t pos)
{
	struct hlist_nulls_node *head = ct_get_first(seq);

	if (head)
		while (pos && (head = ct_get_next(seq, head)))
			pos--;
	return pos ? NULL : head;
}

static void *ct_seq_start(struct seq_file *seq, loff_t *pos)
	__acquires(RCU)
{
	rcu_read_lock();
	return ct_get_idx(seq, *pos);
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	return ct_get_next(s, v);
}

static void ct_seq_stop(struct seq_file *s, void *v)
	__releases(RCU)
{
	rcu_read_unlock();
}

/* return 0 on success, 1 in case of error */
static int ct_seq_show(struct seq_file *s, void *v)
{
	struct nf_conntrack_tuple_hash *hash = v;
	struct nf_conn *ct = nf_ct_tuplehash_to_ctrack(hash);
	const struct nf_conntrack_l3proto *l3proto;
	const struct nf_conntrack_l4proto *l4proto;
	int ret = 0;

	NF_CT_ASSERT(ct);
	if (unlikely(!atomic_inc_not_zero(&ct->ct_general.use)))
		return 0;

	/* we only want to print DIR_ORIGINAL */
	if (NF_CT_DIRECTION(hash))
		goto release;

	l3proto = __nf_ct_l3proto_find(nf_ct_l3num(ct));
	NF_CT_ASSERT(l3proto);
	l4proto = __nf_ct_l4proto_find(nf_ct_l3num(ct), nf_ct_protonum(ct));
	NF_CT_ASSERT(l4proto);

	ret = -ENOSPC;
	if (seq_printf(s, "%-8s %u %-8s %u %ld ",
		       l3proto->name, nf_ct_l3num(ct),
		       l4proto->name, nf_ct_protonum(ct),
		       timer_pending(&ct->timeout)
		       ? (long)(ct->timeout.expires - jiffies)/HZ : 0) != 0)
		goto release;

	if (l4proto->print_conntrack && l4proto->print_conntrack(s, ct))
		goto release;

	if (print_tuple(s, &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple,
			l3proto, l4proto))
		goto release;

	if (seq_print_acct(s, ct, IP_CT_DIR_ORIGINAL))
		goto release;

	if (!(test_bit(IPS_SEEN_REPLY_BIT, &ct->status)))
		if (seq_printf(s, "[UNREPLIED] "))
			goto release;

	if (print_tuple(s, &ct->tuplehash[IP_CT_DIR_REPLY].tuple,
			l3proto, l4proto))
		goto release;

	if (seq_print_acct(s, ct, IP_CT_DIR_REPLY))
		goto release;

	if (test_bit(IPS_ASSURED_BIT, &ct->status))
		if (seq_printf(s, "[ASSURED] "))
			goto release;

	if (seq_printf(s, "utt_oifindex=%u ", ct->utt_oifindex))
		goto release;
#if defined(CONFIG_NF_CONNTRACK_MARK)
	if (seq_printf(s, "mark=%u ", ct->mark))
		goto release;
#endif

#ifdef CONFIG_NF_CONNTRACK_SECMARK
	if (seq_printf(s, "secmark=%u ", ct->secmark))
		goto release;
#endif

#ifdef CONFIG_NF_CONNTRACK_ZONES
	if (seq_printf(s, "zone=%u ", nf_ct_zone(ct)))
		goto release;
#endif

#if defined(CONFIG_NETFILTER_XT_MATCH_LAYER7) || defined(CONFIG_NETFILTER_XT_MATCH_LAYER7_MODULE)
	if(ct->layer7.app_proto &&
           seq_printf(s, "l7proto=%s ", ct->layer7.app_proto))
//		return -ENOSPC;
		goto release;
#endif

#if defined(CONFIG_NETFILTER_XT_TARGET_UTTINDENTIFY) || defined(CONFIG_NETFILTER_XT_TARGET_UTTWEBIND)
	if (seq_printf(s, "uttPriF=%u ", ct->utt_app.priFlag)) {
	    goto release;
	}
	if (seq_printf(s, "uttSecF=%u ", ct->utt_app.secFlag)) {
	    goto release;
	}
	if (seq_printf(s, "uttIdF=%u ", ct->utt_app.uttIdFlag)) {
	    goto release;
	}
#endif

#if 0
	if (seq_printf(s, "stat=%u ", ct->utt_stat.counted)) {
	    goto release;
	}
#endif

#if defined(CONFIG_UTT_CONNLIMIT)
	if (seq_printf(s, "clRecord=%u ", atomic_read(&ct->clRecord))) {
//	    return -ENOSPC;
	    goto release;
	}
#endif

	if (seq_printf(s, "use=%u\n", atomic_read(&ct->ct_general.use)))
		goto release;

	ret = 0;
release:
	nf_ct_put(ct);
	return 0;
}

static const struct seq_operations ct_seq_ops = {
	.start = ct_seq_start,
	.next  = ct_seq_next,
	.stop  = ct_seq_stop,
	.show  = ct_seq_show
};

static int ct_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &ct_seq_ops,
			sizeof(struct ct_iter_state));
}

static const struct file_operations ct_file_ops = {
	.owner   = THIS_MODULE,
	.open    = ct_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release_net,
};

#if defined (CONFIG_UTT_CONNLIMIT)
struct cl_iter_state {
	struct seq_net_private p;
	unsigned int bucket;
};

static struct list_head *cl_get_first(struct seq_file *seq)
{
	struct net *net = seq_file_net(seq);
	struct cl_iter_state *st = seq->private;

	for (st->bucket = 0;
	     st->bucket < UTT_NF_STAT_HASH_MAX;
	     st->bucket++) {
		if (!list_empty(&nf_uttstat_hash[st->bucket]))
			return nf_uttstat_hash[st->bucket].next;
	}
	return NULL;
}

static struct list_head *cl_get_next(struct seq_file *seq, struct list_head *head)
{
	struct net *net = seq_file_net(seq);
	struct cl_iter_state *st = seq->private;

	head = head->next;
	while (head == &nf_uttstat_hash[st->bucket]) {
		if (++st->bucket >= UTT_NF_STAT_HASH_MAX)
			return NULL;
		head = nf_uttstat_hash[st->bucket].next;
	}
	return head;
}

static struct list_head *cl_get_idx(struct seq_file *seq, loff_t pos)
{
	struct list_head *head = cl_get_first(seq);

	if (head)
		while (pos && (head = cl_get_next(seq, head)))
			pos--;
	return pos ? NULL : head;
}

static void *cl_seq_start(struct seq_file *seq, loff_t *pos)
{
	read_lock_bh(&nf_utt_connlimit_lock);
	return cl_get_idx(seq, *pos);
}

static void *cl_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	return cl_get_next(s, v);
}

static void cl_seq_stop(struct seq_file *s, void *v)
{
	read_unlock_bh(&nf_utt_connlimit_lock);
}

/* return 0 on success, 1 in case of error */
static int cl_seq_show(struct seq_file *s, void *v)
{
    const nf_uttstat_hash_node *node = v;
    struct cl_iter_state *st = s->private;

    seq_printf(s, "hashnum=%u, ip=%u.%u.%u.%u all=%u tcp=%u udp=%u icmp=%u\n", 
		st->bucket, NIPQUAD(node->conn_stat.ip), node->conn_stat.itemCnt, 
		node->conn_stat.itemTcpCnt, node->conn_stat.itemUdpCnt,
		node->conn_stat.itemIcmpCnt);

    return 0;
}

static struct seq_operations cl_seq_ops = {
	.start = cl_seq_start,
	.next  = cl_seq_next,
	.stop  = cl_seq_stop,
	.show  = cl_seq_show
};

static int cl_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &cl_seq_ops,
			sizeof(struct cl_iter_state));
}
#if 0
static int cl_open(struct inode *inode, struct file *file)
{
	struct seq_file *seq;
	struct cl_iter_state *st;
	int ret;

	st = kmalloc(sizeof(struct cl_iter_state), GFP_KERNEL);
	if (st == NULL)
		return -ENOMEM;
	ret = seq_open(file, &cl_seq_ops);
	if (ret)
		goto out_free;
	seq          = file->private_data;
	seq->private = st;
	memset(st, 0, sizeof(struct cl_iter_state));
	return ret;
out_free:
	kfree(st);
	return ret;
}
#endif

static const struct file_operations cl_file_ops = {
	.owner   = THIS_MODULE,
	.open    = cl_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release_private,
};

static int utt_conn_global_read( char *page, char **start, off_t off,
                           int count, int *eof, void *data) {
    int len = 0;
    len += sprintf(page + len, "host stat node alloc: %u\n", utt_connlimit_alloc_cnt);

    return len;
}

static int cl_sysctl_switch(struct ctl_table *ctl, int write,
				void __user *buffer,
				size_t *lenp, loff_t *ppos)

{
    int val = utt_connlimit_switch;
    int ret;
    struct net *net;

    ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

    if (write && (val != utt_connlimit_switch) && (utt_connlimit_switch == 0)) {
	/* open -> close, free stat node */
	net = (struct net *)ctl->extra1;
	utt_connlimit_hash_node_free(net);
    }

    return ret;
}

static int cl_sysctl_switch_strategy(ctl_table *table,
				   void __user *oldval,
				   size_t __user *oldlenp,
				   void __user *newval,
				   size_t newlen) 
{
    struct net *net;
    int new, old;
    if ((!newval) || (newlen != sizeof(int)) || 
	(!oldval) || (!oldlenp) || (*oldlenp != sizeof(int))) {
	return -EINVAL;
    }

    if (get_user(new, (int __user *)newval)) {
	return -EFAULT;
    }

    if (get_user(old, (int __user *)oldval)) {
	return -EFAULT;
    }

    if ((new != old) && (new == 0)) {
	net = (struct net *)table->extra1;
	utt_connlimit_hash_node_free(net);
    }

    return 0;
}
#endif /* end CONFIG_UTT_CONNLIMIT */

static void *ct_cpu_seq_start(struct seq_file *seq, loff_t *pos)
{
	struct net *net = seq_file_net(seq);
	int cpu;

	if (*pos == 0)
		return SEQ_START_TOKEN;

	for (cpu = *pos-1; cpu < nr_cpu_ids; ++cpu) {
		if (!cpu_possible(cpu))
			continue;
		*pos = cpu + 1;
		return per_cpu_ptr(net->ct.stat, cpu);
	}

	return NULL;
}

static void *ct_cpu_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct net *net = seq_file_net(seq);
	int cpu;

	for (cpu = *pos; cpu < nr_cpu_ids; ++cpu) {
		if (!cpu_possible(cpu))
			continue;
		*pos = cpu + 1;
		return per_cpu_ptr(net->ct.stat, cpu);
	}

	return NULL;
}

static void ct_cpu_seq_stop(struct seq_file *seq, void *v)
{
}

static int ct_cpu_seq_show(struct seq_file *seq, void *v)
{
	struct net *net = seq_file_net(seq);
	unsigned int nr_conntracks = atomic_read(&net->ct.count);
	const struct ip_conntrack_stat *st = v;

	if (v == SEQ_START_TOKEN) {
		seq_printf(seq, "entries  searched found new invalid ignore delete delete_list insert insert_failed drop early_drop icmp_error  expect_new expect_create expect_delete search_restart\n");
		return 0;
	}

	seq_printf(seq, "%08x  %08x %08x %08x %08x %08x %08x %08x "
			"%08x %08x %08x %08x %08x  %08x %08x %08x %08x\n",
		   nr_conntracks,
		   st->searched,
		   st->found,
		   st->new,
		   st->invalid,
		   st->ignore,
		   st->delete,
		   st->delete_list,
		   st->insert,
		   st->insert_failed,
		   st->drop,
		   st->early_drop,
		   st->error,

		   st->expect_new,
		   st->expect_create,
		   st->expect_delete,
		   st->search_restart
		);
	return 0;
}

static const struct seq_operations ct_cpu_seq_ops = {
	.start	= ct_cpu_seq_start,
	.next	= ct_cpu_seq_next,
	.stop	= ct_cpu_seq_stop,
	.show	= ct_cpu_seq_show,
};

static int ct_cpu_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &ct_cpu_seq_ops,
			    sizeof(struct seq_net_private));
}

static const struct file_operations ct_cpu_seq_fops = {
	.owner	 = THIS_MODULE,
	.open	 = ct_cpu_seq_open,
	.read	 = seq_read,
	.llseek	 = seq_lseek,
	.release = seq_release_net,
};

static int nf_conntrack_standalone_init_proc(struct net *net)
{
	struct proc_dir_entry *pde;

#if defined(CONFIG_UTT_CONNLIMIT)
	pde = create_proc_entry("utt_host_connlimit_global", S_IRUGO, net->proc_net_stat);
	if (!pde) {
	    goto out_nf_conntrack;
	}
	pde->read_proc = utt_conn_global_read;

	pde = proc_net_fops_create(net, "nf_host_connlimit", 0440, &cl_file_ops);
	if (!pde) goto out_connlimit_global_nf_conntrack;
#endif

	pde = proc_net_fops_create(net, "nf_conntrack", 0440, &ct_file_ops);
	if (!pde)
#if defined(CONFIG_UTT_CONNLIMIT)
		goto out_host_connlimit_nf_conntrack;
#else
		goto out_nf_conntrack;
#endif

	pde = proc_create("nf_conntrack", S_IRUGO, net->proc_net_stat,
			  &ct_cpu_seq_fops);
	if (!pde)
		goto out_stat_nf_conntrack;
	return 0;

#if defined(CONFIG_UTT_CONNLIMIT)
out_host_connlimit_nf_conntrack:
	proc_net_remove(net, "nf_host_connlimit");
out_connlimit_global_nf_conntrack:
	remove_proc_entry("utt_host_connlimit_global", net->proc_net_stat);
#endif
out_stat_nf_conntrack:
	proc_net_remove(net, "nf_conntrack");
out_nf_conntrack:
	return -ENOMEM;
}

static void nf_conntrack_standalone_fini_proc(struct net *net)
{
	remove_proc_entry("nf_conntrack", net->proc_net_stat);
	proc_net_remove(net, "nf_conntrack");
#if defined(CONFIG_UTT_CONNLIMIT)
	proc_net_remove(net, "nf_host_connlimit");
	remove_proc_entry("utt_host_connlimit_global", net->proc_net_stat);
#endif
}
#else
static int nf_conntrack_standalone_init_proc(struct net *net)
{
	return 0;
}

static void nf_conntrack_standalone_fini_proc(struct net *net)
{
}
#endif /* CONFIG_PROC_FS */

/* Sysctl support */

#ifdef CONFIG_SYSCTL
/* Log invalid packets of a given protocol */
static int log_invalid_proto_min = 0;
static int log_invalid_proto_max = 255;

static struct ctl_table_header *nf_ct_netfilter_header;

static ctl_table nf_ct_sysctl_table[] = {
	{
		.procname	= "nf_conntrack_max",
		.data		= &nf_conntrack_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{
		.procname	= "nf_conntrack_count",
		.data		= &init_net.ct.count,
		.maxlen		= sizeof(int),
		.mode		= 0444,
		.proc_handler	= proc_dointvec,
	},
	{
		.procname       = "nf_conntrack_buckets",
		.data           = &init_net.ct.htable_size,
		.maxlen         = sizeof(unsigned int),
		.mode           = 0444,
		.proc_handler   = proc_dointvec,
	},
	{
		.procname	= "nf_conntrack_checksum",
		.data		= &init_net.ct.sysctl_checksum,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{
		.procname	= "nf_conntrack_log_invalid",
		.data		= &init_net.ct.sysctl_log_invalid,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &log_invalid_proto_min,
		.extra2		= &log_invalid_proto_max,
	},
	{
		.procname	= "nf_conntrack_expect_max",
		.data		= &nf_ct_expect_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},

#if defined(CONFIG_UTT_CONNLIMIT)
	{
		.procname	= "utt_connlimit_switch",
		.data		= &utt_connlimit_switch,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &cl_sysctl_switch,
	},
	{
		.procname	= "utt_host_conn_max",
		.data		= &utt_host_conn_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
	{
		.procname	= "utt_host_tcp_conn_max",
		.data		= &utt_host_tcp_conn_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
	{
		.procname	= "utt_host_udp_conn_max",
		.data		= &utt_host_udp_conn_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
	{
		.procname	= "utt_host_icmp_conn_max",
		.data		= &utt_host_icmp_conn_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
#if 0
	{
		.procname	= "utt_conn_lan_ip",
		.data		= &utt_conn_lan_ip,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
#endif
#endif /* CONFIG_UTT_CONNLIMIT */
	{
		.procname	= "utt_local_address",
		.data		= &utt_local_address,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
	{
		.procname	= "utt_local_ifname",
		.data		= utt_local_ifname,
		.maxlen		= IFNAMSIZ,
		.mode		= 0644,
		.proc_handler	= &proc_dostring,
	},
#ifdef CONFIG_UTT_WIFIDOG_REDIRECT
	{
		.procname	= "utt_wifi_online",
		.data		= &utt_wifi_online,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
#endif

	{ }
};

#define NET_NF_CONNTRACK_MAX 2089

static ctl_table nf_ct_netfilter_table[] = {
	{
		.procname	= "nf_conntrack_max",
		.data		= &nf_conntrack_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{ }
};

static struct ctl_path nf_ct_path[] = {
	{ .procname = "net", },
	{ }
};

static int nf_conntrack_standalone_init_sysctl(struct net *net)
{
	struct ctl_table *table;

	if (net_eq(net, &init_net)) {
		nf_ct_netfilter_header =
		       register_sysctl_paths(nf_ct_path, nf_ct_netfilter_table);
		if (!nf_ct_netfilter_header)
			goto out;
	}

	table = kmemdup(nf_ct_sysctl_table, sizeof(nf_ct_sysctl_table),
			GFP_KERNEL);
	if (!table)
		goto out_kmemdup;

	table[1].data = &net->ct.count;
	table[2].data = &net->ct.htable_size;
	table[3].data = &net->ct.sysctl_checksum;
	table[4].data = &net->ct.sysctl_log_invalid;
#if defined (CONFIG_UTT_CONNLIMIT)
	table[6].extra1 = net;
#endif

	net->ct.sysctl_header = register_net_sysctl_table(net,
					nf_net_netfilter_sysctl_path, table);
	if (!net->ct.sysctl_header)
		goto out_unregister_netfilter;

	return 0;

out_unregister_netfilter:
	kfree(table);
out_kmemdup:
	if (net_eq(net, &init_net))
		unregister_sysctl_table(nf_ct_netfilter_header);
out:
	printk(KERN_ERR "nf_conntrack: can't register to sysctl.\n");
	return -ENOMEM;
}

static void nf_conntrack_standalone_fini_sysctl(struct net *net)
{
	struct ctl_table *table;

	if (net_eq(net, &init_net))
		unregister_sysctl_table(nf_ct_netfilter_header);
	table = net->ct.sysctl_header->ctl_table_arg;
	unregister_net_sysctl_table(net->ct.sysctl_header);
	kfree(table);
}
#else
static int nf_conntrack_standalone_init_sysctl(struct net *net)
{
	return 0;
}

static void nf_conntrack_standalone_fini_sysctl(struct net *net)
{
}
#endif /* CONFIG_SYSCTL */

static int nf_conntrack_net_init(struct net *net)
{
	int ret;

	ret = nf_conntrack_init(net);
	if (ret < 0)
		goto out_init;
	ret = nf_conntrack_standalone_init_proc(net);
	if (ret < 0)
		goto out_proc;
	net->ct.sysctl_checksum = 1;
	net->ct.sysctl_log_invalid = 0;
	ret = nf_conntrack_standalone_init_sysctl(net);
	if (ret < 0)
		goto out_sysctl;
	return 0;

out_sysctl:
	nf_conntrack_standalone_fini_proc(net);
out_proc:
	nf_conntrack_cleanup(net);
out_init:
	return ret;
}

static void nf_conntrack_net_exit(struct net *net)
{
	nf_conntrack_standalone_fini_sysctl(net);
	nf_conntrack_standalone_fini_proc(net);
	nf_conntrack_cleanup(net);
}

static struct pernet_operations nf_conntrack_net_ops = {
	.init = nf_conntrack_net_init,
	.exit = nf_conntrack_net_exit,
};

static int __init nf_conntrack_standalone_init(void)
{
	return register_pernet_subsys(&nf_conntrack_net_ops);
}

static void __exit nf_conntrack_standalone_fini(void)
{
	unregister_pernet_subsys(&nf_conntrack_net_ops);
}

module_init(nf_conntrack_standalone_init);
module_exit(nf_conntrack_standalone_fini);

/* Some modules need us, but don't depend directly on any symbol.
   They should call this. */
void need_conntrack(void)
{
}
EXPORT_SYMBOL_GPL(need_conntrack);
