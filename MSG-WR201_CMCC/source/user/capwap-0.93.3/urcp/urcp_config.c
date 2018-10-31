/*
 *
 *
 *联动读取配置及下方配置功能函数
 *
 *ke.huatao
 *2010-09-01
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h> /*inet_addr*/
#include <time.h>
#include <config/autoconf.h>
#include <typedef.h>
#include <mibunion.h>
#include <mib.h>
#include <conver.h>
#include <profacce.h>
#include "common.h"

#ifdef URCP_WEB
#include <switch.h>
#include <sw_cfg.h>
#endif /*end URCP_WEB*/

#ifdef URCP_AP
#include "swinfo.h"
#endif
#include "urcp.h"
#include "app.h"
#include "app_msg.h"
#include "urcp_tftp.h"
#include "urcp_config.h"
#include "urcp_cmd.h"
#include "urcp_debug.h"
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include <usb-user.h>
#endif

#define TMP_FILE_NAME   16u

#define DEFAULT_AGE_TIME_STR  "300"
#define DEFAULT_RATE_UNIT_STR  "11161"
extern void dhcp_relay_init(void);
static void urcp_conf_cb(void *ptr, urcp_status_t urcp_state);
static void urcp_conf_temp_cb(void *ptr, urcp_status_t urcp_state);
static void urcp_image_cb(void *ptr, urcp_status_t urcp_state);
#ifdef URCP_AP
static void urcp_ap_auto_get_image_cb(void *ptr, urcp_status_t urcp_state);
#endif
static uint32 get_str(FILE*p, char *sl, const char *str);
static void creat_conf_file(FILE*srcFile, char *confFileName, uchar item[MAX_CONF_ITEM]);
static void complete_file_name(char *name, uint32 inUsb, char *fileName);

static uint32 serial_number = 0u;

#ifdef URCP_WEB
/*
 *读取文件请求
 *KeHuatao
 */
void urcp_msg_read_file(struct msgbuf_t *web_msg, uint32 file_id)
{
    uint32 i, misc;
    uint16 id1;
    urcp_batch_msg_t  urcp_get_conf_msg;
    tftp_opt_t conf_op;

    URCP_DEBUG(("%s\n", __func__));
    /*复制消息*/
    memcpy(&urcp_get_conf_msg, web_msg->sw_msg.buffer, sizeof(urcp_batch_msg_t));

    /*操作参数*/
    memset(&conf_op, 0, sizeof(tftp_opt_t));
    conf_op.op = TFTP_GET_FILE;
    if(file_id==FILE_ID_CONFIG)
    {
	/*配置文件*/
	conf_op.op_file_name = CONFIG_FILE;
	conf_op.cb_func = (cb_tftp)urcp_conf_cb;
	conf_op.rport = PORT_CONFIG;
	misc = MISC_GET_GENERAL;
	db_info(URCP_MODULE_ID_TFTP, "%s: to get profile\n", __func__);
    }
    else if (file_id == FILE_ID_IMAGE)
	{
	conf_op.op_file_name = IMAGE_FILE;
	conf_op.cb_func = (cb_tftp)urcp_get_image_cb;
	conf_op.rport = PORT_IMAGE;
	misc = MISC_GET_IMAGE;
	db_info(URCP_MODULE_ID_TFTP, "%s: to get image\n", __func__);
	}
	else
    {
	/*日志文件*/
	conf_op.op_file_name = LOG_FILE;
	conf_op.cb_func = (cb_tftp)urcp_get_log_cb;
	conf_op.rport = PORT_LOG;
	misc = MISC_GET_LOG;
    }
    /*依次发送多个包 */
    for(i=0u;i<urcp_get_conf_msg.number;i++)
    {
	/*读文件请求*/
        id1 = urcp_tftp_send_file_request(&(urcp_get_conf_msg.dst_mac[i*MAC_LEN]),(uchar*)(urcp_get_conf_msg.password[i]),&conf_op);

	/*在misc buffer中创建记录*/
	urcp_create_config_record(&(urcp_get_conf_msg.dst_mac[i*MAC_LEN]), urcp_get_conf_msg.user_ip, id1, misc);
    }

    return;
}


/*
 *判断sl是否为包含str的配置项, 是的话写入文件
 *
 *KeHuatao
 */
static uint32 get_str(FILE*p, char *sl, const char *str)
{
    int32 l;
    uint32 i=0u;

    /*是否相等*/
    l = strlen(str);
    if(strncmp(sl, str, l)==0) 
    {
	l = strlen(sl);
	/*写入文件*/
	fwrite(sl, 1, l, p);
	i = 1u;
    }
 
    return i;
}
#endif /*end URCP_WEB*/
/*
 *将item内容写入文件
 *KeHuatao
 */
static void write_conf_option(FILE *p, uchar item[MAX_CONF_ITEM], uint32 l)
{
    uint32 i, j=0u;

    /*所有项*/
    fprintf(p, "#");
    for(i=0u;i<l;i++)
    {
	if(item[i]!=0)
	{
	    if(j!=0u)
	    {
		fprintf(p, ",");
	    }
	    /*打印*/
	    fprintf(p, "%d", item[i]);
	    j++;
	}
    }
    /*换行*/
    fprintf(p, "#\n");

    return;
}
/*
 *将要发送的配置项写入文件
 *参数:
 *    srcFile       待发送的源文件指针
 *    urcpFileName  返回对srcFile进行过滤后的文件名,即要发送的真正文件
 *KeHuatao
 */
static void creat_conf_file(FILE*srcFile, char *urcpFileName, uchar item[MAX_CONF_ITEM])
{
    char name[128], tp[256];
    
    memset(tp, 0, sizeof(tp));
    memset(name, 0, sizeof(name));
   
    sprintf(name, "%s", urcpFileName);

    /*生成文件名*/
    create_tmp_file("tmp_", urcpFileName);
    
    /*copy the file to temp file*/
    sprintf(tp, "cp %s %s%s", name, TFTP_ROOT, urcpFileName);
    system(tp);

    URCP_DEBUG(("CMD:%s\n", tp));

    URCP_DEBUG(("%s: file:%s\n", __func__, urcpFileName));

    return;
}

/*
 *将要发送的配置项写入文件
 *参数:
 *    urcpFileName  创建一个临时文件,即要发送的真正文件
 *
 * added by Shiyu.Chen, 2012.07.17
 */
static void create_tftp_transmit_file(char *urcpFileName)
{
    char name[128], tp[256];
    
    memset(tp, 0, sizeof(tp));
    memset(name, 0, sizeof(name));
   
    sprintf(name, "%s", urcpFileName);

    /*生成文件名*/
    create_tmp_file("tmp_", urcpFileName);
    
    /*copy the file to temp file*/
    sprintf(tp, "cp %s %s%s", name, TFTP_ROOT, urcpFileName);
    DBCK("command:%s\n", tp);
    system(tp);


    URCP_DEBUG(("%s: file:%s\n", __func__, urcpFileName));

    return;
}

/*
 *将路径加入文件名
 *
 *KeHuatao
 */
static void complete_file_name(char *name, uint32 inUsb, char *fileName)
{
    char tp[64];

    if(strcmp(name, "")!=0)
    {
	if(inUsb==0u)
	{
	    /*内存中*/
	    strcat(fileName, RAM_CONFIG_PATH);
	}
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	else
	{
	    /*u盘中*/
            isUSBStorageExist(fileName);
	    strcat(fileName, USB_CONFIG_PATH);
	}
#endif
	/*文件名*/
	strcat(fileName, name);
    }
#ifdef URCP_WEB
    else
    {
	sprintf(tp, "ralink_init show 2860 > %snvram_file", TFTP_ROOT);
	/*本机配置文件*/
	system(tp);
	/*返回文件名*/
	sprintf(fileName, "%snvram_file", TFTP_ROOT);
	wait(NULL);
    }
#endif
    return;
}

#ifdef URCP_WEB
/*
 *发送配置文件请求
 *KeHuatao
 */
void urcp_msg_write_conf(struct msgbuf_t *web_msg)
{
    uint32 i;
    uint16 id1;
    char pName[128];
    FILE *srcFile;
    urcp_send_conf_msg_t urcp_write_conf_msg;
    tftp_opt_t conf_op;

    URCP_DEBUG(("%s\n", __func__));
    /*复制消息*/
    memcpy(&urcp_write_conf_msg, web_msg->sw_msg.buffer, sizeof(urcp_send_conf_msg_t));

    /*操作参数*/
    memset(&conf_op, 0, sizeof(tftp_opt_t));
    conf_op.op = TFTP_PUT_FILE;
    conf_op.cb_func = (cb_tftp)urcp_conf_cb;
    conf_op.rport = PORT_CONFIG;

    /*依次发送多个包 */
    URCP_DEBUG(("%s: number: %d, put file:%s\n", __func__, urcp_write_conf_msg.batch_info.number, urcp_write_conf_msg.src_file));
    memset(pName, 0, sizeof(pName));
    complete_file_name(urcp_write_conf_msg.src_file, urcp_write_conf_msg.inUSB, pName);
	DBCK("pName = %s", pName)
    srcFile = fopen(pName, "r");
    if(srcFile!=NULL)
    {
	/*创建待发送文件*/
	creat_conf_file(srcFile, pName, urcp_write_conf_msg.item_id);
	fclose(srcFile);
	conf_op.op_file_name = pName;
	for(i=0u;i<urcp_write_conf_msg.batch_info.number;i++)
	{
	    serial_number = urcp_write_conf_msg.serial_no;
	    
	    DBCK("serial_number = %d\n", serial_number);
	    /*写文件请求*/
	    id1 = urcp_tftp_send_file_request(&(urcp_write_conf_msg.batch_info.dst_mac[i*MAC_LEN]), (uchar*)(urcp_write_conf_msg.batch_info.password[i]), &conf_op);

	    /*在misc buffer中创建记录*/
	    urcp_create_config_record(&(urcp_write_conf_msg.batch_info.dst_mac[i*MAC_LEN]), urcp_write_conf_msg.batch_info.user_ip, id1, MISC_SET_GENERAL);
	}
    }
    
    return ;
}
/*
 *发送配置文件请求
 *
 * added by Shiyu.Chen, 2012.07.16
 */
void urcp_msg_write_file(struct msgbuf_t *web_msg, uint32 file_id)
{ 
    uint32 i;
    uint16 id1;
    char pName[128];
	char tmp[256];
    urcp_send_conf_msg_t urcp_write_conf_msg;
    tftp_opt_t conf_op;
    char tmp_path[256];

    URCP_DEBUG(("line %d\tfunction:%s()\n",__LINE__,  __func__));
    /*复制消息*/
    memcpy(&urcp_write_conf_msg, web_msg->sw_msg.buffer, sizeof(urcp_send_conf_msg_t));

    /*操作参数*/
    memset(&conf_op, 0, sizeof(tftp_opt_t));
    memset(pName, 0, sizeof(pName));
    memset(tmp, 0, sizeof(tmp));
    memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(tmp_path)) {
		if (file_id == FILE_ID_IMAGE) {
			strcat(tmp_path, USB_IMAGE_PATH);
		} else if(file_id == FILE_ID_CONFIG) {
#ifdef URCP_AC
			sprintf(tmp, "cp %s%s%s %s%s%d.xml", tmp_path, USB_CONFIG_PATH, urcp_write_conf_msg.src_file, tmp_path, USB_AP_CONFIG_ROOT, urcp_write_conf_msg.serial_no);
			system(tmp);
#endif
			strcat(tmp_path, USB_CONFIG_PATH);
		} else {
			DBCK();
		}
    } else
#endif
	{
#if (UTT_WX_2000 == FYES)
		if (file_id == FILE_ID_IMAGE) {
			strcat(tmp_path, "/ff/");
			strcat(tmp_path, USB_IMAGE_PATH);
		} else if(file_id == FILE_ID_CONFIG) {
			strcat(tmp_path, "/ff/");
			strcat(tmp_path, USB_CONFIG_PATH);
		} else {
			DBCK();
		}
		sprintf(tmp, "cp %s%s %s%d.xml", "/ff/"USB_CONFIG_PATH, urcp_write_conf_msg.src_file, "/ff"USB_AP_CONFIG_ROOT, urcp_write_conf_msg.serial_no);
		system(tmp);
#else
		if (file_id == FILE_ID_IMAGE) {
			strcat(tmp_path, TFTP_IMAGE_ROOT);
		} else if(file_id == FILE_ID_CONFIG) {
#ifdef URCP_AC
			sprintf(tmp, "cp %s%s %s%d.xml", RAM_CONFIG_PATH, urcp_write_conf_msg.src_file, TFTP_AP_CONFIG_ROOT, urcp_write_conf_msg.serial_no);
			system(tmp);
#endif
			strcat(tmp_path, RAM_CONFIG_PATH);
		} else {
			DBCK();
		}
#endif
    }
    conf_op.op = TFTP_PUT_FILE;
    if(file_id == FILE_ID_IMAGE)
    {
	conf_op.cb_func = (cb_tftp)urcp_image_cb;
	conf_op.rport = PORT_IMAGE;
	sprintf(pName, "%s%s", tmp_path, urcp_write_conf_msg.src_file);
    }
    else if(file_id == FILE_ID_CONFIG_TEMPLATE)
    {
	conf_op.cb_func = (cb_tftp)urcp_conf_temp_cb;
	conf_op.rport = PORT_CONFIG_TEMPLATE;
	sprintf(pName, "%s%s", TFTP_CONF_TEMP_ROOT, urcp_write_conf_msg.src_file);
    }
    else if(file_id == FILE_ID_CONFIG)
    {
	conf_op.cb_func = (cb_tftp)urcp_conf_cb;
	conf_op.rport = PORT_CONFIG;
	sprintf(pName, "%s%s", tmp_path, urcp_write_conf_msg.src_file);
    }

	DBCK("tmp_path = %s\n", tmp_path);
	DBCK("pName = %s\n", pName);
    URCP_DEBUG(("%s: number: %d, put file:%s\n", __func__, urcp_write_conf_msg.batch_info.number, urcp_write_conf_msg.src_file));
    
    /*创建待发送文件*/
    create_tftp_transmit_file(pName);
    conf_op.op_file_name = pName;
    
    /*依次发送多个包 */
    for(i=0u;i<urcp_write_conf_msg.batch_info.number;i++)
    {
	/*写文件请求*/
	id1 = urcp_tftp_send_file_request(&(urcp_write_conf_msg.batch_info.dst_mac[i*MAC_LEN]), (uchar*)(urcp_write_conf_msg.batch_info.password[i]), &conf_op);

	/*在misc buffer中创建记录*/
	urcp_create_config_record(&(urcp_write_conf_msg.batch_info.dst_mac[i*MAC_LEN]), urcp_write_conf_msg.batch_info.user_ip, id1, MISC_SET_GENERAL);
    }

    return;
}
/*
*发送完读写配置文件请求后的回调函数
*KeHuatao
*
*/
static void urcp_conf_cb(void *ptr, urcp_status_t urcp_state)
{
#if 0
    urcp_copy_put_file(ptr, urcp_state, serial_number);
#else
    urcp_file_cb(ptr, urcp_state, TFTP_CONFIG_ROOT);
#endif
    serial_number = 0u;

    return;
}


/*
 * 发送完读写config templete文件请求后的回调函数
 *
 * added by Shiyu.Chen, 2012.07.17
 */
static void urcp_conf_temp_cb(void *ptr, urcp_status_t urcp_state)
{
    urcp_file_cb(ptr, urcp_state, TFTP_CONF_TEMP_ROOT);
}

/*
 * 发送完读写image文件请求后的回调函数
 *
 * added by Shiyu.Chen, 2012.07.17
 */
static void urcp_image_cb(void *ptr, urcp_status_t urcp_state)
{
    urcp_file_cb(ptr, urcp_state, TFTP_IMAGE_ROOT);
}
#endif /*end URCP_WEB*/

/*
 *获取配置文件大小
 *
 *KeHuatao
 */
int32 get_conf_file_size(char *fname)
{
    uchar tp[128];
    int32 l=0;

    URCP_DEBUG(("%s: %s\n", __func__, fname));
    memset(tp, 0, sizeof(tp));
    /*补全路径*/              
    strcpy(tp, TFTP_ROOT);
    /*复制文件名*/
    strcat(tp, fname);

    l = getFileSize(tp, NULL);

    return l;
}

#if (URCP_DEVICE != URCP_ROUTER)
/*
 *如果某配置不为空，则写为空
 *
 *KeHuatao
 */
static void check_null(char *data)
{
    char zeroStr[2]="", *nv;

    /*读配置*/
    nv = nvram_bufget(RT2860_NVRAM, data);
    /*是否为空*/
    if(strcmp(nv, "")!=0)
    {
	nvram_bufset(RT2860_NVRAM, data, zeroStr);
	URCP_DEBUG(("%s: %s\n", __func__, data));
    }

    return;
}
/*
 *将系统设置恢复默认值
 *KeHuatao
 */
static void restore_sys_conf(void)
{
    /*dhcp 中继*/
    nvram_bufset(RT2860_NVRAM, "DhcpRelay", "0");
    nvram_bufset(RT2860_NVRAM, "DHCP_CLIENT", "0");
    nvram_bufset(RT2860_NVRAM, "AGE_TIME", DEFAULT_AGE_TIME_STR);

    return;
}
/*
 *将vlan配置恢复默认值
 *
 *KeHuatao
 */
static void restore_vlan_conf(void)
{
    uint32 i;
    char tp[64];

    /*清空pvlan*/
    for(i=0u;i<NO_OF_PVLANS;i++)/*遍历所有pvlan*/
    {
	if(cfg_read_pvlan_member(i)!=0)
	{
	    cfg_write_pvlan_member(i, 0x0u);
	    set_pvlan(i);/*保存*/
	    sprintf(tp, "PVLANName%d", port2ext(i));
	    check_null(tp);
	}
    }
    add_default_pvlan();
    /*清除tag vlan*/
    reset_qvlan(1);
    for(i=1u; i <= QVLAN_TABLE_ROW; i++)
    {
	sprintf(tp, "VB%d", i);
        check_null(tp);
	sprintf(tp, "PMISC%d", i);
        check_null(tp);
	sprintf(tp, "PVID%d", i);
        check_null(tp);
    }

    return;
}

/*
 *将汇聚设置恢复默认值
 *
 *KeHuatao
 */
static void restore_aggr_conf(void)
{
    int32 index;
    uint16 ugroup = 0;

    for (index = 1; index <= MAX_AGGR_GROUP_INDEX; index++)
    {
	/*组号*/
	ugroup = cfg_read_aggr_group(index);
	if ((ugroup >= MAX_AGGR_GROUP_SIGLE_START) &&
		(ugroup <= MAX_AGGR_GROUP_NUM))
	{
	    URCP_DEBUG(("%s: group %d\n", __func__, ugroup));
	    delete_aggr_group(ugroup);
	}
    }

    return;
}
/*
 *将汇聚设置恢复默认值
 *
 *KeHuatao
 */
static void restore_port_conf(void)
{
    char tmp[16], zeroStr[2]="";
    uint32 m_port_no;

    for (m_port_no = VTSS_PORT_NO_START; m_port_no < VTSS_PORT_NO_END; m_port_no++)
    {
	sprintf(tmp, "PCONF%d", m_port_no);
	nvram_bufset(RT2860_NVRAM, tmp, zeroStr);

	sprintf(tmp, "PORT_REMARK%d", m_port_no);
	nvram_bufset(RT2860_NVRAM, tmp, zeroStr);

	sprintf(tmp, "MaxLen%d", m_port_no);
	nvram_bufset(RT2860_NVRAM, tmp, zeroStr);
    }

    return;
}
/*
 *将速率限制设置恢复默认值
 *
 *KeHuatao
 */
static void restore_ratelimit_conf(void)
{
    uint32 myport = 0u, p, change;
    vtss_port_qos_setup_t myqos;

    URCP_DEBUG(("%s:\n", __func__));
    nvram_bufset(RT2860_NVRAM, "RATEUNIT", DEFAULT_RATE_UNIT_STR);
    for (myport = PORT_NO_START; myport < PORT_NO_END; myport++)
    {
	change=0u;
	p = port2int(myport);
	if(cfg_read_port_qos_policer(p)!=(uint32)(-1))
	{
	    cfg_write_port_qos_policer(p, (uint32)(-1));
	    change=1u;
	}

	if(cfg_read_port_qos_shaper(p)!=(uint32)(-1))
	{
	    cfg_write_port_qos_shaper(p, (uint32)(-1));
	    change=1u;
	}
	if(change==1u)
	{
	    URCP_DEBUG(("%s: port:%d\n", __func__, p));
	    memset(&myqos, 0, sizeof(vtss_port_qos_setup_t));
	    vtss_port_qos_get(myport, &myqos);

	    myqos.policer_port = (uint32)(-1);
	    myqos.shaper_port = (uint32)(-1);

	    vtss_port_qos_set(myport, &myqos);
	    qos_nvram_bufset_by_port(p);
	}
    }

    return;
}
/*
 *将arp配置恢复默认设置
 *KeHuatao
 *
 */
static void restore_arp_conf(void)
{
    web_delete_arp_all();
    if(cfg_read_sys_arp()==0)
    {
	cfg_write_sys_arp(1);
	set_anti_arp_cheat(1);
	nvram_bufset(RT2860_NVRAM, "ARPENABLE", "1");
    }
    return;
}
static void gen_qos_str(char *tp, uint32 myport)
{
    uint32 my_tag_prio[8];

    /*得到tag优先级*/
    memcpy(my_tag_prio, cfg_read_port_qos_tagPrio(myport), sizeof(my_tag_prio)); 
    /*将配置专程字符串*/
    sprintf(tp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	    cfg_read_port_qos_dscpEnable(myport),
	    cfg_read_port_qos_tagEnable(myport),
	    my_tag_prio[0],
	    my_tag_prio[1],
	    my_tag_prio[2],
	    my_tag_prio[3],
	    my_tag_prio[4],
	    my_tag_prio[5],
	    my_tag_prio[6],
	    my_tag_prio[7],
	    cfg_read_port_qos_defaultPrio(myport),
	    cfg_read_port_qos_usrPrio(myport));

    return;
}
#endif

/*
 *将一行配置写入flash
 *
 *KeHuatao
 */
static void read_conf_line(char *tp, uint32 option)
{
#ifdef URCP_WEB
    char nvstr[64], *tmp, *ptr=nvstr;
    //uint32 p, need_set=1u;

    /*行尾为回车符*/
    strtok(tp, "\n");
    /*分析配置*/
    tmp = strtok_r(tp, "=", &ptr);
    if((tmp!=NULL)
	 && (ptr!=NULL)/**/
      )
    {
#if 0
	if(option==0u)/*部分覆盖*/
	{
	    if((strstr(tmp, "PQOS")==tmp) && (strlen(ptr)>24))
	    {
		/*端口号*/
		sscanf(tmp, "PQOS%d", &p);
		/*qos配置中分离出限速配置*/
		memset(nvstr, 0, sizeof(nvstr));
		gen_qos_str(nvstr, p);
		strcat(nvstr, &ptr[24]);
		URCP_DEBUG(("%s: qos str: %s\n", __func__, nvstr));
		ptr = nvstr;
	    }
	}
	else if(option==1u)/*全部覆盖*/
	{
	    URCP_DEBUG(("%s: %s=%s\n", __func__, tmp, ptr));
	    if((strstr(tmp, "_IP_ADDR")!=NULL) || (strstr(tmp, "_MAC_ADDR")!=NULL)
		   || (strstr(tmp, "_IP_MASK")!=NULL) )
	    {
		/*ip和mac不能覆盖*/
                need_set=0u;
		URCP_DEBUG(("%s: not set %s=%s\n", __func__, tmp, ptr));
	    }
	}
	if(need_set==1u)
	{
	    /*写flash*/
	    nvram_bufset(RT2860_NVRAM, tmp, ptr);
	}
#endif
    }
#if DEBUG_URCP
    else
    {
	URCP_DEBUG(("%s: invalid line: %s=%s\n", __func__, tmp, ptr));
    }
#endif
#endif /*URCP_WEB*/

    return;
}

#if (URCP_DEVICE != URCP_ROUTER)
/*
 *分析收到的配置文件的第一行
 *
 *KeHuatao
 */
static int32 parse_config_item(uchar tp[1], uint32 l, uint32 item[1])
{
    uint32  c, i=0u;
    int32 ret=0;
    char *tmp=NULL;

    URCP_DEBUG(("%s: tp: %s\n", __func__, tp));
    if(tp[0]=='#')
    {
	if(tp[l-1]=='\n')
	{
	    l--;
	}
	/*为发送部分配置项*/
	if((l>=2u) && tp[l-1]=='#')
	{
	    if(l>2)
	    {
		tp[l-1]='\0';
		tmp = strtok(&tp[1], ",");
		while(tmp!=NULL)
		{
		    c = (uint32)strtoul(tmp, NULL, 10);
		    /*记录配置项*/
		    if(c!=0)
		    {
			item[i]=c;
			i++;
		    }
		    /*分析*/
		    switch(c)
		    {
			case SYS_CONF:
			    restore_sys_conf();
			    URCP_DEBUG(("%s: clear sys conf\n", __func__));
			    break;
			case VLAN_CONF:
			    restore_vlan_conf();
			    URCP_DEBUG(("%s: clear vlan conf\n", __func__));
			    break;
			case AGGR_CONF:
			    restore_aggr_conf();
			    URCP_DEBUG(("%s: clear aggr conf\n", __func__));
			    break;
			case PORT_CONF:
			    restore_port_conf();
			    URCP_DEBUG(("%s: clear port conf\n", __func__));
			    break;
			case RATELIMIT_CONF:
			    restore_ratelimit_conf();
			    URCP_DEBUG(("%s: clear rate conf\n", __func__));
			    break;
			case ARP_CONF:
			    restore_arp_conf();
			    URCP_DEBUG(("%s: clear arp conf\n", __func__));
			    break;
			default:/*未知*/
			    break;
		    }
		    tmp = strtok(NULL, ",");
		}
	    }
	    else
	    {
		/*文件全部覆盖*/
		ret=1;
	    }
	}
	else
	{
	    ret=-1;
	}
    }
    else
    {
	ret=-1;
    }

    return ret;
}
static void activate_conf(uint32 item[1], uint32 l)
{
    uint32 i=0u;

    while((i<l) && (item[i]>0u))
    {
	switch(item[i])
	{
	    case SYS_CONF:
                nvram_agetime_init();
		dhcp_relay_init();
                nvram_dhcpclient_init();			
		URCP_DEBUG(("%s: clear sys conf\n", __func__));
		break;
	    case VLAN_CONF:
		nvram_vlan_init();
		nvram_cpuvid_init();
		URCP_DEBUG(("%s: init vlan conf\n", __func__));
		break;
	    case AGGR_CONF:
		init_aggr_mode_set();     
		init_aggr_group();
		URCP_DEBUG(("%s: clear aggr conf\n", __func__));
		break;
	    case PORT_CONF:
		nvram_port_init();
		URCP_DEBUG(("%s: clear port conf\n", __func__));
		break;
	    case RATELIMIT_CONF:
		init_qos_rate_unit();
		init_port_qos();
		URCP_DEBUG(("%s: clear rate conf\n", __func__));
		break;
	    case ARP_CONF:
		arp_init();
		URCP_DEBUG(("%s: clear arp conf\n", __func__));
		break;
	    default:/*未知*/
		break;
	}
	i++;
    }
}
/*
 *将ip、mac等非唯一性的配置恢复默认值
 *KeHuataqo
 */
static void clear_nvram_config(void)
{
    char *tmp, wan_ip[24], wan_mac[24], vtss_ip[24], vtss_mac[24];

    /*wan口ip*/
    tmp = nvram_bufget(RT2860_NVRAM, "WAN_IP_ADDR");
    strcpy(wan_ip, tmp);
    /*wan口mac*/
    tmp = nvram_bufget(RT2860_NVRAM, "WAN_MAC_ADDR");
    strcpy(wan_mac, tmp);

    /*交换口*/
    tmp = nvram_bufget(RT2860_NVRAM, "VTSS_IP_ADDR");
    strcpy(vtss_ip, tmp);
    /*交换口*/
    tmp = nvram_bufget(RT2860_NVRAM, "VTSS_MAC_ADDR");
    strcpy(vtss_mac, tmp);

    /*恢复出厂配置*/
#if 1
    system("ralink_init clear 2860");
#else
    system("ralink_init clear 2860; ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
#endif
    nvram_close(RT2860_NVRAM);
    /*回写flash*/
    nvram_bufset(RT2860_NVRAM, "WAN_IP_ADDR", wan_ip);
    nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR", wan_mac);
    nvram_bufset(RT2860_NVRAM, "VTSS_IP_ADDR", vtss_ip);
    nvram_bufset(RT2860_NVRAM, "VTSS_MAC_ADDR", vtss_mac);

    return;
}
#endif

#ifdef URCP_AP
/*
 *分析收到的配置文件
 *KeHuatao
 *
 */
static void parse_config_file(char * file)
{
#if 0
	DBCK();
    char tp[1024];
    char buf[CONFIG_BUFF_SIZE];
    char str[256], tmp[128], *p = NULL, p1 = NULL;
    FILE *fp;
	MibProfileType mibTypeApEffect = MIB_PROF_AP_EFFECT_CONF;
	MibProfileType mibTypeFatFitSw = MIB_PROF_FAT_FIT_SWITCH;
	ApEffectConfProfile profApEffect;
	ApEffectConfProfile *profAE;
	FatFitSwitchProfile profFatFitSw;
	FatFitSwitchProfile *profFFS;
	SlotNumber slot;
    WirelessProfile *prof = NULL;
#if (CAPWAP_CONFIG_AP == FYES)
    ApCapwapConfigProfile *profac = NULL, profac_old;
    InterfaceProfile *profIf = NULL, profIf_old;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES) 
    ApBasicConfProfile *profab = NULL;
    MibProfileType mibTypeab = MIB_PROF_AP_BASIC_CONF;
#endif

    /*文件名*/
    memset(tp, 0, MAX_FILE_NAME_LEN);
    memset(buf, 0, CONFIG_BUFF_SIZE);
    strcat(tp, TFTP_ROOT);
    strcat(tp, file);

	DBCK("parse_file = %s", tp);
	slot.slotNumber = SLOT_NUMBER_ANY;
	memset(&profApEffect, 0, sizeof(profApEffect));
	memset(&profFatFitSw, 0, sizeof(profFatFitSw));
	BuildDefaultFactoryValue(mibTypeApEffect, 0, slot, &profApEffect);
	BuildDefaultFactoryValue(mibTypeFatFitSw, 0, slot, &profFatFitSw);
	getProfileFromXml(tp, mibTypeApEffect, &profApEffect);
	getProfileFromXml(tp, mibTypeFatFitSw, &profFatFitSw);

	DBCK();
	DBCK("ApEffect.config = %d, FatFitSw,fatEnable = %d", profApEffect.config, profFatFitSw.fatEnable);
	DBCK();
    
    /*打开*/
    fp = fopen(tp, "r");
    if(fp!=NULL)
    {
	memset(tmp, 0, sizeof(tmp));
	/*读第一行*/
	memset(str, 0, sizeof(str));
	fgets(str, sizeof(str), fp);

	/*读一行*/
	memset(str, 0, sizeof(str));
	fgets(str, sizeof(str), fp);
	get_sysinfo_machine_model(tmp);
	sprintf(tp, "<apTypes>%s</apTypes>", tmp);
	p = strstr(str,tp);

#if 0
	get_sysinfo_hw_model(tmp);
	sprintf(tp, "<firmVersion>%s</firmVersion>", temp);
	p1 = strstr(str,tp);
#endif

	if((p != NULL) /*&& (p1 != NULL)*/)
	{
	    /*AP自己的配置文件*/
		/* 判断ac下发的配置中是胖AP还是瘦AP */
		if (profFatFitSw.fatEnable == 1) {
			/* 胖AP的情况 */
			DBCK();
			profFFS = (FatFitSwitchProfile *)ProfGetInstPointByIndex(mibTypeFatFitSw, 0);
			profFFS->fatEnable = 1;
			nvramWriteCommit();
			UttSendMsgForSpeed(MSG_PROF_REINIT, 0, NULL, 0);
		} else if ((profFatFitSw.fatEnable == 0) && (profApEffect.config == 1)){
			/* 如果ap接收到的配置文件是本地生效并且是瘦AP */
			DBCK();
			profAE = (ApEffectConfProfile *)ProfGetInstPointByIndex(mibTypeApEffect, 0);
			profAE->config = PROF_ENABLE;
			nvramWriteCommit();
			UttSendMsgForSpeed(MSG_PROF_REINIT, 0, NULL, 0);
		} else {
		/* 下发生效或者是瘦AP */
			DBCK();
		    memset(tp, 0, MAX_FILE_NAME_LEN);
		    fseek(fp, 0, SEEK_SET);
		    /*读后面的配置*/
		    while(fgets(tp, sizeof(tp), fp)!=NULL)
		    {
			strcat(buf,tp);
		    }

			DBCK("buf = %s", buf);
#if (CAPWAP_CONFIG_AP == FYES)
		    profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_CAPWAP_CONFIG, 0);
		    if (profac != NULL)
		    {
			/*保留原来的AC的CAPWAP配置*/
			memcpy(&profac_old, profac, sizeof(ApCapwapConfigProfile));
		    }
#if 0
		    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
		    if (profIf != NULL)
		    {
			memcpy(&profIf_old, profIf, sizeof(InterfaceProfile));
		    }
#endif
#endif
		    /* 全部覆盖 */
		    if(parseProfileFromStr(buf, 1) != 0)
		    {
			DBCK();
			URCP_DEBUG(("warning: the config file error !!!\n"));
		    }
		    else
		    {
			DBCK();
			//ProfWiteToFlash(); /* write to flash*/
			prof = ProfGetInstPointByIndex(MIB_PROF_WIRELESS ,0);
extern ap_info_t ap_infor;
            if (prof != NULL){
            prof->mBaseCf.BYPASS = ap_infor.ac_infor.acMode;
            }
#if (CAPWAP_CONFIG_AP == FYES)
	    profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_CAPWAP_CONFIG, 0);
	    if (profac != NULL)
	    {
		//			    printf("%s, use CAPWAP ap loacal\n", __func__);
		memcpy(profac, &profac_old,sizeof(ApCapwapConfigProfile));
	    }
#if 0
	    profab = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibTypeab,0);
	    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
	    if (profIf != NULL)
	    {
		/*收到配置后新的*/
		if ((profIf->ether.connMode == ETH_CONN_DHCP) && (profIf_old.ether.connMode == ETH_CONN_STATIC) &&(profIf_old.ether.sta.ip != htonl(0)) \
			&& ((profac->acHostInfo[0][0] != '\0') || (profac->acHostInfo[1][0] != '\0') || (profac->acHostInfo[2][0] != '\0')) )
		{
		    memcpy(profIf, &profIf_old, sizeof(InterfaceProfile));	
#if (FEATURE_AP_BASIC_INFO == FYES) 
		    profab->dhcp_en = 0;
#endif
		}
	    }
#endif
#endif
			UttSendMsgForSpeed(MSG_PROF_REINIT, 0, NULL, 0);
		    }
		}
	}
	else
	{
	    /*自动下发的服务区或者模板等默认配置*/
		memset(tp, 0, MAX_FILE_NAME_LEN);
		fseek(fp, 0, SEEK_SET);
		/*读后面的配置*/
		while(fgets(tp, sizeof(tp), fp)!=NULL)
		{
			strcat(buf,tp);
		}
		DBCK("buf = %s", buf);
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
/*将自动下发的服务区中叫wireless1,在5GAP中他用来保存5G的信息，需调整*/
	p = strstr(buf, "name=\"wireless1\">");
	if(p!=NULL)
	{
	    p += strlen("name=\"wireless");
	    *p = '0' + FIT_AP_WL_24G_PF_START;
	}
#endif
#if (CAPWAP_CONFIG_AP == FYES)
		profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
		if (profIf != NULL)
		{
		    memcpy(&profIf_old, profIf, sizeof(InterfaceProfile));
		}
#endif
		/* 全部覆盖 */ 
		if(parseProfileFromStr(buf, 0) != 0)
		{
			URCP_DEBUG(("warning: the config file error !!!\n"));
		}
		else
		{
			DBCK();
			//ProfWiteToFlash(); /* write to flash*/
#if (CAPWAP_CONFIG_AP == FYES)
			profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 1);
			profab = (ApBasicConfProfile*)ProfGetInstPointByIndex(mibTypeab,0);
			profac = (ApCapwapConfigProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_CAPWAP_CONFIG, 0);
			if ((profIf != NULL) && (profac != NULL))
			{
			    /*收到配置后新的*/
			    if ((profIf->ether.connMode == ETH_CONN_DHCP) && (profIf_old.ether.connMode == ETH_CONN_STATIC) &&(profIf_old.ether.sta.ip != htonl(0)) \
				    && ((profac->acHostInfo[0][0] != '\0') || (profac->acHostInfo[1][0] != '\0') || (profac->acHostInfo[2][0] != '\0')) )
			    {
				memcpy(profIf, &profIf_old, sizeof(InterfaceProfile));	
#if (FEATURE_AP_BASIC_INFO == FYES)
				if (profab != NULL)
				{
				    profab->dhcp_en = 0;
				}
#endif
			    }
			}
#endif
			UttSendMsgForSpeed(MSG_PROF_REINIT, 0, NULL, 0);
		}
	}
	}

    fclose(fp);

    memset(tp, 0, sizeof(tp));
	sprintf(tp, "rm %s%s", TFTP_ROOT, file);
	DBCK("%s", tp);
	system(tp);
#endif
    return;
}

static void parse_image_file(char * file)
{
    //printf("\n< %s %d >\n", __func__, __LINE__);
    char tp[MAX_FILE_NAME_LEN], fbuf[128];
    char *buf = NULL;
    FILE *fp;
    char sw_ver[64], hw_ver[64], sw_model[64];
	unsigned int swVer, hwVer;
    image_header_t *head = NULL;
    uint32 length = 0u, update = 1u;
    int32 ret = 0, reboot = 0;

    /*文件名*/
    memset(tp, 0, MAX_FILE_NAME_LEN);
    strcat(tp, TFTP_ROOT);
    strcat(tp, file);

    URCP_DEBUG(("line:%d\tfunction:%s()\n", __LINE__, __func__));
    /*打开*/
    fp = fopen(tp, "rb");
    if(fp!=NULL)
    {
	/*read image head*/
	memset(fbuf, 0, sizeof(fbuf));
	if(fread(fbuf, sizeof(fbuf), 1, fp) != 1)
	{
	    URCP_DEBUG(("Failed: read file %s", tp));
		DBCK();
	}
	else
	{
	    head = (image_header_t*)fbuf;
	    
#ifdef UBOOT_HEADER
		length = ntohl(head->ih_size) + 64u;
#else
	    /*image length*/
	    length = ntohl(head->imageLen)+8u;
#endif
	    fseek(fp, 0, SEEK_SET);
	    /*malloc memory*/
	    buf = (char *)malloc(length);
	    if(buf == NULL)
	    {
		URCP_DEBUG(("\nMalloc failed !!!\n"));
	    }
	    else
	    {
		/*read whole image file to buf*/
		fread(buf, length, 1, fp);
		fclose(fp);/*close file*/

		/*get system information.*/
		memset(sw_model, 0, sizeof(sw_model));
		get_sysinfo_machine_model(sw_model);

		get_verinfo_from_local(&hwVer, &swVer);
		memset(hw_ver, 0, sizeof(hw_ver));
		memset(sw_ver, 0, sizeof(sw_ver));
		hwverinfo_to_str(hwVer, hw_ver);
		swverinfo_to_str(swVer, sw_ver);

		/*check softeware model*/
		if(strcmp(sw_model, head->name) != 0)
		{
		    URCP_DEBUG(("Failed: Software model wrong !!!\n"));
			DBCK();
		    update = 0u;
		}

		/*software CRC check*/
#ifdef UBOOT_HEADER
		if (crc32(0, buf + 8, length - 8) != ntohl(head->ih_dcrc))
#else
		if (crc32 (0, buf+8, length - 8) != ntohl(head->imageCrc))
#endif
		{
		    URCP_DEBUG(("Failed: 软件校验和错误 !!!\n"));
			DBCK();
		    update = 0u;
		}
#if 1
		/*check hardware version*/
		int tmp = 0, major, minor;
		char hwVer[24]={0};
		tmp = ((int)ntohl(head->systemVer) & 0x00FF);
		major = (tmp>>HWVER_2);
		minor = (tmp & 0xF);
		sprintf(hwVer,"V%d.%d",major,minor);
		DBCK("hwVer = %s, hw_ver = %s\n", hwVer, hw_ver);
		URCP_DEBUG(("\n< %s %d > hw_ver:%s head->hw_version:%s\n", __func__, __LINE__, hw_ver, hwVer));
		//printf("\n< %s %d > hw_ver:%s head->hw_version:%s\n", __func__, __LINE__,  hw_ver, hwVer);
		if(strcasecmp(hw_ver, hwVer) != 0)
		{
		    URCP_DEBUG(("Failed: Hardware version wrong !!!\n"));
			DBCK();
		    update = 0u;
		}
#endif

		if(update == 1u)
		{
		    /*update software*/
		    ret = flash_write_mtdname("Kernel", buf, 0, length);
		    if(ret == -1)
		    {
			URCP_DEBUG(("Failed: firmware update !!!\n"));
		    }
		    else
		    {
			URCP_DEBUG(("firmware update success !!! \n\n"));
			reboot = 1;
		    }
		}
	    }
	    free(buf); /*free memery*/
	}
	fclose(fp); /*close file*/
    }

    /*reboot system*/
    if(reboot == 1)
    {
	system("reboot&");
    }

    //printf("\n< %s %d >\n", __func__, __LINE__);
    return;
}
#endif
/*
 *处理完收到的"写请求"后回调函数
 *
 *KeHuatao
 *
 */
void urcp_rev_conf_cb(void *ptr, urcp_status_t urcp_state)
{
	DBCK();
    char tp[256], tmp_path[128];
    int32 del=0;
    urcp_tftp_session_info_t *sess;
#if (UTT_NV_D908W == FYES)
    int32 save = 0;
#endif

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

    sess = (urcp_tftp_session_info_t*)ptr;
    URCP_DEBUG(("%s:%d, file:%s\n", __func__, urcp_state, sess->filename));
    db_info(URCP_MODULE_ID_TFTP, "%s: file %s,state %d, %s\n", __func__, sess->filename, urcp_state,  str_macaddr(sess->mac));
    if((sess->sess_type==TFTP_REV_FILE))
    {
	switch(sess->tftp_port)
	{
	    case PORT_CONFIG:
		if(urcp_state==URCP_STATUS_OK)
		{
#ifdef URCP_AP
		    /*收到下发的配置文件*/
			DBCK("sess->filename = %s", sess->filename);
		    parse_config_file(sess->filename);
#endif
#ifdef URCP_AC
#if (UTT_NV_D908W == FYES)
		    /*
		     * 保存所有AP配置到FALSH中
		     * D908W 不支持usb存储器
		     * */
		    save = 1;
#endif
		    memset(tp, 0, sizeof(tp));
//		    sprintf(tp, "cp %s%s %s%s.xml -f && rm %s%s", TFTP_ROOT, sess->filename, tmp_path, sess->filename, TFTP_ROOT, sess->filename);
		    sprintf(tp, "cp %s%s %s%u.xml -f && rm %s%s", TFTP_ROOT, sess->filename, tmp_path, strtoul(sess->filename, NULL, 10), TFTP_ROOT, sess->filename);
		    system(tp);
#endif
                }
		del=1;
		break;
#ifdef URCP_AP
	    case PORT_IMAGE:
		if(urcp_state==URCP_STATUS_OK)
		{
		    /*收到下发的software*/
		    parse_image_file(sess->filename);
                }
		del=1;
		break;
#endif
	    default:/*其它可不做处理*/
		break;
	}
	URCP_DEBUG(("%s: rev file:%s, to rport:%d\n", __func__, sess->filename, sess->tftp_port));
    }
    else
    {
	/*发送文件*/
	if((sess->tftp_port==PORT_LOG) || (sess->tftp_port == PORT_IMAGE))
	{
	    /*发送日志*/
		DBCK();
	    del=1;
	}
    }

    if(del==1)
    {
	/*删除文件*/
	memset(tp, 0, sizeof(tp));
	strcat(tp, TFTP_ROOT);
	if(sess->tftp_port == PORT_IMAGE)
	{
	    strcat(tp, IMAGE_FILE);
	}
	else
	{
	    strcat(tp, sess->filename);
	}
	unlink(tp);
	DBCK("%s", tp);
	URCP_DEBUG(("%s: unlink: %s\n", __func__, tp));
	db_info(URCP_MODULE_ID_TFTP, "%s: unlink: %s\n", __func__, sess->filename);
    }
#if (UTT_NV_D908W == FYES)
    /*保存所有AP配置到FALSH中*/
    if(save == 1) {
	apConfigWriteToFlash();
    }
#endif

    return;
}

/*
 *将当前配置写入文件
 *KeHuatao
 */
void create_nvram_file(void)
{
    uchar tp[128];
	char buf[CONFIG_BUFF_SIZE];

	memset(buf, 0, sizeof(buf));
	memset(tp, 0, sizeof(tp));

	if (-1 == parseProfileToStr(buf, NULL)) {
		URCP_DEBUG(("read config file error !!!\n"));
	}
	sprintf(tp, "echo '%s' > %s%s", buf, TFTP_ROOT, CONFIG_FILE);
	system(tp);
	wait(NULL);

#if 0
    FILE* p;

    /*文件名*/
    memset(tp, 0, sizeof(tp));
    strcat(tp, TFTP_ROOT);
    strcat(tp, CONFIG_FILE);
    /*打开*/
    p =fopen(tp, "w");
    if(p!=NULL)
    {
	/*文件头改在页面下载该文件时添加*/
	/*文件头*/
	fprintf(p, "#The following line must not be removed.\nDefault\n");
	fclose(p);
	/*生成当前配置文件并移到tftp目录下*/
	sprintf(tp, "echo \"#The following line must not be removed.\" >%s%s;echo \"Default\" >>%s%s; \
	ralink_init show 2860 >> %s%s ",TFTP_ROOT,CONFIG_FILE,TFTP_ROOT,CONFIG_FILE,TFTP_ROOT,CONFIG_FILE);
	system(tp);
	wait(NULL);
    }
#endif

    return;
}

#ifdef URCP_AP
extern int flash_read_image_to_path(char *file, int imageLen);
void create_image_file(void)
{
	char file[64];
	image_header_t info;
	uint32 imageLen;
	flash_read_mtdname("Kernel", (char *)(&info), 0, sizeof(image_header_t));
#ifdef UBOOT_HEADER
	imageLen = ntohl(info.ih_size) + 64;
#else
	imageLen = ntohl(info.imageLen);
	imageLen += 8;
#endif

	memset(file, '\0', sizeof(file));
	sprintf(file, "%s%s", TFTP_ROOT, IMAGE_FILE);
	flash_read_image_to_path(file, imageLen);

	return;
}

/*
 * Function: urcp_ap_auto_get_image_cb()
 *
 * Description:AP自动获取软件回调函数
 *
 * History: Shiyu.Chen, 2012.07.11
 */
static void urcp_ap_auto_get_image_cb(void *ptr, urcp_status_t urcp_state)
{
    FILE* fp;
    char sw_ver[64], hw_ver[64], sw_model[64];
	unsigned int swVer, hwVer;
    urcp_tftp_session_info_t *sess=NULL;
    image_header_t *head = NULL;
    uint32 length = 0u, update = 1u, sw_ver_int = 0;
    int ret = 0, reboot = 0;
    char name[256], fbuf[128];
    char *buf = NULL;
    ap_state_t newstate;

    URCP_DEBUG(("Function:%s\turcp_state:%d\n", __func__, urcp_state));

    newstate = AP_STATE_CONFUPDATE_REQ;
    sess = (urcp_tftp_session_info_t*)ptr;
    db_info(URCP_MODULE_ID_TFTP, "%s: %s state:%d\n", __func__, str_macaddr(sess->mac), urcp_state);
    if(urcp_state == URCP_STATUS_OK)
    {
	DBCK();
	memset(name, 0, sizeof(name));
	strcpy(name, TFTP_ROOT);
	strncat(name, sess->filename, MAX_FILE_NAME_LEN);
	fp = fopen(name, "rb");
	if(fp != NULL)
	{
	    /*read image head*/
	    memset(fbuf, 0, sizeof(fbuf));
	    if(fread(fbuf, sizeof(fbuf), 1, fp) == 0)
	    {
		URCP_DEBUG(("File read error !\n"));
	    }
	    else
	    {
		head = (image_header_t*)fbuf;
#ifdef UBOOT_HEADER
		length = ntohl(head->ih_size) + 64u;
#else
		length = ntohl(head->imageLen)+8u;
#endif
		fseek(fp, 0, SEEK_SET);
		buf = (char *)malloc(length);
		if(buf == NULL)
		{
		    URCP_DEBUG(("\nMalloc failed !!!\n"));
		    db_warn(URCP_MODULE_ID_TFTP, "\nMalloc failed !!!\n");
		}
		else
		{
		    /*read image file to buf*/
		    fread(buf, length, 1, fp);
		    fclose(fp);
		    
		    /*get system information*/
		    memset(sw_model, 0, sizeof(sw_model));
		    get_sysinfo_machine_model(sw_model);

			get_verinfo_from_local(&hwVer, &swVer);
			memset(hw_ver, 0, sizeof(hw_ver));
			memset(sw_ver, 0, sizeof(sw_ver));
			hwverinfo_to_str(hwVer, hw_ver);
			swverinfo_to_str(swVer, sw_ver);

		    /*check softeware model*/
		    if(strcasecmp(sw_model, head->name) != 0)
		    {
			URCP_DEBUG(("Failed: Software model wrong !!!\n"));
			update = 0u;
		    }
		    
		    /*CRC Check*/
#ifdef UBOOT_HEADER
			if (crc32(0, buf + 8, length - 8) != ntohl(head->ih_dcrc))
#else
		    if (crc32 (0, buf+8, length - 8) != ntohl(head->imageCrc))
#endif
			{
			URCP_DEBUG(("Failed: 软件校验和错误 !!!\n"));
		    db_warn(URCP_MODULE_ID_TFTP, "\ncrc error!!!\n");
			update = 0u;
		    }
#if 0
		    /*check hardware version*/
		    int tmp = 0, major, minor, minor2, major_local, minor_local, minor2_local;
			unsigned int tmp1 = 0;
			image_header_t head_local;
		    char hwVer[24]={0};
		    char swVer[24]={0};
		    char *devSwVer=NULL;
		    int verNum=0;

		    tmp = ((int)ntohs(head->systemVer) & 0x001F);
		    major = (tmp>>2);
		    minor = (tmp & 0x03);
		    sprintf(hwVer,"V%d.%d",major,minor);
			DBCK("hwVer = %s", hwVer);
		    if(strcmp(hw_ver, hwVer) != 0)
		    {
			DBCK("hw_ver wrong");
			URCP_DEBUG(("Failed: Hardware version wrong !!!\n"));
			update = 0u;
		    }

		    /* compare software version. 
		     * local software version lower than software version 
		     * from remote AC is OK.
		     */
		    tmp = ((int)ntohs(head->systemVer)>>5);
		    major = (tmp>>7);
		    minor = (tmp>>4) & 0x07;
		    minor2 = tmp & 0x0F;

			ret = flash_read_mtdname("Kernel", &head_local, 0, sizeof(head_local));
			if (ret!= -1 ) {
				tmp1 = (head_local.systemVer) >> 5;
				major_local = tmp1 >> 7;
				minor_local = (tmp1 >> 4) & 0x07;
				minor2_local = tmp1 & 0x0F;
			}
			DBCK("head->systemVer = %d, major = %d, minor = %d, minor2 = %d", head->systemVer, major, minor, minor2);
			DBCK("head_local.systemVer = %d, major_local = %d, minor_local = %d, minor2_local = %d", head_local.systemVer, major_local, minor_local, minor2_local);

		    sprintf(swVer,"V%d.%d.%d",major,minor,minor2);
		    devSwVer=strchr(sw_ver,'v');
		    devSwVer=strtok(devSwVer,".");
		    verNum=strtoll(devSwVer, NULL, 10);
		    //sw_ver_int = strtoll(sw_ver, NULL, 10);
		    //printf("\n< %s %d > verNum:%d tmp:%d\n", __func__, __LINE__, verNum, tmp);
			DBCK("swVer = %s, verNum = %d, major = %d", swVer, verNum, major);
		    if(verNum < major)
		    {
			DBCK("verNum < major");
			URCP_DEBUG(("Failed: Software version lower!!!\n"));
			update = 0u;
		    }
#else
			int hwVer = 0, hwVer_local = 0, swVer = 0, swVer_local = 0;
			get_verinfo_from_file(name, &hwVer, &swVer);
			get_verinfo_from_local(&hwVer_local, &swVer_local);

			DBCK("name = %s, hwVer = %d, hwVer_local = %d, swVer = %d, swVer_local = %d", name, hwVer, hwVer_local, swVer, swVer_local);
			if (hwVer != hwVer_local) {
				update = 0u;
				DBCK("Failed: the hardware version wrong");
			}

			if (swVer <= swVer_local) {
				update = 0u;
				DBCK("Failed: the software version wrong");
			}
#endif
		    if(update == 1u)
		    {
			DBCK("--------------------software update-------------------------");
			/*update software*/
			ret = flash_write_mtdname("Kernel", buf, 0, length);
			if(ret == -1)
			{
			    URCP_DEBUG(("Failed: firmware update !!!\n"));
			}
			else
			{
			    URCP_DEBUG(("firmware update success !!! \n\n"));
			    reboot = 1;
			}
                        newstate = AP_STATE_CONFUPDATE_REQ;
		    } else {
				unlink(name);
				//change_state(AP_STATE_CONFUPDATE_REQ);
			}
		    free(buf); /*free memery*/
		}
	    }

	    fclose(fp); /*close file*/
	}
    }
    else {
	ap_set_update_state(sess->local_id, URCP_STATUS_FAIL);
    }
    /*system reboot*/
    if(reboot == 1)
    {
	system("reboot&");
	db_info(URCP_MODULE_ID_TFTP, "wait for reboot\n");
	while(1); 
    }
    change_state(newstate);

    return;
}

/*
 * Function: urcp_ap_auto_get_config_cb()
 *
 * Description:AP自动获取config回调函数
 *
 * History: Shiyu.Chen, 2012.07.25
 */
static void urcp_ap_auto_get_config_cb(void *ptr, urcp_status_t urcp_state)
{
	extern ap_info_t ap_infor;

    urcp_tftp_session_info_t *sess=NULL;

    URCP_DEBUG(("Function:%s\turcp_state:%d\n", __func__, urcp_state));

    sess = (urcp_tftp_session_info_t*)ptr;
    if(urcp_state == URCP_STATUS_OK)
    {
        DBCK();
        change_state(AP_STATE_NORMAL);
	parse_config_file(sess->filename);
    }
    else {
	ap_set_update_state(sess->local_id, URCP_STATUS_FAIL);
    }

    return;
}

/*
 * Function: urcp_auto_get_file()
 *
 * Description:AP自动获取文件
 *
 * History: Shiyu.Chen, 2012.07.10
 */
extern uint16 urcp_auto_get_file(uchar *mac_addr, uint32 file_id)
{
	uint32 sn;
	char tmp_name[256];
    tftp_opt_t conf_op;
    uint16 ret;
    URCP_DEBUG(("Function: %s\n", __func__));

	sn = getSystemSn();
	memset(tmp_name, '\0', sizeof(tmp_name));
	sprintf(tmp_name, "%s_%u", CONFIG_FILE, sn);
	DBCK("tmp_name = %s", tmp_name);
    memset(&conf_op, 0, sizeof(tftp_opt_t));
    conf_op.op = TFTP_GET_FILE;

    /*自动获取软件相关设置*/
    if(file_id==FILE_ID_IMAGE)
    {
	conf_op.op_file_name = IMAGE_FILE;
	conf_op.cb_func = (cb_tftp)urcp_ap_auto_get_image_cb;
	conf_op.rport = PORT_IMAGE_AUTO;
	db_info(URCP_MODULE_ID_TFTP, "%s: auto get image\n", __func__);
    }
    else if(file_id==FILE_ID_CONFIG)
    {
	conf_op.op_file_name = tmp_name;
	conf_op.cb_func = (cb_tftp)urcp_ap_auto_get_config_cb;
	conf_op.rport = PORT_CONFIG_AUTO;
	db_info(URCP_MODULE_ID_TFTP, "%s: auto get profile\n", __func__);
    }
    ret = urcp_tftp_send_file_request(mac_addr , NULL, &conf_op);
    return ret;
}

/*
 * Function: urcp_conf_backup_cb()
 *
 * Description: call back
 *
 * History: Shiyu.Chen, 2012.07.31
 */
urcp_status_t apStatusForAcOperate;
void urcp_conf_backup_cb(void *ptr, urcp_status_t urcp_state)
{
    urcp_tftp_session_info_t *sess;
    char tp[256];

    /*delelt config file*/
    sess = (urcp_tftp_session_info_t*)ptr;
    memset(tp, 0, sizeof(tp));
    sprintf(tp, "rm %s%s -f", TFTP_ROOT, sess->filename);
    system(tp);
    URCP_DEBUG(("exec CMD: %s\n\n", tp));

	URCP_DEBUG(("debug: %d in %s()\n", __LINE__, __func__));
    URCP_DEBUG(("%s:%d, file:%s\n", __func__, urcp_state, sess->filename));
    //printf("\n< %s %d > urcp_state:%d\n", __func__, __LINE__, urcp_state);
    apStatusForAcOperate = urcp_state;
    if(urcp_state == URCP_STATUS_OK)
    {
		URCP_DEBUG(("debug: %d in %s(), upload_successed \n", __LINE__, __func__));
		msg_send_to_goahead(REQUEST_SEND_FILE, URCP_STATUS_OK);
    } else {
		/* upload failed*/
		URCP_DEBUG(("debug: %d in %s(), upload_failed \n", __LINE__, __func__));
		/*keep NEED_BACKUP state*/
		config_backup_state(NEED_BACKUP);

		msg_send_to_goahead(REQUEST_SEND_FILE, URCP_STATUS_FAIL);
	}

    return;
}

/*
 * Function:upload_current_config_file_to_ac()
 *
 * Description: upload config file to AC.
 *
 * History: chen.shiyu 2012.07.31
 */
void upload_current_config_file_to_ac(char *mac, unsigned int sn)
{
#if 0
	int i;
	printf("\n---------------------------------------\n");
	for (i = 0; i < 6; i++)  {
		printf("%02X ", mac[i]);
	}
	printf("\n---------------------------------------\n");
#endif
	char tp[256], pName[64], *p=tp;
    tftp_opt_t conf_op;
        
    URCP_DEBUG(("Function:%s()\n", __func__));
   
   /*create config file*/ 
    create_file(PORT_CONFIG, &p);
    
    memset(tp, 0, sizeof(tp));
    memset(pName, 0, sizeof(pName));

    /*操作参数*/
    memset(&conf_op, 0, sizeof(tftp_opt_t));
    conf_op.op = TFTP_PUT_FILE;
    conf_op.cb_func = (cb_tftp)urcp_conf_backup_cb;
    conf_op.rport = PORT_CONFIG;

    sprintf(pName, "%d", sn);
	DBCK("pName = %s", pName);
    conf_op.op_file_name = pName;
    
    sprintf(tp, "cp %s%s %s%s -f && rm %s%s -f",/*TFTP_ROOT, pName, */TFTP_ROOT, CONFIG_FILE, TFTP_ROOT, pName, TFTP_ROOT, CONFIG_FILE);
    system(tp);

#if DEBUG_URCP
    sprintf(tp, "cat %s%s", TFTP_ROOT, pName);
    system(tp);
#endif

    urcp_tftp_send_file_request(mac, NULL, &conf_op);
    
    return;
}
#endif
