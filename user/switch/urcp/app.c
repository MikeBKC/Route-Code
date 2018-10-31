/*
 *
 *
 *联动协议v3版处理
 *
 *ke.huatao
 *2010-09-01
 *
 */
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> /*inet_addr*/
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <dirent.h>
#include "common.h"
#include <syslog.h>

#ifdef URCP_WEB
#include <switch.h>
#include <sw_cfg.h>
#include <sw_ioctl.h>
#endif /* end URCP_WEB */

#ifdef URCP_AP
#include <user_oid.h>
#include <user/goahead/src/wuScript.h>
#include "ap_msg.h"
#include "swinfo.h"
#endif

#include "urcp.h"
#include "urcp_cmd.h"
#include "app.h"
#include "urcp_hello.h"
#include "app_msg.h"
#include <mib.h>
#include <profacce.h>
#include <mibUrcp.h>


extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern void get_ip_from_dhcp_server(uint32 ac_ip);
void makeFactoryProfiles(void);
static int sz_num;
static uint32 mac_filter_num;
static int ap_auth_num;
extern int apStatusForAcOperate;
static pthread_mutex_t ap_auth = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lbMutex = PTHREAD_MUTEX_INITIALIZER;


/*
 *静态变量
 *
(* */
/*
 *静态函数
 *
 */

#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET
static void get_tag_vlan_config(urcp_vlan_conf_t* qvlan_config);
#endif
#ifdef URCP_CMD_VLAN_CONFIG_GET
static void get_pvlan_config(urcp_vlan_conf_t* pvlan_config);
static int32 set_pvlan_config(urcp_vlan_conf_t*pvlan_config);
#endif
#ifdef URCP_CMD_SAFEBIND_CONFI_SET
static void safe_bind(void);
#endif

#ifdef URCP_TRAP_AP_CHANNEL
uint16 receive_number_channel(cmd_hdr_t* cmd, urcp_dev_t** dev)
{
    uint16 ret=URCP_OK;
    uint16 *channel_head = NULL, *channel_end = NULL;

    if((ntohs(cmd->length) == sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	channel_head = (uint16 *)((uchar* )cmd + sizeof(cmd_hdr_t));
	channel_end = (uint16 *)((uchar* )cmd + sizeof(cmd_hdr_t) + sizeof(uint16));
	if (0 == ntohs( *channel_head))
	{
	    (*dev)->wl_info.channel_head = ntohs(0);
	    (*dev)->wl_info.channel_end = ntohs(*channel_end);
	}
	else
	{
	    (*dev)->wl_info.channel_head = ntohs(*channel_head);
	    (*dev)->wl_info.channel_end = ntohs(0);
	}
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret; 
}
#endif

//接收4字节数据所使用函数
uint16 receive_number(cmd_hdr_t* cmd, urcp_dev_t** dev, uint32 offset)
{
    uint16 ret=URCP_OK;
    uint32 *ptr;
    cmd_hdr_gen_t *tmp;

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
        ptr = (uint32*)((uchar*)(*dev) + offset);
	*ptr = ntohl(tmp->value);
	URCP_DEBUG(("%s: %d\n", __func__, *ptr));
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#ifdef URCP_CMD_SERIAL_NO_GET_REPLY
/*
 *接收序列号
 *KeHuatao
 */
uint16 receive_serial_no(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint32* sn;
    
    if((dev!=NULL) && (*dev!=NULL))
    {
	/*移动指针*/
	sn = (uint32*)(cmd+1);
	*sn = ntohl(*sn);
	URCP_DEBUG(("SN 0x%08x\n", *sn)); 
    
	/*读取数据*/
	(*dev) -> serial_no = *sn;
    }

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_DEV_NAME_GET_REPLY
/*
 *接收设备名
 *KeHuatao
 */
uint16 receive_dev_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char  *tmp;
    uint32 len;
#if (AP_LOAD_BALANCE == FYES)
	ApLoadBalanceProfile *profLb=NULL;
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    int j;
#endif

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*计算长度*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	if(len<MAX_DEV_NAME_LEN)/*长度合法*/
	{
	    /*复制设备名称*/
	    tmp = (char*)(cmd+1);
	    memcpy((*dev)->dev_name, tmp, len);
	    (*dev)->dev_name[len] = (uchar)('\0');
	    URCP_DEBUG(("dev name:%s\n", (*dev)->dev_name)); 
#if (AP_LOAD_BALANCE == FYES )
        if( (*dev)->load_balance != 0 ){
            profLb=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(mibType,(*dev)->load_balance-1);  /*get inst pointer*/
            if((profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
            {
                for(j=0;j<profLb->num;j++){
                    if (mac_cmp(profLb->ap[j].macAddr.addr, (*dev)->mac) == 0){
                        memcpy(profLb->ap[j].dev_name,tmp,len);
                        break;
                    }
                }
            }

        }
#endif
	}
	else/*长度不对*/
	{
	    URCP_DEBUG(("dev name length err\n")); 
	}
    }

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_UP_PORTS_GET_REPLY
/*
 *接收活动端口数
 *KeHuatao
 *
 */
uint16 receive_up_ports(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint32* ports;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*移动指针*/
	ports = (uint32*)(cmd+1);
	/*读取数据*/
	(*dev)->up_ports = ntohl(*ports);
	URCP_DEBUG(("%s:0x%08x\n", __func__, (*dev)->up_ports)); 
    }

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_PRODUCT_MODEL_GET_REPLY
/*
 *接收产品型号信息
 *KeHuatao
 */
uint16 receive_product_model(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char *tmp;
    uint32 len;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*长度信息*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	/*判断长度是否合法*/
	if(len<DEV_TYPE_SIZE)
	{
	    /*复制数据*/
	    tmp = (char*)(cmd+1);
	    memcpy(((*dev)->dev_model), tmp, len);
	    (*dev)->dev_model[len] = '\0';
	    URCP_DEBUG(("product model :%s\n", (*dev)->dev_model)); 
	}
    }

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_SW_VERSION_GET_REPLY
/*
 *接收版本信息
 *KeHuatao
 */
uint16 receive_sw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char *tmp;
    uint32 len;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*长度信息*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	/*判断长度是否合法*/
	if(len<SW_VERSION_SIZE)
	{
	    /*复制数据*/
	    tmp = (char*)(cmd+1);
	    memcpy((*dev)->sw_version, tmp, len);
	    (*dev)->sw_version[len] = (uchar)('\0');
	    DBCK("sw version:%s\n", (*dev)->sw_version); 
	}
    }

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_HW_VERSION_GET_REPLY
/*
 *接收版本信息
 *KeHuatao
 */
uint16 receive_hw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char *tmp;
    uint32 len;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*长度信息*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	/*判断长度是否合法*/
	if(len<HW_VERSION_SIZE)
	{
	    /*复制数据*/
	    tmp = (char*)(cmd+1);
	    memcpy((*dev)->hw_version, tmp, len);
	    (*dev)->hw_version[len] = (uchar)('\0');
	    DBCK("sw version:%s\n", (*dev)->hw_version);
	}
    }

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_SERIAL_NO_GET
/*
 *
 *读取序列号
 *KeHuatao
 *
 */
uint16 get_serial_no(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info)
{
    uint32 sn, *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_SERIAL_NO);
    cmd -> cmd_status = htons(URCP_OK);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));
    
    /*serial no*/
    sn = getSystemSn();
    URCP_DEBUG(("serial NO is %d\n", sn));
    *tmp = htonl(sn);

    /*length*/
    cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}

uint16 trap_get_serial_no(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info)
{
    uint32 sn, *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_SERIAL_NO);
    cmd -> cmd_status = htons(URCP_OK);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));
    
    /*serial no*/
    sn = getSystemSn();
    URCP_DEBUG(("serial NO is %d\n", sn));
    *tmp = htonl(sn);

    /*length*/
    cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_SW_VERSION_GET
/*
 *
 *读取软件版本
 *KeHuatao
 */
uint16 get_sw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info)
{
    char ver[64], *tmp;
    uint16 ret;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_SW_VERSION);
    cmd -> length = sizeof(cmd_hdr_t);

    memset(ver, 0, sizeof(ver));

    get_sysinfo_sw_version(ver);
    URCP_DEBUG(("sw version=%s\n", ver));
    /*sw version*/
    if(strcmp(ver, "") != 0)
    {
	tmp = *app + sizeof(cmd_hdr_t);
	memcpy(tmp, ver, strlen(ver));
	/*length*/
	cmd -> length += strlen(ver);
	ret = URCP_OK;
    }
    else
    {
	ret = URCP_ANY_ERROR; 
    }

    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app += cmd -> length;
    cmd -> length = htons(cmd->length);

    return ret;
}

uint16 trap_get_sw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info)
{
    char ver[64], *tmp;
    uint16 ret;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_SW_VERSION);
    cmd -> length = sizeof(cmd_hdr_t);

    memset(ver, 0, sizeof(ver));

    get_sysinfo_sw_version(ver);
    URCP_DEBUG(("sw version=%s\n", ver));
    /*sw version*/
    if(strcmp(ver, "") != 0)
    {
	tmp = *app + sizeof(cmd_hdr_t);
	memcpy(tmp, ver, strlen(ver));
	/*length*/
	cmd -> length += strlen(ver);
	ret = URCP_OK;
    }
    else
    {
	ret = URCP_ANY_ERROR; 
    }

    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app += cmd -> length;
    cmd -> length = htons(cmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_HW_VERSION_GET
/*
 *
 *读取软件版本
 *KeHuatao
 */
uint16 get_hw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info)
{
    char ver[64], *tmp;
    uint16 ret;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_HW_VERSION);
    cmd -> length = sizeof(cmd_hdr_t);

    memset(ver, 0, sizeof(ver));

    get_sysinfo_hw_model(ver);
    DBCK("hw version=%s", ver);
    /*sw version*/
    if(strcmp(ver, "") != 0)
    {
	tmp = *app + sizeof(cmd_hdr_t);
	memcpy(tmp, ver, strlen(ver));
	/*length*/
	cmd -> length += strlen(ver);
	ret = URCP_OK;
    }
    else
    {
	ret = URCP_ANY_ERROR; 
    }

    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app += cmd -> length;
    cmd -> length = htons(cmd->length);

    return ret;
}

uint16 trap_get_hw_version(cmd_hdr_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info)
{
    char ver[64], *tmp;
    uint16 ret;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_HW_VERSION);
    cmd -> length = sizeof(cmd_hdr_t);

    memset(ver, 0, sizeof(ver));

    get_sysinfo_hw_model(ver);
    URCP_DEBUG(("hw version=%s\n", ver));
    /*sw version*/
    if(strcmp(ver, "") != 0)
    {
	tmp = *app + sizeof(cmd_hdr_t);
	memcpy(tmp, ver, strlen(ver));
	/*length*/
	cmd -> length += strlen(ver);
	ret = URCP_OK;
    }
    else
    {
	ret = URCP_ANY_ERROR; 
    }

    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app += cmd -> length;
    cmd -> length = htons(cmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_PRODUCT_MODEL_GET
/*
 *读取产品型号
 *KeHuatao
 *
 */
uint16 get_product_model(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{ 
    char model[64], *tmp;
    uint16 ret;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_PRODUCT_MODEL);
    cmd -> length = sizeof(cmd_hdr_t);

    memset(model, 0, sizeof(model));
    
    get_sysinfo_machine_model(model);

    URCP_DEBUG(("product model=%s\n", model));
    /*sw version*/
    if(strcmp(model, "") != 0)
    {
	tmp = *app_data + sizeof(cmd_hdr_t);
	memcpy(tmp, model, strlen(model));
	/*length*/
	cmd -> length += strlen(model);
	ret = URCP_OK;
    }
    else
    {
	ret = URCP_ANY_ERROR; 
    }

    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    /*返回结果*/
    return ret;
}

uint16 trap_get_product_model(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    char model[64], *tmp;
    uint16 ret;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_PRODUCT_MODEL);
    cmd -> length = sizeof(cmd_hdr_t);

    memset(model, 0, sizeof(model));

    get_sysinfo_machine_model(model);

    URCP_DEBUG(("product model=%s\n", model));
    /*sw version*/
    if(strcmp(model, "") != 0)
    {
	tmp = *app_data + sizeof(cmd_hdr_t);
	memcpy(tmp, model, strlen(model));
	/*length*/
	cmd -> length += strlen(model);
	ret = URCP_OK;
    }
    else
    {
	ret = URCP_ANY_ERROR; 
    }

    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    /*返回结果*/
    return ret;
}
#endif

#ifdef URCP_CMD_UP_PORTS_GET
/*
 *发送本机活动端口数
 *KeHuatao
 */
uint16 get_up_ports(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 ports, *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_UP_PORTS);
    cmd -> cmd_status = htons(URCP_OK);

    ports = (uint32)getAlivePort();
    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));
    /*serial no*/
    *tmp = htonl(ports);

    /*length*/
    cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}
#endif

/*
 *收到不能识别的命令时发送回复包
 *KeHuatao
 */
uint16 cmd_unknow(uint32 unknow_type, uchar** app_data)
{
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = unknow_type;
    cmd -> cmd_status = htons(URCP_CMD_UNKNOW);

    /*length*/
    cmd -> length = htons(sizeof(cmd_hdr_t));
    /*数据长度*/
    *app_data += sizeof(cmd_hdr_t);

    return URCP_OK;
}

#ifdef URCP_CMD_DEV_NAME_GET
/*
 *读取设备名
 *KeHuatao
 */
uint16 get_dev_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    char *tmp;
    cmd_hdr_t* cmd;
    ApBasicConfProfile *prof = NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_DEV_NAME);
    cmd -> length = sizeof(cmd_hdr_t);

    /*get name */
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF,0);
    if(prof != NULL)
    {
	tmp = *app_data + sizeof(cmd_hdr_t);
	strcpy(tmp, prof->dev_name);
	/*length*/
	cmd -> length += strlen(prof->dev_name);
    }

    URCP_DEBUG(("%s: dev_name=%s\n", __func__, prof->dev_name));
    cmd -> cmd_status = (int16)htons((uint16)URCP_OK);
    /*数据长度*/
    *app_data += cmd->length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}

uint16 trap_get_dev_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    char *tmp;
    cmd_hdr_t* cmd;
    ApBasicConfProfile *prof = NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_DEV_NAME);
    cmd -> length = sizeof(cmd_hdr_t);

    /*get name */
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF,0);
    if(prof != NULL)
    {
	tmp = *app_data + sizeof(cmd_hdr_t);
	strcpy(tmp, prof->dev_name);
	/*length*/
	cmd -> length += strlen(prof->dev_name);
    }

    URCP_DEBUG(("%s: dev_name=%s\n", __func__, prof->dev_name));
    cmd -> cmd_status = (int16)htons((uint16)URCP_OK);
    /*数据长度*/
    *app_data += cmd->length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_MIRROR_CONFIG_SET
/*
 *设置镜像端口
 *KeHuatao
 *
 */
uint16 set_mirror_config(cmd_hdr_t *cmd, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint32 len, mirr_port, i;
    port_bit_mask_t mirr_source;
    urcp_mirror_conf_t* mirror_config;
    cmd_hdr_t* scmd;
    uint16 ret;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    /*长度信息*/
    i = (uint32)ntohs(cmd->length);
    len = i - sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(len>=(sizeof(urcp_mirror_conf_t)))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	mirror_config = (urcp_mirror_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
	mirr_port = ntohl(mirror_config->mirror_port);
	mirr_source = ntohl(mirror_config->mirror_source);
	URCP_DEBUG(("urcp set mirr:%d, 0x%08x \n", mirr_port, mirr_source));
	/*判断有效范围*/
	if( (mirr_port>=VTSS_PORT_NO_START) && (mirr_port<VTSS_PORT_NO_END)
		&& (mirr_source<=ALL_PORT_MASK))
	{
	    cfg_write_mirror_port(mirr_port);
	    set_mirror_port(mirr_port);
	    mirr_source = mirr_source<<1u;
	    cfg_write_mirror_source_mask(mirr_source);
	    set_mirror_source_mask(mirr_source);/*重新设置*/
	    scmd -> cmd_status = htons(URCP_OK);
	    URCP_DEBUG(("urcp set mirr: %d-%08x\n",mirr_port, mirr_source));
	    ret=0;
	}
	else
	{
	    URCP_DEBUG(("urcp set mirr error\n"));
	    scmd -> cmd_status = htons(URCP_ANY_ERROR);
	    ret=1;
	}
    }
    else
    {
	URCP_DEBUG(("urcp set mirr length error:%d\n", len));
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
	ret=1;
    }
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_GET
/*
 *发送镜像端口信息
 *KeHuatao
 */
uint16 get_mirror_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar *tmp;
    cmd_hdr_t* cmd;
    urcp_mirror_conf_t* mirror_config;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;
    /*构建回复命令*/
    cmd -> type = htonl(CMD_MIRROR_CONFIG);

    /*数据部分*/
    mirror_config = (urcp_mirror_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    /*get mirror*/
    mirror_config->port_num = htonl(PORT_COUNT); 
    mirror_config->mirror_port = htonl(cfg_read_mirror_port()); 
    mirror_config->mirror_source = cfg_read_mirror_source_mask(); 
    mirror_config->mirror_source = mirror_config->mirror_source>>1u;
    mirror_config->mirror_source = htonl(mirror_config->mirror_source);
    /*数据长度及状态*/
    cmd -> cmd_status = htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_mirror_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_MIRROR_CONFIG_GET_REPLY
/*
 *收到镜像信息
 *KeHuatao
 */
uint16 receive_mirror_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint32 len, i;
    urcp_mirror_conf_t *mirror_config;

    /*长度信息*/
    i = ntohs(cmd->length);
    len = i - sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(len>=(sizeof(urcp_mirror_conf_t)))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	mirror_config = (urcp_mirror_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
	mirror_config->port_num = ntohl(mirror_config->port_num);
	mirror_config->mirror_port = ntohl(mirror_config->mirror_port);
	mirror_config->mirror_source = ntohl(mirror_config->mirror_source);
	URCP_DEBUG(("%s: num:%d, port:%d, source:0x%08x\n", __func__, mirror_config->port_num, mirror_config->mirror_port,mirror_config->mirror_source)); 
	urcp_rev_mirror_get_reply(urcp_infor->mac, ntohs(urcp_infor->urcp_header->request_id), mirror_config);
    }

    return URCP_OK;
}
#endif
#if defined(URCP_CMD_MIRROR_CONFIG_GET_REPLY) || defined(URCP_CMD_MIRROR_CONFIG_SET_REPLY) 
/*
 *镜像操作回调函数
 *KeHuatao
 */
void urcp_mirror_cb(uint16 reqId, session_type_t type1, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_mirror_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_TIME_OUT:/*超时*/
		urcp_mirror_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_OK:
	    if(type1==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_mirror_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*不做处理*/
	    break;
    }

    return;
}
#endif

#ifdef URCP_CMD_PASSWD_AUTH_GET
/*
 *收到请求密码验证的包
 *KeHuatao
 * */
uint16 get_passwd_auth(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    cmd_hdr_t* cmd;

    URCP_DEBUG(("%s\n", __func__)); 
    cmd = (cmd_hdr_t*)(*app_data);

    /*构建回复命令*/
    cmd -> type = htonl(CMD_PASSWD_AUTH);

    /*数据长度及状态*/
    cmd -> cmd_status = htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_PASSWD_AUTH_GET_REPLY
/*
 *密码验证回调函数
 *KeHuatao
 *
 */
void urcp_passwd_auth_cb(uint16 reqId, session_type_t type1, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL:/*操作失败*/
	case URCP_STATUS_AUTH_ERROR:/*密码错误*/
	case URCP_STATUS_OK:/*成功*/
	    urcp_passwd_auth_set_status(reqId, urcp_state);
	    URCP_DEBUG(("%s: password %d\n", __func__, urcp_state));
	    /*设置页面提示信息*/
	    break;
	default:
	    /*其它不做处理,由交换互判断为超时*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_AGGR_CONFIG_GET
/*
 *发送汇聚信息
 *bhou
 */
uint16 get_aggr_config (cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32  j = 0u ,ucount = 0u;
    uint16  i= 0u;
    uint32 umember = 0u;
    uint16 ugroup = 0u;
    uchar buf[1];
    uchar *tmp = buf;
    uchar * cmember = buf;
    cmd_hdr_t* cmd;
    urcp_aggr_conf_t* aggr_config;
    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;
    /*构建回复命令*/
    cmd -> type = htonl(CMD_AGGR_CONFIG);
    /*数据部分*/
    aggr_config = (struct urcp_aggr_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    
    memset(aggr_config,0,sizeof(urcp_aggr_conf_t));/*写之前先清除*/
    /*get data*/
    aggr_config->port_num = htonl(PORT_COUNT); /*端口个数，各个设备端口个数可能不一样*/

    for( i = 1u; i <= MAX_AGGR_GROUP_INDEX ; i++)
    {
	ugroup = cfg_read_aggr_group(i);
	umember = 0u;
	if(ugroup > 0u)
	{
	    cmember = cfg_read_aggr_member(i);/*此处用字符串表示，需转换成uint32*/
	    for (j = PORT_NO_START; j < PORT_NO_END; j++)
	    {
		if (cmember[j] == 1u)/*该汇聚组包含该接口*/
		{
		    umember |= ((uint32)1u << (j-1u));
		}
	    }
	    if(umember > 0u)/*汇聚组成员不为空*/
	    {
		aggr_config->aggr_group[ucount].index = htons(ugroup);/*组号*/
		aggr_config->aggr_group[ucount].member = htonl(umember);/*成员，转化为网络字节序*/
		strncpy(aggr_config->aggr_group[ucount].remark , cfg_read_aggr_group_remark(i) ,MAX_REMARK_LEN);/*汇聚组名称*/
		ucount++;
	    }
	}
    }
    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_aggr_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AGGR_CONFIG_GET_REPLY
/*
 *接受汇聚信息
 *bhou
 */
uint16 receive_aggr_config(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    uint16 uindex;
    uint32 umember;
    uint32 i = 0u;
    urcp_aggr_conf_t *aggr_config;

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_aggr_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	aggr_config = (urcp_aggr_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/
	aggr_config->port_num = ntohl(aggr_config->port_num);
	if(aggr_config->port_num > MAX_URCP_PORT_NUM)/*大于最大限制数，则可能发生内存泄露*/
	{
	    aggr_config->port_num = MAX_URCP_PORT_NUM;
	}
	for( i = 1u; i <= aggr_config->port_num ; i++)/*最多port num个汇聚组*/
	{
	    uindex = aggr_config->aggr_group[(i-1u)].index;
	    aggr_config->aggr_group[(i-1u)].index = ntohs(uindex);
	    umember = aggr_config->aggr_group[(i-1u)].member;
	    aggr_config->aggr_group[(i-1u)].member = ntohl(umember);
            aggr_config->aggr_group[(i-1u)].remark[MAX_REMARK_LEN-1u] = '\0';
	    URCP_DEBUG(("%s: aggr_no:%d, aggr_member :0x%x\n", __func__, aggr_config->aggr_group[(i-1u)].index,aggr_config->aggr_group[(i-1u)].member)); 
	}
	urcp_rev_aggr_get_reply(urcp_infor->mac, ntohs(urcp_infor->urcp_header->request_id), aggr_config);/*写入misc buf*/
    }

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AGGR_CONFIG_SET
/*
 *联动设置汇聚
 *bhou
 *
 */
uint16 set_aggr_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 aggr_no =0u ;
    uint16 i = 0u;
    port_bit_mask_t umember = 0u;
    unsigned char aggr_member[PORT_NO_END];

    uint32 u_port_num = 0u;
    char*aggr_remark = NULL;
    urcp_aggr_conf_t* aggr_config = NULL;
    cmd_hdr_t* scmd = NULL;
    uint16 ret = URCP_OK;

    memset(aggr_member,0,sizeof(aggr_member));
    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    /*长度信息*/
    tmp = (uchar*)cmd;
    /*复制数据*/
    aggr_config = (urcp_aggr_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    u_port_num = ntohl(aggr_config->port_num);/*联动操作此设备的设备，传递过来的端口个数*/
    aggr_no = ntohs(aggr_config->aggr_group[0].index);/*组号*/
    umember = ntohl(aggr_config->aggr_group[0].member);/*成员*/
    aggr_config->aggr_group[0].remark[MAX_REMARK_LEN-1u]='\0';/*防止内存泄露*/
    aggr_remark = aggr_config->aggr_group[0].remark;/*名称*/
    URCP_DEBUG(("urcp set aggr:%d, 0x%08x \n", aggr_no, aggr_member));
    //frame_dump1(((uchar*)aggr_config), ((uint32)sizeof(urcp_aggr_conf_t)));
    if(u_port_num == PORT_COUNT)/*端口个数相等才可设置*/
    {
	for(i=PORT_NO_START;i<PORT_NO_END;i++)
	{
	    if( (umember & ((uint32)0x01u << (i-1u))) > 0u)
	    {
		aggr_member[i]=1u;/*转化为汇聚设置所需的格式*/
		if(aggr_no == 0u)
		{
		    /*为代码方便，故当汇聚组号为0表示删除。该汇聚组成员为欲删除的汇聚组掩码*/
		    delete_aggr_group(i);   
		}
	    }

	}
	if(aggr_no > 0u)/*大于零则为编辑*/
	{
	    if((vtss_common_bool_t)is_group_exist(aggr_no)>0u)/*存在该汇聚组则为修改*/
	    {
		update_aggr_group(aggr_member,aggr_no,aggr_remark);
	    }
	    else/*不存在则为添加*/
	    {
		add_aggr_group(aggr_member,aggr_no,aggr_remark);
	    }
	}
	scmd -> cmd_status = htons(URCP_OK);

    }
    else
    {
	URCP_DEBUG(("urcp set aggr error.port num is %d\n",u_port_num));
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
	ret=URCP_ANY_ERROR;
    }
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#if defined(URCP_CMD_AGGR_CONFIG_GET_REPLY) || defined(URCP_CMD_AGGR_CONFIG_SET_REPLY)
/*
 *汇聚操作回调函数
 *bhou
 */
void urcp_aggr_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_aggr_set_status(reqId, urcp_state);
		break;
        case URCP_STATUS_TIME_OUT:/*超时*/
		urcp_aggr_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_aggr_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_VLAN_CONFIG_GET
/**
 *联动获取port vlan相关信息
 *bhou
 */
static void get_pvlan_config(urcp_vlan_conf_t* pvlan_config)
{
    uint16 start_vlan_no = 0u;
    uint16 u_vlan_no = 0u;
    uint16 u_next_vlan_no=0u;
    uint16 i=0u,j=0u;
    port_bit_mask_t pm=0u;

    start_vlan_no = ntohs(pvlan_config->next_vlan_no);/*起始组号*/
   
    URCP_DEBUG(("%s start_vlan_no:%d\n",__func__,start_vlan_no));
    if(start_vlan_no>0u)/*证明还需要获取vlan信息*/
    {
	j=0u;
	for(i=start_vlan_no-1u; i < NO_OF_PVLANS ; i++)
	{
	    pm = cfg_read_pvlan_member(i);/*成员*/
	
	    URCP_DEBUG(("%s vlan_no:%d vlan_member:%d\n",__func__,i,pm));
	    if(pm>0u)/*该vlan还有成员*/
	    {
		if(j < MAX_URCP_BUFF_VLAN_NUM)/*小于每个urcp 包最大含有的vlan组数*/
		{
		    u_vlan_no = i+1u;
		    pvlan_config->vlan_group[j].vlan_no = htons(u_vlan_no);/*组号*/
		    pvlan_config->vlan_group[j].member = htonl(pm);/*成员*/
		    strncpy(pvlan_config->vlan_group[j].vlan_name,cfg_read_pvlan_name(i),MAX_VLAN_NAME_LEN);/*组名*/
		    j++;
		}
		else
		{/*一个包获取不下。需重新发包获取。且起始组号为i+1*/
		    u_next_vlan_no = i+1u;
		    break;
		}
	    }
	}
    }
    pvlan_config->next_vlan_no = htons(u_next_vlan_no);/*下一次获取起始vlan_no*/
    
    URCP_DEBUG(("%s\n",__func__));
    return;
}
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET
/**
 *联动获取tag vlan
 *bhou
 */
static void get_tag_vlan_config(urcp_vlan_conf_t* qvlan_config)
{
    uint16 start_vlan_no = 0u;
    uint16 u_next_vlan_no=0u;
    uint16 i=0u,j=0u;
    uint32 uindex=0u;
    vtss_vid_t uvid=0u;
    port_bit_mask_t pm=0u;

    start_vlan_no = ntohs(qvlan_config->next_vlan_no);/*起始组号*/
    if(start_vlan_no>0u)/*证明还需要获取vlan信息*/
    {
	j=0u;
	for(i=start_vlan_no; i <= MAX_VID ; i++)
	{
	    uindex = cfg_read_qvlan_table_index(i);
	    if(uindex>0u)
	    {
		uindex--;
		cfg_read_qvlan_table(uindex,&uvid,&pm);/*读取*/
		if(pm>0u)/*含有成员*/
		{
		    if(j < MAX_URCP_BUFF_VLAN_NUM)/*小于每个urcp 包最大含有的vlan组数*/
		    {
			qvlan_config->vlan_group[j].vlan_no = htons(uvid);/*组号*/
			qvlan_config->vlan_group[j].member = htonl(pm);/*成员*/
#if 0
			cfg_read_qvlan_name(uindex,qvlan_config->vlan_group[j].vlan_name); /*组名*/
#endif
			j++;
		    }
		    else
		    {/*一个包获取不下。需重新发包获取。且起始组号为i*/
			u_next_vlan_no = i;
			break;
		    }
		}
	    }
	}
    }
    qvlan_config->next_vlan_no = htons(u_next_vlan_no);/*下一次获取起始vid*/
    
    URCP_DEBUG(("%s\n",__func__));
    return;
}
#endif
#if defined(URCP_CMD_VLAN_CONFIG_GET) || defined(URCP_CMD_QVLAN_PCONF_CONFIG_GET)
/*
 *发送vlan信息
 *bhou
 */
uint16 get_vlan_config(cmd_hdr_t *cmd, uchar* app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;

    uint32 u_misc_type = 0u;
    uint32 u_vlan_type = 0u;
    uint16 start_vlan_no = 0u;
    
    urcp_vlan_conf_t* vlan_config = NULL;/*接受到的vlan config地址*/
    urcp_vlan_conf_t* s_vlan_config = NULL;/*构建回复时，指向的vlan config地址*/
    cmd_hdr_t* scmd = NULL;/*指向回复数据的cmd header*/
    uint16 ret = URCP_OK;

    scmd = (cmd_hdr_t*)(*app_data);

    /*接受到的数据*/
    tmp = (uchar*)cmd;
    vlan_config = (urcp_vlan_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    u_misc_type = ntohl(vlan_config->misc_type);
    start_vlan_no = ntohs(vlan_config->next_vlan_no);

    /*发送数据*/
    tmp = *app_data;
    scmd = (cmd_hdr_t*)tmp;
    /*数据部分*/
    s_vlan_config = (struct urcp_vlan_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(s_vlan_config,0,sizeof(urcp_vlan_conf_t));/*写之前先清除*/
    /*get data*/
    u_vlan_type = cfg_read_vlan_type();
    s_vlan_config->port_num = htonl(PORT_COUNT); /*端口个数，各个设备端口个数可能不一样*/
    s_vlan_config->misc_type = htonl(u_misc_type);/*操作类型*/
    s_vlan_config->vlan_type = htonl(u_vlan_type);/*实际vlan type*/
    s_vlan_config->next_vlan_no = htons(start_vlan_no);/*默认为接受包的起始组号*/

    switch(u_misc_type)
    {
	case MISC_GET_PVLAN:
#ifdef URCP_CMD_VLAN_CONFIG_GET
	    get_pvlan_config(s_vlan_config);
#endif
	    break;
	case MISC_GET_VLAN:
	    if(u_vlan_type == 1u)/*当前蝡ort vlan*/
	    {
#ifdef URCP_CMD_VLAN_CONFIG_GET
		get_pvlan_config(s_vlan_config);
#endif
	    }
	    else
	    {
		if(u_vlan_type == 2u)/*当前为tag vlan*/
		{
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET
		    get_tag_vlan_config(s_vlan_config);
#endif
		}
	    }
	    break;
	default:/*其他类型*/
	    break;
    }
    /*构建回复命令*/
    scmd -> type = htonl(CMD_VLAN_CONFIG);
    /*数据长度及状态*/
    scmd -> cmd_status = htons(URCP_OK);
    scmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_vlan_conf_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return ret;
}
#endif
#ifdef URCP_CMD_VLAN_CONFIG_GET_REPLY
/*
 *接受vlan信息
 *bhou
 */
uint16 receive_vlan_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    uint16 uindex;
    uint32 umember;
    uint32 i = 0u;
    urcp_vlan_conf_t *vlan_config;

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_vlan_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	vlan_config = (urcp_vlan_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/
	vlan_config->port_num = ntohl(vlan_config->port_num);
	vlan_config->vlan_type = ntohl(vlan_config->vlan_type);
	vlan_config->misc_type = ntohl(vlan_config->misc_type);
	vlan_config->next_vlan_no = ntohs(vlan_config->next_vlan_no);
	if(vlan_config->port_num > MAX_URCP_PORT_NUM)/*大于最大限制数，则可能发生内存泄露*/
	{
	    vlan_config->port_num = MAX_URCP_PORT_NUM;
	}
	for( i = 0u; i < MAX_URCP_BUFF_VLAN_NUM ; i++)
	{
	    uindex = vlan_config->vlan_group[i].vlan_no;/*组号或vid*/
	    vlan_config->vlan_group[i].vlan_no = ntohs(uindex);
	    umember = vlan_config->vlan_group[i].member;/*成员*/
	    vlan_config->vlan_group[i].member = ntohl(umember);
	    vlan_config->vlan_group[i].vlan_name[MAX_VLAN_NAME_LEN]='\0';
	    URCP_DEBUG(("%s: vlan_no:%d, vlan_member :0x%x\n", __func__, vlan_config->vlan_group[i].vlan_no,vlan_config->vlan_group[i].member)); 
	}
	urcp_rev_vlan_get_reply(urcp_infor->mac, ntohs(urcp_infor->urcp_header->request_id), vlan_config);/*写入misc buf*/
    }

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_VLAN_CONFIG_SET
/**
 *联动设置port vlan
 *bhou
 */
static int32 set_pvlan_config(urcp_vlan_conf_t*pvlan_config)
{
    uint16 u_vlan_no =0u ;
    uint16 i = 0u;
    port_bit_mask_t umember = 0u;
    char*buff[128];

    int32 ret=0;
    uint32 u_port_num = 0u;
    uint32 u_old_vlan_type = 0u;
    char* pvlan_name=NULL;

    u_port_num = ntohl(pvlan_config->port_num);/*联动操作此设备的设备，传递过来的端口个数*/
    if(u_port_num == PORT_COUNT)/*端口个数相等才可设置*/
    {
	u_old_vlan_type = cfg_read_vlan_type();
	if(u_old_vlan_type != PORT_VLAN)/*现有vlan 类型不为port vlan*/
	{
	    cfg_write_vlan_type(PORT_VLAN);/*切换到port vlan*/
	    if(u_old_vlan_type == TAG_VLAN)
	    {
		reset_qvlan(0u);/*清除tag vlan*/
	    }
	    pvlan_init();
	    sprintf(buff,"%u",PORT_VLAN);
	    nvram_bufset(RT2860_NVRAM,"VlanType",buff);
	}
	u_vlan_no = ntohs(pvlan_config->vlan_group[0].vlan_no);/*组号*/
	umember = ntohl(pvlan_config->vlan_group[0].member);/*成员*/
	pvlan_config->vlan_group[0].vlan_name[MAX_VLAN_NAME_LEN]='\0';/*防止内存泄露*/
	pvlan_name=pvlan_config->vlan_group[0].vlan_name;/*名称*/
	URCP_DEBUG(("urcp set vlan:%d, 0x%08x \n", u_vlan_no,umember));
	//frame_dump1(((uchar*)pvlan_config), ((uint32)sizeof(urcp_vlan_conf_t)));


	if(u_vlan_no == 0u)/*为0，则为删除*/
	{
	    for(i=PORT_NO_START;i<PORT_NO_END;i++)
	    {
		if( (umember & ((uint32)0x01u << (i-1u))) > 0u)
		{
		    /*为代码方便，故当汇聚组号为0表示删除。该汇聚组成员为欲删除的汇聚组掩码*/
		    cfg_write_pvlan_member(i-1u,0u);
		    set_pvlan(i-1u);
		}

	    }
	}
	else/*大于零则为编辑*/
	{
	    /*更新成员*/
	    cfg_write_pvlan_member(u_vlan_no-1u,umember);
	    set_pvlan(u_vlan_no-1u);
	    /*更新组名*/
	    cfg_write_pvlan_name(u_vlan_no-1u,pvlan_name);
	    sprintf(buff,"PVLANName%hu",u_vlan_no);
	    nvram_bufset(RT2860_NVRAM,buff,pvlan_name);
	}
	nvram_commit(RT2860_NVRAM);
    }
    else
    {
	URCP_DEBUG(("urcp set aggr error.port num is %d\n",u_port_num));
	ret=1;
    }
    return ret;
}

/*
 *联动设置vlan
 *bhou
 *
 */
uint16 set_vlan_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    urcp_vlan_conf_t* vlan_config = NULL;
    cmd_hdr_t* scmd = NULL;
    uint16 ret = 0;
    uint32 u_misc_type=0u;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);/*指向回复包的cmd header*/
    tmp = (uchar*)cmd;
    /*复制数据*/
    vlan_config = (urcp_vlan_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*指向接受包的数据部分*/
    u_misc_type = ntohl(vlan_config->misc_type);/*操作类型*/

    if(u_misc_type == MISC_SET_PVLAN)/*设置port vlan*/
    {
	ret+=set_pvlan_config(vlan_config);
    }
    if(ret==0)/*无错误*/
    {
	scmd -> cmd_status = htons(URCP_OK);
    }
    else/*有错误*/
    {
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
    }
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}

/*
 *vlan操作回调函数
 *bhou
 */
void urcp_vlan_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_vlan_set_status(reqId, urcp_state);
		break;
        case URCP_STATUS_TIME_OUT:/*超时*/
		urcp_vlan_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_vlan_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET
/*
 *发送tag vlan端口设置
 *bhou
 */
uint16 get_qvlan_pconf_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_qvlan_pconf_conf_t* qvlan_pconf_config;
    uint32 i=0u;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;
    /*构建回复命令*/
    cmd -> type = htonl(CMD_QVLAN_PCONF_CONFIG);
    /*数据部分*/
    qvlan_pconf_config = (struct _urcp_qvlan_pconf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    memset(qvlan_pconf_config,0,sizeof(urcp_qvlan_pconf_conf_t));
    /*get*/
    qvlan_pconf_config->port_num = htonl(PORT_COUNT);/*端口数*/ 
    for(i=0u;i<PORT_COUNT;i++)
    {
	qvlan_pconf_config->pconf_group[i].vlan_aware = htonl(cfg_read_qvlan_aware(i));/*vlan设备*/
	qvlan_pconf_config->pconf_group[i].frame_type = htonl(cfg_read_qvlan_frame_type(i));/*帧类型*/
	qvlan_pconf_config->pconf_group[i].pvid = htons((uint16)cfg_read_qvlan_pvid(i));/*pvid*/
    }
    
    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_qvlan_pconf_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_QVLAN_PCONF_CONFIG_GET_REPLY
/*
 *收到信息
 *bhou
 */
uint16 receive_qvlan_pconf_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    uint32 i=0u;
    urcp_qvlan_pconf_conf_t *qvlan_pconf_config;

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_qvlan_pconf_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	qvlan_pconf_config = (urcp_qvlan_pconf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
	qvlan_pconf_config->port_num = ntohl(qvlan_pconf_config->port_num);/*端口个数*/
	if(qvlan_pconf_config->port_num > MAX_URCP_PORT_NUM)/*大于最大限制数，则可能发生内存泄露*/
	{
	    qvlan_pconf_config->port_num = MAX_URCP_PORT_NUM;
	}
	for(i=0u;i<qvlan_pconf_config->port_num;i++)
	{
	    qvlan_pconf_config->pconf_group[i].vlan_aware = ntohl(qvlan_pconf_config->pconf_group[i].vlan_aware);/*vlan设备*/
	    qvlan_pconf_config->pconf_group[i].frame_type = ntohl(qvlan_pconf_config->pconf_group[i].frame_type);/*帧类型*/
	    qvlan_pconf_config->pconf_group[i].pvid = ntohs(qvlan_pconf_config->pconf_group[i].pvid);/*pvid*/
	}
	urcp_rev_qvlan_pconf_get_reply(urcp_infor->mac, ntohs(urcp_infor->urcp_header->request_id),qvlan_pconf_config);/*add to misc buffer*/
    }

    return URCP_OK;
}

/*
 *回调函数
 *bhou
 */
void urcp_qvlan_pconf_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_qvlan_pconf_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_TIME_OUT:/*超时*/
		urcp_qvlan_pconf_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)/*预留项目。目前不用设置。只用读*/
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_qvlan_pconf_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*默认不更改misc buffer状态*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET
/*
 *发送rstp lacp enable
 *bhou
 */
uint16 get_rstp_lacp_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_rstp_lacp_conf_t* rstp_lacp_config;
    uint32 i=0u;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;
    /*构建回复命令*/
    cmd -> type = htonl(CMD_RSTP_LACP_CONFIG);

    /*数据部分*/
    rstp_lacp_config = (struct _urcp_rstp_lacp_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    memset(rstp_lacp_config,0,sizeof(urcp_rstp_lacp_conf_t));
    /*get*/
    rstp_lacp_config->port_num = htonl(PORT_COUNT);/*端口数*/ 
    for(i=0u;i<PORT_COUNT;i++)
    {
#if FEATURE_RSTP
	if(cfg_read_rstp_enable(i)>0u)/*get rstp enable*/
	{
	    rstp_lacp_config->rstp_enable |= (uint32)(1u<<i);
	}
#endif
#if FEATURE_LACP
	if(cfg_read_lacp_enable(i)>0u)/*get lacp enable*/
	{
	    rstp_lacp_config->lacp_enable |= (uint32)(1u<<i);
	}
#endif
    }
    rstp_lacp_config->rstp_enable = htonl(rstp_lacp_config->rstp_enable);/*转化字节序*/
    rstp_lacp_config->lacp_enable = htonl(rstp_lacp_config->lacp_enable);/*转化字节序*/
    
    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_rstp_lacp_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_RSTP_LACP_CONFIG_GET_REPLY
/*
 *收到信息
 *bhou
 */
uint16 receive_rstp_lacp_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    urcp_rstp_lacp_conf_t *rstp_lacp_config;

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_rstp_lacp_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	rstp_lacp_config = (urcp_rstp_lacp_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
	rstp_lacp_config->port_num = ntohl(rstp_lacp_config->port_num);/*端口个数*/
	rstp_lacp_config->rstp_enable = ntohl(rstp_lacp_config->rstp_enable);/*rstp enable*/
	rstp_lacp_config->lacp_enable = ntohl(rstp_lacp_config->lacp_enable);/*lacp enable*/
	if(rstp_lacp_config->port_num > MAX_URCP_PORT_NUM)/*大于最大限制数，则可能发生内存泄露*/
	{
	   rstp_lacp_config->port_num = MAX_URCP_PORT_NUM;
	}
	urcp_rev_rstp_lacp_get_reply(urcp_infor->mac, ntohs(urcp_infor->urcp_header->request_id), rstp_lacp_config);/*add to misc buffer*/
    }

    return URCP_OK;
}

/*
 *回调函数
 *bhou
 */
void urcp_rstp_lacp_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_rstp_lacp_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_TIME_OUT:/*超时*/
		urcp_rstp_lacp_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_OK:/*预留项目。目前不用设置。只用读*/
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_rstp_lacp_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*默认不更改misc buffer状态*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET
/*
 *发送端口管理信息
 *bhou
 */
uint16 get_port_conf_config(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info)
{
    uint16  i = 0u;
    uint32  j = 0u;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_port_conf_conf_t* _port_config;
    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_PORT_CONFIG);
    /*数据部分*/
    _port_config = (struct urcp_port_conf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(_port_config,0,sizeof(urcp_port_conf_conf_t));/*写之前先清除*/
    /*get data*/
    _port_config->port_num = htonl(PORT_COUNT); /*端口个数，各个设备端口个数可能不一样*/

    shm_get_port_configure();/*获取最新端口信息*/
    for( i = 0u; i < PORT_COUNT; i++)
    {
	j = api_state->io.buffer->port_config[i].selected_mode;
	_port_config->port_conf_group[i].selected_mode = htonl(j);/*设置模式*/
        j = api_state->io.buffer->port_config[i].max_frame;
	_port_config->port_conf_group[i].max_frame = htonl(j);/*允许最大帧*/
	if(api_state->io.buffer->port_config[i].rate_limit > 0)
	{
	    _port_config->port_conf_group[i].rate_limit = 1;/*流控*/
	}
	if(api_state->io.buffer->port_config[i].protect > 0)
	{
	    _port_config->port_conf_group[i].protect =1 ;/*端口保护*/
	}
	if(api_state->io.buffer->port_config[i].flooding > 0)
	{
	    _port_config->port_conf_group[i].flooding = 1;/*泛洪*/
	}
	strncpy(_port_config->port_conf_group[i].link_status , api_state->io.buffer->port_config[i].link_status ,MAX_STATUS_SIZE);/*链路状态*/
	strncpy(_port_config->port_conf_group[i].remark , cfg_read_port_remark(i) ,MAX_PORT_REMARK_LEN);/*组名称*/
    }
    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_port_conf_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_GET_REPLY
/*
 *接受端口设置信息
 *bhou
 */
uint16 receive_port_conf_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    uint32 i = 0u;
    urcp_port_conf_conf_t *_port_config;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_port_conf_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	_port_config = (urcp_port_conf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/
	_port_config->port_num = ntohl(_port_config->port_num);
	if(_port_config->port_num > MAX_URCP_PORT_NUM)/*大于最大限制数，则可能发生内存泄露*/
	{
	   _port_config->port_num = MAX_URCP_PORT_NUM;
	}
	for( i = 0u; i < _port_config->port_num; i++)
	{
	    _port_config->port_conf_group[i].selected_mode = ntohl(_port_config->port_conf_group[i].selected_mode);/*设置模式*/
	    _port_config->port_conf_group[i].max_frame = ntohl(_port_config->port_conf_group[i].max_frame);/*允许最大帧*/
	    _port_config->port_conf_group[i].link_status[MAX_STATUS_SIZE-1u]='\0';/*链路状态*/
	    _port_config->port_conf_group[i].remark[MAX_PORT_REMARK_LEN-1u]='\0';/*组名称*/
	}
	init_port_conf_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)_port_config);/*写入misc buff*/
    }
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_PORT_CONFIG_SET
/*
 *联动设置端口设置
 *bhou
 *
 */
uint16 set_port_conf_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 i = 0u;

    uint32 u_port_num = 0u;
    urcp_port_conf_conf_t* _port_config = NULL;
    cmd_hdr_t* scmd = NULL;

    uint16 ret = 0;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    /*长度信息*/
    tmp = (uchar*)cmd;
    /*复制数据*/
    _port_config = (urcp_port_conf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    u_port_num = ntohl(_port_config->port_num);/*联动操作此设备的设备，传递过来的端口个数*/
    if(u_port_num == PORT_COUNT)/*端口个数相等才可设置*/
    {
	for( i = 0u; i < PORT_COUNT; i++)
	{
	    api_state->io.buffer->port_config[i].selected_mode = (int)ntohl(_port_config->port_conf_group[i].selected_mode) ;/*设置模式*/
	    api_state->io.buffer->port_config[i].max_frame = (int)ntohl(_port_config->port_conf_group[i].max_frame);/*允许最大帧*/
	    api_state->io.buffer->port_config[i].rate_limit = 0; /*流控*/
	    if( _port_config->port_conf_group[i].rate_limit> 0)
	    {
		api_state->io.buffer->port_config[i].rate_limit = 1; /*流控*/
	    }
	    api_state->io.buffer->port_config[i].protect = 0;
	    if(_port_config->port_conf_group[i].protect> 0)
	    {
		api_state->io.buffer->port_config[i].protect = 1 ;/*端口保护*/
	    }
	    api_state->io.buffer->port_config[i].flooding = 0;
	    if(_port_config->port_conf_group[i].flooding > 0)
	    {
		 api_state->io.buffer->port_config[i].flooding = 1;/*泛洪*/
	    }
	    cfg_write_port_remark((uint32)i,_port_config->port_conf_group[i].remark);/*备注*/
	}
	scmd -> cmd_status = htons(URCP_OK);
    }
    else
    {
	URCP_DEBUG(("urcp set port conf error.port num is %d\n",u_port_num));
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
	ret=URCP_ANY_ERROR;
    }
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#if defined(URCP_CMD_PORT_CONFIG_GET_REPLY) || defined(URCP_CMD_PORT_CONFIG_SET_REPLY)
/*
 * 联动端口设置回调函数
 *bhou
 */
void urcp_port_conf_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_port_conf_set_status(reqId, urcp_state);
		break;
        case URCP_STATUS_TIME_OUT:/*超时*/
		urcp_port_conf_set_status(reqId, urcp_state);
		break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_port_conf_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET 
/*
 *发送端口限速配置信息
 *bhou
 */
uint16 get_qos_rate_limit_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32  i = 0u;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_qos_rate_limit_conf_t* conf_buff;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_QOS_RATE_LIMIT_CONFIG);
    /*数据部分*/
    conf_buff = (struct urcp_qos_rate_limit_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(conf_buff,0,sizeof(urcp_qos_rate_limit_conf_t));/*写之前先清除*/
    /*get data*/
    conf_buff->port_num = htonl(PORT_COUNT); /*端口个数，各个设备端口个数可能不一样*/

#if FEATURE_QOS
    conf_buff->rate_unit = htonl(cfg_read_qos_rate_unit());/*颗粒度*/
    for( i = 0u; i < PORT_COUNT; i++)
    {
	conf_buff->qos_rate_limit_group[i].policer = htonl(cfg_read_port_qos_policer(i));/*ingress*/
	conf_buff->qos_rate_limit_group[i].shaper = htonl(cfg_read_port_qos_shaper(i));/*egress*/
    }
#endif
    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_qos_rate_limit_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY
/*
 *接受端口限速信息
 *bhou
 */
uint16 receive_qos_rate_limit_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    uint32 i = 0u;
    urcp_qos_rate_limit_conf_t *conf_buff;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_qos_rate_limit_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	conf_buff = (urcp_qos_rate_limit_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/
	conf_buff->port_num = ntohl(conf_buff->port_num);
	if(conf_buff->port_num > MAX_URCP_PORT_NUM)/*大于最大限制数，则可能发生内存泄露*/
	{
	    conf_buff->port_num = MAX_URCP_PORT_NUM;
	}
	conf_buff->rate_unit = ntohl(conf_buff->rate_unit);/*颗粒度*/
	for( i = 0u; i < conf_buff->port_num; i++)
	{
	    conf_buff->qos_rate_limit_group[i].policer = ntohl(conf_buff->qos_rate_limit_group[i].policer);/*ingress*/
	    conf_buff->qos_rate_limit_group[i].shaper = ntohl(conf_buff->qos_rate_limit_group[i].shaper);/*egress*/
	}
	init_qos_rate_limit_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(conf_buff));/*写入misc buff*/
    }
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET
/*
 *联动设置速率限制
 *bhou
 *
 */
uint16 set_qos_rate_limit_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint32 i = 0u;

    uint32 u_port_num = 0u;
    urcp_qos_rate_limit_conf_t* conf_buff = NULL;
    cmd_hdr_t* scmd = NULL;

    uint16 ret = URCP_OK;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    /*长度信息*/
    tmp = (uchar*)cmd;
    /*复制数据*/
    conf_buff = (urcp_qos_rate_limit_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);
    u_port_num = ntohl(conf_buff->port_num);/*联动操作此设备的设备，传递过来的端口个数*/
    if(u_port_num == PORT_COUNT)/*端口个数相等才可设置*/
    {
#if FEATURE_QOS
	for( i = 0u; i < PORT_COUNT; i++)
	{
	    cfg_write_port_qos_policer(i,ntohl(conf_buff->qos_rate_limit_group[i].policer));/*ingress*/
	    cfg_write_port_qos_shaper(i,ntohl(conf_buff->qos_rate_limit_group[i].shaper));/*egress*/
	}
	cfg_write_qos_rate_unit(ntohl(conf_buff->rate_unit));/*颗粒度*/
	web_qos_ratelimit_config();/*限速生效*/
#endif
	scmd -> cmd_status = htons(URCP_OK);
    }
    else
    {
	URCP_DEBUG(("urcp set port conf error.port num is %d\n",u_port_num));
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
	ret=URCP_ANY_ERROR;
    }
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#if defined(URCP_CMD_QOS_RATE_LIMIT_CONFIG_GET_REPLY) || defined (URCP_CMD_QOS_RATE_LIMIT_CONFIG_SET_REPLY)
/*
 * 联动速率限制回调函数
 *bhou
 */
void urcp_qos_rate_limit_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_qos_rate_limit_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_qos_rate_limit_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_qos_rate_limit_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET
/*
 * 发送风暴抑制
 *bhou
 */
uint16 get_storm_rate_limit_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_storm_rate_limit_conf_t* conf_buff;
    tmp = (*app_data);
    cmd = (cmd_hdr_t*)tmp;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_STORM_RATE_LIMIT_CONFIG);
    /*数据部分*/
    conf_buff = (struct urcp_storm_rate_limit_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(conf_buff,0,sizeof(urcp_storm_rate_limit_conf_t));/*写之前先清除*/
    /*get data*/

    conf_buff->icmp_rate = htonl(cfg_read_storm_qos_icmp());/*icmp*/
    conf_buff->bc_rate = htonl(cfg_read_storm_qos_bc());/*广播*/
    conf_buff->mc_rate = htonl(cfg_read_storm_qos_mc());/*组播*/
    conf_buff->uc_rate = htonl(cfg_read_storm_qos_uc());/*单播*/
    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_storm_rate_limit_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY
/*
 *接受风暴抑制
 *bhou
 */
uint16 receive_storm_rate_limit_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    urcp_storm_rate_limit_conf_t *conf_buff;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_storm_rate_limit_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	conf_buff = (urcp_storm_rate_limit_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/

#if FEATURE_QOS
	conf_buff->icmp_rate = ntohl(conf_buff->icmp_rate);/*icmp*/
	conf_buff->bc_rate = ntohl(conf_buff->bc_rate);/*广播*/
	conf_buff->mc_rate = ntohl(conf_buff->mc_rate);/*组播*/
	conf_buff->uc_rate = ntohl(conf_buff->uc_rate);/*单播*/
#endif

	init_storm_rate_limit_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(conf_buff));/*写入misc buff*/
    }
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET
/*
 *联动设置风暴抑制
 *bhou
 *
 */
uint16 set_storm_rate_limit_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    urcp_storm_rate_limit_conf_t* conf_buff = NULL;
    cmd_hdr_t* scmd = NULL;

    uint16 ret = URCP_OK;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    /*长度信息*/
    tmp = (uchar*)cmd;
    /*复制数据*/
    conf_buff = (urcp_storm_rate_limit_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

#if FEATURE_QOS
    /*设置*/
    cfg_write_storm_qos_icmp(ntohl(conf_buff->icmp_rate));/*icmp*/
    cfg_write_storm_qos_bc(ntohl(conf_buff->bc_rate));/*广播*/
    cfg_write_storm_qos_mc(ntohl(conf_buff->mc_rate));/*组播*/
    cfg_write_storm_qos_uc(ntohl(conf_buff->uc_rate));/*单播*/
    web_storm_config();/*生效*/
#endif

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#if defined(URCP_CMD_STORM_RATE_LIMIT_CONFIG_GET_REPLY) || defined(URCP_CMD_STORM_RATE_LIMIT_CONFIG_SET_REPLY)
/*
 * 联动风暴抑制回调函数
 *bhou
 */
void urcp_storm_rate_limit_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_storm_rate_limit_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_storm_rate_limit_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_storm_rate_limit_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
/*
 *构建联动单命令操作，回复包中命令格式 
 *bhou
 *
 */
uint16 build_cmd_alone_reply_cmd(cmd_hdr_t *cmd, uchar** app_data)
{
    //printf("\n< %s %d > end\n",__func__,__LINE__);
    cmd_hdr_t* scmd = NULL;

    scmd = (cmd_hdr_t*)(*app_data);

    scmd -> cmd_status = htons(URCP_OK);
     /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    URCP_DEBUG(("%s\n",__func__));
    //printf("\n< %s %d > end\n",__func__,__LINE__);
    return URCP_OK;
}

#ifdef URCP_WEB
/*
 *联动单命令操作回调函数
 *bhou
 */
void urcp_cmd_alone_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_cmd_alone_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_cmd_alone_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_cmd_alone_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_SAFEBIND_CONFI_SET_REPLY
/**
 *绑定前的操作
 *KeHuatao
 *
 */
static void set_safebind_cb(void)
{
    /*设置扫描网段*/
    cfg_write_scanf_ip(cfg_read_sys_ip());
    cfg_write_scanf_mask(cfg_read_sys_mask());

    arp_flush();/*清空动态arp信息*/

    return;
}
#endif
#ifdef URCP_CMD_SAFEBIND_CONFI_SET
 /***
 * 联动安全绑定
 * bhou
 */
extern void set_safebind_config(void)
{
    URCP_DEBUG(("%s\n",__func__));
    /*先发包扫描*/
#if 1
    send_arp_request(set_safebind_cb, safe_bind);
#else
    arp_request();/*发包*/
    arp_scanf_timer = 0;/*开始计时*/
#endif
    return;
}
/***
 *安全绑定
 *bhou
 */
static void safe_bind(void)
{
    URCP_DEBUG(("%s\n",__func__));
    uint32 is_change = 0u;
    is_change |= mac_port_all_bind();/*nac port 全绑定*/
    is_change |= ip_port_all_bind();/*ip port全绑定*/
    cfg_clear_arp_scanf();/*清空查询条件*/
 
    if(is_change>0u)
    {
	nvram_commit(RT2860_NVRAM);
    }
    return;
}


/***
 *安全绑定计时函数。每次发送arp包后开始计时
 *bhou
 */
extern void safe_bind_timer(void)
{
    if((arp_scanf_timer >= 0))
    {
	if ( arp_scanf_timer < ARP_SCANF_TIMEOUT)
	{
	    arp_scanf_timer++;
	}
	else 
	{
	    safe_bind();/*安全绑定*/
	    arp_scanf_timer = -1;/*倒计时结束*/
	}
    }
    return;
}
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET
/*
 *发送系统设置信息
 *bhou
 */
uint16 get_sys_conf_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_sys_conf_conf_t* conf_buff;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_SYS_CONF_CONFIG);
    /*数据部分*/
    conf_buff = (struct urcp_sys_conf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(conf_buff,0,sizeof(urcp_sys_conf_conf_t));/*写之前先清除*/

    /*get data*/
#if FEATURE_DHCPRELAY
    conf_buff->dhcp_relay_enable = htonl(cfg_read_sys_dhcp_relay());/*dhcp中继*/
#endif

#if FEATURE_DHCPCLIENT
    conf_buff->dhcp_client_enable = htonl(cfg_read_sys_dhcp_client());/*dhcp客户端*/
#endif

    conf_buff->ip_addr = htonl(cfg_read_sys_ip());/*系统ip*/
    conf_buff->ip_mask = htonl(cfg_read_sys_mask());/*ip掩码*/
    conf_buff->gw_ip = htonl(cfg_read_sys_static_gw());/*系统网关*/
    conf_buff->cpu_vid = htonl(cfg_read_sys_cpu_vid());/*cpu vid*/
    conf_buff->age_time = htonl(cfg_read_sys_mac_agetime());/*老化时间*/
    strncpy(conf_buff->dev_name, cfg_read_sys_dev_name(), MAX_DEV_NAME_LEN);/*设备名*/
    strncpy(conf_buff->password, cfg_read_sys_password(), MAX_PASSWORD_LEN);/*密码*/
    conf_buff->arp_enable = htonl(cfg_read_sys_arp());/*是否开启arp*/

    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_sys_conf_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
} 
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_GET_REPLY
/*
 *接受系统设置信息
 *bhou
 */
uint16 receive_sys_conf_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    urcp_sys_conf_conf_t *conf_buff;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_sys_conf_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	conf_buff = (urcp_sys_conf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/

	/*转化字节序*/
	conf_buff->dhcp_relay_enable = ntohl(conf_buff->dhcp_relay_enable);/*dhcp中继*/
	conf_buff->dhcp_client_enable = ntohl(conf_buff->dhcp_client_enable);/*dhcp客户端*/
	conf_buff->ip_addr = ntohl(conf_buff->ip_addr);/*系统ip*/
	conf_buff->ip_mask = ntohl(conf_buff->ip_mask);/*ip掩码*/
	conf_buff->gw_ip = ntohl(conf_buff->gw_ip);/*系统网关*/
	conf_buff->cpu_vid = ntohl(conf_buff->cpu_vid);/*cpu vid*/
	conf_buff->age_time = ntohl(conf_buff->age_time);/*老化时间*/
	conf_buff->dev_name[MAX_DEV_NAME_LEN - 1u] = '\0';/*设备名*/
	conf_buff->password[MAX_PASSWORD_LEN - 1u] = '\0';/*系统密码*/
	conf_buff->arp_enable = ntohl(conf_buff->arp_enable);/*是否开启arp*/

	init_sys_conf_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(conf_buff));/*写入misc buff*/
    }
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_SYS_CONF_CONFIG_SET
/***
 *联动系统设置,ip相关设置函数
 *bhou
 */
static uint32 set_sys_conf_config_set_ip(urcp_sys_conf_conf_t* conf_buff)
{
    uint32 changetag = 0u;
    uint32 is_set_gw = 0u;
    char strbuf[32];
    memset(strbuf, 0, sizeof(strbuf));

#if FEATURE_DHCPCLIENT
    /*dhcp客户端*/
    if(conf_buff->dhcp_client_enable != cfg_read_sys_dhcp_client())
    {
	cfg_write_sys_dhcp_client(conf_buff->dhcp_client_enable);
	changetag |=((uint32)0x01u << SYS_DHCP_CLIENT);
	init_dhcp_client(conf_buff->dhcp_client_enable);/*重新初始化dhcp*/
	if(conf_buff->dhcp_client_enable == 0u)/*关闭dhcp则重设ip*/
	{
	    init_ip();/*切换到静态ip模式*/
	}
    }

    if(conf_buff->dhcp_client_enable == 0u)/*未开启*/
    {
#endif
	/*系统ip设置*/
	if(conf_buff->ip_addr != cfg_read_sys_ip())
	{/*相对当前数据有改变*/
	    if (set_ip(ETH_NAME, htonl(conf_buff->ip_addr)) == 0)
	    {
		cfg_write_sys_ip(conf_buff->ip_addr);
		changetag |=((uint32)0x01u << SYS_IP);
		is_set_gw = 1u;/*ip变化需重设网关*/
	    }
	}

	/*系统ip掩码*/
	if(conf_buff->ip_mask!= cfg_read_sys_mask())
	{/*相对当前数据有改变*/
	    /*set mask*/
	    if (set_mask(ETH_NAME, htonl(conf_buff->ip_mask)) == 0)
	    {
		cfg_write_sys_mask(conf_buff->ip_mask);
		changetag |=((uint32)0x01u << SYS_MASK);
		is_set_gw = 1u;/*掩码变化需重设网关*/
	    }
	}

	/*网关*/
	if ( conf_buff->gw_ip != cfg_read_sys_static_gw())
	{/*相对当前数据有改变*/
	    cfg_write_sys_static_gw(conf_buff->gw_ip);/*写静态网关*/
	    changetag |=((uint32)0x01u << SYS_GW);
	    is_set_gw = 1u;
	}

	/*网关生效函数*/
	if(is_set_gw > 0u)
	{
	    /*set gw*/
	    set_gw(int2ip(conf_buff->gw_ip, strbuf));
	    cfg_write_sys_gw_ip(conf_buff->gw_ip);/*写实际网关地址（开启dhcp时为自动获得到的ip地址）*/
	}

#if FEATURE_DHCPCLIENT
    }
#endif

    return changetag;
}

/*
 *联动设置系统信息
 *bhou
 *
 */
uint16 set_sys_conf_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint32 changetag = 0u;

    urcp_sys_conf_conf_t* conf_buff = NULL;
    cmd_hdr_t* scmd = NULL;

    uint16 ret = URCP_OK;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    /*长度信息*/
    tmp = (uchar*)cmd;

    /*指向数据部分*/
    conf_buff = (urcp_sys_conf_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    /*转换字节序*/
    conf_buff->dhcp_relay_enable = ntohl(conf_buff->dhcp_relay_enable);/*dhcp中继*/
    conf_buff->dhcp_client_enable = ntohl(conf_buff->dhcp_client_enable);/*dhcp客户端*/
    conf_buff->ip_addr = ntohl(conf_buff->ip_addr);/*系统ip*/
    conf_buff->ip_mask = ntohl(conf_buff->ip_mask);/*ip掩码*/
    conf_buff->gw_ip = ntohl(conf_buff->gw_ip);/*系统网关*/
    conf_buff->cpu_vid = ntohl(conf_buff->cpu_vid);/*cpu vid*/
    conf_buff->age_time = ntohl(conf_buff->age_time);/*老化时间*/
    conf_buff->dev_name[MAX_DEV_NAME_LEN - 1u] = '\0';/*设备名*/
    conf_buff->password[MAX_PASSWORD_LEN - 1u] = '\0';/*系统密码*/
    conf_buff->arp_enable = ntohl(conf_buff->arp_enable);/*是否开启arp*/


    /*设置*/
#if FEATURE_DHCPRELAY
    /*dhcp中继*/
    if(conf_buff->dhcp_relay_enable != cfg_read_sys_dhcp_relay())
    {
	/*改变*/
	cfg_write_sys_dhcp_relay(conf_buff->dhcp_relay_enable);
	changetag |= ((uint32)0x01u << SYS_DHCP_RELAY);
    }
#endif

    /*ip相关*/
    changetag |= set_sys_conf_config_set_ip(conf_buff);

    /*cpu vid*/
    if (cfg_read_sys_cpu_vid() != conf_buff->cpu_vid)
    {
	/*write 共享内存*/
	cfg_write_sys_cpu_vid(conf_buff->cpu_vid);
	changetag |= ((uint32)0x01u << SYS_CPU_VID);
    }

    /*mac老化时间*/
    if (cfg_read_sys_mac_agetime() != conf_buff->age_time)
    {
	/*write 共享内存*/
	cfg_write_sys_mac_agetime(conf_buff->age_time);
	changetag |= ((uint32)0x01u << SYS_MAC_AGE);
    }

    /*设备名*/
    if (strcmp(cfg_read_sys_dev_name(), conf_buff->dev_name) != 0)
    {
	cfg_write_sys_dev_name(conf_buff->dev_name);
	changetag |= ((uint32)0x01u << SYS_DEV);
    }

    /*修改密码*/
    if (strcmp(cfg_read_sys_password(), conf_buff->password) != 0)
    {
	/*write 共享内存*/
	cfg_write_sys_password(conf_buff->password);
	set_sys_pass(conf_buff->password);
	changetag |= ((uint32)0x01u << SYS_PASS);
    }

    /*是否开启arp*/
    if ( conf_buff->arp_enable != cfg_read_sys_arp())
    {
	cfg_write_sys_arp(conf_buff->arp_enable);
	changetag |= ((uint32)0x01u << SYS_ARP);
    }

    /*设置生效*/
    set_sys_conf(changetag);/*相关性设置，以及写flash缓存*/
    nvram_commit(RT2860_NVRAM);
    /*end设置*/

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#if defined(URCP_CMD_SYS_CONF_CONFIG_GET_REPLY) || defined(URCP_CMD_SYS_CONF_CONFIG_SET_REPLY)
/*
 * 联动系统设置回调函数
 * bhou
 */
void urcp_sys_conf_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_sys_conf_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_sys_conf_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_sys_conf_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET
/*
 *发送ip过滤信息
 *bhou
 */
uint16 get_ip_filter_config(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_ip_filter_conf_t* conf_buff;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;
    uint32 i = 0u;
    uint32 src_ip = 0u;
    uint32 src_mask = 0u;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_IP_FILTER_CONFIG);
    /*数据部分*/
    conf_buff = (struct urcp_ip_filter_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(conf_buff,0,sizeof(urcp_ip_filter_conf_t));/*写之前先清除*/
    /*get data*/
    conf_buff->port_num = htonl(PORT_COUNT);/*端口个数*/
    for(i = 0u; i< PORT_COUNT; i++)
    {
	cfg_read_filter_sip_info(i, &src_ip, &src_mask);
	conf_buff->filter_src_ip[i] = htonl(src_ip);/*ip过滤源地址*/
	conf_buff->filter_src_ip_mask[i] = htonl(src_mask);/*ip过滤源地址掩码*/
    }

    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_ip_filter_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
} 
#endif
#ifdef URCP_CMD_IP_FILTER_CONFIG_GET_REPLY
/*
 *接受ip过滤信息
 *bhou
 */
uint16 receive_ip_filter_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    urcp_ip_filter_conf_t *conf_buff;
    misc_opt_assist misc_buff;
    uint32 i = 0u;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_ip_filter_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	conf_buff = (urcp_ip_filter_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/

	/*转化字节序*/
	conf_buff->port_num = ntohl(conf_buff->port_num);/*端口个数*/
	if(conf_buff->port_num > MAX_URCP_PORT_NUM)
	{
	    conf_buff->port_num = MAX_URCP_PORT_NUM;
	}
	for(i = 0u; i< conf_buff->port_num; i++)
	{
	    conf_buff->filter_src_ip[i] = ntohl(conf_buff->filter_src_ip[i]);/*ip过滤源地址*/
	    conf_buff->filter_src_ip_mask[i] = ntohl(conf_buff->filter_src_ip_mask[i]);/*ip过滤源地址掩码*/
	}

	init_ip_filter_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(conf_buff));/*写入misc buff*/
    }
    return URCP_OK;

}
#endif
#if defined(URCP_CMD_IP_FILTER_CONFIG_GET_REPLY)
/*
 * 联动ip过滤回调函数
 * bhou
 */
void urcp_ip_filter_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_ip_filter_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_ip_filter_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_ip_filter_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET
/*
 *发送系统信息
 *bhou
 */
uint16 get_sys_info_config (cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* cmd;
    urcp_sys_info_conf_t* conf_buff;

    tmp = *app_data;
    cmd = (cmd_hdr_t*)tmp;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_SYS_INFO_CONFIG);
    /*数据部分*/
    conf_buff = (struct urcp_sys_info_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);

    memset(conf_buff,0,sizeof(urcp_sys_info_conf_t));/*写之前先清除*/

    /*get data*/
    getSystemNowTime(conf_buff->now_time);/*系统时间*/
    getSystemSysUpTime(conf_buff->up_time);/*系统运行时间*/

    getOuterFlashStat(&(conf_buff->outer_flash_total_space), &(conf_buff->outer_flash_unused_space));/*外接闪存使用状态*/

    getSystemCpu(&(conf_buff->cpu_rate));/*cpu利用率*/
    conf_buff->cpu_rate = htonl(conf_buff->cpu_rate);

    getSystemMemory(&(conf_buff->mem_rate));/*内存利用率*/
    conf_buff->mem_rate = htonl(conf_buff->mem_rate);

    get_ip(ETH_NAME, &(conf_buff->ip_addr));/*系统ip*/
    get_mask(ETH_NAME, &(conf_buff->ip_mask));/*ip掩码*/
    conf_buff->gw_ip = htonl(cfg_read_sys_gw_ip());/*系统网关*/
    memcpy(conf_buff->mac_addr, cfg_read_sys_mac(), MAC_LEN);/*mac地址*/

    conf_buff->sys_serialno = htonl(cfg_read_sys_serialno());/*系列号*/
    strncpy(conf_buff->sw_version, cfg_read_sys_sw_version(), SW_VERSION_SIZE);/*软件版本*/

    conf_buff->cpu_vid = htonl(cfg_read_sys_cpu_vid());/*cpu vid*/
    strncpy(conf_buff->dev_name, cfg_read_sys_dev_name(), MAX_DEV_NAME_LEN);/*设备名*/


    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> length = sizeof(cmd_hdr_t)+sizeof(urcp_sys_info_conf_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_SYS_INFO_CONFIG_GET_REPLY
/*
 *接受系统信息
 *bhou
 */
uint16 receive_sys_info_config(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    uint32 i = 0u;
    urcp_sys_info_conf_t *conf_buff;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_sys_info_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	conf_buff = (urcp_sys_info_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/

	/*转化字节序等处理包信息*/
	i = sizeof(conf_buff->now_time) - 1u;
	conf_buff->now_time[i] = '\0';/*系统时间*/
	i = sizeof(conf_buff->up_time) - 1u;
	conf_buff->up_time[i] = '\0';/*系统运行时间*/

	conf_buff->cpu_rate = ntohl(conf_buff->cpu_rate);/*cpu利用率*/
	conf_buff->mem_rate = ntohl(conf_buff->mem_rate);/*内存利用率*/

	conf_buff->ip_addr = ntohl(conf_buff->ip_addr);/*系统ip*/
	conf_buff->ip_mask = ntohl(conf_buff->ip_mask);/*ip掩码*/
	conf_buff->gw_ip = ntohl(conf_buff->gw_ip);/*系统网关*/

	conf_buff->sys_serialno = ntohl(conf_buff->sys_serialno);/*系列号*/
	conf_buff->sw_version[SW_VERSION_SIZE - 1u] = '\0';/*软件版本*/

	conf_buff->cpu_vid = ntohl(conf_buff->cpu_vid);/*cpu vid*/
	conf_buff->dev_name[MAX_DEV_NAME_LEN - 1u] = '\0';/*设备名*/

	init_sys_info_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(conf_buff));/*写入misc buff*/
    }
    return URCP_OK;
}
#endif
#if defined(URCP_CMD_SYS_INFO_CONFIG_GET_REPLY)
/*
 * 联动获取系统信息回调函数
 * bhou
 */
void urcp_sys_info_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_sys_info_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_sys_info_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_sys_info_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#ifdef URCP_CMD_PASSWORD_CONFIG_SET_REPLY
/*
 *联动密码修改回调函数
 *ChenShiyu
 */
void urcp_password_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_password_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_password_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_password_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif
#if 0
/*
 *联动密码配置函数
 *ChenShiyu
 *
 */
uint16 set_password_config(cmd_hdr_t *cmd, uchar** app_data)
{
	uchar buf[1];
	uchar *tmp = buf;
	uchar dev_passwd[MAX_PASSWORD_LEN];
	uchar* _passwd_config = NULL;
	cmd_hdr_t* scmd = NULL;
	uint16 ret = URCP_OK;

	URCP_DEBUG(("%s\n",__func__));
	scmd = (cmd_hdr_t*)*app_data;
	/*长度信息*/
	tmp = (uchar*)cmd;
	/*复制数据*/
	_passwd_config = (uchar*)(&tmp[sizeof(cmd_hdr_t)]);
	strncpy(dev_passwd, _passwd_config, MAX_PASSWORD_LEN-1u);
	dev_passwd[MAX_PASSWORD_LEN-1u]=(uchar)'\0';
	if(strcmp(dev_passwd,cfg_read_sys_password()) != 0)/**/
	{	
		set_sys_pass((char *)dev_passwd);
		cfg_write_sys_password((char *)dev_passwd);
		nvram_bufset(RT2860_NVRAM, "Password", (char *)dev_passwd);
		nvram_commit(RT2860_NVRAM);
		scmd -> cmd_status = htons(URCP_OK);
	}
	else
	{
		URCP_DEBUG(("URCP set the same password with system. The password is %s\n", dev_passwd));
		scmd -> cmd_status = htons(URCP_OK);
	}
    
	/*构建hello回复命令*/
	scmd -> type = cmd->type;
	scmd -> length = sizeof(cmd_hdr_t);
	*app_data += scmd -> length;
	scmd -> length = htons((uint16)(scmd->length));

	return ret;
}
#endif

#ifdef URCP_CMD_AP_BASIC_CONFIG_GET_REPLY
void urcp_ap_basic_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_ap_basic_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    /*URCP_GET or URCP_SET*/
#if 0
	    if(a_type==SESSION_TYPE_SET)
#endif
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_ap_basic_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif

#ifdef URCP_CMD_AP_MAC_FILTER_SET_REPLY
void urcp_ap_mac_filter_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s ,urcp_ststus=%d\n", __func__,urcp_state));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_ap_mac_filter_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_ap_mac_filter_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif

#ifdef URCP_CMD_AP_JOIN_SZ_SET_REPLY
void urcp_ap_join_sz_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_ap_join_sz_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_ap_join_sz_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif

#ifdef URCP_CMD_WL_MAC_FILTER_SET_REPLY
void urcp_wl_mac_filter_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s\n", __func__));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
		urcp_wl_mac_filter_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    /*URCP_GET or URCP_SET*/
#if 0
	    if(a_type==SESSION_TYPE_SET)
#endif
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_wl_mac_filter_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif

#ifdef URCP_CMD_AP_CHANNEL_GET_REPLY
/********************
 *
 *receive the ap channel
 *KeHuatao
 ********************/
uint16 receive_ap_channel(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;

    URCP_DEBUG(("%s\n", __func__));
//    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.channel_head)));
//    printf("%s\n", __func__);    

//    ret = receive_number_channel(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.channel_head)));
    return ret;
}
#endif

#ifdef URCP_CMD_CURRENT_CLIENTS_GET_REPLY
/********************
 *
 *receive the ap client 
 *KeHuatao
 ********************/
uint16 receive_clients(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uint32 client,*ptr;
    cmd_hdr_gen_t *tmp;
    int j;

    URCP_DEBUG(("%s\n", __func__));


    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
    ptr = (uint32*) &((*dev)->wl_info.clients);
    client = ntohl(tmp->value);
//    if( *ptr != client )
    {
        *ptr = client;
#if (AP_LOAD_BALANCE == FYES )
	ApLoadBalanceProfile *profLb=NULL;
    SystemProfile *profSys = NULL;
    if( (*dev)->load_balance > 0 ){
        profLb=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_LOAD_BALANCE,(*dev)->load_balance-1);  /*get inst pointer*/
        profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
        if((profSys!=NULL) && (profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
        {
            for(j=0;j<profLb->num;j++){
                if (mac_cmp(profLb->ap[j].macAddr.addr, (*dev)->mac) == 0){
                    profLb->ap[j].client = client;
                    break;
                }
            }
            if( profLb->lbEn && profSys->loadBalanceEn ){
                ap_load_balance_process((*dev)->load_balance);    /*处理负载均衡*/
            }
        }
    }
#endif
    }
	URCP_DEBUG(("%s: %d\n", __func__, *ptr));
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#ifdef URCP_CMD_AP_SSID_GET_REPLY
/********************
 *
 *receive the ssid
 *KeHuatao
 ********************/
uint16 receive_ssid(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    //printf("%s\n", __func__);
    int32 val, i;
    uint16 ret=URCP_OK;
    cmd_hdr_gen_t *cmd_ssid=NULL;
    wl_ssid_t *wl_ssid=NULL;
    ServiceZoneProfile *profSZ=NULL;
	char tmp[UTT_SSID_LENTH+1],*ap_ssid=NULL;

    URCP_DEBUG(("%s\n", __func__));
    cmd_ssid = (cmd_hdr_gen_t*)cmd;
    val = ntohl(cmd_ssid->value) * sizeof(wl_ssid_t);
    val = val + sizeof(cmd_hdr_gen_t);
    //printf("\n%s val:%d cmd->length:%d value:%d wl_ssid_t:%d gen_t:%d\n", __func__, val, ntohs(cmd->length), ntohl(cmd_ssid->value),sizeof(wl_ssid_t), sizeof(cmd_hdr_gen_t));
    URCP_DEBUG(("\n%s val:%d cmd->length:%d value:%d wl_ssid_t:%d gen_t:%d\n", __func__, val, ntohs(cmd->length), ntohl(cmd_ssid->value),sizeof(wl_ssid_t), sizeof(cmd_hdr_gen_t)));
    if( (dev!=NULL) && (*dev!=NULL) && (val==ntohs(cmd->length)))
    {
	val = ntohl(cmd_ssid->value);
	if(val>UTT_SSID_NUM)
	{
	    val = UTT_SSID_NUM;
	}
	(*dev)->wl_info.joined_sz_num = (uint16)ntohl(cmd_ssid->value);
	wl_ssid = (wl_ssid_t*)(cmd_ssid+1);
	for(i=0;i<val;i++)
	{
        memset((*dev)->wl_info.ssid_info[i].ssid,0,sizeof((*dev)->wl_info.ssid_info[i].ssid));
#if (SSID_ENCODE_TYPE == FYES)
	    profSZ = (ServiceZoneProfile *)ProfGetInstPointByName(MIB_PROF_SERVICE_ZONE, (wl_ssid + i)->szone);
		if(profSZ != NULL)
		{
		    memset(tmp,0,sizeof(tmp));
		    strncpy(tmp,(wl_ssid + i)->ssid,UTT_SSID_LENTH);
            ap_ssid = iconv_string(tmp,"UTF-8","GBK");
            URCP_DEBUG(("%s,line=%d,ap_ssid=%s,tmp=%s\n", __func__,__LINE__,ap_ssid,tmp));
		    if(ap_ssid != NULL)
		    {
            strncpy((*dev)->wl_info.ssid_info[i].ssid, ap_ssid, UTT_SSID_LENTH);
		    }else
		    {
            strncpy((*dev)->wl_info.ssid_info[i].ssid, tmp, UTT_SSID_LENTH);
		    }
        }else
		{
            strncpy((*dev)->wl_info.ssid_info[i].ssid, (wl_ssid + i)->ssid, UTT_SSID_LENTH);
		}
        if(ap_ssid != NULL)
        {
            free(ap_ssid);
            ap_ssid = NULL;
        }
#else
        strncpy((*dev)->wl_info.ssid_info[i].ssid, (wl_ssid + i)->ssid, UTT_SSID_LENTH);
#endif

    URCP_DEBUG(("%s ssid:%s\n", __func__, (*dev)->wl_info.ssid_info[i].ssid));
    //printf("%s ssid:%s\n", __func__, (*dev)->wl_info.ssid_info[i].ssid);
	    strncpy((*dev)->wl_info.ssid_info[i].szone, (wl_ssid + i)->szone, UTT_SZONE_LENTH);
	    memcpy((*dev)->wl_info.ssid_info[i].mac, (wl_ssid + i)->mac, MAC_LEN);
	}
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }

    //printf("%s\n", __func__);
    return ret;
}
#endif

#ifdef URCP_CMD_AP_ROAMING_TH_GET_REPLY
uint16 receive_ap_roaming_th(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret = URCP_OK;
    uint32 dbm;
    cmd_hdr_gen_t *tmp=NULL;

    URCP_DEBUG(("%s\n", __func__));

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
    (*dev)->dbm = ntohl(tmp->value);
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#ifdef URCP_CMD_AP_DHCP_EN_GET_REPLY
uint16 receive_ap_dhcp_en(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret = URCP_OK;
    uint32 dhcp_en;
    cmd_hdr_gen_t *tmp=NULL;

    URCP_DEBUG(("%s\n", __func__));

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
    (*dev)->dhcp_en = ntohl(tmp->value);
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#ifdef URCP_CMD_AP_WORK_MODE_GET_REPLY
/********************
 *
 *receive the ap work mode
 *xhhu
 ********************/
uint16 receive_ap_work_mode(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret = URCP_OK;
    uint32 num;
    cmd_hdr_gen_t *tmp=NULL;

    URCP_DEBUG(("%s\n", __func__));

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
	num = ntohl(tmp->value);
	if(num==1)
	{
             (*dev)->dev_type |= DEV_TYPE_FIT_AP;
	}
	else
	{
             (*dev)->dev_type |= DEV_TYPE_FAT_AP;
	}
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#if 0
/********************
 *
 *receive the ap device name
 *xhhu
 ********************/
int16 receive_ap_dev_name(cmd_hdr_t* cmd, urcp_infor_t* urcp_infor)
{
    uchar buf[1];
    uchar *tmp = buf;
    uint16 ulen;
    urcp_ap_basic_conf_t *conf_buff;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if(ulen>=(uint16)sizeof(urcp_ap_basic_conf_t))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	conf_buff = (urcp_ap_basic_conf_t*)(&tmp[sizeof(cmd_hdr_t)]);/*跳过命令头数据*/

	/*转化字节序*/
	strcpy(conf_buff->dev_name, (char *)conf_buff);/*dev name*/

	init_ap_basic_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(conf_buff));/*写入misc buff*/
    }
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_IP_ADDR_GET_REPLY
/********************
 *
 *receive the ap ip addr
 *xhhu
 ********************/
uint16 receive_ip_addr(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret=URCP_OK;

    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->ip)));

    return ret;
}
#endif
#ifdef URCP_CMD_NETMASK_GET_REPLY
/********************
 *
 *receive the ap netmask addr
 *xhhu
 ********************/
uint16 receive_netmask(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret;

    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->netmask)));

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_GATEWAY_GET_REPLY
/********************
 *
 *receive the ap gateway addr
 *xhhu
 ********************/
uint16 receive_gateway(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret;

    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->gateway)));

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_USER_NAME_GET_REPLY
/********************
 *
 *receive the ap user name
 *xhhu
 ********************/
uint16 receive_ap_user_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uchar *tmp;
    uint16 ulen, ret=URCP_OK;

    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if((ulen<=MAX_USER_NAME_LEN) && (dev!=NULL) && (*dev!=NULL))
    {
	tmp = (uchar*)cmd;
	/*复制数据*/
	tmp = tmp + sizeof(cmd_hdr_t);/*跳过命令头数据*/

	/*转化字节序*/
	strncpy((*dev)->user, tmp, MAX_USER_NAME_LEN);/*user name*/
    }
    else
    {
	URCP_DEBUG(("%s error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_GET_REPLY
/********************
 *
 *receive the ap max clients
 *xhhu
 ********************/
uint16 receive_ap_max_client(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret;

    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.max_clients)));
    return URCP_OK;
}

uint16 receive_ap_max_client_new(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret, num_sz = 0;
    uchar *name = NULL, *tmp = NULL;
    int i = 0;
    if ((ntohs(cmd->length) > sizeof(cmd_hdr_t)) && (dev != NULL) && ( *dev != NULL) )
    {
	tmp = (uchar *)cmd + sizeof(cmd_hdr_t);
	(*dev)->wl_info.max_clients = (uint32)(ntohs(*((uint16 *)tmp)));	    //获取最大客户端数
	tmp += sizeof(uint16);
	num_sz = ntohs(*((uint16 *)tmp));
	tmp += sizeof(uint16);
//	printf("%s, %d, %d\n", __func__, (*dev)->wl_info.max_clients, num_sz);
	/*获取每个服务区的最大客户端*/
	while((num_sz <= 4) && (num_sz > 0) && (i < 4))
	{
	    if (strncmp((*dev)->wl_info.ssid_info[i].szone, tmp , UTT_INST_NAME_LEN) == 0)
	    {
		(*dev)->wl_info.ssid_info[i].max_clients = (uint16)(tmp[UTT_INST_NAME_LEN]);
		tmp += UTT_INST_NAME_LEN + 1;
		num_sz--;
//		printf("%s, %d\n", __func__, (*dev)->wl_info.ssid_info[i].max_clients);
	    }
	    i++;
	}
    }

    return URCP_OK;
}

#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_GET_REPLY
/********************
 *
 *receive the ap effect_conf
 *xhhu
 ********************/
uint16 receive_ap_effect_conf(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret = URCP_OK;
    uint32 num;
    cmd_hdr_gen_t *tmp=NULL;
    urcp_ap_basic_conf_t conf_buff;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));
    memset(&conf_buff,0,sizeof(urcp_ap_basic_conf_t));/*写之前先清除*/

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
	num = ntohl(tmp->value);
	//printf("\n< %s %d > num:%d\n", __func__,__LINE__,num);
	conf_buff.effect_conf = num;
	//printf("\n< %s %d > conf:%d\n", __func__,__LINE__,conf_buff.effect_conf);
	init_ap_basic_misc_opt_assist(&misc_buff);
	urcp_rev_general_get_reply(urcp_infor->mac,ntohs(urcp_infor->urcp_header->request_id),&misc_buff, (uchar*)(&conf_buff));/*写入misc buff*/
#if 0
	if(num==1)
	{
	printf("\n< %s %d > flag:%d\n", __func__,__LINE__,(*dev)->wl_info.ap_flag);
             (*dev)->wl_info.ap_flag |= AP_PROFILE_LOCAL;
	printf("\n< %s %d > flag:%d\n", __func__,__LINE__,(*dev)->wl_info.ap_flag);
	}
	else
	{
	printf("\n< %s %d > flag:%d\n", __func__,__LINE__,(*dev)->wl_info.ap_flag);
             (*dev)->wl_info.ap_flag |= AP_PROFILE_DOWNLOAD;
	printf("\n< %s %d > flag:%d\n", __func__,__LINE__,(*dev)->wl_info.ap_flag);
	}
#endif
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#ifdef URCP_CMD_AP_AFFIRM_GET
uint16 get_affirm(cmd_hdr_t *cmd, uchar **app, urcp_infor_t* urcp_info)
{
	DBCK();
	cmd_discover_t *cmd_snd, *cmd_rev;

	extern ap_info_t ap_infor;
	FatFitSwitchProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;

	cmd_rev = (cmd_discover_t *)(cmd);
	cmd_snd = (cmd_discover_t *)(*app);

	if (ntohl(cmd_rev->send_flag) & DEV_TYPE_AC) {
		prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
		if (prof != NULL) {
			if (prof->fatEnable == PROF_DISABLE) {
				if ((ap_infor.ap_auth == 1) && (0 == mac_cmp(ap_infor.ac_infor.mac, urcp_info->mac))) {
					DBCK();
					cmd_snd->send_flag = htonl(DEV_TYPE_FIT_AP | DEV_FLAG_AUTHED);
				}
			}
		}
	}

	cmd_snd->recv_flag = htonl(DEV_TYPE_AC);
	cmd_snd->type = htonl(CMD_AP_AFFIRM);
	cmd_snd->length = htons(sizeof(cmd_discover_t));
	*app = *app + sizeof(cmd_discover_t);

	return URCP_OK;
}
#endif

#ifdef URCP_CMD_AP_AFFIRM_GET_REPLY
uint16 receive_affirm(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
	DBCK();

	uint32 val;

	val = ntohl(((cmd_discover_t *)cmd)->send_flag);

	if (val & DEV_FLAG_AUTHED) {
		ap_auth_num++;
		DBCK("ap_auth_num = %d", ap_auth_num);
	}

	num_of_auth_ap_set(ap_auth_num);

	return URCP_OK;
}
#endif

#ifdef URCP_CMD_DISCOVER_GET
uint16 get_discover(cmd_hdr_t *cmd, uchar **app, urcp_infor_t* urcp_info)
{
    cmd_discover_t *cmd_snd, *cmd_rev;
#ifdef URCP_AP
	extern ap_info_t ap_infor;
    FatFitSwitchProfile *prof=NULL;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
#endif
    
    URCP_DEBUG(("%s\n", __func__)); 
    cmd_rev = (cmd_discover_t*)(cmd);
#ifndef URCP_AP
    cmd_snd = (cmd_discover_t*)(*app);
#endif
    /*数据部分*/
#ifdef URCP_AP
    if(ntohl(cmd_rev->send_flag) & DEV_TYPE_AC)
    {
	if((ntohl(cmd_rev->recv_flag) & DEV_TYPE_FIT_AP) && (read_state()== AP_STATE_DISCOVERY))
	{
	    change_state(AP_STATE_UNAUTH);
	}
    }
    prof=(FatFitSwitchProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
	if(prof->fatEnable == PROF_ENABLE)
	{
	    if((ntohl(cmd_rev->recv_flag) & DEV_TYPE_FAT_AP)==0){
		return URCP_FATAL_ERROR;
	    }
#if t
	    cmd_snd -> send_flag = htonl(DEV_TYPE_FAT_AP);
#endif
	}
	else
	{
	    if((ntohl(cmd_rev->recv_flag) & DEV_TYPE_FIT_AP)==0){
		return URCP_FATAL_ERROR;
	    }

		DBCK("urcp_info->mac =  %s", str_macaddr(urcp_info->mac));
		DBCK("ac_infor->mac =  %s", str_macaddr(ap_infor.ac_infor.mac));
#if t
		if (ap_infor.ap_auth == 1) {

			if (0 == mac_cmp(ap_infor.ac_infor.mac, urcp_info->mac)) {
				DBCK();
				cmd_snd -> send_flag = htonl(DEV_TYPE_FIT_AP|DEV_FLAG_MANAGE|DEV_FLAG_AUTHED);
			}
		} else {
			DBCK();
			cmd_snd -> send_flag = htonl(DEV_TYPE_FIT_AP|DEV_FLAG_MANAGE);
		}
#endif
	}
    }
#if t
    cmd_snd -> recv_flag = htonl(DEV_TYPE_AC);
#endif
    ac_ip_mac_set(urcp_info->mac, urcp_info->urcp_header->ip);
	extern uint32 ac_infor_times;
	ac_infor_times = 0u;
#else
    if((ntohl(cmd_rev->recv_flag) & DEV_TYPE_AC) == 0)
    {
	return URCP_FATAL_ERROR;
    }
    cmd_snd -> send_flag = htonl(DEV_TYPE_AC);
    cmd_snd -> recv_flag = htonl(DEV_TYPE_FIT_AP|DEV_TYPE_FAT_AP);
#endif

#ifndef URCP_AP
    /*构建回复命令*/
    cmd_snd -> type = htonl(CMD_DISCOVER);
    /*数据长度及状态*/
    cmd_snd  -> cmd_status = (uint16)htons((uint16)URCP_OK);
    URCP_DEBUG(("\n%s status:%d\n", __func__, ntohs(cmd_snd  -> cmd_status)));
    cmd_snd  -> length = htons(sizeof(cmd_discover_t));
    *app = *app + sizeof(cmd_discover_t);
#endif
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_DISCOVER_GET_REPLY
uint16 receive_discover(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    DBCK("----------------------------------------mac: %s", str_macaddr(urcp_info->mac));
	URCP_DEBUG(("debug: %d in %s()\n", __LINE__, __func__));
#ifdef URCP_AC
    uint32 val;
#endif
    URCP_DEBUG(("%s\n", __func__));

#ifdef URCP_AP
	extern uint32 ac_infor_times;
	DBCK("ac_infor_times = %d\n", ac_infor_times);
	ac_infor_times = 0u;
    if((read_state() == AP_STATE_DISCOVERY) || (read_state() == AP_STATE_UNMANGEDED)) {
		DBCK();
	change_state(AP_STATE_UNAUTH);
	URCP_DEBUG(("debug: %d in %s()\n", __LINE__, __func__));
	ac_ip_mac_set(urcp_info->mac, urcp_info->urcp_header->ip);
	//get_ip_from_dhcp_server(urcp_info->urcp_header->ip);
    } else if (read_state() == AP_STATE_NORMAL) {
		DBCK();
		ac_ip_mac_set(urcp_info->mac, urcp_info->urcp_header->ip);
	} else {
		DBCK();
	}
#endif
#ifdef URCP_AC
    /*添加到发现设备表中*/
    val = ntohl(((cmd_discover_t*)cmd)->send_flag);
//    val |= DEV_FLAG_AUTHED;
#if 0
	DBCK("val = %x", val);
	if ((val & DEV_FLAG_AUTHED) != 0) {
		DBCK();
		num_of_auth_ap_inc(5);
	}
#endif
    *dev = dev_get(urcp_info->mac,urcp_info->vid, ntohl(urcp_info->urcp_header->ip), val);

#endif
    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_UPDATE_FIRMWARE_GET_REPLY
uint16 receive_ap_update_fireware(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    cmd_hdr_gen_t* cmd_hdr_gen;
    cmd_hdr_gen = (cmd_hdr_gen_t *)cmd;

	DBCK("value = %d", ntohl(cmd_hdr_gen->value));
    if(read_state()==AP_STATE_AUTH)
    {     
	if(ntohl(cmd_hdr_gen->value) == 1u) /*update firmware*/
	{ 
	DBCK();
	    change_state(AP_STATE_SOFEUPDATE);
	} 
	else
	{ 
	DBCK();
	    change_state(AP_STATE_CONFUPDATE_REQ);
	} 
    }     

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_UPDATE_FIRMWARE_GET

void swverinfo_str_to_int(char *str_info, int *int_info)
{
	int major = 0, minor = 0, minor1 = 0;

	sscanf(str_info + 1, "%d.%d.%d", &major, &minor, &minor1);

	*int_info = (major << (SWVER_3 + SWVER_2)) | (minor << (SWVER_3)) | minor1;

	return;
}

uint16 get_ap_update_fireware(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
	DBCK();
	int sw_ver = 0, sw_ver_local = 0;
    uchar buf[1], tp[128];
	char tmp_path[256];
    uchar *tmp = buf;
    uint32 value = 0u;
    cmd_hdr_gen_t* cmd;
    urcp_ap_sysinfo_t *ap_sysinfo;
    uchar buffer[URCP_BUF_SIZE];
    MibProfileType mibType = MIB_PROF_AP_SW_MNG;
    ApSwMngProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_IMAGE_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_IMAGE_PATH);
#else
		strcat(tmp_path, TFTP_IMAGE_ROOT);
#endif
	}

    /*get AP system information*/
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, cmd_buf, ntohs(cmd_buf->length));
    ap_sysinfo = (urcp_ap_sysinfo_t *)(&buffer[sizeof(cmd_hdr_t)]);

    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	{
	    DBCK("prof->sw_model:%s, ap_sysinfo->product_type:%s\nprof->hw_version:%s, ap_sysinfo->hw_version:%s, prof->defaulted = %u\n", prof->sw_model, ap_sysinfo->product_type, prof->hw_version, ap_sysinfo->hw_version, prof->defaulted);
	    if((strcmp(prof->sw_model, ap_sysinfo->product_type) == 0)
		    && !(strcmp(prof->hw_version, ap_sysinfo->hw_version))
		    &&(prof->defaulted == 1u))
	    {
		swverinfo_str_to_int(ap_sysinfo->sw_version, &sw_ver);
		swverinfo_str_to_int(prof->sw_version, &sw_ver_local);

		DBCK("ap_sysinfo->sw_version = %s, sw_ver = %d, prof->sw_version = %s, sw_ver_local = %d", ap_sysinfo->sw_version, sw_ver, prof->sw_version, sw_ver_local);

		if (sw_ver < sw_ver_local) {
			memset(tp, 0, sizeof(tp));
			sprintf(tp, "cp %s%s %s%s", tmp_path, prof->sw_name, TFTP_ROOT, IMAGE_FILE);
			DBCK("tp = %s\n", tp);
			system(tp);
			value = 1u;
		}
	    }
	}
    }

    tmp = *app_data;
    cmd = (cmd_hdr_gen_t*)tmp;

    /*构建回复命令*/
    cmd -> type = htonl(CMD_AP_UPDATE_FIRMWARE);

    /*数据部分*/

    /*数据长度及状态*/
    cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
    cmd -> value = htonl(value);
    cmd -> length = sizeof(cmd_hdr_gen_t);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    URCP_DEBUG(("%s\n", __func__)); 

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
uint16 receive_ap_update_profile(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    if(read_state()==AP_STATE_CONFUPDATE_REQ)
    {     
	if(ntohl(((cmd_hdr_gen_t*)cmd)->value) == 0u) /*update profile*/
	{ 
	    change_state(AP_STATE_CONFUPDATE);
	} 
	else
	{
	    /*request failed, return to discover state*/
	    change_state(AP_STATE_DISCOVERY);    
	}
    }     

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET
static void config_temp_convert_config_file(ApConfTempProfile *prof, ServiceZoneProfile *profSz, int32 sn)
{
	DBCK();
    char tp[128], wl_conf[2048], tmp[1024], ftmp[512];
    FILE *fp;
	struct in_addr fromAddr;
#if(SSID_ENCODE_TYPE == FYES)
    char *ssid=NULL;
#endif

    URCP_DEBUG(("Func:%s()\n", __func__)); 

    memset(tp, 0, sizeof(tp));
    sprintf(tp, "%s%s_%d", TFTP_ROOT, CONFIG_FILE, sn);
    fp = fopen(tp, "w");
    if(fp != NULL)
    {
	memset(wl_conf, 0, sizeof(wl_conf));
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"gb2312\"?><config><interface><case\n");
	fprintf(fp, "name=\"1\"><active>Yes</active><ethernet><connMode>DHCP</connMode><static><ip>192.168.1.253</ip></static><pppoe /><wan3g /></ethernet></case></interface><user /><dhcpServer /><TaskSched /><adminConf />\n");
	DBCK();
#if 0
	
	fprintf(fp, "name=\"1\"><active>Yes</active><ethernet><connMode>DHCP</connMode><static><ip>192.168.1.253</ip></static><pppoe /><wan3g /></ethernet></case></interface><user /><dhcpServer /><TaskSched /><adminConf /><wireless><case\n");

	memset(wl_conf, 0, sizeof(wl_conf));
	/*wireless switch*/
	if(prof->wireless != PROF_ENABLE)
	{
	DBCK();
	    strcat(wl_conf, "name=\"wireless0\"><active>No</active>");
	}
	else
	{
	DBCK();
	    strcat(wl_conf, "name=\"wireless0\"><active>Yes</active>");
	}
	memset(tmp, 0, sizeof(tmp));
	/*wireless mode*/
	if(prof->mode == 1u)
	{
	    strcat(tmp, "<wirelessMode>only11g</wirelessMode>");
	}
	else if(prof->mode == 2u)
	{
	    strcat(tmp, "<wirelessMode>only11n</wirelessMode>");
	}
	else if(prof->mode == 3u)
	{
	    strcat(tmp, "<wirelessMode>and11g11n</wirelessMode>");
	}
	/*wireless channel*/
	memset(ftmp, 0, sizeof(ftmp));
	sprintf(ftmp, "<channel>%d</channel>", prof->channel);
	strcat(tmp, ftmp);
	if (prof->channel == 0) {
	    strcat(tmp, "<AutoChannelSelect>2</AutoChannelSelect>");
	} else {
	    strcat(tmp, "<AutoChannelSelect>0</AutoChannelSelect>");
	}
	/*client isolate*/
	/*
	if(prof->cltIsolate != PROF_ENABLE)
	{
	    memset(ftmp, 0, sizeof(ftmp));
		sprintf(ftmp, "<cltIsolate>0</cltIsolate>");
	    strcat(tmp, ftmp);
	}
	else
	{
	    memset(ftmp, 0, sizeof(ftmp));
		sprintf(ftmp, "<cltIsolate>1</cltIsolate>");
	    strcat(tmp, ftmp);
	}
	*/
	/*chan width*/
	memset(ftmp, 0, sizeof(ftmp));
	sprintf(ftmp, "<ChanWidth>%d</ChanWidth>", prof->bw);
	strcat(tmp, ftmp);
	/*Short Guard Interval*/
	if(prof->sgi != PROF_ENABLE)
	{
	    memset(ftmp, 0, sizeof(ftmp));
	    sprintf(ftmp, "<apClient /><apWds /><sgi>0</sgi>");
	    strcat(tmp, ftmp);
	}
	else 
	{
	    memset(ftmp, 0, sizeof(ftmp));
	    sprintf(ftmp, "<apClient /><apWds /><sgi>1</sgi>");
	    strcat(tmp, ftmp);
	}
	/*merge*/
	strcat(wl_conf, "<mbasecfg>");
	strcat(wl_conf, tmp);
	strcat(wl_conf, "</mbasecfg>");
	/*rate*/
	memset(ftmp, 0, sizeof(ftmp));
	memset(tmp, 0, sizeof(tmp));
	if(prof->rate == 0)
	{
	    strcat(tmp, "<rate>0</rate>");
	}
	else if(prof->rate == 11)
	{
	    strcat(tmp, "<rate>11</rate>");
	}
	else if(prof->rate == 54)
	{
	    strcat(tmp, "<rate>54</rate>");
	}
	else if(prof->rate == 150)
	{
	    strcat(tmp, "<rate>150</rate>");
	}
	else if(prof->rate == 300)
	{
	    strcat(tmp, "<rate>300</rate>");
	}
	/*power*/
	if(prof->power == 1u)
	{
	    strcat(tmp, "<autoPower>1</autoPower>");
	}
	else
	{
	    strcat(tmp, "<autoPower>0</autoPower>");
	}
	/*man power*/
	if(prof->manPower == 1u)
	{
	    strcat(tmp, "<manPower>1</manPower>");
	}
	else if(prof->manPower == 2u)
	{
	    strcat(tmp, "<manPower>2</manPower>");
	}
	else if(prof->manPower == 3u)
	{
	    strcat(tmp, "<manPower>3</manPower>");
	}
	/*merge*/	    
	strcat(wl_conf, "<fitAp>");
	strcat(wl_conf, tmp);
	strcat(wl_conf, "</fitAp>");

	strcat(wl_conf, "<msafecfg />");
	memset(tmp, 0, sizeof(tmp));
	/*short preamble*/
	if(prof->preamble != PROF_ENABLE)
	{
	    strcat(tmp, "<preamble>Disable</preamble>");
	}
	else
	{
	    strcat(tmp, "<preamble>Enable</preamble>");
	}
	if(prof->wmm != PROF_ENABLE)
	{
	    strcat(tmp, "<wmm>Disable</wmm>");
	}
	else
	{
	    strcat(tmp, "<wmm>Enable</wmm>");
	}
	/*merge*/
	strcat(wl_conf, "<advance>");
	strcat(wl_conf, tmp);
	strcat(wl_conf, "</advance>");
	strcat(wl_conf, "<mFltGlb /></case>");
#endif
	if (profSz == NULL) {
		strcat(wl_conf,	"<wireless /></config>");
	} else {
	DBCK();
		strcat(wl_conf, "<wireless><case\n");
		/*
		if (prof->wireless == PROF_ENABLE) {
			strcat(wl_conf, "name=\"wireless1\"><active>Yes</active>");
		} else {
			strcat(wl_conf, "name=\"wireless1\"><active>No</active>");
		}
		*/
		strcat(wl_conf, "name=\"wireless1\"><active>Yes</active>");

		memset(tmp, 0, sizeof(tmp));
		memset(ftmp, 0, sizeof(ftmp));
#if(SSID_ENCODE_TYPE == FYES)
        if(profSz->ssid_encode_type == 1)// 1为手机优先，将编码转换为utf-8格式
        {
            ssid = iconv_string(profSz->ssid,"GBK","UTF-8");
		    sprintf(ftmp, "<mbasecfg><ssid>%s</ssid>", ssid);
		}else{
		    sprintf(ftmp, "<mbasecfg><ssid>%s</ssid>", profSz->ssid);
		}
        if(ssid != NULL)
        {
            free(ssid);
            ssid = NULL;
        }
#else
            sprintf(ftmp, "<mbasecfg><ssid>%s</ssid>", profSz->ssid);
#endif
		strcat(tmp, ftmp);
		if (profSz->ssidBcast == 1) {
			strcat(tmp, "<ssidBroadCastEn>Enable</ssidBroadCastEn><apClient />");
		} else {
			strcat(tmp, "<apClient />");
		}
		/* WEP */
		if (profSz->securMode == 1) {
			if (profSz->wepSecurity.auMethod == 1) {
				strcat(tmp, "<apWds><wdsauthMode>OPEN</wdsauthMode><wdsEncrypType>WEP</wdsEncrypType>");
			} else if (profSz->wepSecurity.auMethod == 2) {
				strcat(tmp, "<apWds><wdsauthMode>SHARED</wdsauthMode><wdsEncrypType>WEP</wdsEncrypType>");
			} else {
				strcat(tmp, "<apWds><wdsauthMode>WEPAUTO</wdsauthMode><wdsEncrypType>WEP</wdsEncrypType>");
			}

			if (profSz->wepSecurity.kNum2.key_state_action == 1) {
					strcat(tmp, "<wdswepKeyID>2</wdswepKeyID>");
			} else if (profSz->wepSecurity.kNum3.key_state_action == 1) {
					strcat(tmp, "<wdswepKeyID>3</wdswepKeyID>");
			} else if (profSz->wepSecurity.kNum4.key_state_action == 1) {
					strcat(tmp, "<wdswepKeyID>4</wdswepKeyID>");
			} else {
					strcat(tmp, "<wdswepKeyID>1</wdswepKeyID>");
			}

			if (profSz->wepSecurity.kType == 1) {
					strcat(tmp, "<wdswepKeyType>1</wdswepKeyType>");
			}

			if ((profSz->wepSecurity.kNum1.passwd[0]) != '\0' && (profSz->wepSecurity.kNum1.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<wdswepKey1>%s</wdswepKey1>", profSz->wepSecurity.kNum1.passwd);
					strcat(tmp, ftmp);

			}
			if ((profSz->wepSecurity.kNum2.passwd[0]) != '\0' && (profSz->wepSecurity.kNum2.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<wdswepKey2>%s</wdswepKey2>", profSz->wepSecurity.kNum2.passwd);
					strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum3.passwd[0]) != '\0' && (profSz->wepSecurity.kNum3.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<wdswepKey3>%s</wdswepKey3>", profSz->wepSecurity.kNum3.passwd);
					strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum4.passwd[0]) != '\0' && (profSz->wepSecurity.kNum4.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<wdswepKey4>%s</wdswepKey4>", profSz->wepSecurity.kNum4.passwd);
					strcat(tmp, ftmp);
			}

			if (profSz->wepSecurity.kNum1.kFmt == 1) {
					strcat(tmp, "<wdswepkeyLen1>5</wdswepkeyLen1>");
			}
			if (profSz->wepSecurity.kNum2.kFmt == 1) {
					strcat(tmp, "<wdswepkeyLen2>5</wdswepkeyLen2>");
			}
			if (profSz->wepSecurity.kNum3.kFmt == 1) {
					strcat(tmp, "<wdswepkeyLen3>5</wdswepkeyLen3>");
			}
			if (profSz->wepSecurity.kNum4.kFmt == 1) {
					strcat(tmp, "<wdswepkeyLen4>5</wdswepkeyLen4>");
			}

			strcat(tmp, "</apWds></mbasecfg><fitAp />");

			if (profSz->wepSecurity.auMethod == 1) {
				strcat(tmp, "<msafecfg><authMode>OPEN</authMode><SafeEncrypType>WEP</SafeEncrypType>");
			} else if (profSz->wepSecurity.auMethod == 2) {
				strcat(tmp, "<msafecfg><authMode>SHARED</authMode><SafeEncrypType>WEP</SafeEncrypType>");
			} else {
				strcat(tmp, "<msafecfg><authMode>WEPAUTO</authMode><SafeEncrypType>WEP</SafeEncrypType>");
			}

			if (profSz->wepSecurity.kNum2.key_state_action == 1) {
					strcat(tmp, "<SafewepKeyID>2</SafewepKeyID>");
			} else if (profSz->wepSecurity.kNum3.key_state_action == 1) {
					strcat(tmp, "<SafewepKeyID>3</SafewepKeyID>");
			} else if (profSz->wepSecurity.kNum4.key_state_action == 1) {
					strcat(tmp, "<SafewepKeyID>4</SafewepKeyID>");
			} else {
					strcat(tmp, "<SafewepKeyID>1</SafewepKeyID>");
			}

			if (profSz->wepSecurity.kType == 1) {
					strcat(tmp, "<SafewepKeyType>1</SafewepKeyType>");
			}

			if ((profSz->wepSecurity.kNum1.passwd[0]) != '\0' && (profSz->wepSecurity.kNum1.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<SafewepKey1>%s</SafewepKey1>", profSz->wepSecurity.kNum1.passwd);
					strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum2.passwd[0]) != '\0' && (profSz->wepSecurity.kNum2.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<SafewepKey2>%s</SafewepKey2>", profSz->wepSecurity.kNum2.passwd);
					strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum3.passwd[0]) != '\0' && (profSz->wepSecurity.kNum3.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<SafewepKey3>%s</SafewepKey3>", profSz->wepSecurity.kNum3.passwd);
					strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum4.passwd[0]) != '\0' && (profSz->wepSecurity.kNum4.kFmt != 0)) {
					memset(ftmp, 0, sizeof(ftmp));
					sprintf(ftmp, "<SafewepKey4>%s</SafewepKey4>", profSz->wepSecurity.kNum4.passwd);
					strcat(tmp, ftmp);
			}

			if (profSz->wepSecurity.kNum1.kFmt == 1) {
					strcat(tmp, "<SafewepkeyLen1>5</SafewepkeyLen1>");
			}
			if (profSz->wepSecurity.kNum2.kFmt == 1) {
					strcat(tmp, "<SafewepkeyLen2>5</SafewepkeyLen2>");
			}
			if (profSz->wepSecurity.kNum3.kFmt == 1) {
					strcat(tmp, "<SafewepkeyLen3>5</SafewepkeyLen3>");
			}
			if (profSz->wepSecurity.kNum4.kFmt == 1) {
					strcat(tmp, "<SafewepkeyLen4>5</SafewepkeyLen4>");
			}

			strcat(tmp, "</msafecfg><advance /><mFltGlb /></case></wireless><fitApSZ><case\n");

			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "name=\"%s\"><active>Yes</active><ssidIsolate>%s</ssidIsolate><securMode>1</securMode>", profSz->head.name,profSz->ssidIsolate ? "Yes" : "No");
			strcat(tmp, ftmp);

			if ((profSz->wepSecurity.kNum1.passwd[0]) != '\0' && (profSz->wepSecurity.kNum1.kFmt != 0)) {
				memset(ftmp, 0, sizeof(ftmp));
				sprintf(ftmp, "<wepSecurity><kNum1><passwd>%s</passwd><kFmt>%d</kFmt></kNum1>", profSz->wepSecurity.kNum1.passwd, profSz->wepSecurity.kNum1.kFmt);
				strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum2.passwd[0]) != '\0' && (profSz->wepSecurity.kNum2.kFmt != 0)) {
				memset(ftmp, 0, sizeof(ftmp));
				sprintf(ftmp, "<wepSecurity><kNum2><passwd>%s</passwd><kFmt>%d</kFmt></kNum2>", profSz->wepSecurity.kNum2.passwd, profSz->wepSecurity.kNum2.kFmt);
				strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum3.passwd[0]) != '\0' && (profSz->wepSecurity.kNum3.kFmt != 0)) {
				memset(ftmp, 0, sizeof(ftmp));
				sprintf(ftmp, "<wepSecurity><kNum3><passwd>%s</passwd><kFmt>%d</kFmt></kNum3>", profSz->wepSecurity.kNum3.passwd, profSz->wepSecurity.kNum3.kFmt);
				strcat(tmp, ftmp);
			}
			if ((profSz->wepSecurity.kNum4.passwd[0]) != '\0' && (profSz->wepSecurity.kNum4.kFmt != 0)) {
				memset(ftmp, 0, sizeof(ftmp));
				sprintf(ftmp, "<wepSecurity><kNum4><passwd>%s</passwd><kFmt>%d</kFmt></kNum4>", profSz->wepSecurity.kNum4.passwd, profSz->wepSecurity.kNum4.kFmt);
				strcat(tmp, ftmp);
			}
			strcat(tmp, "</wepSecurity><wpaSecurity><radiusPort>0</radiusPort></wpaSecurity><wpaPskSecurity /></case></fitApSZ></config>");

		/* WPA1_AND_WPA2 */
		} else if (profSz->securMode == 2) {
			if (profSz->wpaSecurity.wVersion == 1) {
				strcat(tmp, "<apWds><wdsauthMode>WPA</wdsauthMode>");
			} else if (profSz->wpaSecurity.wVersion == 2) {
				strcat(tmp, "<apWds><wdsauthMode>WPA2</wdsauthMode>");
			} else {
				strcat(tmp, "<apWds><wdsauthMode>WPA1WPA2</wdsauthMode>");
			}

			if (profSz->wpaSecurity.sAlg == 1) {
				strcat(tmp, "<wdsEncrypType>TKIP</wdsEncrypType>");
			} else if (profSz->wpaSecurity.sAlg == 2) {
				strcat(tmp, "<wdsEncrypType>AES</wdsEncrypType>");
			} else {
				strcat(tmp, "<wdsEncrypType>TKIPAES</wdsEncrypType>");
			}

			strcat(tmp, "</apWds></mbasecfg><fitAp />");

			if (profSz->wpaSecurity.wVersion == 1) {
				strcat(tmp, "<msafecfg><authMode>WPA</authMode>");
			} else if (profSz->wpaSecurity.wVersion == 2) {
				strcat(tmp, "<msafecfg><authMode>WPA2</authMode>");
			} else {
				strcat(tmp, "<msafecfg><authMode>WPA1WPA2</authMode>");
			}

			if (profSz->wpaSecurity.sAlg == 1) {
				strcat(tmp, "<SafeEncrypType>TKIP</SafeEncrypType>");
			} else if (profSz->wpaSecurity.sAlg == 2) {
				strcat(tmp, "<SafeEncrypType>AES</SafeEncrypType>");
			} else {
				strcat(tmp, "<SafeEncrypType>TKIPAES</SafeEncrypType>");
			}

			fromAddr.s_addr = profSz->wpaSecurity.radiusIp;
			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "<SaferadiusIp>%s</SaferadiusIp>", inet_ntoa(fromAddr));
			strcat(tmp, ftmp);


			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "<SaferadiusPort>%d</SaferadiusPort><Saferekeyinterval>%d</Saferekeyinterval><SaferadiusKey>%s</SaferadiusKey></msafecfg><advance /><mFltGlb /></case></wireless><fitApSZ><case\n", profSz->wpaSecurity.radiusPort, profSz->wpaSecurity.updataTime, profSz->wpaSecurity.radiusPasswd);
			strcat(tmp, ftmp);

			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "name=\"%s\"><active>Yes</active><ssidIsolate>%s</ssidIsolate><securMode>2</securMode><wepSecurity><kNum1><key_state_action>No</key_state_action></kNum1><kNum2><key_state_action>No</key_state_action></kNum2><kNum3><key_state_action>No</key_state_action></kNum3><kNum4><key_state_action>No</key_state_action></kNum4></wepSecurity><wpaSecurity><radiusIp>%s</radiusIp><radiusPort>%d</radiusPort><radiusPasswd>%s</radiusPasswd><updataTime>%d</updataTime></wpaSecurity><wpaPskSecurity /></case></fitApSZ></config>", profSz->head.name, profSz->ssidIsolate ? "Yes" : "No",inet_ntoa(fromAddr), profSz->wpaSecurity.radiusPort, profSz->wpaSecurity.radiusPasswd, profSz->wpaSecurity.updataTime);
			strcat(tmp, ftmp);

		/* WPA1_PSK_AND_WPA2_PSK */
		} else if (profSz->securMode == 3) {
			if (profSz->wpaPskSecurity.wVersion == 1) {
				strcat(tmp, "<apWds><wdsauthMode>WPAPSK</wdsauthMode>");
			} else if (profSz->wpaSecurity.wVersion == 2) {
				strcat(tmp, "<apWds><wdsauthMode>WPA2PSK</wdsauthMode>");
			} else {
				strcat(tmp, "<apWds><wdsauthMode>WPAPSKWPA2PSK</wdsauthMode>");
			}

			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "<wdspskPsswd>%s</wdspskPsswd>", profSz->wpaPskSecurity.sharePasswd);
			strcat(tmp, ftmp);

			if (profSz->wpaPskSecurity.sAlg == 1) {
				strcat(tmp, "<wdsEncrypType>TKIP</wdsEncrypType>");
			} else if (profSz->wpaSecurity.sAlg == 2) {
				strcat(tmp, "<wdsEncrypType>AES</wdsEncrypType>");
			} else {
				strcat(tmp, "<wdsEncrypType>TKIPAES</wdsEncrypType>");
			}

			strcat(tmp, "</apWds></mbasecfg><fitAp />");

			if (profSz->wpaPskSecurity.wVersion == 1) {
				strcat(tmp, "<msafecfg><authMode>WPAPSK</authMode>");
			} else if (profSz->wpaSecurity.wVersion == 2) {
				strcat(tmp, "<msafecfg><authMode>WPA2PSK</authMode>");
			} else {
				strcat(tmp, "<msafecfg><authMode>WPAPSKWPA2PSK</authMode>");
			}

			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "<SafepskPsswd>%s</SafepskPsswd>", profSz->wpaPskSecurity.sharePasswd);
			strcat(tmp, ftmp);

			if (profSz->wpaPskSecurity.sAlg == 1) {
				strcat(tmp, "<SafeEncrypType>TKIP</SafeEncrypType>");
			} else if (profSz->wpaSecurity.sAlg == 2) {
				strcat(tmp, "<SafeEncrypType>AES</SafeEncrypType>");
			} else {
				strcat(tmp, "<SafeEncrypType>TKIPAES</SafeEncrypType>");
			}

			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "<Saferekeyinterval>%d</Saferekeyinterval></msafecfg><advance /><mFltGlb /></case></wireless><fitApSZ><case\n", profSz->wpaPskSecurity.updataTime);
			strcat(tmp, ftmp);

			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "name=\"%s\"><active>Yes</active><ssidIsolate>%s</ssidIsolate><securMode>3</securMode><wepSecurity><kNum1><key_state_action>No</key_state_action></kNum1><kNum2><key_state_action>No</key_state_action></kNum2><kNum3><key_state_action>No</key_state_action></kNum3><kNum4><key_state_action>No</key_state_action></kNum4></wepSecurity><wpaSecurity><radiusPort>0</radiusPort></wpaSecurity><wpaPskSecurity><sharePasswd>%s</sharePasswd><updataTime>%d</updataTime></wpaPskSecurity></case></fitApSZ></config>", profSz->head.name, profSz->ssidIsolate ? "Yes" : "No", profSz->wpaPskSecurity.sharePasswd, profSz->wpaPskSecurity.updataTime);
			strcat(tmp, ftmp);
		/* NONE */
		} else {
			strcat(tmp, "<apWds><wdsauthMode>OPEN</wdsauthMode><wdsEncrypType>NONE</wdsEncrypType></apWds></mbasecfg><fitAp /><msafecfg><authMode>OPEN</authMode><SafeEncrypType>NONE</SafeEncrypType></msafecfg><advance /><mFltGlb /></case></wireless><fitApSZ><case\n");
			memset(ftmp, 0, sizeof(ftmp));
			sprintf(ftmp, "name=\"%s\"><active>Yes</active><ssidIsolate>%s</ssidIsolate><wepSecurity><kNum1><key_state_action>No</key_state_action></kNum1><kNum2><key_state_action>No</key_state_action></kNum2><kNum3><key_state_action>No</key_state_action></kNum3><kNum4><key_state_action>No</key_state_action></kNum4></wepSecurity><wpaSecurity><radiusPort>0</radiusPort></wpaSecurity><wpaPskSecurity /></case></fitApSZ></config>", profSz->head.name, profSz->ssidIsolate ? "Yes" : "No");
			strcat(tmp, ftmp);
		}
		strcat(wl_conf, tmp);
	}

	DBCK();

	/*write to file*/
	fprintf(fp, "%s", wl_conf);
	fclose(fp);
    }

    return;
}

/* 判断某个vid是否在ap的vid bit位中，不在返回0，在返回-1*/
static int judgeVidBits(unsigned int vid, unsigned int *ap_vid_bits)
{
	unsigned int i, j;
	int ret = 0;

	i = vid >> 5;
	j = vid & 0x1fu;
	j = 1u << j;

	if ((ap_vid_bits[i] & j) != 0) {
		ret = -1;
	}

	return ret;
}

/* 在设备表中寻找vid vlan中是否有其他设备, 0表示没有 */
unsigned int isExistApInVlan(vid_t vid, int sn)
{
	extern urcp_shm_t* shm_ptr;
	int i, ret = 0;
	urcp_dev_t *tmp = NULL, *prev = NULL;
	int32 tb_index;

	for(i = 0u; i < DEV_HASH_SIZE; i++) {
		prev = NULL;
		tb_index = shm_ptr->dev_tb_hash[i];

		while (tb_index != NONE_NODE) {
			tmp = &shm_ptr->dev_table[tb_index-1];

			if ((tmp->aged != 2) && (tmp->vid == vid)) {
				ret = 1;
				return ret;
			}

			tb_index = tmp->tb_next;
		}
	}

	return ret;
}

uint16 get_ap_update_profile(cmd_hdr_t* cmd_buf, uchar** app_data, urcp_infor_t* urcp_info)
{
    uchar buf[1024], tp[256], tmp_path[128], sn_name[32];
    uchar *tmp = buf;
    uint32 err = 0u, found = 0u;
    cmd_hdr_gen_t* cmd;
    urcp_ap_sysinfo_t *ap_sysinfo;
    uchar buffer[URCP_BUF_SIZE];
    DIR *dir;
    struct dirent *ptr;
	MibProfileType mibTypeSz = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *profSz = NULL;
	uint32 foundSz = 0u;


	DBCK();
	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_AP_CONFIG_ROOT);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_AP_CONFIG_ROOT);
#else
		strcat(tmp_path, TFTP_AP_CONFIG_ROOT);
#endif
	}

    //MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;
    ApConfTempProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;

    URCP_DEBUG(("%s\n", __func__)); 
    /*数据部分*/
    /*get AP system information*/
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, cmd_buf, ntohs(cmd_buf->length));
    ap_sysinfo = (urcp_ap_sysinfo_t *)(&buffer[sizeof(cmd_hdr_t)]);
    ap_sysinfo->product_sn = ntohl(ap_sysinfo->product_sn);

    memset(sn_name, 0, sizeof(sn_name));
    sprintf(sn_name, "%d.xml", ap_sysinfo->product_sn);
    dir=opendir(tmp_path);
	DBCK("tmp_path = %s, sn_name = %s", tmp_path, sn_name);

    if(dir != NULL)
    {	
	while ( (ptr = readdir(dir)) != NULL)
	{
	    if (strcmp (ptr->d_name, sn_name) == 0)
	    {
		found = 1u;
		break;
	    }
	}
    }
	closedir(dir);

	vid_t vid = urcp_info->vid;
	unsigned int noInVlan = 1;
	DBCK("vid = %d", vid);
#ifdef AP_CONFIG_GET_ONE_VLAN
	extern vlan_ap_info_t vlan_ap_info[MAX_AP_VLAN];
	noInVlan = isExistApInVlan(vid, ap_sysinfo->product_sn);
	DBCK("noInVlan = %d", noInVlan);
	if ((found == 0u) && (noInVlan == 0u)){
		DBCK("product_type = %s, hw_version = %s, product_sn = %d", vlan_ap_info[vid].ap_info.product_type, vlan_ap_info[vid].ap_info.hw_version, vlan_ap_info[vid].ap_info.product_sn);
		DBCK("product_type = %s, hw_version = %s, product_sn = %d", ap_sysinfo->product_type, ap_sysinfo->hw_version, ap_sysinfo->product_sn);
		if ((strcmp(ap_sysinfo->product_type, vlan_ap_info[vid].ap_info.product_type) == 0)
				&& (strcmp(ap_sysinfo->hw_version, vlan_ap_info[vid].ap_info.hw_version) == 0)
		   ) {
			DBCK();
			sprintf(sn_name, "%d.xml", vlan_ap_info[vid].ap_info.product_sn);

			dir=opendir(tmp_path);
			if(dir != NULL)
			{
				while ( (ptr = readdir(dir)) != NULL)
				{
					if (strcmp (ptr->d_name, sn_name) == 0)
					{
						DBCK();
						found = 1u;
						memset(tp, 0, sizeof(tp));
						sprintf(tp, "cp %s%s %s%d.xml -f", tmp_path, sn_name, tmp_path, ap_sysinfo->product_sn);
						DBCK("%s", tp);
						system(tp);
						break;
					}
				}
			}
			closedir(dir);

		}
	}
#endif

    if(found == 1u)
    {
	memset(tp, 0, sizeof(tp));
	sprintf(tp, "cp %s%s %s%s_%d -f", tmp_path, sn_name, TFTP_ROOT, CONFIG_FILE, ap_sysinfo->product_sn);
	DBCK("%s", tp);
	URCP_DEBUG(("CMD:%s\n", tp));
	system(tp);
	found = 0u;
    }
    else
    {
	ProfInstLowHigh(mibTypeSz, &max, &min);
	for (index = min; index < max; index++) {
		profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSz, index);
		if ((profSz != NULL) && (profSz->serviceEn == 1) && (profSz->vlanType == 1)) {
			if (judgeVidBits(vid, profSz->apVidBits) != 0) {
				DBCK();
				foundSz = 1u;
				break;
			}
		}
	}
	URCP_DEBUG(("swtype=%s \thw=%s\tsn=%d\n",ap_sysinfo->product_type, ap_sysinfo->hw_version,ap_sysinfo->product_sn));
#if 0
	ProfInstLowHigh(mibType, &max, &min);
	for(index = min; index < max; index++)
	{
	    prof = (ApConfTempProfile*)ProfGetInstPointByIndex(mibType, index);
	    if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	    {

/*
		if((strcmp(prof->suitType, ap_sysinfo->product_type) == 0)
//			&& strcmp(prof->firmware, ap_sysinfo->hw_version)
			&&(prof->def == 1u))
*/
		{
		    found = 1u;
			if (foundSz == 1u) {
				config_temp_convert_config_file(prof, profSz, ap_sysinfo->product_sn);
			} else {
				config_temp_convert_config_file(prof, NULL, ap_sysinfo->product_sn);
			}
		    break;
		}
	    }
	}
#endif
    found = 1u;
	if (foundSz == 1u) {
		config_temp_convert_config_file(prof, profSz, ap_sysinfo->product_sn);
	} else {
		config_temp_convert_config_file(prof, NULL, ap_sysinfo->product_sn);
	}

	if(found == 0u)
	{
	    URCP_DEBUG(("Failed: No default config template !!!"));
	    err = 1u;
	}
    }

	tmp = *app_data;
	cmd = (cmd_hdr_gen_t*)tmp;

	/*构建回复命令*/
	cmd -> type = htonl(CMD_AP_UPDATE_PROFILE);
	/*数据长度及状态*/
	cmd -> cmd_status = (uint16)htons((uint16)URCP_OK);
	cmd->value = ntohl(err);
	cmd -> length = sizeof(cmd_hdr_gen_t);
	*app_data += cmd -> length;
	cmd -> length = htons(cmd->length);

	URCP_DEBUG(("%s\n", __func__));

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_UPDATE_PROFILE_GET_REPLY
uint16 receive_ap_auth(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uchar  *cmd_data_ptr;

    set_ap_authorize(1u);          
    if(read_state() == AP_STATE_UNAUTH) {     
        change_state(AP_STATE_AUTH);   
    }
    cmd_data_ptr = (uchar *)cmd + sizeof(cmd_hdr_t);

extern ap_info_t ap_infor;
    ap_infor.ac_infor.acMode = *cmd_data_ptr;

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_AP_AUTH_PRE_GET
#define SM_DEV_TB   (shm_ptr->dev_table)
extern urcp_shm_t* shm_ptr;

void urcp_ap_affirm(uchar *mac)
{
	uint16 id;
	uint32 len;
	urcp_hdr_t *header;
	cmd_discover_t *cmd;
	uchar buffer[URCP_BUF_SIZE];
	int iftag = 0;

	if (session_alloc(SESSION_TYPE_GET, 0u, URCP_CMD_TIME_OUT, &id, NULL) == 0) {
		DBCK();
		memset(buffer, '\0', sizeof(buffer));
		iftag = urcp_buid_header(buffer, mac, (uint16)URCP_GET, id, NULL);
		if(iftag == 1)
		{
		    /*指向包头 带tag*/
		    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET + sizeof(uint32)]);

		    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t)+sizeof(uint32);
		    /*联动发现 */
			cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)+sizeof(uint32)]);
		}else{
			/*指向包头*/
		    header = (urcp_hdr_t*)(&buffer[ETH_URCP_OFFSET]);

		    len = ETH_URCP_OFFSET+sizeof(urcp_hdr_t);
		    /*联动发现 */
			cmd = (cmd_discover_t*)(&buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
		}

		cmd->type = htonl(CMD_AP_AFFIRM);
		cmd->cmd_status = htonl(URCP_OK);
		cmd->send_flag = htonl(DEV_TYPE_AC);
		cmd->recv_flag = htonl(DEV_TYPE_FIT_AP);
		cmd->length = htonl(sizeof(cmd_discover_t));
		len += sizeof(cmd_discover_t);
		header->cmd_no = 1u;
	}

	urcp_send(0u, &(buffer[0]), len);

	return;
}

void urcp_dev_table_ap_affirm()
{
    urcp_dev_t* tmp;
	char mac[MAC_LEN];
	int i = 0;

	pthread_mutex_lock(&ap_auth);
	ap_auth_num = 0;

	memset(mac, '\0', MAC_LEN);
	for (i = 0; i < DEV_TABLE_SIZE; i++) {
		tmp = &SM_DEV_TB[i];
		if (0 != mac_cmp(tmp->mac, mac)) {
			DBCK("tmp->mac = %s", str_macaddr(tmp->mac));
			urcp_ap_affirm(tmp->mac);
		}
	}
	pthread_mutex_unlock(&ap_auth);

	return;
}

uint16 get_ap_auth_pre(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info)
{
	urcp_dev_table_ap_affirm();
}
#endif

#ifdef URCP_CMD_AP_AUTH_REQ_GET
uint16 get_ap_auth_reqest(cmd_hdr_t* cmd, uchar** app_data, urcp_infor_t* urcp_info)
{
	DBCK();
    cmd_hdr_t* cmd_ptr;
    uchar  *cmd_data_ptr;
    uint16 ret = URCP_OK;     

    AcWorkModeProfile *prof = NULL;
    prof = (AcWorkModeProfile *)ProfGetInstPointByIndex(MIB_PROF_AC_WORK_MODE, 0);
#if 0
    urcp_dev_t* tmp=NULL;
#endif

//	num_of_auth_ap_set(ap_auth_num);
    URCP_DEBUG(("%s***********************************\n", __func__));

#if 0
	if(1)
    tmp = urcp_dev_table_lookup(urcp_info->mac, ntohl(urcp_info->urcp_header->ip));
    if(((tmp!=NULL) && ((tmp->dev_type & DEV_FLAG_AUTHED) != 0)) 
        || (permit_authorized(ntohl(((cmd_ap_auth_req_t*)cmd)->serial_no))==0)
      )
#endif
#if 0
	if (permit_authorized(ntohl(((cmd_ap_auth_req_t*)cmd)->serial_no))==0)
#else
	/*去掉了可管理AP数和AP权限的功能*/
	if (1)
#endif
	{
	URCP_DEBUG(("%s: pass\n", __func__));
	cmd_ptr = (cmd_hdr_t*)(*app_data);

	/*构建hello回复命令*/
	cmd_ptr -> type = htonl(CMD_AP_AUTH_REQ);
	cmd_ptr -> cmd_status = htons(URCP_OK);
	/*length*/
	cmd_ptr -> length = sizeof(cmd_hdr_t);

	/*数据长度*/
	*app_data += cmd_ptr -> length;
    cmd_data_ptr = *app_data;

    if( prof != NULL ){
        *cmd_data_ptr = prof->workMode;
        *app_data += sizeof(uchar);
        cmd_ptr -> length += sizeof(uchar);
    }

	cmd_ptr -> length = htons(cmd_ptr->length);
    }
    else
    {
	URCP_DEBUG(("%s: not pass\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#ifdef URCP_CMD_AP_CHANNEL_GET
uint16 get_ap_channel(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    uint32 channel, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_AP_CHANNEL);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP CHANNEL*/
		if (prof->mBaseCf.AutoChannelSelect != 0) {
			channel = 0;
		} else {
			channel = prof->mBaseCf.Channel;
		}
	URCP_DEBUG(("AP CHANNEL = %d\n", channel));
	*tmp = htonl(channel);

	/*length*/
	cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
	cmd -> cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd -> length = sizeof(cmd_hdr_t);
	cmd -> cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}

uint16 trap_get_ap_channel(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    short int channel = 0;
    int ret = 0;
    FILE *fp = NULL;
    char result[100] = "";
    uint16  *tmp_head = NULL, *tmp_end = NULL;
    cmd_hdr_t* cmd = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_AP_CHANNEL_NEW);

    tmp_head = (uint16*)(*app_data + sizeof(cmd_hdr_t));
    tmp_end = (uint16*)((uchar *)tmp_head + sizeof(uint16));

    if(prof != NULL)
    {	
	fp = popen("iwpriv ra0 get Channel", "r");
	if ( fp != NULL)
	{
	    fgets(result, sizeof(result), fp);
	    fgets(result, sizeof(result), fp);
	    memset(result , 0, sizeof(result));
	    fgets(result, sizeof(result), fp);
	    ret = sscanf(result,"Channel:%d", &channel);
	    if ((ret > 0) && (channel > 0))
	    {
		/*AP CHANNEL*/
		if (prof->mBaseCf.AutoChannelSelect != 0) 
		{
		    *tmp_head = htons(0);
		    *tmp_end = htons(channel);
		}	
		else 
		{
		    *tmp_head = htons(channel);
		}
    
	    }
	    else
	    {
		*tmp_head = htons(0);
		*tmp_end = htons(0);
	    }
            pclose(fp);
	}
	else
	{
	    /*执行本地获取命令失败*/
	    *tmp_head = htons(0);
	    *tmp_end = htons(0);
	}
	URCP_DEBUG(("AP CHANNEL = %d\n", channel));

	/*length*/
	cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
	cmd -> cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd -> length = sizeof(cmd_hdr_t);
	cmd -> cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_GET
uint16 get_ap_roaming_th(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
	DBCK();
	uint32 dbm, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;

    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_ROAMING_TH);

    tmp = (uint32 *)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
		DBCK("dbm = %d", prof->dbm);
		*tmp = htonl(prof->dbm);
		/*length*/
		cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
		cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
		cmd->length = sizeof(cmd_hdr_t);
		cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);
	DBCK();

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_SSID_GET
uint16 get_ap_ssid(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    int i;
    uint16 j=0;
    char *tmp;
    uint16 ret = URCP_OK;
    cmd_hdr_gen_t* cmd;
    wl_ssid_t wl_ssid[4];
    char if_name[32];
    uchar if_hw[MAC_LEN];
    FitApSzProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;
    WirelessProfile *profW = NULL;
    MibProfileType mibTypeW = MIB_PROF_WIRELESS;
    FatFitSwitchProfile *profApType = NULL;
    MibProfileType mibTypeApType = MIB_PROF_FAT_FIT_SWITCH;
    
    for (i=0; i<4; i++)
    {
	prof = (FitApSzProfile*)ProfGetInstPointByIndex(mibType,i);
	if (prof->head.active == TRUE)
	{
	    j++;
	}
    }
    cmd = (cmd_hdr_gen_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_AP_SSID);
    cmd -> length = sizeof(cmd_hdr_gen_t);

    tmp = *app_data + sizeof(cmd_hdr_gen_t);
    profApType = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibTypeApType, 0);
    if (profApType->fatEnable == 0) /* 瘦AP */
    {
	if (j == 0)
	{
	    cmd -> value = htonl(1);
	    profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW,0);
	    memset(&wl_ssid[0], 0, sizeof(wl_ssid_t));
	    memcpy(wl_ssid[0].ssid, profW->mBaseCf.SSID1, strlen(profW->mBaseCf.SSID1));
	    get_switch_if_name(if_name);
	    get_mac(if_name, if_hw);
	    memcpy(wl_ssid[0].mac, if_hw, MAC_LEN);
	    cmd -> length += (sizeof(wl_ssid_t)*1);
	}
	else
	{
		DBCK();
	    cmd -> value = htonl(j);
	    for (i=0; i<j; i++)
	    {
		DBCK("i = %d", i);
		prof = (FitApSzProfile*)ProfGetInstPointByIndex(mibType,i);
		profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW,(i+1));
		if((prof != NULL) && (prof->head.active == TRUE))
		{
			DBCK();
		    memset(&wl_ssid[i], 0, sizeof(wl_ssid_t));
		    memcpy(wl_ssid[i].ssid, profW->mBaseCf.SSID1, strlen(profW->mBaseCf.SSID1));
		    memcpy(wl_ssid[i].szone, prof->head.name, strlen(prof->head.name));
			DBCK("szone:%s, SSID=%s, i = %d\n", prof->head.name, profW->mBaseCf.SSID1, i);
		    get_switch_if_name(if_name);
		    get_mac(if_name, if_hw);
		    memcpy(wl_ssid[i].mac, if_hw, MAC_LEN);
		    cmd -> length += (sizeof(wl_ssid_t));
		}
	    }
	}
    }
    else    /* 胖AP */
    {
	cmd -> value = htonl(1);
	profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW,0);
	memset(&wl_ssid[0], 0, sizeof(wl_ssid_t));
	memcpy(wl_ssid[0].ssid, profW->mBaseCf.SSID1, strlen(profW->mBaseCf.SSID1));
	get_switch_if_name(if_name);
	get_mac(if_name, if_hw);
	memcpy(wl_ssid[0].mac, if_hw, MAC_LEN);
	cmd -> length += (sizeof(wl_ssid_t)*1);
    }
    ret = URCP_OK;
    //printf("\n< %s %d > cmd->length:%d wl_ssid_t:%d value:%d gen_t:%d\n", __func__, __LINE__, cmd->length, sizeof(wl_ssid_t), ntohl(cmd -> value), sizeof(cmd_hdr_gen_t));
    memcpy(tmp, &wl_ssid, sizeof(wl_ssid_t)*4);
    cmd -> cmd_status = (int16)htons((uint16)ret);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return ret;
}

uint16 trap_get_ap_ssid(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    int i;
    uint16 j=0;
    char *tmp;
    uint16 ret = URCP_OK;
    cmd_hdr_gen_t* cmd;
    ssid_t ssid[4];
    char if_name[32];
    uchar if_hw[MAC_LEN];
    FitApSzProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;
    WirelessProfile *profW = NULL;
    MibProfileType mibTypeW = MIB_PROF_WIRELESS;
    FatFitSwitchProfile *profApType = NULL;
    MibProfileType mibTypeApType = MIB_PROF_FAT_FIT_SWITCH;
    
    for (i=0; i<4; i++)
    {
	prof = (FitApSzProfile*)ProfGetInstPointByIndex(mibType,i);
	if (prof->head.active == TRUE)
	{
	    j++;
	}
    }
    cmd = (cmd_hdr_gen_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_AP_SSID);
    cmd -> length = sizeof(cmd_hdr_gen_t);

    tmp = *app_data + sizeof(cmd_hdr_gen_t);
    profApType = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibTypeApType, 0);
    if (profApType->fatEnable == 0) /* 瘦AP */
    {
	if (j == 0)
	{
	    cmd -> value = htonl(1);
	    profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW,0);
	    memset(&ssid[0], 0, sizeof(ssid_t));
	    memcpy(ssid[0].ssid, profW->mBaseCf.SSID1, strlen(profW->mBaseCf.SSID1));
//	    get_switch_if_name(if_name);
        sprintf(if_name,"ra0");
	    get_mac(if_name, if_hw);
	    memcpy(ssid[0].mac, if_hw, MAC_LEN);
	    cmd -> length += (sizeof(ssid_t)*1);
	}
	else
	{
		DBCK();
	    cmd -> value = htonl(j);
	    for (i=0; i<j; i++)
	    {
		DBCK("i = %d", i);
		prof = (FitApSzProfile*)ProfGetInstPointByIndex(mibType,i);
		profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW,(i+1));
		if((prof != NULL) && (prof->head.active == TRUE))
		{
			DBCK();
		    memset(&ssid[i], 0, sizeof(ssid_t));
		    memcpy(ssid[i].ssid, profW->mBaseCf.SSID1, strlen(profW->mBaseCf.SSID1));
		    memcpy(ssid[i].szone, prof->head.name, strlen(prof->head.name));
			DBCK("szone:%s, SSID=%s, i = %d\n", prof->head.name, profW->mBaseCf.SSID1, i);
//		    get_switch_if_name(if_name);
            sprintf(if_name,"ra%d",i);
		    get_mac(if_name, if_hw);
		    memcpy(ssid[i].mac, if_hw, MAC_LEN);
		    cmd -> length += (sizeof(ssid_t));
		}
	    }
	}
    }
    else    /* 胖AP */
    {
	cmd -> value = htonl(1);
	profW = (WirelessProfile*)ProfGetInstPointByIndex(mibTypeW,0);
	memset(&ssid[0], 0, sizeof(ssid_t));
	memcpy(ssid[0].ssid, profW->mBaseCf.SSID1, strlen(profW->mBaseCf.SSID1));
//	get_switch_if_name(if_name);
    sprintf(if_name,"ra0");
	get_mac(if_name, if_hw);
	memcpy(ssid[0].mac, if_hw, MAC_LEN);
	cmd -> length += (sizeof(ssid_t)*1);
    }
    ret = URCP_OK;
    memcpy(tmp, &ssid, sizeof(ssid_t)*4);
    cmd -> cmd_status = (int16)htons((uint16)ret);
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_CURRENT_CLIENTS_GET
/*
 * get wireless AP clients
 *
 * added by Shiyu.Chen, 2012.0620
 */
static uint32 get_wl_hosts(void)
{
    int s;
    struct iwreq iwr;
    uint32 host_no = 0u;
    RT_802_11_MAC_TABLE table = {0};
    
    URCP_DEBUG(("%s()\n", __func__));

    s = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) &table;

    if (s >= 0)
    {
#ifdef CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) >= 0)
#else
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) >= 0)
#endif
	{
	    host_no = table.Num;
	}
	close(s);
    }

    return host_no;
}

/*
 * get AP active clients
 *
 * added by Shiyu.Chen, 2012.06.20
 */
uint16 get_ap_clients(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    uint32 max = 0u, *tmp;
    cmd_hdr_t* cmd;

    URCP_DEBUG(("%s()\n", __func__));

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(CMD_CURRENT_CLIENTS);
    cmd -> cmd_status = htons(URCP_OK);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    /*AP active clients*/
    max = get_wl_hosts();

    URCP_DEBUG(("AP clients = %d\n", max));
    *tmp = htonl(max);

    /*length*/
    cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
    *tmp = htonl(max);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return 0;
}

uint16 trap_get_ap_clients(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    uint32 max = 0u, *tmp;
    cmd_hdr_t* cmd;

    URCP_DEBUG(("%s()\n", __func__));

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_AP_CLIENTS);
    cmd -> cmd_status = htons(URCP_OK);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    /*AP active clients*/
    max = get_wl_hosts();
    URCP_DEBUG(("AP clients = %d\n", max));
    *tmp = htonl(max);

    /*length*/
    cmd -> length = sizeof(cmd_hdr_t) + sizeof(uint32);
    *tmp = htonl(max);
    /*数据长度*/
    *app_data += cmd -> length;
    cmd -> length = htons(cmd->length);

    return 0;
}
#endif

#ifdef URCP_CMD_AP_DHCP_EN_GET
uint16 get_ap_dhcp_en(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
	DBCK();
	uint32 dhcp_en, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;

    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_DHCP_EN);

    tmp = (uint32 *)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
		DBCK("dhcp_en = %d", prof->dhcp_en);
		*tmp = htonl(prof->dhcp_en);

		/*length*/
		cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
		cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
		cmd->length = sizeof(cmd_hdr_t);
		cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);
	DBCK();

    return URCP_OK;
}

uint16 trap_get_ap_dhcp_en(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
	DBCK();
	uint32 dhcp_en, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;

    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(TRAP_AP_DHCP_EN);

    tmp = (uint32 *)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
		DBCK("dhcp_en = %d", prof->dhcp_en);
		*tmp = htonl(prof->dhcp_en);

		/*length*/
		cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
		cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
		cmd->length = sizeof(cmd_hdr_t);
		cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);
	DBCK();

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_AP_WORK_MODE_GET
uint16 get_ap_work_mode(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    uint32 mode, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
    FatFitSwitchProfile *prof= NULL;
    prof = (FatFitSwitchProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_WORK_MODE);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP WORKING MODE*/
	mode = prof->fatEnable;
	if(mode==0)
	{
	    mode = 1;
	}
	else
	{
	    mode = 0;
	}
	URCP_DEBUG(("AP WORK MODE = %d\n", mode));
	*tmp = htonl(mode);

	/*length*/
	cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
	cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd->length = sizeof(cmd_hdr_t);
	cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#if 0
int32 get_ap_dev_name(uchar* app_data, uint32* ulen)
{   
    char *name, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;
    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)app_data;
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_DEV_NAME);
    cmd->cmd_status = htons(URCP_OK);

    tmp = (char *)(app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP DEV NAME*/
	strcpy(name, prof->dev_name);
	URCP_DEBUG(("AP dev_name = %s\n", prof->dev_name));
	strcpy(tmp, name);;

	/*length*/
	cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    }
    else
    {
	cmd->length = sizeof(cmd_hdr_t);
    }
    /*数据长度*/
    *ulen += cmd->length;
    cmd->length = htons(cmd->length);

    return 0;

}
#endif
#ifdef URCP_CMD_IP_ADDR_GET
uint16 get_ip_addr(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    char *name;
    uint32 ip, *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_IP_ADDR);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    name = getLanIfName();
    get_ip(name, &ip);

    /*AP IP*/
    URCP_DEBUG(("AP ip = %d\n", ip));
    *tmp = ip;

    /*length*/
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    cmd->cmd_status = htons(URCP_OK);
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_NETMASK_GET
uint16 get_netmask_addr(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    char *name;
    uint32 ip, *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_NETMASK);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    name = getLanIfName();
    get_mask(name, &ip);

    /*AP IP*/
    URCP_DEBUG(("AP netmask = %d\n", ip));
    *tmp = ip;

    /*length*/
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    cmd->cmd_status = htons(URCP_OK);
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);


    return URCP_OK;
}
#endif
#ifdef URCP_CMD_GATEWAY_GET
uint16 get_gateway_addr(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    uint32 *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_GATEWAY);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 1);
    if ((profIF != NULL) && (ProfInstIsFree((Pointer)profIF)==0) && (profIF->head.active == TRUE))
    {
	/*AP gateway*/
	*tmp = profIF->ether.sta.gw;
    }

    /*length*/
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    cmd->cmd_status = htons(URCP_OK);
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_USER_NAME_GET
uint16 get_ap_user_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    char *tmp, buf[MAX_USER_NAME_LEN+1];
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;

    URCP_DEBUG(("%s\n", __func__));
    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_USER_NAME);

    tmp = (char *)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP USER NAME*/
	memset(buf, 0, sizeof(buf));
	strcpy(buf, "admin");
	strcpy(tmp, buf);
	URCP_DEBUG(("AP user_name = %s\n", tmp));

	/*length*/
	cmd->length = sizeof(cmd_hdr_t) + strlen(buf);
	cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd->length = sizeof(cmd_hdr_t);
	cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_PASSWORD_GET
uint16 get_ap_password(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    char *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;
    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_PASSWORD);

    tmp = (char *)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP PASSWORD*/
	strncpy(tmp, prof->password, MAX_PASSWORD_LEN);
	URCP_DEBUG(("AP password = %s\n", tmp));

	/*length*/
	cmd->length = sizeof(cmd_hdr_t) + MAX_PASSWORD_LEN;
	cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd->length = sizeof(cmd_hdr_t);
	cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_GET
uint16 get_ap_max_client(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    printf("%s\n", __func__);
#if 0
    uint32 num, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    ApBasicConfProfile *prof= NULL;
    prof = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_MAX_CLIENT);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP MAX CLIENT*/
	num = prof->max_clt;
	URCP_DEBUG(("AP max client = %d\n", num));
	*tmp = htonl(num);

	/*length*/
	cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
	cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd->length = sizeof(cmd_hdr_t);
	cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);
#endif

    return URCP_OK;
}

uint16 get_ap_max_client_new(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 num, i = 0;
    uint16 *tmp = NULL, *num_sz = NULL;
    cmd_hdr_t* cmd;
    uchar *name = NULL;
//    WirelessProfile *prof = NULL;
//    MibProfileType mibType = MIB_PROF_WIRELESS;
    FitApSzProfile *profS = NULL;
    MibProfileType mibTypeS = MIB_PROF_FIT_AP_SZ;
    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_MAX_CLIENT_NEW);

    tmp = (uint16 *)(*app_data + sizeof(cmd_hdr_t));	    //总的最大客户端数
    num_sz = tmp + 1;				    //服务区的个数
    name = (uchar *)(num_sz + 1);
    *tmp = htons(AP_MAX_CLIENT_NUM);
    cmd->length += sizeof(cmd_hdr_t) + 2*sizeof(uint16);
    for (i = 0; i < MAX_FIT_AP_SERVICE_ZONE_PROFILES; i++)
    {
	profS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeS, i);
	if ((profS != NULL) && (profS->head.name[0] != '\0'))
	{	    
	    strncpy(name, profS->head.name, UTT_INST_NAME_LEN);
	    name += UTT_INST_NAME_LEN;
	    *name = (uchar)(profS->client);
//	    printf("%s,%d\n",__func__, *name);
	    name += 1;
	}
	else
	{
	    break;
	}
    }
    *num_sz = htons(i);
    cmd->length += i*(UTT_INST_NAME_LEN + 1);

    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

//    printf("%s, %d, %d, %d\n", __func__, i, AP_MAX_CLIENT_NUM, ntohs(cmd->length));
    return URCP_OK;

}

#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_GET
uint16 get_ap_effect_conf(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{   
    uint32 mode, *tmp;
    cmd_hdr_t* cmd;
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;
    ApEffectConfProfile *prof= NULL;
    prof = (ApEffectConfProfile*)ProfGetInstPointByIndex(mibType,0);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_EFFECT_CONF);

    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    if(prof != NULL)
    {	
	/*AP  EFFECT CONF*/
	mode = prof->config;
	//printf("AP effect conf = %d\n", mode);
	*tmp = htonl(mode);

	/*length*/
	cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
	cmd->cmd_status = htons(URCP_OK);
    }
    else
    {
	cmd->length = sizeof(cmd_hdr_t);
	cmd->cmd_status = htons(URCP_ANY_ERROR);
    }
    /*数据长度*/
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif

#ifdef URCP_CMD_AP_DHCP_EN_SET
uint16 set_dhcp_en_config(cmd_hdr_t *cmd, uchar** app_data)
{
	DBCK();
    uchar buf[1];
	uint16 ret = 0;
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
	uint32 dhcp_en;
    uint16 len;
    uint32 isChange = 0u;
	FILE *fp = NULL;
	extern ap_info_t ap_infor;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar *)cmd;
    /*复制数据*/
    dhcp_en = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));/*指向接受包的数据部分*/
	dhcp_en = ntohl(dhcp_en);
	//printf("dhcp_en = %d\n", dhcp_en);

    ApBasicConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;

	MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
	InterfaceProfile *profIF = NULL;

	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 1);
	if ((profIF != NULL) || (profIF->head.active != 0)) {
		if (dhcp_en == 1) {
			profIF->ether.connMode = ETH_CONN_DHCP;
		} else {
			profIF->ether.connMode = ETH_CONN_STATIC;
		}
	}

    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType,0);
    if(prof != NULL)
    {
	if (dhcp_en != prof->dhcp_en)
	{
		prof->dhcp_en = dhcp_en;
	    isChange = 1u;
	}
	else
	{
            isChange = 0u;
	}
    }
    else
    {
	ret = -1;
    }
    if(isChange == 1u)
    {
		if (prof->dhcp_en == 1) {
			/*开启DHCP*/
			get_ip_from_dhcp_server(ap_infor.ac_infor.ip);
		} else {
			/* kill udhcpc 的过程放到设置IP函数中处理 */
#if 0
			/*关闭DHCP*/
			fp = popen("killall udhcpc 1>/dev/null 2>&1", "r");
			if (fp != NULL) {
				pclose(fp);
			}
#endif
		}
    }

	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

	DBCK();
    return ret;
}
#endif

#ifdef URCP_CMD_AP_WORK_MODE_SET
uint16 set_work_mode_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    char xml_buf[MAX_VALUE_LEN + 1];
    cmd_hdr_t* scmd = NULL;
    uint32 mode, isChange=0u;
    int32 ret = 0;
    struct msgbuf_t msgbuf;
    request_cmd_msg_t request_cmd_msg;
    ApEffectConfProfile *profApEffect = NULL;
    MibProfileType mibTypeApEffect = MIB_PROF_AP_EFFECT_CONF;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    /*copy指向接受包的数据部分*/
    mode = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));
    mode = ntohl(mode);
#if(FAT_FIT_SWITCH == FYES)
    FatFitSwitchProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;

    //struProfAlloc *profhead = NULL;
    //ProfChangeType profAction = PROF_CHANGE_EDIT;
    prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType,0);
    profApEffect = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibTypeApEffect, 0);
    if(prof != NULL)
    {
	//ProfBackupByIndex(mibType, profAction, 0, &profhead);
	if(mode==1)
	{
	    mode=0;
	}
	else
	{
	    mode=1;
	}
	if (mode == prof->fatEnable)
	{
	    isChange = 0u;
	}
	else
	{
	    prof->fatEnable = mode;
	    isChange = 1u;
	}
    }
    else
    {
	ret = -1;
    }
    //printf("\n< %s %d > mode:%d\n",__func__,__LINE__,mode);
    if(isChange == 1u)
    {
	if ((prof->fatEnable != 0) && (PROF_DISABLE == profApEffect->config))   //fit --> fat && nonLocal take effect
	{
	    memset(request_cmd_msg.content, '\0', sizeof(request_cmd_msg.content));
	    memset(xml_buf, '\0', sizeof(xml_buf));
	    request_cmd_msg.opt_type = REQUEST_SEND_FILE;
	    request_cmd_msg.sn = getSystemSn();
	    request_cmd_msg.name_len = 0;
	    request_cmd_msg.xml_len = 0;
	    memset(request_cmd_msg.content, '\0', sizeof(request_cmd_msg.content));
	    memcpy(msgbuf.sw_msg.buffer,(char *)&request_cmd_msg,sizeof(request_cmd_msg));
	    memset(xml_buf, '\0', sizeof(xml_buf));
	    urcp_msg_send_prof_to_ac(&msgbuf);
	    sleep(3u);
	    if ((apStatusForAcOperate == URCP_STATUS_OK) && (apStatusForAcOperate == URCP_STATUS_FOUND_NONE))
	    {
		makeFactoryProfiles();
		ProfReadFromFlash();
	    }
	}
	//ProfUpdate(profhead);
	//ProfFreeAllocList(profhead);
	nvramWriteCommit();
	sleep(1u);
	system("reboot");
    }
#endif
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_DEV_NAME_SET
uint16 set_dev_name_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uchar dev_name[MAX_DEV_NAME_LEN];
    int32 ret = 0;
    uint16 len;
    uint32 isChange = 0u;
#if (AP_SET_HOST_NAME == FYES)
    uchar setBuf[50] = {0};
#endif

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    memset(dev_name, 0, sizeof(dev_name));
    len = cmd->length - sizeof(cmd_hdr_t);
    if(len>=MAX_DEV_NAME_LEN)
    {
	len=MAX_DEV_NAME_LEN- 1;
    }
    strncpy(dev_name, (tmp + sizeof(cmd_hdr_t)), len);
				    
    URCP_DEBUG(("%s: %s\n",__func__, dev_name));

    ApBasicConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType,0);
    if(prof != NULL)
    {
	if (strcmp(dev_name, prof->dev_name)!=0)
	{
	    strcpy(prof->dev_name, dev_name);
	    isChange = 1u;
#if (AP_SET_HOST_NAME == FYES)
	    sprintf(setBuf,"hostname %s", dev_name);
	    system(setBuf);
#endif
	}
	else
	{
            isChange = 0u;
	}
    }
    else
    {
	ret = -1;
    }
    if(isChange == 1u)
    {
	//nvramWriteCommit();
    }

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif

#ifdef URCP_CMD_WIRELESS_CLT_GET
static void get_ap_name(char **dev_name)
{
    ApBasicConfProfile *prof = NULL;

    /*get name */
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF,0);
	if (NULL != prof) {
		*dev_name = prof->dev_name;
	}

	return;
}

static void get_fat_ap_wl_info(char **ssid, char **wl_mode)
{
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (NULL != prof) {
		ssid[0] = prof->mBaseCf.SSID1;

		if (4 == prof->mBaseCf.WirelessMode) {
			*wl_mode = "11g";
		} else if (6 == prof->mBaseCf.WirelessMode) {
			*wl_mode = "11n";
		} else {
			*wl_mode = "11b/g/n";
		}
	}

	return;
}

static int get_fit_ap_wl_info(char **ssid, char **wl_mode)
{
	int flag = 0;
	int max, min, index, i = 0;
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

	ProfInstLowHigh(mibType, &max, &min);
	DBCK("min = %d, max = %d", min, max);

	for (index = min; index < max; index++) {
		prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, index);
		DBCK();
		if ((NULL != prof) && (prof->head.active == TRUE)) {
			ssid[i] = prof->mBaseCf.SSID1;
			DBCK("ssid[%d] = %s", i, ssid[i]);

			if (0 == flag) {
				if (4 == prof->mBaseCf.WirelessMode) {
					wl_mode[i] = "11g";
				} else if (6 == prof->mBaseCf.WirelessMode) {
					wl_mode[i] = "11n";
				} else {
					wl_mode[i] = "11b/g/n";
				}

				flag = 1;
			}

			i++;
		}
	}

	return i;
}

static void get_ap_clt_info(char mac[2048])
{
    int s, i = 0, ssid_num = 0;
#if 0
	char interface[10];
#endif
	char *dev_name = NULL, *wl_mode = NULL;
	char tmp[128], *ssid[5] = {NULL, NULL, NULL, NULL, NULL}, *fit_ap_wl[5] = {NULL, NULL, NULL, NULL, NULL};
    struct iwreq iwr;
    RT_802_11_MAC_TABLE table = {0};
	MibProfileType mibType = MIB_PROF_FAT_FIT_SWITCH;
	FatFitSwitchProfile *prof = NULL;

    s = socket(AF_INET, SOCK_DGRAM, 0);
	DBCK("socket = %d", s);

	get_ap_name(&dev_name);

	if (s >= 0) {
		prof = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibType, 0);
		if (NULL != prof) {
			if (1 == prof->fatEnable) {
				DBCK();
			// fat AP
				get_fat_ap_wl_info(ssid, &wl_mode);

				strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
				iwr.u.data.pointer = (caddr_t) &table;
#ifdef CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
		                ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr);
#else
                                ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr);
#endif
				sprintf(mac, "%s,%s,%s,%ld,", dev_name, ssid[0], wl_mode, table.Num);
				DBCK("mac = %s", mac);
				for (i = 0; i < table.Num; i++) {
					DBCK();
					memset(tmp, '\0', 64);
					sprintf(tmp, "%02X%02X%02X%02X%02X%02X", table.Entry[i].Addr[0],
							table.Entry[i].Addr[1], table.Entry[i].Addr[2],
							table.Entry[i].Addr[3], table.Entry[i].Addr[4],
							table.Entry[i].Addr[5]);
					strcat(tmp, ",");
					strcat(mac, tmp);
					DBCK("mac = %s\n", mac);
				}

				strcat(mac, "\n");
			} else {
				DBCK();
			// fit AP
				ssid_num = get_fit_ap_wl_info(ssid, fit_ap_wl);

#if 0
				for (j = 0; j < ssid_num; j++) {
				//	sprintf(interface, "ra%d", j);
				//	sprintf(interface, "ra0");
				//	strncpy(iwr.ifr_name, interface, IFNAMSIZ);
					strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
					iwr.u.data.pointer = (caddr_t) &table;
					DBCK("IOCTL = %d", ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr));
					sprintf(tmp, "%s,%s,%s,%ld,", dev_name, ssid[j], fit_ap_wl[0], table.Num);
					strcat(mac, tmp);
					DBCK("%s", tmp);
					for (i = 0; i < table.Num; i++) {
						memset(tmp, '\0', 64);
						sprintf(tmp, "%02X%02X%02X%02X%02X%02X", table.Entry[i].Addr[0],
								table.Entry[i].Addr[1], table.Entry[i].Addr[2],
								table.Entry[i].Addr[3], table.Entry[i].Addr[4],
								table.Entry[i].Addr[5]);
						strcat(tmp, ",");
						strcat(mac, tmp);
						DBCK("mac = ", mac);
					}
					strcat(mac, "\n");
			}
#else
					strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
					iwr.u.data.pointer = (caddr_t) &table;
#ifdef CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
		                ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr);
#else
                                ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr);
#endif
					sprintf(tmp, "%s,%s,%s,%ld,", dev_name, ssid[0], fit_ap_wl[0], table.Num);
					strcat(mac, tmp);
					DBCK("%s", tmp);
					for (i = 0; i < table.Num; i++) {
						memset(tmp, '\0', 64);
						sprintf(tmp, "%02X%02X%02X%02X%02X%02X", table.Entry[i].Addr[0],
								table.Entry[i].Addr[1], table.Entry[i].Addr[2],
								table.Entry[i].Addr[3], table.Entry[i].Addr[4],
								table.Entry[i].Addr[5]);
						strcat(tmp, ",");
						strcat(mac, tmp);
						DBCK("mac = %s\n", mac);
					}
#endif
					strcat(mac, "\n");
			}
		}

		close(s);
	}

	return;
}

uint16 get_wl_clt(cmd_hdr_t *cmd_buf, uchar** app_data, urcp_infor_t* urcp_info)
{
	DBCK();
	char mac[2048];
	char *tmp;
	uint16 ret;
	cmd_hdr_t *cmd;

	cmd = (cmd_hdr_t *)(*app_data);

	cmd->type = htonl(CMD_WIRELESS_CLT);
	cmd->length = sizeof(cmd_hdr_t);

	memset(mac, 0, sizeof(mac));

	get_ap_clt_info(mac);

	DBCK();
	if (0 != strcmp(mac, "")) {
		tmp = *app_data + sizeof(cmd_hdr_t);
		memcpy(tmp, mac, strlen(mac));

		cmd->length += strlen(mac);
		ret = URCP_OK;
	} else {
		ret = URCP_ANY_ERROR;
	}

	cmd->cmd_status = (int16)htons((uint16)ret);
	*app_data += cmd->length;
	cmd->length = htons(cmd->length);

	return ret;
}

#endif

#ifdef URCP_CMD_WIRELESS_CLT_GET_REPLY
uint16 receive_wl_clt(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
	DBCK();
	int fd, ret;
	char *tmp;
	char *buf[2048];
	char *file = "/tmp/urcp/ap_client_info";
	uint32 len = 0;

	memset(buf, '\0', sizeof(buf));

	if ((NULL != dev) && (NULL != *dev)) {
		DBCK();
		len = ntohs(cmd->length) - sizeof(cmd_hdr_t);
		tmp = (char *)(cmd + 1);

		memcpy(buf, tmp, len);
	}

	fd = open(file, O_RDWR | O_CREAT | O_APPEND, 0644);
	if (fd >= 0) {
		DBCK();
		ret = write(fd, buf, len);
	}

	close(fd);

	return URCP_OK;
}
#endif

#ifdef URCP_CMD_IP_ADDR_SET
/*
 * 判断两个ip地址段是否重叠
 * 输入参数：
 *  ipS1,ipE1: 第一个ip地址段的起始ip和结束ip, 主机字节序
 *  ipS2,ipE2: 第二个ip地址段的起始ip和结束ip, 主机字节序
 * 返回值:
 *  TRUE：     两个ip地址段重叠
 *  FALSE:     两个ip地址段不重叠
 */
static int ipSegmentOverlapUrcp(Uint32 ipS1, Uint32 ipE1, Uint32 ipS2, Uint32 ipE2) {

    if (ipS1 >= ipS2 && ipS1 <= ipE2) {
	return TRUE;
    }
    if (ipE1 >= ipS2 && ipE1 <= ipE2) {
	return TRUE;
    }
    if (ipS2 >= ipS1 && ipS2 <= ipE1) {
	return TRUE;
    }
    if (ipE2 >= ipS1 && ipE2 <= ipE1) {
	return TRUE;
    }
    return FALSE;
}

/*
 * 检验地址池是否冲突
 * 输入参数:
 *  poolType:	要检测的地址池类型，即输入的ipS和ipE所属的pool类型dhcp,pppoe,l2tp or pptp
 *  ipS:	起始Ip地址, 网络字节序
 *  ipE:	结束ip地址, 网络字节序
 * 输出参数：	无
 * 返回值：
 *  与输入地址池冲突的地址池类型
 */
extern IpPoolEnum ipPoolConflict(IpPoolEnum poolType, Uint32 ipS, Uint32 ipE) {
    Uint32 ipStart = ntohl(ipS);
    Uint32 ipEnd = ntohl(ipE);

#if (DHCP_SERVER == FYES)
    /*
     * 是否与dhcp server地址池冲突
     */
    if (poolType != IP_POOL_DHCP) {
	MibProfileType mibTypeDhcp = MIB_PROF_DHCP_SERVER;
	DhcpServerProfile *profDhcp = NULL;
	profDhcp = (DhcpServerProfile *)ProfGetInstPointByIndex(mibTypeDhcp, 0);
	if ((profDhcp != NULL) && (ProfInstIsFree(profDhcp) == 0)) {
	    if (ipSegmentOverlapUrcp(ipStart, ipEnd, ntohl(profDhcp->ipStart), ntohl(profDhcp->ipEnd)) == TRUE) {
		return IP_POOL_DHCP;
	    }
	}
    }
#endif
#if (PPPOE_SERVER == FYES)
    /*
     * 是否与pppoe server地址池冲突
     */
    if (poolType != IP_POOL_PPPOE) {
	MibProfileType mibTypePppoe = MIB_PROF_PPPOE_SRV_GLO;
	PPPoESrvGloProfile *profPppoe = NULL;
	Uint32 pppoeIpE;
	profPppoe = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(mibTypePppoe, 0);
	if ((profPppoe != NULL) && (ProfInstIsFree(profPppoe) == 0)) {
	    if (profPppoe->maxSessCnt == 0) {
		pppoeIpE = ntohl(profPppoe->ipStart);
	    } else {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->ipcount - 1;
#else
		pppoeIpE = ntohl(profPppoe->ipStart) + profPppoe->maxSessCnt - 1;
#endif
	    }
	    if (ipSegmentOverlapUrcp(ipStart, ipEnd, ntohl(profPppoe->ipStart), pppoeIpE) == TRUE) {
		return IP_POOL_PPPOE;
	    }
	}
    }
#endif
#if (L2TP_SERVER == FYES)
    /*
     * 是否与l2tp server地址池冲突
     */
    if (poolType != IP_POOL_L2TP) {
	MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
	L2TPSrvGlobalProfile *profL2tp = NULL;
	Uint32 l2tpIpE;
	profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
	if ((profL2tp != NULL) && (ProfInstIsFree(profL2tp) == 0)) {
	    if (profL2tp->poolIpCnt == 0) {
		l2tpIpE = ntohl(profL2tp->poolIpStart);
	    } else {
		l2tpIpE = ntohl(profL2tp->poolIpStart) + profL2tp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlapUrcp(ipStart, ipEnd, ntohl(profL2tp->poolIpStart), l2tpIpE) == TRUE) {
		return IP_POOL_L2TP;
	    }
	}

    }
#endif
#if (PPTP_SERVER == FYES)
    /*
     * 是否与pptp server地址池冲突
     */
    if (poolType != IP_POOL_PPTP) {
	MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
	PPTPSrvGlobalProfile *profPptp = NULL;
	Uint32 pptpIpE;
	profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
	if ((profPptp != NULL) && (ProfInstIsFree(profPptp) == 0)) {
	    if (profPptp->poolIpCnt == 0) {
		pptpIpE = ntohl(profPptp->poolIpStart);
	    } else {
		pptpIpE = ntohl(profPptp->poolIpStart) + profPptp->poolIpCnt - 1;
	    }

	    if (ipSegmentOverlapUrcp(ipStart, ipEnd, ntohl(profPptp->poolIpStart), pptpIpE) == TRUE) {
		return IP_POOL_PPTP;
	    }
	}

    }
#endif
    return IP_POOL_NONE;
}

int32 set_ip_address_config(uint32 lan_ip)
{
	DBCK();
    int32 ret = 0, update=0;
#ifdef URCP_AP
    uchar str_gw[16]={'\0'};
    char *name;
    uint32 ip;
    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    URCP_DEBUG(("%s\n",__func__));
    profAction = PROF_CHANGE_EDIT;
    /* 备份lan口配置 */
    ProfBackupByIndex(mibTypeIF, profAction, 1, &profhead);
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 1);
    ip = htonl(lan_ip);
    if ((profIF != NULL) && (ProfInstIsFree((Pointer)profIF)==0) && (profIF->head.active == TRUE))
    {
		DBCK();
	    profIF->ether.sta.ip = ip;
	    update = 1;
	    name = getLanIfName();
	    set_ip(name, ip);

	    int2ip(ntohl(profIF->ether.sta.gw), str_gw);
	    //printf("\n< %s %d > str_gw:%s\n",__func__,__LINE__,str_gw);
	    set_gw(str_gw);
    }
#else

#if ((DHCP_SERVER == FYES) || (NET_SECURITY == FYES))
    unsigned int nIp, eIp, uNet, nIp1, eIp1;
#endif
    IpPoolEnum cfPoolType = IP_POOL_NONE;
    Uint32 lanIp, wanIp, wanMask;
    int32 i = 0;
    Uint32 wanCount = 0;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    ProfChangeType profAction;
    struProfAlloc *profhead = NULL;

#if (DHCP_SERVER == FYES)
    MibProfileType mibTypeDhcpS = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *profDhcpS = NULL;
#endif
#if (STATIC_NAT == FYES)
    MibProfileType mibTypeNat = MIB_PROF_STATIC_NAT;
    StaticNatProfile *profNat = NULL;
#endif
#if (L2TP_SERVER == FYES)
    MibProfileType mibTypeL2tp = MIB_PROF_L2TP_SRV_GLOBAL;
    L2TPSrvGlobalProfile *profL2tp = NULL;
#endif
#if (PPTP_SERVER == FYES)
    MibProfileType mibTypePptp = MIB_PROF_PPTP_SRV_GLOBAL;
    PPTPSrvGlobalProfile *profPptp = NULL;
#endif
#if (NET_SECURITY == FYES)
    SystemProfile *profSys = NULL;
#endif

    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF != NULL) && (ProfInstIsFree((Pointer)profIF)) && (profIF->head.active == TRUE))
    {
	if(profIF->ether.sta.ip == lan_ip)
	{
	    goto out;
	}
    }

    lanIp = ntohl(lan_ip);
    wanCount = getWanSlotCount();
    /* 判断lan口ip和wan口ip配置是否重叠,wan口固定IP情况 */
    for (i = 1; i <= wanCount; i++) 
    {
	profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, i);
	if ((profIF == NULL) || (profIF->head.active == 0)) 
	{
	    continue;
	}

	/* wan固定IP */
	if (profIF->ether.connMode == ETH_CONN_STATIC) 
	{
	    wanIp = ntohl(profIF->ether.sta.ip);
	    wanMask = ntohl(profIF->ether.sta.nm);
	    URCP_DEBUG(("%s:wanIp=0x%x, wanMask=0x%x\n", __func__, wanIp, wanMask));
	    /* 检测是否和wan口在同一网段 */
	    if ((wanIp & wanMask) == (lanIp & wanMask)) 
	    {
		URCP_DEBUG(("LAN口IP地址不能与WAN口IP在同一网段"));
		ret = 1;
		goto out;
	    }
	}
    }

    /* 获得lan口profile */
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
    if ((profIF == NULL) || (profIF->head.active == 0)) 
    {
	URCP_DEBUG(("LAN配置失败"));
	ret = 1;
	goto out;
    }
    /* lan ip地址是否和地址池冲突 */
#if ((L2TP_SERVER == FYES) || (PPTP_SERVER == FYES) || (PPPOE_SERVER == FYES))
    cfPoolType = ipPoolConflict(IP_POOL_NONE, htonl(lanIp), htonl(lanIp));
    if (cfPoolType != IP_POOL_NONE) 
    {
	setPoolConflictMsg(cfPoolType);
	ret = 1;
	goto out;
    }
#endif

#if (DHCP_SERVER == FYES)
    /* 获得dhcp起始结束ip */
    profDhcpS = (DhcpServerProfile*)ProfGetInstPointByIndex(mibTypeDhcpS, 0);
    if (profDhcpS == NULL) 
    {
	URCP_DEBUG(("DHCP服务器错误"));
	ret = 1;
	goto out;
    }

    nIp = ntohl(profDhcpS->ipStart);
    eIp = ntohl(profDhcpS->ipEnd);
    /* 主机序，lan口网段, dhcp地址池网段 */
    uNet = lanIp & 0xffffff00;
    /* dhcp地址池随lanIp变化后的地址,主机字节序 */
    nIp1 = uNet + (nIp & 0x000000ff);
    eIp1 = uNet + (eIp & 0x000000ff);

    URCP_DEBUG(("%s: nIp=0x%x,eIp=0x%x,uNet=0x%x,nIp1=0x%x,eIp1=0x%x\n", __func__ , nIp, eIp, uNet, nIp1, eIp1));

    if((lanIp <= eIp1) && (lanIp >= nIp1))
    {
	URCP_DEBUG(("DHCP地址池包含了LAN口地址"));
	ret = 1;
	goto out;
    }
    /* 判断地址池 */
    cfPoolType = ipPoolConflict(IP_POOL_DHCP, htonl(nIp1), htonl(eIp1));
    if (cfPoolType != IP_POOL_NONE) 
    {
	URCP_DEBUG(("与服务器地址池冲突"));
	ret =1;
	goto out;
    }

    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibTypeDhcpS, profAction, 0, &profhead);
    if(profDhcpS->gateway != htonl(lanIp))
    {
	/* IP地址段修改 */
	profDhcpS->ipStart = htonl(nIp1);
	profDhcpS->ipEnd = htonl(eIp1);
	profDhcpS->gateway = htonl(lanIp);
    }
    /* dns代理开启，主dns修改为lan口ip地址 */
    if (profDhcpS->DnsProxyEn == FUN_ENABLE) 
    {
	profDhcpS->pDns = htonl(lanIp);
    }
#if 0
    /* modify dhcp address range */
    if((strncmp(nm, "255.255.255.", 12) == 0) && (((lanIp & 0x000000FFU) < 100) || ((lanIp & 0x000000FFU) > 200))) 
    {
	profDhcpS->ipStart = htonl((lanIp & 0xffffff00) | 100);
	profDhcpS->ipEnd = htonl((lanIp & 0xffffff00) | 200);
	profDhcpS->gateway = htonl(lanIp);
	profDhcpS->pDns = htonl(lanIp);
	URCP_DEBUG(("%s: ipS=0x%x, ipE=0x%x, gw=0x%x, dns=0x%x\n", __func__, profDhcpS->ipStart, profDhcpS->ipEnd, profDhcpS->gateway, profDhcpS->pDns));
    }
#endif
#endif
#if (NET_SECURITY == FYES)
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    nIp = ntohl(profSys->sysNetSe.accessCtrlFromIP);
    eIp = ntohl(profSys->sysNetSe.accessCtrlEndIP);
    uNet = lanIp & profIF->ether.sta.nm;
    /*网段变化。则清设备访问控制*/
    if(((nIp& profIF->ether.sta.nm)!=uNet) || ((eIp& profIF->ether.sta.nm)!=uNet))
    {
	/*这里不需备份实例，触发speedweb，设备访问相关操作
	因设备访问控制相关变化。会根据speedweb中的lan口初始化函数变化*/
	profSys->sysNetSe.accessCtrlFromIP = 0u;
	profSys->sysNetSe.accessCtrlEndIP = 0u;
    }
#endif

#if (STATIC_NAT == FYES)
    /* 远程管理静态nat 内网ip根据lan变化 */
    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, remCtrlName);
    if (profNat != NULL) 
    {
#if 0
	/* lan口profile change会更新nat,所以不用再调用profile change */
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByName(mibTypeNat, profAction, remCtrlName, &profhead);
#endif
	profNat->IP = htonl(lanIp);
	URCP_DEBUG(("%s: profNat->IP=0x%x\n", __func__, profNat->IP));
    }
#endif

    /* 备份lan口配置 */
    profAction = PROF_CHANGE_EDIT;
    ProfBackupByIndex(mibTypeIF, profAction, 0, &profhead);
    profIF->ether.sta.ip = htonl(lanIp);
		    
#if (L2TP_SERVER == FYES)
    /* 监听ip变化 */
    profL2tp = (L2TPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypeL2tp, 0);
    if (profL2tp != NULL) 
    {
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypeL2tp, profAction, 0, &profhead);
	profL2tp->listenIp = htonl(lanIp);
	URCP_DEBUG(("%s: profL2tp->listenIp = 0x%x\n", __func__, profL2tp->listenIp));

#if (STATIC_NAT == FYES)
    /* l2tp静态映射变化 */
    if (profL2tp->head.active == 1) 
    {
	profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, L2TP_NAT_NAME);
	if (profNat != NULL) 
	{
#if 0
	    /* lan口 profile change会更新nat,所以此处无需再调用nat profile change了*/
	    profAction = PROF_CHANGE_EDIT;
	    ProfBackupByName(mibTypeNat, profAction, L2TP_NAT_NAME, &profhead);
#endif
	    profNat->IP = htonl(lanIp);
	    URCP_DEBUG(("%s: l2tp - profNat->IP=0x%x\n", __func__, profNat->IP));
	}
    }
#endif
    }
#endif
#if (PPTP_SERVER == FYES)
    /* 监听ip变化 */
    profPptp = (PPTPSrvGlobalProfile *)ProfGetInstPointByIndex(mibTypePptp, 0);
    if (profPptp != NULL) 
    {
	profAction = PROF_CHANGE_EDIT;
	ProfBackupByIndex(mibTypePptp, profAction, 0, &profhead);
	profPptp->listenIp = htonl(lanIp);
	URCP_DEBUG(("%s: profPptp->listenIp = 0x%x\n", __func__, profPptp->listenIp));
#if (STATIC_NAT == FYES)
	/* pptp静态映射变化 */
	if (profPptp->head.active == 1) 
	{
	    profNat = (StaticNatProfile *)ProfGetInstPointByName(mibTypeNat, PPTP_NAT_NAME);
	if (profNat != NULL) 
	{
	    profNat->IP = htonl(lanIp);
	    URCP_DEBUG(("%s: pptp - profNat->IP=0x%x\n", __func__, profNat->IP));
	}   
	}
#endif
    }
#endif
#endif
    if(update==1)
    {
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	//nvramWriteCommit();
    }

out:
    return ret;
}

uint16 set_ip_addr_config(cmd_hdr_t *cmd, uchar** app_data)
{
	DBCK();
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 ip_addr;
    int32 ret = 0;
	FILE *fp = NULL;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    ip_addr = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));/*指向接受包的数据部分*/
    ip_addr = ntohl(ip_addr);

	fp = popen("killall udhcpc 1>/dev/null 2>&1", "r");
	if (fp != NULL) {
		pclose(fp);
	}

	sleep(2);

    ret = set_ip_address_config(ip_addr);
    if(ret == 0)
    {
	scmd -> cmd_status = htons(URCP_OK);
#if (UTT_NV_510W == FYES)
	system("killall goahead 1>/dev/null 2>&1");
	sleep(1u);
	system("goahead &");
#endif
    }
    else
    {
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
    }
	config_backup_state(NEED_BACKUP);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_NETMASK_SET
uint16 set_netmask_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf, name[12];
    uint16 ret = URCP_ANY_ERROR;
    uint32 net_mask;
    cmd_hdr_t* scmd = NULL;
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    memset(name,'\0',sizeof(name));
    URCP_DEBUG(("\n< %s %d >\n",__func__,__LINE__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    net_mask = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));/*指向接受包的数据部分*/
				    
    /* 备份lan口配置 */
    ProfBackupByIndex(mibTypeIF, PROF_CHANGE_EDIT, 1, &profhead);
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 1);
    if ((profIF != NULL) && (ProfInstIsFree((Pointer)profIF)==0) && (profIF->head.active == TRUE))
    {
//	if(profIF->ether.sta.nm != net_mask)
	{
	    profIF->ether.sta.nm = net_mask;
	    strcpy(name, getLanIfName());
	    set_mask(name, net_mask);
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    //nvramWriteCommit();
	}
	ret = URCP_OK;
    }
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(ret);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_GATEWAY_SET
uint16 set_gateway_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf, str_gw[16]={'\0'};
    uint16 ret = URCP_ANY_ERROR;
    uint32 gate_way;
    cmd_hdr_t* scmd = NULL;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    gate_way = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));/*指向接受包的数据部分*/
				    
    /* 备份lan口配置 */
    //ProfBackupByIndex(mibTypeIF, PROF_CHANGE_EDIT, 1, &profhead);
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 1);
    if ((profIF != NULL) && (ProfInstIsFree((Pointer)profIF)==0) && (profIF->head.active == TRUE))
    {
	if(profIF->ether.sta.gw != gate_way)
	{
	    profIF->ether.sta.gw = gate_way;
	    int2ip(ntohl(gate_way), str_gw);
	    set_gw(str_gw);
	    //ProfUpdate(profhead);
	    //ProfFreeAllocList(profhead);
	    //nvramWriteCommit();
	}
	ret = URCP_OK;
    }
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(ret);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_USER_NAME_SET
uint16 set_user_name_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uchar user_name[MAX_USER_NAME_LEN];
    int32 ret = 0;
    uint16 len;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    memset(user_name, 0, sizeof(user_name));
    len = cmd->length - sizeof(cmd_hdr_t);
    if(len>=MAX_USER_NAME_LEN)
    {
	len = MAX_USER_NAME_LEN - 1;
    }
    strncpy(user_name, (tmp + sizeof(cmd_hdr_t)), len);
				    
#if(ADMIN_CONF == FYES)
    AdminConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    prof  = (AdminConfProfile *)ProfGetInstPointByName(mibType,"admin");
    if(prof != NULL)
    {
	prof->head.active = TRUE;
	strcpy(prof->head.name, user_name);
	//nvramWriteCommit();
    }
    else
    {
	ret = -1;
    }
    /* modify /etc/passwd to new user name and passwd */
    system("sed -e 's/^admin:/admin:/' /etc/passwd > /etc/newpw");
    system("cp /etc/newpw /etc/passwd");
    system("rm -f /etc/newpw");
    //doSystem("chpasswd.sh admin %s", passwd);
#endif
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_PASSWORD_SET
uint16 set_password_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uchar passwd[MAX_PASSWORD_LEN];
    int32 ret = 0;
    uint16 len;
    char str[64];

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    memset(passwd, 0, sizeof(passwd));
    /*copy指向接受包的数据部分*/
    len = cmd->length - sizeof(cmd_hdr_t);
    if(len>=MAX_PASSWORD_LEN)
    {
	len=MAX_PASSWORD_LEN - 1;
    }
    strncpy(passwd, (tmp + sizeof(cmd_hdr_t)), len);
#if(ADMIN_CONF == FYES)
    AdminConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_ADMIN_CONF;
    prof  = (AdminConfProfile *)ProfGetInstPointByName(mibType,"admin");
    if(prof != NULL)
    {
	prof->head.active = TRUE;
	strcpy(prof->password, passwd);
	//nvramWriteCommit();
    }
    else
    {
	ret = -1;
    }
    /* modify /etc/passwd to new user name and passwd */
    system("sed -e 's/^admin:/admin:/' /etc/passwd > /etc/newpw");
    system("cp /etc/newpw /etc/passwd");
    system("rm -f /etc/newpw");
    sprintf(str, "chpasswd.sh admin %s", passwd);
    system(str);

    system("killall goahead 1>/dev/null 2>&1;goahead &");

#endif

	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_CHANNEL_SET
uint16 set_ap_channel_config(cmd_hdr_t *cmd, uchar** app_data)
{
	DBCK();
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    int32 ret = 0;
    uint32 ischanged = 0u, ch;
    char str[64];
    uchar channel_no = 0u;

    URCP_DEBUG(("%s\n",__func__));

    MibProfileType mibType = MIB_PROF_WIRELESS;
    struProfAlloc *profhead = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    WirelessProfile *prof= NULL;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    ch = *((uint32*)&tmp[sizeof(cmd_hdr_t)]);
    ch = ntohl(ch);
    channel_no = (uchar)ch;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof->mBaseCf.Channel != channel_no)
    {
	DBCK();
	ProfBackupByIndex(mibType, profAction, 0, &profhead);
	if(channel_no == 0)//自动
	{
	DBCK();
	    prof->mBaseCf.AutoChannelSelect = 2;
		prof->mBaseCf.Channel = channel_no;
		system("iwpriv ra0 set AutoChannelSel=2");
	}
	else
	{
	DBCK();
	    prof->mBaseCf.AutoChannelSelect = 0;
		prof->mBaseCf.Channel = channel_no;
		sprintf(str, "iwpriv ra0 set Channel=%u", channel_no);
		system(str);
	}
    }

#if 0
    if(ischanged == 1u)
    {
	DBCK();
	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	//nvramWriteCommit();
    }
#endif
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_EFFECT_CONF_SET
uint16 set_effect_conf_config(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 effect_conf, isChange=0u;
    uint16 ret = 0;
    char xml_buf[MAX_VALUE_LEN + 1];
    struct msgbuf_t msgbuf;
    request_cmd_msg_t request_cmd_msg;

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    /*copy指向接受包的数据部分*/
    effect_conf = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));
    effect_conf = ntohl(effect_conf);
#if(FEATURE_AP_EFFECT_CONF == FYES)
    ApEffectConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_EFFECT_CONF;

    //struProfAlloc *profhead = NULL;
    //ProfChangeType profAction = PROF_CHANGE_EDIT;
    prof = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibType,0);
    if(prof != NULL)
    {
	//ProfBackupByIndex(mibType, profAction, 0, &profhead);
	if (effect_conf == prof->config)
	{
		isChange = 0u;
	}
	else
	{
		prof->config = effect_conf;
		isChange = 1u;
	}
	//printf("\n< %s %d >, config:%d\n",__func__,__LINE__,effect_conf);
    }
    else
    {
	ret = -1;
    }
    if(isChange == 1u)
    {
	if (PROF_ENABLE == prof->config)    //nonlocal --> local
    {
        memset(request_cmd_msg.content, '\0', sizeof(request_cmd_msg.content));
        memset(xml_buf, '\0', sizeof(xml_buf));
        request_cmd_msg.opt_type = REQUEST_SEND_FILE;
        request_cmd_msg.sn = getSystemSn();
        request_cmd_msg.name_len = 0;
        request_cmd_msg.xml_len = 0;
        memcpy(msgbuf.sw_msg.buffer,(char *)&request_cmd_msg,sizeof(request_cmd_msg));
        urcp_msg_send_prof_to_ac(&msgbuf);
        sleep(2u);
//        if ((apStatusForAcOperate == URCP_STATUS_OK) && (apStatusForAcOperate == URCP_STATUS_FOUND_NONE))
        {
            makeFactoryProfiles();
            ProfReadFromFlash();
            prof->config = effect_conf;
        }
    }
	//ProfUpdate(profhead);
	//ProfFreeAllocList(profhead);
    sleep(2);
	nvramWriteCommit();
	sleep(1u);
	system("reboot");
    }
#endif
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_BASIC_CONFIG_SET
void ap_basic_config(void)
{
    DBCK();
    ap_up_info_to_ac(0u);
}
#endif
#ifdef URCP_CMD_REBOOT_CONFIG_SET
void sys_reboot(void)
{
    system("reboot");/*重启*/
}
#endif
#ifdef URCP_CMD_RESET_CONFIG_SET
void sys_reset(void)
{
    makeFactoryProfiles(); /*恢复出厂配置*/
    ProfWiteToFlash();/*write flash*/
    system("reboot");/*重启*/
}
#endif
#if defined(URCP_CMD_AIR_PROBE_GET) || defined(URCP_CMD_AIR_PROBE_GET_REPLY)
typedef struct _wifi_sigal_t{
   uint16 length;
   uint16 channel;
   uint16 mode;
   uchar mac[MAC_LEN];
   uchar ssid[0];
}wifi_sigal_t;
#define WIFI_SIGAL_LEN 12
#define MAX_WIFI_DEV 12
#endif
#ifdef URCP_CMD_AIR_PROBE_GET
#if 0
/*
 *去掉mac地址中的分隔符
 *KeHuatao
 *2012-07-18
 */
void strip_mac(char *mac, char *str)
{
    char *src, *dst;

    if((mac!=NULL) && (str!=NULL))
    {
	src = str;
	dst = mac;
	while(*src!='\0')
	{
	    if((*src != ':') && (*src!='-'))
	    {
		*dst = *src;
		dst++;
	    }
	    src++;
	}
    }
}
#endif
/*
 *将无线模式字符转换为数字形式
 *
 *KeHuatao
 *2012.07.19
 */
uint16 get_wl_mode(uchar* mode)
{
    uint16 ret=0;

    if(strcmp(mode, "11b/g/n")==0)
    {
	ret=AP_MODE_11BGN;
    }
    else if(strcmp(mode, "11g")==0)
    {
	ret=AP_MODE_11G;
    }
    else if(strcmp(mode, "11n")==0)
    {
	ret=AP_MODE_11N;
    }
    else if(strcmp(mode, "11a")==0)
    {
	ret=AP_MODE_11A;
    }
    else if(strcmp(mode, "11b")==0)
    {
	ret=AP_MODE_11B;
    }
    else if(strcmp(mode, "11b/g")==0)
    {
	ret=AP_MODE_11BG;
    }
    else if(strcmp(mode, "11a/n")==0)
    {
	ret=AP_MODE_11BG;
    }
    else if(strcmp(mode, "11g/n")==0)
    {
	ret=AP_MODE_11BG;
    }
    
    return ret;
}
uint16 get_air_probe(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    char line[128], ssid[48], macaddr[24], quality[6], mode[12];
    int32 i=0, n, ch;
    uint16 len=0u, cmd_len=0u;
    FILE *fp;
    wifi_sigal_t  *tmp;
    cmd_hdr_t* cmd_ptr;

    URCP_DEBUG(("%s\n", __func__));
    cmd_ptr = (cmd_hdr_t*)(*app_data);
    system("iwpriv ra0 set SiteSurvey=1");
    sleep(2);
    fp = popen("iwpriv ra0 get_site_survey", "r");
    if(fp!=NULL)
    {
	/*跳过开头2行*/
        fgets(line, sizeof(line), fp);
        fgets(line, sizeof(line), fp);
        tmp = (wifi_sigal_t*)(*app_data+sizeof(cmd_hdr_t));
        while(i<=MAX_WIFI_DEV)
	{
	    if(fgets(line, sizeof(line), fp)==NULL)
	    {
		break;
	    }
	    memset(ssid, 0, sizeof(ssid));
	    memset(macaddr, 0, sizeof(macaddr));
	    memset(quality, 0, sizeof(quality));
	    memset(mode, 0, sizeof(mode));
	    len = 0u;
	    n=sscanf(line, "%d %s %s %*s %s %s", &ch, ssid, macaddr, quality, mode);
	    if(n==5)
	    {
		len = strlen(ssid);
		if(len>UTT_SSID_LENTH)
		{
		    len = UTT_SSID_LENTH;
		}
                strncpy(tmp->ssid, ssid, len);
		str2mac(macaddr, tmp->mac);
		tmp->mode = get_wl_mode(mode);
	    }
	    else if(n==4)
	    {/*ssid is not allowed*/
		str2mac(ssid, tmp->mac);
		tmp->mode = get_wl_mode(quality);
	    }
	    else
	    {
		break;
	    }
	    tmp->mode = htons(tmp->mode);
	    tmp->channel = htons(ch);
	    tmp->length = WIFI_SIGAL_LEN + len;
	    cmd_len += tmp->length;
	    tmp->length = htons(tmp->length);
	    tmp = (wifi_sigal_t*)((uchar*)tmp + ntohs(tmp->length));
	    i++;
	}
	pclose(fp);
    }
    cmd_ptr -> type = htonl(CMD_AIR_PROBE);
    cmd_ptr -> cmd_status = htons(URCP_OK);
    /*length*/
    cmd_ptr -> length = sizeof(cmd_hdr_t) + cmd_len;
    *app_data += cmd_ptr -> length;
    cmd_ptr->length = htons(cmd_ptr->length);

    return 0u;
}
#endif
#ifdef URCP_CMD_AIR_PROBE_GET_REPLY
/*
 *
 *判断mac是否已经存在
 *判断是否为ap的多ssid接口
 *KeHuatao
 *2012-07-20
 *
 */
int32 isMACExist(uchar *macaddr)
{
    int32 i, found=0, val, tb_index, j;
    urcp_dev_t *dev;
    urcp_shm_t *mem_shadow=NULL;
    urcp_dev_t *tmp;

    dev = urcp_dev_table_lookup(macaddr, 0u);
    if(dev==NULL)
    {
	/*获取内存指针*/
	mem_shadow = get_shm_ptr();
	if(mem_shadow!=NULL)
	{ 
	    /*遍历*/
	    for(i = 0u; i < DEV_HASH_SIZE; i++) 
	    {
                tb_index = mem_shadow->dev_tb_hash[i];
		while((tb_index!=NONE_NODE) && (found==0))
		{
		    tmp = &(mem_shadow->dev_table[tb_index-1]);
		    val = tmp->dev_type;
		    /*只找无线设备*/
		    if((val&DEV_TYPE_FIT_AP) || (val&DEV_TYPE_FAT_AP) || (val&DEV_TYPE_WIRELESS))
		    {
			/*搜索所有ssid对应的MAC*/
			for(j=0;j<tmp->wl_info.ssid_num;j++)
			{
			    if(mac_cmp(tmp->wl_info.ssid_info[j].mac, macaddr)==0)
			    {
				/*exist*/
				URCP_DEBUG(("%s: exist: %s\n", __func__, str_macaddr(macaddr)));
				found = 1;
				break;
			    }
			}
		    }
		    /*the next*/
		    tb_index = tmp->tb_next;
		}
		/*break*/
		if(found==1)
		{
		    break;
		}
	    }
	}
    }

    return found;
}

/*
 *收到报告周围ssid的包
 *KeHuatao
 *2012-07-20
 */
uint16 receive_air_probe(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint32 i;
    uint16 l, len, ret = URCP_OK;
    wifi_sigal_t *tmp;
    urcp_dev_t *wl_dev;

    l = ntohs(cmd->length);
    URCP_DEBUG(("%s: len:%d\n",__func__, l));
    len = sizeof(cmd_hdr_t);
    tmp = (wifi_sigal_t*)((uchar*)cmd + sizeof(cmd_hdr_t));
    if( l > (len + sizeof(wifi_sigal_t)))
    {
	while(len<l)
	{
	    len += ntohs(tmp->length);

	    URCP_DEBUG(("%s mac: %s\n",__func__, str_macaddr(tmp->mac)));
	    if(isMACExist(tmp->mac)==0)
	    {
		wl_dev=dev_get(tmp->mac, urcp_info->vid, 0u, DEV_TYPE_WIRELESS);
		if(wl_dev!=NULL)
		{
		    i = ntohs(tmp->length) - WIFI_SIGAL_LEN;
		    strncpy(wl_dev->wl_info.ssid_info[0].ssid, tmp->ssid, i);
		    wl_dev->wl_info.ssid_num=1;
		    wl_dev->wl_info.channel_head = ntohs(tmp->channel);
		//    wl_dev->wl_info.channel=ntohs(tmp->channel);
		    wl_dev->wl_info.ap_flag = ntohs(tmp->mode);
		    URCP_DEBUG(("%s %d, mode: %08x\n",__func__, wl_dev->wl_info.channel, wl_dev->wl_info.ap_flag));
		}
	    }
	    tmp = (wifi_sigal_t*)((uchar*)tmp + ntohs(tmp->length));
	}
	    URCP_DEBUG(("%s len:%d, l:%d\n",__func__, len, l));
    }
    else
    {
	ret= URCP_ANY_ERROR;
	URCP_DEBUG(("%s length error\n",__func__));
    }

    return ret;
}
#endif
#ifdef URCP_CMD_WIRELESS_MODE_GET
uint16 get_wireless_mode(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 md;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;   
    cmd_hdr_gen_t* cmd;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if((prof!=NULL) && (prof->head.active == 1))
    {
	md = prof->mBaseCf.WirelessMode;
	if(md==ONLY_11G)
	{
	    md = AP_MODE_11G;
	}
	else if(md==ONLY_11N)
	{
	    md = AP_MODE_11N;
	}
	else if(md==AND_11G_11N)
	{
	    md = AP_MODE_11BGN;
	}
	URCP_DEBUG(("************************************%s: mode: %d\n",__func__, md));
	cmd = (cmd_hdr_t*)(*app_data);
	/*构建hello回复命令*/
	cmd -> type = htonl(CMD_WIRELESS_MODE);
	cmd -> cmd_status = htons(URCP_OK);
        cmd -> value = htonl(md); 
	cmd -> length = sizeof(cmd_hdr_gen_t);
	/*数据长度*/
	*app_data += cmd -> length;
	cmd -> length = htons(cmd->length);
    }
}

uint16 trap_get_wireless_mode(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 md;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;   
    cmd_hdr_gen_t* cmd;

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
    if((prof!=NULL) && (prof->head.active == 1))
    {
	md = prof->mBaseCf.WirelessMode;
	if(md==ONLY_11G)
	{
	    md = AP_MODE_11G;
	}
	else if(md==ONLY_11N)
	{
	    md = AP_MODE_11N;
	}
	else if(md==AND_11G_11N)
	{
	    md = AP_MODE_11BGN;
	}
	URCP_DEBUG(("************************************%s: mode: %d\n",__func__, md));
	cmd = (cmd_hdr_t*)(*app_data);
	/*构建hello回复命令*/
	cmd -> type = htonl(TRAP_WIRELESS_MODE);
	cmd -> cmd_status = htons(URCP_OK);
        cmd -> value = htonl(md); 
	cmd -> length = sizeof(cmd_hdr_gen_t);
	/*数据长度*/
	*app_data += cmd -> length;
	cmd -> length = htons(cmd->length);
    }
}
#endif
#ifdef URCP_CMD_WIRELESS_MODE_GET_REPLY
uint16 receive_wireless_mode(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret = URCP_OK;
    uint32 md;
    cmd_hdr_gen_t *tmp=NULL;

    URCP_DEBUG(("xxxxxxxxxxxxxxxxxxxxxxxxxxxx %s\n", __func__));

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
	md = ntohl(tmp->value);
	if( (md==AP_MODE_11BGN) || (md==AP_MODE_11B) || (md==AP_MODE_11G)
	    || (md==AP_MODE_11N) || (md==AP_MODE_11A) || (md==AP_MODE_11BG)
	    || (md==AP_MODE_11AN) || (md==AP_MODE_11GN))
	{
	    (*dev)->wl_info.ap_flag |= md;
	    URCP_DEBUG(("%s: %d\n", __func__, md));
	}
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }


    return ret;
}
#endif
#ifdef URCP_CMD_CONFIG_TEMPLATE_SET_REPLY
/*
 * 联动config template回调函数
 * added by Shiyu.Chen, 2012.07.23
 */
void urcp_ap_config_template_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{
    URCP_DEBUG(("func:%s(), urcp_status=%d\n", __func__, urcp_state));
    /*判断状态*/
    switch(urcp_state)
    {
	case URCP_STATUS_FAIL: /*致命错误*/
	case URCP_STATUS_AUTH_ERROR: /*密码错误*/
	    urcp_ap_conf_temp_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_TIME_OUT:/*超时*/
	    urcp_ap_conf_temp_set_status(reqId, urcp_state);
	    break;
	case URCP_STATUS_OK:
	    if(a_type==SESSION_TYPE_SET)
	    {
		/*set类型命令收到成功回复后，设置msic buffer操作状态*/
		urcp_ap_conf_temp_set_status(reqId, urcp_state);
	    }
	    break;
	default:/*直接跳出*/
	    break;
    }

    return;
}
#endif

#ifdef URCP_CMD_CONFIG_TEMPLATE_SET
void service_zone_for_conf_temp(void)
{
	DBCK();
    int i;
    char str[64];
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    system("sleep 5");
#if 0
    system("fit_ap_wlan.sh");
    system("sleep 3");
#endif
	DBCK("sz_num = %d", sz_num);
    for (i=0; i<4; i++)
    {
	DBCK("i = %d", i);
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (prof->head.active == TRUE))
	{
	sprintf(str, "iwpriv ra%d set VLANID=%u", i, prof->vid);
	system(str);
	}
    }
    //printf("\n< %s %d > end\n",__func__,__LINE__);
}

/*
 * Function: set_ap_config_template()
 *
 * Description: set AP config template
 *
 * Histroy: Shiyu.Chen, 2012.07.23
 */
uint16 set_ap_config_template(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    int32 ret = 0;
    char swType[(UTT_SW_MODEL_LEN+1u)], hwVersion[(UTT_HW_VERSION_LEN+1u)];
    urcp_ap_conf_temp_conf_t *urcp_conf_temp = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    ApEffectConfProfile *prof1 = NULL;
    MibProfileType mibType1 = MIB_PROF_AP_EFFECT_CONF;
#ifdef CONFIG_UTT_TURBO
    int32 turb=0;
#endif

    URCP_DEBUG(("%s\n",__func__));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    /*copy指向接受包的数据部分*/
    urcp_conf_temp = ((urcp_ap_conf_temp_conf_t*)(&tmp[sizeof(cmd_hdr_t)]));

#if 0
    URCP_DEBUG(("tempname= %s, suitType=%s, firmware=%s\n", urcp_conf_temp->tempName, urcp_conf_temp->suitType, urcp_conf_temp->firmware));

    /* Check product model and HW version */
    memset(swType, 0, sizeof(swType));
    get_sysinfo_machine_model(swType);

    memset(hwVersion, 0, sizeof(hwVersion));
    get_sysinfo_hw_model(hwVersion);

    if((strcmp(swType, urcp_conf_temp->suitType) == 0) 
	    && (strcmp(hwVersion, urcp_conf_temp->firmware) == 0) 
      )
#endif
	if (1)
    {
	urcp_conf_temp->wireless = ntohl(urcp_conf_temp->wireless);
	urcp_conf_temp->sgi = ntohl(urcp_conf_temp->sgi);
	urcp_conf_temp->preamble = ntohl(urcp_conf_temp->preamble);
	urcp_conf_temp->wmm = ntohl(urcp_conf_temp->wmm);
	urcp_conf_temp->rate = ntohl(urcp_conf_temp->rate);
        if(urcp_conf_temp->rate > MAX_WIFI_RATE)
        { 
            urcp_conf_temp->rate = MAX_WIFI_RATE;
        }
	urcp_conf_temp->bw = ntohl(urcp_conf_temp->bw);
	urcp_conf_temp->channel = ntohl(urcp_conf_temp->channel);
	urcp_conf_temp->power = ntohl(urcp_conf_temp->power);
	urcp_conf_temp->manPower = ntohl(urcp_conf_temp->manPower);
	urcp_conf_temp->mode = ntohl(urcp_conf_temp->mode);
	urcp_conf_temp->wlFre = ntohl(urcp_conf_temp->wlFre);
	urcp_conf_temp->beaconPeriod = ntohl(urcp_conf_temp->beaconPeriod);

	URCP_DEBUG(("wireless=%d, sgi=%d, preamble=%d, wmm=%d\n",urcp_conf_temp->wireless,urcp_conf_temp->sgi,urcp_conf_temp->preamble,urcp_conf_temp->wmm));
	URCP_DEBUG(("rate=%d, bw=%d, wlFre=%d, channel=%d, power=%d,manPower=%d,mode=%d\n",urcp_conf_temp->rate,urcp_conf_temp->bw,urcp_conf_temp->wlFre,urcp_conf_temp->channel,urcp_conf_temp->power,urcp_conf_temp->manPower,urcp_conf_temp->mode));

	prof1 = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibType1, 0);
	if((prof1 != NULL) && (prof1->config == PROF_DISABLE)) /*no local*/
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);
	    /*save config*/
	    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType,0);
	    if (NULL != prof ) 
	    {
		/* 启用无线功能开关 */
		if(urcp_conf_temp->wireless == 1u)
		{
		    prof->head.active = TRUE;
		}
		else
		{
		    prof->head.active = FALSE;
		}

		/* short guard interval */
		if(urcp_conf_temp->sgi != prof->mBaseCf.HT_GI)
		{
		    prof->mBaseCf.HT_GI = urcp_conf_temp->sgi;
		}
#if 0
		/* 无线客户端隔离 */
		if(urcp_conf_temp->cltIsolate == 1u)
		{
#ifdef CONFIG_WLAN_LAN_ISOLATION
		    set_br_isolation(1);
#endif
//		    prof->mBaseCf.NoForwarding = FUN_ENABLE;
		}
		else
		{
#ifdef CONFIG_WLAN_LAN_ISOLATION
		    set_br_isolation(0);
#endif
//		    prof->mBaseCf.NoForwarding = FUN_DISABLE;
		}
#endif
		/* short preamble */
		if(urcp_conf_temp->preamble == 1u)
		{
		    prof->advance.preamble = FUN_ENABLE;
		}
		else
		{
		    prof->advance.preamble = FUN_DISABLE;
		}
		/*wmm*/
		if(urcp_conf_temp->wmm== 1u)
		{
		    prof->advance.wmm = FUN_ENABLE;
		}
		else
		{
		    prof->advance.wmm = FUN_DISABLE;
		}
		/*wireless rate*/
		if(urcp_conf_temp->rate != prof->fitAp.rate)
		{
		    prof->fitAp.rate = urcp_conf_temp->rate;
		}

		/* 频道带宽 */
		if(prof->mBaseCf.ChanWidth != urcp_conf_temp->bw)
		{
		    prof->mBaseCf.ChanWidth = urcp_conf_temp->bw;
		}
		/*无线频段*/


		/* Channel 信道 */
		if(prof->mBaseCf.Channel != urcp_conf_temp->channel)
		{
			if (urcp_conf_temp->channel == 0) {
				prof->mBaseCf.AutoChannelSelect = 2;
				prof->mBaseCf.Channel = 0;
			} else {
				prof->mBaseCf.AutoChannelSelect = 0;
				prof->mBaseCf.Channel = urcp_conf_temp->channel;
			}
		}

		/*power*/
		if(prof->fitAp.autoPower != urcp_conf_temp->power)
		{
		    prof->fitAp.autoPower = urcp_conf_temp->power;
		}

		/*man power*/
		if(prof->fitAp.manPower != urcp_conf_temp->manPower)
		{
		    prof->fitAp.manPower = urcp_conf_temp->manPower;
		}

#ifdef CONFIG_UTT_TURBO
                turb = prof->turbo;
#endif
		if ((prof->fitAp.autoPower == 0) || (prof->fitAp.manPower == 3)) {
			prof->mBaseCf.TxPower = 100;
#ifdef CONFIG_UTT_TURBO
#ifdef CONFIG_BOARD_MTK7620_E
			system("gpio l 39 2 2 4000 0 1&");
#endif
			turb = 1;
#endif
                }
                else 
		{
#ifdef CONFIG_UTT_TURBO
		    turb = 0;
#ifdef CONFIG_BOARD_MTK7620_E
		    system("gpio l 39 0 1 0 0 0&");
#endif
#endif
		    if ((prof->fitAp.autoPower == 1) && (prof->fitAp.manPower == 2)) {
			    prof->mBaseCf.TxPower = 40;
		    }
		    else if ((prof->fitAp.autoPower == 1) && (prof->fitAp.manPower == 1))
		    {
			    prof->mBaseCf.TxPower = 10;
		    }else{
			;
		    }
		}

#ifdef CONFIG_UTT_TURBO
		if(turb != prof->turbo){
		     prof->turbo = turb;
		     nvramWriteCommit();
		}
#endif
		/*wireless mode*/
		if(urcp_conf_temp->mode == 1u)
		{
		    prof->mBaseCf.WirelessMode = ONLY_11G;
		}
		else if(urcp_conf_temp->mode == 2u)
		{
		    prof->mBaseCf.WirelessMode = ONLY_11N;
		}
		else
		{
		    prof->mBaseCf.WirelessMode = AND_11G_11N;
		}

		prof->advance.beacon = urcp_conf_temp->beaconPeriod;
	    }

	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    //sleep(5u);
	    //service_zone_for_conf_temp();
	    scmd -> cmd_status = htons(URCP_OK);
	}
    }
    else
    {
	scmd -> cmd_status = htons(URCP_ANY_ERROR);
    }
	config_backup_state(NEED_BACKUP);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif

#ifdef URCP_CMD_WL_MAC_FILTER_SET
static uint16 set_mac_filter_action(int32 action)
{
	int index = 0, flag = 0;
    WirelessMacFilterGlobalProfile *profGlobal = NULL;
    MibProfileType mibTypeGloble = MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL;
	ProfChangeType profAction = PROF_CHANGE_EDIT;
	struProfAlloc *profhead = NULL;

	if (action == 0 || action == 1)
	{
	    profGlobal = (WirelessMacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGloble, index);
	    if(profGlobal != NULL)
	    {
			ProfBackupByIndex(mibTypeGloble, profAction, index, &profhead);

			if (profGlobal->filterEn != FUN_ENABLE)
			{
				profGlobal->filterEn = FUN_ENABLE;
				flag = 1;
			}
			if (profGlobal->action != action)
			{
			    profGlobal->action = action;
				flag = 1;
			}
	    }

		if (1 == flag)
		{
			ProfUpdate(profhead);
			ProfFreeAllocList(profhead);
			nvramWriteCommit();
		}
	}

	return 0;
}

static uint16 add_mac_filter(uchar *mac)
{
    uchar strMac[26];
    const char *profName=NULL;
    uint16 ret;
    int32 min, max, index, update = 1;
    MibProfileType mibType = MIB_PROF_WIRELESS_MAC_FILTER;
    WirelessMacFilterProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_ADD;
    struProfAlloc *profhead = NULL;

    if(PROFFAIL != ProfGetInstIndexFree(mibType))
    {
		ProfInstLowHigh(mibType, &max, &min);
		sprintf(strMac, "%02X%02X%2X%2X%2X%2X", mac[0], mac[1], mac[2],mac[3],mac[4],mac[5]);

#if 0
         for (index = min; index < max; index++)
	{
	     profName = ProfGetInstNameByIndex(mibType, index);
	     if(0 == strcmp(profName, strMac))
	     {
                 index = ProfNewInst(mibType, strMac, FALSE);
		 ProfBackupByIndex(mibType, profAction, index, &profhead);
		 prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		 if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
		 {
		     memcpy(&prof->mac, mac, MAC_LEN);
		     update = 1;
		     URCP_DEBUG(("added\n"));
                 }
             }
	}
        if(update==1)
        {
	     ProfUpdate(profhead);
	     ProfFreeAllocList(profhead);
	}
#endif

		for (index = min; index < max; index++)
		{
			profName = ProfGetInstNameByIndex(mibType, index);
			if (0 == strcmp(profName, strMac))
			{
				update = 0;
				break;
			}
		}

		if(update == 1)
		{
			index = ProfNewInst(mibType, strMac, FALSE);
			ProfBackupByIndex(mibType, profAction, index, &profhead);
			prof = (WirelessMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
			if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0))
			{
				memcpy(&prof->mac, mac, MAC_LEN);
				URCP_DEBUG(("added\n"));
			}

			ProfUpdate(profhead);
			ProfFreeAllocList(profhead);
		}
    }
    else
    {
		ret = URCP_ANY_ERROR;
    }

    return 0;
}

uint16 set_wl_mac_filter(cmd_hdr_t *cmd, uchar** app_data)
{
    uchar buf[1];
    uchar *tmp = buf;
    int32 ret = URCP_OK;
    uint16 len;
    uint32 n, i, enable;
    cmd_hdr_t* scmd = NULL;

    URCP_DEBUG(("%s\n",__func__));

    /*复制数据*/
    len = ntohs(cmd->length) - sizeof(cmd_hdr_t);
    n = *((uint32*)((uchar*)cmd + sizeof(cmd_hdr_t)));
    n = ntohl(n);
    enable = *((uint32*)((uchar*)cmd + sizeof(cmd_hdr_t) + sizeof(uint32)));
    enable = ntohl(enable);

    set_mac_filter_action(enable);

    if(len==((n*MAC_LEN)+2*sizeof(uint32)))
    {
	tmp = (uchar*)((uchar*)cmd + sizeof(cmd_hdr_t) + 2 * sizeof(uint32));
	for(i=0; i<n; i++)
	{
	    URCP_DEBUG(("mac: %s\n ", str_macaddr(tmp)));
	    add_mac_filter(tmp);
	    tmp = tmp + MAC_LEN;
	}
	}
    else
    {
	ret = URCP_ANY_ERROR;
	}

	config_backup_state(NEED_BACKUP);
    /*构建回复命令*/
    scmd = (cmd_hdr_t*)(*app_data);
    scmd -> cmd_status = htons(ret);
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif

#ifdef URCP_CMD_AP_SZ_NAME_GET_REPLY
/********************
 *
 *receive the service zone name
 *xhhu
 ********************/
uint16 receive_ap_sz_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uchar *tmp;
    int i=0;
    char *ptr = NULL;
    char *SZNames = NULL;
    uint16 ulen, ret=URCP_OK;
    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_t); 
    /*判断长度是否合法*/
    if((ulen<=(UTT_INST_NAME_LEN+1)*6) && (dev!=NULL) && (*dev!=NULL))
    {
	tmp = (uchar*)cmd;
	tmp = tmp + sizeof(cmd_hdr_t);/*跳过命令头数据*/
	SZNames=strtok_r(tmp,",",&ptr);
	while(SZNames != NULL)
	{
	    strncpy((*dev)->wl_info.ssid_info[i].szone, SZNames, sizeof(SZNames));/*service zone name*/
	    SZNames=strtok_r(NULL,",",&ptr);
	    i++;
	}
#if 0
	    strncpy((*dev)->wl_info.ssid_info[i].szone, tmp, UTT_SZONE_LENTH);/*service zone name*/
#endif
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#ifdef URCP_CMD_AP_SZ_NUM_GET_REPLY
/********************
 *
 *receive the service zone num
 *xhhu
 ********************/
uint16 receive_ap_sz_num(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;

    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.ssid_num)));

    return ret;
}
#endif

#if 0
#ifdef URCP_CMD_SZ_STATUS_SET
uint16 set_sz_status_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((uint32*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uint32))]));
	num = ntohl(num);
    URCP_DEBUG(("\n< %s %d >num:%d\n",__func__,__LINE__,num));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->serviceEn = num;
    }
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#endif

#ifdef URCP_CMD_MAC_FILTER_NUM_SET
uint16 set_mac_filter_num(cmd_hdr_t *cmd, uchar** app_data)
{
	DBCK();
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    int32 ret = 0;
	mac_filter_num = 0u;
    MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL;
    int32 min, max, index;
	int j;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    mac_filter_num = *((uint32*)(tmp + sizeof(cmd_hdr_t)));
	DBCK("mac_filter_num = %d\n", mac_filter_num);
//  mac_filter_num = ntohl(mac_filter_num);
//	DBCK("mac_filter_num = %d\n", mac_filter_num);
/*
	for (j = 0; j < 4; j++) {
		prof = (FitApSzProfile *)ProfGetInstPointByIndex(MIB_PROF_FIT_AP_SZ, j);
		if(prof!=NULL){
			memset(prof->macFilter.name,'\0',sizeof(prof->macFilter.name));
			memset(prof->macFilter.sz,'\0',sizeof(prof->macFilter.sz));
			memset(&prof->macFilter.mac,'\0',sizeof(prof->macFilter.mac));
			prof->macFilter.mode=0;
			prof->macFilter.num=0;

		}
	}
*/
	/*删除原有配置*/
	ProfInstLowHigh(mibType, &max, &min);
	for(index = min; index < URCP_CMD_AP_MF_NUM; index++)
	{
		prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active))
		{
			ProfDelInstByIndex(mibType, index);
		}
	}

	    /*构建hello回复命令*/
	config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);
    DBCK();
    return ret;
}
#endif

#ifdef URCP_CMD_MAC_FILTER_SET
uint16 set_mac_filter(cmd_hdr_t *cmd, uchar** app_data)
{
    DBCK();
	int16 ret = 0;
	int i = 0, j = 0;
    uchar *tmp = NULL;
	cmd_hdr_t *scmd = NULL;
    int32 min, max, index;

    MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
    ApMacFilterProfile *prof = NULL;
	ApMacFilterProfile *mfTmp = NULL;

	scmd = (cmd_hdr_t *)(*app_data);
	tmp = (uchar *)cmd;

	ProfInstLowHigh(mibType, &max, &min);
	
	DBCK("mac_filter_num = %d\n", mac_filter_num);
	for (i = 0; i < mac_filter_num; i++) {
		mfTmp = (ApMacFilterProfile *)(tmp + sizeof(cmd_hdr_t) + i * sizeof(ApMacFilterProfile));
		DBCK("mfTmp->name=%s,mfTmp->num=%d,mfTmp->mode=%d\n",mfTmp->head.name,mfTmp->num,mfTmp->mode);
		/*
		for(j = min; j < max; j++)
		{
			prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, j);
			if (prof != NULL) {
				DBCK("prof->name=%s\n",prof->head.name);
				if(strcmp(prof->head.name,mfTmp->head.name) == 0)
				{
					memcpy(prof, mfTmp, sizeof(ApMacFilterProfile));
					break;
				}
			}
		}
		*/
//		if(j == max)
		{
			index = ProfNewInst(mibType, mfTmp->head.name, FALSE);
			prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
			memcpy(prof, mfTmp, sizeof(ApMacFilterProfile));
		}

		DBCK("prof->mode = %d,num=%d\n", prof->mode,prof->num);
	}

	config_backup_state(NEED_BACKUP);
	scmd->cmd_status = htons(URCP_OK);
	scmd->type = cmd->type;
	scmd->length = sizeof(cmd_hdr_t);
	*app_data += scmd->length;
	scmd->length = htons(scmd->length);

	return ret;
}
#endif

#ifdef URCP_CMD_SZ_NAME_SET
uint16 set_sz_name_config(cmd_hdr_t *cmd, uchar** app_data)
{
		int i;
		uchar buf[1];
		uchar *tmp = buf;
		char *tmp2 = NULL;
		char *ptr = NULL;
		char tmpStr[24];
		cmd_hdr_t* scmd = NULL;
		uchar str[(UTT_INST_NAME_LEN+1)*8];
		uint16 ret = 0u;
		uint16 len;
		WirelessProfile *prof = NULL;
		MibProfileType mibType = MIB_PROF_WIRELESS;
		FitApSzProfile *profS = NULL;
		MibProfileType mibTypeS = MIB_PROF_FIT_AP_SZ;

		memset(str, 0, sizeof(str));
		memset(tmpStr, '\0', sizeof(tmpStr));
		sz_num = 0;
		scmd = (cmd_hdr_t*)(*app_data);
		tmp = (uchar*)cmd;
		len = ntohs(cmd->length) - sizeof(cmd_hdr_t);
		if(len>=UTT_INST_NAME_LEN)
		{
				strncpy(str, (tmp + sizeof(cmd_hdr_t)), len);
		}
		tmp2=strtok_r(str, ",", &ptr);
		while(tmp2 != NULL)
		{
				prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, (sz_num+1));
				profS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeS, sz_num);
				sprintf(tmpStr, "wireless%d",(sz_num+1));
				ProfSetNameByPoint(prof, tmpStr);
				ProfSetNameByPoint(profS, tmp2);
				URCP_DEBUG(("\n< %s %d > Wireless head.name:%s FitAp head.name:%s\n",__func__, __LINE__, prof->head.name, profS->head.name));
				prof->head.active = TRUE;
				profS->head.active = TRUE;
				profS->client = 0u;
				tmp2=strtok_r(NULL, ",", &ptr);
				sz_num++;
		}
		for (i=sz_num; i<4; i++)
		{
				prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, (i+1));
				prof->head.active = FALSE;
				memset(prof->head.name, '\0', sizeof(prof->head.name));
				profS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeS, i);
				memset(profS->head.name, '\0', sizeof(profS->head.name));
				profS->head.active = FALSE;
				profS->client = 0u;
		}
		config_backup_state(NEED_BACKUP);
		scmd->cmd_status = htons(URCP_OK);
		scmd->type = cmd->type;
		scmd->length = sizeof(cmd_hdr_t);
		*app_data += scmd->length;
		scmd->length = htons(scmd->length);

		return ret;
}
#endif
#ifdef URCP_CMD_SZ_SSID_SET
uint16 set_sz_ssid_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int num=0;
    uchar buf[1];
    uchar *tmp = buf;
    char *tmp2 = NULL;
    char *ptr = NULL;
    cmd_hdr_t* scmd = NULL;
    uchar str[(UTT_SSID_LENTH+1)*8];
    uint16 ret = 0u;
    uint16 len;
    WirelessProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;

    memset(str, 0, sizeof(str));
    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    len = ntohs(cmd->length) - sizeof(cmd_hdr_t);
    if(len>=UTT_SSID_LENTH)
    {
	strncpy(str, (tmp + sizeof(cmd_hdr_t)), len);
    }
    tmp2=strtok_r(str, ",", &ptr);
    while(tmp2 != NULL)
    {
	prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, (num+1));
	strncpy(prof->mBaseCf.SSID1, tmp2, UTT_SSID_LENTH+1);
    URCP_DEBUG(("\n< %s %d > ssid:%s\n",__func__, __LINE__, prof->mBaseCf.SSID1));
	tmp2=strtok_r(NULL, ",", &ptr);
	num++;
    }
	config_backup_state(NEED_BACKUP);
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_SSID_ISOLATE_SET
uint16 set_sz_ssid_isolate_config (cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((uint32*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uint32))]));
	num = ntohl(num);
    URCP_DEBUG(("\n< %s %d >num:%d\n",__func__,__LINE__,num));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->ssidIsolate = num;
    }
	config_backup_state(NEED_BACKUP);
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_SSID_BC_SET
uint16 set_sz_ssid_bc_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 num;
    uint16 ret = 0u;
    WirelessProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((uint32*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uint32))]));
	num = ntohl(num);
	prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, (i+1));
	prof->mBaseCf.SSIDBroadCastEn = num;
    URCP_DEBUG(("\n< %s %d > i+1:%d ssidBC:%d\n",__func__,__LINE__,(i+1),num));
    }
	config_backup_state(NEED_BACKUP);
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_VID_SET
uint16 set_sz_vid_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((uint32*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uint32))]));
	num = ntohl(num);
    URCP_DEBUG(("\n< %s %d >num:%d\n",__func__,__LINE__,num));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->vid = num;
    }
	config_backup_state(NEED_BACKUP);
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_ROAMING_TH_SET
uint16 set_ap_roaming_th(cmd_hdr_t *cmd, uchar** app_data)
{
	DBCK();
    uchar buf[1];
	char str[200];
	uint16 ret = 0;
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
	uint32 dbm;
    uint16 len;
    uint32 isChange = 0u;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar *)cmd;
    /*复制数据*/
    dbm = *((uint32*)(&tmp[sizeof(cmd_hdr_t)]));/*指向接受包的数据部分*/
	dbm = ntohl(dbm);
	DBCK("dbm = %d\n", dbm);
    ApBasicConfProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_BASIC_CONF;
    prof = (ApBasicConfProfile *)ProfGetInstPointByIndex(mibType,0);
    if(prof != NULL)
    {
	if (dbm != prof->dbm)
	{
		prof->dbm = dbm;
	    isChange = 1u;
	}
	else
	{
            isChange = 0u;
	}
    }
    else
    {
	ret = -1;
    }
    if(isChange == 1u)
    {
		sprintf(str, "iwpriv ra0 set KickStaRssiLow=-%u; iwpriv ra1 set KickStaRssiLow=-%u; iwpriv ra2 set KickStaRssiLow=-%u; iwpriv ra3 set KickStaRssiLow=-%u;", prof->dbm, prof->dbm, prof->dbm, prof->dbm);
		DBCK("%s", str);
		printf("%d in %s of %s, %s\n", __LINE__, __func__, __FILE__, str);
		system(str);
		sprintf(str, "iwpriv ra0 set AssocReqRssiThres=-%u; iwpriv ra1 set AssocReqRssiThres=-%u; iwpriv ra2 set AssocReqRssiThres=-%u; iwpriv ra3 set AssocReqRssiThres=-%u;", prof->dbm, prof->dbm, prof->dbm, prof->dbm);
		DBCK("%s", str);
		printf("%d in %s of %s, %s\n", __LINE__, __func__, __FILE__, str);
		system(str);
    }

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

	DBCK();
    return ret;
}
#endif
#ifdef URCP_CMD_AP_MAX_CLIENT_SET
uint16 set_max_client_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    char str[64];
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((uint32*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uint32))]));
	num = ntohl(num);
    URCP_DEBUG(("\n< %s %d >num:%d\n",__func__,__LINE__,num));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->client = num;
    }

    config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}

uint16 set_max_client_config_new(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    char str[64];
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint16 ret = 0u;
    uint16 num_sz = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd + sizeof(cmd_hdr_t);
    num_sz = ntohs(*((uint16 *)tmp));
//    printf("%s, %d\n",__func__, num_sz);
    tmp += sizeof(uint16);
    if (num_sz <= MAX_FIT_AP_SERVICE_ZONE_PROFILES )
    {
	for (i=0; (i < MAX_FIT_AP_SERVICE_ZONE_PROFILES) && (num_sz > 0u); i++)
	{
	    prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	    if ((prof != NULL) && (strncmp(tmp, prof->head.name, UTT_INST_NAME_LEN) == 0))
	    {
		prof->client = (uint32)(tmp[UTT_INST_NAME_LEN]);				//取最大值
//		printf("%s, %d\n", __func__, prof->client);
		tmp += UTT_INST_NAME_LEN + 1;
		num_sz--; 
		sprintf(str, "iwpriv ra%d set MaxStaNum=%u", i, prof->client );
		system(str);
	    }
	}
    }
    config_backup_state(NEED_BACKUP);
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}

#endif
#ifdef URCP_CMD_AP_LB_MAX_CLIENT_SET
uint16 set_lb_max_client_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    char str[64];
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uint32 num;
    uint16 ret = 0u;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
	num = *((uint32*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uint32))]));
	num = ntohl(num);
    URCP_DEBUG(("\n< %s %d >num:%d\n",__func__,__LINE__,num));
	sprintf(str, "iwpriv ra0 set uttMaxStaNum=%u", num);
	printf("%s\n", str);
	system(str);

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_AUTH_SET
uint16 set_sz_auth_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    uth_mode num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((uth_mode*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(uth_mode))]));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->securMode = ntohl(num);
    URCP_DEBUG(("\n< %s %d > num:%d\n",__func__,__LINE__,ntohl(num)));
    }
	config_backup_state(NEED_BACKUP);
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_WEP_SET
uint16 set_sz_wep_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    auth_wep num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((auth_wep*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(auth_wep))]));
    URCP_DEBUG(("\n< %s %d >\n",__func__,__LINE__));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->wepSecurity.auMethod = ntohl(num.auMethod);
	prof->wepSecurity.kType = ntohl(num.kType);
	prof->wepSecurity.kNum1.key_state_action = ntohl(num.kNum1.key_state_action);
	strncpy(prof->wepSecurity.kNum1.passwd, num.kNum1.passwd, UTT_PAS_KEY_VALUE_LEN+1);
	prof->wepSecurity.kNum1.kFmt = ntohl(num.kNum1.kFmt);
	prof->wepSecurity.kNum2.key_state_action = ntohl(num.kNum2.key_state_action);
	strncpy(prof->wepSecurity.kNum2.passwd, num.kNum2.passwd, UTT_PAS_KEY_VALUE_LEN+1);
	prof->wepSecurity.kNum2.kFmt = ntohl(num.kNum2.kFmt);
	prof->wepSecurity.kNum3.key_state_action = ntohl(num.kNum3.key_state_action);
	strncpy(prof->wepSecurity.kNum3.passwd, num.kNum3.passwd, UTT_PAS_KEY_VALUE_LEN+1);
	prof->wepSecurity.kNum3.kFmt = ntohl(num.kNum3.kFmt);
	prof->wepSecurity.kNum4.key_state_action = ntohl(num.kNum4.key_state_action);
	strncpy(prof->wepSecurity.kNum4.passwd, num.kNum4.passwd, UTT_PAS_KEY_VALUE_LEN+1);
	prof->wepSecurity.kNum4.kFmt = ntohl(num.kNum4.kFmt);
    }
	config_backup_state(NEED_BACKUP);
    
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_WPA_SET
uint16 set_sz_wpa_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    auth_wpa num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((auth_wpa*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(auth_wpa))]));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->wpaSecurity.wVersion = ntohl(num.wVersion);
	prof->wpaSecurity.sAlg = ntohl(num.sAlg);
	prof->wpaSecurity.radiusIp = num.radiusIp;
	prof->wpaSecurity.radiusPort = ntohl(num.radiusPort);
	strncpy(prof->wpaSecurity.radiusPasswd, num.radiusPasswd, UTT_RADIUS_KEY_LEN);
	prof->wpaSecurity.updataTime = ntohl(num.updataTime);
    }
	config_backup_state(NEED_BACKUP);
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_SZ_PSK_SET

#if (WIRELESS == FYES)
/* 
 *  * 目前只用支持RT2860 
 *   * 只支持一个ssid, 多ssid需要重写
 *    */
void FitApRestart8021XDaemon(void)
{
#if 0
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    FitApSzProfile *prof1 = NULL;
    FitApSzProfile *prof2 = NULL;
    FitApSzProfile *prof3 = NULL;
    FitApSzProfile *prof4 = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;


    prof1 = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, 0); 
    prof2 = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, 1); 
    prof3 = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, 2); 
    prof4 = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, 3); 
    if((prof1 == NULL) || (prof2 == NULL) || (prof3 == NULL) || (prof4 == NULL)){
	return;
    }   
    system("killall rt2860apd0 1>/dev/null 2>&1");
    system("killall rt2860apd1 1>/dev/null 2>&1");
    system("killall rt2860apd2 1>/dev/null 2>&1");
    system("killall rt2860apd3 1>/dev/null 2>&1");

    if ((strcmp(prof->mSafeCf.SelAuthMode, "WPA") == 0)  
	|| (strcmp(prof->mSafeCf.SelAuthMode, "WPA2") == 0)  
	|| (strcmp(prof->mSafeCf.SelAuthMode, "WPA1WPA2") == 0)
	|| (prof->mSafeCf.ap.AuthMode.IEEE8021X == 1)) {

	system("rt2860apd");
    }
#endif
    system("killall -9 rt2860apd 1>/dev/null 2>&1");
    system("sleep 1");
    system("rt2860apd");
}
#endif
int getIfIp(char *ifname, char *if_addr);
void  updateFlash8021xForFitAp(WirelessProfile *prof) {
    int OperationMode;
    char lan_if_addr[16];
    struct in_addr ip; 
    OperationMode = prof->mBaseCf.OperationMode;
#if 0
    if(OperationMode == 0)
    {   
printf("\n< %s %d>\n",__func__,__LINE__);
	return;
    }
#endif
    if(prof->mSafeCf.ap.AuthMode.RADIUS_Server == 0)
    {   
	return;
    }   
    if (getIfIp((char *)getLanIfName(), lan_if_addr) == -1) 
    {   
	return;
    }   
    if (0 == inet_aton(lan_if_addr, &ip)) 
    {
	return;
    }    
    prof->mBaseCf.own_ip_addr  = ip.s_addr;
    strcpy(prof->mBaseCf.EAPifname, (char *)getLanIfName());
    strcpy(prof->mBaseCf.PreAuthifname, (char *)getLanIfName());
}

void changeToWirelessProfile(void)
{
    int i;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;
    WirelessProfile *profW=NULL, *tmp = NULL;
    MibProfileType mibTypeW = MIB_PROF_WIRELESS;
    
    for (i=0; i<4; i++)
    {
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (prof->head.active == TRUE))
	{
	    profW = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeW, (i+1));
	    memset(&(profW->mSafeCf),0,sizeof(profW->mSafeCf));
	    if (profW != NULL)
	    {
		if (prof->securMode == 0)   /* 无认证 */
		{
		    strcpy(profW->mSafeCf.ap.AuthMode.EncrypType, "NONE");
		    strcpy(profW->mSafeCf.SelAuthMode, "OPEN");
		}
		else if (prof->securMode == 1)	/* WEP */
		{
		    strcpy(profW->mSafeCf.ap.AuthMode.EncrypType, "WEP");
		    if (prof->wepSecurity.auMethod == 0)
		    {
			strcpy(profW->mSafeCf.SelAuthMode,"WEPAUTO");
		    }
		    else if (prof->wepSecurity.auMethod == 1)
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "OPEN");
		    }
		    else
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "SHARED");
		    }
		    if (prof->wepSecurity.kNum1.key_state_action == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepKeyID = 1;
		    }
		    else if (prof->wepSecurity.kNum2.key_state_action == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepKeyID = 2;
		    }
		    else if (prof->wepSecurity.kNum3.key_state_action == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepKeyID = 3;
		    }
		    else
		    {
			profW->mSafeCf.ap.AuthMode.wepKeyID = 4;
		    }
		    if (prof->wepSecurity.kNum1.kFmt == 0)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength1 = 0;
		    }
		    else if (prof->wepSecurity.kNum1.kFmt == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength1 = 5;
		    }
		    else 
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength1 = 13;
		    }
		    if (prof->wepSecurity.kNum2.kFmt == 0)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength1 = 0;
		    }
		    else if (prof->wepSecurity.kNum2.kFmt == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength2 = 5;
		    }
		    else 
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength3 = 13;
		    }
		    if (prof->wepSecurity.kNum3.kFmt == 0)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength3 = 0;
		    }
		    else if (prof->wepSecurity.kNum3.kFmt == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength3 = 5;
		    }
		    else 
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength3 = 13;
		    }
		    if (prof->wepSecurity.kNum4.kFmt == 0)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength4 = 0;
		    }
		    else if (prof->wepSecurity.kNum4.kFmt == 1)
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength4 = 5;
		    }
		    else 
		    {
			profW->mSafeCf.ap.AuthMode.wepkeyLength4 = 13;
		    }
		    strcpy(profW->mSafeCf.ap.AuthMode.wepkey1, prof->wepSecurity.kNum1.passwd);
		    strcpy(profW->mSafeCf.ap.AuthMode.wepkey2, prof->wepSecurity.kNum2.passwd);
		    strcpy(profW->mSafeCf.ap.AuthMode.wepkey4, prof->wepSecurity.kNum4.passwd);
		    profW->mSafeCf.ap.AuthMode.wepKeyType = prof->wepSecurity.kType;
		    profW->mSafeCf.ap.AuthMode.IEEE8021X = 0;
		}
		else if (prof->securMode == 2)	/* WPA */
		{
		    if (prof->wpaSecurity.wVersion == 0)
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "WPA1WPA2");
		    }
		    else if (prof->wpaSecurity.wVersion == 1)
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "WPA");
		    }
		    else
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "WPA2");
		    }
		    profW->mSafeCf.ap.AuthMode.RADIUS_Server = prof->wpaSecurity.radiusIp;
		    if(prof->wpaSecurity.radiusIp !=0 ) {
			tmp = profW;
		    }
		    profW->mSafeCf.ap.AuthMode.RADIUS_Port = prof->wpaSecurity.radiusPort;
		    strcpy(profW->mSafeCf.ap.AuthMode.RADIUS_Key,prof->wpaSecurity.radiusPasswd);
//		    updateFlash8021xForFitAp(profW);
		    if (prof->wpaSecurity.sAlg == 0)
		    {
			strcpy(profW->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
		    }
		    else if (prof->wpaSecurity.sAlg == 1)
		    {
			strcpy(profW->mSafeCf.ap.AuthMode.EncrypType,"TKIP");
		    }
		    else 
		    {
			strcpy(profW->mSafeCf.ap.AuthMode.EncrypType,"AES");
		    }
		    profW->mSafeCf.ap.AuthMode.RekeyInterval = prof->wpaSecurity.updataTime;
		    profW->mSafeCf.ap.AuthMode.IEEE8021X = 0;
		}
		else /* WPA-PSK */
		{
		    if (prof->wpaPskSecurity.wVersion == 0)
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "WPAPSKWPA2PSK");
		    }
		    else if (prof->wpaPskSecurity.wVersion == 1)
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "WPAPSK");
		    }
		    else 
		    {
			strcpy(profW->mSafeCf.SelAuthMode, "WPA2PSK");
		    }
		    if (prof->wpaPskSecurity.sAlg == 0)
		    {
			strcpy(profW->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
		    }
		    else if (prof->wpaPskSecurity.sAlg == 1)
		    {
			strcpy(profW->mSafeCf.ap.AuthMode.EncrypType,"TKIP");
		    }
		    else
		    {
			strcpy(profW->mSafeCf.ap.AuthMode.EncrypType,"AES");
		    }
		    profW->mSafeCf.ap.AuthMode.RekeyInterval = prof->wpaPskSecurity.updataTime;
		    strcpy(profW->mSafeCf.ap.AuthMode.pskPsswd, prof->wpaPskSecurity.sharePasswd);
		    profW->mSafeCf.ap.AuthMode.IEEE8021X = 0;
		}
	    }
	}
    }
    if(tmp != NULL)
    {
	/*只需要一份公共配置*/
	profW = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeW, 0);
	if ((tmp != NULL) && (profW != NULL) && (profW->head.active == TRUE))
	{
	    profW->mSafeCf.ap.AuthMode.RADIUS_Server = tmp->mSafeCf.ap.AuthMode.RADIUS_Server;
	    updateFlash8021xForFitAp(profW);
	}
    }
}

void serivce_zone(void)
{
    URCP_DEBUG(("\n< %s %d > begin\n",__func__,__LINE__));
    int i;
    char str[200];
    uchar str_gw[16]={'\0'};
    FitApSzProfile *prof = NULL;
	ApBasicConfProfile *profBase = NULL;
    InterfaceProfile *profIF = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;
	extern ap_info_t ap_infor;

    szone_updata_ack();
    ap_up_info_to_ac(0u);

    printf("%s, sz=%d\n", __func__, sz_num);
    if(sz_num == 0){
        system("wlan.sh");
        system("sleep 3");
    }else{

    system("fit_ap_wlan.sh");
    system("sleep 3");
    FitApRestart8021XDaemon();
	DBCK("sznum=%d in function %s\n",sz_num,__func__);

    for (i=0; i<4; i++)/*这里用4代替sz_num，使MAC地址过滤能够生效，参数要重新设置*/
    {
        prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	    if (prof->head.active == TRUE)
	    {
			sprintf(str, "iwpriv ra%d set VLANID=%u;iwpriv ra%d set NoForwarding=%u;", i, prof->vid, i, prof->ssidIsolate);
           // printf("%s\n", str);
            system(str);
            if(prof->ssidIsolate){
                sprintf(str, "iwpriv ra%d set NoForwardingBTNBSSID=%u;", i, prof->ssidIsolate);
               // printf("%s\n", str);
                system(str);
            }
	    }
    }

	profBase = (ApBasicConfProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_BASIC_CONF, 0);
	if ((profBase != NULL) && (profBase->dbm != 0)) {
		sprintf(str, "iwpriv ra0 set KickStaRssiLow=-%u;iwpriv ra1 set KickStaRssiLow=-%u;iwpriv ra2 set KickStaRssiLow=-%u;iwpriv ra3 set KickStaRssiLow=-%u", profBase->dbm, profBase->dbm, profBase->dbm, profBase->dbm);
		system(str);
//		printf("%d in %s of %s, %s\n", __LINE__, __func__, __FILE__, str);
		sprintf(str, "iwpriv ra0 set AssocReqRssiThres=-%u;iwpriv ra1 set AssocReqRssiThres=-%u;iwpriv ra2 set AssocReqRssiThres=-%u;iwpriv ra3 set AssocReqRssiThres=-%u", profBase->dbm, profBase->dbm, profBase->dbm, profBase->dbm);
		system(str);
//		printf("%d in %s of %s, %s\n", __LINE__, __func__, __FILE__, str);
	}

    }
#if 0
    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
    if ((profIF != NULL) && (ProfInstIsFree((Pointer)profIF)==0) && (profIF->head.active == TRUE))
    {
		if (profIF->ether.connMode == ETH_CONN_STATIC) {
			int2ip(ntohl(profIF->ether.sta.gw), str_gw);
		} else {
			int2ip(ntohl(ap_infor.ac_infor.ip), str_gw);
		}
	    set_gw(str_gw);
    }
#endif
	
    URCP_DEBUG(("\n< %s %d > end\n",__func__,__LINE__));
}
uint16 set_sz_psk_config(cmd_hdr_t *cmd, uchar** app_data)
{
    int i;
    uchar buf[1];
    uchar *tmp = buf;
    cmd_hdr_t* scmd = NULL;
    auth_wpa_psk num;
    uint16 ret = 0u;
    FitApSzProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    for (i=0; i<sz_num; i++)
    {
	num = *((auth_wpa_psk*)(&tmp[sizeof(cmd_hdr_t) + (i*sizeof(auth_wpa_psk))]));
    URCP_DEBUG(("\n< %s %d> \n",__func__,__LINE__));
	prof = (FitApSzProfile *)ProfGetInstPointByIndex(mibType, i);
	prof->wpaPskSecurity.wVersion = ntohl(num.wVersion);
	prof->wpaPskSecurity.sAlg = ntohl(num.sAlg);
	strncpy(prof->wpaPskSecurity.sharePasswd, num.sharePasswd, UTT_RADIUS_KEY_LEN);
	prof->wpaPskSecurity.updataTime = ntohl(num.updataTime);
    }
    scmd->cmd_status = htons(URCP_OK);
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);
    changeToWirelessProfile();
	config_backup_state(NEED_BACKUP);
    //sleep(1);
    //serivce_zone();

    return ret;
}
#endif

#ifdef URCP_CMD_AP_SZ_NAME_GET
uint16 get_ap_sz_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    int i;
    char *tmp=NULL;
    char *index=NULL;
    cmd_hdr_t* cmd;
    FitApSzProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_FIT_AP_SZ;

    cmd = (cmd_hdr_t*)(*app_data);
    cmd -> type = htonl(CMD_AP_SZ_NAME);
    tmp = (char *)(*app_data + sizeof(cmd_hdr_t));
    URCP_DEBUG(("< %s %d > sz_num:%d\n", __func__, __LINE__, sz_num));
    memset(tmp, '\0', 64);
    for (i=0; i<sz_num; i++)
    {
	prof = (FitApSzProfile*)ProfGetInstPointByIndex(mibType,i);
	if((prof != NULL) && (prof->head.active == TRUE))
	{
	    index = strchr(tmp, '\0');
	    if (i == 0)
	    {
		strncpy(&index[0], prof->head.name, (UTT_INST_NAME_LEN+1));
	    }
	    else
	    {
		index[0]=',';
		strncpy(&index[1], prof->head.name, (UTT_INST_NAME_LEN+1));
	    }
	}
    }
    URCP_DEBUG(("< %s %d > tmp:%s\n", __func__, __LINE__, tmp));
    cmd->length = sizeof(cmd_hdr_t) + 64;
    cmd->cmd_status = htons(URCP_OK);
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_AP_SZ_NUM_GET
uint16 get_ap_sz_num(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    cmd -> type = htonl(CMD_AP_SZ_NUM);
    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));
    URCP_DEBUG(("< %s %d > MAX_FIT_AP_SERVICE_ZONE_PROFILES:%d\n", __func__, __LINE__, MAX_FIT_AP_SERVICE_ZONE_PROFILES));
    *tmp = htonl(MAX_FIT_AP_SERVICE_ZONE_PROFILES);
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    cmd->cmd_status = htons(URCP_OK);
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}

uint16 trap_get_ap_sz_num(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    cmd -> type = htonl(TRAP_AP_SZ_NUM);
    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));
    URCP_DEBUG(("< %s %d > MAX_FIT_AP_SERVICE_ZONE_PROFILES:%d\n", __func__, __LINE__, MAX_FIT_AP_SERVICE_ZONE_PROFILES));
    *tmp = htonl(MAX_FIT_AP_SERVICE_ZONE_PROFILES);
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    cmd->cmd_status = htons(URCP_OK);
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_SZONE_UPDATA_GET
uint16 trap_get_szone_updata_ack(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 *tmp;
    cmd_hdr_t* cmd;

    cmd = (cmd_hdr_t*)(*app_data);
    cmd -> type = htonl(TRAP_SZONE_UPDATA);
    tmp = (uint32*)(*app_data + sizeof(cmd_hdr_t));

    *tmp = htonl(1u);
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);
    cmd->cmd_status = htons(URCP_OK);
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}
#endif
#ifdef URCP_CMD_CHANGE_CONF_GET
uint16 ac_change_conf(cmd_hdr_t *cmd, uchar **app_data)
{
    cmd_hdr_t* scmd = NULL;
	uint32 sn;
	uint16 ret, name_len, xml_len;
	uchar buf[1], *tmp = buf;
	uchar file_name[128], name[32], xml_buf[512];

	memset(file_name, '\0', sizeof(file_name));
	memset(name, '\0', sizeof(name));
	memset(xml_buf, '\0', sizeof(xml_buf));

	tmp = (uchar *)cmd;
    scmd = (cmd_hdr_t*)(*app_data);
	memcpy(&sn, (tmp + sizeof(cmd_hdr_t)), sizeof(sn));
	sn = ntohl(sn);
	memcpy(&name_len, (tmp + sizeof(cmd_hdr_t) + sizeof(sn)), sizeof(name_len));
	name_len = ntohs(name_len);
	memcpy(&xml_len, (tmp + sizeof(cmd_hdr_t) + sizeof(sn) + sizeof(name_len)), sizeof(xml_len));
	xml_len = ntohs(xml_len);
	memcpy(name, (tmp + sizeof(cmd_hdr_t) + sizeof(sn) + 2 * sizeof(name_len)), name_len);
	memcpy(xml_buf, (tmp + sizeof(cmd_hdr_t) + sizeof(sn) + 2 * sizeof(name_len) + name_len), xml_len);

	DBCK("name_len = %d, xml_len = %d, name = %s, xml_buf = %s", name_len, xml_len, name, xml_buf);
	sprintf(file_name, "%s%d.xml", "/tmp/urcp/ap_configs/", sn);
	DBCK("file_name = %s", file_name);

	ret = setProfileInstToXml(file_name, name, xml_buf);
    scmd -> cmd_status = htons(URCP_OK);
     /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    URCP_DEBUG(("%s\n",__func__));
    return URCP_OK;
}
#endif
#ifdef URCP_AP
void msg_send_to_goahead(uint32 type, uint32 status)
{
	sendwebmsg(type, status);

	return;
}
#endif

#ifdef URCP_CMD_AC_CHANGE_CONF_SET
void urcp_ap_request_cb(uint16 reqId, session_type_t a_type, urcp_status_t urcp_state)
{

	switch (urcp_state) {
		case URCP_STATUS_OK:
			URCP_DEBUG(("debug: %d in %s\n", __LINE__, __func__));
			msg_send_to_goahead(REQUEST_CHANGE_CONF, URCP_STATUS_OK);
			break;

		default:
			URCP_DEBUG(("debug: %d in %s\n", __LINE__, __func__));
			msg_send_to_goahead(REQUEST_CHANGE_CONF, URCP_STATUS_FAIL);
			break;
	}

	return;
}
#endif
#ifdef URCP_CMD_AC_MODE_SET
uint16 set_ac_work_mode (cmd_hdr_t *cmd, uchar **app_data)
{
    cmd_hdr_t* scmd = NULL;
    uchar mode,*tmp;
    int32 ret = 0;
    WirelessProfile *prof = NULL;
    prof = ProfGetInstPointByIndex(MIB_PROF_WIRELESS ,0);

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uchar*)cmd;
    /*复制数据*/
    /*copy指向接受包的数据部分*/
    mode = *((uchar*)(&tmp[sizeof(cmd_hdr_t)]));
    if (mode != prof->mBaseCf.BYPASS ){
        prof->mBaseCf.BYPASS = mode;
        if (mode==1){
            system("iwpriv ra0 set BYPASS=1");
        }else if( mode == 0){
            system("iwpriv ra0 set BYPASS=0");
        }
    }
    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd->type = cmd->type;
    scmd->length = sizeof(cmd_hdr_t);
    *app_data += scmd->length;
    scmd->length = htons(scmd->length);

}
#endif
#if AP_WL_POWER_AUTO
#ifdef URCP_CMD_AP_RA0_MAC_GET
uint16 get_ap_ra0_mac(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar *pMac = NULL;
    cmd_hdr_t* cmd;
    uchar if_hw[MAC_LEN];
    DBCK();

    get_mac("ra0",if_hw);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_RA0_MAC);


	cmd->cmd_status = htons(URCP_OK);

    pMac = (uchar*)(*app_data + sizeof(cmd_hdr_t));

    /*数据长度*/
    memcpy(pMac,if_hw,MAC_LEN);
    cmd->length = sizeof(cmd_hdr_t) + MAC_LEN;

    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;

}
uint16 trap_get_ap_ra0_mac(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uchar *pMac = NULL;
    cmd_hdr_t* cmd;
    uchar if_hw[MAC_LEN];
    DBCK();

    get_mac("ra0",if_hw);

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(TRAP_AP_RA0_MAC);


	cmd->cmd_status = htons(URCP_OK);

    pMac = (uchar*)(*app_data + sizeof(cmd_hdr_t));

    /*数据长度*/
    memcpy(pMac,if_hw,MAC_LEN);
    cmd->length = sizeof(cmd_hdr_t) + MAC_LEN;

    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;

}
#endif
#ifdef URCP_CMD_AP_RA0_MAC_GET_REPLY
uint16 receive_ap_ra0_mac(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uchar * pMac = NULL;
    uchar if_hw[MAC_LEN];
    DBCK();

    pMac= (uchar*)((uchar*)cmd + sizeof(cmd_hdr_t));
    memcpy(if_hw,pMac,MAC_LEN);

    DBCK("ra0 Mac: %2x:%2x:%2x:%2x:%2x:%2x\n",if_hw[0],if_hw[1],if_hw[2],if_hw[3],if_hw[4],if_hw[5]);

    if((dev!=NULL) && (*dev!=NULL)){
        memcpy((*dev)->ra0_mac,if_hw,MAC_LEN);
    }else{
        URCP_DEBUG(("%s: error\n", __func__));
        ret = URCP_ANY_ERROR;
    }

    return ret;

}
#endif
#ifdef URCP_TRAP_AP_RA0_MAC
uint16 trap_receive_ap_ra0_mac(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uchar * pMac = NULL;
    uchar if_hw[MAC_LEN];
    DBCK();

    pMac= (uchar*)((uchar*)cmd + sizeof(cmd_hdr_t));
    memcpy(if_hw,pMac,MAC_LEN);

    if((dev!=NULL) && (*dev!=NULL)){
        memcpy((*dev)->ra0_mac,if_hw,MAC_LEN);
    }else{
        URCP_DEBUG(("%s: error\n", __func__));
        ret = URCP_ANY_ERROR;
    }

    return ret;

}
#endif
#ifdef URCP_CMD_AP_RA0_SSID_GET
uint16 get_ap_ra0_ssid(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    cmd_hdr_t* cmd,*scmd;
    uint32 i,num,*apNum = NULL,*rssi=NULL;
    around_ssid_info_t *ssid = NULL;

    uchar *pMac = NULL,*sMac=NULL;

    scmd = (cmd_hdr_t*)cmd_buf;
    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_RA0_SSID);
	cmd->cmd_status = htons(URCP_OK);

    apNum = (uint32*)((uchar*)cmd_buf + sizeof(cmd_hdr_t));
    num = ntohl(*apNum);
    sMac = (uchar*)((uchar*)apNum + sizeof(*apNum));
    apNum = (uint32*)(*app_data + sizeof(cmd_hdr_t));
    *apNum = 0u;
    pMac = (uchar*)((uchar*)apNum + sizeof(*apNum));
    rssi= (uint32*)(pMac + MAC_LEN);
    for(i=0;i<num;i++){
        ssid = urcp_ssid_table_lookup(sMac,0u);
        if(ssid != NULL){
            if(ssid->rssi > -80){
                memcpy(pMac,sMac+MAC_LEN,MAC_LEN);
                *rssi = htonl(ssid->rssi);
                (*apNum)++;
                pMac += MAC_LEN + sizeof(uint32);
                rssi= (uint32*)(pMac + MAC_LEN);
            }
        }
        sMac += MAC_LEN*2;
    }

    /*数据长度*/
    *apNum = htonl(*apNum);
    cmd->length = sizeof(cmd_hdr_t) + (long)pMac - (long)apNum;
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;

}
#endif
#ifdef URCP_CMD_AP_RA0_SSID_GET_REPLY
uint16 receive_ap_ra0_ssid(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint32 i,num,*apNum = NULL,*rssi= NULL;
    int32 raw = -1,col = -1;

    uchar *pMac = NULL;

    apNum = (uint32*)((uchar*)cmd + sizeof(cmd_hdr_t));
    num = ntohl(*apNum);
    pMac = (uchar*)((uchar*)apNum + sizeof(*apNum));
    rssi = (uint32*)(pMac + MAC_LEN);
    col = dev_get_index_by_mac(urcp_info->mac,0u);
    if(col == NONE_NODE){
        return -1;
    }
    for(i=0;i<num;i++){
        raw = dev_get_index_by_mac(pMac,0u);
        if(col != NONE_NODE){
            shm_ptr->ra0_rssi[col-1][raw-1] = ntohl(*rssi);
        }
        pMac += MAC_LEN + sizeof(uint32);
        rssi = (uint32*)(pMac + MAC_LEN);
    }

    return URCP_OK;

}
#endif

#ifdef URCP_CMD_AP_WL_POWER_GET
uint16 get_ap_wl_power(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 *power = NULL;
    cmd_hdr_t* cmd;
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(CMD_AP_WL_POWER);
	cmd->cmd_status = htons(URCP_OK);

    power = (uint32*)(*app_data + sizeof(cmd_hdr_t));
	prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);
	if (NULL != prof) {
        if(!(prof->fitAp.autoPower)){
            *power = (uint32)prof->fitAp.manPower;
        }else{
            *power = 0u;
        }
        *power = htonl(*power);
    }

    /*数据长度*/
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);

    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;

}
uint16 trap_get_ap_wl_power(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint32 *power = NULL;
    cmd_hdr_t* cmd;
	MibProfileType mibType = MIB_PROF_WIRELESS;
	WirelessProfile *prof = NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd->type = htonl(TRAP_AP_WL_POWER);
	cmd->cmd_status = htons(URCP_OK);

    power = (uint32*)(*app_data + sizeof(cmd_hdr_t));
	prof = (WirelessProfile *)ProfGetInstPointByIndex(mibType, 0);

	if (NULL != prof) {
        if(!(prof->fitAp.autoPower)){
            *power = (uint32)prof->fitAp.manPower;
        }else{
            *power = 0u;
        }
        *power = htonl(*power);
    }

    /*数据长度*/
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uint32);

    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;

}
#endif

#ifdef URCP_CMD_AP_WL_POWER_GET_REPLY
uint16 receive_ap_wl_power(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uint32 *power = NULL;

    power = (uint32*)((uchar*)cmd + sizeof(cmd_hdr_t));

    if((dev!=NULL) && (*dev!=NULL)){
        (*dev)->wl_info.power = ntohl(*power);
    }else{
        URCP_DEBUG(("%s: error\n", __func__));
        ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#ifdef URCP_TRAP_AP_WL_POWER
uint16 trap_receive_ap_wl_power(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uint32 *power = NULL;

    power = (uint32*)((uchar*)cmd + sizeof(cmd_hdr_t));

    if((dev!=NULL) && (*dev!=NULL)){
        (*dev)->wl_info.power = ntohl(*power);
    }else{
        URCP_DEBUG(("%s: error\n", __func__));
        ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#ifdef URCP_CMD_AP_WL_POWER_SET
uint16 set_ap_wl_power(cmd_hdr_t *cmd, uchar** app_data)
{
    uint32 *tmp = NULL,wl_pw = 100u;
    cmd_hdr_t* scmd = NULL;
    int32 ret = 0;
    uchar pw;
    char str[64];

    URCP_DEBUG(("%s\n",__func__));

    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uint32*)((uchar*)cmd + sizeof(cmd_hdr_t));
    pw = (uchar)ntohl(*tmp);

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof->fitAp.manPower != pw)
    {
        prof->fitAp.manPower = (uchar)pw;
        switch(pw){
            case 1:
                wl_pw = 10u;
                break;
            case 2:
                wl_pw = 40u;
                break;
            case 3:
                wl_pw = 100u;
                break;
            default:
                wl_pw = 100u;
                break;
        }
        sprintf(str, "iwpriv ra0 set TxPower=%u",wl_pw);
        system(str);
    }

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#ifdef URCP_CMD_AP_SITE_SURVEY_SET
uint16 set_ap_site_survey(cmd_hdr_t *cmd, uchar** app_data)
{
    cmd_hdr_t* scmd = NULL;
    int32 ret = 0;
    uint32 wait_sec,*tmp = 0;

    URCP_DEBUG(("%s\n",__func__));

    scmd = (cmd_hdr_t*)(*app_data);
    tmp = (uint32*)((uchar*)cmd + sizeof(cmd_hdr_t));
    wait_sec = ntohl(*tmp);

    set_ap_collect_timer_allow(wait_sec);

    scmd -> cmd_status = htons(URCP_OK);
    /*构建hello回复命令*/
    scmd -> type = cmd->type;
    scmd -> length = sizeof(cmd_hdr_t);
    *app_data += scmd -> length;
    scmd -> length = htons(scmd->length);

    return ret;
}
#endif
#endif


#if (AP_LOAD_BALANCE == FYES )
int32 ap_load_balance_process(uint32 lbGroudNum){
    int32 ret = 0;
    int32 i;
    uint32 minClient = MAX_SOURCE_SUM,maxClient = 0u,midClient = 0;
    uint32 tmpSum;
    uint32 isAlive = 0u;
    SystemProfile *profSys = NULL;
    ApLoadBalanceProfile *prof=NULL;
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;

    DBCK();

    prof = (ApLoadBalanceProfile*)ProfGetInstPointByIndex(mibType, lbGroudNum-1);
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if((profSys!=NULL) && (prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
    {
        if(prof->lbEn && profSys->loadBalanceEn ){
            tmpSum = prof->source_sum;

            for(i=0;i<prof->num;i++){
                if( prof->ap[i].isAlive ){
                    if( prof->ap[i].client > maxClient ){
                        maxClient = prof->ap[i].client;
                    }
                    if( prof->ap[i].client < minClient ){
                        minClient = prof->ap[i].client;
                    }
                    isAlive++;
                }
            }
            midClient = (minClient+maxClient)/2;

            DBCK("maxClient: %d, midClient: %d, minClient: %d, tmpSum: %d\n",maxClient,midClient,minClient,tmpSum);

	        pthread_mutex_lock(&lbMutex);
#if 0
            if( maxClient < prof->threshold ){
                prof->source_sum = prof->threshold;
            }else{
                if(maxClient-minClient>prof->threshold){
                    prof->source_sum = midClient>prof->threshold ? midClient : prof->threshold;
                }
            }
#else
            prof->source_sum = minClient + prof->threshold;
#endif

            if(isAlive < 2){
                prof->source_sum = MAX_SOURCE_SUM;
            }
            DBCK("prof->threshold: %d , prof->source_sum: %d\n",prof->threshold,prof->source_sum);
            for(i=0;i<prof->num;i++){
                if( prof->ap[i].isAlive ){
                    if( prof->ap[i].client >= prof->source_sum){
                        prof->ap[i].isFull = 1u;
                    }else{
                        prof->ap[i].isFull = 0u;
                    }
                }
            }
            if( prof->source_sum != tmpSum ){
                urcp_set_ap_lb_source_sum(prof,prof->source_sum);
            }

            pthread_mutex_unlock(&lbMutex);

        }
    }

    return ret;

}
#endif

#ifdef URCP_TRAP_CMD_UPPER_LIMIT
uint16 trap_set_upper_limit(cmd_hdr_t *cmd_buf,urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    DBCK();
    uint32 allFull = 1u,isAlive = 0u;
    int i;
    ApLoadBalanceProfile *profLb=NULL;
    SystemProfile *profSys = NULL;
    cmd_hdr_gen_t *trap_hdr_gen_p;

    trap_hdr_gen_p = (cmd_hdr_gen_t *) cmd_buf;

    if((dev!=NULL) && (*dev!=NULL)){

        if( (*dev)->load_balance != 0 ){
            profLb=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_LOAD_BALANCE,(*dev)->load_balance-1);  /*get inst pointer*/
            profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
            if((profSys!=NULL) && (profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
            {
                if(profLb->lbEn && profSys->loadBalanceEn){

	            pthread_mutex_lock(&lbMutex);
                for(i=0;i<profLb->num;i++){
                    if( profLb->ap[i].isAlive ){
                    if (mac_cmp(profLb->ap[i].macAddr.addr, (*dev)->mac) == 0){
                        profLb->ap[i].isFull= 1u;
                        profLb->ap[i].client = ntohl(trap_hdr_gen_p->value);
                        (*dev)->wl_info.clients = ntohl(trap_hdr_gen_p->value);
                    }
                    allFull &= profLb->ap[i].isFull;
                    isAlive++;
                    }
                    DBCK("AP%d is Full %d\n",i,profLb->ap[i].isFull);
                }
                if(allFull && (isAlive >= 2)){
                    DBCK("all AP is Full\n");
                    profLb->source_sum += profLb->threshold;
                    for(i=0;i<profLb->num;i++){
                        if((profLb->ap[i].client<profLb->source_sum) && profLb->ap[i].isAlive){
                            profLb->ap[i].isFull = 0u;
                        }
                    }

                    urcp_set_ap_lb_source_sum(profLb,profLb->source_sum);
                }
	            pthread_mutex_unlock(&lbMutex);
                }
            }
        }
    }

    return 0;
}

#endif

#ifdef URCP_TRAP_CMD_UPPER_LIMIT_SYSLOG
uint16 trap_syslog_upper_limit(cmd_hdr_t *cmd_buf, urcp_dev_t **dev, urcp_infor_t *urcp_info)       //命名规范：trap_syslog_xxxx
{

	return 0;
}
#endif

#ifdef URCP_TRAP_DISCOVER
uint16 trap_receive_discover(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    DBCK("mac: %s", str_macaddr(urcp_info->mac));
	URCP_DEBUG(("debug: %d in %s()\n", __LINE__, __func__));
#ifdef URCP_AC
    uint32 val;
#endif
    URCP_DEBUG(("%s\n", __func__));

#ifdef URCP_AC
    val = ntohl(((cmd_discover_t*)cmd)->send_flag);
	DBCK();
    *dev = dev_get(urcp_info->mac, urcp_info->vid, ntohl(urcp_info->urcp_header->ip), val);
	DBCK();

#endif
    return URCP_OK;
}
#endif

#ifdef URCP_TRAP_PRODUCT_MODEL
/*
 *接收产品型号信息
 *KeHuatao
 */
uint16 trap_receive_product_model(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char *tmp;
    uint32 len;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*长度信息*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	/*判断长度是否合法*/
	if(len<DEV_TYPE_SIZE)
	{
	    /*复制数据*/
	    tmp = (char*)(cmd+1);
	    memcpy(((*dev)->dev_model), tmp, len);
	    (*dev)->dev_model[len] = '\0';
	    URCP_DEBUG(("product model :%s\n", (*dev)->dev_model)); 
	}
    }

    return URCP_OK;
}
#endif

#ifdef URCP_TRAP_SERIAL_NO
/*
 *接收序列号
 *KeHuatao
 */
uint16 trap_receive_serial_no(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint32* sn;
    
    if((dev!=NULL) && (*dev!=NULL))
    {
	/*移动指针*/
	sn = (uint32*)(cmd+1);
	*sn = ntohl(*sn);
	URCP_DEBUG(("SN 0x%08x\n", *sn)); 
    
	/*读取数据*/
	(*dev) -> serial_no = *sn;
    }

    return URCP_OK;
}
#endif

#ifdef URCP_TRAP_DEV_NAME
/*
 *接收设备名
 *KeHuatao
 */
uint16 trap_receive_dev_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char  *tmp;
    uint32 len;
#if (AP_LOAD_BALANCE == FYES)
	ApLoadBalanceProfile *profLb=NULL;
    MibProfileType mibType = MIB_PROF_AP_LOAD_BALANCE;
    int j;
#endif

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*计算长度*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	if(len<MAX_DEV_NAME_LEN)/*长度合法*/
	{
	    /*复制设备名称*/
	    tmp = (char*)(cmd+1);
	    memcpy((*dev)->dev_name, tmp, len);
	    (*dev)->dev_name[len] = (uchar)('\0');
	    URCP_DEBUG(("dev name:%s\n", (*dev)->dev_name)); 
#if (AP_LOAD_BALANCE == FYES )
        if( (*dev)->load_balance != 0 ){
            profLb=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(mibType,(*dev)->load_balance-1);  /*get inst pointer*/
            if((profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
            {
                for(j=0;j<profLb->num;j++){
                    if (mac_cmp(profLb->ap[j].macAddr.addr, (*dev)->mac) == 0){
                        memcpy(profLb->ap[j].dev_name,tmp,len);
                        break;
                    }
                }
            }

        }
#endif
	}
	else/*长度不对*/
	{
	    URCP_DEBUG(("dev name length err\n")); 
	}
    }

    return URCP_OK;
}
#endif

#ifdef URCP_TRAP_SW_VERSION
uint16 trap_receive_sw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char *tmp;
    uint32 len;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*长度信息*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	/*判断长度是否合法*/
	if(len<SW_VERSION_SIZE)
	{
	    /*复制数据*/
	    tmp = (char*)(cmd+1);
	    memcpy((*dev)->sw_version, tmp, len);
	    (*dev)->sw_version[len] = (uchar)('\0');
	    DBCK("sw version:%s\n", (*dev)->sw_version); 
	}
    }

    return URCP_OK;
}
#endif

#ifdef URCP_TRAP_HW_VERSION
uint16 trap_receive_hw_version(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    char *tmp;
    uint32 len;

    if((dev!=NULL) && (*dev!=NULL))
    {
	/*长度信息*/
	len = ntohs(cmd->length) - sizeof(cmd_hdr_t); 
	/*判断长度是否合法*/
	if(len<HW_VERSION_SIZE)
	{
	    /*复制数据*/
	    tmp = (char*)(cmd+1);
	    memcpy((*dev)->hw_version, tmp, len);
	    (*dev)->hw_version[len] = (uchar)('\0');
	    DBCK("hw version:%s\n", (*dev)->hw_version); 
	}
    }

    return URCP_OK;
}
#endif

#if SZONE_AUTO_UPDATA
void szone_auto_updata_to_ap(urcp_dev_t *dev)
{
    int i;
    urcp_ap_join_sz_msg_t urcp_ap_join_sz_msg;
    memset(&urcp_ap_join_sz_msg,0,sizeof(urcp_ap_join_sz_msg));
    for(i=0;i<dev->wl_info.szone_info.szone_num;i++){
        if( i !=0 ){
	        strcat(urcp_ap_join_sz_msg.sz_name, ",");
        }
	    strcat(urcp_ap_join_sz_msg.sz_name, dev->wl_info.szone_info.szone[i]);
    }
    printf("auto updata sz_name: %s %s\n",urcp_ap_join_sz_msg.sz_name,__func__);
	memcpy(urcp_ap_join_sz_msg.general_msg.dst_mac, dev->mac, MAC_LEN);
	urcp_ap_join_sz_msg.general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_AP_JOIN_SZ, (char*)&urcp_ap_join_sz_msg, sizeof(urcp_ap_join_sz_msg));
}
#endif

#ifdef URCP_TRAP_AP_SSID
uint16 trap_receive_ssid(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    //printf("%s\n", __func__);
    int32 val, i;
    uint16 ret=URCP_OK;
    cmd_hdr_gen_t *cmd_ssid=NULL;
    ssid_t *ssid=NULL;
	char *ap_ssid=NULL,tmp[UTT_SSID_LENTH+1];
	ServiceZoneProfile * profSZ=NULL;

    URCP_DEBUG(("%s\n", __func__));
    cmd_ssid = (cmd_hdr_gen_t*)cmd;
    val = ntohl(cmd_ssid->value) * sizeof(ssid_t);
    val = val + sizeof(cmd_hdr_gen_t);
    if( (dev!=NULL) && (*dev!=NULL) && (val==ntohs(cmd->length)))
    {
	val = ntohl(cmd_ssid->value);
	if(val>UTT_SSID_NUM)
	{
	    val = UTT_SSID_NUM;
	}
	(*dev)->wl_info.joined_sz_num = (uint16)ntohl(cmd_ssid->value);
	ssid = (ssid_t*)(cmd_ssid+1);
#if SZONE_AUTO_UPDATA
    uttSemP(SEM_SZONE_UPDATA, 0);
#if 0
    if((*dev)->wl_info.szone_info.auto_updata != 1){
        (*dev)->wl_info.szone_info.szone_changed = 0;
    }
#endif
    if((*dev)->wl_info.szone_info.szone_changed == 1){
        if((*dev)->wl_info.szone_info.count > 0){
            (*dev)->wl_info.szone_info.count -= 1;
        }else{
            szone_auto_updata_to_ap(*dev);
            (*dev)->wl_info.szone_info.r_times += 1;
            switch((*dev)->wl_info.szone_info.r_times){
                case 0:
                    break;
                case 1:
                    (*dev)->wl_info.szone_info.count = 2;
                    break;
                case 2:
                    (*dev)->wl_info.szone_info.count = 4;
                    break;
                case 3:
                    (*dev)->wl_info.szone_info.count = 8;
                    break;
                case 4:
                    (*dev)->wl_info.szone_info.szone_changed = 0;
                    (*dev)->wl_info.szone_info.r_times = 0;
                    (*dev)->wl_info.szone_info.count = 0;
                    break;
                default:
                    (*dev)->wl_info.szone_info.r_times = 0;
                    (*dev)->wl_info.szone_info.count = 0;
                    break;
            }
        }
    }else{
        memset((*dev)->wl_info.szone_info.szone,0,sizeof((*dev)->wl_info.szone_info.szone));
        (*dev)->wl_info.szone_info.szone_num = val;
    }
#endif
    for(i=0;i<val;i++)
    {
        memset((*dev)->wl_info.ssid_info[i].ssid,0,sizeof((*dev)->wl_info.ssid_info[i].ssid));
        URCP_DEBUG(("%s,line=%d,val=%d\n", __func__,__LINE__,val));
#if (SSID_ENCODE_TYPE == FYES)
	    profSZ = (ServiceZoneProfile *)ProfGetInstPointByName(MIB_PROF_SERVICE_ZONE, (ssid + i)->szone);
		if(profSZ != NULL)
		{
	        memset(tmp,0,sizeof(tmp));
	        strncpy(tmp,(ssid + i)->ssid,UTT_SSID_LENTH);
            ap_ssid = iconv_string(tmp,"UTF-8","GBK");
            URCP_DEBUG(("%s,line=%d,ap_ssid=%s,tmp=%s\n", __func__,__LINE__,ap_ssid,tmp));
	        if(ap_ssid != NULL)
	        {
            strncpy((*dev)->wl_info.ssid_info[i].ssid, ap_ssid, UTT_SSID_LENTH);
	        }else
	        {
            strncpy((*dev)->wl_info.ssid_info[i].ssid, tmp, UTT_SSID_LENTH);
	        }
		}
		else
		{
            URCP_DEBUG(("%s,line=%d\n", __func__,__LINE__));
            strncpy((*dev)->wl_info.ssid_info[i].ssid, (ssid + i)->ssid, UTT_SSID_LENTH);
		}
        if(ap_ssid != NULL)
        {
            free(ap_ssid);
            ap_ssid = NULL;
        }
#else
        strncpy((*dev)->wl_info.ssid_info[i].ssid, (ssid + i)->ssid, UTT_SSID_LENTH);
#endif

        URCP_DEBUG(("%s ,%d,ssid:%s\n", __func__,__LINE__, (*dev)->wl_info.ssid_info[i].ssid));
        strncpy((*dev)->wl_info.ssid_info[i].szone, (ssid + i)->szone, UTT_SZONE_LENTH);
#if SZONE_AUTO_UPDATA
        if((*dev)->wl_info.szone_info.szone_changed == 0){
            strncpy((*dev)->wl_info.szone_info.szone[i], (ssid + i)->szone, UTT_SZONE_LENTH);
        }
#endif
        memcpy((*dev)->wl_info.ssid_info[i].mac, (ssid + i)->mac, MAC_LEN);
    }
#if SZONE_AUTO_UPDATA
    uttSemV(SEM_SZONE_UPDATA, 0);
#endif
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }

        URCP_DEBUG(("%s,line=%d\n", __func__,__LINE__));
    //printf("%s\n", __func__);
    return ret;
}
#endif

#ifdef URCP_TRAP_AP_CHANNEL
uint16 trap_receive_ap_channel(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    URCP_DEBUG(("%s\n", __func__));
//    ret = receive_number_channel(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.channel_head))); 
    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.channel_head)));
    if (ret == URCP_OK)
    {
	if (*((uint32 *)&((*dev)->wl_info.channel_head)) != 0)
	{
	    (*dev)->wl_info.channel_head = *((uint32 *)&((*dev)->wl_info.channel_head));
	    (*dev)->wl_info.channel_end = 0;
	}
    }
    return ret;
}

uint16 trap_receive_ap_channel_new(cmd_hdr_t* cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    URCP_DEBUG(("%s\n", __func__));
    ret = receive_number_channel(cmd, dev); 
//    ret = receive_number_channel(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.channel_head)));
    return ret;
}
#endif

#ifdef URCP_TRAP_CURRENT_CLIENTS
uint16 trap_receive_clients(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uint32 client,*ptr;
    cmd_hdr_gen_t *tmp;
    int j;

    URCP_DEBUG(("%s\n", __func__));


    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
    ptr = (uint32*) &((*dev)->wl_info.clients);
    client = ntohl(tmp->value);
//    if( *ptr != client )
    {
        *ptr = client;
#if (AP_LOAD_BALANCE == FYES )
	ApLoadBalanceProfile *profLb=NULL;
    SystemProfile *profSys = NULL;
    if( (*dev)->load_balance > 0 ){
        profLb=(ApLoadBalanceProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_LOAD_BALANCE,(*dev)->load_balance-1);  /*get inst pointer*/
        profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
        if((profSys!=NULL) && (profLb != NULL) && (ProfInstIsFree(profLb) == 0) && (profLb->head.active == TRUE))
        {
            for(j=0;j<profLb->num;j++){
                if (mac_cmp(profLb->ap[j].macAddr.addr, (*dev)->mac) == 0){
                    profLb->ap[j].client = client;
                    break;
                }
            }
            if( profLb->lbEn && profSys->loadBalanceEn ){
                ap_load_balance_process((*dev)->load_balance);    /*处理负载均衡*/
            }
        }
    }
#endif
    }
	URCP_DEBUG(("%s: %d\n", __func__, *ptr));
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#ifdef URCP_TRAP_WIRELESS_MODE
uint16 trap_receive_wireless_mode(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret = URCP_OK;
    uint32 md;
    cmd_hdr_gen_t *tmp=NULL;

    URCP_DEBUG(("xxxxxxxxxxxxxxxxxxxxxxxxxxxx %s\n", __func__));

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
	md = ntohl(tmp->value);
	if( (md==AP_MODE_11BGN) || (md==AP_MODE_11B) || (md==AP_MODE_11G)
	    || (md==AP_MODE_11N) || (md==AP_MODE_11A) || (md==AP_MODE_11BG)
	    || (md==AP_MODE_11AN) || (md==AP_MODE_11GN))
	{
	    (*dev)->wl_info.ap_flag = md;
	    URCP_DEBUG(("%s: %d\n", __func__, md));
	}
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }


    return ret;
}
#endif

#ifdef URCP_TRAP_AP_DHCP_EN
uint16 trap_receive_ap_dhcp_en(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_infor)
{
    uint16 ret = URCP_OK;
    uint32 dhcp_en;
    cmd_hdr_gen_t *tmp=NULL;

    URCP_DEBUG(("%s\n", __func__));

    if((ntohs(cmd->length)==sizeof(cmd_hdr_gen_t)) && (dev!=NULL) && (*dev!=NULL))
    {
	/*复制数据*/
	tmp = (cmd_hdr_gen_t *)cmd;
    (*dev)->dhcp_en = ntohl(tmp->value);
    }
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif

#ifdef URCP_TRAP_AP_SZ_NUM
uint16 trap_receive_ap_sz_num(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;

    ret = receive_number(cmd, dev, (uint32)(&(((urcp_dev_t*)0)->wl_info.ssid_num)));

    return ret;
}
#endif

#ifdef URCP_TRAP_CMD_SZONE_UPDATA_ACK
uint16 trap_receive_szone_updata_ack(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
#if SZONE_AUTO_UPDATA
    uttSemP(SEM_SZONE_UPDATA, 0);
    (*dev)->wl_info.szone_info.szone_changed = 0;
    (*dev)->wl_info.szone_info.r_times = 0;
    (*dev)->wl_info.szone_info.count = 0;
    uttSemV(SEM_SZONE_UPDATA, 0);
#endif
    return ret;
}
#endif

#ifdef URCP_TRAP_CMD_STA_INFO
uint16 trap_set_sta_info(cmd_hdr_t *cmd_buf,urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
        uint16 ret=0u;
        cmd_hdr_sta_info_t *cmd; 

	cmd = (cmd_hdr_sta_info_t *)cmd_buf;
	if (ntohl(cmd->sta_status) == AP_STA_DISCONNECT) {
		ac_sta_table_del(cmd->sta_mac);
	}

	return ret;
}

#endif

#ifdef URCP_TRAP_CMD_STA_INFO_AUTO
uint16 trap_receive_clts_info(cmd_hdr_t *cmd,urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
    uint16 ret=URCP_OK;
    uint16 len;
    cmd_hdr_gen_t *tmp = NULL;
    sta_info_auto_t *sta_info;
    unsigned char *p = NULL; 
    unsigned char *pMac = NULL;
	uint32 i,j;
    char tmp_ssid[32],*ap_ssid=NULL;
    char ssid_encode[32];

    if((dev!=NULL) && (*dev!=NULL))
	{
		/*复制数据*/
        p = (unsigned char *)cmd;
        p += sizeof(cmd_hdr_gen_t);
	    tmp = (cmd_hdr_gen_t *)cmd;
		for(i = 0; i < ntohl(tmp->value); i++) {
            sta_info = (sta_info_auto_t *)p;

#if (SSID_ENCODE_TYPE == FYES)
            memset(tmp_ssid,0,sizeof(tmp_ssid));
            memset(ssid_encode,0,sizeof(ssid_encode));
            strcpy(tmp_ssid,sta_info->ssid);
            ap_ssid = iconv_string(tmp_ssid,"UTF-8","GBK");
            URCP_DEBUG(("%s,line=%d,ap_ssid=%s,tmp_ssid=%s\n", __func__,__LINE__,ap_ssid,tmp_ssid));
            if(ap_ssid != NULL)
            {
                   strcpy(ssid_encode,ap_ssid);
            }else
            {
                   strcpy(ssid_encode,tmp_ssid);
            }
            if(ap_ssid != NULL)
            {
                free(ap_ssid);
                ap_ssid = NULL;
            }
#else
            strcpy(ssid_encode,sta_info->ssid);
#endif
            if( !strcmp((*dev)->wl_info.ssid_info[i].ssid, ssid_encode) ){
		        memset((*dev)->wl_info.ssid_info[i].sta_info,0,sizeof((*dev)->wl_info.ssid_info[i].sta_info));
                (*dev)->wl_info.ssid_info[i].sta_num = ntohs(sta_info->sta_num);
                pMac = p + ntohs(sta_info->sta_mac_offset);
                for(j=0;j<ntohs(sta_info->sta_num);j++){
                    memcpy((*dev)->wl_info.ssid_info[i].sta_info[j].sta_mac, pMac, MAC_LEN);
                    pMac += MAC_LEN;
                }
            }
            p += ntohs(sta_info->info_length);
        }
	}
    else
    {
	URCP_DEBUG(("%s: error\n", __func__));
	ret = URCP_ANY_ERROR;
    }

    return ret;

}
#endif

#ifdef URCP_TRAP_CMD_STA_INFO_SYSLOG
uint16 trap_syslog_sta_info(cmd_hdr_t *cmd_buf, urcp_dev_t **dev, urcp_infor_t *urcp_info)
{
	DBCK("%s", str_macaddr(urcp_info->mac));
	cmd_hdr_sta_info_t *cmd;
	char log_buf[MAX_LOG_LENGTH - 32];
	char log_buf_tmp[MAX_LOG_LENGTH - 32];

	cmd = (cmd_hdr_sta_info_t *)cmd_buf;

	if (ntohl(cmd->sta_status) == AP_STA_CONNECT) {
		DBCK();
		sprintf(log_buf, "ap mac:%s; ", str_macaddr(urcp_info->mac));
		sprintf(log_buf_tmp, "event:STA %s connect", str_macaddr(cmd->sta_mac));
		strcat(log_buf, log_buf_tmp);
	} else {
		sprintf(log_buf, "ap mac:%s; ", str_macaddr(urcp_info->mac));
		sprintf(log_buf_tmp, "event:STA %s disconnect", str_macaddr(cmd->sta_mac));
		strcat(log_buf, log_buf_tmp);
	}

	DBCK("%s", log_buf);
	ap_syslog(APUSERLOG, log_buf);

	if ((ntohl(cmd->sta_status) == AP_STA_CONNECT) && (ntohl(cmd->value) == 20)) {
		sprintf(log_buf, "ap mac:%s; event:The client num reached 20", str_macaddr(urcp_info->mac));
		DBCK("%s", log_buf);
		ap_syslog(APUSERNUM, log_buf);
	}

	return 0;
}

#endif
#ifdef URCP_CMD_AP_MF_NAME_GET
uint16 trap_get_ap_mf_name(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    int i;
	uint16 j=0;
    char *tmp=NULL;
    char *index=NULL;
    cmd_hdr_gen_t* cmd;
    ApMacFilterProfile *prof= NULL;
    MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;

    cmd = (cmd_hdr_gen_t*)(*app_data);
    cmd -> type = htonl(TRAP_AP_MF_NAME);
    tmp = (char *)(*app_data + sizeof(cmd_hdr_gen_t));

    memset(tmp, '\0', 64);
    for (i=0; i< URCP_CMD_AP_MF_NUM; i++)
    {
	prof = (ApMacFilterProfile*)ProfGetInstPointByIndex(mibType,i);
	if((prof != NULL) && (prof->head.active == TRUE))
	{
		j++;
	    index = strchr(tmp, '\0');
	    if (i == 0)
	    {
		strncpy(&index[0], prof->head.name, (UTT_INST_NAME_LEN+1));
	    }
	    else
	    {
		index[0]=',';
		strncpy(&index[1], prof->head.name, (UTT_INST_NAME_LEN+1));
	    }
	}
    }
    URCP_DEBUG(("< %s %d > tmp:%s\n", __func__, __LINE__, tmp));
    cmd -> value = htonl(j);
    cmd->length = sizeof(cmd_hdr_gen_t) + 64;
    cmd->cmd_status = htons(URCP_OK);
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return URCP_OK;
}

#endif

#ifdef URCP_TRAP_AP_MF_NAME
uint16 trap_receive_ap_mf_name(cmd_hdr_t* cmd, urcp_dev_t** dev, urcp_infor_t* urcp_info)
{
    uchar *tmp;
    int i=0;
    char *ptr = NULL;
    char *mfNames = NULL;
    uint16 ulen, ret=URCP_OK;
    /*长度信息*/
    ulen = ntohs(cmd->length) - (uint16)sizeof(cmd_hdr_gen_t); 
    /*判断长度是否合法*/
    if((ulen<=(UTT_INST_NAME_LEN+1)*6) && (dev!=NULL) && (*dev!=NULL))
    {
	tmp = (uchar*)cmd;
	tmp = tmp + sizeof(cmd_hdr_gen_t);/*跳过命令头数据*/
	DBCK("tmp=%s\n",tmp);
	mfNames=strtok_r(tmp,",",&ptr);
	while(mfNames != NULL)
	{
	    strncpy((*dev)->wl_info.mfNames[i], mfNames, UTT_SSID_LENTH);/*service zone name*/
	//    strncpy((*dev)->wl_info.mfNames[i], mfNames, sizeof(mfNames));/*service zone name*/
	    mfNames=strtok_r(NULL,",",&ptr);
	    i++;
	}
	(*dev)->wl_info.ap_mac_filter_num = ntohl(((cmd_hdr_gen_t*)cmd)->value);
#if 0
	    strncpy((*dev)->wl_info.ssid_info[i].szone, tmp, UTT_SZONE_LENTH);/*service zone name*/
#endif
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }

    return ret;
}
#endif
#ifdef URCP_CMD_AP_DISCONNECT_STA_SET
uint16 ap_disconnect_sta(cmd_hdr_t* cmd, uchar** app)
{
     uchar buf[64], *data=NULL;
     uint16 i, num, ret=URCP_OK;
 
     num = *((uint16*)((uchar*)cmd + sizeof(cmd_hdr_t)));
     num = ntohs(num);
 
     data = (uchar*)((uchar*)cmd + sizeof(cmd_hdr_t) + sizeof(uint16));
     for(i=0; i<num; i++)
     {
         if(isValidMac(data) == 1) {
           sprintf(buf, "iwpriv ra0 set DisConnectSta=%02x:%02X:%02X:%02X:%02X:%02X",
                   data[0],data[1],data[2],data[3],data[4],data[5]);
           data += MAC_LEN;
           system(buf);
       }
     }
 
     return ret;
}
#endif
#ifdef URCP_AP
#ifdef URCP_TRAP_CMD_AP_STAT//AP流量统计功能
uint16 send_if_stat(cmd_hdr_stat_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info)
{
    uint16 ret;
    cmd_hdr_stat_t* cmd;
	
	unsigned int rx = 0,tx = 0;
	uchar if_name[32];
	memset(if_name, 0, sizeof(if_name));	
	get_switch_eth_name(if_name);/*获取LAN名字,此函数要将LAN口收发字节数封装到数据报文中*/
	get_if_stat(if_name,&rx,&tx);/*获取收发字节数rx和tx*/ 
    cmd = (cmd_hdr_stat_t*)(*app);
    /*构建hello回复命令*/
    cmd ->type = htonl(TRAP_CMD_AP_STAT);
    cmd ->length = sizeof(cmd_hdr_stat_t);
	cmd->rx_byte = htonl(rx);
	cmd->tx_byte = htonl(tx);
	ret = URCP_OK;
    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app += cmd -> length;
    cmd -> length = htons(cmd->length);
    return ret;
}

uint16 send_if_rate(cmd_hdr_rate_t *cmd_buf, uchar **app, urcp_infor_t* urcp_info)
{
    uint16 ret;
    cmd_hdr_rate_t* cmd;
	uint32 rx_rate = 0,tx_rate = 0;	
	get_if_rate(&rx_rate,&tx_rate);
    cmd = (cmd_hdr_rate_t*)(*app);
    cmd -> type = htonl(TRAP_CMD_AP_RATE);
    cmd -> length = sizeof(cmd_hdr_rate_t);
	cmd-> rx_rate = htonl(rx_rate);
	cmd-> tx_rate = htonl(tx_rate);
	ret = URCP_OK;
    cmd -> cmd_status = (int16)htons((uint16)ret);
    /*数据长度*/
    *app += cmd -> length;
    cmd -> length = htons(cmd->length);
    return ret;
}
#endif
#endif
#ifdef URCP_AC
#ifdef URCP_TRAP_CMD_AP_STAT//AP流量统计功能
uint16 ac_save_stat_call_back(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
	cmd_hdr_stat_t* scmd = NULL;
	scmd = (cmd_hdr_stat_t *)(uchar*)cmd;
	if((*dev) != NULL)
	{
		(*dev)->acStat.rx_byte = ntohl(scmd->rx_byte);
		(*dev)->acStat.tx_byte = ntohl(scmd->tx_byte);
		(*dev)->updateStat = 1;
		ac_auto_save_stat();
	}
	return 0;
}
uint16 ac_save_rate_call_back(cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info)
{
	cmd_hdr_rate_t* scmd = NULL;
	scmd = (cmd_hdr_rate_t *)(uchar*)cmd;
	if((*dev) != NULL)
	{
		(*dev)->acStat.rx_rate = ntohl(scmd->rx_rate);
		(*dev)->acStat.tx_rate = ntohl(scmd->tx_rate);
		(*dev)->updateRate = 1;
		ac_auto_save_rate();
	}
	return 0;
}
#endif
#endif


#ifdef URCP_CMD_AP_SYS_INFO_GET
/*获取AP的当前运行时间，当前时间点的CPU使用率，内存使用率*/
uint16 trap_get_ap_sys_info(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    int ret = URCP_OK;
    cmd_hdr_t *cmd = NULL;
    struct sysinfo sys;
    uint32 *p_up_time = NULL;
    uchar *p_cpu = NULL;
    uchar *p_mem = NULL;
    int cpu = 0, mem = 0;   

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_AP_SYS_INFO);
    p_cpu  = *app_data + sizeof(cmd_hdr_t);
    p_mem = p_cpu + sizeof(uchar);
    p_up_time = (uint32 *)(p_mem + sizeof(uchar));
    /*获取CPU,MEM使用率*/
    getSystemCpu(&cpu); 
    *p_cpu = (uchar)cpu;
    if (!getSystemMemory(&mem))
    {
	*p_mem = (uchar)mem;
    }
    else
    {
	*p_mem = (uchar)0u;
    }
    /*获取时间*/
    if( !sysinfo(&sys))
    {
	*p_up_time = htonl((uint32)(sys.uptime));
    }
    else
    {
	*p_up_time = htonl(0);
    }
//    printf("%s, %d, %d, %d, %d\n", __func__, *p_mem, *p_cpu, ntohl(*p_up_time));
    cmd->length = sizeof(cmd_hdr_t) + sizeof(uchar) + sizeof(uchar) + sizeof(uint32);
    cmd->cmd_status = htons(ret);
    
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return ret;

}

uint16 trap_get_ap_rf_rtx(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint16 ret = URCP_OK;
    cmd_hdr_t *cmd = NULL;
    uint32 *p_tx_p = NULL, *p_tx_b = NULL, *p_rx_p = NULL, *p_rx_b = NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_AP_RF_RTX_INFO);
    p_tx_p = (uint32 *)(*app_data + sizeof(cmd_hdr_t));
    p_tx_b = p_tx_p + 1;
    p_rx_p = p_tx_b + 1;
    p_rx_b = p_rx_p + 1;

    if (!get_ap_RTX_info(p_tx_p, p_tx_b, p_rx_p, p_rx_b))
    {
	*p_tx_p = htonl(*p_tx_p);
	*p_tx_b = htonl(*p_tx_b);
	*p_rx_p = htonl(*p_rx_p);
	*p_rx_b = htonl(*p_rx_b);
    }
    else
    {
    	*p_tx_p = htonl(0);
	*p_tx_b = htonl(0);
	*p_rx_p = htonl(0);
	*p_rx_b = htonl(0);
    }

//    printf("%s, %u, %u, %u, %u\n", __func__, ntohl(*p_tx_p), ntohl(*p_tx_b), ntohl(*p_rx_p), ntohl(*p_rx_b));
    cmd->length = sizeof(cmd_hdr_t) + 4*sizeof(uint32);
    cmd->cmd_status = htons(ret);
    
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return ret;
}

/*发送AP射频相关信息*/
uint16 trap_get_ap_rf(cmd_hdr_t *cmd_buf, uchar **app_data, urcp_infor_t* urcp_info)
{
    uint16 ret = URCP_OK;
    cmd_hdr_t *cmd = NULL;
    uint16 rate = 0, power = 0, bea = 0, chan_width = 0;
    uint16 *p_rate = NULL, *p_power = NULL, *p_bea = NULL, *p_chanw = NULL;
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;

    cmd = (cmd_hdr_t*)(*app_data);
    /*构建hello回复命令*/
    cmd -> type = htonl(TRAP_AP_RF_INFO);
    /*速率,频宽，功率，beacon*/
    p_rate = (uint16 *)(*app_data + sizeof(cmd_hdr_t));
    p_chanw = p_rate + 1;
    p_power = p_chanw + 1;
    p_bea = p_power + 1;

    if (!get_ap_rf_rate(&rate))
    {
	*p_rate = htons(rate);
    }
    else
    {
	*p_rate = htons(0);
    }
    if (!get_ap_power(&power))
    {
	*p_power = htons(power);
    }
    else
    {
	*p_power = htons(0);
    }
    if (!get_ap_beacon(&bea))
    {
	*p_bea = htons(bea);
    }
    else
    {
	*p_bea = htons(0);
    }

    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (prof != NULL)
    {
	*p_chanw = htons(prof->mBaseCf.ChanWidth);
    }
    else
    {
	*p_chanw = htons(0);
    }

//    printf("%s, %d, %d, %d, %d\n", __func__, ntohs(*p_rate), ntohs(*p_power), ntohs(*p_bea), ntohs(*p_chanw));
    cmd->length = sizeof(cmd_hdr_t) + 4*sizeof(uint16);
    cmd->cmd_status = htons(ret);
    
    *app_data += cmd->length;
    cmd->length = htons(cmd->length);

    return ret;
}
#endif

#ifdef URCP_TRAP_AP_SYS_INFO
uint16 trap_receive_ap_sys_info( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info )
{
    uint16 ret = URCP_OK;
    uchar *p_cpu = (uchar *)cmd + sizeof(cmd_hdr_t);
    uchar *p_mem = p_cpu + sizeof(uchar);
    uint32 *p_up_time = (uint32 *)(p_mem + sizeof(uchar));

    if((ntohs(cmd->length) == sizeof(cmd_hdr_t) + sizeof(uchar) + sizeof(uchar) + sizeof(uint32)) && (dev!=NULL) && (*dev!=NULL))
    {
	(*dev)->wl_info.cpu_rate = *p_cpu;
	(*dev)->wl_info.mem_rate = *p_mem;
	(*dev)->wl_info.up_time = ntohl(*p_up_time);
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }

//    printf("%s, %d, %d, %d\n", __func__, (*dev)->wl_info.cpu_rate,(*dev)->wl_info.mem_rate,(*dev)->wl_info.up_time);
    return ret;
}

uint16 trap_receive_ap_rf_info( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info )
{
    uint16 ret = URCP_OK;
    uint16 *p_rate = NULL, *p_power = NULL, *p_bea = NULL, *p_chanw = NULL;
    
    p_rate = (uint16 *)((uchar *)cmd + sizeof(cmd_hdr_t));
    p_chanw = p_rate + 1;
    p_power = p_chanw + 1;
    p_bea = p_power + 1;

    if ((ntohs(cmd->length) == sizeof(cmd_hdr_t) + 4*sizeof(uint16)) && (dev != NULL) && (*dev != NULL))
    {
	(*dev)->wl_info.bit_rate = ntohs(*p_rate);
	(*dev)->wl_info.chan_width = ntohs(*p_chanw);
	(*dev)->wl_info.power_new = ntohs(*p_power);
	(*dev)->wl_info.beacon = ntohs(*p_bea);
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }

//    printf("%s, %d, %d, %d, %d\n", __func__,(*dev)->wl_info.bit_rate, (*dev)->wl_info.chan_width, (*dev)->wl_info.power_new, (*dev)->wl_info.beacon );
    return ret;
}

uint16 trap_receive_ap_rf_rtx_info( cmd_hdr_t *cmd, urcp_dev_t **dev, urcp_infor_t* urcp_info )
{
    uint16 ret = URCP_OK;
    uint32 *p_tx_p = NULL, *p_tx_b = NULL, *p_rx_p = NULL, *p_rx_b = NULL;
   
    p_tx_p = (uint32 *)((uchar *)cmd + sizeof(cmd_hdr_t));
    p_tx_b = p_tx_p + 1;
    p_rx_p = p_tx_b + 1;
    p_rx_b = p_rx_p + 1;
    
    if ((ntohs(cmd->length) == sizeof(cmd_hdr_t) + 4*sizeof(uint32)) && (dev != NULL) && (*dev != NULL))
    {
    	(*dev)->wl_info.tx_packets = ntohl(*p_tx_p);
	(*dev)->wl_info.tx_bytes = ntohl(*p_tx_b);
	(*dev)->wl_info.rx_packets = ntohl(*p_rx_p);
	(*dev)->wl_info.rx_bytes = ntohl(*p_rx_b);
    }
    else
    {
	ret = URCP_ANY_ERROR;
    }
    
//    printf("%s, %d, %d, %d, %d\n", __func__, ntohl(*p_tx_p), ntohl(*p_tx_b), ntohl(*p_rx_p), ntohl(*p_rx_b));
    return ret;
}
#endif

#ifdef URCP_CMD_AP_ADD_MAC_BLACK
#define SSID_NUM 10
struct _ssid_id_info
{
    int id;
    char ssid[UTT_SSID_LENTH + 1];
    char sz[UTT_SZONE_LENTH + 1];
};
typedef struct _urcp_black_list_t{
    char ssid[UTT_SSID_LENTH];   /* SSID名称        */
    unsigned char staMac[MAC_LEN];
}urcp_black_list_t;
typedef struct _urcp_black_list_msg_t{
    unsigned short mode;	/*1添加，2删除*/
    unsigned short staNum;
    urcp_black_list_t black_list[1];
}urcp_black_list_msg_t;

static int find_ssid(struct _ssid_id_info ssid_info[], char *str)
{
    int i = 0;
    for (i = 0; i < SSID_NUM; i++)
    {
	if (strncmp(ssid_info[i].ssid, str, UTT_SSID_LENTH) == 0)
	{
	    return i;
	}
    }
    if (i == SSID_NUM)
    {
	return -1;
    }

    return 0;
}
static int find_sz(struct _ssid_id_info ssid_info[], char *sz)
{
    int i = 0;
    for (i = 0; i < SSID_NUM; i++)
    {
	if (strncmp(ssid_info[i].sz, sz, UTT_SZONE_LENTH) == 0)
	{
	    return i+1;
	}
    }
    return 0;
}
static char *get_sz_by_ssid(struct _ssid_id_info ssid_info[], char *str)
{
    int i = 0;
    for (i = 0; i < SSID_NUM; i++)
    {
	if (strncmp(ssid_info[i].ssid, str, UTT_SSID_LENTH) == 0)
	{
	    return ssid_info[i].sz;
	}
    }
    if (i == SSID_NUM)
    {
	return NULL;
    }
}

/*比较服务区的名称*/
static int sz_cmp(char *sz_all, char *sz_name)
{
    int i = 0;
    char *sz = NULL;
    if ( (sz_all == NULL) || (sz_name == NULL))
    {
	return -1;
    }
    for ( sz = sz_all; sz < sz_all + MAX_AP_MAC_FILTER_NUM*(UTT_INST_NAME_LEN + 1); sz += (UTT_INST_NAME_LEN + 1))
    {
	if (strncmp(sz, sz_name, UTT_INST_NAME_LEN) == 0)
	{
	    return 0;
	}
    }
    if (sz == sz_all + MAX_AP_MAC_FILTER_NUM*(UTT_INST_NAME_LEN + 1) )
    {
	return 1;
    }
}

static void del_all_black()
{
    ApMacFilterProfile *prof = NULL, *prof_free = NULL;
    MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
    int min = 0, max = 0, index = 0, k =0;
    ProfInstLowHigh(mibType, &max, &min);
    for (index = URCP_CMD_AP_MF_NUM; index < max; index++)
    {
	prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (TRUE == prof->head.active) && ((ProfInstIsFree((Pointer)prof)) == 0) && (prof->mode == 2))
	{
#if 0
	    /*清空后十六MAC的空间*/
	    memset(&(prof->mac[MAX_MAC_FILTER_SIGNAL_AP]), 0, (MAX_MAC_BLACK_NUM - MAX_MAC_FILTER_SIGNAL_AP)*MAC_LEN);
	    if (0 == prof->num)
	    {
//		prof->head.active = FALSE;
		ProfDelInstByIndex(mibType, index);
	    }
#else
	
	    ProfDelInstByIndex(mibType, index);
#endif
	}
    }
}

static int change_mac_filter(int mode, char *sz, unsigned char mac[MAC_LEN])
{
    ApMacFilterProfile *prof = NULL, *prof_free = NULL;
    MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
    int min = 0, max = 0, index = 0, k =0, index_free = 0, zero_flag = 0;
    unsigned char mac_null[MAC_LEN] = "";
    char name_free[16] ="";

    ProfInstLowHigh(mibType, &max, &min);
    for (index = URCP_CMD_AP_MF_NUM; index < max; index++)
    {
	prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
//	printf("%s,index:%d,head.name:%s\n",__func__, index, prof->head.name);
	if ((prof != NULL) && (TRUE == prof->head.active) && ((ProfInstIsFree((Pointer)prof)) == 0) && (prof->mode == 2))
	{
	    if (sz_cmp(prof->sz[0], sz) != 0)
	    {
		/*说明该mac过滤表中不包含对应的服务区*/
		continue;
	    }

	    if (1 == mode)
	    {
//		for( k = MAX_MAC_FILTER_SIGNAL_AP; k < MAX_MAC_BLACK_NUM; k++)
		for( k = 0; k < MAX_MAC_BLACK_NUM; k++)
		{
		    if (memcmp(&(prof->mac[k]), mac_null, MAC_LEN) == 0)
		    {
			memcpy(&(prof->mac[k]), mac, MAC_LEN);
			/*要不要增加prof->num*/
			break;
		    }
		}
		if (k != MAX_MAC_BLACK_NUM)
		{
		    break;
		}
	    }
	    else if (2 == mode)
	    {
		zero_flag = 0;
//		for (k = MAX_MAC_FILTER_SIGNAL_AP; k < MAX_MAC_BLACK_NUM; k++)
		for (k = 0; k < MAX_MAC_BLACK_NUM; k++)
		{
		    if (memcmp(&(prof->mac[k]), mac, MAC_LEN) == 0)
		    {
			memset(&(prof->mac[k]), 0, MAC_LEN);
			//break;
		    }
		    else if(memcmp(&(prof->mac[k]), mac_null, MAC_LEN) != 0)
		    {
			zero_flag++;
		    }
		}
		if (0 == zero_flag)
		{
		    if ( 0 == prof->num)
		    {
//			prof->head.active = FALSE;
			ProfDelInstByIndex(mibType, index);
		    }
		}
//		if (k != MAX_MAC_BLACK_NUM)
//		{
//		    break;
//		}
	    }
	}
	else if((prof != NULL) && (prof->head.active != TRUE))
	{
	    prof_free = prof;
	    index_free = index;
	}
    }
    if ((index == max) && (prof_free != NULL) && (1 == mode))
    {
	/*新建实例*/
	prof_free->head.active = TRUE;
//	sprintf(name_free, "BlackList%d", index_free);
//	strcpy(prof_free->head.name, name_free);
	prof_free->num = 0;			    /*新建实例，不会使用前32的空间，数目是否要增减*/
	prof_free->mode = 2;			    /*禁止*/
	strncpy(prof_free->sz[0], sz, UTT_INST_NAME_LEN);
	memcpy(&(prof_free->mac[0]), mac, MAC_LEN);
//	printf("%s,新增实例%d\n", __func__,index_free);
    }
    return 0;
}

uint16 ap_add_mac_black(cmd_hdr_t *cmd, uchar** app_data)
{
    uint32 num  = 0, i = 0, j = 0, k = 0, rel_num = 0, flag = 0, mode = 0;
    urcp_black_list_msg_t *msg;
    int min = 0, max = 0;
    struct _ssid_id_info ssid_info[SSID_NUM];
    MibProfileType mibTypeFAS = MIB_PROF_FIT_AP_SZ;
    MibProfileType mibTypeWl = MIB_PROF_WIRELESS;
    MibProfileType mibTypeMC = MIB_PROF_AP_MAC_FILTER;
//    MibProfileType mibTypeBl=MIB_PROF_AP_BLACK_WHITE_LIST;
//    ApBlackWhiteListProfile *profBl = NULL, *bl_free = NULL;;
    WirelessProfile *profWl = NULL;
    ApMacFilterProfile *profMC = NULL;
    FitApSzProfile *profFAS = NULL;
    urcp_black_list_t *list;
    msg = (urcp_black_list_msg_t *)((uchar *)cmd + sizeof(cmd_hdr_t));
    num = ntohs(msg->staNum);
    rel_num = num;	    //记录当前黑名单个数
    mode = ntohs(msg->mode);
    char *sz = NULL;
    unsigned char mac_null[MAC_LEN] = "";
#if 0
    printf("%s, num:%d, mode:%d\n", __func__, num, mode);
    for (i = 0; i < num; i++)
    {
	list = &(msg->black_list[i]);
	printf("%s,%d,ssid:%s, MAC:%s\n", __func__, i, list->ssid, str_macaddr(list->staMac));
    }
#endif
#if 1
        
    if (rel_num > 0)
    {
	/*记录当前AP的所有的ssid*/
	memset(ssid_info, 0, sizeof(ssid_info));
	ProfInstLowHigh(mibTypeWl, &max, &min);
	for (i = min + 1; i < max; i++) 
	{
	    profWl = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeWl, i);
	    if ((profWl != NULL) && (TRUE == profWl->head.active) && ((ProfInstIsFree((Pointer)profWl)) == 0))
	    {
		flag = 1;
		strncpy(ssid_info[i-1].ssid, profWl->mBaseCf.SSID1, UTT_SSID_LENTH);
//		printf("%s,ssid:%s\n", __func__, profWl->mBaseCf.SSID1);
		ssid_info[i-1].id = i - 1;
	    }
	}
	if (1 != flag) 
	{
	    rel_num=0;
	    /*
	       profWl = (WirelessProfile *)ProfGetInstPointByIndex(mibTypeWl, 0);
	       if ((profWl != NULL) && (TRUE == profWl->head.active) && ((ProfInstIsFree((Pointer)profWl)) == 0))
	       {
	       strncpy(ssid_info[0].ssid, proWl->rofWl->mBaseCf.SSID1, UTT_SSID_LENTH);
	       ssid_info[0].id = 0;
	       }
	       */
	}
    }
    /*记录当前AP所加入的服务区*/
    if (rel_num > 0)
    {
	ProfInstLowHigh(mibTypeFAS, &max, &min);
	for (i = min; i < max; i++) 
	{
	    profFAS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeFAS, i);
	    if ((profFAS != NULL) && (TRUE == profFAS->head.active) && ((ProfInstIsFree((Pointer)profFAS)) == 0))
	    {   
		strncpy(ssid_info[i].sz, profFAS->head.name, UTT_SZONE_LENTH);
//		printf("%s, sz:%s\n", __func__, profFAS->head.name);
		flag = 1;
	    }   
	}   
	if (1 != flag) 
	{
	    rel_num = 0;
	}
    }

    if (rel_num > 0)
    {
	/*检查当前黑名单数据是否有多余服务区*/
	ProfInstLowHigh(mibTypeMC, &max, &min);
	for (i = URCP_CMD_AP_MF_NUM; i < max; i++)
	{
	    profMC = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibTypeMC, i);
	    if ((profMC != NULL) && (TRUE == profMC->head.active) && ((ProfInstIsFree((Pointer)profMC)) == 0))
	    {
		if (find_sz(ssid_info, profMC->sz[0]) == 0)
		{
		    /*多余的黑名单列表*/
//		    printf("%s,删除多余的黑名单列表%d,sz:%s\n",__func__, i, profMC->sz[0]);
		    ProfDelInstByIndex(mibTypeMC, i);
		}
	    }
	}
    }

    /*去除其中不属于自己的黑名单*/
    for (i = 0; (i < num) && (rel_num > 0) && (mode != 3); i++)
    {
	list = &(msg->black_list[i]);
	if (find_ssid(ssid_info, list->ssid) < 0)
	{
	    list->ssid[0] = '\0';		/*从黑名数据中删除不符合的数据*/
	    rel_num--;
	    continue;
	}

	sz = get_sz_by_ssid(ssid_info, list->ssid);
	/*遍历和和过滤黑白MAC地址表*/
	ProfInstLowHigh(mibTypeMC, &max, &min);
	for (j = min; j < URCP_CMD_AP_MF_NUM; j++)
	{
	    profMC = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibTypeMC, j);
	    if ((profMC != NULL) && (TRUE == profMC->head.active) && ((ProfInstIsFree((Pointer)profMC)) == 0) && (profMC->num > 0))
	    {
		if (sz_cmp(profMC->sz[0], sz) != 0)
		{
		    /*说明该mac过滤表中不包含对应的服务区*/
		    continue;
		}
		if (1 == profMC->mode)
		{
		    if (1 == mode)
		    {
			/*mac过滤允许表,查找重复的从白名单中删除,添加黑名单模式*/
			for (k = 0 ; k < MAX_MAC_FILTER_SIGNAL_AP; k++)
			{
			    if (memcmp(list->staMac, &(profMC->mac[k]), MAC_LEN) == 0)
			    {
				/*从白名单中删除*/
				memset(&(profMC->mac[k]), 0, MAC_LEN);
				profMC->num--;
				sz = NULL;
				break;
			    }
			}
			if (k != MAX_MAC_FILTER_SIGNAL_AP)
			{
			    break;
			}
		    }
		    else if (2 == mode)
		    {
			/*MAC过滤允许表,删除黑名单模式*/
		    }
		}
		else if (2 == profMC->mode)
		{
		    if (1 == mode)
		    {
			/*MAC过滤禁止表,既然在MAC禁止表，说明MAC不应该被AC所看到，添加黑名单模式*/
		    }
		    else if (2 == mode)
		    {
			/*MAC过滤禁止表，从中去除相应的mac，删除黑名单模式*/
		    	for (k = 0 ; k < MAX_MAC_FILTER_SIGNAL_AP; k++)
			{
			    if (memcmp(list->staMac, &(profMC->mac[k]), MAC_LEN) == 0)
			    {
				memset(&(profMC->mac[k]), 0, MAC_LEN);
				profMC->num--;
				sz = NULL;
				break;
			    }
			}
			if (k != MAX_MAC_FILTER_SIGNAL_AP)
			{
			    break;
			}
		    }
		}

	    }
	}

	if (sz != NULL)
	{
	    /*修改黑名单实例，包括增加和删除*/
//	    printf("%s,修改实例\n", __func__);
	    change_mac_filter(mode, sz, list->staMac);
	}

    }

    if (3 == mode)
    {
	del_all_black();
    }
    
    if ((rel_num > 0) || (3 == mode))
    {
	nvramWriteCommit();
	config_backup_state(NEED_BACKUP);
    }
#endif
    return 0;
}

/**/
static int enabled_mac_filter(int mode, int ra_num, char *mac_all)
{
    char str[100] = "";
    sprintf(str, "iwpriv ra%d set ACLClearAll=1", ra_num);
    system(str);
    memset(str, 0, sizeof(str));
    sprintf(str, "iwpriv ra%d set AccessPolicy=%d", ra_num, mode);
    system(str);
    system(mac_all);
//    printf("%s,mac_all:%s\n",__func__, mac_all);
    return 0;
}

static char *smac2str(unsigned char mac[MAC_LEN], char *str)
{
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X;", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return str;
}

void serivce_zone_mac(void)
{
    char sz[4][UTT_SZONE_LENTH];
    int sz_num = 0, i = 0, mode = 0, min =0 , max = 0, j = 0, index = 0;
    MibProfileType mibTypeFAS = MIB_PROF_FIT_AP_SZ;
    MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
    ApMacFilterProfile *prof = NULL;
    FitApSzProfile *profFAS = NULL;
    char *chr = NULL;
    char mac_all[8700] = "";
    char mac_str[18+1] = "";
    unsigned char mac_null[MAC_LEN] = "";

    memset(sz, 0, sizeof(sz));
    /*获取当前AP的服务区*/
    sz_num = 0;
    ProfInstLowHigh(mibTypeFAS, &max, &min);
    for (i = min; i < max; i++) 
    {
	profFAS = (FitApSzProfile *)ProfGetInstPointByIndex(mibTypeFAS, i);
	if ((profFAS != NULL) && (TRUE == profFAS->head.active) && ((ProfInstIsFree((Pointer)profFAS)) == 0))
	{   
	    strncpy(sz[sz_num], profFAS->head.name, UTT_SZONE_LENTH);
	    sz_num++;
	}   
    }   

    /*以服务区为单位查找自己服务区下的规则和数据*/
    for(i = 0; i < sz_num; i++)
    {
	mode = 0;
	memset(mac_all, 0, sizeof(mac_all));
	sprintf(mac_all, "iwpriv ra%d set ACLAddEntry=\"", i);
	ProfInstLowHigh(mibType, &max, &min);
	for (index = min; index < max; index++)
	{
	    prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	    if ((prof != NULL) && (TRUE == prof->head.active) && ((ProfInstIsFree((Pointer)prof)) == 0))
	    {
		for(j = 0; j < MAX_AP_MAC_FILTER_NUM; j++)
		{
		    if (strncmp(prof->sz[j], sz[i], UTT_INST_NAME_LEN) == 0)
		    {
		//	printf("%s,找到满足条件的SZ:%s\n", __func__, sz[i]);
			break;
		    }
		}
		if (j == MAX_AP_MAC_FILTER_NUM)
		{
		    continue;
		}

		if (0 == mode)
		{
		    mode = prof->mode;
		}
		for (j = 0; j < MAX_MAC_BLACK_NUM ; j++)
		{
		    if (memcmp(&(prof->mac[j]), mac_null, MAC_LEN) == 0)
		    {
			continue;
		    }
		    strncat(mac_all, smac2str(&(prof->mac[j]), mac_str) ,sizeof(mac_str) - 1);
		}
		
	    }
	}
	if (mode != 0)
	{
	    chr = strrchr(mac_all, ';');
	    if (chr != NULL)
	    {
		*chr = '"';
	    }
	    else
	    {
		strcat(mac_all,"\"");
	    }
	}
	else
	{
	    mode = 2;
	    strcat(mac_all, "\"");
	}
	enabled_mac_filter(mode, i, mac_all); 
    }

//    printf("%s,cmd end\n", __func__);
    return 0;
}
#endif


