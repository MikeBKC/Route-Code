#include "arpa/inet.h"



int  cpe_refresh_igd_wanipconnection(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
    parameter_node_t * tmp_param, *tmp_node, *child_param;
    child_param = param_node->child;
    if(child_param)
    {
        for(tmp_param=child_param->next_sibling; tmp_param; )
        {
            cwmp_log_info("refresh WANConnectionDevice node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        int wan_index = get_index_after_paramname(param_node, "WANDevice"); 
        int wan_conn_dev_index = get_index_after_paramname(param_node, "WANConnectionDevice"); 

        wan_index = 1;
        wan_conn_dev_index = 1;
        if(wan_index == 1)
        {
            parameter_node_t * ipconn_param;
            switch(wan_conn_dev_index)
            {
                case 1: 
                    cwmp_model_copy_parameter(param_node, &ipconn_param, 1);
                    break;

                case 2:
                    cwmp_model_copy_parameter(param_node, &ipconn_param, 1); 
                    cwmp_model_copy_parameter(param_node, &ipconn_param, 2);
                    break;
            }
        }
        else
        if(wan_index == 2)
        {
            //don't support
        }

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }

    return FAULT_CODE_OK;
}

int getpppoeStatus(int wannum)
{
    int returnval = 0;
    char if_addr[16];

    if(getIfIp(getWanIfNamePPP(wannum), if_addr) == 0) {
        returnval = 1;
    } else {
        returnval = 0;
    }
    return returnval;
}

int wan_get_igd_wd_ip_connectionstatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int state = 0;
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);

    if(profIf)
    {
        switch(profIf->ether.connMode) 
        {
            case ETH_CONN_DHCP:
                if(getIfIp(getWanIfName(1), NULL) == 0) 
                {
                    state=1;
                }    
                state &= get_phylink_status(1);
                break;
            case ETH_CONN_STATIC:
                state=get_phylink_status(1);
                break;
            case ETH_CONN_PPPOE:
                state = getpppoeStatus(1);
                state &= get_phylink_status(1);
                break;
            default:
                break;
        }
    }
    if(state == 1)
    {
        *value = "Connected";
    }else{
        *value = "Not Connected";
    }
    //get_method(cwmp,name,value,pool);
    return FAULT_CODE_OK;
}

int wan_set_igd_wd_ip_connectionstatus(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int wan_get_igd_wd_ip_externalipaddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);

    struct in_addr addr;                                                                                                                                          
    addr.s_addr = profIf->ether.sta.ip;
    *value = inet_ntoa(addr);
    return FAULT_CODE_OK;
}

int wan_set_igd_wd_ip_externalipaddress(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int wan_get_igd_wd_ip_macaddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    InterfaceProfile *profIf = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);

    *value = converMac6To12Byte(profIf->ether.mac);
    return FAULT_CODE_OK;
}

int wan_set_igd_wd_ip_macaddress(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}
#if 0
typedef unsigned long long uint64;
typedef struct _sw_port_stat_t{
    uint64 rxPkts;
    uint64 txPkts;

    uint64 rxOctets;
    uint64 txOctets;

    uint64 rxPktsErrors;
    uint64 txPktsErrors;
}sw_port_stat_t;

void wan_lan_port_statics(unsigned int eth2Port , sw_port_stat_t *port_stat)
{
    FILE *f;
    char buf[256];
    unsigned long rx_bytes, tx_bytes;
    unsigned int rx_pkts, tx_pkts, rx_err, tx_err;
    char *p, *ifname;
    unsigned int wanIndex=0;

    if ((f = fopen("/proc/net/dev", "r")) != NULL) 
    {
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
		//wanIndex = which_wan_lan_name(ifname);
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
#endif
int cpe_get_igd_wd_status(cwmp_t * cwmp, const char * name, const char ** value, int length, callback_register_func_t callback_reg)
{
    sw_port_stat_t tmp;
    char childname[128];
    parameter_node_t * pn;
    memset(&tmp, 0, sizeof(sw_port_stat_t));
    memset(&childname, 0, sizeof(childname));

    snprintf(childname,128,"%s.EthernetBytesReceived",name);
    pn = cwmp_get_parameter_node(cwmp->root, childname);
    int wan_index = get_index_after_paramname(pn, "WANDevice"); 
    wan_lan_port_statics(wan_index, &tmp);

    if(pn->value == NULL){
	pn->value = (char *) malloc(128);
	if(pn->value == NULL)
	    return FAULT_CODE_9002;
	 pn->value_length = 128;
    }
    snprintf(pn->value,128,"%llu",tmp.rxOctets);

    snprintf(childname,128,"%s.EthernetPacketsReceived",name);
    pn = cwmp_get_parameter_node(cwmp->root, childname);
     if(pn->value == NULL){
	pn->value = (char *) malloc(128);
	if(pn->value == NULL)
	    return FAULT_CODE_9002;
	 pn->value_length = 128;
    }
    snprintf(pn->value,128,"%llu",tmp.rxPkts);

    return FAULT_CODE_OK;
}

int cpe_get_value_by_name(cwmp_t * cwmp, const char * name, const char ** value, int length, callback_register_func_t callback_reg){

    parameter_node_t * pn = cwmp_get_parameter_node(cwmp->root, name);
    if(!pn ||  pn->value == NULL){
	    return FAULT_CODE_9002;
    }
    *value =  pn->value; 
    return FAULT_CODE_OK;
}
