/*
 *
 *拓扑发现功能处理
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
 *获取一个空闲的节点          
 *
 **/    
static topoy_t* get_free_node(void)
{
    topoy_t* tmp;

    /*是否还有空节点*/
    if (free_index >= MAX_TOPOY_DEV ) 
    {
	/*用完了*/
	URCP_ERROR(("topoy:get free node failed\n"));
	return NULL;
    }

    URCP_DEBUG(("get node: %d\n", free_index));

    tmp = &(SM_TOPOY_TB[free_index]);
    free_index += 1;

    tmp->tb_index = free_index;/*比实际索引大一*/
    /*返回指针*/
    return  tmp;
}


/*
 *
 *分析头部信息
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

    /*判断hop值*/
    max_hop = ntohs(topoy->max_hop);
    hop = ntohs(topoy->hop);
    
    /*不能为0*/
    if ( hop == 0u ) 
    {
	URCP_ERROR(("topoy hop err:%d\n", hop));
	return -1;
    }

    /*判断mac地址有效性*/
    isValidMac((uchar*)topoy->root_id);/*根地址*/
    isValidMac((uchar*)topoy->parent_id);/*父节点地址*/

    if((topoy->type == TOPOY_HELLO) && (infor->packet_type != PACKET_MULTICAST))/*拓扑发现只能为组播*/
    {
	URCP_ERROR(("topoy hello DMAC err:\n"));
        return -1;
    }
    else if ((topoy->type == TOPOY_REPLY) && (infor->packet_type != PACKET_HOST))/*回复包只能为单播*/
    {
	URCP_ERROR(("topoy reply DMAC err:\n"));
        return -1;
    }

    if(hop > max_hop)/*hop值超限*/
    {
	URCP_ERROR(("topoy hop err:%d\n", hop));
	return -1;
    }
#if 0
#ifdef CONFIG_VSC7390
    else if((topoy->type == TOPOY_HELLO) && (hop < max_hop))/*转发*/
    {
	memset(name, 0, sizeof(name));
	memset(hwaddr, 0, sizeof(hwaddr));
	get_switch_if_name(name);
	get_mac(name, hwaddr);

	memcpy(mac, topoy->parent_id, MAC_LEN); /*保存原始mac*/
	parent_port = topoy->parent_port_id;/*保存port*/

	/*设置新值*/
	topoy->hop = htons(hop+1u);/*更新hop*/
	memcpy(topoy->parent_id, hwaddr, MAC_LEN); /*本机mac地址*/

	aggr = topoy->parent_aggr_mask;

	/*发送*/
#if 0
	memcpy(((eth_hdr_t*)(infor->raw))->src_addr, api_state->io.hwaddr, MAC_LEN);/*修改SMAC，否则会收不到包*/
#endif
	topoy_forward(infor->source_port, infor->raw, infor->length);

	/*恢复原始数据*/
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
 *发送拓扑发现包
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
    /*广播或组播*/
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
    /*依次从各端口发送*/
    for (port = PORT_NO_START; port < PORT_NO_END; port++)
    {
	/*不是和源端口同一汇聚组才发送*/
	if(ret.dest[port]!=0u)
	{
	    sw_trunk_members_get(port, member);
#ifdef CONFIG_IP1725
	    /*7390在ioctl里面实现链路状态的判断*/
	    sw_check_ports_link_status(member);
#endif
	    mask = bit_array_to_int(&member[START_INDEX], PORT_COUNT);
	    topoy->parent_port_id = htons((uint16)port);/*端口号*/
	    topoy->vlan_id = htons((uint16)sw_cpu_vid_get());/*这里尚需完善，改成各端口vid*/
	    topoy->parent_aggr_mask = htonl(mask);/*端口号*/
	    URCP_DEBUG(("%s: port:%d\n", __func__, port));
	    /*send*/
	    h2_send_by_port(port, frame, len, 0);
	}
    }
    printf("\n");
#endif
}

/*
 *初始化拓扑发现
 *
 */
void topoy_init(void)
{
    shm_ptr = get_shm_ptr();
    /*clear*/
    memset(&topoy_state, 0, sizeof(topoy_state));
    /*初始化链表*/
    memset(SM_TOPOY_LIST, 0, sizeof(SM_TOPOY_LIST));
    memset(SM_TOPOY_TB, 0, sizeof(SM_TOPOY_TB));
    topoy_root = &SM_TOPOY_TB[0];
    wait_list = NULL;
    free_index = INDEX_START;
}

/*
 *拓扑发现处理
 *
 */
int32 topoy_process(urcp_infor_t* infor)
{
    topoy_hdr_t* topoy;

    URCP_DEBUG(("%s\n", __func__));
    topoy = (topoy_hdr_t*)(infor->cmd_buff);
    /*分析头部信息*/
    if(topoy_header_parse(infor, topoy)<0)
    {
	return URCP_ANY_ERROR;
    }
    /*分析拓扑报文类型*/
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
 *收到 拓扑发现包
 *向root发送回复包
 */
static void topoy_hello(urcp_infor_t* infor)
{
    char buffer[ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(topoy_hdr_t)];/*发送缓存*/
    char dat[65];
    uchar macaddr[MAC_LEN];
    uint16 n=0;
    uint32 val;
    topoy_hdr_t* topoy;
    urcp_hdr_t* pkt;

    URCP_DEBUG(("%s\n", __func__));
    
    memset(buffer, 0, sizeof(buffer));
    /*复制收到的包*/
    topoy = (topoy_hdr_t*)(buffer+ETH_URCP_OFFSET+sizeof(urcp_hdr_t));
    memcpy(topoy, infor->cmd_buff, sizeof(topoy_hdr_t));
    
    memset(dat, 0, sizeof(dat));
    get_switch_if_name(dat);
    get_mac(dat, macaddr);
    /*重新设置*/
    memcpy(topoy->child_id, macaddr, MAC_LEN);/*本机mac地址*/
    topoy->child_port_id = htons((uint16)(infor->source_port));/*收包端口*/
    topoy->type = TOPOY_REPLY;
    topoy->flag = 0u;
    val = get_alarm_level();
    topoy->alarm_level = htonl(val);
    val = get_aggr_members(infor->source_port);
    topoy->child_aggr_mask = htonl(val);/*汇聚组成员*/
    topoy->dev_model = htons(DEV_MODEL);/*设备类型*/
    /*活动端口数*/
    n = countOne(get_link_status_mask());
    topoy->up_ports = htons(n);
    /*型号*/
    memset(dat, 0, sizeof(dat));
    getSystemInfoMachineNo(dat);
    strncpy(topoy->dev_type, dat, DEV_TYPE_SIZE-1);
    
    /*构建头部*/
    memcpy(infor->mac, topoy->root_id, MAC_LEN);
    urcp_build_reply_header(infor, (uchar*)buffer);

    /*重新设置*/
    pkt = (urcp_hdr_t*)(buffer+ETH_URCP_OFFSET);
    pkt->flag = 0u;
    pkt->cmd_no = 1;
    pkt->type = htons((uint16)URCP_TOPOY);
    pkt->error_status = htons((uint16)URCP_OK);

    /*发送其他信息，如设备型号，报警信息等*/

    /*发送回复包*/
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
 *拓扑回复包处理
 *
 */
static void topoy_reply(urcp_infor_t* infor)
{
    topoy_hdr_t* topoy;
    topoy_t* tmp;

    URCP_DEBUG(("%s\n", __func__));
    /*检查request id*/
    if(check_request_id(infor)<0)
    {
	return;
    }

    /*获取指针 */
    topoy = (topoy_hdr_t*)(infor->cmd_buff);
    /*判断hop值是否在可接受范围*/
    if(ntohs(topoy->hop)>MAX_TOPOY_HOP)
    {
	URCP_ERROR(("hop value too large\n"));
	return;
    }
    /*是否达到最大设备数*/
    if(topoy_state.dev_num+1>MAX_TOPOY_DEV)
    {
	URCP_ERROR(("connot save more dev\n"));
	return;
    }
    /*更新已接收设备数*/
    topoy_state.dev_num = topoy_state.dev_num + 1u;

    /*分配内存*/
    tmp = get_free_node();
    if(tmp == NULL)
    {
	return;
    }

    /*设置节点参数*/
    memcpy(tmp->parent_mac, topoy->parent_id,MAC_LEN);
    memcpy(tmp->mac, topoy->child_id,MAC_LEN);
    tmp->parent_port = ntohs(topoy->parent_port_id);
    tmp->port = ntohs(topoy->child_port_id);
    tmp->ip = ntohl(infor->urcp_header->ip); 
    tmp->vlan_id = ntohs(topoy->vlan_id);
    tmp->hop = ntohs(topoy->hop);

    /*节点其它信息*/
    tmp->alarm_level = ntohl(topoy->alarm_level);
    tmp->parent_aggr_mask = ntohl(topoy->parent_aggr_mask);
    tmp->child_aggr_mask = ntohl(topoy->child_aggr_mask);
    tmp->dev_model = ntohs(topoy->dev_model);
    tmp->up_ports = ntohs(topoy->up_ports);
    strncpy(tmp->dev_type, topoy->dev_type, DEV_TYPE_SIZE-1);

    topoy_dev_add(tmp);
}

/*
 *添加一个节点到二叉树中
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

    tmp = topoy_search(topoy_root, dev);/*搜索父节点*/

    if(tmp != NULL)/*找到了*/
    {
	/*添加到孩子指针中*/
	topoy_add_child(tmp, dev);

	/*在wait_list中找子节点*/
	tmp = wait_list;
	while((tmp != NULL) && (mac_cmp(dev->mac, tmp->parent_mac) != 0) )/*有效节点*/
	{
	    tmp = tmp->next;/*继续*/
	}

	if(tmp != NULL)
	{
	    URCP_DEBUG(("found child in waitlist\n"));
	    dev->first_child = tmp;/*指向子节点*/
	}
    }
    else/*在wait_list中搜索父节点*/
    {
	URCP_DEBUG(("find parent fail\n"));
	if(wait_list != NULL)/*wait_list为空*/
	{
	    tmp = wait_list;
	    tmp1 = tmp;
	    /*依次搜索*/
	    while( (tmp != NULL) && (topoy_search(tmp, dev) == NULL)) 
	    {
		tmp1 = tmp;
		tmp = tmp->next;/*下一个*/
	    }

	    if(tmp == NULL)/*仍没找到*/
	    {
		/*插入wait_list*/
		tmp1->next = dev;
	    }
	    else
	    {
		URCP_DEBUG(("found parent in waitlist\n"));
		topoy_add_child(tmp, dev);/*插入节点*/
	    }
	}
	else
	{
	    /*插入wait_list*/
	    wait_list = dev;
	}
    }
}


/*
 *添加一个孩子
 */
static void topoy_add_child(topoy_t* parent, topoy_t* child)
{
    topoy_t* tmp=NULL, *tmp1=NULL, *tmp2=NULL;

    URCP_DEBUG(("%s\n", __func__));
    tmp = parent->first_child;
    if(tmp==NULL)
    {
	/*是第一个孩子*/
	parent->first_child = child;
	URCP_DEBUG(("the first child\n"));
	return;
    }

    /*找到合适兄弟*/
    while((tmp != NULL) && (tmp->parent_port < child->parent_port))
    {
	URCP_DEBUG(("%s:0\n", __func__));
	tmp1 = tmp;
	tmp = tmp->next_sibling;/*下一个兄弟*/
    }

    URCP_DEBUG(("%s:1\n", __func__));
    if( (tmp !=  NULL) && (tmp->parent_port > 0u)/*父节点为路由器的话，该值为0*/
	&& (tmp->parent_port == child->parent_port))/*两个兄弟连在同一端口下*/
    {
	URCP_DEBUG(("unknow dev\n"));
	if(tmp->virtual_dev == 1u)/*已经是虚拟设备了*/
	{
	    child->next_sibling = tmp -> first_child;
	    tmp -> first_child = child;
	    return;
	}

	/*中间有未知设备*/
	tmp2 = get_free_node();
	URCP_DEBUG(("%s:2\n", __func__));
	if(tmp2==NULL)
	{
	    return;
	}
	/*第一个儿子*/
	if(parent->first_child == tmp)
	{
	    URCP_DEBUG(("%s:3\n", __func__));
	    parent->first_child = tmp2;
	}
	else
	{
	    tmp1->next_sibling = tmp2;
	}
	tmp2->hop = child->hop; /*虚拟设备的hop为0*/
	tmp2->virtual_hop = 0u; 
	tmp2->virtual_dev = 1u; /*标记为虚拟设备*/
	tmp2->parent_port = child->parent_port;/*相同*/
	/*插入树中*/
	tmp2->next_sibling = tmp->next_sibling;/*连接下一个兄弟*/
	tmp2->first_child = child;
	tmp->next_sibling = NULL;
	child->next_sibling = tmp;
	URCP_DEBUG(("%s:4\n", __func__));

#if 0
	/*将virtual_parents加1*/
	topoy_hop_pulse(tmp);
#endif
    }
    else
    {
	URCP_DEBUG(("%s:5\n", __func__));
	/*插入节点*/
	if(parent->first_child == tmp)/*第一个孩子*/
	{
	    URCP_DEBUG(("%s:6\n", __func__));
	    parent->first_child = child;
	}
	else/*前面有其他兄弟*/
	{
	    tmp1->next_sibling = child; 
	}
	child->next_sibling = tmp;/*链接起来*/
    }
    URCP_DEBUG(("%s:7\n", __func__));
}
#if 0
/*
 *添加虚拟设备后
 *将子孙的hop值加1
 *
 */
static void  topoy_hop_pulse(topoy_t*dev)
{
    URCP_DEBUG(("%s:\n", __func__));
    if(dev != NULL)
    { 
	URCP_DEBUG(("%s:1\n", __func__));
	/* 加1*/
	dev->virtual_hop += 1;

	/*遍历孩子*/
	if(dev->first_child != NULL)
	{
	    topoy_hop_pulse(dev->first_child);/*递归调用*/
	}

	/*遍历兄弟*/
	if (dev->next_sibling != NULL)
	{
	    topoy_hop_pulse(dev->next_sibling);/*递归调用*/
	}
    }
}
#endif
/* *
 *遍历二叉树搜索父节点
 *dev 为待插入节点
 */
static topoy_t* topoy_search(topoy_t* root, topoy_t* dev)
{
    topoy_t* parent=NULL;

    URCP_DEBUG(("%s\n", __func__));
    if((root == NULL) || (root->hop > (dev->hop-1u)))/*不是*/
    {
	URCP_DEBUG(("%s:1\n", __func__));
	return NULL;
    }

    if(mac_cmp(root->mac, dev->parent_mac) == 0)/*找到*/
    {
	URCP_DEBUG(("find parent:%s\n", str_macaddr(root->mac)));
	parent = root;/*返回父节点指针*/
    }
    else
    {
	if(root->first_child != NULL)/*非空*/
	{
	    URCP_DEBUG(("%s:child\n", __func__));
	    /*遍历孩子*/
	    parent = topoy_search(root -> first_child, dev);
	}

	if( (parent ==NULL)&&/*尚未找到，需继续查找*/ 
	     (root->next_sibling != NULL))/*非空*/
	{
	    URCP_DEBUG(("%s:slibling\n", __func__));
	    /*遍历兄弟*/
	    parent = topoy_search(root -> next_sibling, dev);
	}
    }

    return parent;
}


/*
 *
 * 将二叉树中某个节点插入
 * topology_table中：
 */
static void  topoy_tb_parse(topoy_t* dev)
{
    URCP_DEBUG(("%s:\n", __func__));
    topogy_list_t* head; 

    /*找到topology_table中本层的头指针head*/
    URCP_DEBUG(("hop:%d, vhop:%d\n",dev->hop, dev->virtual_hop));
    head = &SM_TOPOY_LIST[dev->hop + dev->virtual_hop];

    dev -> index = (uint16)(head -> count);
    head -> count ++;
    /*将dev插入topology_table中本层链表的末尾*/
    if(head->last_node != NULL)
    {
	URCP_DEBUG(("%s:1\n", __func__));
	head->last_node->tb_next = dev->tb_index;
    }
    else
    {
	URCP_DEBUG(("%s:2\n", __func__));
	head -> first_node = dev->tb_index;/*第一个节点*/
    }
    URCP_DEBUG(("%s:3\n", __func__));
    head -> last_node = dev;/*添加到本层末尾*/
    dev->tb_next = NONE_NODE;/*末尾*/
    URCP_DEBUG(("%s:4\n", __func__));
}

/*
 *将二叉树节点信息
 *转换成可供页面显示结构
 */
void topoy_tb_form(topoy_t* root)
{
    URCP_DEBUG(("%s:\n", __func__));
    if(root != NULL)
    { 
	topoy_tb_parse(root);/*进行转换*/

	URCP_DEBUG(("node MAC:%s\n", str_macaddr(root->mac)));
	/*遍历孩子节点*/
	if(root->first_child != NULL)
	{
	    URCP_DEBUG(("node first child MAC:%s\n", str_macaddr(root->first_child->mac)));
	    root->first_child->parent_index = root->index;
	    /*virtual hop传给子节点节点*/
	    if(root->virtual_dev == 0u)/*非虚拟节点*/
	    {
		root->first_child->virtual_hop = root->virtual_hop;
	    }
	    else
	    {
		root->first_child->virtual_hop = root->virtual_hop + 1u;/**/
	    }
	    topoy_tb_form(root->first_child);/*递归*/
	}

	/*遍历兄弟节点*/
	if (root->next_sibling != NULL)
	{
	    URCP_DEBUG(("node next sibling MAC:%s\n", str_macaddr(root->next_sibling->mac)));
	    root->next_sibling->parent_index = root->parent_index ;

	    /*virtual hop传给兄弟节点*/
	    root->next_sibling->virtual_hop = root->virtual_hop;
	    topoy_tb_form(root->next_sibling);/*递归*/
	}
    }
}

/*
 *发送拓扑发现报文
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

    /*等待其他设备关闭镜像功能, 以免影响拓扑功能*/
    usleep(50000); /*毫秒*/
    /*分配会话id*/
    if(session_alloc(SESSION_TYPE_HELLO, SESSION_SINGLE, TOPOY_CMD_TIME_OUT, &id, topoy_timer)<0)
    {
	return -1;
    }
    memset(name, 0, sizeof(name));
    get_switch_if_name(name);
    get_ip(name, &ipaddr);
    get_mac(name, macaddr);
    memset(buffer, 0, sizeof(buffer));
    /*只允许一个topoy会话存在*/
    iftag = urcp_buid_header(buffer, mac_addr_cooperate, (uint16)URCP_TOPOY, id, NULL);
    /*初始化*/
    topoy_init();

    /*更新ip和mac地址*/
    topoy_root->ip = ntohl(ipaddr);
    topoy_root->tb_index = INDEX_START;/*第一个*/
    topoy_root->dev_model = DEV_MODEL;/*第一个*/
    memcpy(topoy_root->mac, macaddr, MAC_LEN);

    /*构建拓扑报文*/
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
    topoy -> hop = htons((uint16)1);/*根节点发出*/

    /*广播发送*/
    topoy_forward(PORT_NULL, (uchar*)buffer, (uint32)sizeof(buffer));
    return 0;
}


/*
 *拓扑发现超时
 *要将数据发送到页面显示缓存
 */
void topoy_timer(uint16 reqId, session_type_t type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));

    reqId = reqId;
    type = type;
    /*超时才处理*/
    if(urcp_state == URCP_STATUS_TIME_OUT)
    {
    /*从子节点开始遍历*/
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
 *打印拓扑结构信息
 *显示和父节点的连接关系
 */
static void topoy_tb_dump(void)
{
    int32 ip, i, index;
    struct in_addr ip_addr;
    topoy_t* dev;

    ip = htonl(topoy_root->ip);
    memcpy(&ip_addr, &ip, 4);
    /*从第一层开始，依次打印各层节点*/
    for(i=0;i<MAX_TOPOY_HOP;i++)
    {
	index = SM_TOPOY_LIST[i].first_node;
	if(index == NONE_NODE)
	    break;
	URCP_DEBUG(("tb[%d]: dev num:%d\n", i, SM_TOPOY_LIST[i].count));
			/*打印本层所有节点信息*/
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
