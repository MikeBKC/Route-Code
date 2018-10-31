#include <string.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include  <linux/types.h>
#include  <linux/socket.h>
#include  <linux/if.h>
#include <linux/wireless.h>
#include <dirent.h>
#include <time.h>
#include "common.h"
#include <mib.h>
#include <profacce.h>
#ifdef URCP_AC
#include <switch.h>
#include <sw_cfg.h>
#include "urcp_ap.h"
#endif
#include <mibUrcp.h>
#include "urcp.h"
#include "urcp_cmd.h"
#include "app_msg.h"
#ifdef URCP_AP
#include "urcp_config.h"
#include "app.h"
#include "ap_msg.h"
#include "urcp_ap.h"
#include <user_oid.h>
#include "trap_sta.h"
#endif
#include "urcp_debug.h"



#ifdef URCP_AP
#ifdef URCP_TRAP_CMD_AP_STAT
static mtimer_t ap_stat_timer;
/*HC AP流量统计功能*/
//#define AP_HEART_BEAT_TIME_MS 20u //用于beat_timer的定时时间
#define AP_STAT_PERIOD_SEC  10u//用于ap_stat_timer定时时间
#define AP_STAT_TIME_MS  (AP_HEART_BEAT_TIME_MS - AP_STAT_PERIOD_SEC*1000)
static uint32 rx_byte = 0u;
static uint32 tx_byte = 0u;
#endif
#endif

#ifdef URCP_AC
unsigned int age_time = AP_AGE_TIME_AUTO_MS;
static mtimer_t age_timer;
static mtimer_t discover_timer;
#ifdef URCP_TRAP_CMD_AP_STAT
static mtimer_t ac_auto_save_stat_timer;
static uint16 stat_renew = 0u;
static uint16 rate_renew = 0u;
#define AC_SAVE_STAT_TIME_MS 10000
#define DATE_BASE_STAT 1900
void ac_save_ap_stat(void)
{
    static struct tm lasttm;
    static time_t lasttime;
    struct tm *nowtm = NULL;
    time_t nowtime;
    unsigned long diff=0ul;
    uint32 i = 0u,tb_index,val = 0u;
    urcp_shm_t *mem_shadow = NULL;
    wl_info_stat_t* acStat = NULL;
    urcp_dev_t *tmp = NULL;
    uint32 rxTemp = 0u;
    uint32 txTemp = 0u;
    if(MTIMER_TIMEOUT(&ac_auto_save_stat_timer))
    { /*10s定时到*/
	mem_shadow = get_shm_ptr();
	if(mem_shadow == NULL)
	{
	    return;
	}
	nowtime = time(NULL);
	nowtm = localtime(&nowtime);
	if(stat_renew == 1u)//AC接收到了来自AP的新数据
	{
	    stat_renew = 0u;
	    rate_renew = 0u;			
	    for(i = 0u; i < DEV_HASH_SIZE; i++) 
	    {
		tb_index = mem_shadow->dev_tb_hash[i];
		while(tb_index!=NONE_NODE) 
		{
		    tmp = &(mem_shadow->dev_table[tb_index-1]);
		    val = tmp->updateStat;
		    //根据所连AP的流量与速率更新情况，将rate和收发流量进行更新
		    if(val == 1)
		    {
			acStat = &(tmp->acStat);
			tmp->updateStat = 0;
			tmp->updateRate = 0;
			val = 0;
			if(acStat->rx_byte >= acStat->last_rx_byte)
			{
			    rxTemp = acStat->rx_byte - acStat->last_rx_byte;
			}
			else 
			{
			    rxTemp = 0xFFFFFFFF - acStat->last_rx_byte + acStat->rx_byte;
			}
			if(acStat->tx_byte >= acStat->last_tx_byte)
			{
			    txTemp = acStat->tx_byte - acStat->last_tx_byte;
			}
			else
			{
			    txTemp = 0xFFFFFFFF - acStat->last_tx_byte + acStat->tx_byte;
			}
			if((acStat->last_rx_byte == 0) && (acStat->last_tx_byte == 0))
			{
			    rxTemp = 0u;
			    txTemp = 0u;
			}
			else if(rxTemp >= 1000000000ull || txTemp >= 1000000000ull)
			{
			    rxTemp = 0u;
			    txTemp = 0u;
			}
			acStat->last_rx_byte = acStat->rx_byte;
			acStat->last_tx_byte = acStat->tx_byte;
			diff = (unsigned long)(nowtime - lasttime);
			if( (lasttime != 0) && (diff > (AC_SAVE_STAT_TIME_MS/1000+60)))
			{
			    //记录收发流量
#if 0
			    acStat->wl_his_stat.day_pos += 1;//系统时间被修改了
			    acStat->wl_his_stat.day_pos = (acStat->wl_his_stat.day_pos)%7;
#endif
			    uint32 day_pos = acStat->wl_his_stat.day_pos;
			    memset(&(acStat->wl_his_stat.stat_day[day_pos]),0,sizeof(acStat->wl_his_stat.stat_day[day_pos]));
			    acStat->wl_his_stat.stat_day[day_pos].year = DATE_BASE_STAT + nowtm->tm_year;
			    acStat->wl_his_stat.stat_day[day_pos].mon = 1 + nowtm->tm_mon;
			    acStat->wl_his_stat.stat_day[day_pos].day = nowtm->tm_mday;

			    acStat->wl_stat_current.min_pos = nowtm->tm_min;
			    acStat->wl_stat_current.stat_min[acStat->wl_stat_current.min_pos].rx = 0;
			    acStat->wl_stat_current.stat_min[acStat->wl_stat_current.min_pos].tx = 0;			
			}
			if(acStat->wl_stat_current.min_pos == nowtm->tm_min)//分钟数未变
			{
			    //保存60分钟的数据
			    acStat->wl_stat_current.stat_min[acStat->wl_stat_current.min_pos].rx += rxTemp;
			    acStat->wl_stat_current.stat_min[acStat->wl_stat_current.min_pos].tx += txTemp;
			    //保存7天的数据
			    uint32 day_pos = acStat->wl_his_stat.day_pos;
			    uint32 hour_pos = nowtm->tm_hour;
			    acStat->wl_his_stat.stat_day[day_pos].stat_hour[hour_pos].rx += rxTemp;          
			    acStat->wl_his_stat.stat_day[day_pos].stat_hour[hour_pos].tx += txTemp;
			    //如果没有进行日期的初始化，则初始化日期
			    if(acStat->wl_his_stat.stat_day[day_pos].year == 0)
			    {
				acStat->wl_his_stat.stat_day[day_pos].year = DATE_BASE_STAT + nowtm->tm_year;
				acStat->wl_his_stat.stat_day[day_pos].mon = 1 + nowtm->tm_mon;
				acStat->wl_his_stat.stat_day[day_pos].day = nowtm->tm_mday;
			    }
			}
			else if(acStat->wl_stat_current.min_pos != nowtm->tm_min)//分钟数改变
			{
			    acStat->wl_stat_current.min_pos = nowtm->tm_min;
			    //保存60分钟的数据
			    acStat->wl_stat_current.stat_min[acStat->wl_stat_current.min_pos].rx = rxTemp;
			    acStat->wl_stat_current.stat_min[acStat->wl_stat_current.min_pos].tx = txTemp;
			    //保存7天的数据
			    if(nowtm->tm_min != 0)//小时数未变
			    {
				uint32 day_pos = acStat->wl_his_stat.day_pos;
				uint32 hour_pos = nowtm->tm_hour ;
				acStat->wl_his_stat.stat_day[day_pos].stat_hour[hour_pos].rx += rxTemp;
				acStat->wl_his_stat.stat_day[day_pos].stat_hour[hour_pos].tx += txTemp;
				//日期没有初始化，进行日期初始化
				if(acStat->wl_his_stat.stat_day[day_pos].year == 0)
				{
				    acStat->wl_his_stat.stat_day[day_pos].year = DATE_BASE_STAT + nowtm->tm_year;
				    acStat->wl_his_stat.stat_day[day_pos].mon = 1 + nowtm->tm_mon;
				    acStat->wl_his_stat.stat_day[day_pos].day = nowtm->tm_mday;
				}
			    }
			    else// if(nowtm->tm_min == 0)//小时数改变了
			    {
				//天数为改变
				uint32* day_pos = &(acStat->wl_his_stat.day_pos);
				uint32* hour_pos = &(acStat->wl_his_stat.stat_day[(*day_pos)].hour_pos);
				if((*hour_pos) == 23)//天数改变了
				{
				    (*day_pos) += 1;
				    (*day_pos) = (*day_pos)%7;
				    memset(&(acStat->wl_his_stat.stat_day[(*day_pos)]),0,sizeof(acStat->wl_his_stat.stat_day[(*day_pos)]));
				    hour_pos = &(acStat->wl_his_stat.stat_day[(*day_pos)].hour_pos);
				}
				(*hour_pos) = nowtm->tm_hour;				

				acStat->wl_his_stat.stat_day[(*day_pos)].stat_hour[(*hour_pos)].rx = rxTemp;
				acStat->wl_his_stat.stat_day[(*day_pos)].stat_hour[(*hour_pos)].tx = txTemp;
				acStat->wl_his_stat.stat_day[(*day_pos)].year = DATE_BASE_STAT + nowtm->tm_year;
				acStat->wl_his_stat.stat_day[(*day_pos)].mon = 1 + nowtm->tm_mon;
				acStat->wl_his_stat.stat_day[(*day_pos)].day = nowtm->tm_mday;				
			    }
			}		
		    }//if(val)
		    tb_index = tmp->tb_next;
		}//while
	    }//for
	}//if(renew)
	lasttime = nowtime;
	lasttm = *nowtm;
	MTIMER_START(&ac_auto_save_stat_timer,AC_SAVE_STAT_TIME_MS);
    }
    return;
}
uint16 ac_auto_save_stat(void)
{
	stat_renew = 1u;
	return 0;
}
uint16 ac_auto_save_rate(void)
{
	rate_renew = 1u;
	return 0;
}
#endif
#ifdef AP_CONFIG_GET_ONE_VLAN
vlan_ap_info_t vlan_ap_info[MAX_AP_VLAN];
#endif
#if AP_WL_POWER_AUTO
static mtimer_t ac_collect_timer;
static wl_power_stage_t wl_power_stage = POWER_STAGE_INIT;
#endif
#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
static mtimer_t ac_auto_collect_clts_timer;
#endif
static ac_management_info_t ac_management = {0};

extern urcp_shm_t* shm_ptr;

pthread_mutex_t ap_auth_mutex = PTHREAD_MUTEX_INITIALIZER;

void num_of_auth_ap_set(int num)
{
	pthread_mutex_lock(&ap_auth_mutex);
	ac_management.max_ap_auth = num;
	pthread_mutex_unlock(&ap_auth_mutex);
}

int32 num_of_auth_ap_inc(int max)
{
    int32 ret = -1;

    pthread_mutex_lock(&ap_auth_mutex);
    if(ac_management.max_ap_auth < max)
    {
        ac_management.max_ap_auth++;
	ret = 0;
    }
    pthread_mutex_unlock(&ap_auth_mutex);

	DBCK("----------------------------ap_auth = %d", ac_management.max_ap_auth);
    return ret;
}
void num_of_auth_ap_dec(void)
{
    pthread_mutex_lock(&ap_auth_mutex);
    if(ac_management.max_ap_auth>0) {
	ac_management.max_ap_auth--;
    }
    pthread_mutex_unlock(&ap_auth_mutex);
	DBCK("----------------------------ap_auth = %d", ac_management.max_ap_auth);
}
/*
 * check the serial no whether in the range of authorized.
 *
 * added by Shiyu.Chen, 2012.06.19
 */
int32 permit_authorized(uint32 serial_no)
{
    MibProfileType mibType = MIB_PROF_AP_PERMISSION;
    ApPermissionProfile *prof = NULL;
    int min = 0, max =0, index = 0, ret = -1;
    ApScanProfile *prof_scan = NULL;
    uint32 in_range = 0u;
    
    URCP_DEBUG(("%s\n", __func__));
    
    ProfInstLowHigh(mibType, &max, &min);
    for(index = min+1; index < max; index++)
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0)&& (prof->head.active == TRUE))
	{
	    if((serial_no >= prof->num_start) && (serial_no <= prof->num_end))
	    {
		in_range = 1u;
		break;
	    }
	}

    }

    /* The serial no is not in the range of authorized. */
    if(in_range == 0u)  /* get default value.*/
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, 0);
    }
    prof_scan = (ApScanProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_SCAN, 0);
    if(prof->action == PROF_ENABLE)
    {
	ret = num_of_auth_ap_inc(prof_scan->maxAp);
#ifdef URCP_DEBUG
	if(ret!=0)
	{
	    URCP_DEBUG(("The max management AP count is %d, current is %d\n", prof_scan->maxAp, ac_management.max_ap_auth));	
	    DBCK("Auth failed: max = %d, current = %d\n", prof_scan->maxAp, ac_management.max_ap_auth);
	}
#endif
    }
    URCP_DEBUG(("%s****************max:%d, current: %d\n", __func__,prof_scan->maxAp, ac_management.max_ap_auth));
    DBCK("Auth successed: max = %d, current = %d\n", prof_scan->maxAp, ac_management.max_ap_auth);

    return ret;
}

int32 urcp_ap_clt_discover(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_discover_t* cmd;
    cmd_hdr_t* cd;

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
		DBCK();
		return -1;
    }

    URCP_DEBUG(("%s\n", __func__));
    memset(buffer, 0, sizeof(buffer));
    urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_GET, id, NULL);
	/*指向包头 带tag*/
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*联动发现 */
	cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    cmd -> type = htonl(CMD_DISCOVER);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> send_flag = htonl(DEV_TYPE_AC);
    cmd -> recv_flag = htonl(DEV_TYPE_FIT_AP|DEV_TYPE_FAT_AP);
    cmd -> length = htons(sizeof(cmd_discover_t));
    len += sizeof(cmd_discover_t);
    header -> cmd_no = 1u;

    cd = (cmd_hdr_t*)(cmd + 1);
    len += add_cmd(cd, CMD_WIRELESS_CLT);
    header -> cmd_no += 1u;

	DBCK();
    /*广播发送*/
	urcp_send_by_vlan(buffer,len);

    return 0;
}

void urcp_ac_heart_beat(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    urcp_hdr_t* header;
    cmd_discover_t* cmd;

    URCP_DEBUG(("%s\n", __func__));	
    URCP_FDEBUG("%s\n", __func__);	
    //printf("%s\n", __func__);	
    memset(buffer, 0, sizeof(buffer));
    urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_GET, 0, NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

    cmd -> type = htonl(CMD_DISCOVER);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> send_flag = htonl(DEV_TYPE_AC);
    cmd -> recv_flag = htonl(DEV_TYPE_FIT_AP|DEV_TYPE_FAT_AP);
    cmd -> length = htons(sizeof(cmd_discover_t));
    len += sizeof(cmd_discover_t);
    header -> cmd_no = 1u; 

#if 0
    cd++;
    len += add_cmd(cd, CMD_AP_SZ_NAME);
    header -> cmd_no += 1u; 
#endif

    /*广播发送*/
	urcp_send_by_vlan(buffer,len);
}

/*
 * The AC send packet to lookup the AP
 *
 * KeHuatao
 *
 */
int32 urcp_ap_discover(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_discover_t* cmd;
    cmd_hdr_t* cd;
    ApScanProfile *prof;
    MibProfileType mibType = MIB_PROF_AP_SCAN;

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT2, &id, NULL)<0)
    {
	return -1;
    }

    URCP_DEBUG(("%s\n", __func__));	
    URCP_FDEBUG("%s\n", __func__);	
    //printf("%s\n", __func__);	
    memset(buffer, 0, sizeof(buffer));
	urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_GET, id, NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
    /*联动发现 */
	cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);


    cmd -> type = htonl(CMD_DISCOVER);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> send_flag = htonl(DEV_TYPE_AC);
    cmd -> recv_flag = htonl(DEV_TYPE_FIT_AP|DEV_TYPE_FAT_AP);
    cmd -> length = htons(sizeof(cmd_discover_t));
    len += sizeof(cmd_discover_t);
    header -> cmd_no = 1u; 

    cd = (cmd_hdr_t*)(cmd+1);
    len += add_cmd(cd, CMD_PRODUCT_MODEL);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_SERIAL_NO);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_DEV_NAME);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_SW_VERSION);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_HW_VERSION);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_AP_SSID);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_AP_CHANNEL);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_CURRENT_CLIENTS);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_WIRELESS_MODE);
    header -> cmd_no += 1u; 

    cd++;
    len += add_cmd(cd, CMD_AP_DHCP_EN);
    header -> cmd_no += 1u; 

    prof = (ApScanProfile *)ProfGetInstPointByIndex(mibType, 0);
    if(prof!=NULL)
    {
	if(prof->scanAll==TRUE)
	{
	    cd++;
	    len += add_cmd(cd, CMD_AIR_PROBE);
	    header -> cmd_no += 1u; 
	}
    }

    cd++;
    len += add_cmd(cd, CMD_AP_SZ_NUM);
    header -> cmd_no += 1u; 

#if 0
    cd++;
    len += add_cmd(cd, CMD_AP_SZ_NAME);
    header -> cmd_no += 1u; 
#endif

    /*广播发送*/
#if 1
	urcp_send_by_vlan(buffer,len);
#else
	/* 此处未兼容1.6版本胖AP发现，只能发现直连的胖AP */
	h2_send_frame(buffer, len);
#endif

    return 0;
}

#if (AP_LOAD_BALANCE == FYES)
void lb_set_when_ap_online(urcp_dev_t* tmp)
{
    DBCK();
    int min = 0, max = 0;
    int i,j;
    uchar found = 0u;
    uint32 aliveNum = 0u;
	ApLoadBalanceProfile *profLb=NULL;
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    SystemProfile *profSys = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        aliveNum = 0u;
        profLb = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, i);
        if((profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
        {
            for(j=0;j<profLb->num;j++){
                if (mac_cmp(profLb->ap[j].macAddr.addr, tmp->mac) == 0){
                    profLb->ap[j].isAlive = 1;
                    tmp->load_balance = i+1;
                    profLb->ap[j].client = tmp->wl_info.clients;
                    if(profLb->ap[j].client >= profLb->source_sum ){
                        profLb->ap[j].isFull = 1u;
                    }else{
                        profLb->ap[j].isFull = 0u;
                    }
                    found = 1u;
                }
                if(profLb->ap[j].isAlive){
                    aliveNum++;
                }
            }
        }
        if(found){
            break;
        }
    }
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if((aliveNum>=2) && found && profLb->lbEn && profSys->loadBalanceEn){
            DBCK("Load Balance Group %d has %d AP alive!\n",tmp->load_balance,aliveNum);
            ap_load_balance_process(tmp->load_balance);
    }
}

void lb_set_after_ap_offline(urcp_dev_t* tmp)
{
    ApLoadBalanceProfile *profLb=NULL;
    SystemProfile *profSys = NULL;
    uint32 aliveNum = 0u;
    int32 j;

    if( tmp->load_balance != 0 ){
        profLb=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_LOAD_BALANCE,tmp->load_balance-1);  /*get inst pointer*/
        profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
        if((profSys!=NULL) && (profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
        {
            for(j=0;j<profLb->num;j++){
                if (mac_cmp(profLb->ap[j].macAddr.addr, tmp->mac) == 0){
                    profLb->ap[j].isAlive = 0u;
                    profLb->ap[j].client = 0u;
                    profLb->ap[j].isFull = 0u;
                }
                if(profLb->ap[j].isAlive){
                    aliveNum++;
                }
            }
            if(aliveNum < 2){
                DBCK("Load Balance Group %d has only %d AP alive!\n",tmp->load_balance,aliveNum);
                profLb->source_sum = MAX_SOURCE_SUM;
                urcp_set_ap_lb_source_sum(profLb,profLb->source_sum);
            }
        }
    }
}
#endif


#if 1
void ac_timer(void)
{
#if 1
    int32 tb_index;
    uint32 i;
    urcp_dev_t *tmp=NULL, *prev = NULL;
#endif
    ApScanProfile *prof=NULL;

    prof = (ApScanProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_SCAN, 0);

#if 1
    if(MTIMER_TIMEOUT(&age_timer))
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    prev = NULL;
	    tb_index = shm_ptr->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		tmp = &shm_ptr->dev_table[tb_index-1];
		if(tmp->aged==1)
		{
		    db_loud(URCP_MODULE_ID_URCP, "%s: node: %d, MAC: %s aged\n", __func__, tb_index, str_macaddr(tmp->mac));
		    URCP_FDEBUG("node: %d, MAC: %s aged\n", tb_index, str_macaddr(tmp->mac));
		    DBCK("MAC: %s aged\n", str_macaddr(tmp->mac));

#if (AP_LOG_MANAGE == FYES)
			char log_buf[MAX_LOG_LENGTH - 32];
			sprintf(log_buf, "ap mac: %s; event: offline", str_macaddr(tmp->mac));
			ap_syslog(APLINKLOG, log_buf);
#endif

#ifdef AP_CONFIG_GET_ONE_VLAN
			//获取对应的vlan id，以vlan id为数组下标
			vid_t vid = tmp->vid;
			vlan_ap_info[vid].vid = vid;
			vlan_ap_info[vid].ap_info.product_sn = tmp->serial_no;
			strncpy(vlan_ap_info[vid].ap_info.sw_version, tmp->sw_version, SW_VERSION_SIZE);
			strncpy(vlan_ap_info[vid].ap_info.hw_version, tmp->hw_version, HW_VERSION_SIZE);
			strncpy(vlan_ap_info[vid].ap_info.product_type, tmp->dev_model, DEV_TYPE_SIZE);
			printf("vid = %u, %d, %s, %s, %s\n", vid, tmp->serial_no, tmp->sw_version, tmp->hw_version, tmp->dev_model);

#endif

#if (AP_LOAD_BALANCE == FYES)
            if( tmp->load_balance != 0 ){
                lb_set_after_ap_offline(tmp);
            }
#endif

		    tmp->used = 1;
		    tmp->aged = 2;
			tmp->wl_info.clients = 0;
#if 0
		    if(prev!=NULL) {
			prev->tb_next = tmp->tb_next;
		    }
		    else {
			shm_ptr->dev_tb_hash[i] = tmp->tb_next;
		    }
//			num_of_auth_ap_dec();
#endif
		}
		else if (tmp->aged == 0)
		{
		    tmp->aged = 1;
		    prev = tmp;
		} else {
			/* aged = 2*/
		}

		tb_index = tmp->tb_next;
	    }
	}
        MTIMER_START(&age_timer, age_time);
    }
#endif
#if 1
    if(MTIMER_TIMEOUT(&discover_timer))
    {
	//if((prof!=NULL) && (prof->autoScan==SCAN_AUTO))/*auto scan*/
	{
#if 0
	    dev_table_init();
#endif
		DBCK("---------------------------------------------------discover\n");
	    urcp_ap_discover();
//	    urcp_ac_heart_beat();
	}
        MTIMER_START(&discover_timer, AP_DISCOVER_TIME_MS);
    }
#endif
}
#else

void ac_timer(void)
{
	int32 tb_index;
	uint32 i;
	urcp_dev_t* tmp = NULL;

	if(MTIMER_TIMEOUT(&discover_timer))
	{
		DBCK("auth_num = %d", ac_management.max_ap_auth);
		pthread_mutex_lock(&ap_auth_mutex);
		ac_management.max_ap_auth = 0;
		dev_table_init();
		pthread_mutex_unlock(&ap_auth_mutex);

		urcp_ap_discover();
        MTIMER_START(&discover_timer, AP_DISCOVER_TIME_MS);
	}
}

#endif

void ac_timer_init(void)
{
    ApScanProfile *prof=NULL;
	
    prof = (ApScanProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_SCAN, 0);
    if(prof!=NULL)
    {
	//if(prof->autoScan==SCAN_AUTO)/*auto scan*/
	{
	    age_time = AP_AGE_TIME_AUTO_MS;
	    MTIMER_START(&discover_timer, 0);
	}
#if 0
        else
	{
	    age_time = AP_AGE_TIME_MANU_MS;
	}
#endif
	MTIMER_START(&age_timer, age_time);
    }
#ifdef URCP_TRAP_CMD_AP_STAT	
	MTIMER_START(&ac_auto_save_stat_timer, AC_SAVE_STAT_TIME_MS);//AC自动保存AP流量10s计时器
#endif
#if AP_WL_POWER_AUTO
    MTIMER_START(&ac_collect_timer, AC_POWER_AUTO_TIME_MS);
#endif
#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
	MTIMER_START(&ac_auto_collect_clts_timer,AC_AUTO_COLLECT_CLTS_TIME_MS);
#endif
}

#if AP_WL_POWER_AUTO
void ac_collect_around_ap_signal(void)
{
    int32 i=0, num=0;
    uchar buffer[URCP_BUF_SIZE];
    uint16 id;
    uint32 len,tb_index;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd; 
    uchar *pMac = NULL;
    uint32 *apNum = NULL;
    urcp_dev_t *tmp=NULL;

    DBCK();

//    if(MTIMER_TIMEOUT(&ac_collect_timer))
    {
        URCP_DEBUG(("%s\n", __func__));
        memset(buffer, 0, sizeof(buffer));

        urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_GET, id, NULL);
		/*指向包头 带tag*/
		header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

        header ->flag = htons(FLAG_NO_PASSWD);/*不需要密码*/

        len += add_cmd(cmd, CMD_AP_RA0_SSID);
        apNum = (uint32*) (&buffer[len]);
        *apNum = 0u;
        pMac = (uchar*)(&buffer[len+sizeof(uint32)]);

        for(i = 0u; i < DEV_HASH_SIZE; i++)
        {
            tb_index = shm_ptr->dev_tb_hash[i];
            while(tb_index!=NONE_NODE)
            {
                tmp = &shm_ptr->dev_table[tb_index-1];
                if((tmp->used==1) && (tmp->aged != 2))
                {
                    DBCK("ra0_mac: %s\n",str_macaddr(tmp->ra0_mac));
                    memcpy(pMac,tmp->ra0_mac,MAC_LEN);
                    pMac += MAC_LEN;
                    memcpy(pMac,tmp->mac,MAC_LEN);
                    pMac += MAC_LEN;
                    (*apNum)++;

                    num++;
                    if( (num%100) == 1){
                        /*分配会话id*/
                        if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0) {
                            break;
                        }
                        header -> request_id =  htons(id);
                    }

                    if( (num%100) == 0){
                        *apNum = htonl(*apNum);
                        cmd->length = ntohs(cmd->length);
                        cmd->length += ((long)pMac - (long)apNum);
                        len += (cmd->length-sizeof(cmd_hdr_t));
                        cmd->length = htons(cmd->length);
                        h2_send_frame(buffer, len);

                        *apNum = 0u;
                        pMac = (uchar*)((uchar*)apNum+sizeof(uint32));
                        len = (long)apNum - (long)buffer;
                    }
                }
                tb_index = tmp->tb_next;
            }
        }

        if( (num%100) != 0 ){
            *apNum = htonl(*apNum);
            cmd->length = ntohs(cmd->length);
            cmd->length += ((long)pMac-(long)apNum);
            len += (cmd->length-sizeof(cmd_hdr_t));
            cmd->length = htons(cmd->length);
			urcp_send_by_vlan(buffer,len);
        }

//        MTIMER_START(&ac_collect_timer, AC_POWER_AUTO_TIME_MS);
    }
}

void quickSort(int *array, int left, int right)
{
    int i,j,tmp;
    if(left < right){
        i = left;
        j = right;
        tmp = array[left];
        do{
            while(array[j]>=tmp && j>i){
                j--;
            }
            if(i<j){
                array[i] = array[j];
            }
            while(array[i]<=tmp && i<j){
                i++;
            }
            if(i<j){
                array[j] = array[i];
            }
        }while(i!=j);
        array[i] = tmp;
        quickSort(array,left,i-1);
        quickSort(array,i+1,right);
    }
}
#define SM_RA0_RSSI (shm_ptr->ra0_rssi)
WlanPowerManual get_wl_power_by_rssi(WlanPowerManual prePower,int32 *array, int32 n)
{
    WlanPowerManual power = prePower;
    int32 rssi;

    if(n>0){
        rssi = array[0];
        if(rssi > -50){
            if( power > LOW){
                power--;
            }
        }else if(rssi < -70){
            if( power < HIGH){
                power++;
            }
        }else{
            ;
        }
    }

    return power;

}

void ap_wl_adjust(void)
{
    int32 i,j,num,tb_index,rssi_array[DEV_TABLE_SIZE];
    urcp_dev_t *tmp=NULL;
    WlanPowerManual power;

    memset(rssi_array,0,sizeof(rssi_array));

    for(i = 0u; i < DEV_HASH_SIZE; i++)
    {
        tb_index = shm_ptr->dev_tb_hash[i];
        while(tb_index!=NONE_NODE)
        {
            tmp = &shm_ptr->dev_table[tb_index-1];
            num = 0;
            if((tmp->used==1) && (tmp->aged != 2) && tmp->wl_info.power && (tmp->dev_type & DEV_TYPE_FIT_AP) )
            {
                power = tmp->wl_info.power;

                for(j=0;j<DEV_TABLE_SIZE;j++){
                    if(SM_RA0_RSSI[tb_index-1][j]){
                        rssi_array[num] = SM_RA0_RSSI[tb_index-1][j];
                        num++;
                    }
                }

                if( num >0 ){
                    quickSort(rssi_array,0,num-1);
                    power = get_wl_power_by_rssi(tmp->wl_info.power,rssi_array,num);
                }
                DBCK("tmp->wl_info.power: %d , power: %d\n",tmp->wl_info.power,power);

                if(tmp->wl_info.power != power ){
                    tmp->wl_info.power = power;
                    urcp_set_ap_wl_power(tmp->mac,tmp->wl_info.power);
                }
            }
            tb_index = tmp->tb_next;
        }
    }
}

/********************************************************************
* 函数：wl_power_auto
* 功能： 功率自动调节主函数
* 创建： 2012-06-20
* 参数： 无
* 输出： 无
* 返回： 无
* 作者：
*******************************************************************/
void wl_power_auto(void)
{
	uint32 clients = 0u,autoPower = 0u;
    uint32 i;
    int32 tb_index;
    uint32 ac_collect_time = AC_POWER_AUTO_TIME_MS;
    urcp_dev_t *tmp=NULL;

    if(MTIMER_TIMEOUT(&ac_collect_timer)){ /*定时到*/

        DBCK("wl_power_stage: %d\n",wl_power_stage);
        switch(wl_power_stage){
            case POWER_STAGE_INIT:  /*初始状态*/
                ac_collect_time = AC_POWER_AUTO_TIME_MS;
                wl_power_stage = POWER_STAGE_SURVEY;
                break;
            case POWER_STAGE_SURVEY: /*检测周期AP*/
                for(i = 0u; i < DEV_HASH_SIZE; i++)
                {
                    tb_index = shm_ptr->dev_tb_hash[i];
                    while(tb_index!=NONE_NODE) {
                        tmp = &shm_ptr->dev_table[tb_index-1];
                        if((tmp->used==1) && (tmp->aged != 2)) { /*检测在线AP*/
                            clients |= tmp->wl_info.clients;
                            autoPower |= tmp->wl_info.power;
                        }
                        tb_index = tmp->tb_next;
                    }
                }
                DBCK("clients: %d, autoPower: %d\n" ,clients,autoPower);

                if( !clients && autoPower ){ /*AP为自动功率调节 并且 周围所有AP没有客户端连上来才进行调节*/
                    ac_collect_time = (urcp_set_ap_site_survey() + 15u)*1000u;
                    wl_power_stage = POWER_STAGE_COLLECT;
                }else{
                    ac_collect_time = AC_POWER_AUTO_TIME_MS;
                }

                break;
            case POWER_STAGE_COLLECT: /*收集周围AP信号*/
                memset(shm_ptr->ra0_rssi,0,sizeof(shm_ptr->ra0_rssi));
                ac_collect_around_ap_signal(); /*给AP发命令收集*/
                ac_collect_time = 30000u;
                wl_power_stage = POWER_STAGE_ADJUST;
                break;
            case POWER_STAGE_ADJUST: /*调节周围AP*/
                ap_wl_adjust(); /*调节AP*/
                ac_collect_time = AC_POWER_AUTO_TIME_MS;
                wl_power_stage = POWER_STAGE_INIT;
                break;
            default:
                ac_collect_time = AC_POWER_AUTO_TIME_MS;
                wl_power_stage = POWER_STAGE_INIT;
                break;
        }

        DBCK("ac_collect_time: %u\n",ac_collect_time);
        MTIMER_START(&ac_collect_timer, ac_collect_time); /*定时*/
    }
}
#endif

#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
const char *wlModeToStr(unsigned int mode)
{
    const char *tmp="";

    if(mode & AP_MODE_11BGN)
    {
	tmp = "11b/g/n";
    }
    else if(mode & AP_MODE_11B)
    {
	tmp = "11b";
    }
    else if(mode & AP_MODE_11G)
    {
	tmp = "11g";
    }
    else if(mode & AP_MODE_11N)
    {
	tmp = "11n";
    }
    else if(mode & AP_MODE_11A)
    {
	tmp = "11a";
    }
    else if(mode & AP_MODE_11BG)
    {
	tmp = "11b/g";
    }
    else if(mode & AP_MODE_11AN)
    {
	tmp = "11a/n";
    }
    else if(mode & AP_MODE_11GN)
    {
	tmp = "11g/n";
    }

    return tmp;
}

#define DATE_BASE 1900
void ac_auto_collect_clts_to_file(void)
{
    static struct tm lasttm;
    static time_t lasttime;
    struct tm *nowtm = NULL;
    time_t nowtime;
    unsigned long diff=0ul;
    unsigned int sta_num = 0;
	uint32 i = 0u,tb_index,val;
	urcp_shm_t *mem_shadow = NULL;
    wl_statis_t * wl_statis = NULL;
	urcp_dev_t *tmp = NULL;


	mem_shadow = get_shm_ptr();
    wl_statis = &mem_shadow->wl_statis;

    if(MTIMER_TIMEOUT(&ac_auto_collect_clts_timer)){ /*定时到*/
        if(mem_shadow!=NULL) {
			for(i = 0u; i < DEV_HASH_SIZE; i++) {
			    tb_index = mem_shadow->dev_tb_hash[i];
				while(tb_index!=NONE_NODE) {
					tmp = &(mem_shadow->dev_table[tb_index-1]);
					val = tmp->dev_type;
					if(((val&DEV_TYPE_FIT_AP) || (val&DEV_TYPE_FAT_AP) || (val&DEV_TYPE_WIRELESS))&&(tmp->aged != 2)) {
                        sta_num += tmp->wl_info.clients;
					}
					tb_index = tmp->tb_next;
				}
			}
		}

    wl_statis->total_of_sta = sta_num;
    nowtime = time(NULL);
    nowtm = localtime(&nowtime);

    diff = (unsigned long)(nowtime - lasttime);
    if( (lasttime != 0) && (diff > (AC_AUTO_COLLECT_CLTS_TIME_MS/1000 +60)) ){
        wl_statis->wl_histroy[wl_statis->current_index].year = DATE_BASE + nowtm->tm_year;
        wl_statis->wl_histroy[wl_statis->current_index].mon = 1 + nowtm->tm_mon;
        wl_statis->wl_histroy[wl_statis->current_index].day = nowtm->tm_mday;

        if(nowtm->tm_hour != lasttm.tm_hour) /*新的小时*/
        {
            wl_statis->wl_histroy[wl_statis->current_index].wl_data[nowtm->tm_hour].client_num = sta_num;
            if(nowtm->tm_hour > lasttm.tm_hour ){
                memmove(&wl_statis->wl_histroy[wl_statis->current_index].wl_data[nowtm->tm_hour-lasttm.tm_hour],
                        &wl_statis->wl_histroy[wl_statis->current_index].wl_data[0],
                        sizeof(wl_statis->wl_histroy[wl_statis->current_index].wl_data[0])*lasttm.tm_hour);

                memset(&wl_statis->wl_histroy[wl_statis->current_index].wl_data[0], 0,
                        sizeof(wl_statis->wl_histroy[wl_statis->current_index].wl_data[0])*(nowtm->tm_hour-lasttm.tm_hour));

            }else{
                memmove(&wl_statis->wl_histroy[wl_statis->current_index].wl_data[0],
                        &wl_statis->wl_histroy[wl_statis->current_index].wl_data[lasttm.tm_hour-nowtm->tm_hour],
                        sizeof(wl_statis->wl_histroy[wl_statis->current_index].wl_data[0])*lasttm.tm_hour);

                memset(&wl_statis->wl_histroy[wl_statis->current_index].wl_data[nowtm->tm_hour+1], 0,
                        sizeof(wl_statis->wl_histroy[wl_statis->current_index].wl_data[0])*(lasttm.tm_hour-nowtm->tm_hour));
            }
        }
    }else{
        if( nowtm->tm_mday != lasttm.tm_mday ) /*day 变化，mon，year也可能一起变化*/
        {
            wl_statis->current_index = (wl_statis->current_index+1) % MAX_WL_STATIS_DAY;
            memset(&wl_statis->wl_histroy[wl_statis->current_index],0,sizeof(wl_statis->wl_histroy[0]));

            wl_statis->wl_histroy[wl_statis->current_index].year = DATE_BASE + nowtm->tm_year;
            wl_statis->wl_histroy[wl_statis->current_index].mon = 1 + nowtm->tm_mon;
            wl_statis->wl_histroy[wl_statis->current_index].day = nowtm->tm_mday;
        }
        if( (nowtm->tm_hour < MAX_WL_STATIS_DATA)
                && (sta_num > wl_statis->wl_histroy[wl_statis->current_index].wl_data[nowtm->tm_hour].client_num) )
        {
            wl_statis->wl_histroy[wl_statis->current_index].wl_data[nowtm->tm_hour].client_num = sta_num;
        }
    }
    lasttime = nowtime;
    lasttm = *nowtm;

	URCP_DEBUG(("%s,%d\n", __func__,__LINE__));

	MTIMER_START(&ac_auto_collect_clts_timer, AC_AUTO_COLLECT_CLTS_TIME_MS);/*定时*/
	}
}
#endif

#endif




#ifdef URCP_AP

#define MAX_OVERTIME_NUMBER	    2u
#define AP_STATE_AUTH_TIME_OUT	    2u
#define AP_STATE_UNAUTH_TIME_OUT    2u
#define MAX_AC_INFOR_TIMES    90u
#define AC_TIME_OUT		    4u
#define AP_STATE_CONFUPDATE_REQ_TIME_OUT    2u
#if AP_WL_POWER_AUTO
static uchar ap_collect_allow = 0u;
static mtimer_t ap_collect_timer;
#endif


ap_info_t ap_infor;
static pthread_mutex_t apMutex = PTHREAD_MUTEX_INITIALIZER;
static mtimer_t state_timer, conf_timer;
static mtimer_t ap_auth_timer;
static mtimer_t beat_timer;
static uint32 timeout = 0u, unauth_times = 0u, auth_times = 0u, fat_ap = 1u, ap_update_flag = 0u;
uint32 ac_infor_times = 0u;
static uint32 update_times = 0u;
static bakup_state_t backup=NEED_NOT_BACKUP;
extern void create_file(uint32 rport, char** p);

void send_ap_upper_limit_msg(unsigned int max_num)
{
	DBCK();
	uchar buffer[URCP_BUF_SIZE], *data;
	uint32 len;
	urcp_hdr_t *header;
	cmd_hdr_gen_t *cmd;
	int iftag = 0;

	memset(buffer, 0, sizeof(buffer));
	iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_gen_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_hdr_gen_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	cmd->type = htonl(TRAP_UPPER_LIMIT);
	cmd->cmd_status = htons(URCP_OK);
	cmd->value = htonl(max_num);
	len += sizeof(cmd_hdr_gen_t);
	cmd->length = htons(len);

	urcp_send(0u, &(buffer[0]), len);

	return;
}

void urcp_msg_send_prof_to_ac(struct msgbuf_t *msgbuf)
{
	DBCK();
	int i;
	uint32 sn = 0;
	unsigned int count = 0u;
	request_cmd_msg_t request_cmd_msg;

	for (i = 0; i < MAC_LEN; i++) {
		DBCK("%02x ", ap_infor.ac_infor.mac[i]);
		if (ap_infor.ac_infor.mac[i] == 0) {
			count++;
		}
	}

	DBCK();

	memcpy(&request_cmd_msg, msgbuf->sw_msg.buffer, sizeof(struct _request_cmd_msg_t));
	sn = request_cmd_msg.sn;

	if (MAC_LEN == count) {
		DBCK();
		msg_send_to_goahead(REQUEST_SEND_FILE, URCP_STATUS_FOUND_NONE);
	} else if (backup == NEED_NOT_BACKUP){
		DBCK();
		msg_send_to_goahead(REQUEST_SEND_FILE, URCP_STATUS_OK);
	} else {
		DBCK();
		upload_current_config_file_to_ac(ap_infor.ac_infor.mac, sn);
	}

	return;
}

#if 0
void urcp_msg_change_config(struct msgbuf_t *msgbuf)
{
	int i;
	uchar buf[URCP_BUF_SIZE];
	uint16 id, name_len, xml_len;
	uint32 ulen = 0u, content_len = 0u;
	uint32 sn = 0;
	urcp_hdr_t *header;
	cmd_hdr_t *cmd;
	uint32 type;
	request_cmd_msg_t request_cmd_msg;
	unsigned int count = 0u;

	memcpy(&request_cmd_msg, msgbuf->sw_msg.buffer, sizeof(struct _request_cmd_msg_t));
	type = request_cmd_msg.opt_type;
	if (type == REQUEST_FAT_TO_FIT) {
		DBCK();
		urcp_ac_discover();
		usleep(2000 * 1000);
	}

	for (i = 0; i < MAC_LEN; i++) {
		printf("%02x ", ap_infor.ac_infor.mac[i]);
		if (ap_infor.ac_infor.mac[i] == 0) {
			count++;
		}
	}
	DBCK();

	if (MAC_LEN == count) {
		msg_send_to_goahead(REQUEST_CHANGE_CONF, URCP_STATUS_FAIL);
	} else {
		memset(buf, '\0', sizeof(buf));
		memcpy(&request_cmd_msg, msgbuf->sw_msg.buffer, sizeof(struct _request_cmd_msg_t));

		if (session_alloc(SESSION_TYPE_SET, 0u, URCP_CMD_TIME_OUT, &id, (cb_fp)urcp_ap_request_cb) == 0) {
			urcp_buid_header(buf, ap_infor.ac_infor.mac, (uint16)URCP_SET, id, NULL);

			header = (urcp_hdr_t *)(&buf[ETH_URCP_OFFSET]);
			ulen += ETH_URCP_OFFSET + sizeof(urcp_hdr_t);
			header->cmd_no = 1u;

			cmd = (cmd_hdr_t *)(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);

			type = request_cmd_msg.opt_type;
			ulen += add_cmd(cmd, type);
			cmd->length = ntohs(cmd->length);

			sn = request_cmd_msg.sn;
			name_len = request_cmd_msg.name_len;
			xml_len = request_cmd_msg.xml_len;
			content_len = name_len + xml_len;
			sn = htonl(sn);
			name_len = htons(name_len);
			xml_len = htons(xml_len);

			memcpy(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t)], &sn, sizeof(sn));
			cmd->length += sizeof(sn);
			ulen += sizeof(sn);

			memcpy(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t) + sizeof(sn)],
					&name_len, sizeof(name_len));
			cmd->length += sizeof(name_len);
			ulen += sizeof(name_len);

			memcpy(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t) + sizeof(sn) + sizeof(name_len)],
					&xml_len, sizeof(xml_len));
			cmd->length += sizeof(xml_len);
			ulen += sizeof(xml_len);

			memcpy(&buf[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(cmd_hdr_t) + sizeof(sn) + 2 * sizeof(name_len)],
					&(request_cmd_msg.content), content_len);
			cmd->length += content_len;
			cmd->length = htons(cmd->length);
			ulen += content_len;
		}

		DBCK("content_len = %d, content = %s", content_len, request_cmd_msg.content);

	    urcp_send(0u, &(buf[0]), ulen);
	}

	return;
}
#endif

int32 urcp_ac_discover(void)
{
	DBCK();
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_discover_t* cmd;
	int iftag = 0;

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
	return -1;
    }
    memset(buffer, 0, sizeof(buffer));
    iftag = urcp_buid_header(buffer, mac_addr_cooperate, (uint16)URCP_GET, id, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    cmd -> type = htonl(CMD_DISCOVER);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> send_flag = htonl(DEV_TYPE_FIT_AP);
    cmd -> recv_flag = htonl(DEV_TYPE_AC);
    cmd -> length = htons(sizeof(cmd_discover_t));
    len += sizeof(cmd_discover_t);

    header -> cmd_no = 1u; 
    
    URCP_DEBUG(("%s\n", __func__));
    /*广播发送*/
    urcp_send(0u, &(buffer[0]), len);
	DBCK();

    return 0;
}

int32 urcp_send_ap_auth_prepare(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_ap_auth_req_t* cmd;
	int iftag = 0;

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
	return -1;
    }

    iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_GET, id, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_ap_auth_req_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_ap_auth_req_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    cmd -> type = htonl(CMD_AP_AUTH_PRE);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> serial_no = htonl(getSystemSn());
    cmd -> length = htons(sizeof(cmd_ap_auth_req_t));
    len += sizeof(cmd_ap_auth_req_t);

    header -> cmd_no += 1u;
    URCP_DEBUG(("%s\n", __func__));
    /*广播发送*/
    urcp_send(0u, &(buffer[0]), len);

    return 0;
}

int32 urcp_send_ap_auth_request(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_ap_auth_req_t* cmd;
	int iftag = 0;
    
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
	return -1;
    }

    iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_GET, id, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_ap_auth_req_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_ap_auth_req_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    cmd -> type = htonl(CMD_AP_AUTH_REQ);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> serial_no = htonl(getSystemSn());
    cmd -> length = htons(sizeof(cmd_ap_auth_req_t));
    len += sizeof(cmd_ap_auth_req_t);

    header -> cmd_no += 1u; 
    
    URCP_DEBUG(("%s\n", __func__));
    /*广播发送*/
    urcp_send(0u, &(buffer[0]), len);

    return 0;
}

void change_state(ap_state_t newstate)
{
    pthread_mutex_lock(&apMutex);
    ap_infor.state = newstate;
	if (newstate == AP_STATE_NORMAL) {
		ap_update_flag = 1u;
	}
    pthread_mutex_unlock(&apMutex);
DBCK("newState=%d\n",newstate);
    return;
}


ap_state_t read_state(void)
{
    ap_state_t state = AP_STATE_INIT;

    pthread_mutex_lock(&apMutex);
    state = ap_infor.state;
    pthread_mutex_unlock(&apMutex);

    return state;
}

void set_ap_authorize(uint32 ap_auth)
{
    pthread_mutex_lock(&apMutex);
    ap_infor.ap_auth = ap_auth;
    pthread_mutex_unlock(&apMutex);
    
    return;
}

int urcp_send_update_fireware_reqest(void)
{
    int ret = 0, swVer = 0, hwVer = 0;
    urcp_ap_sysinfo_t ap_sysinfo;
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd;
    char buf[64];
	int iftag = 0;

    memset(&ap_sysinfo, 0, sizeof(urcp_ap_sysinfo_t));

    /*get AP system information.*/
    ap_sysinfo.product_sn = htonl(getSystemSn());
    /*product product model*/
    get_sysinfo_machine_model(buf);
    strcpy(ap_sysinfo.product_type, buf);

	get_verinfo_from_local(&hwVer, &swVer);
	DBCK("hwVer = %d, swVer = %d\n", hwVer, swVer);
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "V%d.%d", hwVer >> HWVER_2, hwVer & 0x0F);
    strcpy(ap_sysinfo.hw_version, buf);

	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "V%d.%d.%d", swVer >> (SWVER_3 + SWVER_2), (swVer >> (SWVER_2)) & 0x0F, swVer & 0x0F);
    strcpy(ap_sysinfo.sw_version, buf);

    DBCK("product_type = %s, hw_version = %s, sw_version = %s\n", ap_sysinfo.product_type, ap_sysinfo.hw_version, ap_sysinfo.sw_version);

    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
    {
	return -1;
    }
    iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_GET, id, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    cmd -> type = htonl(CMD_AP_UPDATE_FIRMWARE);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t);
    len += sizeof(cmd_hdr_t);

    memcpy(&buffer[len], &ap_sysinfo, sizeof(urcp_ap_sysinfo_t));
    cmd -> length += sizeof(urcp_ap_sysinfo_t);
    cmd -> length = htons(cmd -> length);
    len += sizeof(urcp_ap_sysinfo_t);

    header -> cmd_no = 1u; 
    
    URCP_DEBUG(("%s\n", __func__));
    /*广播发送*/
    urcp_send(0u, &(buffer[0]), len);

    return ret;
}


int urcp_send_config_update_reqest(void)
{
    int ret = 0;
    urcp_ap_sysinfo_t ap_sysinfo;
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd;
    char buf[64];
    ApEffectConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;
	int iftag = 0;

    prof = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibType, 0);
    if((prof != NULL) && (prof->config != PROF_ENABLE))/*local*/
    {
		DBCK();
	memset(&ap_sysinfo, 0, sizeof(urcp_ap_sysinfo_t));

	/*get AP system information.*/
	ap_sysinfo.product_sn = htonl(getSystemSn());
	/*product product model*/
	get_sysinfo_machine_model(buf);
	strcpy(ap_sysinfo.product_type, buf);
	get_sysinfo_hw_model(buf);
	strcpy(ap_sysinfo.hw_version, buf);


	/*分配会话id*/
	if(session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
	{
	    return -1;
	}
	iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_GET, id, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	cmd -> type = htonl(CMD_AP_UPDATE_PROFILE);
	cmd -> cmd_status = htons(URCP_OK);
	cmd -> length = sizeof(cmd_hdr_t);
	len += sizeof(cmd_hdr_t);

	memcpy(&buffer[len], &ap_sysinfo, sizeof(urcp_ap_sysinfo_t));
	cmd -> length += sizeof(urcp_ap_sysinfo_t);
	cmd -> length = htons(cmd -> length);
	len += sizeof(urcp_ap_sysinfo_t);

	header -> cmd_no = 1u; 

	URCP_DEBUG(("%s\n", __func__));
	/*广播发送*/
	urcp_send(0u, &(buffer[0]), len);
    }
    else
    {
	DBCK("urcp_send_config_update_reqest");
	change_state(AP_STATE_NORMAL);
	//get_ip_from_dhcp_server(ap_infor.ac_infor.ip);
    }

    return ret;    
}

void init_ap_state(void)
{
    memset(&ap_infor, 0, sizeof(ap_info_t));

    pthread_mutex_lock(&apMutex);
    ap_infor.state = AP_STATE_INIT;
    ap_infor.ac_timeout = AC_TIME_OUT;
    ap_infor.ap_auth = CANCEL_AUTH;
    pthread_mutex_unlock(&apMutex);

    return;
}
int32 isValidMac(uchar *mac)
{ 
    uchar nullMac [6]={'\0','\0','\0','\0','\0','\0'};
    int32 ret=1;
   
    if(mac_cmp(nullMac, mac) == 0) {   
        ret = 0;         
    }
    else if((mac[0] & 0x1u) == 1u){
        ret = 0;         
    }

    return ret; 
}
void ac_ip_mac_set(uchar *ac_mac, uint32 ac_ip)
{
	DBCK();
    URCP_DEBUG(("%s\n", __func__));
    pthread_mutex_lock(&apMutex);
    memcpy(ap_infor.ac_infor.mac, ac_mac, 6u);
    ap_infor.ac_infor.ip = ac_ip;
    pthread_mutex_unlock(&apMutex);

    return;
}

char* int2ip(unsigned int lip,char * ip)
{
    unsigned char i=0;        
    char *tmp=ip;             

    while(i++<3)              
    {  
	sprintf(tmp,"%u.",(lip&0xff000000)>>24);
	lip=lip<<8;           
	tmp+=strlen(tmp);     
    }  
    sprintf(tmp,"%u",(lip&0xff000000)>>24);
    return ip;
}

static unsigned int ap_dhcp_en()
{
	unsigned int dhcp_en = 0;

    ApBasicConfProfile *prof = NULL;

    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF,0);
	if (NULL != prof) {
		dhcp_en = prof->dhcp_en;
	}

    DBCK("dhcp_en=%d\n",dhcp_en);
	return dhcp_en;
}

void get_ip_from_dhcp_server(uint32 ac_ip)
{
    char str[100];
    char ip[16]={0};

	if ((1 == ap_dhcp_en()) && (ac_ip != 0)) {
    DBCK();
		system("killall udhcpc 1>/dev/null 2>&1");
		memset(str, '\0', sizeof(str));
		sprintf(str, "udhcpc -i br0 -r %s -s /sbin/udhcpc.sh -p /var/run/udhcpd.pid &", int2ip(ntohl(ac_ip),ip));
		system(str);
	}

    return;
}

void setInterfaceDhcp( int dhcp_en)
{
	InterfaceProfile *profIF = NULL;
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
	if ((profIF != NULL) || (profIF->head.active != 0)) {
		if (dhcp_en == 1) {
			profIF->ether.connMode = ETH_CONN_DHCP;
			DBCK();
		} else {
			profIF->ether.connMode = ETH_CONN_STATIC;
		}
	}
	return;
}
#if AP_WL_POWER_AUTO
void set_ap_collect_timer_allow(uint32 timer_sec)
{
    DBCK();
    MTIMER_START(&ap_collect_timer, timer_sec*1000);
    ap_collect_allow = 1u;
}
void ap_collect_around_ap_signal()
{
    uint32 overTime = 0u;
    char line[128], macaddr[24];
    int32 i=0, n;
    int32 rssi = 0;
    uint32 ch=0;
    FILE *fp;
    uchar if_hw[MAC_LEN];
    around_ssid_info_t *ssid = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);

    if(ap_collect_allow){
    if(MTIMER_TIMEOUT(&ap_collect_timer))
    {
        URCP_DEBUG(("%s\n", __func__));
        ap_collect_allow = 0u;
        system("iwpriv ra0 set SiteSurvey=1");
        sleep(2);
        fp = popen("iwpriv ra0 get_site_survey", "r");
        if(fp!=NULL)
        {
            /*跳过开头2行*/
            fgets(line, sizeof(line), fp);
            fgets(line, sizeof(line), fp);
            while(i<=SSID_HASH_SIZE)
            {
                if(fgets(line, sizeof(line), fp)==NULL)
                {
                    break;
                }
                memset(macaddr, 0, sizeof(macaddr));
                rssi = 0;
                n=sscanf(line, "%u %*s %s %*s %*s %d",&ch, macaddr, &rssi);
                DBCK("ch: %u, rssi: %d, macaddr: %s\n",ch,rssi,str_macaddr(macaddr));
                DBCK("prof->mBaseCf.Channel: %u\n",prof->mBaseCf.Channel);
                if(n==3)
                {
                    if( (prof->mBaseCf.Channel-ch<5) || (ch-prof->mBaseCf.Channel<5)){
                        str2mac(macaddr, if_hw);
                        ssid = ssid_tb_get(if_hw,0u);
                        if(ssid != NULL){
                            memcpy(ssid ->ra_mac,if_hw,MAC_LEN);
                            ssid -> rssi = rssi;
                        }
                    }
                }
                else
                {
                    break;
                }
                i++;
            }
            pclose(fp);
        }
        check_if_has_aged();
//        overTime = AP_POWER_AUTO_TIME_MS  + rand()%(AP_POWER_AUTO_TIME_MS);
//        printf("************************* overTime : %d : %s ************************\n",overTime,__func__);
//        MTIMER_START(&ap_collect_timer, overTime);
    }
    }
}
#endif

void send_ap_clients(unsigned int sta_num, uchar *sta_mac, sta_status stu)
{
	uchar buffer[URCP_BUF_SIZE];
	uint32 len;
	urcp_hdr_t *header;
	cmd_hdr_sta_info_t *cmd;
	int iftag = 0;

	memset(buffer, 0, sizeof(buffer));
	iftag = urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_hdr_sta_info_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_hdr_sta_info_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

	cmd->type = htonl(TRAP_STA_INFO);
	cmd->cmd_status = htons(URCP_OK);
	cmd->value = htonl(sta_num);
	cmd->sta_status = htonl(stu);
	cmd->length = htons(sizeof(cmd_hdr_sta_info_t));
	memmove(cmd->sta_mac, sta_mac, MAC_LEN);

	len += sizeof(cmd_hdr_sta_info_t);

	urcp_send(0u, &(buffer[0]), len);
	if(stu==AP_STA_CONNECT) {
	    ap_trap_sta_timer_set(1u);
	}
}

static int szone_updatad = 0;
void szone_updata_ack(void)
{
    szone_updatad = 1;
}

void ap_send_hello(void)
{
    uchar  buffer[URCP_BUF_SIZE], *data;
    uint32 len;
    urcp_hdr_t* header;
    cmd_discover_t* cmd;
    FatFitSwitchProfile *prof=NULL;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
	uchar nomac[MAC_LEN]={'\0','\0','\0','\0','\0','\0'};
	int iftag = 0;

    memset(buffer, 0, sizeof(buffer));
	if(mac_cmp(ap_infor.ac_infor.mac,(uchar*)nomac) == 0)
	{
		memcpy(ap_infor.ac_infor.mac, mac_addr_cooperate, MAC_LEN);
	}
	iftag =  urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
		cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		/*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
		cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}

    cmd -> type = htonl(TRAP_DISCOVER);
    cmd -> cmd_status = htons(URCP_OK);
    cmd -> send_flag = htonl(DEV_TYPE_FIT_AP);
    cmd -> recv_flag = htonl(DEV_TYPE_AC);
    cmd -> length = htons(sizeof(cmd_discover_t));
    len += sizeof(cmd_discover_t);
    
    prof=(FatFitSwitchProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
		if(prof->fatEnable == PROF_ENABLE) 
		{
	    	cmd -> send_flag = htonl(DEV_TYPE_FAT_AP);
		}
		else
		{
	    	cmd -> send_flag = htonl(DEV_TYPE_FIT_AP|DEV_FLAG_MANAGE);
		}
    }
    data = (uchar*)(&buffer[len]);
    trap_get_product_model(NULL, &data, NULL);
    header -> cmd_no += 1u; 

#ifdef URCP_CMD_SZONE_UPDATA_GET
    if(szone_updatad){
        trap_get_szone_updata_ack(NULL, &data, NULL);
        header -> cmd_no += 1u; 
        szone_updatad = 0;
    }
#endif
    trap_get_serial_no(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_dev_name(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_sw_version(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_hw_version(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_ap_ssid(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_ap_channel(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_ap_clients(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_wireless_mode(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_ap_dhcp_en(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_ap_sz_num(NULL, &data, NULL);
    header -> cmd_no += 1u; 
	
    trap_get_ap_rf_rtx(NULL, &data, NULL);
    header -> cmd_no += 1u;
    trap_get_ap_sys_info(NULL, &data, NULL);	    /*AP详情显示*/ 
    header -> cmd_no += 1u;
    trap_get_ap_rf(NULL, &data, NULL);
    header -> cmd_no += 1u;
	
#if AP_WL_POWER_AUTO
    trap_get_ap_ra0_mac(NULL, &data, NULL);
    header -> cmd_no += 1u; 
    trap_get_ap_wl_power(NULL, &data, NULL);
    header -> cmd_no += 1u; 
#endif
#if (FIT_AP_MAC_FILTER == FYES)
    trap_get_ap_mf_name(NULL, &data, NULL);
    header -> cmd_no += 1u;
#endif
#ifdef URCP_TRAP_CMD_AP_STAT
	send_if_stat(NULL, &data, NULL);
	header -> cmd_no += 1u;
#endif
	len = data - buffer;

    //printf("%s\n", __func__);
    /*广播发送*/
    urcp_send(0u, &(buffer[0]), len);
}
void ap_up_info_to_ac(uint32 sec)
{
    DBCK();
#ifdef URCP_TRAP_CMD_AP_STAT
    /*当AC请求读取AP配置时，速率包不需要和心跳包一起发送*/
#if 0
	if(sec > 10)
	{	
		MTIMER_START(&ap_stat_timer,sec*1000u - AP_STAT_PERIOD_SEC*1000);
	}
	else
	{	
		MTIMER_START(&ap_stat_timer,0);
	}
#endif
#endif
    MTIMER_START(&beat_timer, sec * 1000u);
}
void ap_heart_beat(void)
{
    if(MTIMER_TIMEOUT(&beat_timer))
    {
        ap_send_hello();
	MTIMER_START(&beat_timer,AP_HEART_BEAT_TIME_MS);
    }
}
void ap_state_handle(void)
{
	uint32 lan_ip = 0u, mask = 0u;
	ap_state_t state;
	FatFitSwitchProfile *prof=NULL;
	MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
	uchar if_name[32];

	if(MTIMER_TIMEOUT(&state_timer))
	{
		prof=(FatFitSwitchProfile*)ProfGetInstPointByIndex(mibType, 0);
		if(prof != NULL)
		{
			if(prof->fatEnable == PROF_ENABLE)
			{
				change_state(AP_STATE_UNMANGEDED);
				fat_ap = 1u;
			}
			else
			{
				if(fat_ap == 1u)
				{
					change_state(AP_STATE_INIT);
					fat_ap = 0u;
				}
			}
		}
		state = read_state();
		switch(state)
		{
			case AP_STATE_INIT:
				DBCK("AP_STATE_INIT");
				memset(if_name, 0, sizeof(if_name));
				get_switch_if_name(if_name);
				get_ip(if_name, &lan_ip);
				get_mask(if_name, &mask);
				lan_ip = ntohl(lan_ip);
				mask = ntohl(mask);

				if((lan_ip & mask) > 0u)
				{
					change_state(AP_STATE_DISCOVERY);
					timeout = 0u;
					unauth_times = 0u;
					auth_times = 0u;
					update_times = 0u;
				}
				break;
			case AP_STATE_DISCOVERY:
				DBCK("AP_STATE_DISCOVERY");
				if(timeout == 0u)
				{
					urcp_ac_discover();
					pthread_mutex_lock(&apMutex);
					timeout = ap_infor.ac_timeout;
					pthread_mutex_unlock(&apMutex);
				}
				else
				{
					timeout--;
				}
				break;
			case AP_STATE_UNAUTH:
				DBCK("AP_STATE_UNAUTH");
#if 1
				if(timeout == 0u)
				{
					timeout = AP_STATE_UNAUTH_TIME_OUT;
					unauth_times++;
					if(unauth_times > MAX_OVERTIME_NUMBER)
					{
						change_state(AP_STATE_DISCOVERY);
						timeout = 0u;
						unauth_times = 0u;
						auth_times = 0u;
						update_times = 0u;
					}
					else
					{
						DBCK("ap request auth");
						urcp_send_ap_auth_request();
					}
				}
				else
				{
					timeout--;
				}
#else
				if (MTIMER_TIMEOUT(&ap_auth_timer)) {
					DBCK();
					urcp_send_ap_auth_prepare();
					DBCK("sleep 5......");
					sleep(5);
					urcp_send_ap_auth_request();
					MTIMER_START(&ap_auth_timer, AP_AUTH_TIME_MS);
				}
#endif
				break;
			case AP_STATE_AUTH: 
				DBCK("AP_STATE_AUTH");
				if(timeout == 0u)
				{
					timeout = AP_STATE_AUTH_TIME_OUT;
					auth_times++;
					if(auth_times > MAX_OVERTIME_NUMBER)
					{
						DBCK();
						change_state(AP_STATE_DISCOVERY);
						timeout = 0u;
						unauth_times = 0u;
						auth_times = 0u;
						update_times = 0u;
					}
					else
					{
						DBCK();
						if (ap_update_flag == 0) {
							DBCK();
							urcp_send_update_fireware_reqest();
						} else {
							DBCK();
							change_state(AP_STATE_NORMAL);
						}
					}
				}
				else
				{
					DBCK();
					timeout--;
				}
				break;
			case AP_STATE_SOFEUPDATE: 
				DBCK("AP_STATE_SOFEUPDATE");
				//get_ip_from_dhcp_server(ap_infor.ac_infor.ip);
				urcp_auto_get_file(ap_infor.ac_infor.mac, FILE_ID_IMAGE);
				break;
			case AP_STATE_CONFUPDATE_REQ:
				DBCK("AP_STATE_CONFUPDATE_REQ");
				if(timeout == 0u)
				{
					timeout = AP_STATE_CONFUPDATE_REQ_TIME_OUT;
					update_times++;
					if(update_times > MAX_OVERTIME_NUMBER)
					{
						change_state(AP_STATE_DISCOVERY);
						timeout = 0u;
						unauth_times = 0u;
						auth_times = 0u;
						update_times = 0u;
					}
					else
					{
						urcp_send_config_update_reqest();
						/*验证过程中改为由DHCP获取IP地址*/
			//			setInterfaceDhcp(1);
			//			get_ip_from_dhcp_server(ap_infor.ac_infor.ip);
					}
				}
				else
				{
					timeout--;
				}
				break;
			case AP_STATE_CONFUPDATE:
				DBCK("AP_STATE_CONFUPDATE");
				urcp_auto_get_file(ap_infor.ac_infor.mac, FILE_ID_CONFIG);
				break;
			case AP_STATE_UNMANGEDED:
                ap_heart_beat();
				break;
			case AP_STATE_NORMAL:
				DBCK("AP_STATE_NORMAL");
				if (ac_infor_times < MAX_AC_INFOR_TIMES) {
					ac_infor_times++;
//					printf("ac_infor_times = %d\n", ac_infor_times);
                    ap_heart_beat();
				} else {
//                    ap_heart_beat();
					change_state(AP_STATE_UNAUTH);
				}
				break;
			default:
				DBCK("default");
				break;
		}

		MTIMER_START(&state_timer, AP_STATE_TIME_MS);
	}
}

/*
 * initial ap state timer.
 */
void ap_timer_init(void)
{
    MTIMER_START(&state_timer, 0);
    MTIMER_START(&ap_auth_timer, 0);
    MTIMER_START(&beat_timer, AP_TIMER_DELAY_MS + AP_HEART_BEAT_TIME_MS);
#ifdef URCP_TRAP_CMD_AP_STAT
    MTIMER_START(&ap_stat_timer, AP_TIMER_DELAY_MS + AP_STAT_TIME_MS);
#endif
#if AP_WL_POWER_AUTO
    MTIMER_START(&ap_collect_timer, AP_TIMER_DELAY_MS + AP_POWER_AUTO_TIME_MS);
#endif

    MTIMER_START(&conf_timer, AP_TIMER_DELAY_MS + AP_CONFIG_BACKUP_TO_AC_TIME_MS);
#if AP_SEND_CLTS_AUTO
    ap_trap_sta_timer_init();
#endif
}
/*
 * set backup state NEED_NOT_BACKUP or NEED_BACKUP.
 */
void config_backup_state(bakup_state_t enable)
{
    URCP_DEBUG(("Function:%s()\n", __func__));
    backup = enable; /*need backup*/
    return;
}

/* 
 * AP auto Backup config file to AC /tmp/urcp/ap_configs/serial.xml file
 */
void config_backup_timer(void)
{
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;
    ApEffectConfProfile *prof= NULL;
    char mac[MAC_LEN];
	int32 sn;
	sn = getSystemSn();

    prof = (ApEffectConfProfile*)ProfGetInstPointByIndex(mibType,0);
    if((prof != NULL) && (prof->config == PROF_DISABLE)) /*not local*/
    {
	if(MTIMER_TIMEOUT(&conf_timer))
	{
	    memset(mac, 0, sizeof(mac));
	    URCP_DEBUG(("Function:%s(), backup=%d\n", __func__,backup));
	    if((backup == NEED_BACKUP) && (memcmp(mac, ap_infor.ac_infor.mac, MAC_LEN) != 0))
	    {
		upload_current_config_file_to_ac(ap_infor.ac_infor.mac, sn);
		config_backup_state(NEED_NOT_BACKUP);
	    }
	    MTIMER_START(&conf_timer, AP_CONFIG_BACKUP_TO_AC_TIME_MS);
	}
    }

    return;
}

#ifdef URCP_TRAP_CMD_AP_STAT
/*
 *
 *
 *
 * */
void ap_sendRate_init(void)
{
    uchar if_name[32];
    unsigned int rx = 0,tx = 0;
    memset(if_name, 0, sizeof(if_name));	
    get_switch_eth_name(if_name);//获取LAN；
    get_if_stat(if_name,&rx,&tx);//读取rx和tx；
    rx_byte = rx;
    tx_byte = tx;
    return;
}
/***************************************
* 函数名：get_if_stat()
* 功能：读取接口的收发字节数
* 参数：ifname为查找的lan接口名，rx返回接收字节数，tx返回发送字节数
* 返回值：0读取成功，1读取失败
***************************************/
int get_if_stat(char *ifname, unsigned int *rx, unsigned int *tx)
{
    FILE *f;
    char buf[256];
    unsigned int rx_pkts = 0, tx_pkts = 0, rx_err, tx_err;
    char *p,*tempStr = NULL;
    int ret = 1;
    if ((f = fopen("/proc/net/dev", "r")) != NULL) 
    {
	fgets(buf, sizeof(buf), f);
	fgets(buf, sizeof(buf), f);
	while (fgets(buf, sizeof(buf), f)) 
	{
	    if ((p = strchr(buf, ':')) != NULL)
	    {
		*p = 0;
		if ((tempStr = strrchr(buf, ' ')) == NULL) 
		{
		    tempStr = buf; /* To get interface name */
		}
		else 
		{
		    ++tempStr;//跳过空格
		}
		if (strcmp(ifname,tempStr) == 0)
		{
		    if (sscanf(p + 1, "%lu%u%u%*u%*u%*u%*u%*u%lu%u%u", rx,
				&rx_pkts, &rx_err, tx, &tx_pkts, &tx_err) == 6)
		    {
			ret = 0;
			break;
		    }
		}
	    }
	}
	fclose(f);
    }
    return ret;
}
/*
 *
 *
 *
 * */
static void send_rate_to_ac(void)
{
    uchar  buffer[URCP_BUF_SIZE], *data;
    uint32 len;
    urcp_hdr_t* header;
    cmd_discover_t* cmd;
    FatFitSwitchProfile *prof=NULL;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
    uchar nomac[MAC_LEN]={'\0','\0','\0','\0','\0','\0'};
    int iftag = 0;

    memset(buffer, 0, sizeof(buffer));
    if(mac_cmp(ap_infor.ac_infor.mac,(uchar*)nomac) != 0)
    {
	iftag =  urcp_buid_header(buffer, ap_infor.ac_infor.mac, (uint16)URCP_TRAP, 0, NULL);
	if(iftag == 1)
	{
	    /*指向包头 带tag*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);
	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	    /*联动发现 */
	    cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}
	else
	{
	    /*指向包头*/
	    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);
	    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
	    /*联动发现 */
	    cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	cmd -> type = htonl(TRAP_DISCOVER);
	cmd -> cmd_status = htons(URCP_OK);
	cmd -> send_flag = htonl(DEV_TYPE_FIT_AP);
	cmd -> recv_flag = htonl(DEV_TYPE_AC);
	cmd -> length = htons(sizeof(cmd_discover_t));
	len += sizeof(cmd_discover_t);
	prof=(FatFitSwitchProfile*)ProfGetInstPointByIndex(mibType, 0);
	if(prof != NULL)
	{
	    if(prof->fatEnable == PROF_ENABLE) 
	    {
		cmd -> send_flag = htonl(DEV_TYPE_FAT_AP);
	    }
	    else
	    {
		cmd -> send_flag = htonl(DEV_TYPE_FIT_AP|DEV_FLAG_MANAGE);
	    }
	}
	data = (uchar*)(&buffer[len]);
	send_if_rate(NULL, &data, NULL);
	header -> cmd_no += 1u;

	len = data - buffer;
	/*广播发送*/
	urcp_send(0u, &(buffer[0]), len);
    }
    return;
}

/***************************************
* 函数名：ap_stat()
* 功能：AP定时函数，读取接口的收发字节数
* 参数： 无
* 返回值：无
***************************************/
void ap_stat(void)
{
    if(MTIMER_TIMEOUT(&ap_stat_timer))
    { 	
	MTIMER_START(&ap_stat_timer,AP_STAT_TIME_MS);
	/*send rate to AC*/
	send_rate_to_ac();
    }
    return;
}
/***************************************
* 函数名：get_if_rate()
* 功能：计算AP LAN口收发速率
* 参数：rx_rate返回接收速率，tx_rate返回发送速率
* 返回值：无
***************************************/
void get_if_rate(uint32* rx_rate,uint32* tx_rate)
{
    uchar if_name[32];
    unsigned int rx = 0,tx = 0;
    memset(if_name, 0, sizeof(if_name));	
    get_switch_eth_name(if_name);//获取LAN名字；
    get_if_stat(if_name,&rx,&tx);//读取rx和tx；
    //计算rx_rate
    if(rx >= rx_byte)
    {
	(*rx_rate) = (rx - rx_byte)/(AP_STAT_TIME_MS/1000);
    }
    else// if(rx < rx_byte)
    {
	(*rx_rate) = (0xFFFFFFFFu + rx - rx_byte)/(AP_STAT_TIME_MS/1000);
    }
    //计算tx_rate
    if(tx >= tx_byte)
    {
	(*tx_rate) = (tx - tx_byte)/(AP_STAT_TIME_MS/1000);
    }
    else// if(tx < tx_byte)
    {
	(*tx_rate) = (0xFFFFFFFFu + tx - tx_byte)/(AP_STAT_TIME_MS/1000);
    }
    rx_byte = rx;
    tx_byte = tx;
    return;
}
#endif
#endif
