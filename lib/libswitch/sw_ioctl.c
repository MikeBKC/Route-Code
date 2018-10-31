#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/autoconf.h>
#include "sw_ioctl.h"


/**
 * ������ioctl����
 * ������cmd �������� pData�û����ݵ�ַ
 * ���أ�0 �ɹ� -1 ʧ��
 * 2011-08-29
 * bhou
 */
int32 sw_ioctl(int32 cmd, unsigned char* pData)
{
    int32 fd = 0;
    int32 ret = 0;

    fd = open(SWIOC_DEV_NAME, O_RDWR);/*���豸*/
    if(fd < 0 )
    {
	ret = -1;/*��ʧ��*/
	perror(OPEN_SW_DEV_ERROR);
    }
    else
    {
	ret = ioctl(fd, cmd, pData);/*�ں�ͨ��*/
	close(fd);/*�ر�*/
    }
    return ret;
}
/***
 * ������ioctl����ʧ�ܴ�����
 * bhou
 * 2011-08-29
 */
void sw_ioctl_error(const char* p_str)
{
    char err_str[128];
    uint32 len = 0u;

    memset(err_str, 0, sizeof(err_str));/*��ʼ��*/

    strncpy(err_str, p_str, sizeof(err_str)-1u);/*��������ֵ*/

    len = strlen(err_str);/*������ʹ�ó���*/

    strncpy(&err_str[len], " error:", sizeof(err_str)-len-1u);/*����ʣ���ִ�*/
    
    perror(err_str);

    return;
}

/*����vid��mac��ַɾ����Ӧ�ı���
 *bhou
 */
int32 sw_mac_table_forget_vid_mac(vid_mac_t* vid_mac)
{
    int32 ret = 0;
    ret = sw_ioctl(SWIOC_MAC_DEL, (unsigned char*)vid_mac);/*�ں�ͨ��*/

    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}

/*mac��Ŀѧϰ������Ϊ��̬�󶨣�
 *bhou
 */
int32 sw_mac_table_static_learn(mac_table_entry_t* entry)
{
    int32 ret = 0;
    ret = sw_ioctl(SWIOC_MAC_ADD, (unsigned char*)entry);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}

/*mac���ϻ�ʱ������
 *bhou
 */
int32 sw_mac_table_age_time_set(mac_age_time_t age_time)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_MAC_AGE_TIME_SET, (unsigned char*)&age_time);/*�ں�ͨ��*/

    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}

/*
 * get MAC address table aging time.
 *
 * added by Chen.Shiyu, 2011.08.23
 *
 */
int32 sw_mac_table_age_time_get(mac_age_time_t *age_time)
{
    int32 ret = 0;
    ret = sw_ioctl(SWIOC_MAC_AGE_TIME_GET, (unsigned char*)age_time);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;

}

/*
 * set address table aging function enable or disable 
 *
 * added by Chen.Shiyu, 2011.08.22
 *
 */

int32 sw_mac_table_age_time_enable_set(int32 enable)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_MAC_AGE_TIME_ENABLE_SET, (unsigned char*)&enable);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}

/*
 * get address table aging function.
 *
 * added by Chen.Shiyu, 2011.08.22
 *
 */
int32 sw_mac_table_age_time_enable_get(int32 *enable)
{
    int32 ret = 0;
    ret = sw_ioctl(SWIOC_MAC_AGE_TIME_ENABLE_GET, (unsigned char*)enable);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}

#if 0
/*
 *  ����vlan����
 */
int32 sw_vlan_type_set(uint32 vlan_mode)
{
    int32 ret = 0;
    ret = sw_ioctl(SWIOC_VLAN_TYPE_SET, &vlan_mode);
    return ret;
}

/*��ȡvlan����*/
int32 sw_vlan_type_get(uint32* vlan_mode)
{
   int32 ret = 0;
   ret = sw_ioctl(SWIOC_VLAN_TYPE_GET, vlan_mode);
   return ret;
}
#endif

#if SW_FEATURE_PORT_VLAN
#if 0
/*port vlan��ȡvlan��Ա*/
int32 sw_pvlan_port_members_get(pvlan_no_t pvlan_no, BOOL member[PORT_ARRAY_SIZE])
{
    int32 ret = 0;
    /*�ں�ͨ�����ݽṹ*/
    swioc_pvlan_t tmp;

    /*��ʼ��*/
    memset(&tmp, 0, sizeof(tmp));
    tmp.pvlan_no = pvlan_no;

    ret = sw_ioctl(SWIOC_PORT_VLAN_GET, (unsigned char*)&tmp);/*�ں�ͨ��*/

    if(ret == 0)
    {/*�����ɹ�*/
	memcpy(member, tmp.member, sizeof(BOOL)*PORT_ARRAY_SIZE);
    }
    else
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
    
}
#endif

/*port vlan����vlan��Ա
 *bhou
 */
int32 sw_pvlan_port_members_set(pvlan_no_t pvlan_no, BOOL member[PORT_ARRAY_SIZE])
{
    int32 ret = 0;
    swioc_pvlan_t tmp;

    memset(&tmp, 0, sizeof(tmp));

    tmp.pvlan_no = pvlan_no;/*port vlan ���*/
    memcpy(tmp.member, member, sizeof(BOOL)*PORT_ARRAY_SIZE);/*port vlan��Ա*/

    ret = sw_ioctl(SWIOC_PORT_VLAN_SET, (unsigned char*)(&tmp));/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}
#endif

#if 0
/*tag vlan��ȡvlan��Ա*/
int32 sw_qvlan_port_members_get(vid_t vid, BOOL member[PORT_ARRAY_SIZE])
{
    int32 ret = 0;
    /*�ں�ͨ�����ݽṹ*/
    swioc_qvlan_t tmp;

    /*��ʼ��*/
    memset(&tmp, 0, sizeof(tmp));
    tmp.vid = vid;

    ret = sw_ioctl(fd , SWIOC_TAG_VLAN_GET, &tmp);
    if(ret > 0)
    {/*�����ɹ�*/
	vid = tmp.vid;
	memcpy(member, tmp.member, sizeof(BOOL)*PORT_ARRAY_SIZE);
    }
    return ret;
    
}

/*tag vlan����vlan��Ա*/
int32 sw_qvlan_port_members_set(vid_t vid, BOOL member[PORT_ARRAY_SIZE])
{

    int32 ret = 0;
    swioc_qvlan_t tmp;
    memset(&tmp, 0, sizeof(tmp));
    tmp.vid = vid;
    memcpy(tmp.member, member, sizeof(BOOL)*PORT_ARRAY_SIZE);

    ret = sw_ioctl(fd , SWIOC_TAG_VLAN_SET, &tmp);

    return ret;
}

/*tag vlan�˿�����*/
typedef struct _vlan_port_mode_t
{
    BOOL              aware;          /* VLAN awareness */
    vid_t             pvid;           /* Port VLAN ID (ingress) */
    vtss_vlan_frame_t frame_type;     /* Acceptable frame type (ingress) */
} vlan_port_mode_t;
typedef struct _swioc_vlan_port_mode_t {
    port_no_t         port_no;        /*port no*/
    vlan_port_mode_t  vlan_mode;
} swioc_vlan_port_mode_t;

/*��ȡtag vlan�˿�����*/
int32 sw_vlan_port_mode_get(port_no_t port_no, vlan_port_mode_t * vlan_mode)
{
    int32 ret = 0;
    swioc_vlan_port_mode_t tmp;
    memset(&tmp, 0, sizeof(tmp));

    tmp.port_no = port_no;
	
    ret = sw_ioctl(fd , SWIOC_TAG_VLAN_MODE_GET, vlan_mode);
    if(ret > 0)
    {
	memcpy(vlan_mode, &(tmp.vlan_mode), sizeof(vlan_port_mode_t));
    }
    return ret;
}

/*����tag vlan�˿�����*/
int32 sw_vlan_port_mode_set(port_no_t port_no, vlan_port_mode_t * vlan_mode)
{
    int32 ret = 0;
    swioc_vlan_port_mode_t tmp;
    memset(&tmp, 0, sizeof(tmp));

    tmp.port_no = port_no;
    memcpy(&(tmp.vlan_mode), vlan_mode, sizeof(vlan_port_mode_t));

    ret = sw_ioctl(fd , SWIOC_TAG_VLAN_MODE_SET, &tmp);
    return ret;
}
#endif

#if SW_FEATURE_PORT_CONF
/*�˿ڹ�������
 *bhou
 */
int32 sw_port_conf_set(port_no_t port_no, port_conf_t* conf)
{
    int32 ret = 0;
    swioc_port_conf_t tmp;

    memset(&tmp, 0, sizeof(tmp));

    tmp.port_no = port_no;/*�˿ں�*/
    tmp.port_conf = *conf;/*�˿�����*/

    ret = sw_ioctl(SWIOC_PORT_CONF_SET, (unsigned char*)(&tmp));/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }    
    return ret;
}

/*�˿ڹ�������
 *bhou
 */
int32 sw_link_status_get(port_no_t port_no, char* status)
{
    int32 ret = 0;
    swioc_link_status_t tmp;

    memset(&tmp, 0, sizeof(tmp));

    tmp.port_no = port_no;/*�˿ں�*/

    ret = sw_ioctl(SWIOC_LINK_STATUS_GET, (unsigned char*)(&tmp));/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    else
    {
	strncpy(status, tmp.link_status, MAX_LINK_STATUS_SIZE - 1);
    }
    return ret;
}
#endif


#if SW_FEATURE_PORT_MIRROR
/*
 * Description: To configure the monitored port mode, it decide the
 * 				port Egress or Ingress traffic whether to be copied 
 *				to morroring port or not.
 * monitored mode : Disable 
 *					Egress 
 *					Ingress 
 *					Egress and Ingress
 *
 * added by Chen.Shiyu, 2011.08.24
 *
 */
int32 sw_mirror_egress_ingress_port_mode_set(BOOL port_no[PORT_ARRAY_SIZE], int32 m_mode)
{
    int32 ret = 0;
    swioc_monitor_t tmp;
    memset(&tmp, 0, sizeof(tmp));
	
	/* all monitored ports*/
    tmp.m_mode = m_mode; /*monitored mode*/
    memcpy(tmp.pmonitor_no, port_no, sizeof(BOOL)* PORT_ARRAY_SIZE);

    ret = sw_ioctl(SWIOC_MORROR_EGRESS_ENGRESS_PORT_MODE_SET, (unsigned char*)(&tmp));/*�ں�ͨ��*/

    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}


/*
 * Description: To get all the monitored ports and the mode.
 *
 * added by Chen.Shiyu, 2011.08.24
 *
 */
int32 sw_mirror_egress_ingress_port_mode_get(BOOL port_no[PORT_ARRAY_SIZE], int32 *m_mode)
{
    int32 ret = 0;
    swioc_monitor_t tmp;

    memset(&tmp, 0, sizeof(tmp));

    ret = sw_ioctl(SWIOC_MORROR_EGRESS_ENGRESS_PORT_MODE_GET, (unsigned char*)(&tmp));/*�ں�ͨ��*/

    if(ret == 0)
    {	
		*m_mode = tmp.m_mode;
		memcpy(port_no, tmp.pmonitor_no, sizeof(BOOL)* PORT_ARRAY_SIZE);
    }
    else
    {
		sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}

/*
 * Description: To configure the mirroring port that monitor the all 
 * 				monitored ports traffic.
 *
 * added by Chen.Shiyu, 2011.08.24
 *
 */
int32 sw_mirror_monitor_port_set(port_no_t port_no)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_MORROR_MONITOR_PORT_SET, (unsigned char*)&port_no);/*�ں�ͨ��*/

    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}


/*
 * Description: To get mirroring port.
 *
 * added by Chen.Shiyu, 2011.08.24
 *
 */
int32 sw_mirror_monitor_port_get(port_no_t *port_no)
{
    int32 ret = 0;
    ret = sw_ioctl(SWIOC_MORROR_MONITOR_PORT_GET, (unsigned char*)port_no);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    return ret;
}
#endif /* End SW_FEATURE_PORT_MIRROR */

/*
 *��ȡ��ǰvlan ģʽ: port vlan or 802.1Q vlan
 *
 *KeHuatao
 *2011-08-30
 */
int32 sw_vlan_type_get(void)
{
#ifdef CONFIG_IP1725
    int32 ret = 0;
#if 0
    int32  type = 0;

    ret = sw_ioctl(SWIOC_VLAN_TYPE_GET, (unsigned char*)(&type));/*�ں�ͨ��*/

    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
#else
    ret = VLAN_TYPE_PORT;
#endif
    return ret;
#else
    /*read vlan_type from the nvram*/
    return VLAN_TYPE_PORT;
#endif
}

#if SW_FEATURE_ACL
int32 sw_acl_enable(int32 enable
#if 0
	, int32 source_entry_type
#endif
	)
{
    int32 ret = 0, source;
    sw_acl_enable_t tmp;

    memset(&tmp, 0, sizeof(sw_acl_enable_t));
#if 1
    source = 1;
#else
    source = source_entry_type;
#endif
    tmp.source_entry_mode = source;
    tmp.enable = enable;
    ret = sw_ioctl(SWIOC_ACL_ENABLE, (unsigned char*)&tmp);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
int32 sw_acl_status_get(int32 *enable
#if 0
	, int32 *source_entry_type
#endif
	)
{
    int32 ret = 0;
    sw_acl_enable_t tmp;

    memset(&tmp, 0, sizeof(sw_acl_enable_t));

    ret = sw_ioctl(SWIOC_ACL_STATUS_GET, (unsigned char*)&tmp);/*�ں�ͨ��*/
    *enable = tmp.enable;
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
/*
 *add a acl ruler
 *
 *KeHuatao
 *2011-09-05
 */
int32  sw_acl_add(sw_acl_t * acl)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_ACL_ADD, (unsigned char*)acl);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}

/*
 *delete a acl rule
 *
 *KeHutao
 *2011-09-05
 */
int32  sw_acl_del(int32 id)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_ACL_DEL, (unsigned char*)(&id));/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
/*
 *read a acl rule
 *
 *KeHutao
 *2011-09-05
 */
int32  sw_acl_get(sw_acl_t * acl)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_ACL_GET, (unsigned char*)acl);/*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
#endif

/*
 *��link down�Ķ˿����
 *KeHuatao
 */
#if SW_FEATURE_PORT_CONF
void sw_check_ports_link_status(BOOL *ports)
{
    char str[64];
    uint32 port_no;

    /*all ports*/
    for(port_no=PORT_NO_START; port_no<PORT_NO_END; port_no++)
    {
	memset(str, 0, sizeof(str));
	sw_link_status_get(port_no, str);
	if(strcmp(str, "Down")==0)
	{
	    *(ports + port_no) = '\0';
	}
    }
}
#endif

#if SW_FEATURE_PORT_TRUNK
/*
 * Function: sw_trunk_group_set
 *
 * Description: To set trunk group number and members to deamon layer. 
 *		Each group at least comprise two ports.
 *
 * History: Chen.Shiyu 2011.09.06   created
 */
int32 sw_trunk_group_set(int32 group_no, BOOL members[PORT_ARRAY_SIZE])
{
	int32 ret = 0;
	swioc_trunk_group_t tmp;

	memset(&tmp, 0, sizeof(tmp));
	tmp.gp_no = group_no;
	memcpy(tmp.members, members, sizeof(BOOL)*PORT_ARRAY_SIZE);
	
	ret = sw_ioctl(SWIOC_TRUNK_GROUP_SET, (unsigned char*)(&tmp)); /*�ں�ͨ��*/ 
	if(ret < 0)
	{
	sw_ioctl_error(__func__);/*����ʧ��*/
	}

	return ret;
}

/*
 * Function: sw_trunk_group_members_get
 *
 * Description: To get trunk group number and members from deamon layer.
 * 			Each group at least comprise two ports.
 *
 * History: Chen.Shiyu 2011.09.06   created
 */
int32 sw_trunk_group_get(int32 group_no, BOOL members[PORT_ARRAY_SIZE])
{
	int32 ret = 0;
	swioc_trunk_group_t tmp;

	memset(&tmp, 0, sizeof(tmp));

	tmp.gp_no = group_no;
	ret = sw_ioctl(SWIOC_TRUNK_GROUP_GET, (unsigned char*)(&tmp)); /*�ں�ͨ��*/ 
	if(ret < 0)
	{
	sw_ioctl_error(__func__);/*����ʧ��*/
	}

	/* copy the data and return it. */
	memcpy(members, tmp.members, sizeof(BOOL)*PORT_ARRAY_SIZE);
	
	return ret;
}

/*
 * Function: sw_trunk_mode_set
 *
 * Description: To set Trunk mode which is used to select Hash algorithm 
 *		to deamon layer.
 *
 * History: Chen.Shiyu 2011.09.06   created
 */
int32 sw_trunk_mode_set(int32 value)
{
	int32 ret = 0;
	
	ret = sw_ioctl(SWIOC_TRUNK_MODE_SET, (unsigned char*)&value); /*�ں�ͨ��*/ 
	if(ret < 0)
	{
	sw_ioctl_error(__func__);/*����ʧ��*/
	}

	return ret;
}

/*
 * Function: sw_trunk_mode_get
 *
 * Description: To get Trunk mode which is used to select Hash algorithm
 * 		from deamon layer.
 *
 * History: Chen.Shiyu 2011.09.06   created
 */
int32 sw_trunk_mode_get(int32 *value)
{
	int32 ret = 0;

	ret = sw_ioctl(SWIOC_TRUNK_MODE_GET, (unsigned char*)value); /*�ں�ͨ��*/ 
	if(ret < 0)
	{
	sw_ioctl_error(__func__);/*����ʧ��*/
	}

	return ret;
}

/*
 * ��ȡĳ�˿����ڻ�����Ա
 * KeHuatao
 */
int32 sw_trunk_members_get(port_no_t port_no, BOOL member[PORT_ARRAY_SIZE])
{
    int32 ret = 0;
#ifdef CONFIG_VSC7390
    swioc_trunk_group_t tmp;

    memset(&tmp, 0, sizeof(tmp));

    tmp.gp_no = port_no;/*����ż�¼��ǰ��Ա��һ���˿ں�*/
    ret = sw_ioctl(SWIOC_TRUNK_GROUP_GET_BY_PORT, (unsigned char*)(&tmp)); /*�ں�ͨ��*/ 
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    /* copy the data and return it. */
    memcpy(member, tmp.members, sizeof(BOOL)*PORT_ARRAY_SIZE);
#else
    BOOL member1[PORT_ARRAY_SIZE];
    poag_no_t  poag;

    /*�Ƿ�Ϸ�*/
    if((port_no>=PORT_NO_START) && (port_no<PORT_NO_END))
    {
        for(poag=0;poag<MAX_TRUNK;poag++)
	{
	    memset(member1, 0, sizeof(member1));
	    /*get trunk members*/
	    sw_trunk_group_get(poag, member1);
	    if(member1[port_no]!=0)
	    {
		memcpy(member, member1, sizeof(member1));
		break;
	    }
	}
    }
#endif
    return ret;
}
#endif /* End SW_FEATURE_PORT_TRUNK */

#if SW_FEATURE_PACKET
#if 0
/***
 * broadcast packet detination ports query
 * exclude_port: Դ�˿ڻ�Ŀ�Ķ˿��ų��Ķ˿� 
 * dst_ports : Ŀ�Ķ˿�
 * bhou
 * 2011-09-28
 */
int32 sw_bcp_get_dst_ports(int32 exclude_port, BOOL** dst_ports)
{
    static swioc_bcp_dst_ports_t bcp_dst;
    int32  swioc_ret = 0;

    memset(&bcp_dst, 0, sizeof(bcp_dst));

    bcp_dst.src_port = exclude_port;

    swioc_ret = sw_ioctl(SWIOC_BCP_DST_PORTS_GET, (unsigned char*)(&bcp_dst)); /*�ں�ͨ��*/ 
    if(swioc_ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    else
    {
	/*�ɹ�*/
	*dst_ports = bcp_dst.dest;
    }
    return swioc_ret;
}
#endif

/***
 * cpu forward packet: destination ports query
 * cond: query condition
 * ret : query result
 * bhou
 * 2011-09-28
 */
int32 sw_fwd_get_dst_ports(fwd_dst_ports_cond_t* cond, fwd_dst_ports_ret_t* ret)
{
    static swioc_fwd_dst_ports_t fwd_dst;
    int32 swioc_ret = 0;

    memset(&fwd_dst, 0, sizeof(fwd_dst));

    fwd_dst.condition = *cond;

    swioc_ret = sw_ioctl(SWIOC_FWD_DST_PORTS_GET, (unsigned char*)(&fwd_dst)); /*�ں�ͨ��*/ 
    if(swioc_ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }
    else
    {
	/*�ɹ�*/
	*ret = fwd_dst.result;
    }

    return swioc_ret;
}
#endif

#if SW_FEATURE_PORT_STATISTICS
/*
 * Function: sw_port_counter_get
 *
 * Description: To get the port statistics information(Rx/Tx packets).
 *
 * History: Chen.Shiyu 2011.09.27   created
 */
int32 sw_port_counter_get(port_no_t port_no, sw_port_stat_t *stat)
{
    int32 ret = 0;
    swioc_port_stat_t tmp;

    memset(&tmp, 0, sizeof(swioc_port_stat_t));
    tmp.port_no = port_no;
    tmp.port_stat = *stat;

    ret = sw_ioctl(SWIOC_PORT_COUNTER_GET, (unsigned char*)(&tmp)); /*�ں�ͨ��*/ 
    *stat = tmp.port_stat;
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
/*
 * clear the port counter
 * KeHuatao
 *
 */
int32 sw_port_counter_clear(port_no_t port_no)
{
    int32 ret = 0;
    port_no_t tmp;

    tmp = port_no;
    ret = sw_ioctl(SWIOC_PORT_COUNTER_CLEAR, (unsigned char*)(&tmp)); /*�ں�ͨ��*/
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
#endif /* End SW_FEATURE_PORT_STATISTICS */
int32 sw_cpu_vid_get(void)
{
    return 1;
}
#if SW_FEATURE_MAC_TABLE_OPT
/*
 *show the mac table
 *
 *KeHuatao
 */
int32 sw_show_mac_table(void)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_SHOW_MAC_TABLE, NULL); /*�ں�ͨ��*/ 
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
#endif

#if SW_FEATURE_PORT_PRIORITY
/* 
 * Function: sw_port_priority_set
 * 
 * Description: To set port priority
 *
 * Input: port_prio
 *
 * History: 2011.11.07 added by Chen.Shiyu
 */
int32 sw_port_priority_set(uint32 port_prio[PORT_ARRAY_SIZE])
{
    int32 ret = 0;
    
    ret = sw_ioctl(SWIOC_PORT_PRIORITY_SET, (unsigned char*)port_prio);/*�ں�ͨ��*/

    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
#endif

#if SW_FEATURE_STORM_CONTROL
/*
 * Function: sw_broadcast_storm_control_set
 *
 * Description: To set the ports broadcast storm control threshold and timer unit.
 *
 * History: History: added by Chen.Shiyu  2011.11.08
 */
int32 sw_broadcast_storm_control_set(BOOL ctrl_port[PORT_ARRAY_SIZE], uint32 value, BOOL timer_sel)
{
	int32 ret = 0;
	swioc_storm_control_t tmp;

	memset(&tmp, 0, sizeof(tmp));
	tmp.pCtrlThreshold = value;
	tmp.pTimerSel = timer_sel;
	memcpy(tmp.pStormCtrlBC, ctrl_port, sizeof(BOOL)*PORT_ARRAY_SIZE);
	
	ret = sw_ioctl(SWIOC_BOARDCAST_STORM_CONTROL_SET, (unsigned char*)(&tmp)); /*�ں�ͨ��*/ 
	if(ret < 0)
	{
	sw_ioctl_error(__func__);/*����ʧ��*/
	}

	return ret;
}
#endif

#if SW_FEATURE_RATE_LIMIT
/*
 * Function: sw_port_bandwidth_func_set
 *
 * Description: To set the port bandwidth ingress and egress rate.
 *
 *  History: added by Chen.Shiyu  2011.11.09
 */
int32 sw_port_bandwidth_func_set(port_no_t port_no, uchar type, uint32 tx_rate, uint32 rx_rate)
{
	int32 ret = 0;
	swioc_rate_ctrl_t tmp;

	memset(&tmp, 0, sizeof(tmp));
	tmp.port_no = port_no;
	tmp.rate_type = type;
	tmp.tx_rate = tx_rate;
	tmp.rx_rate = rx_rate;
    	
	ret = sw_ioctl(SWIOC_RATE_LIMIT_SET, (unsigned char*)(&tmp)); /*�ں�ͨ��*/ 
	if(ret < 0)
	{
	sw_ioctl_error(__func__);/*����ʧ��*/
	}

	return ret;
}

/*
 * Function: sw_port_bandwidth_throttle_set
 *
 * Description: To select port bandwidth throttle
 *	    0:32kbps unit	    1:512kbps unit
 *	    if changed, all ports rate clear to 0.
 *
 * History: added by Chen.Shiyu  2011.11.09
 */
int32 sw_port_bandwidth_throttle_set(uchar throttle)
{
    int32 ret = 0;

    ret = sw_ioctl(SWIOC_RATE_LIMIT_THROTTLE_SET, (unsigned char*)(&throttle)); /*�ں�ͨ��*/ 
    if(ret < 0)
    {
	sw_ioctl_error(__func__);/*����ʧ��*/
    }

    return ret;
}
#endif
