/*
   syslog_nat( LOG_LOCAL1, LEVEL_INFO, syslogMsg);
 * This is a module which is used for UTT LOG.
 *
 * Copyright (C) 2000 Marc Boucher <marc@mbsi.ca>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/utt_filter_kernel.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_tcpudp.h>
#include <linux/netfilter/xt_UTTLOG.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wang bingrong<ff0619@gmail.com>");
MODULE_DESCRIPTION("x_tables UTT LOG modification module");
MODULE_ALIAS("ipt_UTTLOG");

extern __u8 *bm_strstr(__u8 *y, int n, __u8 *x, int m);
#if 1
/* 当前可以记录log的数组位置 */
static int current_rec_num;
/* 记录日志的数组, 目前共400条 */
static UttErrLog utt_err_log[NUM_OF_ERR_LOG];
#endif

static char DATA[] = "DATA"; 
static char Data[] = "Data";
static char smtp_354[] = "354";
#if 1
/* OK */
static __u8 msn_OK[] = {
    0x4f, 0x4b
};
#endif
/* Host */
static __u8 HTTP_HOST[] = {
    0x48, 0x6f, 0x73, 0x74, 0x3a
};
/* .*/
static __u8 GET_DOT[] = {
    0x2e
};

/* Referer */
static __u8 URL_REFERER[] = {
    0x52, 0x65, 0x66, 0x65, 0x72, 0x65, 0x72 
};

/*
 * pop and smtp argments
 */
/* RETR */
static __u8 POP_RETR[] = {
    0x52, 0x45, 0x54, 0x52
};
/* Received */
static __u8 pop_Received[] = {
    0x52, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x64
};
/*content-Disposition: attachment*/
static __u8 SMTP_ATTACH[] = {
    0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x44, 0x69, 0x73, 0x70,
    0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3a 
};



/* To: */
static __u8 MAIL_TO[] = {
    0x54, 0x6f, 0x3a
};
/* From: */
static __u8 MAIL_FROM[] = {
    0x46, 0x72, 0x6f, 0x6d, 0x3a
};
/* < */
static __u8 MAIL_LEFT_BRACE[] = {
    0x3c
};
/* Subject: */
static __u8 MAIL_SUBJECT[] = {
    0x53, 0x75, 0x62, 0x6a, 0x65, 0x63, 0x74
};
/* GET */
static __u8 WEB_GET[] = {
    0x47, 0x45, 0x54
};
#if CONFIG_XPORT_LOG  
/* WEIBO_TEXT */
static __u8 WEIBO_TEXT[] = {
     0x74, 0x65, 0x78,0x74,0x3d
};
static __u8 WEIBO_PIC_ID[] = {
    0x26, 0x70, 0x69, 0x63,0x5F,0x69,0x64,0x3D 
};
static __u8 WEIBO_CENNECTION[] = {
    0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E,0x3A
};
static __u8 WEIBO_NAME[] = {
    0x6E, 0x61, 0x6D, 0x65, 0x3D 
};
#endif
/* asp, aspx, html, htm, php, jsp, shtml */
static char GET_ASP[] = "asp";
static char GET_HTM[] = "htm";
static char GET_PHP[] = "php";
static char GET_JSP[] = "jsp";
static char GET_SHTML[] = "shtml";


static int uttlog_identify(struct sk_buff *skb, struct xt_uttlog_info *info, UttNfHashConf *uttNfHashConf);
static void uttlog_notice(int flag); 
static int uttlog_web_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev,__u32 type);
static int uttlog_web_parse_identify(IP_HDR *iph, __u8* httpData, int datalen,__u32 forum,struct in_device* in_dev);
static int uttlog_qq_udp_identify(IP_HDR *iph, UDP_HDR *udp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data, struct in_device *in_dev);
static int uttlog_qq_tcp_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev);
static int uttlog_msn_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev);
static int uttlog_msn_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev);
static int uttlog_smtp_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* smtpData,struct in_device* in_dev);
static int uttlog_smtp_parse_identify(IP_HDR *iph, __u8 *smtpData, int datalen,struct in_device* in_dev);
static int uttlog_pop_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* popData,struct in_device* in_dev);
static int uttlog_pop_parse_identify(IP_HDR *iph, __u8 *popData, int datalen,struct in_device* in_dev);
#if CONFIG_XPORT_LOG  
static int uttlog_weibo_parse_identify(IP_HDR *iph, __u8* httpData, int datalen,struct in_device *in_dev);
static void urldecode(char *p);
static int uttlog_forum_parse_identify(IP_HDR *iph, __u8* httpData, int datalen,__u32 forum,struct in_device* in_dev);
#endif
#if 0
#define SWORD_PRINTK(fmt, args...)		printk_audit(fmt, ## args)
#else
#define SWORD_PRINTK(fmt, args...)
#endif
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printk(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#define SWORD_XPORT_DEBUG 0
#if SWORD_XPORT_DEBUG
#define SWORD_XPORT(fmt, args...)		printk_audit(fmt, ## args)
#else
#define SWORD_XPORT(fmt, args...)
#endif
#define SWORD_DEBUGK(fmt, args...)		printk_xport(fmt, ## args)

#define SYSLOG_ENABLE 1
#if SYSLOG_ENABLE
#define SYSLOG_PRINTK(fmt, args...)		printk_audit(fmt, ## args)
#else
#define SYSLOG_PRINTK(fmt, args...)
#endif
#if 1
/*
 * 记录log到静态数组中, 并添加记录时间，循环记录
 * 输入参数: log    -	log msg
 *	    sec	    -	当前时间，距1970-1-1秒数
 */
extern void recordErrLog(char *log, __u32 sec) {
    UttErrLog *logEntry;

    /* 防错 */
    if (current_rec_num >= NUM_OF_ERR_LOG) {
	current_rec_num = START_OF_ERR_LOG;
    }

    logEntry = &utt_err_log[current_rec_num];
    memset(logEntry, 0, sizeof(UttErrLog));

#if 0
    strncpy(logEntry->msg, log, MAX_ERROR_LOG_MSG_LENGTH);
#endif
    snprintf(logEntry->msg, MAX_ERROR_DATELOG_LENGTH, "%u %s", sec, log);

    /* 下一个 */
    current_rec_num++;

    /* 越界，从头开始 */
    if (current_rec_num >= NUM_OF_ERR_LOG) {
	current_rec_num = START_OF_ERR_LOG;
    }

    return;
}

/*
 * /proc/uttErrLog读取函数
 */
extern int uttErrLogRead( char *page, char **start, off_t off,
                           int count, int *eof, void *data) {
    int i, len = 0, index;
    UttErrLog *logEntry;

    /* 从当前要记录的位置开始遍历 */
    index = current_rec_num;
    
    /* 400条都遍历 */
    for (i = 0; i < NUM_OF_ERR_LOG; i++) {
	if (index >= NUM_OF_ERR_LOG) {
	    index = START_OF_ERR_LOG;
	}
	logEntry = &utt_err_log[index];

	if (logEntry->msg[0] != '\0') {
	    len += sprintf(page + len, "%s\n", logEntry->msg);
	}

	index++;
    }
    return len;
}

/*
 * 清除log
 */
static void clearErrLog(void) {
    memset(utt_err_log, 0, sizeof(utt_err_log));
    current_rec_num = START_OF_ERR_LOG;
    return;
}

/*
 * 接收log消息
 */
extern void uttNlLog(struct sk_buff *skb) {
    struct nlmsghdr *nlh;
    UttNlNfGeneral *nlData;
    nlh = nlmsg_hdr(skb);
    nlData = (UttNlNfGeneral *)NLMSG_DATA(nlh);

    switch(nlData->confType) {
	case UTT_NFCONF_DEL:
	    clearErrLog();
	    break;
	default:
	    break;
    }
    return;
}

/*===========proc==================*/
static void *uttlog_seq_start(struct seq_file *seq, loff_t *pos)
{
    int index;
    if (*pos >= NUM_OF_ERR_LOG) {
	return NULL;
    }

    index = *pos + current_rec_num;
    if (index >= NUM_OF_ERR_LOG) {
	index -= NUM_OF_ERR_LOG; 
    }

    return utt_err_log + index;
}
static void *uttlog_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
    int index;
    (*pos)++;
    if (*pos >= NUM_OF_ERR_LOG) {
	return NULL;
    }
    
    index = *pos + current_rec_num;
    if (index >= NUM_OF_ERR_LOG) {
	index -= NUM_OF_ERR_LOG; 
    }

    return utt_err_log + index;
}
static int uttlog_seq_show(struct seq_file *seq, void *v)
{
    UttErrLog *logEntry = (UttErrLog *)v;
    if (logEntry->msg[0] != '\0') {
	seq_printf(seq, "%s\n", logEntry->msg);
    }
    return 0;
}

static void uttlog_seq_stop(struct seq_file *s, void *v)
{

#if 0
    //    statsPerIpIndex = 0;
    rcu_read_unlock();
#endif
    return;
}
static const struct seq_operations uttlog_seq_ops = {
	.start  = uttlog_seq_start,
	.next   = uttlog_seq_next,
	.stop   = uttlog_seq_stop,
	.show   = uttlog_seq_show,
};

static int uttlog_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &uttlog_seq_ops);
#if 0
	return seq_open(file, &statsPerIp_seq_ops);
#endif
}

static const struct file_operations uttlog_seq_fops = {
	.owner		= THIS_MODULE,
	.open           = uttlog_seq_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release	= seq_release,
#if 0
	.release	= seq_release
#endif
};

#if 0
static int __net_init uttlog_net_init(struct net *net)
{
	if (!proc_net_fops_create(net, "uttErrLog", S_IRUGO, &uttlog_seq_fops))
		return -ENOMEM;
	return 0;
}
static void __net_exit uttlog_net_exit(struct net *net)
{
	proc_net_remove(net, "uttErrLog");
}

static struct pernet_operations uttlog_net_ops = {
	.init = uttlog_net_init,
	.exit = uttlog_net_exit,
};
#endif

/*
 * 注册proc文件
 */
static void uttLogProcInit(void) {
    static struct proc_dir_entry *entry;

    entry = create_proc_entry( "uttErrLog", 0644, NULL );
    if (!entry) {
	printk("uttErrLog: create failed\n");
    } else {
	entry->proc_fops = &uttlog_seq_fops;
    }

#if 0
    register_pernet_subsys(&uttlog_net_ops);
#endif
    return;
}

/*===========end prof============*/
#endif

asmlinkage int printk_xport(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return r;
}
/*
 *根据识别的类型，发送syslog信息
 */
static void uttlog_notice(int flag) 
{
	switch(flag) {
		/* 注意syslog/syslog信息中需要加上行为审计标识头，用来在页面读取信息是何系统信息加以区分 */
#if 0
		case QQ: syslog/SWORD_PRINTF();
#endif
	}

}
static unsigned int
xt_uttlog_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{
    int flag = 0;
    UttNfHashConf *UttNfHashConf;
    struct xt_uttlog_info *info = (struct xt_uttlog_info *)par->targinfo;
    /* info->sw即为用户空间传过来的参数 */
    UttNfHashConf = uttNfHashConfGetBySkb(skb, 0);
    if(UttNfHashConf != NULL)
    {
	flag = uttlog_identify(skb, info, UttNfHashConf);
	/* 发送通知 */
	uttlog_notice(flag);
    }
    return XT_CONTINUE;
}

static int
xt_uttlog_checkentry4(const struct xt_tgchk_param *par)
{
        return 0;
}


static struct xt_target xt_uttlog_reg[] = {
	{
		.family		= AF_INET,
		.name		= "UTTLOG",
		.checkentry	= xt_uttlog_checkentry4,
		.target		= xt_uttlog_target4,
		.targetsize	= sizeof(struct xt_uttlog_info),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttlog_init(void)
{
    memset(utt_err_log, 0, sizeof(utt_err_log));
    current_rec_num = START_OF_ERR_LOG;

#if 0
    static struct proc_dir_entry *entry;
    memset(utt_err_log, 0, sizeof(utt_err_log));
    current_rec_num = START_OF_ERR_LOG;

    entry = create_proc_entry( "uttErrLog", 0644, NULL );
    if (!entry) {
	printk("uttErrLog: create failed\n");
    } else {
	entry->read_proc = uttErrLogRead;
    }

#endif
#if 1
    uttLogProcInit();
#endif
	return xt_register_targets(xt_uttlog_reg, ARRAY_SIZE(xt_uttlog_reg));
}

static void __exit xt_uttlog_fini(void)
{
	xt_unregister_targets(xt_uttlog_reg, ARRAY_SIZE(xt_uttlog_reg));
}

module_init(xt_uttlog_init);
module_exit(xt_uttlog_fini);

/*
 * skb:数据包
 * info:用户配置信息
 * 返回值：被识别为什么信息
 */
static int uttlog_identify(struct sk_buff *skb,struct xt_uttlog_info *info, UttNfHashConf *uttNfHashConf)
{
    int flag = 0;

    int tcplen = 0, tcpHeadLen = 0, datalen = 0, udplen = 0;
    TCP_HDR *tcp;
    UDP_HDR *udp;
    IP_HDR *iph = ip_hdr(skb);
    __u8 *data;

    struct in_device *in_dev;
    __u16 iph_len = 0; 
    iph_len = ip_hdrlen(skb);
    if(skb->dev != NULL)
    {
    in_dev = __in_dev_get_rcu(skb->dev);
    }

    switch (iph->protocol) 
    {
	case PROT_TCP:
	    tcplen = skb->len - iph_len;
	    tcp = (TCP_HDR *)(skb_network_header(skb) + iph_len); 
	    tcpHeadLen = tcp->doff * 4;
	    datalen = tcplen - tcpHeadLen;
	    data = (__u8 *)((void *)tcp + tcpHeadLen);

            if ((ntohs(tcp->source) == IPPORT_SMTP)
		|| (ntohs(tcp->dest) == IPPORT_SMTP))
	    {
		/*
		 * smtp
		 */
		if(((info->sw & UTT_MAIL_LOG_ENABLE) != 0) && (flag == 0))
		{
		    flag = uttlog_smtp_identify(iph, tcp, uttNfHashConf, datalen, data, in_dev);
		}
	    }
	    else if ((ntohs(tcp->source) == IPPORT_POP3)
		|| (ntohs(tcp->dest) == IPPORT_POP3))
	    {
		/*
		* POP
		 */
		if(((info->sw & UTT_MAIL_LOG_ENABLE) != 0) && (flag == 0))
		{
		    flag = uttlog_pop_identify(iph, tcp, uttNfHashConf, datalen, data, in_dev);
		}
	    }
	    else if ((ntohs(tcp->dest) == IPPORT_MSN)
		|| (ntohs(tcp->source) == IPPORT_MSN))
	    {
		/*
		* msn
		*/
		if(((info->sw & UTT_MSN_LOG_ENABLE) != 0) && (flag == 0))
		{
		    flag = uttlog_msn_identify(iph, tcp, uttNfHashConf, datalen, data,in_dev);
		}
	    }
	    else if ((ntohs(tcp->dest) == IPPORT_SSL)
		|| (ntohs(tcp->dest) == IPPORT_HTTP))
	    {
	        /*
	         * TCP qq
	         */
		if(((info->sw & UTT_QQ_LOG_ENABLE) != 0) && (flag == 0))
		{
		    flag = uttlog_qq_tcp_identify(iph, tcp, uttNfHashConf, datalen, data,in_dev);
		}
		if(isSkbComeFromLan(skb) && (ntohs(tcp->dest) == IPPORT_HTTP) && ((info->sw & (UTT_WEB_LOG_ENABLE | UTT_WEIBO_LOG_ENABLE | UTT_LUNTAN_LOG_ENABLE)) != 0) && (flag == 0))
		{
		    flag = uttlog_web_identify(iph, tcp, uttNfHashConf, datalen, data,in_dev,info->sw);
		}
	    }
	    else
	    {
		/*test*/
	    }
	    break;

	case PROT_UDP:
	    udplen = skb->len - iph_len;
	    udp = (UDP_HDR *)(skb_network_header(skb) + iph_len);
	    datalen = udp->len - sizeof(UDP_HDR);
	    data = (__u8 *)((void *)udp + sizeof(UDP_HDR));

	    if (ntohs(udp->dest) == IPPORT_UDPQQ2)
	    {
		/*
	         * QQ
		 */
		if(((info->sw & UTT_QQ_LOG_ENABLE) != 0) && (flag == 0))
		{
		    flag = uttlog_qq_udp_identify(iph, udp, uttNfHashConf, datalen, data,in_dev);
		}
	    }
	    break;

	default:/*test*/
	    break;
    }

    return flag;
}

#if CONFIG_XPORT_LOG
static __u8 *match_forum[][4] = {   
    {"","","",""},
    {"passport.tianya.cn","/login","vwriter","action"},
    {"www.tianya.cn","method","vwriter","action"},
    {"passport.mop.com"," ","loginName","loginPasswd"},
    {"passport.liba.com","/login.php","username","password"},
    {"val_code","act=login","name","password"},
    {"passport.pchome.net","/login.php?action=login","username","password"},
    //{"Host: reg.tiexue.net","/loginbar.aspx?FunName=Login","LoginName","Password"}
};  

extern __u32 have_forum(__u8* httpData,int datalen)
{
    int i = 0;
    int len = sizeof(match_forum)/sizeof(match_forum[i]); 

    for(i = 1;i < len;i++){
        if(bm_strstr(httpData, datalen, match_forum[i][0], strlen(match_forum[i][0])) != NULL &&
            bm_strstr(httpData, datalen, match_forum[i][1], strlen(match_forum[i][1])) != NULL &&
            bm_strstr(httpData, datalen, match_forum[i][2], strlen(match_forum[i][2])) != NULL &&
            bm_strstr(httpData, datalen, match_forum[i][3], strlen(match_forum[i][3])) != NULL)

            return i;
    }              
    return 0;
}

void query_forum_account(char* httpData,int datalen,char* account,int forum)
{
    char *start = NULL;
    char *end   = NULL;
    int len = sizeof(match_forum)/sizeof(match_forum[0]);
    if(forum >= len)
        return ;
    start = strstr(httpData,match_forum[forum][2]);
    end = strstr(httpData,match_forum[forum][3]) - 1;
    if(start != NULL && end != NULL && end > start){
        start += strlen(match_forum[forum][2]) + 1;
        memcpy(account,start,end - start);
        urldecode(account);
    }
}
/***********************************************************************
 * 函 数 名：	uttlog_forum_parse_identify
 * 功能描述： 	记录论坛登录帐号
 * 输入参数:	IP包，udp包头指针，udp数据包长度，qq数据包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_forum_parse_identify(IP_HDR *iph, __u8* httpData, int datalen,__u32 forum,struct in_device* in_dev)
{
    int flag = 0;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    __u32    myCurrentTime;
    char forumSyslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    static char forumlogtype[4]="05";
    static char userName[50]= {0};	    
    static char versions[2]="1";       
    static char lognum[4]="01";         
    static char defaultIp[16] = {0};
    static char mbid_len='7';
    static char mbid[]="9000001";
    char account[50] = {0};

    if(forum != 0){
        if(in_dev != NULL)
        {
            if((in_dev->ifa_list) != NULL)
            {
                snprintf(defaultIp,16,NIPQUAD_FMT, NIPQUAD(in_dev->ifa_list->ifa_local)); 
            }
        }

        sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
        do_gettimeofday(&(txc.time));
        myCurrentTime = txc.time.tv_sec;
        httpData += 4;

        query_forum_account(httpData,datalen,account,forum);
        if(forum == 5)
        {
            snprintf(forumSyslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, "%s%s%s%c%stime=%u;deviceip=%s;srcip=%s;username=%s;account=%s;url=%s\r\n", 
                versions, forumlogtype, lognum, mbid_len, mbid, myCurrentTime,
                defaultIp, ipaddr, userName, account, match_forum[forum - 1][0]);
        } else {
        snprintf(forumSyslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, "%s%s%s%c%stime=%u;deviceip=%s;srcip=%s;username=%s;account=%s;url=%s\r\n", 
                versions, forumlogtype, lognum, mbid_len, mbid, myCurrentTime,
                defaultIp, ipaddr, userName, account, match_forum[forum][0]);
        }
        SYSLOG_PRINTK(KERN_USERDEF "%s", forumSyslogMsg);
        return flag;
    }
}
#endif
/***********************************************************************
 * 函 数 名：	httpDisplayFun
 * 功能描述： 	http处理函数, 记录登陆主机的日志，在UttNfHashConf.c中调用
 * 输入参数:	IP包, 限速HASH表节点
 * 输出参数：	无
 * 返回值：   	无
 * 重入性：	是
***********************************************************************/
static int uttlog_web_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* httpData, struct in_device *in_dev,__u32 type)
{
    int flag = 0;
     __u32 forum = 0;

#if CONFIG_XPORT_LOG
     if((type & UTT_LUNTAN_LOG_ENABLE) != 0 ){
     forum = have_forum(httpData,datalen);
        if(forum != 0){
            uttlog_forum_parse_identify(iph, httpData, datalen,forum, in_dev);
        }
     }
#endif

    if (datalen >= 10)
    {
	if (memcmp(httpData, WEB_GET, sizeof(WEB_GET)) == 0 && (type & UTT_WEB_LOG_ENABLE) != 0)
	{
        uttlog_web_parse_identify(iph, httpData, datalen,forum, in_dev);
	}
#if CONFIG_XPORT_LOG  
	else if((type & UTT_WEIBO_LOG_ENABLE) != 0){
	    uttlog_weibo_parse_identify(iph, httpData, datalen,in_dev);	
    }
#endif
    }
    return flag;

}

static int uttlog_web_parse_identify(IP_HDR *iph, __u8* httpData, int datalen,__u32 forum,struct in_device* in_dev)
{
    int flag = 0,i,j;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    __u8 *refer;
    __u8 *host;
    __u8 *tmpData;
    __u32    myCurrentTime;
    __u8 url[MAX_URL_LEN + 1] = {0};
    __u8 uri[MAX_URL_LEN + 1] = {0};
    char msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    static char urllogtype[]="02";	    /*added by jqSun20110907*/
    static char userName[]="";	    /*added by jqSun20110824*/
    static char versions[]="1";         /*added by jqSun20110907*/
    static char lognum[]="01";          /*added by jqSun20110914*/
    static char defaultIp[MAX_IP_LEN] = "";
    static char account[50] = {0};

#if CONFIG_XPORT_LOG    
    char mbid_len[]="7";
    char mbid[]="9000001";
#endif

	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}

    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;
#if 0
    snprintf(mbidlen, MAX_MBID_LEN, "%u", getSystemVersionHandle()->serialNumber);
#endif
    httpData += 4;
    /*
     * 排除含Referer的网址
     * added by jqSun20110904
     */
    refer = (__u8 *)bm_strstr(httpData, datalen, URL_REFERER, sizeof(URL_REFERER));
    if (refer == NULL) 
    {
     /*
     * 排除URI中含有"."的数据包
     */
    for (i = 0; *httpData != 0x20; i++, httpData++)
    {
        if (i >= MAX_URL_LEN)
        {
	    break;
        }
        uri[i] = *httpData;
    }
    uri[i] = 0;
    tmpData = (__u8 *)bm_strstr(uri, i, GET_DOT, sizeof(GET_DOT));
    if (tmpData != NULL)
    {
        /*
	 * URI字段如果有.asp, .aspx, .html, .htm, .php, .jsp, .shtml则记录日志，
	 * 否则其他带有"."的数据包返回
         */
	tmpData++;

	if (memcmp(tmpData, GET_ASP, 3)
		&& memcmp(tmpData, GET_HTM, 3)
		&& memcmp(tmpData, GET_JSP, 3)
		&& memcmp(tmpData, GET_PHP, 3)
		&& memcmp(tmpData, GET_SHTML, 5))
	{
	    return flag;
	}
    }

    /*
     * 寻找访问主机
     */
    host = (__u8 *)bm_strstr(httpData, datalen, HTTP_HOST, sizeof(HTTP_HOST));
    if (host != NULL)
    {
	host += 6; 
	for (j = 0; *host != '\r'; j++, host++)
	{	
	    if (j >= MAX_URL_LEN)
	    {
		break;
	    }
	    if (_checkData(*host) == 1)
	    {
		url[j] = *host;
	    }
	}
	url[j] = 0;
	snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "srcip=%s;url=%s", ipaddr, url);
#if CONFIG_XPORT_LOG
	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
	snprintf(syslogMsg, (MAX_SYSLOG_LOG_MSG_LENGTH-2), "%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;site=%s;uri=%s", 
		versions, urllogtype, lognum,mbid_len,mbid, myCurrentTime, 
		defaultIp, ipaddr, userName, url, uri);
#else
	snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, "%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;site=%s;uri=%s\r\n", 
		versions, urllogtype, lognum, myCurrentTime, 
		defaultIp, ipaddr, userName, url, uri);
#endif
	SWORD_PRINTF("%s-%u, msg = %s\n", __func__, __LINE__, msg);
	SWORD_PRINTK("%s\n", msg);
#if 1
	recordErrLog(msg, myCurrentTime);
#endif
	SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s\n", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG
	SYSLOG_PRINTK(KERN_USERDEF"%s\r\n", syslogMsg);
#else
	SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
    }
    }
    return flag;
}
/***********************************************************************
 * 函 数 名：	_qqLog
 * 功能描述： 	记录QQ登陆和退出消息
 * 输入参数:	IP包，udp包头指针，udp数据包长度，qq数据包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_qq_udp_identify(IP_HDR *iph, UDP_HDR *udp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev)
{
    int flag = 0;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    char msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    static char qqLog[]="1";
    static char imLogin[]="1";         
    static char imLogout[]="2";         
    static char defaultIp[MAX_IP_LEN]="";	    
    static char userName[]="";	   
    static char versions[]="1";   
    static char lognum[]="01";   
    static char imlogtype[]="03";
    __u32    myCurrentTime;
#if CONFIG_XPORT_LOG   
    char mbid_len[]="7";
    char mbid[]="9000001";
#endif

    QQ_HDR *qqData;
    qqData = (QQ_HDR *)data;

    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;
#if 0
		struct rtc_time tm;
		rtc_time_to_tm(txc.time.tv_sec,&tm);
o#endif		SWORD_PRINTF(“UTC time :%u.%u-%u %u:%u:%u /n”,tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
#endif
#if 0
    snprintf(mbidatalen, MAX_MBID_LEN, "%u",getSystemVersionHandle()->serialNumber);
#endif
    if (qqData->flag == 0x02)
    { 
	if (datalen >= LOG_UDP_MIN_LEN) 
	{ 
	    if (((ntohs(qqData->command) == OICQ_LOGIN) && (ntohs(qqData->version) <= OICQ_2008_VERSION)) 
		    || ((ntohs(qqData->command) == OICQ_LOGIN_2009) && (ntohs(qqData->version) > OICQ_2008_VERSION)))
	    {
		snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "qq login ip=%s;qq=%u", ipaddr, ntohl(qqData->qq_number));
		SWORD_PRINTF("%s-%u: msg:%s\n", __func__, __LINE__, msg);
		SWORD_PRINTK("%s\n", msg);
#if 1
		recordErrLog(msg, myCurrentTime);
#endif
#if CONFIG_XPORT_LOG   
	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
		snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			"%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;imtype=%s;account=%u;action=%s\r\n", 
			versions, imlogtype, lognum,mbid_len,mbid,
			myCurrentTime, defaultIp, ipaddr, 
			userName, qqLog, ntohl(qqData->qq_number), imLogin); 
#else
		snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			"%s%s%stime=%u;srcip=%s;username=%s;type=%s;account=%u;action=%s\r\n", 
			versions, imlogtype, lognum,
			myCurrentTime, ipaddr, 
			userName, qqLog, ntohl(qqData->qq_number), imLogin); 
#endif
#if 0
		errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		SWORD_XPORT(KERN_INFO "%s-%u: qq login, syslogMsg:%s\n", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG    
		SYSLOG_PRINTK(KERN_USERDEF"%s", syslogMsg);
#else
		SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
	    }
	    else if (((ntohs(qqData->command) == OICQ_LOGOUT) && (ntohs(qqData->version) <= OICQ_2008_VERSION))
		    || ((ntohs(qqData->command) == OICQ_LOGOUT_2009) && (ntohs(qqData->version) > OICQ_2008_VERSION)))
	    {
		/*
		 * * 解决QQ退出包有4个的问题
		 * */
		if (uttNfHashConf->logNode.qq_seq != ntohs(qqData->seq) 
			|| (uttNfHashConf->logNode.qq_number != ntohl(qqData->qq_number)))
		{
		    uttNfHashConf->logNode.qq_seq = ntohs(qqData->seq);
		    uttNfHashConf->logNode.qq_number = ntohl(qqData->qq_number);
		    snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "qq logout ip=%s;qq=%u", ipaddr, ntohl(qqData->qq_number));
#if CONFIG_XPORT_LOG    
	if((in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
		    snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			    "%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;imtype=%s;account=%u;action=%s\r\n",
			    versions,imlogtype,lognum,mbid_len,mbid,
			    myCurrentTime, defaultIp, ipaddr, 
			    userName, qqLog, ntohl(qqData->qq_number),imLogout);
#else
		    snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			    "%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;type=%s;account=%u;action=%s\r\n",
			    versions,imlogtype,lognum,
			    myCurrentTime, defaultIp, ipaddr, 
			    userName, qqLog, ntohl(qqData->qq_number),imLogout);
#endif
		    SWORD_PRINTF("%s-%u: msg:%s\n", __func__, __LINE__, msg);
		    SWORD_PRINTK("%s\n", msg);
#if 1
		    recordErrLog(msg, myCurrentTime);
#endif
		    SWORD_XPORT(KERN_INFO "%s-%u,qq logout, syslogMsg:%s", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG    
		    SYSLOG_PRINTK(KERN_USERDEF"%s", syslogMsg);
#else
		    SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
#if 0
		    syslog(LOG_NOTICE, msg);
#endif
		}
		else
		{
		    SWORD_PRINTF("%s-%u: not only one logout packet! qq = %u\n",
				__func__, __LINE__, ntohl(qqData->qq_number));
		}
	    }
	    else
	    {
		/*test*/
	    }
	}
    }

    return flag;
}
/***********************************************************************
 * 函 数 名：	_qqTcpLog
 * 功能描述： 	记录QQ tcp 登陆和退出消息
 * 输入参数:	IP包，tcp包头指针，tcp数据包长度，qq数据包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_qq_tcp_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev)
{
    int flag = 0;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    char msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    static char qqLog[]="1";
    static char imLogin[]="1";     
    static char imLogout[]="2";   
    static char defaultIp[MAX_IP_LEN]="";	 
    static char versions[]="1"; 
    static char lognum[]="01"; 
    static char imlogtype[]="03";	  
    static char userName[]="";	  
    __u32    myCurrentTime;
#if CONFIG_XPORT_LOG   
    char mbid_len[]="7";
    char mbid[]="9000001";
#endif
    
#if 0
    snprintf(mbidatalen, MAX_MBID_LEN, "%u",getSystemVersionHandle()->serialNumber);
#endif
    QQ_HDR *qqData;
    qqData = (QQ_HDR *)((char *)data + 2);
    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;

    if (qqData->flag == 0x02)
    { 
	if (datalen >= LOG_TCP_MIN_LEN) 
	{ 
	    if (ntohs(qqData->command) == OICQ_TCP_LOGIN) 
	    {
		snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "qq login ip=%s;qq=%u", ipaddr, ntohl(qqData->qq_number));
#if CONFIG_XPORT_LOG   
	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
		snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			"%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;imtype=%s;account=%u;action=%s\r\n", 
			versions,imlogtype,lognum,mbid_len,mbid,
			myCurrentTime, defaultIp, ipaddr, 
			userName, qqLog, ntohl(qqData->qq_number),imLogin);
#else
		snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			"%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;type=%s;account=%u;action=%s\r\n", 
			versions,imlogtype,lognum,
			myCurrentTime, defaultIp, ipaddr, 
			userName, qqLog, ntohl(qqData->qq_number),imLogin);
#endif

		SWORD_PRINTF("%s-%u: msg:%s\n", __func__, __LINE__, msg);
		SWORD_PRINTK("%s\n", msg);
#if 1
		recordErrLog(msg, myCurrentTime);
#endif
		SWORD_XPORT(KERN_INFO "%s-%u,qq login, syslogMsg:%s", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG   
		SYSLOG_PRINTK(KERN_USERDEF"%s", syslogMsg);
#else
		SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
#if 0
		syslog(LOG_NOTICE, msg);
#endif
	    }
	    else if (((ntohs(qqData->command) == OICQ_TCP_2008_LOGOUT) && (ntohs(qqData->version) <= OICQ_2008_VERSION))
		    || ((ntohs(qqData->command) == OICQ_TCP_2009_LOGOUT) && (ntohs(qqData->version) > OICQ_2008_VERSION)))
	    {
		/*
		 * * 解决QQ退出包有多个的问题
		 * */
		if ((uttNfHashConf->logNode.qq_seq & 0xff00) != (ntohs(qqData->seq) & 0xff00)
		|| (uttNfHashConf->logNode.qq_number) != ntohl(qqData->qq_number))
		{
		    uttNfHashConf->logNode.qq_seq = ntohs(qqData->seq);
		    uttNfHashConf->logNode.qq_number = ntohl(qqData->qq_number); 
		    snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "qq logout ip=%s;qq=%u", ipaddr, ntohl(qqData->qq_number));
#if CONFIG_XPORT_LOG   
	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
		    snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			    "%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;imtype=%s;account=%u;action=%s\r\n",
			    versions, imlogtype, lognum,mbid_len,mbid, 
			    myCurrentTime, defaultIp, ipaddr, userName, 
			    qqLog, ntohl(qqData->qq_number), imLogout);
#else
		    snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			    "%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;type=%s;account=%u;action=%s\r\n",
			    versions, imlogtype, lognum, 
			    myCurrentTime, defaultIp, ipaddr, userName, 
			    qqLog, ntohl(qqData->qq_number), imLogout);
#endif
		    SWORD_PRINTF("%s-%u: msg:%s\n", __func__, __LINE__, msg);
		    SWORD_PRINTK("%s\n", msg);
#if 1
		    recordErrLog(msg, myCurrentTime);
#endif
		    SWORD_XPORT(KERN_INFO "%s-%u,qq logout, syslogMsg:%s", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG  
		    SYSLOG_PRINTK(KERN_USERDEF"%s", syslogMsg);
#else
		    SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
#if 0
		    syslog(LOG_NOTICE, msg);
		    errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		}
		else
		{
		    SWORD_PRINTF("%s-%u: there are two or more logout packet.\n",
			__func__, __LINE__);
		}
	    }
	    else
	    {
		/*test*/
	    }
	}
    }

    return flag;
}
/***********************************************************************
 * 函 数 名：	_msnLog
 * 功能描述： 	记录MSN登陆和退出消息，
 * 输入参数:	IP包, TCP包头，限速HASH表节点，tcp数据包长度，msn数据包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_msn_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* data,struct in_device *in_dev)
{
    int flag = 0;
    struct timex  txc;
    char ipaddr_src[MAX_IP_LEN];
    char ipaddr_dst[MAX_IP_LEN];
    static char msnLoginOK[] = "USR";
    static char msnLogoutOK[] = "OUT";
    static char msnLogin13[] = "13";     /*解决2个相同的登陆包问题*/
    static char msnID[MSN_ID_LEN + 1];

    char    msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char    syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    static char    imLogin[]="1";          /*added by jqSun20110823*/
    static char    imLogout[]="2";         /*added by jqSun20110823*/
    static char    defaultIp[MAX_IP_LEN]="";	 /*added by jqSun20110824*/
    static char    userName[]="";		 /*added by jqSun20110824*/
    static char    versions[]="1";         /*added by jqSun20110907*/
    static char    lognum[]="01";          /*added by jqSun20110914*/
    static char    imlogtype[]="03";	    /*added by jqSun20111008*/
    int	    msglen;
    int	    i;
    __u32    myCurrentTime;
    char    *msnData = (char *)data;
#if 1
    char    *msnData1 = NULL;
#endif
    char   msnLog[]="2";/*added by jqSun20110823*/
#if CONFIG_XPORT_LOG  
    char mbid_len[]="7";
    char mbid[]="9000001";
#endif

    sprintf(ipaddr_src, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
    sprintf(ipaddr_dst, NIPQUAD_FMT, NIPQUAD(iph->daddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;
#if 0
    snprintf(mbidatalen,MAX_MBID_LEN, "%u",getSystemVersionHandle()->serialNumber);
#endif
    if (ntohs(tcp->source) == IPPORT_MSN)
    {
	if (datalen >= LOG_TCP_MIN_LEN)
	{ 
#if 0
	    if (memcmp(msnData, msnLoginOK, 3) == 0)
	    { 
		if (memcmp(msnData + 4, msnLogin13, 2) != 0)
		{
		    msnData = strstr(msnData + 4, "OK"); 
		    if (msnData != NULL) 
		    { 
			msnData += 3;
			for (i = 0; *msnData != 0x20; msnData++, i++) 
			{
			    if (i >= MSN_ID_LEN)
			    {
				break;
			    } 
			    if (_checkData(*msnData) == 1)
			    {
				msnID[i] = *msnData;
			    }
			}
			msnID[i] = 0; 
#else

	    if (memcmp(msnData, msnLoginOK, 3) == 0)
	    {
		if(datalen < 6)
		{
		    return flag;
		}
		if (memcmp(msnData + 4, msnLogin13, 2) != 0)
		{
		    msnData1 = (char *)bm_strstr((msnData + 4), (datalen - 4), msn_OK, sizeof(msn_OK)); 
		    datalen -= (msnData1 - msnData);
		    if (msnData1 != NULL) 
		    { 
			datalen -= 3;
			if(datalen < 0)
			{
			    return flag; 
			}
			msnData = msnData1 + 3;
			for (i = 0; *msnData != 0x20; msnData++, i++) 
			{
			    datalen --;
			    if(datalen < 0)
			    {
				return flag;
			    }
			    if (i >= MSN_ID_LEN)
			    {
				break;
			    } 
			    if (_checkData(*msnData) == 1)
			    {
				msnID[i] = *msnData;
			    }
			}
			msnID[i] = 0; 
#endif
			SWORD_PRINTF("%s-%u, msnID = %s\n", __func__, __LINE__, msnID);
			/* store msn id and src ip into hash table*/ 
			memcpy(uttNfHashConf->logNode.msnID, msnID, MSN_ID_LEN + 1);
			uttNfHashConf->logNode.msnIP = iph->saddr;

			snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "msn login ip=%s;ID=%s", ipaddr_dst, msnID);
#if CONFIG_XPORT_LOG  
	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
			snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
				"%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;imtype=%s;account=%s;action=%s\r\n", 
				versions, imlogtype, lognum,mbid_len,mbid,
				myCurrentTime, defaultIp, ipaddr_dst, 
				userName, msnLog, msnID, imLogin);
#else
			snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
				"%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;type=%s;account=%s;action=%s\r\n", 
				versions, imlogtype, lognum,
				myCurrentTime, defaultIp, ipaddr_dst, 
				userName, msnLog, msnID, imLogin);
#endif

			msglen = strlen(msg);

			if (msglen < (MAX_ERROR_LOG_MSG_LENGTH + 1))
			{ 
			    SWORD_PRINTF("%s-%u msg: %s\n", __func__, __LINE__, msg);
			    SWORD_PRINTK("%s\n", msg);
#if 1
			    recordErrLog(msg, myCurrentTime);
#endif
			    SWORD_XPORT(KERN_INFO "%s-%u,syslogMsg:%s", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG  
			    SYSLOG_PRINTK(KERN_USERDEF"%s", syslogMsg);
#else
			    SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
#if 0
			    syslog(LOG_NOTICE, msg);
			    errorSLogEx(LOG_QQ_LOGIN, msg);

#endif
			}
			else
			{
			    SWORD_PRINTF("%s-%u message length of msn is exceeded\n",
				    __func__, __LINE__);
			}
		    }
		}
		else
		{
		    SWORD_PRINTF("%s-%u: there is already a login packet\n",
				__func__, __LINE__); 
		}
	    }
	}
    }
    else if (ntohs(tcp->dest) == IPPORT_MSN)
    {
	if (memcmp(msnData, msnLogoutOK, 3) == 0)
	{
	    if (uttNfHashConf->logNode.msnIP == iph->daddr)
	    {
		memcpy(msnID, uttNfHashConf->logNode.msnID, MSN_ID_LEN + 1);
	
		snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "msn logout ip=%s;ID=%s", ipaddr_src, msnID);
#if CONFIG_XPORT_LOG   
	if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
	    sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
	}
		snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			"%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;imtype=%s;account=%s;action=%s\r\n", 
			versions, imlogtype, lognum,mbid_len,mbid,
			myCurrentTime, defaultIp, ipaddr_src, 
			userName, msnLog, msnID, imLogout);
#else
		snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, 
			"%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;type=%s;account=%s;action=%s\r\n", 
			versions, imlogtype, lognum,
			myCurrentTime, defaultIp, ipaddr_src, 
			userName, msnLog, msnID, imLogout);
#endif
		msglen = strlen(msg);

		if (msglen < (MAX_ERROR_LOG_MSG_LENGTH + 1))
		{
		    SWORD_PRINTF("%s-%u msg: %s\n", __func__, __LINE__, msg); 
		    SWORD_PRINTK("%s\n", msg); 
#if 1
		    recordErrLog(msg, myCurrentTime);
#endif
		    SWORD_XPORT(KERN_INFO "%s-%u,syslogMsg:%s", __func__, __LINE__, syslogMsg);
#if CONFIG_XPORT_LOG  
		    SYSLOG_PRINTK(KERN_USERDEF"%s", syslogMsg);
#else
		    SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#endif
#if 0
		    syslog(LOG_NOTICE, msg);
		    errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		}
		else
		{
		    SWORD_PRINTF("%s-%u message length of msn is exceeded\n",
			__func__, __LINE__); 
		}
	    }
	    else
	    {
		SWORD_PRINTF("%s-%u: there is already a logout packet!\n",
			__func__, __LINE__); 
	    }
	}
    }
    else
    {
	/*test*/
    }
    return flag;
    
}
/***********************************************************************
 * 函 数 名：	_smtpDisplayFun
 * 功能描述： 	smtp邮件处理函数
 * 输入参数:	IP包, TCP包头，限速HASH表节点， tcp数据包长度，smtp包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_smtp_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* smtpData,struct in_device* in_dev)
{
    int flag = 1;
    if (datalen >= LOG_TCP_MIN_LEN)
    { 
	if (ntohs(tcp->dest) == IPPORT_SMTP) 
	{ 
	    /* smtp client send "DATA" and receive data */
	    if ((memcmp(smtpData, (const char *)DATA, 4) != 0)
		    && (memcmp(smtpData, (const char *)Data, 4) != 0))
	    {
		switch(uttNfHashConf->logNode.smtpStat)
		{
		    case SMTP_OK_SEND_DATA:
			uttlog_smtp_parse_identify(iph, smtpData, datalen, in_dev);
			uttNfHashConf->logNode.smtpStat = SMTP_NULL_STAT;
			break;
		    default:/*test*/
			break;
		}
	    }
	    else
	    {
		/* set status prepare send smtp data */
		uttNfHashConf->logNode.smtpStat = SMTP_WAIT_SEND_DATA;
	    }
	}
	else
	{
	    /* smtp server send 354 to client */
	    switch(uttNfHashConf->logNode.smtpStat)
	    {
		case SMTP_WAIT_SEND_DATA:
		    if ((memcmp(smtpData, smtp_354, 3)) == 0)
		    {
			uttNfHashConf->logNode.smtpStat = SMTP_OK_SEND_DATA;
		    }
		    break;
		default:/*test*/
		    break;
	    }
	}
    }
    else
    {
	flag = 0;
    }
    return flag;
}
/***********************************************************************
 * 函 数 名：	_smtpDisplayFun
 * 功能描述： 	smtp邮件处理函数
 * 输入参数:	IP包, TCP包头，限速HASH表节点， tcp数据包长度，smtp包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_smtp_parse_identify(IP_HDR *iph, __u8 *smtpData, int datalen,struct in_device* in_dev)
{
    int flag = 1;
    __u32    myCurrentTime;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    __u8 *tmpData;
    char msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    char mail[MAIL_ADDRESS_LEN + 1];
    int i, msglen,loglen;
    int len;
#if 1
    int lenleft = 0;
#endif
    static char defaultIp[MAX_IP_LEN]="";	   
    static char userName[]="";	  
    static char smtpDirection[]="1"; 
    static char versions[]="1";     
    static char lognum[]="01";    
    static char emaillogtype[]="04";
    static char mbid_len='7';
    static char mbid[]="9000001";

    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;
#if 0
    sprintf(mbidatalen,"%u",getSystemVersionHandle()->serialNumber);
#endif

    if(in_dev != NULL)
    {
        if((in_dev->ifa_list) != NULL)
        {
            snprintf(defaultIp,16,NIPQUAD_FMT, NIPQUAD(in_dev->ifa_list->ifa_local)); 
        }
    }

    snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "smtp mail ip=%s;from=", ipaddr);
    sprintf(syslogMsg, "%s%s%s%s%c%stime=%u;deviceip=%s;userip=%s;username=%s;from=", 
	 KERN_USERDEF,versions, emaillogtype, lognum, mbid_len,mbid,
	    myCurrentTime, defaultIp, ipaddr, userName);
    msglen = strlen(msg);
    loglen = strlen(syslogMsg);
 
   /*
    * 解决smtp发送2个包的问题
    */
    tmpData = (__u8 *)bm_strstr(smtpData, datalen, SMTP_ATTACH, sizeof(SMTP_ATTACH));

    if (tmpData != NULL)
    {
	SWORD_PRINTF("%s-%u: there are two smtp packets\n", 
		    __func__, __LINE__);
    }
    else
    { 
	/*
	 * 保存邮件发送地址 
	 */
	tmpData = (__u8 *)bm_strstr(smtpData, datalen, MAIL_FROM, sizeof(MAIL_FROM));
	if (tmpData != NULL)
	{
	    len = tmpData - smtpData; 
	    tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), MAIL_LEFT_BRACE, sizeof(MAIL_LEFT_BRACE));
#if 1
	if (tmpData == NULL) {
	    flag = 0;
	} else {
	    len = tmpData - smtpData;
	    lenleft = datalen - len;
#endif
	    tmpData++; 
#if 1
	    lenleft--;
#endif
#if 0
	    for (i = 0; *tmpData != MAIL_RIGHT_BRACE; tmpData++, i++)
#else
	    for (i = 0; (*tmpData != MAIL_RIGHT_BRACE) && (lenleft > 0); tmpData++, i++, lenleft--)
#endif
	    {
		if (i == MAIL_ADDRESS_LEN)
		{
		    break;
		} 
		if (_checkData(*tmpData) == 1)
		{
		    mail[i] = *tmpData;
		}
	    }
	    mail[i] = 0; 
	    msglen += i;
        loglen += i;
	    if (msglen >= (MAX_ERROR_LOG_MSG_LENGTH + 1))
	    {
#if 0
		errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		flag = 0;
	    }
	    else
	    {
		strcat(msg, mail);
		strcat(syslogMsg, mail);
		/*
		 * * 保存关键字
		 * */
		msglen += 4;
        loglen += 4;
		if (msglen >= (MAX_ERROR_LOG_MSG_LENGTH + 1))
		{
#if 0
		errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		flag = 0;
		}
		else
		{
		    strcat(msg, ";to=");
		    strcat(syslogMsg, ";to="); 
#if 1
		    len = tmpData - smtpData;
#endif
		    /*
		     * * 保存邮件目的地址 
		     * */
#if 1
		    tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), MAIL_TO, sizeof(MAIL_TO));
#else
		    tmpData = (__u8 *)bm_strstr(tmpData, datalen, MAIL_TO, sizeof(MAIL_TO));
#endif
		    len = tmpData - smtpData;
		    if (tmpData == NULL) 
		    {
			flag = 0;
		    }
		    else
		    { 
			while (1)
			{
#if 1
			    memset(mail, 0, sizeof(mail));

#endif
			    tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), MAIL_LEFT_BRACE, sizeof(MAIL_LEFT_BRACE));
#if 1
			    if (tmpData == NULL) {
				flag = 0;
				break;
			    }
			    len = tmpData - smtpData;
			    lenleft = datalen - len;
#endif
			    tmpData++; 
#if 1
			    lenleft--;
#endif
#if 0
			    for (i = 0; *tmpData != MAIL_RIGHT_BRACE; tmpData++, i++)
#else
			    for (i = 0; (*tmpData != MAIL_RIGHT_BRACE) && (lenleft > 0); tmpData++, i++, lenleft--)
#endif
			    {
				if (i == MAIL_ADDRESS_LEN)
				{
				    break;
				} 
				if (_checkData(*tmpData) == 1)
				{
				    mail[i] = *tmpData;
				}
			    }
			    mail[i] = 0; 
			    msglen += i; 
			    loglen += i; 
			    if (msglen < (MAX_ERROR_LOG_MSG_LENGTH + 1))
			    {
				strcat(msg, mail);
				strcat(syslogMsg, mail); 
#if 1
				/* 增加长度检查判断 */
				if (lenleft <= 0) {
				    break;
				}
#endif
				/*
				 * * 判断是否存在多个目的地址，检查是否是","
				 * */
				tmpData++;
				if (*tmpData != 0x2c) 
				{
				    break;
				} 
				msglen++;
				loglen++;
				if (msglen >= (MAX_ERROR_LOG_MSG_LENGTH + 1))
				{
#if 0
				    errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
				    SWORD_PRINTK("%s\n", msg); 
#if 1
				    recordErrLog(msg, myCurrentTime);
#endif
				    SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				    SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
				    flag = 0;
				    return flag;
				}
				else
				{
				    strcat(msg, ",");
				    strcat(syslogMsg, ","); 
				    /*
				     * * 变化数据包查找长度，保存下一个To地址
				     * */
				    len = tmpData - smtpData;
				}
			    }
			    else
			    {
				SWORD_PRINTF("%s-%u: The length is exclude!\n",
					__func__, __LINE__);
				SWORD_PRINTK("%s\n", msg); 
#if 1
				recordErrLog(msg, myCurrentTime);
#endif
				SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
				flag = 0;
				return flag;
#if 0
				errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
			    }
			}
#if 0
			strcat(syslogMsg,";subject=");
#endif
#if 1
			/* 增加空指针判断 */
			if (tmpData != NULL) {
			    len = tmpData - smtpData;
			    tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), 
				MAIL_SUBJECT, sizeof(MAIL_SUBJECT));
#else
			tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), 
				MAIL_SUBJECT, sizeof(MAIL_SUBJECT));
#endif
			if (tmpData != NULL)
			{
			    tmpData += 9;
			    for(i = 0;*tmpData != SUBJECT_END;tmpData++,i++)
			    { 
				if(i == MAIL_ADDRESS_LEN)
				{
				    break;
				}
				mail[i] = *tmpData; 
			    }
			    mail[i]=0;
			    msglen += i; 
			    loglen += i; 
			    if(loglen + 9 + 1 + 10 + 1 + 2 >= MAX_SYSLOG_LOG_MSG_LENGTH + 1){
#if 0
			    if(msglen > MAX_ERROR_LOG_MSG_LENGTH + 1){
#endif
#if 0
				errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
				SWORD_PRINTK("%s\n", msg); 
#if 1
				recordErrLog(msg, myCurrentTime);
#endif
				SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				//SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
				flag = 0;
			    }
			    else
			    { 
                strcat(syslogMsg,";subject=");
				strcat(syslogMsg,mail);
				strcat(syslogMsg,";");
				strcat(syslogMsg,"direction=");
				strcat(syslogMsg,smtpDirection);
				strcat(syslogMsg,"\r\n");
				SWORD_PRINTF("%s-%u: msg: %s\n", __func__, __LINE__, msg); 
				SWORD_PRINTK("%s\n", msg); 
#if 1
				recordErrLog(msg, myCurrentTime);
#endif
				SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#if 0
				errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
			    }
			}
#if 1
			}
#endif
		    }
		}
	    }
	}
	}
	else
	{
	    flag = 0;
	}
    }
    return flag;
}
/***********************************************************************
 * 函 数 名：	_smtpDisplayFun
 * 功能描述： 	smtp邮件处理函数
 * 输入参数:	IP包, TCP包头，限速HASH表节点， tcp数据包长度，smtp包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_pop_identify(IP_HDR *iph, TCP_HDR *tcp, UttNfHashConf *uttNfHashConf, int datalen, __u8* popData,struct in_device* in_dev)
{
    int flag = 1;
    if (datalen >= POP_TCP_MIN_LEN)
    { 
	if (ntohs(tcp->dest) == IPPORT_POP3)
	{
	    /* pop client send "RETR *" and wait data */
	    if (memcmp(popData, POP_RETR, sizeof(POP_RETR)) == 0)
	    {
		uttNfHashConf->logNode.popStat = POP_WAIT_RECEIVE_DATA;
	    }else{
            uttNfHashConf->logNode.popStat = POP_NULL_STAT;
	    }
	}
	else
	{
	    /* pop server send data to client */
	    if (uttNfHashConf->logNode.popStat == POP_WAIT_RECEIVE_DATA)
	    {
		if (bm_strstr(popData, datalen, pop_Received, sizeof(pop_Received)) != NULL)
		{
            uttlog_pop_parse_identify(iph, popData, datalen, in_dev);
            uttNfHashConf->logNode.popStat = POP_NULL_STAT;
		}
	    }
	}
    }
    else
    {
	flag = 0;
    }
    return flag;
}
/***********************************************************************
 * 函 数 名：	_smtpDisplayFun
 * 功能描述： 	smtp邮件处理函数
 * 输入参数:	IP包, TCP包头，限速HASH表节点， tcp数据包长度，smtp包指针
 * 输出参数：	无
 * 返回值：   	无
***********************************************************************/
static int uttlog_pop_parse_identify(IP_HDR *iph, __u8 *popData, int datalen,struct in_device* in_dev)
{
    int flag = 1;
    int i, msglen,loglen;
#if 1
    int lenleft = 0;
#endif
    __u32    myCurrentTime;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    char msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    
    char mail[MAIL_ADDRESS_LEN + 1];
    static char defaultIp[MAX_IP_LEN]="";	    /*added by jqSun20110824*/
    static char userName[]="";	    /*added by jqSun20110824*/
    static char popDirection[]="2";     /*added by jqSun20110824*/
    static char versions[]="1";         /*added by jqSun20110907*/
    static char lognum[]="01";          /*added by jqSun20110914*/
    static char emaillogtype[]="04";    /*added by jqSun20111008*/
    int len;
    __u8 *tmpData;

    char mbid_len='7';
    char mbid[]="9000001";

    if(in_dev != NULL)
    {
        if((in_dev->ifa_list) != NULL)
        {
            snprintf(defaultIp,16,NIPQUAD_FMT, NIPQUAD(in_dev->ifa_list->ifa_local)); 
        }
    }

    memset(mail, 0, sizeof(mail));
    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->daddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;
#if 0
    snprintf(mbidatalen, MAX_MBID_LEN, "%u",getSystemVersionHandle()->serialNumber);
#endif
    snprintf(msg, MAX_ERROR_LOG_MSG_LENGTH, "pop mail ip=%s;from=", ipaddr); 
    snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, "%s%s%s%s%c%stime=%u;deviceip=%s;userip=%s;username=%s;from=", 
	 KERN_USERDEF,versions, emaillogtype, lognum, mbid_len, mbid,
	    myCurrentTime, defaultIp, ipaddr, userName);
    msglen = strlen(msg); 
    loglen = strlen(syslogMsg); 
    /*
     * 保存邮件发送地址 
     */
    tmpData = (__u8 *)bm_strstr(popData, datalen, MAIL_FROM, sizeof(MAIL_FROM));
    len = tmpData - popData;

    if (tmpData == NULL)
    {
	flag = 0;
    }
    else
    { 
	tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), MAIL_LEFT_BRACE, sizeof(MAIL_LEFT_BRACE));
#if 1
	if (tmpData == NULL) {
	    flag = 0;
	} else {
	    len = tmpData - popData;
	    lenleft = datalen - len;
#endif
	    tmpData++; 
#if 1
	    lenleft--;
#endif
#if 0
	    for (i = 0; *tmpData != MAIL_RIGHT_BRACE; tmpData++, i++)
#else
	    for (i = 0; (*tmpData != MAIL_RIGHT_BRACE) && (lenleft > 0); tmpData++, i++, lenleft--)
#endif
	    {
		if (i == MAIL_ADDRESS_LEN)
		{
		    break;
		} 
		if (_checkData(*tmpData) == 1)
		{
		    mail[i] = *tmpData;
		}
	    }
	    mail[i] = 0; 
	    msglen += i; 
	    loglen += i; 
	    if (msglen >= (MAX_ERROR_LOG_MSG_LENGTH + 1))
	    {
#if 0
		errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		flag = 0;
	    }
	    else
	    {
		strcat(msg, mail);
		strcat(syslogMsg, mail); 
		/*
		 * * 保存关键字
		 * */
		msglen += 4; 
		if (msglen >= (MAX_ERROR_LOG_MSG_LENGTH + 1))
		{
#if 0
		    errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
		    flag = 0;
		}
		else
		{
		    strcat(msg, ";to=");
		    strcat(syslogMsg, ";to="); 
#if 1
		    len = tmpData - popData;
#endif
		    /*
		     * * 保存邮件目的地址 
		     * */
#if 1
		    tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), MAIL_TO, sizeof(MAIL_TO));
#else
		    tmpData = (__u8 *)bm_strstr(tmpData, datalen, MAIL_TO, sizeof(MAIL_TO));
#endif
		    len = tmpData - popData;
		    if (tmpData == NULL) 
		    {
			flag = 0;
		    }
		    else
		    { 
#if 0
			memset(mail, 0, sizeof(mail));
#endif
			while (1)
			{
#if 1
			    memset(mail, 0, sizeof(mail));
#endif

#if 0
			    if(strlen(mail) == 0)
			    {
#endif
				tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), MAIL_LEFT_BRACE, sizeof(MAIL_LEFT_BRACE));
#if 1
				if (tmpData == NULL) {
				    flag = 0;
				    break;
				} 
				len = tmpData - popData;
				lenleft = datalen - len;
#endif
				tmpData++; 
#if 1
				lenleft--;
#endif
#if 0
				for (i = 0; *tmpData != MAIL_RIGHT_BRACE; tmpData++, i++)
#else
	    
				for (i = 0; (*tmpData != MAIL_RIGHT_BRACE) && (lenleft > 0); tmpData++, i++, lenleft--)
#endif
				{

				    if (i == MAIL_ADDRESS_LEN)
				    {
					break;
				    } 
				    if (_checkData(*tmpData) == 1)
				    {
					mail[i] = *tmpData;
				    }

				}
				mail[i] = 0; 
#if 0
				/* 防止死循环 */
				if (mail[0] == '\0') {
				    flag = 0;
				    return flag;
				}
				}
#endif
#if 0
			    }
			    else
			    {
#if 1
				if (lenleft > 2) {
#endif
				tmpData += 2; 
#if 1
				lenleft -= 2;
				}

#endif
				if(*tmpData == 0x0d)
				{
#if 1
				    if (lenleft > 3) {
#endif
				    tmpData += 3; 
#if 1
				    lenleft -= 3;
				    }
#endif
				}

#if 0
				for (i = 0; *tmpData != 0x2c; tmpData++, i++)
#else
				for (i = 0; (*tmpData != 0x2c) && (lenleft > 0); tmpData++, i++, lenleft--)
#endif
				{
				    if(*tmpData == 0x0d)
				    {
					break;
				    }
				    if (i == MAIL_ADDRESS_LEN)
				    {
					break;
				    } 
				    if (_checkData(*tmpData) == 1)
				    {
					mail[i] = *tmpData;
				    }
				}
				mail[i] = 0; 
			    }
#endif
			    msglen += i; 
			    loglen += i; 
			    if (msglen < (MAX_ERROR_LOG_MSG_LENGTH + 1))
			    {
				strcat(msg, mail);
				strcat(syslogMsg, mail); 
#if 1
				/* 增加长度检查判断 */
				if (lenleft <= 0) {
				    break;
				}
#endif
				/*
				 * * 判断是否存在多个目的地址，检查是否是","
				 * */
#if 0
				if ((*tmpData != 0x2c) && (*(tmpData += 1) != 0x2c)) 
#endif
				tmpData++;
				if (*tmpData != 0x2c)
				{
				    break;
				} 
				msglen++;
				loglen++;
				if (msglen >= (MAX_ERROR_LOG_MSG_LENGTH + 1))
				{
#if 0
				    errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
				    SWORD_PRINTK("%s\n", msg); 
#if 1
				    recordErrLog(msg, myCurrentTime);
#endif
				    SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				    SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
				    flag = 0;
				    return flag;
				}
				else
				{
				    strcat(msg, ",");
				    strcat(syslogMsg, ","); 
				    /*
				     * * 变化数据包查找长度，保存下一个To地址
				     * */
				    len = tmpData - popData;
				}
			    }
			    else
			    {
				SWORD_PRINTF("%s-%u: The length is exclude!\n",
					__func__, __LINE__);
#if 0
				errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
				SWORD_PRINTK("%s\n", msg); 
#if 1
				recordErrLog(msg, myCurrentTime);
#endif
				SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
				flag = 0;
				return flag;
			    }
			}
#if 0
			strcat(syslogMsg,";subject=");
#endif
#if 1
			/* 增加空指针判断 */
			if (tmpData != NULL) {
			    len = tmpData - popData;
			    tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), 
				MAIL_SUBJECT, sizeof(MAIL_SUBJECT));
#else
			tmpData = (__u8 *)bm_strstr(tmpData, (datalen - len), 
				MAIL_SUBJECT, sizeof(MAIL_SUBJECT));
#endif
			if (tmpData != NULL)
			{
#if 1
			    len = tmpData - popData;
			    lenleft = datalen - len;
#endif
			    tmpData += 9;
#if 1
			    lenleft -= 9;
#endif
#if 0
			    for(i = 0;*tmpData != SUBJECT_END;tmpData++,i++)
#else
			    for(i = 0;(*tmpData != SUBJECT_END) && (lenleft > 0);tmpData++,i++, lenleft--)
#endif
			    { 
				if(i == MAIL_ADDRESS_LEN)
				{
				    break;
				}
				mail[i] = *tmpData; 
			    }
			    mail[i]=0;
			    msglen += i; 
			    loglen += i; 
#if 1
			    if(loglen + 9 + 1 + 10 + 1 + 2 > MAX_SYSLOG_LOG_MSG_LENGTH + 1){
#if 0
			    if(msglen > MAX_ERROR_LOG_MSG_LENGTH + 1){
#endif
#else
				/* 越界判断 */
			    if(msglen + 9 + 1 + 10 + 1 + 2 >= MAX_ERROR_LOG_MSG_LENGTH + 1){
#endif
#if 0
				errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
				SWORD_PRINTK("%s\n", msg); 
#if 1
				recordErrLog(msg, myCurrentTime);
#endif
				SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				//SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
				flag = 0;
			    }
			    else
			    { 
#if 1
				strcat(syslogMsg,";subject=");
#endif
				strcat(syslogMsg,mail);
				strcat(syslogMsg,";");
				strcat(syslogMsg,"direction=");
				strcat(syslogMsg,popDirection);
				strcat(syslogMsg,"\r\n");
				SWORD_PRINTF("%s-%u: msg: %s\n", __func__, __LINE__, msg); 
				SWORD_PRINTK("%s\n", msg); 
#if 1
				recordErrLog(msg, myCurrentTime);
#endif
				SWORD_XPORT(KERN_INFO "%s-%u, syslogMsg = %s",  __func__, __LINE__, syslogMsg);
				SYSLOG_PRINTK(KERN_INFO "%s", syslogMsg);
#if 0
				yslog(LOG_NOTICE, msg);
				errorSLogEx(LOG_QQ_LOGIN, msg);
#endif
			    }
			}
#if 1
			}
#endif
		    }
		}
	    }
#if 1
	}
#endif
    }
    return flag;
}


#if CONFIG_XPORT_LOG  
 /***********************************************************************
 * 函 数 名：	weibo日志
 * 功能描述： 	 记录微博的日志
***********************************************************************/
 static void urldecode(char *p)  
{  
    int i=0;  
    while(*(p+i))  
    {  
	 if ((*p=*(p+i)) == '%'){ 
	      *p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');  
	     *p=(*p) * 16;  
	    *p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');  
	    i+=2;  
	}else if (*(p+i)=='+'){  
	    *p=' ';  
       }  
	 p++;  
    }  
    *p='\0';  

} 
static int uttlog_weibo_parse_identify(IP_HDR *iph, __u8* httpData, int datalen,struct in_device *in_dev)
{
    int flag = 0,i,j;
    struct timex  txc;
    char ipaddr[MAX_IP_LEN];
    __u8 *text;
    __u8 *acc_pos;
    __u8 *conn_pos;
    static __u8 *host;
    __u32    myCurrentTime;
    __u8 content[281] = {0};
    char msg[MAX_ERROR_LOG_MSG_LENGTH + 1];
    char syslogMsg[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    char tmp_nu[MAX_SYSLOG_LOG_MSG_LENGTH + 1];
    static char urllogtype[]="06";	    
    static char userName[]="";	    
    static char versions[]="1";         
    static char lognum[]="01";          
    static char defaultIp[MAX_IP_LEN] = "";
    char account[60] = "";
    int str_len =0;

    char mbid_len[]="7";
    char mbid[]="9000001";
    sprintf(ipaddr, NIPQUAD_FMT, NIPQUAD(iph->saddr)); 
    do_gettimeofday(&(txc.time));
    myCurrentTime = txc.time.tv_sec;
    conn_pos = (__u8 *)bm_strstr(httpData, datalen, WEIBO_CENNECTION, sizeof(WEIBO_CENNECTION));
    acc_pos = httpData;
    if(acc_pos != NULL){
    	for (j = 0; acc_pos != conn_pos; j++, acc_pos++)
	{	
	    if (j >= MAX_SYSLOG_LOG_MSG_LENGTH)
	    {
		break;
	     }
	    tmp_nu[j] = *acc_pos;
	}
    }
    tmp_nu[j]=0;
    acc_pos =  tmp_nu;
    urldecode(tmp_nu);
    urldecode(tmp_nu);
    acc_pos = (__u8 *)bm_strstr(tmp_nu, conn_pos-httpData, WEIBO_NAME, sizeof(WEIBO_NAME));
    if(acc_pos != NULL){
	acc_pos +=5;
	for (j = 0; *acc_pos != '&'; j++, acc_pos++)
	{	
	    if (j >= 50)
	    {
		break;
	     }
	    account[j] = *acc_pos;
	}
	account[j]=0;
	}
	text = (__u8 *)bm_strstr(httpData,datalen, WEIBO_TEXT, sizeof(WEIBO_TEXT));
	if(text != NULL){
	    acc_pos = (__u8 *)bm_strstr(text,httpData+datalen-text, WEIBO_PIC_ID, sizeof(WEIBO_PIC_ID));
	    if(acc_pos != NULL && (httpData + datalen - text) > 5){
		text += 5; 
		for (j = 0; *text != '&'; j++, text++)
		{	
		    if (j >= 280)
		    {
			break;
		     }
		    content[j] = *text;
		}
		content[j]=0;
		urldecode(content);
		str_len = strlen(content);
		for(j=0;j < str_len;j++){
		    if(content[j] == '\n')
			content[j] = ' ';
		}    
	    }
	}
   if(content[0] != 0 || account[0] != 0){
        if( (in_dev != NULL) && (in_dev->ifa_list !=NULL)){
            sprintf(defaultIp,  NIPQUAD_FMT,NIPQUAD(in_dev->ifa_list->ifa_local));
        }
	snprintf(syslogMsg, MAX_SYSLOG_LOG_MSG_LENGTH, "%s%s%s%s%stime=%u;deviceip=%s;srcip=%s;username=%s;account=%s;content=%s\r\n", 
		versions, urllogtype, lognum,mbid_len,mbid, myCurrentTime, 
		defaultIp, ipaddr, userName, account, content);
	SYSLOG_PRINTK(KERN_USERDEF "%s", syslogMsg);
    }
    return flag;
}
#endif
