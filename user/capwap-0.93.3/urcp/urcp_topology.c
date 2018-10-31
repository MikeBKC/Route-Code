/*
 *
 *���˷��ֹ��ܴ���
 *
 *KeHuatao
 *2010-09-13
 *
 */


#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> /*inet_addr*/
#include <unistd.h>
#include <switch.h>
#include <sw_ioctl.h>
#include <sw_cfg.h>
#include "common.h"
#include "urcp.h"
#include "app_msg.h"
#include "urcp_hello.h"
#include "urcp_topology.h"

#define  INDEX_START   1

#define SM_TOPOY_TB    (shm_ptr->topoy_table)
#define SM_TOPOY_LIST  (shm_ptr->topoy_list)

static int32 free_index = INDEX_START;
static urcp_shm_t* shm_ptr = NULL;

static topoy_t* topoy_root;
static topoy_t* wait_list = NULL;
static topoy_state_t topoy_state;


static int32 topoy_header_parse(urcp_infor_t* infor, topoy_hdr_t* topoy);
static void topoy_forward(uint32 source_port, uchar* frame, uint32 len);
#if 0
#ifdef CONFIG_VSC7390
static void topoy_hello(urcp_infor_t* infor);
#endif
#endif
static void topoy_reply(urcp_infor_t* infor);
static int32 topoy_dev_add(topoy_t* dev);
static void topoy_add_child(topoy_t* parent, topoy_t* child);
static topoy_t* topoy_search(topoy_t* root, topoy_t* dev);
static void  topoy_tb_parse(topoy_t* dev);
#if DEBUG_URCP
static void topoy_tb_dump(void);
#endif

/*
 *��ȡһ�����еĽڵ�          
 *
 **/    
static topoy_t* get_free_node(void)
{
    topoy_t* tmp;

    /*�Ƿ��пսڵ�*/
    if (free_index >= MAX_TOPOY_DEV ) 
    {
	/*������*/
	URCP_ERROR(("topoy:get free node failed\n"));
	return NULL;
    }

    URCP_DEBUG(("get node: %d\n", free_index));

    tmp = &(SM_TOPOY_TB[free_index]);
    free_index += 1;

    tmp->tb_index = free_index;/*��ʵ��������һ*/
    /*����ָ��*/
    return  tmp;
}


/*
 *
 *����ͷ����Ϣ
 *
 */
static int32 topoy_header_parse(urcp_infor_t* infor, topoy_hdr_t* topoy)
{
#if 0
#ifdef CONFIG_VSC7390
    char name[64];
    uchar mac[MAC_LEN], hwaddr[MAC_LEN];
    uint16  parent_port;
    uint32  aggr;
#endif
#endif
    uint16 max_hop, hop;

    /*�ж�hopֵ*/
    max_hop = ntohs(topoy->max_hop);
    hop = ntohs(topoy->hop);
    
    /*����Ϊ0*/
    if ( hop == 0u ) 
    {
	URCP_ERROR(("topoy hop err:%d\n", hop));
	return -1;
    }

    /*�ж�mac��ַ��Ч��*/
    isValidMac((uchar*)topoy->root_id);/*����ַ*/
    isValidMac((uchar*)topoy->parent_id);/*���ڵ��ַ*/

    if((topoy->type == TOPOY_HELLO) && (infor->packet_type != PACKET_MULTICAST))/*���˷���ֻ��Ϊ�鲥*/
    {
	URCP_ERROR(("topoy hello DMAC err:\n"));
        return -1;
    }
    else if ((topoy->type == TOPOY_REPLY) && (infor->packet_type != PACKET_HOST))/*�ظ���ֻ��Ϊ����*/
    {
	URCP_ERROR(("topoy reply DMAC err:\n"));
        return -1;
    }

    if(hop > max_hop)/*hopֵ����*/
    {
	URCP_ERROR(("topoy hop err:%d\n", hop));
	return -1;
    }
#if 0
#ifdef CONFIG_VSC7390
    else if((topoy->type == TOPOY_HELLO) && (hop < max_hop))/*ת��*/
    {
	memset(name, 0, sizeof(name));
	memset(hwaddr, 0, sizeof(hwaddr));
	get_switch_if_name(name);
	get_mac(name, hwaddr);

	memcpy(mac, topoy->parent_id, MAC_LEN); /*����ԭʼmac*/
	parent_port = topoy->parent_port_id;/*����port*/

	/*������ֵ*/
	topoy->hop = htons(hop+1u);/*����hop*/
	memcpy(topoy->parent_id, hwaddr, MAC_LEN); /*����mac��ַ*/

	aggr = topoy->parent_aggr_mask;

	/*����*/
#if 0
	memcpy(((eth_hdr_t*)(infor->raw))->src_addr, api_state->io.hwaddr, MAC_LEN);/*�޸�SMAC��������ղ�����*/
#endif
	topoy_forward(infor->source_port, infor->raw, infor->length);

	/*�ָ�ԭʼ����*/
	topoy->parent_aggr_mask = aggr;
	topoy->hop= htons(hop);
	topoy->parent_port_id = parent_port; 
	memcpy(topoy->parent_id, mac, MAC_LEN);
    }
#endif
#endif
    else
    {
	;
    }

    return 0;
}
/*
 *�������˷��ְ�
 *
 */
static void topoy_forward(uint32 source_port, uchar* frame, uint32 len)
{
#if CONFIG_SWITCH_EXT
    uint32 port, mask;
    topoy_hdr_t* topoy;
    BOOL member[PORT_ARRAY_SIZE];
    fwd_dst_ports_cond_t  cond;
    fwd_dst_ports_ret_t   ret;

    memset(&cond, 0, sizeof(fwd_dst_ports_cond_t));
    memset(&ret, 0, sizeof(fwd_dst_ports_ret_t));
    /*�㲥���鲥*/
    if((frame[0] & 0x01u) == 1u)
    {
	cond.src_port = source_port;
	cond.vid_mac.vid = sw_cpu_vid_get();
	memcpy((cond.vid_mac.mac.addr), frame, MAC_LEN);
	cond.vlan = 1;
	sw_fwd_get_dst_ports(&cond, &ret);
    }
    else
    {
	ret.dest[source_port] = 1u;
    }

    topoy = (topoy_hdr_t*)(frame + ETH_URCP_OFFSET + sizeof(urcp_hdr_t));
    /*���δӸ��˿ڷ���*/
    for (port = PORT_NO_START; port < PORT_NO_END; port++)
    {
	/*���Ǻ�Դ�˿�ͬһ�����ŷ���*/
	if(ret.dest[port]!=0u)
	{
	    sw_trunk_members_get(port, member);
#ifdef CONFIG_IP1725
	    /*7390��ioctl����ʵ����·״̬���ж�*/
	    sw_check_ports_link_status(member);
#endif
	    mask = bit_array_to_int(&member[START_INDEX], PORT_COUNT);
	    topoy->parent_port_id = htons((uint16)port);/*�˿ں�*/
	    topoy->vlan_id = htons((uint16)sw_cpu_vid_get());/*�����������ƣ��ĳɸ��˿�vid*/
	    topoy->parent_aggr_mask = htonl(mask);/*�˿ں�*/
	    URCP_DEBUG(("%s: port:%d\n", __func__, port));
	    /*send*/
	    h2_send_by_port(port, frame, len, 0);
	}
    }
    printf("\n");
#endif
}

/*
 *��ʼ�����˷���
 *
 */
void topoy_init(void)
{
    shm_ptr = get_shm_ptr();
    /*clear*/
    memset(&topoy_state, 0, sizeof(topoy_state));
    /*��ʼ������*/
    memset(SM_TOPOY_LIST, 0, sizeof(SM_TOPOY_LIST));
    memset(SM_TOPOY_TB, 0, sizeof(SM_TOPOY_TB));
    topoy_root = &SM_TOPOY_TB[0];
    wait_list = NULL;
    free_index = INDEX_START;
}

/*
 *���˷��ִ���
 *
 */
int32 topoy_process(urcp_infor_t* infor)
{
    topoy_hdr_t* topoy;

    URCP_DEBUG(("%s\n", __func__));
    topoy = (topoy_hdr_t*)(infor->cmd_buff);
    /*����ͷ����Ϣ*/
    if(topoy_header_parse(infor, topoy)<0)
    {
	return URCP_ANY_ERROR;
    }
    /*�������˱�������*/
    switch(topoy->type)
    {
#if 0
#ifdef CONFIG_VSC7390
	case TOPOY_HELLO:
	    topoy_hello(infor);
	    break;
#endif
#endif
	case TOPOY_REPLY:
	    topoy_reply(infor);
	    break;
	default: /*unknow*/
	    urcp_unknow(infor);
	    URCP_DEBUG(("topoy type unknow\n"));
	    break;
    }
    return URCP_OK;
}

#if 0
#ifdef CONFIG_VSC7390
/*
 *�յ� ���˷��ְ�
 *��root���ͻظ���
 */
static void topoy_hello(urcp_infor_t* infor)
{
    char buffer[ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(topoy_hdr_t)];/*���ͻ���*/
    char dat[65];
    uchar macaddr[MAC_LEN];
    uint16 n=0;
    uint32 val;
    topoy_hdr_t* topoy;
    urcp_hdr_t* pkt;

    URCP_DEBUG(("%s\n", __func__));
    
    memset(buffer, 0, sizeof(buffer));
    /*�����յ��İ�*/
    topoy = (topoy_hdr_t*)(buffer+ETH_URCP_OFFSET+sizeof(urcp_hdr_t));
    memcpy(topoy, infor->cmd_buff, sizeof(topoy_hdr_t));
    
    memset(dat, 0, sizeof(dat));
    get_switch_if_name(dat);
    get_mac(dat, macaddr);
    /*��������*/
    memcpy(topoy->child_id, macaddr, MAC_LEN);/*����mac��ַ*/
    topoy->child_port_id = htons((uint16)(infor->source_port));/*�հ��˿�*/
    topoy->type = TOPOY_REPLY;
    topoy->flag = 0u;
    val = get_alarm_level();
    topoy->alarm_level = htonl(val);
    val = get_aggr_members(infor->source_port);
    topoy->child_aggr_mask = htonl(val);/*������Ա*/
    topoy->dev_model = htons(DEV_MODEL);/*�豸����*/
    /*��˿���*/
    n = countOne(get_link_status_mask());
    topoy->up_ports = htons(n);
    /*�ͺ�*/
    memset(dat, 0, sizeof(dat));
    getSystemInfoMachineNo(dat);
    strncpy(topoy->dev_type, dat, DEV_TYPE_SIZE-1);
    
    /*����ͷ��*/
    memcpy(infor->mac, topoy->root_id, MAC_LEN);
    urcp_build_reply_header(infor, (uchar*)buffer);

    /*��������*/
    pkt = (urcp_hdr_t*)(buffer+ETH_URCP_OFFSET);
    pkt->flag = 0u;
    pkt->cmd_no = 1;
    pkt->type = htons((uint16)URCP_TOPOY);
    pkt->error_status = htons((uint16)URCP_OK);

    /*����������Ϣ�����豸�ͺţ�������Ϣ��*/

    /*���ͻظ���*/
#if 1
    vtss_cpu_tx_poag_frame(vtss_aggr_port_member_get(infor->source_port), sw_cpu_vid_get(), (uchar*)buffer, sizeof(buffer));
#else
    vtss_cpu_tx_raw_frame(infor->source_port, (uchar*)buffer, sizeof(buffer), 1u);
#endif
}
#endif
#endif
/*
 *
 *���˻ظ�������
 *
 */
static void topoy_reply(urcp_infor_t* infor)
{
    topoy_hdr_t* topoy;
    topoy_t* tmp;

    URCP_DEBUG(("%s\n", __func__));
    /*���request id*/
    if(check_request_id(infor)<0)
    {
	return;
    }

    /*��ȡָ�� */
    topoy = (topoy_hdr_t*)(infor->cmd_buff);
    /*�ж�hopֵ�Ƿ��ڿɽ��ܷ�Χ*/
    if(ntohs(topoy->hop)>MAX_TOPOY_HOP)
    {
	URCP_ERROR(("hop value too large\n"));
	return;
    }
    /*�Ƿ�ﵽ����豸��*/
    if(topoy_state.dev_num+1>MAX_TOPOY_DEV)
    {
	URCP_ERROR(("connot save more dev\n"));
	return;
    }
    /*�����ѽ����豸��*/
    topoy_state.dev_num = topoy_state.dev_num + 1u;

    /*�����ڴ�*/
    tmp = get_free_node();
    if(tmp == NULL)
    {
	return;
    }

    /*���ýڵ����*/
    memcpy(tmp->parent_mac, topoy->parent_id,MAC_LEN);
    memcpy(tmp->mac, topoy->child_id,MAC_LEN);
    tmp->parent_port = ntohs(topoy->parent_port_id);
    tmp->port = ntohs(topoy->child_port_id);
    tmp->ip = ntohl(infor->urcp_header->ip); 
    tmp->vlan_id = ntohs(topoy->vlan_id);
    tmp->hop = ntohs(topoy->hop);

    /*�ڵ�������Ϣ*/
    tmp->alarm_level = ntohl(topoy->alarm_level);
    tmp->parent_aggr_mask = ntohl(topoy->parent_aggr_mask);
    tmp->child_aggr_mask = ntohl(topoy->child_aggr_mask);
    tmp->dev_model = ntohs(topoy->dev_model);
    tmp->up_ports = ntohs(topoy->up_ports);
    strncpy(tmp->dev_type, topoy->dev_type, DEV_TYPE_SIZE-1);

    topoy_dev_add(tmp);
}

/*
 *���һ���ڵ㵽��������
 *
 */
static int32 topoy_dev_add(topoy_t* dev)
{   
    topoy_t* tmp, *tmp1;
    struct in_addr ip_addr;
    int32 ip;

    ip = htonl(dev->ip);
    memcpy(&ip_addr, &ip, 6);
    URCP_DEBUG(("%s:\n", __func__));
    URCP_DEBUG(("              MAC:%s\n", str_macaddr(dev->mac)));
    URCP_DEBUG(("              IP:%s\n", inet_ntoa(ip_addr)));
    URCP_DEBUG(("              parent MAC:%s\n", str_macaddr(dev->parent_mac)));
    URCP_DEBUG(("              parent port:%d\n", dev->parent_port));
    URCP_DEBUG(("              vlan id:%d\n", dev->vlan_id));
    URCP_DEBUG(("              child port:%d\n", dev->port));
    URCP_DEBUG(("              hop:%d\n", dev->hop));
    URCP_DEBUG(("              alarm_lev:%d\n", dev->alarm_level));
    URCP_DEBUG(("              p_aggr:%08x\n", dev->parent_aggr_mask));
    URCP_DEBUG(("              c_aggr:%08x\n", dev->child_aggr_mask));
    URCP_DEBUG(("              up ports:%d\n", dev->up_ports));
    URCP_DEBUG(("              type:%s\n", dev->dev_type));

    tmp = topoy_search(topoy_root, dev);/*�������ڵ�*/

    if(tmp != NULL)/*�ҵ���*/
    {
	/*��ӵ�����ָ����*/
	topoy_add_child(tmp, dev);

	/*��wait_list�����ӽڵ�*/
	tmp = wait_list;
	while((tmp != NULL) && (mac_cmp(dev->mac, tmp->parent_mac) != 0) )/*��Ч�ڵ�*/
	{
	    tmp = tmp->next;/*����*/
	}

	if(tmp != NULL)
	{
	    URCP_DEBUG(("found child in waitlist\n"));
	    dev->first_child = tmp;/*ָ���ӽڵ�*/
	}
    }
    else/*��wait_list���������ڵ�*/
    {
	URCP_DEBUG(("find parent fail\n"));
	if(wait_list != NULL)/*wait_listΪ��*/
	{
	    tmp = wait_list;
	    tmp1 = tmp;
	    /*��������*/
	    while( (tmp != NULL) && (topoy_search(tmp, dev) == NULL)) 
	    {
		tmp1 = tmp;
		tmp = tmp->next;/*��һ��*/
	    }

	    if(tmp == NULL)/*��û�ҵ�*/
	    {
		/*����wait_list*/
		tmp1->next = dev;
	    }
	    else
	    {
		URCP_DEBUG(("found parent in waitlist\n"));
		topoy_add_child(tmp, dev);/*����ڵ�*/
	    }
	}
	else
	{
	    /*����wait_list*/
	    wait_list = dev;
	}
    }
}


/*
 *���һ������
 */
static void topoy_add_child(topoy_t* parent, topoy_t* child)
{
    topoy_t* tmp=NULL, *tmp1=NULL, *tmp2=NULL;

    URCP_DEBUG(("%s\n", __func__));
    tmp = parent->first_child;
    if(tmp==NULL)
    {
	/*�ǵ�һ������*/
	parent->first_child = child;
	URCP_DEBUG(("the first child\n"));
	return;
    }

    /*�ҵ������ֵ�*/
    while((tmp != NULL) && (tmp->parent_port < child->parent_port))
    {
	URCP_DEBUG(("%s:0\n", __func__));
	tmp1 = tmp;
	tmp = tmp->next_sibling;/*��һ���ֵ�*/
    }

    URCP_DEBUG(("%s:1\n", __func__));
    if( (tmp !=  NULL) && (tmp->parent_port > 0u)/*���ڵ�Ϊ·�����Ļ�����ֵΪ0*/
	&& (tmp->parent_port == child->parent_port))/*�����ֵ�����ͬһ�˿���*/
    {
	URCP_DEBUG(("unknow dev\n"));
	if(tmp->virtual_dev == 1u)/*�Ѿ��������豸��*/
	{
	    child->next_sibling = tmp -> first_child;
	    tmp -> first_child = child;
	    return;
	}

	/*�м���δ֪�豸*/
	tmp2 = get_free_node();
	URCP_DEBUG(("%s:2\n", __func__));
	if(tmp2==NULL)
	{
	    return;
	}
	/*��һ������*/
	if(parent->first_child == tmp)
	{
	    URCP_DEBUG(("%s:3\n", __func__));
	    parent->first_child = tmp2;
	}
	else
	{
	    tmp1->next_sibling = tmp2;
	}
	tmp2->hop = child->hop; /*�����豸��hopΪ0*/
	tmp2->virtual_hop = 0u; 
	tmp2->virtual_dev = 1u; /*���Ϊ�����豸*/
	tmp2->parent_port = child->parent_port;/*��ͬ*/
	/*��������*/
	tmp2->next_sibling = tmp->next_sibling;/*������һ���ֵ�*/
	tmp2->first_child = child;
	tmp->next_sibling = NULL;
	child->next_sibling = tmp;
	URCP_DEBUG(("%s:4\n", __func__));

#if 0
	/*��virtual_parents��1*/
	topoy_hop_pulse(tmp);
#endif
    }
    else
    {
	URCP_DEBUG(("%s:5\n", __func__));
	/*����ڵ�*/
	if(parent->first_child == tmp)/*��һ������*/
	{
	    URCP_DEBUG(("%s:6\n", __func__));
	    parent->first_child = child;
	}
	else/*ǰ���������ֵ�*/
	{
	    tmp1->next_sibling = child; 
	}
	child->next_sibling = tmp;/*��������*/
    }
    URCP_DEBUG(("%s:7\n", __func__));
}
#if 0
/*
 *��������豸��
 *�������hopֵ��1
 *
 */
static void  topoy_hop_pulse(topoy_t*dev)
{
    URCP_DEBUG(("%s:\n", __func__));
    if(dev != NULL)
    { 
	URCP_DEBUG(("%s:1\n", __func__));
	/* ��1*/
	dev->virtual_hop += 1;

	/*��������*/
	if(dev->first_child != NULL)
	{
	    topoy_hop_pulse(dev->first_child);/*�ݹ����*/
	}

	/*�����ֵ�*/
	if (dev->next_sibling != NULL)
	{
	    topoy_hop_pulse(dev->next_sibling);/*�ݹ����*/
	}
    }
}
#endif
/* *
 *�����������������ڵ�
 *dev Ϊ������ڵ�
 */
static topoy_t* topoy_search(topoy_t* root, topoy_t* dev)
{
    topoy_t* parent=NULL;

    URCP_DEBUG(("%s\n", __func__));
    if((root == NULL) || (root->hop > (dev->hop-1u)))/*����*/
    {
	URCP_DEBUG(("%s:1\n", __func__));
	return NULL;
    }

    if(mac_cmp(root->mac, dev->parent_mac) == 0)/*�ҵ�*/
    {
	URCP_DEBUG(("find parent:%s\n", str_macaddr(root->mac)));
	parent = root;/*���ظ��ڵ�ָ��*/
    }
    else
    {
	if(root->first_child != NULL)/*�ǿ�*/
	{
	    URCP_DEBUG(("%s:child\n", __func__));
	    /*��������*/
	    parent = topoy_search(root -> first_child, dev);
	}

	if( (parent ==NULL)&&/*��δ�ҵ������������*/ 
	     (root->next_sibling != NULL))/*�ǿ�*/
	{
	    URCP_DEBUG(("%s:slibling\n", __func__));
	    /*�����ֵ�*/
	    parent = topoy_search(root -> next_sibling, dev);
	}
    }

    return parent;
}


/*
 *
 * ����������ĳ���ڵ����
 * topology_table�У�
 */
static void  topoy_tb_parse(topoy_t* dev)
{
    URCP_DEBUG(("%s:\n", __func__));
    topogy_list_t* head; 

    /*�ҵ�topology_table�б����ͷָ��head*/
    URCP_DEBUG(("hop:%d, vhop:%d\n",dev->hop, dev->virtual_hop));
    head = &SM_TOPOY_LIST[dev->hop + dev->virtual_hop];

    dev -> index = (uint16)(head -> count);
    head -> count ++;
    /*��dev����topology_table�б��������ĩβ*/
    if(head->last_node != NULL)
    {
	URCP_DEBUG(("%s:1\n", __func__));
	head->last_node->tb_next = dev->tb_index;
    }
    else
    {
	URCP_DEBUG(("%s:2\n", __func__));
	head -> first_node = dev->tb_index;/*��һ���ڵ�*/
    }
    URCP_DEBUG(("%s:3\n", __func__));
    head -> last_node = dev;/*��ӵ�����ĩβ*/
    dev->tb_next = NONE_NODE;/*ĩβ*/
    URCP_DEBUG(("%s:4\n", __func__));
}

/*
 *���������ڵ���Ϣ
 *ת���ɿɹ�ҳ����ʾ�ṹ
 */
void topoy_tb_form(topoy_t* root)
{
    URCP_DEBUG(("%s:\n", __func__));
    if(root != NULL)
    { 
	topoy_tb_parse(root);/*����ת��*/

	URCP_DEBUG(("node MAC:%s\n", str_macaddr(root->mac)));
	/*�������ӽڵ�*/
	if(root->first_child != NULL)
	{
	    URCP_DEBUG(("node first child MAC:%s\n", str_macaddr(root->first_child->mac)));
	    root->first_child->parent_index = root->index;
	    /*virtual hop�����ӽڵ�ڵ�*/
	    if(root->virtual_dev == 0u)/*������ڵ�*/
	    {
		root->first_child->virtual_hop = root->virtual_hop;
	    }
	    else
	    {
		root->first_child->virtual_hop = root->virtual_hop + 1u;/**/
	    }
	    topoy_tb_form(root->first_child);/*�ݹ�*/
	}

	/*�����ֵܽڵ�*/
	if (root->next_sibling != NULL)
	{
	    URCP_DEBUG(("node next sibling MAC:%s\n", str_macaddr(root->next_sibling->mac)));
	    root->next_sibling->parent_index = root->parent_index ;

	    /*virtual hop�����ֵܽڵ�*/
	    root->next_sibling->virtual_hop = root->virtual_hop;
	    topoy_tb_form(root->next_sibling);/*�ݹ�*/
	}
    }
}

/*
 *�������˷��ֱ���
 */
int32 topoy_discover(void)
{
    char name[64];
    uchar macaddr[MAC_LEN], buffer[ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(topoy_hdr_t)];
    uint16 id;
    uint32 ipaddr;
    topoy_hdr_t *topoy;
	int iftag = 0;

    urcp_mirror_can();

    /*�ȴ������豸�رվ�����, ����Ӱ�����˹���*/
    usleep(50000); /*����*/
    /*����Ựid*/
    if(session_alloc(SESSION_TYPE_HELLO, SESSION_SINGLE, TOPOY_CMD_TIME_OUT, &id, topoy_timer)<0)
    {
	return -1;
    }
    memset(name, 0, sizeof(name));
    get_switch_if_name(name);
    get_ip(name, &ipaddr);
    get_mac(name, macaddr);
    memset(buffer, 0, sizeof(buffer));
    /*ֻ����һ��topoy�Ự����*/
    iftag = urcp_buid_header(buffer, mac_addr_cooperate, (uint16)URCP_TOPOY, id, NULL);
    /*��ʼ��*/
    topoy_init();

    /*����ip��mac��ַ*/
    topoy_root->ip = ntohl(ipaddr);
    topoy_root->tb_index = INDEX_START;/*��һ��*/
    topoy_root->dev_model = DEV_MODEL;/*��һ��*/
    memcpy(topoy_root->mac, macaddr, MAC_LEN);

    /*�������˱���*/
	if(iftag == 1){
		topoy = (topoy_hdr_t*)(&buffer[ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32)]);
	}else{
		topoy = (topoy_hdr_t*)(&buffer[ETH_URCP_OFFSET+sizeof(urcp_hdr_t)]);
	}
    topoy -> version = TOPOY_VERSION;
    topoy -> type = TOPOY_HELLO;
    topoy -> flag = 0u;
    memcpy(topoy->root_id, macaddr, MAC_LEN);
    memcpy(topoy->parent_id, macaddr, MAC_LEN);
    topoy -> vlan_id = htons((uint16)sw_cpu_vid_get());
    topoy -> max_hop = htons((uint16)MAX_TOPOY_HOP);
    topoy -> hop = htons((uint16)1);/*���ڵ㷢��*/

    /*�㲥����*/
    topoy_forward(PORT_NULL, (uchar*)buffer, (uint32)sizeof(buffer));
    return 0;
}


/*
 *���˷��ֳ�ʱ
 *Ҫ�����ݷ��͵�ҳ����ʾ����
 */
void topoy_timer(uint16 reqId, session_type_t type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));

    reqId = reqId;
    type = type;
    /*��ʱ�Ŵ���*/
    if(urcp_state == URCP_STATUS_TIME_OUT)
    {
    /*���ӽڵ㿪ʼ����*/
#if 0
    if(topoy_root->first_child != NULL)
#endif
    {
	topoy_tb_form(topoy_root);
#if DEBUG_URCP
	topoy_tb_dump();
#endif
    }
    }

    return;
}
#if DEBUG_URCP
/*
 *��ӡ���˽ṹ��Ϣ
 *��ʾ�͸��ڵ�����ӹ�ϵ
 */
static void topoy_tb_dump(void)
{
    int32 ip, i, index;
    struct in_addr ip_addr;
    topoy_t* dev;

    ip = htonl(topoy_root->ip);
    memcpy(&ip_addr, &ip, 4);
    /*�ӵ�һ�㿪ʼ�����δ�ӡ����ڵ�*/
    for(i=0;i<MAX_TOPOY_HOP;i++)
    {
	index = SM_TOPOY_LIST[i].first_node;
	if(index == NONE_NODE)
	    break;
	URCP_DEBUG(("tb[%d]: dev num:%d\n", i, SM_TOPOY_LIST[i].count));
			/*��ӡ�������нڵ���Ϣ*/
	while(index != NONE_NODE)
	{
	    dev = &SM_TOPOY_TB[index-1];
	    ip = htonl(dev->ip);
	    memcpy(&ip_addr, &ip, 4);
	    URCP_DEBUG(("        MAC:%s\n", str_macaddr(dev->mac)));
	    URCP_DEBUG(("        IP:%s\n", inet_ntoa(ip_addr)));
	    URCP_DEBUG(("        parent MAC:%s\n", str_macaddr(dev->parent_mac)));
	    URCP_DEBUG(("        parent port:%d\n", dev->parent_port));
	    URCP_DEBUG(("        parent index:%d\n", dev->parent_index));
	    URCP_DEBUG(("        port:%d\n", dev->port));
	    URCP_DEBUG(("        hop:%d, vhop:%d\n", dev->hop, dev->virtual_hop));
	    URCP_DEBUG(("        index:%d\n", dev->index));
	    URCP_DEBUG(("        vlan id:%d\n", dev->vlan_id));
	    URCP_DEBUG(("        **********************************\n"));
	    index = dev->tb_next;
	}
	URCP_DEBUG(("\n"));
    }
}
#endif
