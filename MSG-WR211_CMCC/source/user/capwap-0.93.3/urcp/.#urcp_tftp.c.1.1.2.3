/*
 *二层实现文件传输
 *
 *KeHuatao
 *2011-04-07
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <arpa/inet.h> /*inet_addr*/
#include <sys/types.h>
#include <sys/wait.h>
#include "common.h"
#ifdef URCP_WEB
#include <switch.h>
#include <sw_cfg.h>
#include <sw_ioctl.h>
#endif
#include "urcp.h"
#include "app_msg.h"
#include "urcp_config.h"
#include "urcp_tftp.h"
#include "urcp_debug.h"


static urcp_tftp_session_t  urcp_tftp_session;
static uint16 urcp_tftp_session_alloc(uchar *macaddr,urcp_tftp_sess_type_t session_type,urcp_tftp_state_t session_status,urcp_tftp_session_info_t**sess, cb_tftp cb_func);
static void urcp_tftp_lookup_session(uchar *macaddr, uint16 lId, uint16 rId, urcp_tftp_session_info_t** sess);
static void urcp_tftp_session_clear(urcp_tftp_session_info_t* sess, urcp_status_t urcp_state);
static void urcp_tftp_send_pkt(urcp_tftp_session_info_t* tftp_info, uint16 vid, uint32 type);
static uint16 urcp_tftp_send_request(uchar* macaddr, uchar* passwd, tftp_opt_t* opt, uchar* pkt, uint32 len);
static void urcp_tftp_rev_file_request(urcp_infor_t* urcp_info, uint32 op);
static void set_file_name(urcp_infor_t* urcp_info, urcp_tftp_session_info_t* sess);
#if DEBUG_URCP 
void printf_time(void);
#endif
#if ERROR_URCP
static const char* statestr[]={ "IDLE",
                                 "WAIT_R_OACK",
                                 "WAIT_OACK_ACK",
                                 "WAIT_W_OACK",
                                 "WAIT_DATA",
                                 "WAIT_ACK",
                                 "WAIT_FIN"};
#endif
/***********************************************************************
*函数名      urcp_tftp_session_alloc
*功能        创建一个会话
*参数        macaddr         会话远程交换机MAC地址
*            session_type    会话类型：读取文件、普通数据(数据量较大的配置)
*            session_status  设置会话初始状态 
*            ses_id          用于返回会话id
***********************************************************************/
static uint16 urcp_tftp_session_alloc(uchar *macaddr,urcp_tftp_sess_type_t session_type,urcp_tftp_state_t session_status,urcp_tftp_session_info_t**sess, cb_tftp cb_func)
{
    uint32 i;
    uint16 ret_id=0u;

    *sess=NULL;
    /*搜索所有会话*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	/*取一个空闲会话*/
	if(urcp_tftp_session.session[i].state==IDLE)
	{
	    URCP_DEBUG(("%s: id:%d\n", __func__, i));
	    /*空闲的*/
	    *sess=&urcp_tftp_session.session[i];
	    break;
	}
    }

    /*是否找到空闲会话*/
    if(*sess!=NULL)
    {
	/*找到了*/
	memset(*sess, 0, sizeof(struct _urcp_tftp_session_info_t));
	/*初始化*/
	if(urcp_tftp_session.id_counter==0u)
	{
	    /*会话id从1开始, 0表示会话开始，不知道对方id*/
	    urcp_tftp_session.id_counter = urcp_tftp_session.id_counter + 1u;
	}
	(*sess)->local_id = urcp_tftp_session.id_counter;
	(*sess)->sess_type = session_type;
	(*sess)->state = session_status;
	(*sess)->cb = cb_func;
	memcpy((*sess)->mac, macaddr, MAC_LEN);

	URCP_DEBUG(("%s: local id:%d\n", __func__, (*sess)->local_id));
        db_warn(URCP_MODULE_ID_TFTP, "%s: local id:%d\n", __func__, (*sess)->local_id);
	/*会话id加1*/
	urcp_tftp_session.id_counter = urcp_tftp_session.id_counter + 1u;
       
	ret_id = (*sess)->local_id;
    }
    else
    {
	/*查找失败*/
	URCP_ERROR(("No idle session!\n"));
        db_warn(URCP_MODULE_ID_TFTP, "%s: No idle tftp session!\n", __func__);
    }

    return ret_id;
}
#if DEBUG_URCP
struct timeval tpstart;
void printf_time(void)
{
    struct timeval tpend;
    float  timeuse;

    gettimeofday(&tpend, NULL);
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ 
             tpend.tv_usec-tpstart.tv_usec; 
    timeuse/=1000000;

    printf("Used Time:%f\n",timeuse);
}
#endif
/*
 *初始化
 *KeHuatao
 */
void urcp_tftp_init(void)
{
    char tp[64];

    memset(tp, 0, sizeof(tp));
    memset(&urcp_tftp_session, 0, sizeof(urcp_tftp_session_t));
    /*创建configs文件夹*/
    sprintf(tp, "mkdir %s -p", RAM_CONFIG_PATH);
    system(tp);

    sprintf(tp, "mkdir %s -p", TFTP_LOG_ROOT);
    system(tp);

    return;
}

/*
 *打开文件
 *KeHuatao
 */
void urcp_open_file(uchar* filename, uint32 len, const uchar* mode, FILE**fp)
{
    uchar tp[128]={0};

    /*补全路径*/
    strcpy(tp, TFTP_ROOT);
    /*复制文件名*/
    strncat(tp, filename, len);
    /*打开文件*/
    *fp = fopen(tp, mode);
    URCP_DEBUG(("%s: read file: %s\n", __func__, tp));
    db_info(URCP_MODULE_ID_TFTP, "%s open file:%s\n", __func__, tp);
#if DEBUG_URCP
    if(*fp==NULL)
    {
	perror("fopen");
	URCP_DEBUG(("%s: open file failed\n", __func__));
    }
#endif

    return;
}

void create_file(uint32 rport, char** p, uchar *mac)
{
#ifdef URCP_AC
    urcp_dev_t* dev=NULL;
#endif
    /*port*/
    URCP_DEBUG(("%s:rport:%d\n", __func__, rport));
    switch(rport)
    {
#ifdef URCP_AP
        case PORT_CONFIG:/*读取配置文件*/
	    create_nvram_file();
	    *p = (uchar*)(CONFIG_FILE);
	    db_info(URCP_MODULE_ID_TFTP, "AP request profile\n");
	    break;
	case PORT_LOG:/*读取日志文件*/
//		create_log_file(*p);
	    break;
	case PORT_IMAGE:
		create_image_file();
		*p = (uchar *)(IMAGE_FILE);
	    db_info(URCP_MODULE_ID_TFTP, "AP request image\n");
		break;
	case PORT_CONFIG_AUTO:
	    /*config file have been created on dir /tmp/urcp */
//	    *p = (uchar*)(CONFIG_FILE);
//	    break;
	    db_info(URCP_MODULE_ID_TFTP, "AP auto profile\n");
#endif
#ifdef URCP_AC
	case PORT_IMAGE_AUTO:
	    /*image file have been created on dir /tmp/urcp */
            dev = urcp_dev_table_lookup(mac, 0);
	    if(dev!=NULL)
	    {
	        sprintf(*p, "%s_%s.bin",dev->dev_model, dev->hw_version);
	    }
	    //*p = (uchar*)(IMAGE_FILE);
	    break;
#endif
	default:/*未知*/
	    break;
    }

    return;
}    

/*
 *收到读写文件的请求的处理
 *KeHuatao
 */
static void urcp_tftp_rev_file_request(urcp_infor_t* urcp_info, uint32 op)
{
	DBCK();
    char tp[256], *p=tp;
    uchar *tmp;
    uint32 len=0u, blk_size;
    FILE* fp=NULL;
    urcp_tftp_file_request_t* tftp_file_hdr;
    urcp_tftp_session_info_t* tftp_session=NULL;
    urcp_tftp_state_t st;
    urcp_tftp_sess_type_t ty;

    URCP_DEBUG(("%s\n", __func__));
    tftp_file_hdr = (urcp_tftp_file_request_t*)(urcp_info->cmd_buff);
    /*读取文件名长度*/
    tmp = (uchar*)(urcp_info->cmd_buff) + sizeof(urcp_tftp_file_request_t);
    len = ntohl(tftp_file_hdr ->name_len);

    /*打开文件*/
    if(op==URCP_TFTP_READ_FILE)
    {
	memset(tp, 0, sizeof(tp));
	create_file(ntohl(tftp_file_hdr->tftp_hdr.remote_port), &p, urcp_info->mac);
	if(strcmp(p, "")==0)/*可能为普通文件*/
	{
	    /*复制文件名*/
	    strncpy(p, tmp, len);
	}
	/*打开要读的文件*/
	db_info(URCP_MODULE_ID_TFTP, "%s to request file:%s\n", str_macaddr(urcp_info->mac), p);
        urcp_open_file(p, strlen(p), "rb", &fp);
	st = WAIT_OACK_ACK;
	ty = TFTP_SEND_FILE;
    }
    else
    {
	/*创建接收文件*/
        urcp_open_file(tmp, len, "wb", &fp);
	st = WAIT_DATA;
	ty = TFTP_REV_FILE;
    }

    if(fp!=NULL)
    {
	/*分配会话*/
	urcp_tftp_session_alloc(urcp_info->mac, ty, st, &tftp_session, (cb_tftp)urcp_rev_conf_cb);
	if(tftp_session!=NULL)/*该会话不存在*/
	{
	    blk_size = ntohl(tftp_file_hdr->blksize);
	    /*确block大小*/
	    if(blk_size>TFTP_BLOCK_LEN)
	    {
		blk_size=TFTP_BLOCK_LEN;
	    }
	    /*文件存在，分析文件长度*/
            if(op==URCP_TFTP_READ_FILE)
	    {
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		strncpy(tftp_session->filename, tp, MAX_FILE_NAME_LEN);
	    }
	    else
	    {
		memcpy(tftp_session->filename, tmp, len);
		/*跳过文件名*/
		tmp = tmp + len;
		/*取文件长度*/
		len = *((uint32*)tmp);
		len = ntohl(len);
	    }

	    /*各个参数*/
	    tftp_session->remote_id = ntohs(tftp_file_hdr->tftp_hdr.lid);
	    tftp_session->tftp_port = ntohl(tftp_file_hdr->tftp_hdr.remote_port);
	    tftp_session->fd = fp;
	    tftp_session->block_size = blk_size;
	    tftp_session->block_total = len/blk_size;
	    tftp_session->last_block_size = len%blk_size;
	    tftp_session->fromL3 = urcp_info->fromL3;
	    URCP_DEBUG(("%s: file len:%d, blksize:%d, port:%d\n", __func__, len, blk_size, (tftp_session->tftp_port)));
	    /**调整大小*/
	    if(tftp_session->last_block_size!=0u)
	    {
		tftp_session->block_total = tftp_session->block_total+1u;
	    }
	    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_OACK);
	}
    }
#ifndef URCP_REL
    else
    {
	db_info(URCP_MODULE_ID_TFTP, "%s file not exist\n", str_macaddr(urcp_info->mac));
    }
#endif

    return;
}
void urcp_tftp_session_clear_by_id(uint32 id)
{
    int32 i;

    /*所有会话*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	if((urcp_tftp_session.session[i].state!=IDLE)
	   && (urcp_tftp_session.session[i].local_id==id)
	  )
	{
	    /*clear*/
	    urcp_tftp_session_clear(&(urcp_tftp_session.session[i]), URCP_STATUS_WAIT);
	}
    }
}
/*
 * 清除所有会话
*
* KeHuatao  
*/
void urcp_tftp_session_clear_all(void)
{
    uint32 i;

    /*所有会话*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	if(urcp_tftp_session.session[i].state!=IDLE)
	{
	    /*clear*/
	    urcp_tftp_session_clear(&(urcp_tftp_session.session[i]), URCP_STATUS_WAIT);
	}
    }

    return;
}

/*
 *清除一个会话，释放内存或文件指针
 *KeHuatao
 *
 */
static void urcp_tftp_session_clear(urcp_tftp_session_info_t* sess, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s: clear session:%d\n", __func__, sess->local_id));
    /*状态置为空闲*/
    sess->state=IDLE;

    /*释放缓冲区*/
    if(sess->fd!=NULL)
    {
	fclose(sess->fd);
	URCP_DEBUG(("%s: fclose fd\n", __func__));
    }

    /*释放缓冲区*/
    if(sess->buffer!=NULL)
    {
	free(sess->buffer);
	URCP_DEBUG(("%s: free buffer\n", __func__));
    }

    /*释放缓冲区*/
    if(sess->last_pkt!=NULL)
    {
	free(sess->last_pkt);
	URCP_DEBUG(("%s: free last pkts\n", __func__));
    }

    /*指向回调函数*/
    if(sess->cb!=NULL)
    {
        sess->cb((void*)sess, urcp_state);
    }

    return;
}

/*************************************
 *搜索会话表，判断某个id会话是否存在
 *KeHuatao
 *
 ************************************/
static void urcp_tftp_lookup_session(uchar *macaddr, uint16 lId, uint16 rId, urcp_tftp_session_info_t** sess)
{
    uint32 i;

    URCP_DEBUG(("%s: lId:%d, rId:%d\n", __func__, lId, rId));
    *sess=NULL;
    /*搜索所有会话*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	/*id相等，且非空闲的会话*/
	if((urcp_tftp_session.session[i].state!=IDLE)
            && ((lId==0u) || (urcp_tftp_session.session[i].local_id==lId))/*local id*/
            && ((rId==0u) || (urcp_tftp_session.session[i].remote_id==rId))/*remote id*/
	    && (mac_cmp((uchar*)(urcp_tftp_session.session[i].mac), macaddr)==0)
	)
	{
	    /*返回会话结构*/
	    *sess = &urcp_tftp_session.session[i];
	    URCP_DEBUG(("%s: found session: %d\n", __func__, i));
	    break;
	}
    }
#if DEBUG_URCP
    if(*sess==NULL)
    {
	URCP_DEBUG(("%s: failed\n", __func__));
    }
#endif
    return;
}
/*
 *发送ack确认
 *KeHuatao
 */
static void urcp_tftp_send_pkt(urcp_tftp_session_info_t* tftp_info, uint16 vid, uint32 type)
{
    uchar  buf[URCP_BUF_SIZE], *tmp=buf;
    uint32 len=0u;
	int iftag = 0;
    urcp_tftp_hdr_t* tftp_hdr;
    urcp_tftp_ack_t* tftp_ack;
    urcp_tftp_data_t* tftp_data;
    urcp_tftp_oack_hdr_t* tftp_oack;

    if(vid >= (uint16)1u)
	{
		iftag = 1;
	}else{
		iftag = 0;
	}
    URCP_DEBUG(("%s:block size:%d, total:%d, last block size:%d\n", __func__, tftp_info->block_size, tftp_info->block_total, tftp_info->last_block_size));
    /*头部*/
    switch(type)
    {
	case URCP_TFTP_ACK:/*发送ACK*/
	    URCP_DEBUG(("%s:URCP_TFTP_ACK\n", __func__));
	    /*初始化*/
	    memset(buf, 0, sizeof(buf));
		if(iftag == 1)
		{
		    len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_ack_t) + sizeof(uint32);

		    /*各部分数据*/
		    tftp_ack = (urcp_tftp_ack_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32)]);
		}else{
			len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_ack_t);

		    /*各部分数据*/
		    tftp_ack = (urcp_tftp_ack_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
		}
	    tftp_ack->block_num=htonl(tftp_info->blockct);
	    break;
	case URCP_TFTP_DATA:/*发送下一块数据*/
	    URCP_DEBUG(("%s:URCP_TFTP_DATA\n", __func__));
	    if(tftp_info->last_pkt==NULL)/*尚未分配空间*/
	    {
		URCP_DEBUG(("%s: malloc for last_pkt\n", __func__));
		if(iftag == 1)
		{
			len = ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_data_t) + tftp_info->block_size;
		}else{
			len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_data_t) + tftp_info->block_size;
		}
		tftp_info->last_pkt = (uchar*)malloc(len);
		if(tftp_info->last_pkt==NULL)
		{
		    URCP_ERROR(("malloc failed: len:%d\n", len));
		    /*分配失败*/
		    break;
		}
		tftp_info->last_pkt_len = len;
	    }
	    /*block num++*/
	    tftp_info->blockct = tftp_info->blockct + 1u;

	    tmp = tftp_info->last_pkt;
	    memset(tmp, 0, tftp_info->last_pkt_len);
	    /*包信息*/
		if(iftag == 1)
		{
			tftp_data = (urcp_tftp_data_t*)(&tmp[ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t)]);
		}else{
			tftp_data = (urcp_tftp_data_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
		}
	    tftp_data->block_num=htonl(tftp_info->blockct);
	    /*确定包大小*/
	    if(tftp_info->blockct<tftp_info->block_total)
	    {
		/*数据包大小*/
		len = tftp_info->block_size;
	    }
	    else
	    {
		/*最后一个数据包可能长度不够*/
		len = tftp_info->last_block_size;
	    }

	    URCP_DEBUG(("%s: send block:%d\n", __func__, tftp_info->blockct));
	    /*复制要发送的数据*/
	    if(tftp_info->fd!=NULL)
	    {
		/*发送文件*/
		URCP_DEBUG(("%s: now read file len:%d\n", __func__, len));
		if(fread(((uchar*)tftp_data+sizeof(urcp_tftp_data_t)), 1, len, tftp_info->fd)!=len)
		{
		    /*长度不够*/
		    if(feof(tftp_info->fd)!=0)
		    {
			/*出错,退出*/
			len=0u;
			urcp_tftp_session_clear(tftp_info, URCP_STATUS_FAIL);
			URCP_DEBUG(("%s: read file failed\n", __func__));
			break;
		    }
		}
	    }
	    else if(tftp_info->buffer!=NULL)
	    {
		/*发送数据*/
		/*复制数据*/
		memcpy((uchar*)(tftp_data+sizeof(urcp_tftp_data_t)), tftp_info->buffer_ptr, len);
		tftp_info->buffer_ptr = tftp_info->buffer_ptr + len;
	    }
	    else/*不应该走到这里*/
	    {
		len=0u;
		URCP_DEBUG(("%s:URCP_TFTP_DATA, both fd and buffer are null!\n", __func__));
		break;
	    }
	    tftp_data->data_len=htonl(len);
	    URCP_DEBUG(("%s: send pkt header len:%d\n", __func__, (sizeof(urcp_tftp_data_t) + len)));
		if(iftag == 1)
		{
			len = ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_data_t) + len;
		}else{
			len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_data_t) + len;
		}
	    tftp_info->last_pkt_len = len;
	    break;
	case URCP_TFTP_OACK:/*发送OACK*/
	    URCP_DEBUG(("%s:URCP_TFTP_OACK, sess type:%d\n", __func__, tftp_info->sess_type));
	    db_info(URCP_MODULE_ID_TFTP, "%s: oack to %s\n", __func__,str_macaddr(tftp_info->mac));
	    /*初始化*/
	    memset(buf, 0, sizeof(buf));
		if(iftag == 1)
		{
			tftp_oack = (urcp_tftp_oack_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t)]);
			len = ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_oack_hdr_t);
		}else{
			tftp_oack = (urcp_tftp_oack_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
			len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_oack_hdr_t);
		}
	    tftp_oack->blksize = htonl(tftp_info->block_size);
	    tftp_oack->length = tftp_info->block_size * (tftp_info->block_total-1u) + tftp_info->last_block_size;
	    URCP_DEBUG(("%s: local id:%d, file len:%d\n", __func__, tftp_info->local_id, tftp_oack->length));
	    tftp_oack->length = htonl(tftp_oack->length);
#if 0
	    if(tftp_info->sess_type==TFTP_SEND_FILE)/*要读文件*/
	    {
	    }
	    else if(tftp_info->sess_type==TFTP_REV_FILE)
	    {
		URCP_DEBUG(("%s: rev file\n", __func__));
#if 0
                tftp_oack = (urcp_tftp_oack_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
		tftp_oack->blksize = htonl(tftp_info->block_size);
		len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_oack_hdr_t);
#endif
	    }
	    else if(tftp_info->sess_type==TFTP_SEND_DATA)
	    {
	    }
	    else if(tftp_info->sess_type==TFTP_REV_DATA)
	    {
	    }
#endif
	    break;
	case URCP_TFTP_ERROR:/*发送错误信息*/
	    /**/
	    break;
	default:/*未知*/
	    break;
    }

    if(len>0u)
    {
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
		urcp_buid_tftp_header(tmp, tftp_info->mac, (uint16)URCP_TFTP, tftp_info->remote_id, NULL, vid);
		if(iftag == 1)
		{
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
			/*填写tftp数据部分*/
			tftp_hdr = (urcp_tftp_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32)]);
		}else{
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
			/*填写tftp数据部分*/
			tftp_hdr = (urcp_tftp_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	tftp_hdr->opcode=htonl(type);
	tftp_hdr->lid=htons(tftp_info->local_id);
#if 1
	/*校验和*/
	if(iftag == 1)
	{
		tftp_hdr->chksum=chk_sum((uint16*)tftp_hdr, len-ETH_URCP_OFFSET-sizeof(urcp_hdr_t) - sizeof(uint32));
	}else{
		tftp_hdr->chksum=chk_sum((uint16*)tftp_hdr, len-ETH_URCP_OFFSET-sizeof(urcp_hdr_t));
	}
	tftp_hdr->chksum=~(tftp_hdr->chksum);
	URCP_DEBUG(("%s: chk len:%d, chksum:%08x\n", __func__, len-ETH_URCP_OFFSET-sizeof(urcp_hdr_t), tftp_hdr->chksum));
#endif
	/*发送*/
	h2_send_frame(tmp, len
#ifdef URCP_AC
		,tftp_info->fromL3
#endif
		);
	URCP_DEBUG(("%s: send len:%d\n", __func__, len));
    }
#if DEBUG_URCP
    else
    {
	URCP_DEBUG(("%s: no packet to be send\n", __func__));
    }
#endif

    return;
}

/*
 *保存收到数据
 *KeHuatao
 *
 **/
static void urcp_tftp_save_data(urcp_tftp_session_info_t* tftp_session, uchar *tftp_data, uint32 len)
{
    if((tftp_session->sess_type==TFTP_GET_FILE) || (tftp_session->sess_type==TFTP_REV_FILE))
    {
	/*写文件*/
	if(tftp_session->fd!=NULL)
	{
	    URCP_DEBUG(("%s\n",__func__));
	    /*写入文件*/
	    fwrite(tftp_data, 1, len, tftp_session->fd);
	}
    }
    else if(tftp_session->sess_type==TFTP_GET_DATA)
    {
	/*写数据*/
	if(tftp_session->buffer_ptr!=NULL)
	{
	    /*复制数据*/
	    memcpy(tftp_session->buffer_ptr, tftp_data, len);
	    /*移动写指针*/
	    tftp_session->buffer_ptr = tftp_session->buffer_ptr + len;
	}
    }

    return;
}
/*
 *发送读文件请求 
 *KeHuatao
 */
uint16 urcp_tftp_send_file_request(uchar *macaddr, uchar* passwd, tftp_opt_t *opt)
{
    uchar buf[128], *tmp=buf;
    uint32 l=0u, i, *p;
    int32 k=1;
    uint16 id1=0;
    urcp_tftp_file_request_t* req;

    URCP_DEBUG(("%s: %s\n", __func__, opt->op_file_name));
    /*头部数据*/
    memset(buf, 0, sizeof(buf));
    req = (urcp_tftp_file_request_t*)tmp;
    req->blksize = htonl(TFTP_BLOCK_LEN);
    /*文件名长度*/
    l = strlen(opt->op_file_name);
    if(l>MAX_FILE_NAME_LEN)
    {
        return 0u;
    }
    req->name_len = htonl(l);
    /*文件名*/
    tmp = &tmp[sizeof(urcp_tftp_file_request_t)];
    strcpy(tmp, opt->op_file_name);
	DBCK("op_file_name = %s", tmp);
    req->tftp_hdr.remote_port=htonl(opt->rport);
    /*设置opcode*/
    if(opt->op==TFTP_PUT_FILE)
    {
	req->tftp_hdr.opcode=htonl(URCP_TFTP_WRITE_FILE);

	/*文件长度*/
	k = get_conf_file_size(opt->op_file_name);
	if(k>0)
	{
	    URCP_DEBUG(("%s: put file len:%d\n", __func__, k));
	    p = (uint32*)(&tmp[l]);
	    /*写文件长度*/
	    i = (uint32)k;
	    i = htonl(i);
	    *p = i;
	    l = l + sizeof(uint32);
	}
#if DEBUG_URCP
	else
	{
	    URCP_DEBUG(("%s: file len: 0\n", __func__));
	}
#endif
    }
    else 
    {
	req->tftp_hdr.opcode=htonl(URCP_TFTP_READ_FILE);
    }
    if(k>0)
    {
	/*不包含urcp_tftp_hdr_t的长度*/
	l = l+sizeof(urcp_tftp_file_request_t);
	URCP_DEBUG(("%s: header len:%d\n", __func__, l));
	/*发送请求*/
	id1 = urcp_tftp_send_request(macaddr, passwd, opt, buf, l);
    }

    return id1;
}
/*
 *重命名文件名 ip+时间 保证文件名唯一性和易于理解
 *KeHuatao
 *
 **/
static void set_file_name(urcp_infor_t* urcp_info, urcp_tftp_session_info_t* sess)
{
    char tp[256];
    uint32  l;
    time_t timep;
#ifdef URCP_WEB
    uint32 tmp;
    urcp_dev_t *dev=NULL;
#endif
	DBCK("sess->filename = %s", sess->filename);


    memset(tp, 0, sizeof(tp));
    /*复制文件名*/
    strncpy(tp, sess->filename, MAX_FILE_NAME_LEN);
#ifdef URCP_WEB
    /*ip*/
    tmp = ntohl(urcp_info->urcp_header->ip);
    /*搜索*/
    dev=urcp_dev_table_lookup(urcp_info->mac, tmp);
    if(dev!=NULL)
    {
	/*将型号信息写入文件名*/
	strncat(tp, dev->dev_model, DEV_TYPE_SIZE);
    }
#else
    strncat(tp, "AP", DEV_TYPE_SIZE);
#endif

    /*分隔符*/
    l = strlen(tp);
    tp[l] = '_';
    l++;

#if 0
    if(tmp!=0u) /*ip*/
    {
	int2ip(tmp, &tp[l]);
    }
    else/*mac*/
#endif
    {
	sprintf(&tp[l], "%s", str_macaddr(urcp_info->mac));
    }

    /*长度*/
    l = strlen(tp);
    /*读取当前时间*/
    time(&timep);
    sprintf(&tp[l], ":%d", (int)timep);
    
    URCP_DEBUG(("%s: file:%s\n", __func__, tp));
    /*更改filename*/
    strncpy(sess->filename, tp, MAX_FILE_NAME_LEN);

    return;
}
#if 0
int32 urcp_tftp_send_data_request(uchar *macaddr, uchar* passwd, uint32 type, const uchar* buffer, uint32 len, uint32 op)
{
    urcp_tftp_data_request_t data_req; 

    data_req=data_req;

    return 0;
}
#endif
/***********************************************************************
 *函数名      urcp_tftp_read_request
 *功能        向远程交换机发送一个读请求包
 *参数        
 *            mac   远程交换机地址
 ************************************************************************/
static uint16 urcp_tftp_send_request(uchar* macaddr, uchar* passwd, tftp_opt_t* opt, uchar* pkt, uint32 len)
{
    uchar  buffer[URCP_BUF_SIZE];
    uchar *tmp=buffer;
    urcp_tftp_state_t status;
    urcp_tftp_session_info_t* sess=NULL;
    urcp_tftp_hdr_t* tftp_hdr=NULL;
    urcp_tftp_file_request_t* tftp_file_hdr;
	int iftag = 0;

    /*设置初始状态*/
    if((opt->op==TFTP_GET_FILE) || (opt->op==TFTP_GET_DATA))
    {
	/*read request*/
	status = WAIT_R_OACK;
    }
    else
    {
	/*write request*/
	status = WAIT_W_OACK;
    }
    /*分配会话*/
    urcp_tftp_session_alloc(macaddr, opt->op, status, &sess, opt->cb_func);
    if(sess!=NULL)
    {
	/*构建包头*/
	iftag = urcp_buid_header(buffer, macaddr, (uint16)URCP_TFTP, 0u, passwd);
	if(iftag == 1)
	{
		/*发送文件或数据信息*/
		tmp = &buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32)];
		len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32) + len; 
	}else{
		/*发送文件或数据信息*/
		tmp = &buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)];
		len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + len; 
	}
	/*包内容*/
	memcpy(tmp, pkt, len);
	tftp_hdr = (urcp_tftp_hdr_t*)tmp;
	/*本地会话id*/
	tftp_hdr->lid=htons(sess->local_id);
	tftp_hdr->chksum = chk_sum((uint16*)tmp, len);
	tftp_hdr->chksum =~(tftp_hdr->chksum);

	URCP_DEBUG(("%s: len:%d, sizeof(urcp_tftp_hdr_t):%d, chksum:0x%04x\n", __func__, len, sizeof(urcp_tftp_hdr_t), tftp_hdr->chksum));
	/*发送*/
//#ifdef URCP_WEB
//	h2_fwd_frame(buffer, len, sw_cpu_vid_get(), 0u);
//#else
//	h2_send_frame(buffer, len);
//#endif
	/*存储文件名*/
	if((opt->op==TFTP_GET_FILE) || (opt->op==TFTP_PUT_FILE))
	{
	    /*存储文件名*/
	    tftp_file_hdr = (urcp_tftp_file_request_t*)tmp;
	    tmp = tmp+sizeof(urcp_tftp_file_request_t);
	    memcpy(sess->filename, tmp, ntohl(tftp_file_hdr->name_len));
	    if(opt->op==TFTP_PUT_FILE)
	    {
		/*保存文件指针*/
		urcp_open_file(sess->filename, MAX_FILE_NAME_LEN, "rb", &(sess->fd));
	    }
	}

	h2_send_frame(buffer, len
#ifdef URCP_AC
				   , -1
#endif
		);
    }

    return (sess->local_id);
}
/*****************************************************
 * *函数名     urcp_tftp_process
 * *功能       收到urcp tftp 协议报文后的处理
 * *参数
 * *           urcp_infor  包含数据包起始地址，长度等信息
 * ****************************************************/
int32 urcp_tftp_process(urcp_infor_t* urcp_info)
{
    uchar *tmp;
    uint16 localid, remoteid, checksum;
    uint32 block, len, op_code;
    urcp_tftp_data_t* tftp_data;
    urcp_tftp_oack_hdr_t* tftp_oack_hdr;
    urcp_tftp_session_info_t* tftp_session;

    /*报文长度*/
    len = urcp_info->length;
    /*计算校验和*/
#if 1
    checksum = chk_sum((uint16*)(urcp_info->cmd_buff), len);
    checksum = ~checksum;
    if(checksum!=0u)
    {
	URCP_DEBUG(("%s: checksum error len:%d\n", __func__, len));
	return -1;
    }
#endif
    /*本地会话id*/
    localid = ntohs(urcp_info->urcp_header->request_id);
    /*报文类型*/
    op_code = ((urcp_tftp_hdr_t*)(urcp_info->cmd_buff))->opcode;
    op_code = ntohl(op_code);
    /*搜索会话id*/
    urcp_tftp_lookup_session(urcp_info->mac, localid, 0u, &tftp_session);
#if DEBUG_URCP
    if(tftp_session!=NULL)
    {
	//URCP_DEBUG(("%s: my state:%s\n", __func__, statestr[tftp_session->state]));
    }
#endif
    switch(op_code)
    {
	case URCP_TFTP_DATA:/*普通数据包*/
	    URCP_DEBUG(("%s: URCP_TFTP_DATA\n", __func__));
	    if(tftp_session!=NULL)
	    {
		/*取报文*/
		tftp_data = (urcp_tftp_data_t*)(urcp_info->cmd_buff);
		/*块号*/
		block = ntohl(tftp_data->block_num);
		URCP_DEBUG(("%s: block num:%d, expected:%d, total:%d\n", __func__, block, tftp_session->blockct+1u,tftp_session->block_total));
		if((block<=tftp_session->block_total)/*没超过最大块*/
			&& ( (tftp_session->state==WAIT_DATA)
			    || (tftp_session->state==WAIT_FIN)/*此状态也可接受数据*/
			   )
		  )
		{
		    /*是否期望报文*/
		    if(block == tftp_session->blockct+1u)/*block号正确*/
		    {
			/*block num++*/
			tftp_session->blockct = block;

			/*发送ack*/
			urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);

			/*数据长度*/
			len = ntohl(tftp_data->data_len);
			/*数据部分*/
			tmp = (uchar*)tftp_data;
			tmp = tmp + sizeof(urcp_tftp_data_t);
			/*保存数据*/
			urcp_tftp_save_data(tftp_session, tmp, len);

			/*是否发送完毕*/
			if((block==tftp_session->block_total)/*达到最大块则结束*/
                           || (len<tftp_session->block_size)/*如果收到块长度不够block_size则认为提前结束*/
			  )
			{
			    URCP_DEBUG(("%s: URCP_TFTP_DATA to WAIT_FIN\n", __func__));
			    db_info(URCP_MODULE_ID_TFTP, "%s: URCP_TFTP_DATA to WAIT_FIN\n", __func__);
			    /*the last block, waiting for fin*/
			    tftp_session->state = WAIT_FIN;
			}
		    }
		    else if(block == tftp_session->blockct)/*重发*/
		    {
			/*为已经确认过的块，可能是上次发送的ACK丢失了*/
			URCP_ERROR(("%s: URCP_TFTP_DATA retransmission for block:%d\n", __func__, block));
			db_info(URCP_MODULE_ID_TFTP, "%s: get DATA retry for block:%d %s\n", __func__,block, 
				str_macaddr(tftp_session->mac));
			/*block号正确*/
			urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);
		    }
		    else
		    {
			URCP_ERROR(("%s: block num:%d, expected:%d\n", __func__, block, tftp_session->blockct+1u));
			break;
		    }
		    /*复位*/
		    tftp_session->timer = 0;
		}
#if DEBUG_URCP
		else
		{
		    URCP_DEBUG(("%s: URCP_TFTP_DATA block or state error\n", __func__));
		}
#endif
	    }
#if DEBUG_URCP
	    else
	    {
		URCP_DEBUG(("%s: lookup session failed\n", __func__));
	    }
#endif
	    break;
	case URCP_TFTP_ACK:/*回复*/
	    URCP_DEBUG(("%s: URCP_TFTP_ACK\n", __func__));
	    if(tftp_session!=NULL)/*已经找到*/
	    {
		tftp_data = (urcp_tftp_data_t*)(urcp_info->cmd_buff);
		block = ntohl(tftp_data->block_num);
		URCP_DEBUG(("%s: ACK block:%d\n", __func__, block));
		if(block==tftp_session->blockct)/*block有效*/
		{
		    /*为等待ack状态*/
		    if(tftp_session->state==WAIT_ACK)
		    {
			tftp_session->retries = 0u;
			tftp_session->timer = 0;
			if(block<tftp_session->block_total)
			{
			    /*数据尚未发送完毕,继续发送*/
			    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_DATA);
			}
			else/*收到最后一个包的确认*/
			{
			    /*发送FIN(ACK)*/
			    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);
			    URCP_DEBUG(("%s: send data over\n", __func__));
			    db_info(URCP_MODULE_ID_TFTP, "%s: get last ack: %d from: %s\n", __func__, block, str_macaddr(tftp_session->mac));

			    /*数据全部发送成功，注销会话*/
			    urcp_tftp_session_clear(tftp_session, URCP_STATUS_OK);
			}
		    }
		    else if(tftp_session->state==WAIT_OACK_ACK)
		    {
			/*发送数据集*/
			urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_DATA);
			if(tftp_session->last_pkt!=NULL)/*分配空间成功*/
			{
			    /*复位*/
			    tftp_session->timer = 0;
			    tftp_session->state=WAIT_ACK;
			}
		    }
		    else if(tftp_session->state==WAIT_FIN)
		    {
			tftp_session->retries = 0u;
			/*会话成功结束，保存数据*/
			URCP_DEBUG(("%s: the session sucessful\n", __func__));
			db_info(URCP_MODULE_ID_TFTP, "%s: the session sucessful\n", __func__);
#if DEBUG_URCP
                        printf_time();
#endif
			/*数据全部发送成功，注销会话*/
			urcp_tftp_session_clear(tftp_session, URCP_STATUS_OK);
		    }
		}
#ifndef URCP_REL
		else
		{
		    db_warn(URCP_MODULE_ID_TFTP, "%s: error ack for block:%d, expected:%d from %s\n", __func__, block, tftp_session->blockct, str_macaddr(tftp_session->mac));
		}
#endif
	    }
	    break;
	case URCP_TFTP_OACK:/*request ack*/
	    URCP_DEBUG(("%s: URCP_TFTP_OACK\n", __func__));
	    db_info(URCP_MODULE_ID_TFTP, "%s: URCP_TFTP_OACK\n", __func__);
	    if(tftp_session!=NULL)/*已经找到*/
	    {
		/*远端ID*/
                tftp_session->remote_id=ntohs(((urcp_tftp_hdr_t*)(urcp_info->cmd_buff))->lid);
		tftp_oack_hdr = (urcp_tftp_oack_hdr_t*)(urcp_info->cmd_buff);
		tftp_session->block_size = ntohl(tftp_oack_hdr->blksize);
		/*长度*/
		len = ntohl(tftp_oack_hdr->length); 
		URCP_DEBUG(("%s: len:%d\n", __func__, len));
		/*计算块*/
		tftp_session->block_total=len/(tftp_session->block_size);
		tftp_session->last_block_size=len%(tftp_session->block_size);
		if(tftp_session->last_block_size!=0u)
		{
                    tftp_session->block_total = tftp_session->block_total+1u;
		}
		tftp_session->timer = 0;
		tftp_session->fromL3 = urcp_info->fromL3;
		if(tftp_session->state==WAIT_R_OACK) /*发送过读请求*/
		{
		    if(tftp_session->sess_type==TFTP_GET_FILE)/*读文件*/
		    {
			/*设置文件名*/
			set_file_name(urcp_info, tftp_session);
			/*创建接收文件*/
			urcp_open_file(tftp_session->filename, MAX_FILE_NAME_LEN, "wb", &(tftp_session->fd));
			if(tftp_session->fd==NULL)
			{
			    break;
			}
		    }
		    else if(tftp_session->sess_type==TFTP_GET_DATA)/*读取数据*/
		    {
			/*分配内存*/
			tftp_session->buffer = (uchar*)malloc(len);
			if(tftp_session->buffer==NULL)
			{
			    break;
			}
		    }
		    else
		    {
			break;
		    }
		    /*发送ack*/
		    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);
		    /*复位*/
		    tftp_session->state=WAIT_DATA;
		    URCP_DEBUG(("%s: total:%d\n", __func__, tftp_session->block_total));
		}
		else if(tftp_session->state==WAIT_W_OACK)
		{
		    /*发送数据集*/
		    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_DATA);
		    if(tftp_session->last_pkt!=NULL)/*分配空间成功*/
		    {
			/*复位*/
			tftp_session->state=WAIT_ACK;
		    }
		}
		else
		{
		    /*跑到这里是不对的*/
		}
	    }
	    break;
	case URCP_TFTP_READ_FILE:/*read file*/
	    URCP_DEBUG(("%s: URCP_TFTP_READ_FILE\n", __func__));
//	    db_info(URCP_MODULE_ID_TFTP, "%s: URCP_TFTP_READ_FILE\n", __func__);
	    if((tftp_session==NULL) /*&& (urcp_info->certified==1)*/)/*该会话不存在*/
	    {
		/*判断有没有remote id相同的,避免在回环时不停创建会话*/
		remoteid = ((urcp_tftp_hdr_t*)(urcp_info->cmd_buff))->lid;
		remoteid = ntohs(remoteid);
		urcp_tftp_lookup_session(urcp_info->mac, 0, remoteid, &tftp_session);
		if(tftp_session==NULL)
		{
		    urcp_tftp_rev_file_request(urcp_info, op_code);
		}
	    }
	    break;
	case URCP_TFTP_WRITE_FILE:/*write file*/
	    URCP_DEBUG(("%s: URCP_TFTP_WRITE_FILE\n", __func__));
	    db_info(URCP_MODULE_ID_TFTP, "%s: URCP_TFTP_READ_FILE\n", __func__);
	    if((tftp_session==NULL) /*&& (urcp_info->certified==1)*/)/*该会话不存在*/
	    {
		/*判断有没有remote id相同的,避免在回环时不停创建会话*/
		remoteid = ((urcp_tftp_hdr_t*)(urcp_info->cmd_buff))->lid;
		remoteid = ntohs(remoteid);
		urcp_tftp_lookup_session(urcp_info->mac, 0, remoteid, &tftp_session);
		if(tftp_session==NULL)
		{
		    urcp_tftp_rev_file_request(urcp_info, op_code);
		}
	    }
	    break;
	case URCP_TFTP_READ_DATA:/*read data*/
	    URCP_DEBUG(("%s: URCP_TFTP_READ_DATA\n", __func__));
	    break;
	case URCP_TFTP_WRITE_DATA:/*write data*/
	    URCP_DEBUG(("%s: URCP_TFTP_WRITE_DATA\n", __func__));
	    break;
	case URCP_TFTP_ERROR:/*error*/
	    break;
	default:/*unknow or error*/
	    URCP_DEBUG(("%s: type unknow\n", __func__));
	    break;
    }

    return 0;
}
/*********************************************************
*函数名    urcp_tftp_poll
*功能      定时更新会话状态,对超时会话做出注销或重发处理
*参数      无
*********************************************************/
void urcp_tftp_poll(void)
{
    uint32 i;
    urcp_tftp_session_info_t* sess;

    /*依次查询所有会话*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	/*非空闲会话*/
	if(urcp_tftp_session.session[i].state!=IDLE)
	{
	    sess = &urcp_tftp_session.session[i];
	    /*时间*/
	    if(sess->timer>TFTP_MAX_TIMEOUT_MS)/*严重超时*/
	    {
		db_info(URCP_MODULE_ID_TFTP, "%s: max timeout:%s\n", __func__, str_macaddr(sess->mac));
		//URCP_DEBUG(("%s: session max time out, state:%s\n", __func__, statestr[sess->state]));
#if 0
	        switch (sess->state)
		{
		    case WAIT_DATA:/*等待数据*/
		    case WAIT_R_OACK: /*等待对读请求的确认*/
		    case WAIT_W_OACK: /*等待对写请求的确认*/
		    case WAIT_OACK_ACK:/*收到读请求后，已经发送了ACK，等待确认*/
		    default:
			break;
		}
#endif
                /*注销会话*/
                urcp_tftp_session_clear(sess, URCP_STATUS_TIME_OUT);
	    }
	    else if(sess->timer>TFTP_TIMEOUT_MS)
	    {
		//URCP_ERROR(("%s: session time out, state:%s\n", __func__, statestr[sess->state]));
		/*判断会话状态*/
		switch(sess->state)
		{
		    case WAIT_ACK:
			if(sess->retries<TFTP_MAX_RETRIES)
			{
			    /*重传*/
//#ifdef URCP_WEB
//			    h2_fwd_frame(sess->last_pkt, sess->last_pkt_len, sw_cpu_vid_get(), 0u);
//#else
			    h2_send_frame(sess->last_pkt, sess->last_pkt_len
#ifdef URCP_AC
				   , sess->fromL3
#endif
				    );
//#endif
			    sess->retries = sess->retries + 1u;
			    sess->timer = 0;
			    URCP_ERROR(("%s: retransmission\n", __func__));
			    db_info(URCP_MODULE_ID_TFTP, "%s: %d retry block %d to %s\n", __func__, sess->retries, 
				    sess->blockct, str_macaddr(sess->mac));
			}
			else
			{
			    db_info(URCP_MODULE_ID_TFTP, "%s: session timeout\n", __func__);
			    /*超过最大重传次数,注销会话*/
			    urcp_tftp_session_clear(sess, URCP_STATUS_TIME_OUT);
			}
			break;
		    case WAIT_FIN:
                        /*注销会话*/
                        urcp_tftp_session_clear(sess, URCP_STATUS_OK);
			db_info(URCP_MODULE_ID_TFTP, "%s: wait_fin timeout\n",__func__);
			break;
#if 0
		    case WAIT_R_OACK:
		    case WAIT_W_OACK:
		    case WAIT_OACK_ACK:
                        /*注销会话*/
			db_info(URCP_MODULE_ID_TFTP, "%s: wait oack or ack timeout:%s\n",__func__,str_macaddr(sess->mac));
			break;
#endif
		    default:
			URCP_ERROR(("%s: time out default\n", __func__));
			db_info(URCP_MODULE_ID_TFTP, "%s: %s timeout, state:%d, block:%d\n",__func__, 
				str_macaddr(sess->mac), sess->state, sess->blockct);
			/*计数加1*/
			sess->timer = sess->timer + 1;
			break;
		}
	    }
	    else
	    {
		/*计数加1*/
                sess->timer = sess->timer + 1;
	    }
	}
    }

    return;
}

