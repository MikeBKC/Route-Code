/*
 *
 *联动发现处理函数
 *KeHuatao
 *2010-09-04
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> /*inet_addr*/
#include <linux/switch/swioc_common.h>
#include <switch.h>
#include <sw_cfg.h>
#include <sw_ioctl.h>
#include <time.h>
#include "common.h"
#include "urcp.h"
#include "urcp_cmd.h"
#include "app_msg.h"
#include "urcp_hello.h"
#include "urcp_tftp.h"
#include "urcp_debug.h"
#include <syslog.h>
#include <uttSem.h>


#define SM_DEV_TB   (shm_ptr->dev_table)

//static int free_index = DEV_HASH_SIZE;
urcp_shm_t* shm_ptr = NULL;

/*
 *
 *静态变量
 *
 */
static int32 get_free_node(void);

/*
 *获取一个空闲的节点
 *
 */
static int32 get_free_node(void)
{
    int free = NONE_NODE;
#if 1
    int i;

    for(i=0;i<DEV_TABLE_SIZE;i++)
    {
	if(SM_DEV_TB[i].used==0)
	{
	    free = i + 1;
	    break;
	}
    }
    if(i>=DEV_TABLE_SIZE)
    {
	URCP_ERROR(("urcp_dev: get free node failed\n"));
    }
#else
    urcp_dev_t* tmp;

    /*是否还有空节点*/
    if (free_index >= DEV_TABLE_SIZE)
    {
	/*用完了*/
	URCP_ERROR(("urcp_dev: get free node failed\n"));
	return NONE_NODE;
    }

    URCP_DEBUG(("get node: %d\n", free_index));
    tmp =  &(SM_DEV_TB[free_index]);
    free_index = free_index + 1;
    tmp->tb_index = free_index;/*比实际索引大一*/
#endif

    /*返回指针*/
    return free;
}
/*
 * send packet
 * KeHuatao
 */
void urcp_reset(void)
{
    dev_table_init();
}
/*
 *将设备表清0
 *
 */
void dev_table_init(void)
{
    shm_ptr = get_shm_ptr();
    /*clear*/
    memset(shm_ptr->dev_table, 0, sizeof(shm_ptr->dev_table));
    memset(shm_ptr->dev_tb_hash, 0, sizeof(shm_ptr->dev_tb_hash));
    /*clear index*/
    //free_index = DEV_HASH_SIZE;
}
#if 0
void dev_dump(urcp_dev_t*dev)
{
    uint32 ip;
    struct in_addr ip_addr;

    URCP_DEBUG(("%s:\n", __func__));
    ip = htonl(dev->ip);
    memcpy(&ip_addr, &ip, IP_LEN);
    URCP_DEBUG(("        MAC:%s\n", str_macaddr(dev->mac)));
    URCP_DEBUG(("        IP:%s\n", inet_ntoa(ip_addr)));
    URCP_DEBUG(("        name:%s\n", dev->dev_name));
    URCP_DEBUG(("        SN:%d\n", dev->serial_no));
    URCP_DEBUG(("        sw:%s\n", dev->sw_version));
    URCP_DEBUG(("        up_ports:%d\n", dev->up_ports));
}
#endif
#if 0
/*
 *从设备表中读取密码
 *
 */
void get_dev_password(uchar* mac, uint32 ip, uchar* password)
{
    urcp_dev_t* tmp;

    URCP_DEBUG(("%s\n", __func__));
    /*搜索设备表*/
    tmp = urcp_dev_table_lookup(mac , ip);
    if (tmp != NULL)
    {
	/*copy password*/
	memcpy(password, tmp -> password, MAX_PASSWORD_LEN);
    }
}
#endif
#ifdef URCP_CMD_HELLO_GET
/*
 *
 *收到联动发现请求包
 * 发送回复包
 *
 */
uint16 get_hello(cmd_hdr_t *cmd, uchar** app_data, urcp_infor_t* urcp_info)
{
    cmd_hdr_t* cmd_snd; 

    URCP_DEBUG(("%s\n", __func__));
    cmd_snd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd_snd -> type = htonl(CMD_HELLO);
    cmd_snd -> cmd_status = htons(URCP_OK);
    /*length*/
    cmd_snd -> length = htons(sizeof(cmd_hdr_t));
    *app_data = *app_data + sizeof(cmd_hdr_t);

    return 0;
}
#endif
#ifdef URCP_CMD_HELLO_GET_REPLY
/*
 *收到联动回复包
 *将设备添加到设备表
 */
uint16 receive_hello(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    URCP_DEBUG(("%s\n", __func__));
    (*dev)->link_port = urcp_info->source_port;
    (*dev)->urcp_version = ntohs(urcp_info->urcp_header->version) & VERSION_MASK;

    return URCP_OK;
}
#endif
/*
  *发送一个联动发现包
 *
 */
int32 urcp_discover(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd;

    urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_GET, id, NULL);
	/*指向包头 带tag*/
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

	len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*联动发现 */
	cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
	/*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
	{
		return -1;
	}
    len += add_cmd(cmd, CMD_HELLO);
    header -> cmd_no = 1u; 

    /*读取序列号*/
    cmd++;
    len += add_cmd(cmd, CMD_SERIAL_NO);
    header -> cmd_no += 1u; 
    
    /*读取设备命令*/
    cmd++;
    len += add_cmd(cmd, CMD_DEV_NAME);
    header -> cmd_no += 1u; 

    /*读取活动端口数*/
    cmd++;
    len += add_cmd(cmd, CMD_UP_PORTS);
    header -> cmd_no += 1u; 

    /*读取软件版本 */
    cmd++;
    len += add_cmd(cmd, CMD_SW_VERSION);
    header -> cmd_no += 1u; 

    cmd++;
    len += add_cmd(cmd, CMD_HW_VERSION);
    header -> cmd_no += 1u; 

    cmd++;
    len += add_cmd(cmd, CMD_PRODUCT_MODEL);
    header -> cmd_no += 1u; 
	/*发包*/
	urcp_send_by_vlan(buffer,len);

    return 0;
}

/******************************************************************************
 *函数：cooperate_mac_add
 *功能：将联动的组播包mac加入mac表中
 *参数： 无
 *返回： 无
 *日期：2010-09-07
 *作者：jfgu
 ****************************************************************************/ 
extern void cooperate_mac_add(void)
{
    mac_table_entry_t mac_entry;
    
    memset(&mac_entry, 0, sizeof(mac_table_entry_t));

    mac_entry.vid_mac.vid = sw_cpu_vid_get();

    mac_entry.destination[CPU_PORT] = 1;
    memcpy(mac_entry.vid_mac.mac.addr, mac_addr_cooperate, MAC_LEN);

    sw_mac_table_static_learn(&mac_entry);
}

/*
 *根据mac和ip查找设备节点
 *没有找到则新分配空间
 */
urcp_dev_t* dev_get(uchar* dev_mac, uint32 vid, uint32 dev_ip, int32 dev_type)
{
    uint32 key, index,drawing_dev_type_tmp;
    urcp_dev_t* tmp=NULL;

    /*calcu hash key*/
    key = urcp_dev_table_hash(dev_mac, dev_ip);
    db_loud(URCP_MODULE_ID_URCP, "%s: mac: %s key: %d, type: 0x%x\n", __func__, str_macaddr(dev_mac), key, dev_type);   
    index = shm_ptr -> dev_tb_hash[key];
    if(index == NONE_NODE)
    {
	db_loud(URCP_MODULE_ID_URCP, "hash :%d not used\n", key);
	index = get_free_node();
	shm_ptr -> dev_tb_hash[key] = index;
    }
    else
    {
	db_loud(URCP_MODULE_ID_URCP, "hash used\n");
	tmp = &(SM_DEV_TB[index-1]);
	/*找到hash表末尾*/
	while(
		(
#if DEV_HASH_USE_IP  
                 (tmp->ip != dev_ip) || 
#endif
                 (mac_cmp(tmp->mac, dev_mac) != 0))/*和要插入的设备不是同一个设备*/    
		&& (tmp->tb_next != NONE_NODE)) 
	{
#ifdef DEBUG_URCP
	    index = tmp->tb_next;
#endif
	    tmp = &SM_DEV_TB[tmp->tb_next-1];
	}

	if(
#if DEV_HASH_USE_IP  
		(tmp->ip == dev_ip) &&
#endif
		(mac_cmp(tmp->mac, dev_mac)==0))/*已经保存过了*/
	{
	    db_loud(URCP_MODULE_ID_URCP, "dev exist: node %d\n", index);

#if (AP_LOG_MANAGE == FYES)
		DBCK("tmp->aged = %d", tmp->aged);
	if (tmp->aged == 2) {
		DBCK();
		char log_buf[MAX_LOG_LENGTH - 32];
		sprintf(log_buf, "ap mac: %s; event: online", str_macaddr(dev_mac));
		DBCK();
		ap_syslog(APLINKLOG, log_buf);
		DBCK();
	}
#endif
#if (AP_LOAD_BALANCE == FYES)
	DBCK("tmp->aged = %d", tmp->aged);
    if (tmp->aged == 2) {
        lb_set_when_ap_online(tmp);
    }
#endif
	    tmp->aged = 0;
		tmp->ip = dev_ip;
		tmp->vid = vid;
		/*drawing_type的值放到drawing_dev_type_tmp*/
	    drawing_dev_type_tmp = tmp->dev_type & 0x20u;
	    tmp->dev_type = dev_type;
	    tmp->dev_type |= drawing_dev_type_tmp;/*保留建筑图纸的管理状态*/
	    return tmp;
	}
	/*获取空节点*/
	index = get_free_node();
	tmp -> tb_next = index;
    }

new_node:
    db_loud(URCP_MODULE_ID_URCP, "dev new node: %d\n", index);
    if(index!=NONE_NODE)
    {
	tmp = &(SM_DEV_TB[index-1]);
	memset(tmp, 0, sizeof(urcp_dev_t));
	/*copy*/
	tmp->used = 1;
	tmp->aged = 0;
	tmp->tb_next = NONE_NODE;
	tmp->ip = dev_ip;
	tmp->dev_type = dev_type;
	memcpy(tmp->mac, dev_mac, MAC_LEN);
	tmp->vid = vid;

#if (AP_LOG_MANAGE == FYES)
	DBCK();
	char log_buf[MAX_LOG_LENGTH - 32];
	sprintf(log_buf, "ap mac: %s; event: online", str_macaddr(dev_mac));
	ap_syslog(APLINKLOG, log_buf);
#endif
#if (AP_LOAD_BALANCE == FYES)
    lb_set_when_ap_online(tmp);
#endif

    return tmp;
    }
    return NULL;
}

#if AP_WL_POWER_AUTO
int32 dev_get_index_by_mac(uchar* mac, uint32 ip)
{
    uint32  key;
    int32 index;
    urcp_dev_t* tmp=NULL;

    /*calcu key*/
    key = urcp_dev_table_hash(mac, ip);
    index = shm_ptr->dev_tb_hash[key];
    if(index != NONE_NODE)
    {
	/*不为空*/
	tmp = &SM_DEV_TB[index-1]; 
	while(index != NONE_NODE)
	{
	    /*compare*/
	    if ((mac_cmp(SM_DEV_TB[index -1].mac, mac) == 0)
#if DEV_HASH_USE_IP
		    && ((SM_DEV_TB[index -1]).ip==ip)
#endif
		    )
	    {
		/*get it*/
#if (DEV_HASH_USE_IP!=1)
//		SM_DEV_TB[index -1].ip=ip;
#endif
//		return &SM_DEV_TB[index - 1];
		return index;
	    }
	    index = SM_DEV_TB[index-1].tb_next;
	}
    }
    /*没找到*/
    return NULL;
}
#endif

/*
 *发送一个联动发现包
 *
 */
int32 urcp_mirror_can(void)
{
    uchar  buffer[URCP_BUF_SIZE];
    uint32 len;
    uint16 id;
    urcp_hdr_t* header;
    cmd_hdr_t* cmd;

	urcp_buid_header_with_8021q(buffer, mac_addr_cooperate, (uint16)URCP_SET, id, NULL);
    /*指向包头 带tag*/
    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
	/*联动发现 */
	cmd = (cmd_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);

	/*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, 0u, URCP_CMD_TIME_OUT, &id, NULL)<0)
	{
		return -1;
	}
    len += add_cmd(cmd, CMD_MIRROR_CAN);

	/*发包*/
	urcp_send_by_vlan(buffer,len);

    return 0;
}
#if (FEATURE_AP_MAIL == FYES)
void write_msg_to_ap_mail(char* msg)
{
	FILE *fp = NULL;
	char linestr[MAX_LOG_LENGTH + 1];
	char logstr[MAX_LOG_LENGTH + 1];
	char *strp = NULL;
	unsigned long sec;
	struct tm *date;

	memset(linestr, 0, sizeof(linestr));
	memset(logstr, 0, sizeof(logstr));

	memcpy(linestr, msg, MAX_LOG_LENGTH);
	strp = strchr(linestr, ' ');
	sec = strtoul(linestr, NULL, 10);

	if (sec) {
			date = localtime((time_t*)&sec);
			snprintf(logstr, MAX_LOG_LENGTH, "%d-%02d-%02d %02d:%02d:%02d %s", \
					(1900 + date->tm_year), (1 + date->tm_mon), date->tm_mday, \
					date->tm_hour, date->tm_min, date->tm_sec, strp+1);
	}

	uttSemP(SEM_AP_MAIL, 0);/*与goahead冲突*/
	if ((fp = fopen(AP_MAIL_FILE_PATH,"a+")) == NULL)
	{
		printf("Open the file %s error!",AP_MAIL_FILE_PATH);
		return;
	}

	fputs(logstr,fp);
	fclose(fp);
	uttSemV(SEM_AP_MAIL, 0);/*与goahead冲突*/
	fp = NULL;
	return;
}
#endif

#if (AP_LOG_MANAGE == FYES)||(FEATURE_AP_MAIL == FYES)
void getSystemTime(char buf[32])
{
	FILE *fp = NULL;

	memset(buf, 0, sizeof(buf));

	fp = popen("date '+%s'", "r");
	if (fp) {
		fgets(buf, 32, fp);
		pclose(fp);

		strtok(buf, "\n");
	}

	return;
}

void clean_ap_syslog()
{
	memset(&(shm_ptr->ap_msg), 0, sizeof(ap_log));

	return;
}

void ap_syslog(ap_log_list logType, char *msg)
{
	MibProfileType mibType = MIB_PROF_AP_LOG_MANAGE;
	ApLogManageProfile *prof = NULL;
	char buf[32];

	prof = (ApLogManageProfile *)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
		if (prof->apLogType[logType] == LOG_ENABLE) {
			getSystemTime(buf);
			if (buf[0] != '\0') {
				uttSemP(SEM_AP_LOG, 0);/*与goahead冲突*/
				if (shm_ptr->ap_msg.current_msg_index >= NUM_OF_AP_LOG) {
					shm_ptr->ap_msg.current_msg_index = 0;
				}

				snprintf(shm_ptr->ap_msg.msg[shm_ptr->ap_msg.current_msg_index].msg, MAX_LOG_LENGTH, "%s %s\r\n", buf, msg);
				DBCK("current_msg_index = %d, msg = %s", shm_ptr->ap_msg.current_msg_index, shm_ptr->ap_msg.msg[shm_ptr->ap_msg.current_msg_index].msg);
				shm_ptr->ap_msg.current_msg_index++;
				uttSemV(SEM_AP_LOG, 0);/*与goahead冲突*/
#if (FEATURE_AP_MAIL == FYES)
				if(logType == APLINKLOG)
				{
					write_msg_to_ap_mail(shm_ptr->ap_msg.msg[shm_ptr->ap_msg.current_msg_index - 1].msg);
				}
#endif
			}
		}
	}

	return;
}

#endif
