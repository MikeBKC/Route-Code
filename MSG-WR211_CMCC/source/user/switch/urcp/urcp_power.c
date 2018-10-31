/*
 *
 *�������ִ�����
 *KeHuatao
 *2010-09-04
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> /*inet_addr*/
#include <linux/switch/swioc_common.h>
#include "common.h"
#include "urcp.h"
#include "urcp_cmd.h"
#include "app_msg.h"
#include "urcp_hello.h"
#include "urcp_tftp.h"

#if AP_WL_POWER_AUTO
#ifdef URCP_AP


#define NONE_NODE    0
extern around_env_ssid_info_t around_env_ssid_info;
#define GL_SSID_TB   (around_env_ssid_info.ssid_info)

/*
 *  *
 *   *�����豸hash���key
 *    *
 *     */
uint32 urcp_ssid_table_hash(uchar* mac, uint32 ip)
{
    uint32 key;

    /*�����ֽ����*/
    key = (mac[0] ^ mac[1] ^ mac[2] ^ mac[3] ^
	    mac[4]) ^ mac[5];
    /*��λ�͵�λ���*/
    key = ((key >> 4) | key) % SSID_HASH_SIZE;
#if  DEV_HASH_USE_IP  /*not use ip,keHuatao*/
    /*��ip���*/
    key = (ip & 0x1fu) ^ ((ip >> 5) & 0x1fu);
#endif

    return key;
}

/*
 *��ȡһ�����еĽڵ�
 *
 */
static int32 get_free_ssid_node(void)
{
    int free = NONE_NODE;
    int i;

    for(i=0;i<MAX_AROUND_SSID ;i++)
    {
	if(GL_SSID_TB[i].action==0)
	{
	    free = i + 1;
	    break;
	}
    }
    if(i>=MAX_AROUND_SSID )
    {
	URCP_ERROR(("urcp_dev: get free node failed\n"));
    }

    /*����ָ��*/
    return free;
}

around_ssid_info_t* ssid_tb_get(uchar* ssid_mac, uint32 ssid_ip)
{
    uint32 key, index;
    around_env_ssid_info_t* env_p = &around_env_ssid_info;
    around_ssid_info_t* tmp=NULL;

    /*calcu hash key*/
    key = urcp_ssid_table_hash(ssid_mac, ssid_ip);
    index = env_p-> ssid_tb_hash[key];
    if(index == NONE_NODE)
    {
	URCP_DEBUG(("hash :%d not used\n", key));
	index = get_free_ssid_node();
	env_p-> ssid_tb_hash[key] = index;
    }
    else
    {
	URCP_DEBUG(("hash used\n"));
	tmp = &(GL_SSID_TB[index-1]);
	/*�ҵ�hash��ĩβ*/
	while(
		(
#if DEV_HASH_USE_IP  
//                 (tmp->ip != dev_ip) || 
#endif
                 (mac_cmp(tmp->ra_mac, ssid_mac) != 0))/*��Ҫ������豸����ͬһ���豸*/    
		&& (tmp->tb_next != NONE_NODE)) 
	{
#ifdef DEBUG_URCP
	    index = tmp->tb_next;
#endif
	    tmp = &GL_SSID_TB[tmp->tb_next-1];
	}

	if(
#if DEV_HASH_USE_IP  
//		(tmp->ip == dev_ip) &&
#endif
		(mac_cmp(tmp->ra_mac, ssid_mac)==0))/*�Ѿ��������*/
	{
	    URCP_DEBUG(("dev exist: node %d\n", index));
	    tmp->aged = 0;
	    return tmp;
	}
	/*��ȡ�սڵ�*/
	index = get_free_ssid_node();
	tmp -> tb_next = index;
    }

new_node:
    URCP_DEBUG(("dev new node: %d\n", index));
    if(index!=NONE_NODE)
    {
	tmp = &(GL_SSID_TB[index-1]);
	memset(tmp, 0, sizeof(around_ssid_info_t));
	/*copy*/
	tmp->action = 1;
	tmp->aged = 0;
	tmp->tb_next = NONE_NODE;
	memcpy(tmp->ra_mac, ssid_mac, MAC_LEN);
    DBCK("+++++++++++++++ dev new node: %d : ra_mac: %s\n", index,str_macaddr(tmp->ra_mac));
//printf("+++++++++++++ dev new node: %d : ra_mac: %s \n", index,str_macaddr(tmp->ra_mac));
	return tmp;
    }
    return NULL;
}

void check_if_has_aged(void){
    int i;
    int32 tb_index;
    around_ssid_info_t *prev = NULL,*tmp = NULL;
    for(i = 0u; i < SSID_HASH_SIZE; i++)
    {
        prev = NULL;
//        tb_index = shm_ptr->dev_tb_hash[i];
        tb_index = around_env_ssid_info.ssid_tb_hash[i];
        while(tb_index!=NONE_NODE)
        {
//            tmp = &shm_ptr->dev_table[tb_index-1];
            tmp = &(GL_SSID_TB[tb_index-1]);
            if(tmp->aged>=2)
            {
                DBCK("MAC: %s aged\n", str_macaddr(tmp->ra_mac));
//printf("----------------- ra_mac: %s index: %d rssi:%d \n",str_macaddr(tmp->ra_mac),tb_index,tmp->rssi);
                DBCK("---------------------- ra_mac: %s is offline\n",str_macaddr(tmp->ra_mac));

                tmp->action = 0;
                tmp->aged = 0;
                if(prev!=NULL) {
                    prev->tb_next = tmp->tb_next;
                }
                else {
                    around_env_ssid_info.ssid_tb_hash[i] = tmp->tb_next;
                }
            }
            else
            {
//printf("================== ra_mac: %s index: %d rssi:%d \n",str_macaddr(tmp->ra_mac),tb_index,tmp->rssi);
                tmp->aged += 1;
                prev = tmp;
            }
            tb_index = tmp->tb_next;
        }
    }
}

around_ssid_info_t* urcp_ssid_table_lookup(uchar* mac, uint32 ip)
{
    uint32  key;
    int32 index;
    urcp_dev_t* tmp=NULL;
    around_env_ssid_info_t* env_p = &around_env_ssid_info;

    /*calcu key*/
    key = urcp_ssid_table_hash(mac, ip);
    index = env_p-> ssid_tb_hash[key];
//    index = shm_ptr->dev_tb_hash[key];
    if(index != NONE_NODE)
    {
	/*��Ϊ��*/
	tmp = &GL_SSID_TB[index-1]; 
	while(index != NONE_NODE)
	{
	    /*compare*/
	    if ((mac_cmp(GL_SSID_TB[index -1].ra_mac, mac) == 0)
#if DEV_HASH_USE_IP
//		    && ((GL_SSID_TB[index -1]).ip==ip)
#endif
		    )
	    {
		/*get it*/
#if (DEV_HASH_USE_IP!=1)
//		GL_SSID_TB[index -1].ip=ip;
#endif
		return &GL_SSID_TB[index - 1];
	    }
	    index = GL_SSID_TB[index-1].tb_next;
	}
    }
    /*û�ҵ�*/
    return NULL;
}
#endif
#endif
