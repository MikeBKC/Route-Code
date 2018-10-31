#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"


#if (FEATURE_PORT_STATISTICS==FYES)
#ifdef CONFIG_SWITCH_EXT 
#include <sw_ioctl.h>
#else
#define PORT_COUNT  24
typedef unsigned long long uint64;
typedef struct _sw_port_stat_t{
    uint64 rxPkts;
    uint64 txPkts;

    uint64 rxOctets;
    uint64 txOctets;

    uint64 rxPktsErrors;
    uint64 txPktsErrors;
}sw_port_stat_t;
#endif


static sw_port_stat_t if_eth2[(MAX_WAN_IF_COUNT+1u)];
static int getPortStatics(int eid, webs_t wp, int argc, char **argv);
static void  get_wan_lan_port_statics(unsigned int port, sw_port_stat_t *port_stat);
static void wan_lan_port_statics(unsigned int eth2Port, sw_port_stat_t *port_stat);
static void formStatReset(webs_t wp, char_t *path, char_t *query);
extern char* getWanIfName(int num);
extern char* getLanIfName(void);
#if (UTT_SMART_UI == FYES)
static void  get_wanPort_statics(unsigned int port, sw_port_stat_t *port_stat);
#endif


/***********************************************************
 * Function: which_wan_lan_name
 * 
 * Description: get ethernet interface number according to 
 *	    interface name.
 *
 * History: added by Chen.Shiyu 2011-10-14
************************************************************/
static unsigned int which_wan_lan_name(char *ifname)
{
    unsigned int i;
    unsigned int result = 0u;

    if(strcmp(ifname, getLanIfName()) == 0)
    {
	result = 1u;  /*lan: eth2.1*/
    }
    else
    {	
	for(i = 1u; i <= MAX_WAN_IF_COUNT; i++)
	{
	    if(strcmp(ifname, getWanIfName((int)i)) == 0)
	    {
		result = i + 1u; /* WAN: eth2.x */
		break;
	    }
	}
    }

    return result;
}

#if (UTT_SMART_UI == FYES)
static void  get_wanPort_statics(unsigned int port, sw_port_stat_t *port_stat)
{
    sw_port_stat_t tmp;

    memset(&tmp, 0, sizeof(sw_port_stat_t));
    wan_lan_port_statics(port, &tmp);
    port_stat->rxPkts = tmp.rxPkts;
    port_stat->txPkts = tmp.txPkts;
    /* rx or tx bytes */
    port_stat->rxOctets = tmp.rxOctets;
    port_stat->txOctets = tmp.txOctets;

    /* rx or tx error packets */
    port_stat->rxPktsErrors = tmp.rxPktsErrors;
    port_stat->txPktsErrors = tmp.txPktsErrors;

}
#endif
/*
 * get wan or lan statistics
 * KeHuatao
 */
static void  get_wan_lan_port_statics(unsigned int port, sw_port_stat_t *port_stat)
{
    sw_port_stat_t tmp;

    memset(&tmp, 0, sizeof(sw_port_stat_t));
    wan_lan_port_statics(port, &tmp);
    port_stat->rxPkts = tmp.rxPkts - if_eth2[port].rxPkts;
    port_stat->txPkts = tmp.txPkts - if_eth2[port].txPkts;
    /* rx or tx bytes */
    port_stat->rxOctets = tmp.rxOctets - if_eth2[port].rxOctets;
    port_stat->txOctets = tmp.txOctets - if_eth2[port].txOctets;

    /* rx or tx error packets */
    port_stat->rxPktsErrors = tmp.rxPktsErrors - if_eth2[port].rxPktsErrors;
    port_stat->txPktsErrors = tmp.txPktsErrors - if_eth2[port].txPktsErrors;

}
/****************************************************************
 * Function wan_lan_port_statics_get
 *
 * Description: To get informations about wan and lan port statistics
 *
 * History: added by Chen.Shiyu 2011-10-12
*****************************************************************/
static void wan_lan_port_statics(unsigned int eth2Port, sw_port_stat_t *port_stat)
{
    FILE *f;
    char buf[256];
    unsigned long rx_bytes, tx_bytes;
    unsigned int rx_pkts, tx_pkts, rx_err, tx_err;
    char *p, *ifname;
    unsigned int wanIndex=0;

    /* open the file and the status is read only. */
    if ((f = fopen("/proc/net/dev", "r")) != NULL) 
    {
	/* read one line from the file and write it to buffer. */
	fgets(buf, sizeof(buf), f);
	
	fgets(buf, sizeof(buf), f);
	while (fgets(buf, sizeof(buf), f)) 
	{
	    if ((p = strchr(buf, ':')) != NULL)
	    {
		*p = 0;
		
		if ((ifname = strrchr(buf, ' ')) == NULL) 
		{
		    ifname = buf; /* To get interface name */
		}
	       	else 
		{
		    ++ifname;
		}
		
		/* get interface number. */
		wanIndex = which_wan_lan_name(ifname);
		if (wanIndex == eth2Port)
		{
		    /* read interface informations */
		    if (sscanf(p + 1, "%lu%u%u%*u%*u%*u%*u%*u%lu%u%u", &rx_bytes,
				&rx_pkts, &rx_err, &tx_bytes, &tx_pkts, &tx_err) == 6)
		    {
		        /* statistics the current interface informations */
                        /* rx or tx packets */
		        port_stat->rxPkts = rx_pkts;
		        port_stat->txPkts = tx_pkts;
		        /* rx or tx bytes */
		        port_stat->rxOctets = rx_bytes;
                        port_stat->txOctets = tx_bytes;

		        /* rx or tx error packets */
		        port_stat->rxPktsErrors = rx_err;
		        port_stat->txPktsErrors = tx_err;

			break;
		    }
		}
	    }
	}
    }

    fclose(f); /* close the file */

    return;
}

void getPortStaticsForSvg(webs_t wp, char comma)
{
#if SW_FEATURE_PORT_STATISTICS
#ifdef CONFIG_VSC7390
    char rxbuffer[64], txbuffer[64];
    uint32 port;
    sw_port_stat_t port_stat;

    for(port=PORT_NO_START;port<PORT_NO_END;port++)
    {
	memset(&port_stat, 0, sizeof(sw_port_stat_t));
        sw_port_counter_get(port, &port_stat);
	sprintf(rxbuffer, "0x%llx", port_stat.rxOctets);
	sprintf(txbuffer, "0x%llx", port_stat.txOctets);
	websWrite(wp, "%c'%d':{rx:%s,tx:%s}", comma, port, rxbuffer, txbuffer);
	comma = ',';/*只要打印过一次。则再次打印 首部必须是 逗号*/
    }
#endif
#endif
}
/********************************************************************
 * 函数： fromDefinePortStatics
 * 功能： 页面与交互转换函数
 * 创建： 2010-08-16
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： jfgu
 * 日期： 2010-09-01 
 *******************************************************************/
void formDefinePortStatics(void)
{
	websFormDefine(T("formStatReset"), formStatReset);	
	websAspDefine(T("aspOutPortStatics"), getPortStatics);
	memset(if_eth2, 0, sizeof(if_eth2));
}

/*
 * Function:  formStatDetailed
 *
 * Description: this function is used to clear the port count and redirect
 *		current url page in the end.
 *
 * History: added by Chen.Shiyu	    2011-09-29
 */
static void formStatReset(webs_t wp, char_t *path, char_t *query)
{
    char *m_port;
    char m_url[] = "switch/stat_detailed.asp?port=";
    char tmpurl[100] = {0};
    unsigned int i_port = 0u, wanLanPort = 0u;
    sw_port_stat_t port_stat;

    /*取出页面控件中的port值*/
    m_port = websGetVar(wp, T("port"), T(""));
    if(strncasecmp(m_port, "LAN", 3) == 0)
    {
	wanLanPort = 1u;
    }
    else if(strncasecmp(m_port, "WAN", 3) == 0)
    {
	wanLanPort = (unsigned int)strtoul(&m_port[3], NULL, 0);  
	wanLanPort += 1u;/*跳过lan口*/
    }
    else
    {
	i_port = (unsigned int)strtoul(m_port, NULL, 0);
    }

    if (wanLanPort > 0u)
    {
	/* backup the WAN and LAM port counters */
	memset(&port_stat, 0, sizeof(sw_port_stat_t));
	wan_lan_port_statics(wanLanPort, &port_stat);
        if_eth2[wanLanPort].rxPkts = port_stat.rxPkts;
        if_eth2[wanLanPort].txPkts = port_stat.txPkts;
        /* rx or tx bytes */
        if_eth2[wanLanPort].rxOctets = port_stat.rxOctets;
        if_eth2[wanLanPort].txOctets = port_stat.txOctets;
        /* rx or tx error packets */
        if_eth2[wanLanPort].rxPktsErrors = port_stat.rxPktsErrors;
        if_eth2[wanLanPort].txPktsErrors = port_stat.rxPktsErrors;
    }
#if SW_FEATURE_PORT_STATISTICS
#ifdef CONFIG_SWITCH_EXT
    else
    {
	/* clear the counter */
	sw_port_counter_clear(i_port);
    }
#endif
#endif

    /*重定向到原先的url*/
    memcpy(tmpurl, m_url, sizeof(m_url));
    strcat(tmpurl, m_port);

    websRedirect(wp, tmpurl);
}

/*
 * Function:	getPortStatics
 *
 * Description: To get some variables to the page.
 *
 * History: added by Chen.Shiyu	    2011.09.29
 */
int getPortStatics(int eid, webs_t wp, int argc, char **argv)
{
    char *m_port_no = NULL;
    char m_buffer[1024];
    unsigned int wanLanPort = 0u, i_port_no = 0u;
    sw_port_stat_t port_stat;
   
    /*读取aspout参数*/
    if(ejArgs(argc,argv, T("%s"), &m_port_no) < 1) 
    {
	getTheErrorMsg(eid, wp, argc, argv);
    }
    else
    {
	/*如果port为空，默认为端口1*/
	if (m_port_no == NULL)
	{
	    i_port_no = 1u;
	}
	else
	{
	    if(strncasecmp(m_port_no, "LAN", 3) == 0)
	    {
		wanLanPort = 1u;
	    }
	    else if(strncasecmp(m_port_no, "WAN", 3) == 0)
	    {
		wanLanPort = (unsigned int)strtoul(&m_port_no[3], NULL, 0);  
		wanLanPort += 1u;/*跳过lan口*/
	    }
	    else
	    {
		/* change the port type */
		i_port_no = (unsigned int)strtoul(m_port_no, NULL, 0);  
		if(i_port_no==0)
		{
		    i_port_no = 1u;
		}
	    }
	}
	
	/* confirm the page WAN port numbers */
	websWrite(wp, "var wanNumPorts = %d;\n", MAX_WAN_IF_COUNT);

	/* read the port information */
	websWrite(wp, "var port =\"%s\";\n", m_port_no);
	websWrite(wp, "var counters = new Array();\n");

	memset(&port_stat, 0, sizeof(sw_port_stat_t));
	/* get the port counters statistics informations */
	if(i_port_no > 0u)
	{
#if SW_FEATURE_PORT_STATISTICS
	    /* get the switch port informations */
	    sw_port_counter_get(i_port_no, &port_stat);
#endif
	}
	else
	{
#if (UTT_SMART_UI == FYES)
	    get_wanPort_statics(wanLanPort, &port_stat);
#else
	    /* To get the WAN and LAN port inforamations */
	    get_wan_lan_port_statics(wanLanPort, &port_stat);
#endif
	}

	/*帧数*/
	sprintf(m_buffer, "%llu", port_stat.rxPkts);
	websWrite(wp, "counters[0] = \"%s\";\n", m_buffer);
	
	sprintf(m_buffer, "%llu", port_stat.txPkts);
	websWrite(wp, "counters[1] = \"%s\";\n", m_buffer);

	/*字节数*/
        sprintf(m_buffer, "%llu", port_stat.rxOctets);
        websWrite(wp, "counters[2] = \"%s\";\n", m_buffer);

	sprintf(m_buffer, "%llu", port_stat.txOctets);
	websWrite(wp, "counters[3] = \"%s\";\n", m_buffer);
#if SW_FEATURE_PORT_STATISTICS
#ifdef CONFIG_VSC7390 
	/*组播广播包数*/
	sprintf(m_buffer, "%llu", port_stat.rxBcastMcastPkts);
	websWrite(wp, "counters[4] = \"%s\";\n", m_buffer);

	sprintf(m_buffer, "%llu", port_stat.txBcastMcastPkts);
	websWrite(wp, "counters[5] = \"%s\";\n", m_buffer);

	/*64 bytes*/
	sprintf(m_buffer, "%llu", port_stat.rxPkts64Octets);
	websWrite(wp, "counters[8] = \"%s\"\n", m_buffer);

	sprintf(m_buffer, "%llu", port_stat.txPkts64Octets);
	websWrite(wp, "counters[9] = \"%s\";\n", m_buffer);

	/*1024 bytes*/
	sprintf(m_buffer, "%llu", port_stat.rxPkts1024to1518Octets);
	websWrite(wp, "counters[10] = \"%s\";\n", m_buffer);

	sprintf(m_buffer, "%llu", port_stat.txPkts1024to1518Octets);
	websWrite(wp, "counters[11] = \"%s\";\n", m_buffer);
#endif	
#endif	
        /*错误帧数*/
        sprintf(m_buffer, "%llu", port_stat.rxPktsErrors);
        websWrite(wp, "counters[6] = \"%s\";\n", m_buffer);

        sprintf(m_buffer, "%llu", port_stat.txPktsErrors);
        websWrite(wp, "counters[7] = \"%s\";\n", m_buffer);
	/* get the error msg. */
	getTheErrorMsg(eid, wp, argc, argv);
    }
    return 0;
}
#endif
