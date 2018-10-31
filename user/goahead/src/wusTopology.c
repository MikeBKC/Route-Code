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
 * 函数： formDefineTopoyDiscovery
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
extern void formDefineTopoyDiscovery(void)
{
	websAspDefine(T("aspOutPutGetTopoy"), getTopoy);
	websAspDefine(T("aspTopoInfo"), aspTopoInfo);
	websFormDefine(T("formTopoyDiscovery"), formTopoy);
}

/*******************************************************************
 *函数： formTopoyDiscovery
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2010-09-20
 ******************************************************************/
static void formTopoy(webs_t wp, char_t *path, char_t *query)
{
    /*发消息*/
    sendwebmsg(MSG_TYPE_GET, GET_URCP_TOPOY);

    websRedirect(wp, "switch/wait_topoy.asp");
}


/********************************************************************
 * 函数： getTopoy
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2010-09-20
 ********************************************************************/ 
static int getTopoy(int eid, webs_t wp, int argc, char **argv)
{
    int ip, i, j, index;
    char *tmp;
    struct in_addr ip_addr;
    topoy_t* dev;
    urcp_shm_t *mem_shadow=NULL;

    mem_shadow = get_shm_ptr();
    /*读取拓扑表*/
    for(i=0;i<MAX_TOPOY_HOP;i++)
    {
	/*本层第一个节点*/
	index = mem_shadow->topoy_list[i].first_node;
	/*为空说明到头了*/
	if(index == NONE_NODE)
	{
	    break;
	}
	/*遍历本层所有节点*/
	j = 0;
	websWrite(wp, "var level%d = new Array();\n", i);
	while(index != NONE_NODE)
	{ 
	    dev = &mem_shadow->topoy_table[index-1];
	    ip = htonl(dev->ip);
	    memcpy(&ip_addr, &ip, 4);
	    websWrite(wp, "level%d[%d] = new topoNode(", i, j++);
	    websWrite(wp, "%d,",  dev->parent_index);/*父节点位置*/
	    tmp = str_macaddr(dev->mac);
	    websWrite(wp, "\'%s\',", tmp);/*mac*/
	    tmp = inet_ntoa(ip_addr);
	    websWrite(wp, "\'%s\',", tmp);/*ip*/
	    websWrite(wp, "\'%d\',", dev->dev_model);/*设备类型*/
	    websWrite(wp, "%d,", dev->virtual_dev);/*是否虚拟设备*/
	    websWrite(wp, "%d,", dev->alarm_level);/*报警信息*/
	    websWrite(wp, "\'%d\',", dev->parent_port);/*父节点port*/
	    websWrite(wp, "\'%d\',", dev->port);/*与父节点相连port*/
	    websWrite(wp, "\'%d\',", dev->vlan_id);/*父节点对应port的vlan id*/
	    websWrite(wp, "\'%d\',", dev->up_ports);/*活动端口数*/
	    websWrite(wp, "\'%s\',", dev->dev_type);/*设备型号*/
            webs_write_port_list(wp, dev->parent_aggr_mask);
	    websWrite(wp, ",");/*父节点对应port的vlan id*/
            webs_write_port_list(wp, dev->child_aggr_mask);
	    websWrite(wp, ");\n");

	    /*下一个节点*/
	    index = dev->tb_next;
	}
    }

    websWrite(wp, "var maxDepth = %d;\n", i); /*深度*/

    /*不止一个节点*/
    if(i>0)
    {
	websWrite(wp, "level0[0].parent = -1;\n");/*根节点*/
    }
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
/********************************************************************
 * 函数： aspTopoInfo
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-06-03
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
