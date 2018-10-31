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
 * ͳ���Ƿ���Ҫ���ź���?
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
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/string.h>
#include <linux/inet.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("chen.bin<brianchen829@gmail.com>");
MODULE_DESCRIPTION("x_tables UTT NET SNIPER modification module");
MODULE_ALIAS("ipt_UTTNETSNIPER");

#define DEBUG_UTT_NETSNIPER 1
#if DEBUG_UTT_NETSNIPER
#define UTT_NETSNIPER_DEBUG(fmt,argx...) printk_netsniper(fmt,##argx)
#else
#define UTT_NETSNIPER_DEBUG(fmt,argx...)
#endif


#define NTSR_IP_TTL_FLAG    (1U)

/* ������ip idʶ���ʶ */
#define NTSR_IP_ID_FLAG     (1U << 1U)

/* ������ʶ��Ϊ��������ʶ */
#define NTSR_FLAG       (NTSR_IP_TTL_FLAG | NTSR_IP_ID_FLAG)

/* TP2 flag */
#define NTSR_ANTI_TP2_FLAG  (1U)

/* �������ƽ�flag */
#define NTSR_ANTI_FLAG  (NTSR_ANTI_TP2_FLAG)

#ifndef NIPQUAD
#define NIPQUAD(addr) \
	        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define NIPQUAD_FMT "%u.%u.%u.%u"
#endif
#define DEF_USER_TTL_CNT  4
__u8 def_user_ttl[DEF_USER_TTL_CNT] = {32U, 64U, 128U, 255U};
struct timer_list ntsrTimer;
__u32 ntsr_global_control_time = 300U;
__u32 ntsr_global_ipid_check_time_interval = 10U;
__u32 ntsr_global_ipid_linear_interval = 500U;
__u32 ntsr_global_ipid_check_cnt = 10U;
extern UttNfHashBuf *uttNfIpHashTable[MAX_UTTNF_HASH_TABLE];
static __u32 get_current_time(void);

asmlinkage int printk_netsniper(const char *fmt,...)
{
	va_list args;
	int r;

	va_start(args,fmt);
	r = vprintk(fmt,args);
	va_end(args);

	return r;
}

static void ntsr_anti_flag(struct sk_buff *skb,UttNetSniper *netSniper ,struct iphdr *ipH) {
	TCP_HDR *tcp;
    __u8 *data;
    __u32 tcpHeadLen = 0U;
	__u16 iph_len = 0U;
	iph_len = ip_hdrlen(skb);
	tcp = (TCP_HDR *)(skb_network_header(skb) + iph_len);
	tcpHeadLen = tcp->doff * 4U; 
	data = (__u8 *)((void *)tcp + tcpHeadLen);/*tcp��ǰ���2λ*/
	if(data[0] == 'G' && data[1] == 'E' && data[2] == 'T'){
		 char strCookie[50];
		 strcpy(strCookie,"Cookie: ");
		 if(strstr(data,strCookie) != NULL){
			 strcpy(strCookie, "Connection: \r\nCookie: \r\n");
			 if(strstr(data,strCookie) != NULL){
				netSniper->ntsr_anti_flag |= NTSR_ANTI_TP2_FLAG;	
			 }else{
				netSniper->ntsr_anti_flag &= ~NTSR_ANTI_TP2_FLAG;
			 }
		 }
	}
}

static __u32 get_current_time(void){
	struct timeval curTv;
	do_gettimeofday(&curTv);
	return curTv.tv_sec;
}

static bool ntsr_ip_id_chk_linear(struct iphdr *ipH,__u16 ip_id1, __u16 ip_id2){
	/*����ip_id�Ĳ�ֵ*/
	__u32 interval = ntsr_global_ipid_linear_interval;
	/*���Ա�־*/
	bool linear = false;
	/*�����־*/
	bool overflow_small = false,overflow_big = false;
	 /* ���ִ��������С,ͨ�������ж����� */
	__u16 ip_id_small,ip_id_big;
	/* ����������ֵ */
	__u16 big_overflow = 0U;
	
	/*ip_id��ȣ��������ش��İ�*/
	if(ip_id1 == ip_id2){
		linear = true;
	}else{
		ip_id_small = (ip_id1 > ip_id2) ? ip_id2 : ip_id1;
		ip_id_big = (ip_id1 < ip_id2) ? ip_id2 : ip_id1;

		/*�ж��Ƿ����*/
		if ((__u32)((__u32)ip_id_small + (__u32)interval) > 65535U) {
			overflow_small = true;
		}

		if ((__u32)((__u32)ip_id_big + (__u32)interval) > 65535U) {
			overflow_big = true;
			big_overflow = ip_id_big + interval;
		}

		/* Сֵ���,�϶�Ϊ���� */
		if (overflow_small) {
			linear = true;
		}else{
			/* Сֵ���ֵ����interval��,���� */
			if (ip_id_small + interval > ip_id_big) {
				linear = true;
			}else if(overflow_big && (big_overflow > ip_id_small)){
				/* ��ֵ������������ֵ����Сֵ������ */
				linear = true;
			}
		}
	}
	//UTT_NETSNIPER_DEBUG("ipd1 = %d ipd2 = %d linear%d.\n",ip_id1,ip_id2,linear);
	return linear;
}

static void ntsr_ip_id_identify(struct iphdr *ipH,UttNetSniper *netSniper){
	__u16 pkt_ip_id,last_ip_id;
	bool linear = false;
	pkt_ip_id = ntohs(ipH->id);
	/*���ip_id ��ʶ��������*/
	if ((netSniper->ntsr_flag & NTSR_IP_ID_FLAG) != 0U) {
		return;
	}
	last_ip_id = netSniper->ntsr_ip_id;
	linear = ntsr_ip_id_chk_linear(ipH,last_ip_id,pkt_ip_id);
	UTT_NETSNIPER_DEBUG("ipH->saddr = "NIPQUAD_FMT" ipid1:%d. ipid2:%d linear:%d\n",NIPQUAD(ipH->saddr),last_ip_id,pkt_ip_id,linear);
	/*idΪ0ʱ�����*/
	if ((netSniper->ntsr_ip_id != 0U) && (!linear)){
	/*������*/
		/*�����Դ�ۼ�*/
		netSniper->ntsr_ip_id_cnt++;
		UTT_NETSNIPER_DEBUG("ip_id_cnt %d.\n",netSniper->ntsr_ip_id_cnt);
		if(netSniper->ntsr_ip_id_cnt > ntsr_global_ipid_check_cnt){
			/*ip_idʶ��*/
			netSniper->ntsr_flag |= NTSR_IP_ID_FLAG;
			/*��λ����*/

			netSniper->ntsr_ip_id = 0U;
			netSniper->ntsr_ip_id_cnt = 0U;
			netSniper->ntsr_ip_id_identify_time = 0U;
		}else{
			netSniper->ntsr_ip_id = pkt_ip_id;
			/*��¼��һ�μ���ʱ��*/
			if(netSniper->ntsr_ip_id_cnt == 1U){
				UTT_NETSNIPER_DEBUG("ip_id_cnt first getcurTime%d.\n",get_current_time());
				netSniper->ntsr_ip_id_identify_time = get_current_time();
			}
		}
	}else{
	/*ip_idΪ0*/
		netSniper->ntsr_ip_id = pkt_ip_id;
	}


}

static void ntsr_ttl_identify(struct iphdr *ipH,UttNetSniper *netSniper){
	__u8 ttl = ipH->ttl;
	int i;
	bool found = false;

	if ((netSniper->ntsr_flag & NTSR_IP_TTL_FLAG) != 0U) {
		return;
	}

	//UTT_NETSNIPER_DEBUG("ipH->saddr = "NIPQUAD_FMT" ttl:%d.\n",NIPQUAD(ipH->saddr),ttl);
	for(i=0;i<DEF_USER_TTL_CNT;i++){
		if(ttl == def_user_ttl[i]){
			found = true;
			break;
		}
	}

	if (!found) {
		UTT_NETSNIPER_DEBUG("ttl not found.%d\n",ttl);
		netSniper->ntsr_flag |= NTSR_IP_TTL_FLAG;
	}

	return;


}

static __u32 rl_age(__u32 identify_time){
	return get_current_time() - identify_time;
}

static void ntsr_overtime(void) {
	int i;
	UttNfHashBuf **hashBufPP = NULL;/*����ָ��*/
	UttNfHashBuf *hashBufP = NULL;
	UttNetSniper  *netSniper = NULL;	
	for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++){
		hashBufPP = &(uttNfIpHashTable[i]);/*��λ����ͷ*/
		while(*hashBufPP != NULL){
		    /* ����ʱ�䵽�� */
			hashBufP = *hashBufPP;
			netSniper = &(hashBufP->conf.netSniper);
			if (netSniper->ntsr_flag == NTSR_FLAG) {
			/* ��ʱ */
				//UTT_NETSNIPER_DEBUG("ctl time count:%d.\n",rl_age(netSniper->ntsr_identify_time));
				if ((ntsr_global_control_time != 0U) && (rl_age(netSniper->ntsr_identify_time) > ntsr_global_control_time)) {
					/* �����ر�ʶ */
					netSniper->ntsr_flag = 0U; 
					netSniper->ntsr_identify_time = 0U; 
				}   
			}   
			/* ip id��Ϊ0��ʾ����ʶ��ip id�Ĺ����У����һ�ûʶ����� */
			else if (netSniper->ntsr_ip_id_cnt != 0U) {
			/* ip id ʶ��ʱ */
				if (rl_age(netSniper->ntsr_ip_id_identify_time) > ntsr_global_ipid_check_time_interval) {
				/* ���ʶ�� ip id ��ر�ʶ */
					netSniper->ntsr_ip_id_cnt = 0U; 
					netSniper->ntsr_ip_id_identify_time = 0U; 
				}   
			}   
			hashBufPP = &((*hashBufPP)->next);/*������һ����ͻ��*/
		}
	}
	mod_timer(&ntsrTimer, jiffies + HZ);/*���¼��ʱ��*/

}

static void ntsr_task(void){
    init_timer(&ntsrTimer);
	ntsrTimer.expires = jiffies + HZ;   /* ��ʱ����ʱʱ�Ľ�����*/
	ntsrTimer.data     = 0;                /* ����ʱ���������������0*/
	ntsrTimer.function = ntsr_overtime;      /* ��ʱ����ʱʱ���õĺ���*/
	add_timer(&ntsrTimer);/*���ʱ��*/
	return;
}

static void reset_flag(void){
	int i;
	UttNfHashBuf **hashBufPP = NULL;/*����ָ��*/
	UttNfHashBuf *hashBufP = NULL;
	UttNetSniper  *netSniper = NULL;    
	for(i = 0; i < MAX_UTTNF_HASH_TABLE; i++){
		hashBufPP = &(uttNfIpHashTable[i]);/*��λ����ͷ*/
		while(*hashBufPP != NULL){
			/* ����ʱ�䵽�� */
			hashBufP = *hashBufPP;
			netSniper = &(hashBufP->conf.netSniper);
			/* �����ر�ʶ */
			netSniper->ntsr_flag = 0U; 
			netSniper->ntsr_identify_time = 0U; 
			hashBufPP = &((*hashBufPP)->next);/*������һ����ͻ��*/
		}   
	}   
}

static void del_task(void){
	del_timer(&ntsrTimer);
	reset_flag();
	return;
}

static void del_host(char* delstr){
	char *tmp;
	UttNfHashConf *hashConf = NULL;
	UttNetSniper  *netSniper = NULL;	
	tmp = strsep(&delstr,",");
	while(tmp!=NULL&&*tmp!=0){
		UTT_NETSNIPER_DEBUG("kernel del host :%s\n",tmp);
		u_long ipaddr = in_aton(tmp);
		hashConf = uttNfHashConfGet(ipaddr,0);
		if( hashConf!=NULL ){
			UTT_NETSNIPER_DEBUG("kernel del \n");
			netSniper = &(hashConf->netSniper);
			/* �����ر�ʶ */
			netSniper->ntsr_flag = 0U; 
			netSniper->ntsr_identify_time = 0U; 
		}
		tmp = strsep(&delstr,",");
	}
}

void uttNlNetSniper(struct sk_buff *skb){
	struct nlmsghdr *nlh;
	UttNlNfNetSniperConf *nlData;
	NetSniperEntryVal entryVal;
	nlh = nlmsg_hdr(skb);
	nlData = (UttNlNfNetSniperConf*)NLMSG_DATA(nlh);
	entryVal = nlData->netSniperEntryVal;

	switch(nlData->confType){
		case UTT_NFCONF_INIT:
			UTT_NETSNIPER_DEBUG("Nl Init\n");
			ntsr_global_control_time = entryVal.ctlTime;
			UTT_NETSNIPER_DEBUG("ctlTime:%d\n",ntsr_global_control_time);
			ntsr_global_ipid_check_time_interval = entryVal.chkIntvl;
			UTT_NETSNIPER_DEBUG("chkIntl:%d\n",ntsr_global_ipid_check_time_interval);
			ntsr_global_ipid_linear_interval = entryVal.linearIntvl;
			UTT_NETSNIPER_DEBUG("linearIntl:%d\n",ntsr_global_ipid_linear_interval);
			ntsr_global_ipid_check_cnt = entryVal.chkCnt;
			UTT_NETSNIPER_DEBUG("chkCnt:%d\n",ntsr_global_ipid_check_cnt);
			ntsr_task();
			break;
		case UTT_NFCONF_FINI:
			del_task();
		case UTT_NFCONF_DEL:
			del_host(nlData->delstr);
		default:
			break;
	}
	return;
}


static unsigned int
xt_uttnetsniper_target4(struct sk_buff *skb,
		  const struct xt_target_param *par)
{

	struct iphdr *ipH = NULL;
	UttNfHashConf *hashConf = NULL;
	UttNetSniper  *netSniper = NULL;	
	ipH = ip_hdr(skb);
	if (ipH->protocol!=PROT_TCP){
		return XT_CONTINUE;
	}
	if(isSkbComeFromLan(skb)){
		hashConf = uttNfHashConfGet(ipH->saddr,1);
		if( hashConf!=NULL ){

	//UTT_NETSNIPER_DEBUG("%s-%d ipH->saddr = "NIPQUAD_FMT"ipH->daddr= "NIPQUAD_FMT" netsniper lastId:%d curId%d ttl:%d\n",__FUNCTION__,__LINE__,NIPQUAD(ipH->saddr),NIPQUAD(ipH->daddr),netSniper->ntsr_ip_id,ntohs(ipH->id),ipH->ttl);
	//UTT_NETSNIPER_DEBUG("ipH->saddr = "NIPQUAD_FMT" netsniper curId%d.\n",NIPQUAD(ipH->saddr),ntohs(ipH->id));
			//netSniper->ntsr_ip_id = ntohs(ipH->id);
			
			netSniper = &(hashConf->netSniper);
			/*��������Ƿ������������ƽ�*/
			ntsr_anti_flag(skb,netSniper,ipH);
			/*�ѱ����Ϊ��������*/

			if(netSniper->ntsr_flag == NTSR_FLAG){
				//UTT_NETSNIPER_DEBUG("ipH->saddr = "NIPQUAD_FMT" shared host XT_CONTINUE.\n",NIPQUAD(ipH->saddr));
				return XT_CONTINUE;
			}
			/*ip idʶ��*/
			ntsr_ip_id_identify(ipH,netSniper);
			/*ttlʶ��*/
			ntsr_ttl_identify(ipH,netSniper);
			
			if(netSniper->ntsr_flag == NTSR_FLAG){
				UTT_NETSNIPER_DEBUG("ipH->saddr = "NIPQUAD_FMT" shared host curTime:%d.\n",NIPQUAD(ipH->saddr),get_current_time());
				netSniper->ntsr_identify_time = get_current_time();
			}
			

		}
	}
    return XT_CONTINUE;
}

static bool
xt_uttnetsniper_checkentry4(const struct xt_tgchk_param *par)
{
	/*
	struct xt_uttnetsniper_info *info = (struct xt_uttnetsniper_info *)par->targinfo;
	ntsr_global_control_time = info->ctlTime;
	UTT_NETSNIPER_DEBUG("ctlTime:%d\n",ntsr_global_control_time);
	ntsr_global_ipid_check_time_interval = info->chkIntvl;
	UTT_NETSNIPER_DEBUG("chkIntl:%d\n",ntsr_global_ipid_check_time_interval);
	ntsr_global_ipid_linear_interval = info->linearIntvl;
	UTT_NETSNIPER_DEBUG("linearIntl:%d\n",ntsr_global_ipid_linear_interval);
	ntsr_global_ipid_check_cnt = info->chkCnt;
	UTT_NETSNIPER_DEBUG("chkCnt:%d\n",ntsr_global_ipid_check_cnt);
	*/
    return 1;
}

static void xt_uttnetsniper_destroy(const struct xt_tgdtor_param *par) {

    return;
}


static struct xt_target xt_uttnetsniper_reg[] = {
	{
		.family		= NFPROTO_IPV4,
		.name		= "UTTNETSNIPER",
		.checkentry	= xt_uttnetsniper_checkentry4,
		.destroy	= xt_uttnetsniper_destroy,
		.target		= xt_uttnetsniper_target4,
		.targetsize	= sizeof(int),
		.me		= THIS_MODULE,
	},
};

static int __init xt_uttnetsniper_init(void)
{
    return xt_register_targets(xt_uttnetsniper_reg, ARRAY_SIZE(xt_uttnetsniper_reg));
}

static void __exit xt_uttnetsniper_fini(void)
{
    xt_unregister_targets(xt_uttnetsniper_reg, ARRAY_SIZE(xt_uttnetsniper_reg));
}

module_init(xt_uttnetsniper_init);
module_exit(xt_uttnetsniper_fini);
