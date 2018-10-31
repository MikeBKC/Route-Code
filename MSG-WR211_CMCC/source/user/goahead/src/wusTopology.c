#include    <stdio.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    "webs.h"
#include    "utils.h"
#include    "webmsg.h"
#include    "uttfunction.h"

#ifdef CONFIG_SWITCH_EXT
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#endif

#if SW_FEATURE_URCP
#define  DEBUG_TOPOY 1
#if DEBUG_TOPOY 
#define TOPOY_DEBUG(argx) printf argx
#else
#define TOPOY_DEBUG(argx) 
#endif

static void formTopoy(webs_t wp, char_t *path, char_t *query);
static int getTopoy(int eid, webs_t wp, int argc, char **argv);
static int aspTopoInfo(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineTopoyDiscovery
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
 *******************************************************************/
extern void formDefineTopoyDiscovery(void)
{
	websAspDefine(T("aspOutPutGetTopoy"), getTopoy);
	websAspDefine(T("aspTopoInfo"), aspTopoInfo);
	websFormDefine(T("formTopoyDiscovery"), formTopoy);
}

/*******************************************************************
 *������ formTopoyDiscovery
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� KeHuatao
 *������ 2010-09-20
 ******************************************************************/
static void formTopoy(webs_t wp, char_t *path, char_t *query)
{
    /*����Ϣ*/
    sendwebmsg(MSG_TYPE_GET, GET_URCP_TOPOY);

    websRedirect(wp, "switch/wait_topoy.asp");
}


/********************************************************************
 * ������ getTopoy
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
 ********************************************************************/ 
static int getTopoy(int eid, webs_t wp, int argc, char **argv)
{
    int ip, i, j, index;
    char *tmp;
    struct in_addr ip_addr;
    topoy_t* dev;
    urcp_shm_t *mem_shadow=NULL;

    mem_shadow = get_shm_ptr();
    /*��ȡ���˱�*/
    for(i=0;i<MAX_TOPOY_HOP;i++)
    {
	/*�����һ���ڵ�*/
	index = mem_shadow->topoy_list[i].first_node;
	/*Ϊ��˵����ͷ��*/
	if(index == NONE_NODE)
	{
	    break;
	}
	/*�����������нڵ�*/
	j = 0;
	websWrite(wp, "var level%d = new Array();\n", i);
	while(index != NONE_NODE)
	{ 
	    dev = &mem_shadow->topoy_table[index-1];
	    ip = htonl(dev->ip);
	    memcpy(&ip_addr, &ip, 4);
	    websWrite(wp, "level%d[%d] = new topoNode(", i, j++);
	    websWrite(wp, "%d,",  dev->parent_index);/*���ڵ�λ��*/
	    tmp = str_macaddr(dev->mac);
	    websWrite(wp, "\'%s\',", tmp);/*mac*/
	    tmp = inet_ntoa(ip_addr);
	    websWrite(wp, "\'%s\',", tmp);/*ip*/
	    websWrite(wp, "\'%d\',", dev->dev_model);/*�豸����*/
	    websWrite(wp, "%d,", dev->virtual_dev);/*�Ƿ������豸*/
	    websWrite(wp, "%d,", dev->alarm_level);/*������Ϣ*/
	    websWrite(wp, "\'%d\',", dev->parent_port);/*���ڵ�port*/
	    websWrite(wp, "\'%d\',", dev->port);/*�븸�ڵ�����port*/
	    websWrite(wp, "\'%d\',", dev->vlan_id);/*���ڵ��Ӧport��vlan id*/
	    websWrite(wp, "\'%d\',", dev->up_ports);/*��˿���*/
	    websWrite(wp, "\'%s\',", dev->dev_type);/*�豸�ͺ�*/
            webs_write_port_list(wp, dev->parent_aggr_mask);
	    websWrite(wp, ",");/*���ڵ��Ӧport��vlan id*/
            webs_write_port_list(wp, dev->child_aggr_mask);
	    websWrite(wp, ");\n");

	    /*��һ���ڵ�*/
	    index = dev->tb_next;
	}
    }

    websWrite(wp, "var maxDepth = %d;\n", i); /*���*/

    /*��ֹһ���ڵ�*/
    if(i>0)
    {
	websWrite(wp, "level0[0].parent = -1;\n");/*���ڵ�*/
    }
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
/********************************************************************
 * ������ aspTopoInfo
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2011-06-03
 ********************************************************************/ 
static int aspTopoInfo(int eid, webs_t wp, int argc, char **argv)
{
    char param[8], *tmp; 

    strcpy(param, PAGE_TOPOY);
    tmp = param;
    aspSignalRemoteAuth(eid, wp, 1, &tmp);

    return 0;
}
#endif
