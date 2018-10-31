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
#include <mib.h>
#include <profacce.h>
#include <mibUrcp.h>
#include "common.h"
#include "hash.h"
#include "urcp_tftp.h"
#include "urcp_cmd.h"
#include "urcp_debug.h"

#ifdef URCP_AP
#include "dhcp.h"
#include "packet.h"
#include "trap_sta.h"

#if (CLIENT_HOST_NAME == FYES)
static struct dhcp_interface  dhcpif;   /* Only one interface for now */

extern hash_table_info_t *pStaTb_ap;

static uchar scan_opts(struct dhcp_interface  *dif, const uchar  *options, uint32 maxlen)
{
    db_info(URCP_MODULE_ID_HOST_NAME,"\n ---> scan_opts() begin  <---\n");
    uchar *op, len, overload = 0;

    for (op = options; op < &options[maxlen]; op += len) 
    {
	len = op[DHCP_OPOFFS_LEN];
//	db_info(URCP_MODULE_ID_HOST_NAME,("option tag:%d, len:%d\n", op[DHCP_OPOFFS_TAG], len));	 
	switch (op[DHCP_OPOFFS_TAG]) 
	{
	    case DHO_PAD :
		len = 1;
		break;
	    case DHO_DHCP_MESSAGE_TYPE :
		if (len != 1) 
		{ /* Option 53 : DHCP Message Type Length */
		    db_info(URCP_MODULE_ID_HOST_NAME,"%s, %d\n", __func__,len );
		}
		else 
		{
		    dif->dhcp_msgtype = op[DHCP_OPOFFS_DATA];
		}
		return 0;
		break;
	    case DHO_DHCP_OPTION_OVERLOAD :
#if 0
		if (len != 1) 
		{ /* Option 52 : Overload Length */
		    DHCP_DEBUG(("DHCP: Bad len %u for option %u\n",
				(unsigned)len, (unsigned)*op));
		    len = 0;
		}
		else 
		{
		    overload = op[DHCP_OPOFFS_DATA];
		}
#endif
		break;
	    case DHO_DHCP_AGENT_OPTIONS:
		break;
	    case DHO_END :
		break;
	    default :
		break;
	}
	len += DHCP_OPOFFS_DATA;
    }
    db_info(URCP_MODULE_ID_HOST_NAME,"\n ---> scan_opts() end  <---\n");
    return overload;
}

static void scan_options(struct dhcp_interface  *dif, struct dhcp_packet  *dp, uint32 frmlen)
{
    db_info(URCP_MODULE_ID_HOST_NAME,"\n ---> scan_options() begin  <---\n");
    uchar overload;

    if (memcmp(&dp->options[0], DHCP_OPTIONS_COOKIE, 4)) /* No cookie -> must be BOOTP */
    {
	dif->dhcp_msgtype = DHCPBOOTP;
    }
    else 
    {
	overload = scan_opts(dif, &dp->options[4], frmlen - DHCP_FIXED_NON_UDP - 4u);
	if (overload & 1)
	{
	    (void)scan_opts(dif, dp->file, DHCP_FILE_LEN);
	}
	if (overload & 2)
	{
	    (void)scan_opts(dif, dp->sname, DHCP_SNAME_LEN);
	}
    }
    db_info(URCP_MODULE_ID_HOST_NAME,"\n ---> scan_options() end  <---\n");
}

/*从options选项中筛选host_name, 返回host_name的长度*/
static int get_client_opt_value(uchar type, struct dhcp_packet  *dp, uint32 maxlen, uchar *host_name, uint32 name_size)
{
    uchar *op, len;
    if (memcmp(&dp->options[0], DHCP_OPTIONS_COOKIE, 4) == 0)
    {
	for (op = &dp->options[4]; op < &dp->options[maxlen - DHCP_FIXED_NON_UDP - 4u]; op += len) 
	{
	    len = op[DHCP_OPOFFS_LEN];
	    db_info(URCP_MODULE_ID_HOST_NAME,"option tag:%d, len:%d\n", op[DHCP_OPOFFS_TAG], len);  
	    if ( (op[DHCP_OPOFFS_TAG] == type) && (len != 0)) 
	    {
		if (len <= name_size )
		{
		    name_size = len;   
		}
		strncpy(host_name, &op[DHCP_OPOFFS_DATA], name_size);
		return name_size;   
	    }
	    len += DHCP_OPOFFS_DATA;
	}
    }
    return 0;
}

int32 dhcpport_receive(system_frame_header_t* header)
{
    db_info(URCP_MODULE_ID_HOST_NAME,"\n ---> dhcpport_receive() begin  <---\n");
    uint32 dhcp_len;
    struct dhcp_interface *dif;
    struct dhcp_packet *dp;
    mac_addr_t  mac_addr;
    uchar name[CLIENT_HOST_NAME_SIZE] = "";
    tb_entry_hdr_t tp;
    sta_entry_ap_t  *entry=NULL;
    int ret = 0;
    uint32 ip_addr = 0u;

    dp = (struct dhcp_packet*)(header->data);
#if 0
    get_mac(ETH_NAME, mac_addr);
    if (mac_cmp(mac_addr, dp->chaddr)== 0)
    {
	/*发给本机地址不处理*/
	db_info(URCP_MODULE_ID_HOST_NAME,("\nmac_cmp()==0\n"));
	return FALSE;
    }
#endif
    db_info(URCP_MODULE_ID_HOST_NAME,"%s,client mac:%02x%02x%02x%02x%02x%02x**\n",__func__, dp->chaddr[0],dp->chaddr[1],dp->chaddr[2],\
	    dp->chaddr[3],dp->chaddr[4],dp->chaddr[5]);
    
    dhcp_len = header->length;/*dhcp数据长度*/
    dif = &dhcpif;              /* Map source_port into dhcp interface */

    if (dhcp_len >= BOOTP_MIN_LEN /* && dp->op==BOOTREPLY && dp->xid == dif->xid*/) 
    {
	scan_options(dif, dp, dhcp_len);
	switch (dif->dhcp_msgtype) 
	{
	    case DHCPDISCOVER :
		db_info(URCP_MODULE_ID_HOST_NAME,"%s, DHCPDISCOVER\n", __func__);
		break;
	    case DHCPOFFER :
		db_info(URCP_MODULE_ID_HOST_NAME,"%s, DHCPOFFER\n", __func__);
		break;
	    case DHCPREQUEST :
		db_info(URCP_MODULE_ID_HOST_NAME,"%s, DHCPREQUEST\n", __func__);
#if 1
		if ( get_client_opt_value(DHO_HOST_NAME, dp, dhcp_len, name, sizeof(name)) )
		{
		    db_info(URCP_MODULE_ID_HOST_NAME,"%s,name:%s\n", __func__, name);
		    /*添加到hash表*/
		    memset(&tp, 0, sizeof(tb_entry_hdr_t));
		    db_info(URCP_MODULE_ID_HOST_NAME,"%s,mac:%02x%02x%02x%02x%02x%02x\n",__func__, dp->chaddr[0],dp->chaddr[1],dp->chaddr[2],\
			    dp->chaddr[3],dp->chaddr[4],dp->chaddr[5]);
		    memcpy(tp.mac_addr,dp->chaddr, MAC_LEN);
		    ret = hash_add_entry(pStaTb_ap, &tp);
		    db_info(URCP_MODULE_ID_HOST_NAME,"%s,ret = %d\n", __func__, ret);
		    if (ret != HASH_RET_FAILED )
		    {
			entry = (sta_entry_ap_t*)(pStaTb_ap->pDst);
			strncpy(entry->host_name, name, CLIENT_HOST_NAME_SIZE);
			db_info(URCP_MODULE_ID_HOST_NAME,"%s, hash_name:%s\n", __func__, entry->host_name);
			get_client_opt_value(DHO_DHCP_REQUESTED_ADDRESS, dp, dhcp_len, &ip_addr, sizeof(ip_addr));
			entry->ip = ip_addr;
			db_info(URCP_MODULE_ID_HOST_NAME,"%s, ip:%s\n", __func__, inet_ntoa(ip_addr));
		    }
		}
#endif
		/*取host_name*/
		break;
	    case DHCPACK :
		db_info(URCP_MODULE_ID_HOST_NAME,"%s, DHCPACK\n", __func__);
		/*取IP，如果是组播的方式*/
		break;
	    case DHCPNAK :
		break;
	    case DHCPBOOTP :
		break;
	    case DHCPRELEASE :
		break;
	    default :
		break;
	}
    }
    db_info(URCP_MODULE_ID_HOST_NAME,"\n ---> dhcpport_receive() end  <---\n");
    return TRUE;
}

#endif

#endif	    /*end URCP_AP*/
