/*
 *
 *
 *����Э��v3�洦��
 *
 *ke.huatao
 *2010-09-01
 *
 */
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> /*inet_addr*/
#include <linux/if_ether.h>

#include "common.h"

#ifdef URCP_WEB 
#include <switch.h>
#include <sw_ioctl.h>
#include "urcp.h"
#include <sw_cfg.h>
#include <packet.h>
#include "urcp_topology.h"
#include "urcp_hello.h"
#else
#include "urcp.h"
#include "packet.h"
#endif

#include <mib.h>
#include <profacce.h>
#include <mibUrcp.h>
#include "app.h"
#include "urcp_tftp.h"
#include "urcp_cmd.h"
#include "urcp_debug.h"
#include "urcp_ap.h"
#include "../CWAC.h"


#define URCP_HOST          0
#define URCP_MULTICAST     1
#define URCP_BRODCAST      2
#define TIME_OUT           10

#define MAX_ACTIONS  32

typedef struct _action_to_do_t{
    fp fp_delay;
}action_to_do_t;

uchar mac_addr_cooperate[MAC_LEN] = {0x01,0x00,0x00,'U','T','T'};
uchar mac_addr_broadcast[MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static urcp_state_t urcp_state;
/*
 *��̬����
 *
 */

/*
 *��̬����
 *
 */
static void urcp_next_header(urcp_hdr_t* header, urcp_infor_t* urcp_infor);
static int32 urcp_password(uchar* passwd);
static void urcp_get(urcp_infor_t* urcp_infor);
static void urcp_get_reply(urcp_infor_t* urcp_infor);
static void urcp_set(urcp_infor_t* urcp_infor);
static void urcp_set_reply(urcp_infor_t* urcp_infor);
static void urcp_trap(urcp_infor_t* urcp_infor);
static uint32 urcp_header_parse(system_frame_header_t* header, urcp_infor_t* urcp_infor);
static void urcp_error_reply(urcp_infor_t* urcp_infor);
/*
 *
 *�������ײ�next_header�ֶ�
 *
 *
 */
static void urcp_next_header(urcp_hdr_t* header, urcp_infor_t* urcp_infor)
{
    ;
}

#if 0
/*
 *�յ���λmac��ַ����Ĵ���
 *
 */
void mac_location(uchar* data, urcp_infor_t * urcp_infor, uchar* app_data, uint32* len)
{
    int idx, ret;
    mac_location_t* location_infor;
    vtss_mac_table_entry_t entry;
    vtss_vid_mac_t vid_mac;
    vtss_pgid_no_t pgid_no = 0xFFu;

    location_infor = (mac_location_t*)data;
    /*����mac������Ŀ��mac*/

    entry.vid_mac.vid = DEF_CPU_VID; 
    vid_mac.mac = location_infor->target_mac;

    if(vtss_ll_mac_table_lookup(&vid_mac, &pgid_no)!=VTSS_OK)
    {
	URCP_ERROR("mac location lookup error\n");
	return;
    }

    if(pgid_no< VTSS_PORT_NO_START || pgid_no>VTSS_PORT_NO_END)
    {
	/*û���ҵ�ת������*/
	location->hop = location->hop + 1;
	len = 
    }
    else if(pgid_no == sourc_port)
    {
	/*��Դ�˿���,��ת��*/
	*len = 0;
	return; 
    }
    else
    {
	/*���²�˿���,��������mac��*/
	get_port_mac_num();
    }
#if 0
    vtss_mac_entry_destination_set(entry, pgid_no);
#endif

    /*�ж�Ŀ��˿�*/
    :
}
#endif
/*������������
 * frame: �����͵��Ѿ���װ�õİ�
 * len�����ĳ���
 */ 
int32 urcp_send_by_vlan(uchar* frame, uint32 len)
{
#ifdef URCP_AC
    URCP_DEBUG(("%s\n",__func__));
    extern urcp_shm_t* shm_ptr;
    int i,j,k, error = 0;
    urcp_dev_t *tmp = NULL;
    int32 tb_index;
    int32 ap_vid[128]={0};
    int32 utagged = 0;
    char name[64];
    uchar mac[MAC_LEN];
    uchar *buffer=NULL;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);

    /*�㲥���鲥*/
    if((frame[0] & 0x01u) == 1u)
    {
	/*����vid��ͬ���Ͷ���㲥��*/
	/*����hash��*/
	for(i = 0u; i < DEV_HASH_SIZE; i++) {
		tb_index = shm_ptr->dev_tb_hash[i];

		while (tb_index != NONE_NODE) {
			tmp = &shm_ptr->dev_table[tb_index-1];
			if (tmp->aged != 2) {
				if(tmp->vid > 0)
				{
					j = tmp->vid >> 5u;
					k = tmp->vid & 0x1fu;
					if((ap_vid[j] & (1u << k)) == 0){
						ap_vid[j] |= (1u << k);
						*(uint16*)(frame + ETH_URCP_OFFSET) = htons((uint16)((tmp->vid & 0xfff))|0x2000);/*�޸����ݰ�vid��Ϣ*/
						/*�㲥����*/
						h2_send_frame(frame, len, tmp->fromL3);
					}
				}
			}
			tb_index = tmp->tb_next;
		}
	}
	utagged = 1;
	}else{
		tmp = urcp_dev_table_lookup(((eth_hdr_t*)frame)->dst_addr,0);
		if(tmp == NULL) {
		    error = 1;
		}
		else 
		{
		    if(tmp->vid > 0)
		    {
			    memcpy(((eth_hdr_t*)frame)->src_addr, mac, MAC_LEN);/*SMAC*/
			    ((eth_hdr_t*)frame)->protocol = htons((uint16)ETH_P_8021Q);
			    *(uint16*)(frame + ETH_URCP_OFFSET) = htons((uint16)((tmp->vid & 0xfff))|0x2000);/*�޸����ݰ�vid��Ϣ*/
			    h2_send_frame(frame, len, tmp->fromL3);
		    }else{
			    utagged = 1;
		    }
		}
	}

	if( (error == 0) && (utagged == 1) )
	{
		memcpy(((eth_hdr_t*)frame)->src_addr, mac, MAC_LEN);/*SMAC*/
		buffer = frame;
		mem_dec_cpy((buffer + 2*MAC_LEN +sizeof(uint32) - 1), (buffer+2*MAC_LEN-1), (2*MAC_LEN));
		buffer = buffer + sizeof(uint32);
		len = len - sizeof(uint32);
		if(gSocketL2>=0){
		    h2_send_frame(buffer, len, 0);
		}
		if(gSocketL3>=0){
		    h2_send_frame(buffer, len, 1);
		}
	}
#else
	h2_send_frame(frame, len);
#endif

	return 0;
}
/*
 *������������
 *�ڷ����鲥��㲥��ʱ��������������յ����Ķ˿�
 ֻҪ��sourc_portΪ0����
 */
int32 urcp_send(const uint32 source_port, uchar*  frame, uint32  length
#ifdef URCP_AC
	, int32 from
#endif
	)
{
    URCP_DEBUG(("%s\n",__func__));
#if 1
#if 0
    /*�㲥���鲥*/
    if((frame[0] & 0x01u) == 1u)
    {
	if(source_port>0u)
	{
	    h2_fwd_frame(frame, length, sw_cpu_vid_get(), source_port);
	}
	else
	{
	    h2_send_frame(frame, length);
	}
    }
    else
    {
	h2_send_by_port(source_port, frame, length, 0);
    }
#else

#ifdef URCP_AC
    db_loud(URCP_MODULE_ID_URCP, "urcp send to:%d\n", from);
    h2_send_frame(frame, length, from);
#else
    h2_send_frame(frame, length);
#endif

#endif
#else
    uint32 poag_no;
    port_bit_mask_t port_mask, pm;
    BOOL member[PORT_ARRAY_SIZE];
    /*�㲥���鲥*/
    if((frame[0] & 0x01u) == 1u)
    {
        port_mask = ALL_PORT_MASK;
	/*�Ƿ����Դ�˿�*/
	if(source_port>0u)
	{
	    /*����Դ�˿�*/
	    poag_no = vtss_aggr_port_member_get(source_port);
	    vtss_aggr_port_members_get(poag_no, member);
	    /*������Ա�˿�*/
	    pm = bit_array_to_int(&member[1], PORT_COUNT);
	    port_mask &= ~pm;
	    pm = get_pvlan_members(source_port);
	    port_mask &= pm;
	}
	send_broadcast(frame, length, port_mask);
    }
    else/*������*/
    {
	vtss_cpu_tx_poag_frame(vtss_aggr_port_member_get(source_port), cfg_read_sys_cpu_vid(), frame, length);
    }
#endif
    return 0;
}
/*
 *�������Ͱ���ͷ��
 *KeHautao
 *����ֵ 1 ��tag
 *		 0 ����tag
 */
int urcp_build_reply_header(urcp_infor_t* urcp_infor, uchar buffer[URCP_BUF_SIZE])
{
    char *name;
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    urcp_hdr_t* raw, *tmp;
	int ret = 0;

    URCP_DEBUG(("%s\n", __func__));
    memset(mac, 0, sizeof(mac));
    name = getLanIfName();
    get_mac(name, mac);
    get_ip(name, &ipaddr);
    /*���ͻ���*/
    raw = urcp_infor->urcp_header;
    /*��̫��֡�ײ�*/
    memcpy(((eth_hdr_t*)buffer)->dst_addr, urcp_infor->mac, MAC_LEN);/*DMAC*/
    memcpy(((eth_hdr_t*)buffer)->src_addr, mac, MAC_LEN);/*SMAC*/
	/*�Ƿ��TAG*/
	if(urcp_infor->vid >= 1u)
	{
		tmp = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);
	    memset(tmp, 0, sizeof(urcp_hdr_t));
		((eth_hdr_t*)buffer)->protocol = htons(ETH_P_8021Q);
		*(uint16*)(&buffer[ETH_URCP_OFFSET]) = htons((uint16)((urcp_infor->vid & 0xfff))|0x2000);
		*(uint16*)(&buffer[ETH_URCP_OFFSET + sizeof(uint16)]) = htons((uint16)ETH_P_URCP);
		ret = 1;

	}else{
		tmp = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);
		memset(tmp, 0, sizeof(urcp_hdr_t));
		((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_URCP);
		ret = 0;
	}
    tmp -> version = (uint16)raw -> version;
    /*����ip*/
    tmp -> ip = ipaddr;
    /*auth key*/
    tmp -> auth_key = raw -> auth_key;
    /*Զ���豸����*/
    tmp -> request_id = raw -> request_id;
    /*��������*/
    tmp -> next_header = 0;
	return ret;
}


/*
 *�������������Ч��
 *KeHuatao
 */
static uint32 urcp_header_parse(system_frame_header_t* header, urcp_infor_t* urcp_infor)
{
    urcp_hdr_t* pkt; 
    char name[64];
    uchar mac[MAC_LEN];

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_mac(name, mac);
    /*ָ�����ײ�*/
//    pkt = (urcp_hdr_t*)(&frame[ETH_URCP_OFFSET]);
    pkt = (urcp_hdr_t*)(header->data);
    /*�жϰ�����*/
    if (mac_cmp(header->raw, mac_addr_cooperate) == 0)
    {
	urcp_infor->packet_type = PACKET_MULTICAST; /*multicast*/
    }
    else if (mac_cmp(header->raw, mac) == 0)
    {
	urcp_infor->packet_type = PACKET_HOST; /*���͸�����*/
    }
    else 
    {
	/*forward*/
#if 0
	packet_type = PACKET_OTHERHOST;
#endif
	URCP_DEBUG(("%s: DMAC error\n", __func__));
	return URCP_FATAL_ERROR;
    }
    /*version >= 2*/
    if ( (ntohs(pkt->version)&VERSION_MASK) < URCP_VERSION)
    {
	URCP_DEBUG(("%s: version error\n", __func__));
	return URCP_FATAL_ERROR;
    }

    if(pkt->cmd_no==0)
    {
	URCP_DEBUG(("%s: cmd no error\n", __func__));
	return URCP_FATAL_ERROR;
    }

#ifdef URCP_AP
    if((ap_infor.joined == 1) && (mac_cmp(&(header->raw[MAC_LEN]), ap_infor.ac_infor.mac) == 0)) {
	urcp_infor->certified = 1;/*����֤AC����*/
    }
    else {
#endif
    /*�ж�flag �������Ƿ���Ҫ������֤*/
    if ( (uint16)(ntohs(pkt->flag) & FLAG_PASSWD) > 0u )
    {
	    URCP_DEBUG(("%s: pkt->flag:%d FLAG_PASSWD:%d\n", __func__, ntohs(pkt->flag), FLAG_PASSWD));
	if (urcp_password((uchar*)(pkt -> password)) < 0)/*�������*/
	{
	    URCP_DEBUG(("%s: passwd error: %s\n", __func__, pkt -> password));
	    return URCP_AUTH_ERROR;
	}
	urcp_infor->certified = 1;/*����֤*/
    }
#ifdef URCP_AP
    }
#endif
    /*�ж�flag������bit*/

    return URCP_OK;
}
/*
 *�����������ʱ�����ͻظ���
 *KeHutao
 */
static void urcp_error_reply(urcp_infor_t* urcp_infor)
{
    char name[64];
    uchar tmp[1], mac[MAC_LEN];
    uchar *ptr=tmp;
    uint32 ipaddr=0;
    urcp_hdr_t* pkt;

    URCP_DEBUG(("%s\n", __func__));
    if(urcp_infor->error_state==URCP_AUTH_ERROR)
    {
	memset(name, 0, sizeof(name));
	memset(mac, 0, sizeof(mac));
	get_switch_if_name(name);
	get_mac(name, mac);
	get_ip(name, &ipaddr);
	/*���������ظ�*/
	ptr = urcp_infor->raw;
	/*����д���ַ*/
	memcpy(ptr, &ptr[MAC_LEN], MAC_LEN);
	memcpy(&ptr[MAC_LEN], mac, MAC_LEN);
	pkt = (urcp_hdr_t*)(&ptr[ETH_URCP_OFFSET]);
	/*��������ip*/
	pkt ->ip = ipaddr;
	switch(ntohs(urcp_infor->urcp_header->type))
	{
	    case URCP_GET: /*get cmd*/
		pkt -> type = htons((uint16)URCP_GET_REPLY);
		break;
	    case URCP_SET: /*set cmd*/
		pkt -> type = htons((uint16)URCP_SET_REPLY);
		break;
	    default:
		break;
	}
	/*����״̬*/
	pkt->error_status = htons((uint16)URCP_AUTH_ERROR);
	pkt->flag = 0u; 
	/*����*/
	urcp_send(urcp_infor -> source_port, urcp_infor->raw, urcp_infor->length
#ifdef URCP_AC
		,urcp_infor->fromL3
#endif
	);
    }

    return;
}
/*
 *����������֤
 *KeHuatao
 */
static int32 urcp_password(uchar* password)
{
    int32 ret=-2, i, max=0, min=0;
    AdminConfProfile *prof=NULL;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++)
    {
	/*��ȡ����*/
	prof = (AdminConfProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL)
	{
	    /*free or not*/
	    if(ProfInstIsFree(((Pointer*)prof)) == 0)
	    {
		if (strcmp(prof->head.name, "admin") == 0)
		{
		    if (strncmp(prof->password, password, MAX_PASSWORD_LEN) != 0)
		    {
			/*correct*/
			ret = -1;
		    }
		    else
		    {
			ret = 0;
		    }
		    break;
		}
	    }
	}
    }

    if((ret==-2) && (*password=='\0'))
    {
	ret = 0;
    }
    /*error*/
    return ret;
}

/*
 *
 *�����������ݰ�
 *
 */
void urcp_process(system_frame_header_t* header)
{
    urcp_infor_t urcp_infor;
    urcp_hdr_t* urcp_packet;

    memset(&urcp_infor, 0, sizeof(urcp_infor_t));
    urcp_infor.raw = header -> raw;
    urcp_infor.vid = header -> tag.vid;
    urcp_infor.source_port = header -> source_port_no;
    urcp_infor.length = header -> length;
    /*ָ����������ͷ��*/
    urcp_packet = (urcp_hdr_t*)(header->data);
    urcp_infor.urcp_header = urcp_packet;
    urcp_infor.fromL3 = header->fromL3;

    URCP_DEBUG(("%s\n", __func__));
    /*������Ч��*/
    urcp_infor.error_state = urcp_header_parse(header, &urcp_infor); 
    if (urcp_infor.error_state != URCP_OK)
    {
	/*�ظ�������Ϣ*/
		DBCK();
	urcp_error_reply(&urcp_infor);
	return;
    }

    /*����next header*/
    urcp_next_header(urcp_packet, &urcp_infor);

    /*����ͷ����Ϣ*/
    memcpy(urcp_infor.mac, &(header->raw[MAC_LEN]), MAC_LEN);
    /*����ָ��*/
    urcp_infor.cmd_buff = &(header->data[sizeof(urcp_hdr_t)]);
    urcp_infor.length = header -> length - ETH_URCP_OFFSET - sizeof(urcp_hdr_t);

#ifdef URCP_AP
    reset_ac_dead_timer(urcp_infor.mac, urcp_infor.fromL3);
#endif
    /*�ж��������ݰ�����*/
    switch (ntohs(urcp_packet->type))
    {
	case URCP_TFTP: /*trap*/
	    urcp_tftp_process(&urcp_infor);
	    break;
	case URCP_GET: /*get cmd*/
	    urcp_get(&urcp_infor);
	    break;
	case URCP_SET: /*set cmd*/
	    urcp_set(&urcp_infor);
	    break;
#if 0
#ifdef URCP_WEB
	case URCP_TOPOY: /*message*/
	    topoy_process(&urcp_infor);
	    break;
#endif
#endif
	case URCP_GET_REPLY: /*get cmd reply*/
	    urcp_get_reply(&urcp_infor);
	    break;
	case URCP_SET_REPLY: /*set cmd reply*/
	    urcp_set_reply(&urcp_infor);
	    break;
	case URCP_TRAP: /*trap*/
	    urcp_trap(&urcp_infor);
	    break;
	default: /*unknow*/
	    urcp_unknow(&urcp_infor);
	    break;
    }

    return;
}

void urcp_get(urcp_infor_t* urcp_info)
{
    uchar *app_data, buffer[URCP_BUF_SIZE], *tmp; /*���ͻ���*/
    uint16 error=0u, ret = 0u;
    int32  i, len=0, drop=0, unknow=0, support, cmd_reply=0;/*ͳ�ƴ����������*/
    uint32 rev_len=0u, cmd_len, cmd_type;
    cmd_hdr_t *cmd;
    urcp_hdr_t *pkt;
	int iftag = 0;

    URCP_DEBUG(("%s\n", __func__));
    memset(buffer, 0, sizeof(buffer));
	if(urcp_info->vid >= 1u)
	{
		app_data = &buffer[ETH_URCP_OFFSET + +sizeof(uint32) + sizeof(urcp_hdr_t)];/*ָ��ظ����ݵ�ĩβ*/
	}else{
		app_data = &buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)];/*ָ��ظ����ݵ�ĩβ*/
	}
    cmd = (cmd_hdr_t*)(urcp_info -> cmd_buff);
    cmd_len = urcp_info->length;
    /*��������*/
#ifdef URCP_AC
    //frame_dump(urcp_info->raw, urcp_info->length);
#endif
	while(((cmd_len-rev_len) >= sizeof(cmd_hdr_t)) && (drop==0))
    {
	cmd = (cmd_hdr_t*)((uchar*)(urcp_info->cmd_buff) + rev_len);
	cmd_type = ntohl(cmd->type);
	support = 0u;
	if((cmd_type>CMD_START) && (cmd_type<CMD_END))
	{
	    i = 0;
	    while(cmd_tb[i].cmd_id!=CMD_END)
	    {
		if(cmd_type == cmd_tb[i].cmd_id)
		{
		    db_loud(URCP_MODULE_ID_URCP, "%s: cmd:%d\n", __func__, cmd_type);
		    if(cmd_tb[i].func_get!=NULL)
		    {
			support = 1u;
			if((cmd_tb[i].need_auth==1) && (urcp_info->certified==0)) {
			    /*�������*/
			    if(error<URCP_AUTH_ERROR) {
				error = URCP_AUTH_ERROR;
			    }
			}
			else {
			    tmp = app_data;
			    ret = cmd_tb[i].func_get(cmd, &app_data, urcp_info);
			    if(ret == URCP_FATAL_ERROR) {
				drop = 1;
				URCP_DEBUG(("%s drop\n", __func__));
			    }
			    else if(error<ret) {
				error = ret;
			    }
			    if(tmp!=app_data) {
				cmd_reply++;
			    }
			}
			break;
		    }
		}
		i++;
	    }
	}
	if(support == 0)
	{
	    unknow++;
	    cmd_reply++;
	    cmd_unknow(cmd->type, &app_data);
	    URCP_DEBUG(("%s undefined cmd: %d\n", __func__, cmd_type));
	}
	if(ntohs(cmd->length)>=sizeof(cmd_hdr_t)) {
	    rev_len += ntohs(cmd->length);
	}
	else {
	    URCP_DEBUG(("%s error: 1\n", __func__));
	    break;
	}
    }

#if 0
    /*Ӳ�������Զ�ת���鲥��������Ҫ���ת��*/
    if(urcp_info->packet_type == PACKET_MULTICAST)
    {
	/*foward the PACKET_MULTICAST packet*/
	urcp_send(urcp_info->source_port, urcp_info->raw, urcp_info->length);
    }

    if((ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + rev_len)<64)
    {
	rev_len = 64 - ETH_URCP_OFFSET - sizeof(urcp_hdr_t);
    }
#endif
    if((rev_len<=cmd_len) && (drop == 0) && (unknow<cmd_reply))
    {
	len = app_data - buffer;
	if((len>(ETH_URCP_OFFSET+sizeof(urcp_hdr_t))) && (len<=sizeof(buffer)))
	{
	    URCP_DEBUG(("%s: send reply\n", __func__));
	    /*������ͷ��*/
	    iftag = urcp_build_reply_header(urcp_info, &buffer[0]);
		if(iftag == 1)
		{
			pkt = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]); /*ת����ʽ*/ 
		}else{
			pkt = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]); /*ת����ʽ*/ 
		}
	    /*����������֤*/
	    pkt -> flag = 0u;
	    pkt -> type = htons((uint16)URCP_GET_REPLY);
	    pkt -> error_status = htons(error);
	    pkt -> cmd_no = cmd_reply;
	    /*��������*/
	    urcp_send(urcp_info -> source_port, buffer, len
#ifdef URCP_AC
		    , urcp_info->fromL3
#endif
		    );
	}
    }
#if DEBUG_URCP
    else
    {
	URCP_DEBUG(("%s: not reply, because: ", __func__));
	if(rev_len!=cmd_len)
	{
	    URCP_DEBUG(("    rev_len=%d, but cmd_len=%d\n", rev_len, cmd_len));
	}
        if(unknow>=cmd_reply)
	{
	    URCP_DEBUG(("    unknow=%d, cmd_reply=%d\n", unknow, cmd_reply));
	}
	if(drop!=0)
	    URCP_DEBUG(("    drop=%d\n", drop));

    }
#endif
}
/*
 * get_reply���Ͱ�����
 * ��ȡ������Ϣ
 *
 */
static void urcp_get_reply(urcp_infor_t* urcp_info)
{
    uint16 status;
    uint32 cmd_type, rev_len=0u, cmd_len;
    int32 i;
    urcp_hdr_t* urcp_raw;
    cmd_hdr_t* cmd;
    urcp_dev_t* dev=NULL;
    urcp_status_t stat; 

    URCP_DEBUG(("%s\n", __func__));
    if(check_request_id(urcp_info)<0)
    {
	cmd = (cmd_hdr_t*)((uchar*)(urcp_info->cmd_buff));
	cmd_type = ntohl(cmd->type);
	if((cmd_type != CMD_DISCOVER) && (cmd_type != CMD_CURRENT_CLIENTS))
	    return ;
    }
    
    urcp_raw = urcp_info -> urcp_header;
    cmd = (cmd_hdr_t*)(urcp_info -> cmd_buff);
    cmd_len = urcp_info->length;
#ifdef URCP_AC
    //frame_dump(urcp_info->raw, urcp_info->length);
#endif
    /*���η���������*/
#ifndef URCP_AP
    dev = urcp_dev_table_lookup(urcp_info->mac, ntohl(urcp_info->urcp_header->ip));
#if DEBUG_URCP
    if(dev!=NULL)
    {
//	dev->aged = 0;
    }
    else
    {
	URCP_DEBUG(("%s: dev=NULL\n", __func__));
    }
#endif
#endif
    while(((cmd_len-rev_len) >= sizeof(cmd_hdr_t)))
    {
	cmd = (cmd_hdr_t*)((uchar*)(urcp_info->cmd_buff) + rev_len);
	cmd_type = ntohl(cmd->type);
	if((cmd_type>CMD_START) && (cmd_type<CMD_END))
	{
	    i = 0;
	    while(cmd_tb[i].cmd_id!=CMD_END)
	    {
		if(cmd_type == cmd_tb[i].cmd_id)
		{
		    db_loud(URCP_MODULE_ID_URCP, "%s: cmd:%d\n", __func__, cmd_type);
		    if(cmd->cmd_status==htons(URCP_OK))
		    {
		    URCP_DEBUG(("%s cmd: %d ok\n", __func__, cmd_type));
			if(cmd_tb[i].func_get_reply!=NULL)
			{
			    cmd_tb[i].func_get_reply(cmd, &dev, urcp_info);
			}
		    }
#if DEBUG_URCP
		    else {
			URCP_DEBUG(("%s: cmd: %d error staus: %08x\n", __func__, cmd_type, ntohs(cmd->cmd_status)));
		    }
#endif
		}
		i++;
	    }
	}
#if DEBUG_URCP
	else{
	    URCP_DEBUG(("%s unknow cmd: %d\n", __func__, cmd_type));
	}
#endif
	if(ntohs(cmd->length)>=sizeof(cmd_hdr_t)) {
	    rev_len += ntohs(cmd->length);
	}
	else {
	    break;
	}
    }

    status = ntohs(urcp_raw->error_status);
    /*�յ����Ƿ���Ч*/
    if (status==URCP_OK)
    {
	/*���»Ự״̬*/
	stat = URCP_STATUS_OK;
	URCP_DEBUG(("%s urcp session: %d success\n", __func__, urcp_info->session_id));
    }
    else if(status == URCP_AUTH_ERROR)
    {
	stat = URCP_STATUS_AUTH_ERROR;
	URCP_DEBUG(("%s urcp session: %d passwd err\n", __func__, urcp_info->session_id));
    }
    else
    {
	stat = URCP_STATUS_FAIL;
	URCP_DEBUG(("%s urcp session: %d failed\n", __func__, urcp_info->session_id));
    }
    session_set(urcp_info, stat);
}

/*
 *set ���Ͱ�����
 *KeHuatao
 */
static void urcp_set(urcp_infor_t* urcp_info)
{
    uchar *app_data, *tmp, buffer[URCP_BUF_SIZE]; /*���ͻ���*/
    uint16 error=0;
    int32  i, num=0, len=0, unknow=0, support, cmd_reply=0;/*ͳ�ƴ����������*/
    uint32 rev_len=0u, cmd_len, cmd_type;
    cmd_hdr_t *cmd;
    urcp_hdr_t *pkt;
    action_to_do_t action_to_do[MAX_ACTIONS];
	int iftag = 0;

    URCP_DEBUG(("%s\n", __func__));
#ifdef URCP_AP
    if((ap_infor.joined == 1) && (mac_cmp(ap_infor.ac_infor.mac, urcp_info->mac) != 0))
    {
	/*ֻ�ܱ���ǰAC����*/
	return;
    }
#endif
    memset(buffer, 0, sizeof(buffer));
    memset(action_to_do, 0, sizeof(action_to_do));
	if (urcp_info->vid >= 1u)
	{
		app_data = &buffer[ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t)];/*ָ��ظ����ݵ�ĩβ*/
	}else{
		app_data = &buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)];/*ָ��ظ����ݵ�ĩβ*/
	}
    cmd = (cmd_hdr_t*)(urcp_info -> cmd_buff);
    cmd_len = urcp_info->length;
    /*��������*/
    while(((cmd_len-rev_len) >= sizeof(cmd_hdr_t)))
    {
	cmd = (cmd_hdr_t*)((uchar*)(urcp_info->cmd_buff) + rev_len);
	cmd_type = ntohl(cmd->type);
	support = 0u;
	if((cmd_type>CMD_START) && (cmd_type<CMD_END))
	{
	    i = 0;
	    while(cmd_tb[i].cmd_id!=CMD_END)
	    {
		if(cmd_type == cmd_tb[i].cmd_id)
		{
		    URCP_DEBUG(("%s cmd: %d\n", __func__, cmd_type));
		    if(cmd_tb[i].func_set!=NULL)
		    {
			support = 1u;
			if((cmd_tb[i].need_auth==1) && (urcp_info->certified==0)) {
		    URCP_DEBUG(("%s auth:%d certified:%d\n", __func__, cmd_tb[i].need_auth, urcp_info->certified));
			    /*�������*/
			    if(error<URCP_AUTH_ERROR) {
				error = URCP_AUTH_ERROR;
			    }
			}
			else {
			    tmp = app_data;
			    if(cmd_tb[i].func_set(cmd, &app_data)==0)
			    {
				if((cmd_tb[i].func_aux!=NULL ) && (num<(MAX_ACTIONS - 1))){
				    action_to_do[num++].fp_delay = cmd_tb[i].func_aux;/*��¼�ظ�����ִ�е�����*/
				}
			    }
			    if(tmp!=app_data) {
				cmd_reply++;
			    }
			}
			break;
		    }
		}
		i++;
	    }
	}
	if(support == 0)
	{
	    unknow++;
	    cmd_reply++;
	    cmd_unknow(cmd->type, &app_data);
	    URCP_DEBUG(("%s undefined cmd: %d\n", __func__, cmd_type));
	}
	if(ntohs(cmd->length)>=sizeof(cmd_hdr_t)) {
	    rev_len += ntohs(cmd->length);
	}
	else {
	    break;
	}
    }
#if 0
    if(urcp_info->packet_type == PACKET_MULTICAST)
    {
	/*Ӳ�������Զ�ת���鲥��������Ҫ���ת��*/
	/*foward the PACKET_MULTICAST packet*/
	urcp_send(urcp_info->source_port, urcp_info->raw, urcp_info->length);
    }
#endif

    if((rev_len<=cmd_len) && (unknow<cmd_reply))
    {
	len = app_data - buffer;
	if((len>(ETH_URCP_OFFSET+sizeof(urcp_hdr_t))) && (len<=sizeof(buffer)))
	{
	    /*������ͷ��*/
	    iftag = urcp_build_reply_header(urcp_info, &buffer[0]);
		if(iftag == 1)
		{
			pkt = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]); /*ת����ʽ*/

		}else{
			pkt = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]); /*ת����ʽ*/
		}
	    /*����������֤*/
	    pkt -> flag = 0u;
	    pkt -> type = htons((uint16)URCP_SET_REPLY);
	    pkt -> error_status = htons(error);
	    URCP_DEBUG(("%s status: %d\n", __func__, error));
	    pkt -> cmd_no = cmd_reply;
	    /*��������*/
	    urcp_send(urcp_info -> source_port, buffer, len
#ifdef URCP_AC
		    , urcp_info->fromL3
#endif
		    );
	}
    }
#if DEBUG_URCP
    else
    {
	printf("%s: error 4\n", __func__);
    }
#endif
    for(i=0;i<num;i++)
    {
        if(action_to_do[i].fp_delay!=NULL)
	{
	    action_to_do[i].fp_delay();
	}
    }
}


/*
 *
 *set_reply���Ͱ�����
 *KeHuatao
 */
static void urcp_set_reply(urcp_infor_t* urcp_infor)
{
    uint16 state;
    urcp_hdr_t* urcp_raw;
    urcp_status_t stat; 

    URCP_DEBUG(("%s\n", __func__));
    /*���request id*/
    if(check_request_id(urcp_infor)<0)
    {
	return;
    }
    urcp_raw = urcp_infor -> urcp_header;
    state = ntohs(urcp_raw->error_status);
    if(state == URCP_OK)
    {
        /*���»Ự״̬*/
	stat = URCP_STATUS_OK;
        URCP_DEBUG(("%s urcp session: %d success\n", __func__, urcp_infor->session_id));
    }
    else if(state == URCP_AUTH_ERROR)
    {
	/*�������*/
	stat = URCP_STATUS_AUTH_ERROR;
        URCP_DEBUG(("%s urcp set passwd error\n", __func__));
    }
    else
    {
	/*��������*/
	stat = URCP_STATUS_FAIL;
        URCP_DEBUG(("%s urcp set failed\n", __func__));
    }
    /*���»Ự״̬*/
    session_set(urcp_infor, stat);

    return;
}

/*
 *
 *trap ���Ͱ�����
 *
 *KeHuatao
 */
static void urcp_trap(urcp_infor_t* urcp_info)
{
	int32  i, support;/*ͳ�ƴ����������*/
	uint32 rev_len=0u, cmd_len, cmd_type;
	cmd_hdr_t *cmd;
    urcp_dev_t* dev=NULL;

	cmd = (cmd_hdr_t*)(urcp_info -> cmd_buff);
	cmd_len = urcp_info->length;

#ifndef URCP_AP
    dev = urcp_dev_table_lookup(urcp_info->mac, ntohl(urcp_info->urcp_header->ip));
    if( dev != NULL ){
//		dev->aged = 0;
    } else {
		DBCK();
	}
#endif
	/*��������*/
	while (((cmd_len-rev_len) >= sizeof(cmd_hdr_t))) {
		cmd = (cmd_hdr_t*)((uchar*)(urcp_info->cmd_buff) + rev_len);
		cmd_type = ntohl(cmd->type);
		support = 0u;

		if ((cmd_type > TRAP_START) && (cmd_type < TRAP_END)) {
			i = 0;
			while (trap_cmd_tb[i].cmd_id != CMD_END) {
				if (cmd_type == trap_cmd_tb[i].cmd_id) {
		    db_loud(URCP_MODULE_ID_URCP, "%s: cmd:%d\n", __func__, cmd_type);
					if (trap_cmd_tb[i].func_set != NULL ) {
						trap_cmd_tb[i].func_set(cmd, &dev, urcp_info); /* trap������ */
					}

					if (trap_cmd_tb[i].syslog != NULL) {
						trap_cmd_tb[i].syslog(cmd, &dev, urcp_info);
					}

					break;
				}
				i++;
			}
		}

		if(ntohs(cmd->length)>=sizeof(cmd_hdr_t)) {
			rev_len += ntohs(cmd->length);
		}
		else {
			break;
		}
	}

	return;
}

/*
 *�����ʹ���
 *Ӧ�ö���
 *KeHuatao
 */
void urcp_unknow(urcp_infor_t* urcp_infor)
{
#if 0
#ifdef CONFIG_VSC7390
    /*7390ͨ�����üĴ���0xf0ʵ�����������Զ�ת��*/
    if(urcp_infor->packet_type == PACKET_MULTICAST) /*multicast*/
    {
        urcp_send(urcp_infor->source_port, urcp_infor->raw, urcp_infor->length);
    }
#endif
#endif
    URCP_DEBUG(("packet type unknow\n"));
}

/*
 *������ʼ��``
 *
 */
void urcp_init(void)
{
#ifdef URCP_WEB
    shm_init();
    /*���������豸�����*/
    dev_table_init();
#endif
    /*״̬�ṹ���*/
    memset(&urcp_state, 0, sizeof(urcp_state));

#if (AP_LOG_MANAGE == FYES)
	extern urcp_shm_t* shm_ptr;

    memset(&(shm_ptr->ap_msg), 0, sizeof(ap_log));
#endif

    return;
}

/*
 *�ж�urcp�����Ƿ�ʱ 
 *
 */
void urcp_timer(void)
{
    uint32 i;

    /*���β�ѯ��������״̬*/
    for(i=0u;i<MAX_URCP_SESSION;i++)
    {
	/*��ʱ*/
	if(urcp_state.session[i].status == URCP_STATUS_WAIT)
	{
	   if (urcp_state.session[i].timer <= 0)
	   {
		URCP_DEBUG(("urcp age session:%d\n", i));
		URCP_FDEBUG("urcp age session:%d\n", i);
		//printf("urcp age session:%d\n", i);
		urcp_state.session[i].status = URCP_STATUS_TIME_OUT;/*��ʱ*/
		if(urcp_state.session[i].cb_func != NULL)/*���˷���*/
	    	{
	    	    urcp_state.session[i].cb_func(urcp_state.session[i].req_id, urcp_state.session[i].session_type, urcp_state.session[i].status);/*�ص�����*/
	    	}
	    }
	   else
	   {
	       urcp_state.session[i].timer = urcp_state.session[i].timer - 1;/*ʱ�䵹��ʱ*/
	   }
	}
    }
}

/*
 *����ĳ���Ự��״̬
 *
 */
void session_set(urcp_infor_t* urcp_infor, urcp_status_t state)
{
    uint32 id;
    uint16  req;

    id = urcp_infor->session_id;
    /*SESSION_TYPE_HELLO���ͻỰֻ�ܵȴ���ʱ,û������״̬*/
    if( urcp_state.session[id].session_type != SESSION_TYPE_HELLO)
    {
	/*�ı�״̬*/
	urcp_state.session[id].status = state;
	/*ʧ��ʱ���ûص�����*/
	if(urcp_state.session[id].cb_func!=NULL)
	{
	    req = ntohs(urcp_infor->urcp_header->request_id);
	    urcp_state.session[id].cb_func(req, urcp_state.session[id].session_type, state);
	    urcp_state.session[id].cb_func = NULL;
	}
    }

    return;
}
/*
 * ÿ�����������������һ���Ựid
 * ����һ�����еĻỰid
 *
 */
int32 session_alloc(session_type_t type, uint32 flag, int32 time_out, uint16 *request_id, void (*cb)(uint16 reqid, session_type_t type, urcp_status_t state))
{
    uint32 id, i=0u;
    int32 time = 0x7FFFFFFF;

    /*��ѯ���лỰ*/
    for(id=0u;id<MAX_URCP_SESSION;id++)
    {

	/*�Ƿ�ֻ����һ��ͬ���͵ĻỰ*/
	if( ((flag & SESSION_SINGLE)>0u)
		&& (urcp_state.session[id].session_type == type)/*�����ͻỰ�Ѿ�������*/
	  )
	{
	    if(urcp_state.session[i].status == URCP_STATUS_WAIT)/*��δ��ʱ*/
	    {
		/*���ٷ���Ựid��*/
		URCP_ERROR(("The session already exists\n"));
		return -1;
	    }
	}
	if(urcp_state.session[id].status != URCP_STATUS_WAIT)/*��ʹ��*/
	{
	    i = id;
	    if((flag & SESSION_SINGLE)==0u) /*������ͬ��Ự*/
	    {
		/*�˳�*/
		break;
	    }
	}
	else if (urcp_state.session[id].timer<time)/*ѡ�����ϵ�һ��id*/
	{
	    time = urcp_state.session[i].timer;
	    i = id;/*��¼�Ựid*/
	}
	else
	{
	    ;
	}
    }

    /*�ҵ����ʵ�id*/
    urcp_state.session[i].req_id = urcp_state.req_id_counter;
    urcp_state.session[i].status = URCP_STATUS_WAIT;
    urcp_state.session[i].timer = time_out;
    urcp_state.session[i].session_type = type;
    urcp_state.session[i].cb_func = cb;
    urcp_state.req_id_counter++;
    *request_id = urcp_state.session[i].req_id;
    URCP_DEBUG(("alloc session:%d, req_id:%d\n", i, urcp_state.session[i].req_id));
    URCP_FDEBUG("alloc session:%d, req_id:%d\n", i, urcp_state.session[i].req_id);
    //printf("alloc session:%d, req_id:%d\n", i, urcp_state.session[i].req_id);

    return 0;
}


/*
 *�ж�request id
 *�Ƿ�Ϊ����ǰ������
 */
int32 check_request_id(urcp_infor_t* urcp_infor)
{
    uint32 i;
    uint16 id;

    id = ntohs(urcp_infor->urcp_header->request_id);
    for(i=0u;i<MAX_URCP_SESSION;i++)/*������лỰ*/
    {
	/*id��ͬ����û�г�ʱ*/
	if((urcp_state.session[i].req_id == id)
		&& (urcp_state.session[i].status == URCP_STATUS_WAIT))
	{
	    urcp_infor->session_id = i;
	    URCP_DEBUG(("find session:%d, req_id:%d\n", i, id));
	    return 0;
	}
    }

    URCP_DEBUG(("error request_id: %d\n", id));
    /*û�з���ƥ��id*/
    return -1;
}
/*
 *�򻺳��������һ������
 *
 * */
uint16 add_cmd(cmd_hdr_t* cmd_buffer, uint32 type)
{
    cmd_buffer -> type = htonl(type);
    cmd_buffer -> cmd_status = htons(URCP_OK);
    cmd_buffer -> length = htons(sizeof(cmd_hdr_t));

    return sizeof(cmd_hdr_t);
}
#if 0

/**
 * ��ȡ���˿ڵ�mac��
 * @param mac_num ���δ洢���˿ڵ�mac��
 *  
 */
void get_port_mac_num(port_mac_number_t *mac_num)
{
    uchar port_no;
    vtss_pgid_no_t pgid_no;

    /*��0*/
    memset(mac_num, 0, sizeof(port_mac_number_t));
    /**����mac��*/
    for (idx = VTSS_MAC_ADDR_START; idx < VTSS_MAC_ADDRS; idx++)
    {
	pgid_no = 0xFFu;
	/* ��ȡ�ڵ�����*/
	ret = vtss_ll_mac_table_list(idx, &entry, &pgid_no);
	if (ret == VTSS_OK) 
	{
	    /*�˿ں��Ƿ�Ϸ�*/
	    if((pgid_no >= VTSS_PORT_NO_START) &&  (pgid_no < VTSS_PORT_NO_END))
	    {
		/*��1*/
		*(mac_num+pgid_no-1u) = *(mac_num+pgid_no-1u) + 1u; 
	    }
#if 0
	    vtss_mac_entry_destination_set(entry, pgid_no);
#endif
	}
    }
}
#endif
int urcp_buid_header(uchar buffer[URCP_BUF_SIZE], uchar dst_mac[MAC_LEN], uint16 type, uint16 id, uchar *passwd)
{
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    urcp_hdr_t* header;
#ifdef URCP_AC
    urcp_dev_t* dev=NULL;
#endif
	int ret = 0;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_ip(name, &ipaddr);
#ifdef URCP_AP
    memset(name, 0, sizeof(name));
    get_switch_eth_name(name);
#endif
    get_mac(name, mac);
    /*ָ���ͷ*/
    memcpy(((eth_hdr_t*)buffer)->dst_addr, dst_mac, MAC_LEN);/*DMAC*/
    memcpy(((eth_hdr_t*)buffer)->src_addr, mac, MAC_LEN);/*SMAC*/
#ifdef URCP_AC
	dev = urcp_dev_table_lookup(dst_mac,0);
    if((dev!=NULL)&&(dev->vid >= 1u))
    {
		header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);
	    memset(header, 0, sizeof(urcp_hdr_t));
		((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_8021Q);
		*(uint16*)(&buffer[ETH_URCP_OFFSET]) = htons((uint16)((dev->vid & 0xfff))|0x2000);
		*(uint16*)(&buffer[ETH_URCP_OFFSET + sizeof(uint16)]) = htons((uint16)ETH_P_URCP);
		ret = 1;
	}else{
		header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);
	    memset(header, 0, sizeof(urcp_hdr_t));
		((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_URCP);
		ret = 0;
	}
#else
	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);
    memset(header, 0, sizeof(urcp_hdr_t));
	((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_URCP);
#endif
    /*����*/
    if(passwd!=NULL)
    {
		URCP_DEBUG(("%s passwd:%s\n", __func__, passwd));
		strncpy(header->password, passwd, MAX_PASSWORD_LEN);
		header ->flag = htons(FLAG_PASSWD);
	    URCP_DEBUG(("header->flag: %d\n", header ->flag));
    }

    /*��������*/
    header -> version = htons((uint16)URCP_VERSION);
    header -> type = htons(type);
#if ((UTT_AC_300 == FYES)||(UTT_AP_801 == FYES))
    header -> auth_key = htons((uint16)URCP_AUTH_KEY_FRL);
#else
    header -> auth_key = htons((uint16)URCP_AUTH_KEY);
#endif
    header -> ip = ipaddr;
    header -> request_id =  htons(id);
    header -> error_status =  0u;
    header -> cmd_no = 1u; 
    header -> next_header = 0u; 

    return ret;
}
int urcp_buid_header_with_8021q(uchar buffer[URCP_BUF_SIZE], uchar dst_mac[MAC_LEN], uint16 type, uint16 id, uchar *passwd)
{
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    urcp_hdr_t* header;
	int ret = 0;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_ip(name, &ipaddr);
#ifdef URCP_AP
    memset(name, 0, sizeof(name));
    get_switch_eth_name(name);
#endif
    get_mac(name, mac);
    /*ָ���ͷ*/
    memcpy(((eth_hdr_t*)buffer)->dst_addr, dst_mac, MAC_LEN);/*DMAC*/
    memcpy(((eth_hdr_t*)buffer)->src_addr, mac, MAC_LEN);/*SMAC*/

	header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);
    memset(header, 0, sizeof(urcp_hdr_t));
	((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_8021Q);
//	*(uint16*)(&buffer[ETH_URCP_OFFSET]) = htons((uint16)((dev->vid & 0xfff))|0x2000);
	*(uint16*)(&buffer[ETH_URCP_OFFSET + sizeof(uint16)]) = htons((uint16)ETH_P_URCP);
    /*����*/
    if(passwd!=NULL)
    {
		URCP_DEBUG(("%s passwd:%s\n", __func__, passwd));
		strncpy(header->password, passwd, MAX_PASSWORD_LEN);
		header ->flag = htons(FLAG_PASSWD);
	    URCP_DEBUG(("header->flag: %d\n", header ->flag));
    }

    /*��������*/
    header -> version = htons((uint16)URCP_VERSION);
    header -> type = htons(type);
#if ((UTT_AC_300 == FYES)||(UTT_AP_801 == FYES))
    header -> auth_key = htons((uint16)URCP_AUTH_KEY_FRL);
#else
    header -> auth_key = htons((uint16)URCP_AUTH_KEY);
#endif
    header -> ip = ipaddr;
    header -> request_id =  htons(id);
    header -> error_status =  0u;
    header -> cmd_no = 1u; 
    header -> next_header = 0u; 

    return ret;
}
void urcp_buid_tftp_header(uchar buffer[URCP_BUF_SIZE], uchar dst_mac[MAC_LEN], uint16 type, uint16 id, uchar *passwd,uint16 vid)
{
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
    char name[64];
    uchar mac[MAC_LEN];
    uint32 ipaddr=0;
    urcp_hdr_t* header;

    memset(name, 0, sizeof(name));
    memset(mac, 0, sizeof(mac));
    get_switch_if_name(name);
    get_ip(name, &ipaddr);
#ifdef URCP_AP
    memset(name, 0, sizeof(name));
    get_switch_eth_name(name);
#endif
    get_mac(name, mac);
    /*ָ���ͷ*/

    memcpy(((eth_hdr_t*)buffer)->dst_addr, dst_mac, MAC_LEN);/*DMAC*/
    memcpy(((eth_hdr_t*)buffer)->src_addr, mac, MAC_LEN);/*SMAC*/
    if(vid >= (uint16)1u)
    {
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
		header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);
	    memset(header, 0, sizeof(urcp_hdr_t));
		((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_8021Q);
		*(uint16*)(&buffer[ETH_URCP_OFFSET]) = htons((uint16)((vid & 0xfff))|0x2000);
		*(uint16*)(&buffer[ETH_URCP_OFFSET + sizeof(uint16)]) = htons((uint16)ETH_P_URCP);
	}else{
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
		header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);
	    memset(header, 0, sizeof(urcp_hdr_t));
		((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_URCP);
	}
    /*����*/
    if(passwd!=NULL)
    {
		URCP_DEBUG(("%s passwd:%s\n", __func__, passwd));
		strncpy(header->password, passwd, MAX_PASSWORD_LEN);
		header ->flag = htons(FLAG_PASSWD);
	    URCP_DEBUG(("header->flag: %d\n", header ->flag));
    }
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));

    /*��������*/
    header -> version = htons((uint16)URCP_VERSION);
    header -> type = htons(type);
#if ((UTT_AC_300 == FYES)||(UTT_AP_801 == FYES))
    header -> auth_key = htons((uint16)URCP_AUTH_KEY_FRL);
#else
    header -> auth_key = htons((uint16)URCP_AUTH_KEY);
#endif
    header -> ip = ipaddr;
    header -> request_id =  htons(id);
    header -> error_status =  0u;
    header -> cmd_no = 1u;
    header -> next_header = 0u;
}

