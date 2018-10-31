#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include <user_oid.h>
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

void ac_sta_table_init(void)
{
    pStaTb = &sta_table; 
    memset(pStaTb, 0, sizeof(hash_table_info_t));
    pStaTb->pHash = (hash_entry_hdr_t*)(shm_ptr -> sta_tb_hash);
    pStaTb->pTb = (tb_entry_hdr_t*)(shm_ptr -> sta_tb);
    pStaTb->unit = sizeof(sta_entry_t);
    pStaTb->hash_size = STA_TB_HASH_SIZE;
    pStaTb->table_size = STA_TB_SIZE;
    memset(shm_ptr->sta_tb_hash, 0, sizeof(shm_ptr->sta_tb_hash));
    memset(shm_ptr->sta_tb, 0, sizeof(shm_ptr->sta_tb));
    MTIMER_START(&ac_sta_table_timer, AC_STA_TABLE_TIME_MS);
}
void ac_sta_table_del(uchar* mac)
{
    sta_entry_t *entry=NULL;

    db_info(URCP_MODULE_ID_TRAP_STA, "%s: del sta: %s \n",__func__, str_macaddr(mac));
    entry =  ac_sta_table_lookup(mac);
    if(entry!=NULL)
    {
        entry->hdr.aged = 2;
    }
}
static int32 ac_sta_table_add(cmd_hdr_sta_info_auto_t* cmd, char* ap_mac)
{
    int32 ret=HASH_RET_FAILED;
    int16 i, num, size;
    sta_entry_t  *entry=NULL;
    cmd_sta_stat_t *tmp=NULL;
    tb_entry_hdr_t tp;

    num = ntohs(cmd -> entry_num);
    size = ntohs(cmd -> entry_size);
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
		memcpy(entry->ap_mac, ap_mac, MAC_LEN);
		db_loud(URCP_MODULE_ID_TRAP_STA, "%s: mac: %s time: %d, rssi:%d, ap_idx:%d, rate:%d, rb:%lld,rr:%d,tb:%lld,tr:%d\n",
			__func__, str_macaddr(entry->hdr.mac_addr), entry->con_time,entry->rssi, entry->ssid_idx, entry->rate
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

    if(MTIMER_TIMEOUT(&ac_sta_table_timer))
    {
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
		}
		tb_index = tmp->hdr.tb_next;
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

#define MAX_TRAP_STA_ONE_CMD 40 /*一次最多发送40个STA信息*/

static uint32 trap_sta_enable = 1u;
static int32 ap_trap_sta_timeout = 1u;
static uint32 next_sta_idx=0u;
static RT_SSID_STA_INFO_TABLE mac_table ={0};
static mtimer_t ap_trap_sta_timer;

extern ap_info_t ap_infor;

void ap_trap_sta_timer_init(void)
{
    MTIMER_START(&ap_trap_sta_timer, 30000);
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
        if((trap_sta_enable==1) && (isValidMac(ap_infor.ac_infor.mac)==1))
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
		}
		if(ret < 0)/*error*/
		{
		    mac_table.Num = 0;
		    db_error(URCP_MODULE_ID_TRAP_STA, "%s: read table error\n",__func__);
		}
		db_info(URCP_MODULE_ID_TRAP_STA, "%s: read table size:%d\n",__func__, mac_table.Num);
	    }

	    last_sta_idx = next_sta_idx + MAX_TRAP_STA_ONE_CMD;
	    if(last_sta_idx > mac_table.Num) {
		last_sta_idx = mac_table.Num;
	    }

	    if(mac_table.Num>0)
	    {
		cmd->table_offset = htonl(next_sta_idx);
		cmd->entry_num = (uint16)(last_sta_idx - next_sta_idx);
		cmd->entry_num = htons(cmd->entry_num);
		cmd->entry_size = htons(sizeof(cmd_sta_stat_t));
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
		sta_info->rate = (uint16)(pEntry->Rate);
		sta_info->rate = htons(sta_info->rate);
		sta_info->rx_bytes = htonl(pEntry->RxBytes);
		sta_info->rx_rate = htonl(pEntry->RxDataRate);
		sta_info->tx_bytes = htonl(pEntry->TxBytes);
		sta_info->tx_rate = htonl(pEntry->TxDataRate);
		len += sizeof(cmd_sta_stat_t);
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
	    }
	}
	else {
            ap_trap_sta_timeout = 2;
	}
	ap_trap_sta_timeout = ap_trap_sta_timeout * 1000;
	db_info(URCP_MODULE_ID_TRAP_STA, "%s: next timer: %dms\n", __func__, ap_trap_sta_timeout);
	MTIMER_START(&ap_trap_sta_timer, ap_trap_sta_timeout);
    }
}
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

