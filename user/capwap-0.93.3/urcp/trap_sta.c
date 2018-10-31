#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include <user_oid.h>
#include <mibUrcp.h>
#include "common.h"
#include "urcp.h"
#include "urcp_cmd.h"
#include "hash.h"
#include "trap_sta.h"
#include "urcp_ap.h"
#include "urcp_debug.h"
#ifdef URCP_AC
#include <switch.h>
#include <sw_cfg.h>
#include "urcp_ap.h"
#endif

#ifdef URCP_AC

extern urcp_shm_t* shm_ptr;
#ifdef URCP_STA_MAC_TABLE 
hash_table_info_t sta_table, *pStaTb=NULL;
static mtimer_t ac_sta_table_timer;

/***********************
 * 功能： 初始化STA表的各个数据结构
 * 参数： 无
 * 作者： KeHuatao
 * 日期： 2014-09-03
 **********************/
void ac_sta_table_init(void)
{
    /*获取table指针*/
    pStaTb = &sta_table; 
    /*清零*/
    memset(pStaTb, 0, sizeof(hash_table_info_t));
    pStaTb->pHash = (hash_entry_hdr_t*)(shm_ptr -> sta_tb_hash);
    pStaTb->pTb = (tb_entry_hdr_t*)(shm_ptr -> sta_tb);
    /*初始化节点大小和表大小*/
    pStaTb->unit = sizeof(sta_entry_t);
    pStaTb->hash_size = STA_TB_HASH_SIZE;
    pStaTb->table_size = STA_TB_SIZE;
    /*清零*/
    memset(shm_ptr->sta_tb_hash, 0, sizeof(shm_ptr->sta_tb_hash));
    memset(shm_ptr->sta_tb, 0, sizeof(shm_ptr->sta_tb));
#if (STA_ONOFF_LOG == FYES)
    int i = 0;
    pStaTb->first_free = 1;  /**/
    for (i = 0; i < STA_TB_SIZE - 1; i++)
    {
	shm_ptr->sta_tb[i].hdr.tb_next = i + 1;
    }
#endif
    /*启动定时器*/
    MTIMER_START(&ac_sta_table_timer, AC_STA_TABLE_TIME_MS);
}
void ac_sta_table_del(uchar* mac)
{
    sta_entry_t  *entry=NULL;
    db_info(URCP_MODULE_ID_TRAP_STA, "%s: del sta: %s \n",__func__, str_macaddr(mac));
    entry =  ac_sta_table_lookup(mac);
    if(entry!=NULL)
    {
        entry->hdr.aged = 2;
	entry = (sta_entry_t *)(pStaTb->pDst);
#if (STA_ONOFF_LOG == FYES)
	/*增加删除日志*/
	ac_sta_add_down_log(mac, entry->rx_bytes, entry->tx_bytes);
#endif
    }

}
static int32 ac_sta_table_add(cmd_hdr_sta_info_auto_t* cmd, uchar* ap_mac)
{
    int32 ret=HASH_RET_FAILED;
    int16 i, num, size;
    sta_entry_t  *entry=NULL;
    cmd_sta_stat_t *tmp=NULL;
    tb_entry_hdr_t tp;
    urcp_dev_t *pAp=NULL;
    num = ntohs(cmd -> entry_num);
    size = ntohs(cmd -> entry_size);
    uint32 *ip = NULL;

    pAp = urcp_dev_table_lookup(ap_mac, 0);
    if (pAp != NULL)
    {
	if (RF_FLAG_5G&ntohs(cmd->cmd_status))
	{
	    pAp->wl_info_5G.clients = ntohs(cmd->sta_num);
	}
	else
	{
	    pAp->wl_info.clients = ntohs(cmd->sta_num);
	}
    }
    db_info(URCP_MODULE_ID_TRAP_STA, "\n\n%s: num: %d, size: %d \n",__func__, num, size);
    if(num>0)
    {
	for(i=0;i<num;i++)
	{
	    tmp = (cmd_sta_stat_t*)( (uchar*)(cmd->sta_entry) + (size * i) );
	    memset(&tp, 0, sizeof(tb_entry_hdr_t));
	    memcpy(tp.mac_addr, tmp->addr, MAC_LEN);
	    ret = hash_add_entry(pStaTb, &tp);
	    if(ret != HASH_RET_FAILED)
	    {
//		db_info(URCP_MODULE_ID_STA_LOG, "%s,STA entry sta ok\n", __func__);
                entry = (sta_entry_t*)(pStaTb->pDst);
		entry -> phy.word = ntohs(tmp->phy);
		entry -> con_time = ntohl(tmp->con_time);
		entry -> rssi = tmp->rssi;
		entry -> ssid_idx = (tmp->ssid_idx)&0xFu;
		entry -> rate = ntohs(tmp->rate);
		entry -> rx_bytes = ntohl(tmp->rx_bytes);
		entry -> rx_rate = ntohl(tmp->rx_rate);
		entry -> tx_bytes = ntohl(tmp->tx_bytes);
		entry -> tx_rate = ntohl(tmp->tx_rate);
		entry -> flag = ntohs(cmd->cmd_status);
		memcpy(entry->ap_mac, ap_mac, MAC_LEN);
		if ((tmp->ssid_idx)&0x80u)
		{
		    db_info( URCP_MODULE_ID_STA_LOG, "%s, recv mac: %02x%02x%02x%02x%02x%02x, name:%s\n", __func__, tmp->addr[0], tmp->addr[1], tmp->addr[2],tmp->addr[3],tmp->addr[4], tmp->addr[5],(uchar *)tmp + sizeof(cmd_sta_stat_t));
		    /*AC储存AP发过来的sta host_name信息*/
		    strncpy(entry->host_name, (uchar *)tmp + sizeof(cmd_sta_stat_t), sizeof(entry->host_name));
		    ip = (uint32 *)((uchar *)tmp + sizeof(cmd_sta_stat_t) + sizeof(entry->host_name));
		    entry->ip = *ip;
		    db_info(URCP_MODULE_ID_STA_LOG ,"%s, ip:%s\n", __func__, inet_ntoa(entry->ip));
		}
		db_loud(URCP_MODULE_ID_TRAP_STA, "%s: mac: %s ,is5G:%d, time: %d, rssi:%d, ap_idx:%d, rate:%d, rb:%lld,rr:%d,tb:%lld,tr:%d\n",
			__func__, str_macaddr(entry->hdr.mac_addr), entry->flag, entry->con_time,entry->rssi, entry->ssid_idx, entry->rate
			,entry->rx_bytes, entry->rx_rate, entry->tx_bytes,entry->rx_rate);  
	    }
	}
    }
    return ret; 
}

sta_entry_t* ac_sta_table_lookup(uchar *mac)
{
    tb_entry_hdr_t tmp;
    sta_entry_t *entry = NULL;

    memset(&tmp, 0, sizeof(tb_entry_hdr_t));
    memcpy(tmp.mac_addr, mac, MAC_LEN);
    entry = (sta_entry_t*)hash_lookup_entry(pStaTb, &tmp);

    return entry;
}

void ac_sta_table_handle(void)
{
    int32 i, unit;
    int16 tb_index, preIdx;
    sta_entry_t* tmp=NULL, *tp=NULL;
#if (STA_ONOFF_LOG == FYES)
    uint32 node = 0;
#endif
    if(MTIMER_TIMEOUT(&ac_sta_table_timer))
    {
	db_info(URCP_MODULE_ID_STA_LOG, "%s, check\n", __func__);
        unit = pStaTb->unit;
	for(i = 0u; i < pStaTb->hash_size; i++)
	{
	    preIdx = NONE_NODE;
	    tb_index = pStaTb->pHash[i].idx;

#ifndef URCP_REL
	    if(tb_index!=NONE_NODE){
		db_loud(URCP_MODULE_ID_TRAP_STA, "%s: hash:%d, used idx:%d\n", __func__,i, tb_index);
	    }
#endif
	    while(tb_index!=NONE_NODE)
	    {
		tmp = (sta_entry_t*)( (char*)pStaTb->pTb + (unit * (tb_index-1)) );
		if(tmp->hdr.aged==0) {
		    tmp->hdr.aged = 1;
		    preIdx = tb_index;
#if (STA_ONOFF_LOG == FYES)
		    node = tmp->hdr.tb_next;
#endif
		    db_loud(URCP_MODULE_ID_TRAP_STA, "idx:%d, STA: %s\n", tb_index, str_macaddr(tmp->hdr.mac_addr));
		}
		else {
		    tmp->hdr.aged = 2;
		    tmp->hdr.used = 0;
		    db_loud(URCP_MODULE_ID_TRAP_STA, "idx:%d, preIdx:%d, STA: %s aged\n", tb_index, preIdx, str_macaddr(tmp->hdr.mac_addr));
		    /*delete*/
		    if(preIdx==NONE_NODE) {
			pStaTb->pHash[i].idx = tmp->hdr.tb_next;
			db_loud(URCP_MODULE_ID_TRAP_STA, "set pHash[%d].idx:%d\n", i, pStaTb->pHash[i].idx);
		    }
		    else {
		        tp = (sta_entry_t*)( (char*)pStaTb->pTb + (unit * (preIdx-1)) );
			tp->hdr.tb_next = tmp->hdr.tb_next;
			db_loud(URCP_MODULE_ID_TRAP_STA, "set preIdx:%d next to %d\n", preIdx, tp->hdr.tb_next);
		    }
#if (STA_ONOFF_LOG == FYES)
		    if (pStaTb->first_free != 0)
		    {
			db_info(URCP_MODULE_ID_STA_LOG, "%s, first:%d\n", __func__, pStaTb->first_free);
			node = tmp->hdr.tb_next;
			tmp->hdr.tb_next = pStaTb->first_free;
			pStaTb->first_free = tb_index - 1;
			db_info(URCP_MODULE_ID_STA_LOG, "%s, first:%d, next:%d\n",__func__, pStaTb->first_free,\
				((sta_entry_t*)((char*)pStaTb->pTb + (unit*(pStaTb->first_free))))->hdr.tb_next);
		    }
#endif
		}
#if (STA_ONOFF_LOG == FYES)
		tb_index = node;
#else
		tb_index = tmp->hdr.tb_next;
#endif
	    }
	}

        MTIMER_START(&ac_sta_table_timer, AC_STA_TABLE_TIME_MS);
    }
}

uint16 receive_ap_mac_table(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    cmd_hdr_sta_info_auto_t *header=NULL;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
        header = (cmd_hdr_sta_info_auto_t*)cmd;
        ac_sta_table_add(header, urcp_info->mac);
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#endif

#ifdef URCP_AP

#if AP_SEND_CLTS_AUTO

#if (CLIENT_HOST_NAME == FYES)
#define MAX_TRAP_STA_ONE_CMD 20 /*一次最多发送40个STA信息*/
#else
#define MAX_TRAP_STA_ONE_CMD 40 /*一次最多发送40个STA信息*/
#endif
static uint32 trap_sta_enable = 1u;
static int32 ap_trap_sta_timeout = 1u;
static uint32 next_sta_idx=0u;
static RT_SSID_STA_INFO_TABLE mac_table ={0};
static mtimer_t ap_trap_sta_timer;

#if (CLIENT_HOST_NAME == FYES)
static hash_table_info_t sta_table_ap; 
hash_table_info_t *pStaTb_ap=NULL;
static hash_entry_hdr_t sta_tb_hash_ap[STA_TB_HASH_SIZE_AP];
static sta_entry_ap_t sta_tb_ap[STA_TB_SIZE_AP];
#endif

#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
static uint32 next_sta_5g_idx=0u;
static int32 ap_trap_sta_5g_timeout = 1u;
static mtimer_t ap_trap_sta_5g_timer;
static RT_SSID_STA_INFO_TABLE mac_table_5g ={0};
#endif
extern ap_info_t ap_infor;

#if (CLIENT_HOST_NAME == FYES)
void ap_sta_table_init(void)
{
    /*获取table指针*/
    pStaTb_ap = &sta_table_ap; 
    /*清零*/
    memset(pStaTb_ap, 0, sizeof(hash_table_info_t));
    pStaTb_ap->pHash = (hash_entry_hdr_t*)(sta_tb_hash_ap);
    pStaTb_ap->pTb = (tb_entry_hdr_t*)(sta_tb_ap);
    /*初始化节点大小和表大小*/
    pStaTb_ap->unit = sizeof(sta_entry_ap_t);
    pStaTb_ap->hash_size = STA_TB_HASH_SIZE_AP;
    pStaTb_ap->table_size = STA_TB_SIZE_AP;
    /*清零*/
    memset(sta_tb_hash_ap, 0, sizeof(sta_tb_hash_ap));
    memset(sta_tb_ap, 0, sizeof(sta_tb_ap));
}

sta_entry_ap_t* ap_sta_table_lookup(uchar *mac)
{
    tb_entry_hdr_t tmp;
    sta_entry_ap_t *entry = NULL;

    memset(&tmp, 0, sizeof(tb_entry_hdr_t));
    memcpy(tmp.mac_addr, mac, MAC_LEN);
    entry = (sta_entry_ap_t*)hash_lookup_entry(pStaTb_ap, &tmp);

    return entry;
}

sta_entry_ap_t* sta_mac_search(uchar *mac)
{
    int i = 0;
    sta_entry_ap_t *pSta = NULL;
    db_info(URCP_MODULE_ID_HOST_NAME, "%s, search mac:%02x%02x%02x%02x%02x%02x\n", __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    for (i = 0; i < STA_TB_SIZE_AP; i++)
    {
	pSta = &sta_tb_ap[i];
	if ((pSta->hdr.used != 0) && ( mac_cmp(pSta->hdr.mac_addr, mac) == 0))
	{
	    break;
	}
    }
    if (i == STA_TB_SIZE_AP)
    {
	pSta = NULL;
    }
    return pSta;
}

void ap_sta_check(uint32 flag)
{
#if 0
    int i = 0;
    sta_entry_ap_t *pSta = NULL;
    for (i = 0; i < STA_TB_SIZE_AP; i++)
    {
	pSta = &sta_tb_ap[i];
	if ((pSta->hdr.used != 0) && (pSta->hdr.aged == 1) && (pSta->flag == flag))
	{
	    pSta->hdr.aged = 2;	
	}
	else if ((pSta->hdr.used != 0) && (pSta->hdr.aged == 2) && (pSta->flag == flag))
	{
	    pSta->flag = 0;
	    pSta->hdr.aged = 0;
	    db_info(URCP_MODULE_ID_HOST_NAME,"%s, del mac:%02x%02x%02x%02x%02x%02x\n", __func__, pSta->hdr.mac_addr[0],  pSta->hdr.mac_addr[1], pSta->hdr.mac_addr[2],\
		     pSta->hdr.mac_addr[3], pSta->hdr.mac_addr[4], pSta->hdr.mac_addr[5]);
	    memset(pSta->hdr.mac_addr, 0, MAC_LEN);
	    memset(pSta->host_name, 0, sizeof(pSta->host_name));
	    pSta->ip = 0;
	    pSta->hdr.used = 0;
	}
    }
#else
    int32 i, unit;
    int16 tb_index, preIdx, next = 0;
    sta_entry_ap_t* tmp=NULL, *tp=NULL;

    db_info(URCP_MODULE_ID_HOST_NAME, "%s,  sta mac check\n", __func__);
    unit = pStaTb_ap->unit;
    for(i = 0u; i < pStaTb_ap->hash_size; i++)
    {
	preIdx = NONE_NODE;
	tb_index = pStaTb_ap->pHash[i].idx;
	
	db_info(URCP_MODULE_ID_HOST_NAME, "%s, i : %d, start:%d->>",__func__, i, tb_index);
	while(tb_index != NONE_NODE)
	{
	    db_info(URCP_MODULE_ID_HOST_NAME, " %d ", tb_index);
	    tmp = (sta_entry_ap_t*)( (char*)pStaTb_ap->pTb + (unit * (tb_index-1)) );
	    if((tmp->hdr.used != 0) && (tmp->hdr.aged == 1) && (tmp->flag == flag)) 
	    {
		tmp->hdr.aged = 2;
		preIdx = tb_index;
		next = tmp->hdr.tb_next;
	    }
	    else if ( (tmp->hdr.used != 0) && (tmp->hdr.aged == 2) && (tmp->flag == flag) ) 
	    {
		tmp->hdr.aged = 0;
		tmp->flag = 0;
		memset(tmp->hdr.mac_addr, 0, MAC_LEN);
		memset(tmp->host_name, 0, sizeof(tmp->host_name));
		tmp->ip = 0;

		db_info(URCP_MODULE_ID_HOST_NAME, "idx:%d, preIdx:%d, STA: %s aged\n", tb_index, preIdx, str_macaddr(tmp->hdr.mac_addr));
		/*delete*/
		if(preIdx == NONE_NODE) 
		{
		    pStaTb_ap->pHash[i].idx = tmp->hdr.tb_next;
		    next = tmp->hdr.tb_next;
		    tmp->hdr.tb_next = NONE_NODE;
		    tmp->hdr.used = 0;
		    db_info(URCP_MODULE_ID_HOST_NAME, "Del head set pHash[%d].idx:%d\n", i, pStaTb_ap->pHash[i].idx);
		}
		else 
		{
		    tp = (sta_entry_ap_t*)( (char*)pStaTb_ap->pTb + (unit * (preIdx-1)) );
		    tp->hdr.tb_next = tmp->hdr.tb_next;
		    next = tmp->hdr.tb_next;
		    tmp->hdr.tb_next = NONE_NODE;		/*删除该节点后清空向下的指向*/ 
		    tmp->hdr.used = 0;
		    db_info(URCP_MODULE_ID_HOST_NAME, "Del normal set preIdx:%d next to %d\n", preIdx, tp->hdr.tb_next);
		}
	    }
	    else
	    {
		preIdx = tb_index;
		next = tmp->hdr.tb_next;
	    }
	    tb_index = next;
	}
	db_info(URCP_MODULE_ID_HOST_NAME, "\n");
    }

#endif
}

void ap_sta_set(uint32 num, uint32 flag, uchar *mac)
{
    int i = 0;
    RT_SSID_STA_INFO_TABLE *mac_list = NULL;
    RT_SSID_STA_INFO_ENTRY *pEntry=NULL;
    sta_entry_ap_t *entry = NULL;
    tb_entry_hdr_t tp;
    int ret = 0;

    if (mac != NULL)
    {
	/*根据mac查找hash*/
	entry = ap_sta_table_lookup(mac);
	if (entry != NULL)
	{
	    db_info(URCP_MODULE_ID_HOST_NAME,"%s,*#*#*# find mac:%02x%02x%02x%02x%02x%02x\n",__func__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	    entry->hdr.aged = num;
	    entry->flag = flag;
	}
	else
	{
	    if ( ( entry = sta_mac_search(mac)) != NULL)
	    {
		/*跑到这里是有问题的*/
		db_info(URCP_MODULE_ID_HOST_NAME,"%s,*#*#*# lost, search mac:%02x%02x%02x%02x%02x%02x\n",__func__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		entry->hdr.aged = num;
		entry->flag = flag;
	    }
	    else
	    {
		db_info(URCP_MODULE_ID_HOST_NAME,"%s,*#*#*# lost mac:%02x%02x%02x%02x%02x%02x\n",__func__, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		memset(&tp, 0, sizeof(tb_entry_hdr_t));
		memcpy(tp.mac_addr, mac, MAC_LEN);
		ret = hash_add_entry(pStaTb_ap, &tp);
		db_info(URCP_MODULE_ID_HOST_NAME,"%s,add ret = %d\n", __func__, ret);
		if (ret != HASH_RET_FAILED)
		{
		    entry = (sta_entry_ap_t*)(pStaTb_ap->pDst);
		    entry->flag = flag;
		    entry->hdr.aged = num;
		    //		entry->ip = 0;
		    //		memset(entry->host_name, 0, CLIENT_HOST_NAME_SIZE);
		}
	    }
	}
    }
    else
    {
	/*查找全部*/
	if (flag == RF_FLAG_24G)
	{
	    mac_list = &mac_table;
	}
	else
	{
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
	    mac_list = &mac_table_5g;
#endif
	}
	if (mac_list != NULL)
	{
	    for (i = 0; i < mac_list->Num; i++)
	    {
		pEntry =&mac_list->Entry[i];  
		ap_sta_set(num, flag, pEntry->StaMac);
	    }
	}
    }
}

void ap_get_host_by_mac(uchar *mac, uchar *name)
{
    sta_entry_ap_t *entry = NULL;
    uint32 *ip = NULL;
    if ((mac != NULL) && (name != NULL))
    {
	entry = ap_sta_table_lookup(mac);
	if ((entry != NULL) && (entry->host_name[0] != '\0'))
	{
	    db_info(URCP_MODULE_ID_HOST_NAME,"%s, send name:%s\n", __func__, entry->host_name);
	    strncpy(name, entry->host_name, sizeof(entry->host_name));
	    ip = (uint32 *)(name + sizeof(entry->host_name));
	    *ip = entry->ip;
	}
	else
	{
	    if ( ( entry = sta_mac_search(mac)) != NULL)
	    {
		db_info(URCP_MODULE_ID_HOST_NAME,"%s, *#*#*# lost, get name search mac:%02x%02x%02x%02x%02x%02x, name:%s\n",__func__, \
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], entry->host_name);
		strncpy(name, entry->host_name, sizeof(entry->host_name));
		ip = (uint32 *)(name + sizeof(entry->host_name));
		*ip = entry->ip;
	    }
	    else
	    {
		name[0] = '\0';
	    }
	}
    }
} 
#endif

void ap_trap_sta_timer_init(void)
{
    MTIMER_START(&ap_trap_sta_timer, 30000);
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
    MTIMER_START(&ap_trap_sta_5g_timer, 30000);
#endif
}

void ap_trap_sta_enable(uint32 enable)
{
//    MTIMER_START(&ap_collect_timer, timer_sec*1000);
    trap_sta_enable = enable;
}

void ap_trap_sta_timer_set(uint32 sec)
{
    MTIMER_START(&ap_trap_sta_timer, sec * 1000u);
}

#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
void ap_trap_sta_5g_timer_set(uint32 sec)
{
    MTIMER_START(&ap_trap_sta_5g_timer, sec * 1000u);
}
#endif


int make_rand_num(int m,int n)
{
    struct timeval tpstart;
    int ret;

    gettimeofday(&tpstart,NULL);
    srand(tpstart.tv_usec);
    ret = m + rand() % (n - m);
    
    DBCK("rand_num=%d,rand=%d\n",ret,rand());

    return ret;
}

void get_ap_trap_sta_num(uint32 flag, int num)
{
    uchar buffer[URCP_BUF_SIZE];
    uint32 len;
    urcp_hdr_t *header;
    cmd_hdr_t *cmd;
    int iftag = 0;
    memset(buffer, 0, sizeof(buffer));
    iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);
    if(iftag == 1)
    {   
	/*指向包头 带tag*/
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);
	len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*联动发现 */
	cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
    }   
    else
    {   
	/*指向包头*/
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);
	len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	/*联动发现 */
	cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
    }

    if (flag == RF_FLAG_24G)
    {
	cmd -> type = htonl(TRAP_AP_CLIENTS);
    }
    else if(flag == RF_FLAG_5G)
    { 
	cmd -> type = htonl(TRAP_AP_CLIENTS_5G);
    }
    cmd -> cmd_status = htons(URCP_OK);
    *((uint32*)((uchar *)cmd + sizeof(cmd_hdr_t))) = htonl(num);
    cmd -> length = htons(sizeof(cmd_hdr_t) + sizeof(uint32));

    len += sizeof(cmd_hdr_t) + sizeof(uint32);
    header -> cmd_no += 1u; 

    urcp_send(0u, &(buffer[0]), len);
}

void ap_trap_sta_poll(void)
{
    int ret=-1, val;
    uchar buffer[URCP_BUF_SIZE];
    char rssi=0, j;
    uint32 len, s, i, last_sta_idx=0u;
    urcp_hdr_t *header;
    cmd_hdr_sta_info_auto_t *cmd;
    cmd_sta_stat_t *sta_info;
    RT_SSID_STA_INFO_ENTRY *pEntry=NULL;
    struct iwreq iwr;

    if(MTIMER_TIMEOUT(&ap_trap_sta_timer))
    {
        if((trap_sta_enable==1) && (isACValid()==1))
	{
	    memset(buffer, 0, sizeof(buffer));
	    urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);

	    header = (urcp_hdr_t *)(&buffer[ETH_URCP_OFFSET]);
	    len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t);

	    cmd = (cmd_hdr_sta_info_auto_t *)(&buffer[len]);
	    cmd->type = htonl(TRAP_AP_MAC_TABLE);
	    cmd->cmd_status = htons(URCP_OK);
	    len += sizeof(cmd_hdr_sta_info_auto_t);

	    if(next_sta_idx==0u)
	    {
		s = socket(AF_INET, SOCK_DGRAM, 0);

		if (s >= 0) {
		    /*read the mac table*/
		    memset(&mac_table, 0, sizeof(RT_SSID_STA_INFO_TABLE));
		    strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
		    iwr.u.data.pointer = (caddr_t) &mac_table;
		    ret = ioctl(s, RTPRIV_IOCTL_SSID_STA_INFO, &iwr);
		    close(s);
#if (CLIENT_HOST_NAME == FYES)
		    ap_sta_set(0, RF_FLAG_24G, NULL);
		    ap_sta_check(RF_FLAG_24G);
		    db_info(URCP_MODULE_ID_HOST_NAME,"%s, check over\n",__func__);
#endif
		}
		if(ret < 0)/*error*/
		{
		    mac_table.Num = 0;
		    db_error(URCP_MODULE_ID_TRAP_STA, "%s: read table error\n",__func__);
#if (CLIENT_HOST_NAME == FYES)
		    ap_sta_check(RF_FLAG_24G);
#endif
		}
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: read table size:%d\n",__func__, mac_table.Num);
	    }

	    last_sta_idx = next_sta_idx + MAX_TRAP_STA_ONE_CMD;
	    if(last_sta_idx > mac_table.Num) {
		last_sta_idx = mac_table.Num;
	    }

	    if(mac_table.Num>0)
	    {
		cmd->table_offset = htons(next_sta_idx);
		cmd->sta_num = htons(mac_table.Num);
		cmd->entry_num = (uint16)(last_sta_idx - next_sta_idx);
		cmd->entry_num = htons(cmd->entry_num);
#if (CLIENT_HOST_NAME == FYES)
		cmd->entry_size = htons(sizeof(cmd_sta_stat_t) + CLIENT_HOST_NAME_SIZE + sizeof(uint32));
#else
		cmd->entry_size = htons(sizeof(cmd_sta_stat_t));
#endif
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: will send sta, offset:%d, num:%d, unit: %d \n",__func__, 
			next_sta_idx, (last_sta_idx-next_sta_idx), sizeof(cmd_sta_stat_t));
	    }
	    for(i=next_sta_idx; i<last_sta_idx; i++){
		pEntry = &mac_table.Entry[i];
#if 1
		db_info(URCP_MODULE_ID_TRAP_STA, "STA:%02X%02X%02X-%02X%02X%02X rss0=%d,rss1=%d, rss2=%d,time=%d,mcs=%d,rate=%ld,tp=%d, rp=%d, tb=%d,rb=%d,rxRate=%d, txRate=%d\n", 
			pEntry->StaMac[0],pEntry->StaMac[1],pEntry->StaMac[2],
			pEntry->StaMac[3],pEntry->StaMac[4],pEntry->StaMac[5],
			pEntry->Rssi0, pEntry->Rssi1, pEntry->Rssi2, 
			pEntry->ConnectedTime, pEntry->TxStat.field.MCS, pEntry->Rate,
			pEntry->TxPackets, pEntry->RxPackets,
			pEntry->TxBytes, pEntry->RxBytes, pEntry->RxDataRate,pEntry->TxDataRate);
#endif
		j = 0;
		val = 0;
		if(pEntry->Rssi0 !=0 ) {
		    j++;
		    val += pEntry->Rssi0;
		}
		if(pEntry->Rssi1 !=0 ) {
		    j++;
		    val += pEntry->Rssi1;
		}
		if(pEntry->Rssi2 !=0 ) {
		    j++;
		    val += pEntry->Rssi2;
		}
		if(j>0){
		    rssi =  (char)(val / j);
		}

		sta_info = (cmd_sta_stat_t*)(&buffer[len]);
		memcpy(sta_info->addr, pEntry->StaMac, MAC_LEN);
		sta_info->phy = htons(pEntry->TxStat.word);
		sta_info->con_time = htonl(pEntry->ConnectedTime);
		sta_info->rssi = rssi;
		sta_info->ssid_idx = pEntry->ApIdx;
#if (CLIENT_HOST_NAME == FYES)
		sta_info->ssid_idx = (sta_info->ssid_idx)|0x80u;
#endif
		sta_info->rate = (uint16)(pEntry->Rate);
		sta_info->rate = htons(sta_info->rate);
		sta_info->rx_bytes = htonl(pEntry->RxBytes);
		sta_info->rx_rate = htonl(pEntry->RxDataRate);
		sta_info->tx_bytes = htonl(pEntry->TxBytes);
		sta_info->tx_rate = htonl(pEntry->TxDataRate);
#if (CLIENT_HOST_NAME == FYES)
		len += sizeof(cmd_sta_stat_t) + CLIENT_HOST_NAME_SIZE + sizeof(uint32);
		ap_get_host_by_mac(pEntry->StaMac, (uchar *)sta_info + sizeof(cmd_sta_stat_t));
		ap_sta_set(1, RF_FLAG_24G, pEntry->StaMac);
#else
		len += sizeof(cmd_sta_stat_t);
#endif
	    }
	    if(last_sta_idx >= mac_table.Num) {
		next_sta_idx = 0u;
		/*已经发完了，等待较长时间再发送*/
		ap_trap_sta_timeout = make_rand_num(15,30);
	    }
	    else {
		next_sta_idx = last_sta_idx;
		/*未发完，等待较短时间再发送*/
		ap_trap_sta_timeout = 2;
	    }

	    if(mac_table.Num>0)
	    {
		cmd->length = htons((len - ETH_URCP_OFFSET - sizeof(urcp_hdr_t)));
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: send len: %d\n", __func__, len);
		urcp_send(0u, &(buffer[0]), len);
//		get_ap_trap_sta_num(RF_FLAG_24G, mac_table.Num);
	    }
	}
	else {
            ap_trap_sta_timeout = 2;
	}
	ap_trap_sta_timeout = ap_trap_sta_timeout * 1000;
	db_loud(URCP_MODULE_ID_TRAP_STA, "%s: next timer: %dms\n", __func__, ap_trap_sta_timeout);
	MTIMER_START(&ap_trap_sta_timer, ap_trap_sta_timeout);
    }
}

#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
void ap_trap_sta_5g_poll(void)
{
    int ret=-1, val;
    uchar buffer[URCP_BUF_SIZE];
    char rssi=0, j;
    uint32 len, s, i, last_sta_idx=0u;
    urcp_hdr_t *header;
    cmd_hdr_sta_info_auto_t *cmd;
    cmd_sta_stat_t *sta_info;
    RT_SSID_STA_INFO_ENTRY *pEntry=NULL;
    struct iwreq iwr;

    if(MTIMER_TIMEOUT(&ap_trap_sta_5g_timer))
    {
        if((trap_sta_enable==1) && (isACValid()==1))
	{
	    memset(buffer, 0, sizeof(buffer));
	    urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);

	    header = (urcp_hdr_t *)(&buffer[ETH_URCP_OFFSET]);
	    len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t);

	    cmd = (cmd_hdr_sta_info_auto_t *)(&buffer[len]);
	    cmd->type = htonl(TRAP_AP_MAC_TABLE);
	    cmd->cmd_status = htons(RF_FLAG_5G);
	    len += sizeof(cmd_hdr_sta_info_auto_t);

	    if(next_sta_5g_idx==0u)
	    {
		s = socket(AF_INET, SOCK_DGRAM, 0);

		if (s >= 0) {
		    /*read the mac table*/
		    memset(&mac_table_5g, 0, sizeof(RT_SSID_STA_INFO_TABLE));
		    strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
		    iwr.u.data.pointer = (caddr_t) &mac_table_5g;
		    ret = ioctl(s, RTPRIV_IOCTL_SSID_STA_INFO, &iwr);
		    close(s);
#if (CLIENT_HOST_NAME == FYES)
		    ap_sta_set(0, RF_FLAG_5G, NULL);
		    ap_sta_check(RF_FLAG_5G);
#endif
		}
		if(ret < 0)/*error*/
		{
		    mac_table_5g.Num = 0;
		    db_error(URCP_MODULE_ID_TRAP_STA, "%s: read table error\n",__func__);
#if (CLIENT_HOST_NAME == FYES)
		    ap_sta_check(RF_FLAG_5G);
#endif
		}
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: read table size:%d\n",__func__, mac_table_5g.Num);
	    }

	    last_sta_idx = next_sta_5g_idx + MAX_TRAP_STA_ONE_CMD;
	    if(last_sta_idx > mac_table_5g.Num) {
		last_sta_idx = mac_table_5g.Num;
	    }

	    if(mac_table_5g.Num>0)
	    {
		cmd->table_offset = htons(next_sta_5g_idx);
		cmd->sta_num = htons(mac_table_5g.Num);
		cmd->entry_num = (uint16)(last_sta_idx - next_sta_5g_idx);
		cmd->entry_num = htons(cmd->entry_num);
#if (CLIENT_HOST_NAME == FYES)
		cmd->entry_size = htons(sizeof(cmd_sta_stat_t) + CLIENT_HOST_NAME_SIZE + sizeof(uint32));
#else
		cmd->entry_size = htons(sizeof(cmd_sta_stat_t));
#endif
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: will send sta, offset:%d, num:%d, unit: %d \n",__func__, 
			next_sta_5g_idx, (last_sta_idx-next_sta_5g_idx), sizeof(cmd_sta_stat_t));
	    }
	    for(i=next_sta_5g_idx; i<last_sta_idx; i++){
		pEntry = &mac_table_5g.Entry[i];
#if 1
		db_info(URCP_MODULE_ID_TRAP_STA, "STA:%02X%02X%02X-%02X%02X%02X rss0=%d,rss1=%d, rss2=%d,time=%d,mcs=%d,rate=%ld,tp=%d, rp=%d, tb=%d,rb=%d,rxRate=%d, txRate=%d\n", 
			pEntry->StaMac[0],pEntry->StaMac[1],pEntry->StaMac[2],
			pEntry->StaMac[3],pEntry->StaMac[4],pEntry->StaMac[5],
			pEntry->Rssi0, pEntry->Rssi1, pEntry->Rssi2, 
			pEntry->ConnectedTime, pEntry->TxStat.field.MCS, pEntry->Rate,
			pEntry->TxPackets, pEntry->RxPackets,
			pEntry->TxBytes, pEntry->RxBytes, pEntry->RxDataRate,pEntry->TxDataRate);
#endif
		j = 0;
		val = 0;
		if(pEntry->Rssi0 !=0 ) {
		    j++;
		    val += pEntry->Rssi0;
		}
		if(pEntry->Rssi1 !=0 ) {
		    j++;
		    val += pEntry->Rssi1;
		}
		if(pEntry->Rssi2 !=0 ) {
		    j++;
		    val += pEntry->Rssi2;
		}
		if(j>0){
		    rssi =  (char)(val / j);
		}

		sta_info = (cmd_sta_stat_t*)(&buffer[len]);
		memcpy(sta_info->addr, pEntry->StaMac, MAC_LEN);
		sta_info->phy = htons(pEntry->TxStat.word);
		sta_info->con_time = htonl(pEntry->ConnectedTime);
		sta_info->rssi = rssi;
		sta_info->ssid_idx = pEntry->ApIdx;
#if (CLIENT_HOST_NAME == FYES)
		sta_info->ssid_idx = (sta_info->ssid_idx)|0x80u;
#endif
		sta_info->rate = (uint16)(pEntry->Rate);
		sta_info->rate = htons(sta_info->rate);
		sta_info->rx_bytes = htonl(pEntry->RxBytes);
		sta_info->rx_rate = htonl(pEntry->RxDataRate);
		sta_info->tx_bytes = htonl(pEntry->TxBytes);
		sta_info->tx_rate = htonl(pEntry->TxDataRate);
#if (CLIENT_HOST_NAME == FYES)
		len += sizeof(cmd_sta_stat_t) + CLIENT_HOST_NAME_SIZE + sizeof(uint32);
		ap_get_host_by_mac(pEntry->StaMac, (uchar *)sta_info + sizeof(cmd_sta_stat_t));
		ap_sta_set(1, RF_FLAG_5G, pEntry->StaMac);
#else
		len += sizeof(cmd_sta_stat_t);
#endif
	    }
	    if(last_sta_idx >= mac_table_5g.Num) {
		next_sta_5g_idx = 0u;
		/*已经发完了，等待较长时间再发送*/
		ap_trap_sta_5g_timeout = make_rand_num(15,30);
	    }
	    else {
		next_sta_5g_idx = last_sta_idx;
		/*未发完，等待较短时间再发送*/
		ap_trap_sta_5g_timeout = 2;
	    }

	    if(mac_table_5g.Num>0)
	    {
		cmd->length = htons((len - ETH_URCP_OFFSET - sizeof(urcp_hdr_t)));
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: send len: %d\n", __func__, len);
		urcp_send(0u, &(buffer[0]), len);	
//		get_ap_trap_sta_num(RF_FLAG_5G, mac_table_5g.Num);
	    }
	}
	else {
            ap_trap_sta_5g_timeout = 2;
	}
	ap_trap_sta_5g_timeout = ap_trap_sta_5g_timeout * 1000;
	db_loud(URCP_MODULE_ID_TRAP_STA, "%s: next timer: %dms\n", __func__, ap_trap_sta_5g_timeout);
	MTIMER_START(&ap_trap_sta_5g_timer, ap_trap_sta_5g_timeout);
    }
}
#endif
#if 0
 #define RTPRIV_IOCTL_STATISTICS                     (SIOCIWFIRSTPRIV + 0x09)
void get_ap_stat(void)
{
    int s;
    struct iwreq iwr;
    char buf[1024];

    s = socket(AF_INET, SOCK_DGRAM, 0);

    if (s >= 0) {
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
	iwr.u.data.pointer = (void *) buf;
	memset(buf,0,sizeof(buf));
	if(ioctl(s, RTPRIV_IOCTL_STATISTICS, &iwr)==0)
	{
	    printf("%s: buf:%s\n", __func__, buf);
	}
	close(s);

    }
}
#endif
#endif
#endif

