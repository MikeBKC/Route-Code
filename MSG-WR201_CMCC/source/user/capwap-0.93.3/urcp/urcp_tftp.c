/*
 *����ʵ���ļ�����
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
*������      urcp_tftp_session_alloc
*����        ����һ���Ự
*����        macaddr         �ỰԶ�̽�����MAC��ַ
*            session_type    �Ự���ͣ���ȡ�ļ�����ͨ����(�������ϴ������)
*            session_status  ���ûỰ��ʼ״̬ 
*            ses_id          ���ڷ��ػỰid
***********************************************************************/
static uint16 urcp_tftp_session_alloc(uchar *macaddr,urcp_tftp_sess_type_t session_type,urcp_tftp_state_t session_status,urcp_tftp_session_info_t**sess, cb_tftp cb_func)
{
    uint32 i;
    uint16 ret_id=0u;

    *sess=NULL;
    /*�������лỰ*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	/*ȡһ�����лỰ*/
	if(urcp_tftp_session.session[i].state==IDLE)
	{
	    URCP_DEBUG(("%s: id:%d\n", __func__, i));
	    /*���е�*/
	    *sess=&urcp_tftp_session.session[i];
	    break;
	}
    }

    /*�Ƿ��ҵ����лỰ*/
    if(*sess!=NULL)
    {
	/*�ҵ���*/
	memset(*sess, 0, sizeof(struct _urcp_tftp_session_info_t));
	/*��ʼ��*/
	if(urcp_tftp_session.id_counter==0u)
	{
	    /*�Ựid��1��ʼ, 0��ʾ�Ự��ʼ����֪���Է�id*/
	    urcp_tftp_session.id_counter = urcp_tftp_session.id_counter + 1u;
	}
	(*sess)->local_id = urcp_tftp_session.id_counter;
	(*sess)->sess_type = session_type;
	(*sess)->state = session_status;
	(*sess)->cb = cb_func;
	memcpy((*sess)->mac, macaddr, MAC_LEN);

	URCP_DEBUG(("%s: local id:%d\n", __func__, (*sess)->local_id));
        db_warn(URCP_MODULE_ID_TFTP, "%s: local id:%d\n", __func__, (*sess)->local_id);
	/*�Ựid��1*/
	urcp_tftp_session.id_counter = urcp_tftp_session.id_counter + 1u;
       
	ret_id = (*sess)->local_id;
    }
    else
    {
	/*����ʧ��*/
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
 *��ʼ��
 *KeHuatao
 */
void urcp_tftp_init(void)
{
    char tp[64];

    memset(tp, 0, sizeof(tp));
    memset(&urcp_tftp_session, 0, sizeof(urcp_tftp_session_t));
    /*����configs�ļ���*/
    sprintf(tp, "mkdir %s -p", RAM_CONFIG_PATH);
    system(tp);

    sprintf(tp, "mkdir %s -p", TFTP_LOG_ROOT);
    system(tp);

    return;
}

/*
 *���ļ�
 *KeHuatao
 */
void urcp_open_file(uchar* filename, uint32 len, const uchar* mode, FILE**fp)
{
    uchar tp[128]={0};

    /*��ȫ·��*/
    strcpy(tp, TFTP_ROOT);
    /*�����ļ���*/
    strncat(tp, filename, len);
    /*���ļ�*/
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
        case PORT_CONFIG:/*��ȡ�����ļ�*/
	    create_nvram_file();
	    *p = (uchar*)(CONFIG_FILE);
	    db_info(URCP_MODULE_ID_TFTP, "AP request profile\n");
	    break;
	case PORT_LOG:/*��ȡ��־�ļ�*/
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
	default:/*δ֪*/
	    break;
    }

    return;
}    

/*
 *�յ���д�ļ�������Ĵ���
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
    /*��ȡ�ļ�������*/
    tmp = (uchar*)(urcp_info->cmd_buff) + sizeof(urcp_tftp_file_request_t);
    len = ntohl(tftp_file_hdr ->name_len);

    /*���ļ�*/
    if(op==URCP_TFTP_READ_FILE)
    {
	memset(tp, 0, sizeof(tp));
	create_file(ntohl(tftp_file_hdr->tftp_hdr.remote_port), &p, urcp_info->mac);
	if(strcmp(p, "")==0)/*����Ϊ��ͨ�ļ�*/
	{
	    /*�����ļ���*/
	    strncpy(p, tmp, len);
	}
	/*��Ҫ�����ļ�*/
	db_info(URCP_MODULE_ID_TFTP, "%s to request file:%s\n", str_macaddr(urcp_info->mac), p);
        urcp_open_file(p, strlen(p), "rb", &fp);
	st = WAIT_OACK_ACK;
	ty = TFTP_SEND_FILE;
    }
    else
    {
	/*���������ļ�*/
        urcp_open_file(tmp, len, "wb", &fp);
	st = WAIT_DATA;
	ty = TFTP_REV_FILE;
    }

    if(fp!=NULL)
    {
	/*����Ự*/
	urcp_tftp_session_alloc(urcp_info->mac, ty, st, &tftp_session, (cb_tftp)urcp_rev_conf_cb);
	if(tftp_session!=NULL)/*�ûỰ������*/
	{
	    blk_size = ntohl(tftp_file_hdr->blksize);
	    /*ȷblock��С*/
	    if(blk_size>TFTP_BLOCK_LEN)
	    {
		blk_size=TFTP_BLOCK_LEN;
	    }
	    /*�ļ����ڣ������ļ�����*/
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
		/*�����ļ���*/
		tmp = tmp + len;
		/*ȡ�ļ�����*/
		len = *((uint32*)tmp);
		len = ntohl(len);
	    }

	    /*��������*/
	    tftp_session->remote_id = ntohs(tftp_file_hdr->tftp_hdr.lid);
	    tftp_session->tftp_port = ntohl(tftp_file_hdr->tftp_hdr.remote_port);
	    tftp_session->fd = fp;
	    tftp_session->block_size = blk_size;
	    tftp_session->block_total = len/blk_size;
	    tftp_session->last_block_size = len%blk_size;
	    tftp_session->fromL3 = urcp_info->fromL3;
	    URCP_DEBUG(("%s: file len:%d, blksize:%d, port:%d\n", __func__, len, blk_size, (tftp_session->tftp_port)));
	    /**������С*/
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

    /*���лỰ*/
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
 * ������лỰ
*
* KeHuatao  
*/
void urcp_tftp_session_clear_all(void)
{
    uint32 i;

    /*���лỰ*/
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
 *���һ���Ự���ͷ��ڴ���ļ�ָ��
 *KeHuatao
 *
 */
static void urcp_tftp_session_clear(urcp_tftp_session_info_t* sess, urcp_status_t urcp_state)
{
    URCP_DEBUG(("%s: clear session:%d\n", __func__, sess->local_id));
    /*״̬��Ϊ����*/
    sess->state=IDLE;

    /*�ͷŻ�����*/
    if(sess->fd!=NULL)
    {
	fclose(sess->fd);
	URCP_DEBUG(("%s: fclose fd\n", __func__));
    }

    /*�ͷŻ�����*/
    if(sess->buffer!=NULL)
    {
	free(sess->buffer);
	URCP_DEBUG(("%s: free buffer\n", __func__));
    }

    /*�ͷŻ�����*/
    if(sess->last_pkt!=NULL)
    {
	free(sess->last_pkt);
	URCP_DEBUG(("%s: free last pkts\n", __func__));
    }

    /*ָ��ص�����*/
    if(sess->cb!=NULL)
    {
        sess->cb((void*)sess, urcp_state);
    }

    return;
}

/*************************************
 *�����Ự���ж�ĳ��id�Ự�Ƿ����
 *KeHuatao
 *
 ************************************/
static void urcp_tftp_lookup_session(uchar *macaddr, uint16 lId, uint16 rId, urcp_tftp_session_info_t** sess)
{
    uint32 i;

    URCP_DEBUG(("%s: lId:%d, rId:%d\n", __func__, lId, rId));
    *sess=NULL;
    /*�������лỰ*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	/*id��ȣ��ҷǿ��еĻỰ*/
	if((urcp_tftp_session.session[i].state!=IDLE)
            && ((lId==0u) || (urcp_tftp_session.session[i].local_id==lId))/*local id*/
            && ((rId==0u) || (urcp_tftp_session.session[i].remote_id==rId))/*remote id*/
	    && (mac_cmp((uchar*)(urcp_tftp_session.session[i].mac), macaddr)==0)
	)
	{
	    /*���ػỰ�ṹ*/
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
 *����ackȷ��
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
    /*ͷ��*/
    switch(type)
    {
	case URCP_TFTP_ACK:/*����ACK*/
	    URCP_DEBUG(("%s:URCP_TFTP_ACK\n", __func__));
	    /*��ʼ��*/
	    memset(buf, 0, sizeof(buf));
		if(iftag == 1)
		{
		    len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_ack_t) + sizeof(uint32);

		    /*����������*/
		    tftp_ack = (urcp_tftp_ack_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32)]);
		}else{
			len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(urcp_tftp_ack_t);

		    /*����������*/
		    tftp_ack = (urcp_tftp_ack_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
		}
	    tftp_ack->block_num=htonl(tftp_info->blockct);
	    break;
	case URCP_TFTP_DATA:/*������һ������*/
	    URCP_DEBUG(("%s:URCP_TFTP_DATA\n", __func__));
	    if(tftp_info->last_pkt==NULL)/*��δ����ռ�*/
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
		    /*����ʧ��*/
		    break;
		}
		tftp_info->last_pkt_len = len;
	    }
	    /*block num++*/
	    tftp_info->blockct = tftp_info->blockct + 1u;

	    tmp = tftp_info->last_pkt;
	    memset(tmp, 0, tftp_info->last_pkt_len);
	    /*����Ϣ*/
		if(iftag == 1)
		{
			tftp_data = (urcp_tftp_data_t*)(&tmp[ETH_URCP_OFFSET + sizeof(uint32) + sizeof(urcp_hdr_t)]);
		}else{
			tftp_data = (urcp_tftp_data_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
		}
	    tftp_data->block_num=htonl(tftp_info->blockct);
	    /*ȷ������С*/
	    if(tftp_info->blockct<tftp_info->block_total)
	    {
		/*���ݰ���С*/
		len = tftp_info->block_size;
	    }
	    else
	    {
		/*���һ�����ݰ����ܳ��Ȳ���*/
		len = tftp_info->last_block_size;
	    }

	    URCP_DEBUG(("%s: send block:%d\n", __func__, tftp_info->blockct));
	    /*����Ҫ���͵�����*/
	    if(tftp_info->fd!=NULL)
	    {
		/*�����ļ�*/
		URCP_DEBUG(("%s: now read file len:%d\n", __func__, len));
		if(fread(((uchar*)tftp_data+sizeof(urcp_tftp_data_t)), 1, len, tftp_info->fd)!=len)
		{
		    /*���Ȳ���*/
		    if(feof(tftp_info->fd)!=0)
		    {
			/*����,�˳�*/
			len=0u;
			urcp_tftp_session_clear(tftp_info, URCP_STATUS_FAIL);
			URCP_DEBUG(("%s: read file failed\n", __func__));
			break;
		    }
		}
	    }
	    else if(tftp_info->buffer!=NULL)
	    {
		/*��������*/
		/*��������*/
		memcpy((uchar*)(tftp_data+sizeof(urcp_tftp_data_t)), tftp_info->buffer_ptr, len);
		tftp_info->buffer_ptr = tftp_info->buffer_ptr + len;
	    }
	    else/*��Ӧ���ߵ�����*/
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
	case URCP_TFTP_OACK:/*����OACK*/
	    URCP_DEBUG(("%s:URCP_TFTP_OACK, sess type:%d\n", __func__, tftp_info->sess_type));
	    db_info(URCP_MODULE_ID_TFTP, "%s: oack to %s\n", __func__,str_macaddr(tftp_info->mac));
	    /*��ʼ��*/
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
	    if(tftp_info->sess_type==TFTP_SEND_FILE)/*Ҫ���ļ�*/
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
	case URCP_TFTP_ERROR:/*���ʹ�����Ϣ*/
	    /**/
	    break;
	default:/*δ֪*/
	    break;
    }

    if(len>0u)
    {
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
		urcp_buid_tftp_header(tmp, tftp_info->mac, (uint16)URCP_TFTP, tftp_info->remote_id, NULL, vid);
		if(iftag == 1)
		{
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
			/*��дtftp���ݲ���*/
			tftp_hdr = (urcp_tftp_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32)]);
		}else{
		URCP_DEBUG(("%s: %d\n", __func__,__LINE__));
			/*��дtftp���ݲ���*/
			tftp_hdr = (urcp_tftp_hdr_t*)(&tmp[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)]);
	}
	tftp_hdr->opcode=htonl(type);
	tftp_hdr->lid=htons(tftp_info->local_id);
#if 1
	/*У���*/
	if(iftag == 1)
	{
		tftp_hdr->chksum=chk_sum((uint16*)tftp_hdr, len-ETH_URCP_OFFSET-sizeof(urcp_hdr_t) - sizeof(uint32));
	}else{
		tftp_hdr->chksum=chk_sum((uint16*)tftp_hdr, len-ETH_URCP_OFFSET-sizeof(urcp_hdr_t));
	}
	tftp_hdr->chksum=~(tftp_hdr->chksum);
	URCP_DEBUG(("%s: chk len:%d, chksum:%08x\n", __func__, len-ETH_URCP_OFFSET-sizeof(urcp_hdr_t), tftp_hdr->chksum));
#endif
	/*����*/
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
 *�����յ�����
 *KeHuatao
 *
 **/
static void urcp_tftp_save_data(urcp_tftp_session_info_t* tftp_session, uchar *tftp_data, uint32 len)
{
    if((tftp_session->sess_type==TFTP_GET_FILE) || (tftp_session->sess_type==TFTP_REV_FILE))
    {
	/*д�ļ�*/
	if(tftp_session->fd!=NULL)
	{
	    URCP_DEBUG(("%s\n",__func__));
	    /*д���ļ�*/
	    fwrite(tftp_data, 1, len, tftp_session->fd);
	}
    }
    else if(tftp_session->sess_type==TFTP_GET_DATA)
    {
	/*д����*/
	if(tftp_session->buffer_ptr!=NULL)
	{
	    /*��������*/
	    memcpy(tftp_session->buffer_ptr, tftp_data, len);
	    /*�ƶ�дָ��*/
	    tftp_session->buffer_ptr = tftp_session->buffer_ptr + len;
	}
    }

    return;
}
/*
 *���Ͷ��ļ����� 
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
    /*ͷ������*/
    memset(buf, 0, sizeof(buf));
    req = (urcp_tftp_file_request_t*)tmp;
    req->blksize = htonl(TFTP_BLOCK_LEN);
    /*�ļ�������*/
    l = strlen(opt->op_file_name);
    if(l>MAX_FILE_NAME_LEN)
    {
        return 0u;
    }
    req->name_len = htonl(l);
    /*�ļ���*/
    tmp = &tmp[sizeof(urcp_tftp_file_request_t)];
    strcpy(tmp, opt->op_file_name);
	DBCK("op_file_name = %s", tmp);
    req->tftp_hdr.remote_port=htonl(opt->rport);
    /*����opcode*/
    if(opt->op==TFTP_PUT_FILE)
    {
	req->tftp_hdr.opcode=htonl(URCP_TFTP_WRITE_FILE);

	/*�ļ�����*/
	k = get_conf_file_size(opt->op_file_name);
	if(k>0)
	{
	    URCP_DEBUG(("%s: put file len:%d\n", __func__, k));
	    p = (uint32*)(&tmp[l]);
	    /*д�ļ�����*/
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
	/*������urcp_tftp_hdr_t�ĳ���*/
	l = l+sizeof(urcp_tftp_file_request_t);
	URCP_DEBUG(("%s: header len:%d\n", __func__, l));
	/*��������*/
	id1 = urcp_tftp_send_request(macaddr, passwd, opt, buf, l);
    }

    return id1;
}
/*
 *�������ļ��� ip+ʱ�� ��֤�ļ���Ψһ�Ժ��������
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
    /*�����ļ���*/
    strncpy(tp, sess->filename, MAX_FILE_NAME_LEN);
#ifdef URCP_WEB
    /*ip*/
    tmp = ntohl(urcp_info->urcp_header->ip);
    /*����*/
    dev=urcp_dev_table_lookup(urcp_info->mac, tmp);
    if(dev!=NULL)
    {
	/*���ͺ���Ϣд���ļ���*/
	strncat(tp, dev->dev_model, DEV_TYPE_SIZE);
    }
#else
    strncat(tp, "AP", DEV_TYPE_SIZE);
#endif

    /*�ָ���*/
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

    /*����*/
    l = strlen(tp);
    /*��ȡ��ǰʱ��*/
    time(&timep);
    sprintf(&tp[l], ":%d", (int)timep);
    
    URCP_DEBUG(("%s: file:%s\n", __func__, tp));
    /*����filename*/
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
 *������      urcp_tftp_read_request
 *����        ��Զ�̽���������һ���������
 *����        
 *            mac   Զ�̽�������ַ
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

    /*���ó�ʼ״̬*/
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
    /*����Ự*/
    urcp_tftp_session_alloc(macaddr, opt->op, status, &sess, opt->cb_func);
    if(sess!=NULL)
    {
	/*������ͷ*/
	iftag = urcp_buid_header(buffer, macaddr, (uint16)URCP_TFTP, 0u, passwd);
	if(iftag == 1)
	{
		/*�����ļ���������Ϣ*/
		tmp = &buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32)];
		len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + sizeof(uint32) + len; 
	}else{
		/*�����ļ���������Ϣ*/
		tmp = &buffer[ETH_URCP_OFFSET + sizeof(urcp_hdr_t)];
		len = ETH_URCP_OFFSET + sizeof(urcp_hdr_t) + len; 
	}
	/*������*/
	memcpy(tmp, pkt, len);
	tftp_hdr = (urcp_tftp_hdr_t*)tmp;
	/*���ػỰid*/
	tftp_hdr->lid=htons(sess->local_id);
	tftp_hdr->chksum = chk_sum((uint16*)tmp, len);
	tftp_hdr->chksum =~(tftp_hdr->chksum);

	URCP_DEBUG(("%s: len:%d, sizeof(urcp_tftp_hdr_t):%d, chksum:0x%04x\n", __func__, len, sizeof(urcp_tftp_hdr_t), tftp_hdr->chksum));
	/*����*/
//#ifdef URCP_WEB
//	h2_fwd_frame(buffer, len, sw_cpu_vid_get(), 0u);
//#else
//	h2_send_frame(buffer, len);
//#endif
	/*�洢�ļ���*/
	if((opt->op==TFTP_GET_FILE) || (opt->op==TFTP_PUT_FILE))
	{
	    /*�洢�ļ���*/
	    tftp_file_hdr = (urcp_tftp_file_request_t*)tmp;
	    tmp = tmp+sizeof(urcp_tftp_file_request_t);
	    memcpy(sess->filename, tmp, ntohl(tftp_file_hdr->name_len));
	    if(opt->op==TFTP_PUT_FILE)
	    {
		/*�����ļ�ָ��*/
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
 * *������     urcp_tftp_process
 * *����       �յ�urcp tftp Э�鱨�ĺ�Ĵ���
 * *����
 * *           urcp_infor  �������ݰ���ʼ��ַ�����ȵ���Ϣ
 * ****************************************************/
int32 urcp_tftp_process(urcp_infor_t* urcp_info)
{
    uchar *tmp;
    uint16 localid, remoteid, checksum;
    uint32 block, len, op_code;
    urcp_tftp_data_t* tftp_data;
    urcp_tftp_oack_hdr_t* tftp_oack_hdr;
    urcp_tftp_session_info_t* tftp_session;

    /*���ĳ���*/
    len = urcp_info->length;
    /*����У���*/
#if 1
    checksum = chk_sum((uint16*)(urcp_info->cmd_buff), len);
    checksum = ~checksum;
    if(checksum!=0u)
    {
	URCP_DEBUG(("%s: checksum error len:%d\n", __func__, len));
	return -1;
    }
#endif
    /*���ػỰid*/
    localid = ntohs(urcp_info->urcp_header->request_id);
    /*��������*/
    op_code = ((urcp_tftp_hdr_t*)(urcp_info->cmd_buff))->opcode;
    op_code = ntohl(op_code);
    /*�����Ựid*/
    urcp_tftp_lookup_session(urcp_info->mac, localid, 0u, &tftp_session);
#if DEBUG_URCP
    if(tftp_session!=NULL)
    {
	//URCP_DEBUG(("%s: my state:%s\n", __func__, statestr[tftp_session->state]));
    }
#endif
    switch(op_code)
    {
	case URCP_TFTP_DATA:/*��ͨ���ݰ�*/
	    URCP_DEBUG(("%s: URCP_TFTP_DATA\n", __func__));
	    if(tftp_session!=NULL)
	    {
		/*ȡ����*/
		tftp_data = (urcp_tftp_data_t*)(urcp_info->cmd_buff);
		/*���*/
		block = ntohl(tftp_data->block_num);
		URCP_DEBUG(("%s: block num:%d, expected:%d, total:%d\n", __func__, block, tftp_session->blockct+1u,tftp_session->block_total));
		if((block<=tftp_session->block_total)/*û��������*/
			&& ( (tftp_session->state==WAIT_DATA)
			    || (tftp_session->state==WAIT_FIN)/*��״̬Ҳ�ɽ�������*/
			   )
		  )
		{
		    /*�Ƿ���������*/
		    if(block == tftp_session->blockct+1u)/*block����ȷ*/
		    {
			/*block num++*/
			tftp_session->blockct = block;

			/*����ack*/
			urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);

			/*���ݳ���*/
			len = ntohl(tftp_data->data_len);
			/*���ݲ���*/
			tmp = (uchar*)tftp_data;
			tmp = tmp + sizeof(urcp_tftp_data_t);
			/*��������*/
			urcp_tftp_save_data(tftp_session, tmp, len);

			/*�Ƿ������*/
			if((block==tftp_session->block_total)/*�ﵽ���������*/
                           || (len<tftp_session->block_size)/*����յ��鳤�Ȳ���block_size����Ϊ��ǰ����*/
			  )
			{
			    URCP_DEBUG(("%s: URCP_TFTP_DATA to WAIT_FIN\n", __func__));
			    db_info(URCP_MODULE_ID_TFTP, "%s: URCP_TFTP_DATA to WAIT_FIN\n", __func__);
			    /*the last block, waiting for fin*/
			    tftp_session->state = WAIT_FIN;
			}
		    }
		    else if(block == tftp_session->blockct)/*�ط�*/
		    {
			/*Ϊ�Ѿ�ȷ�Ϲ��Ŀ飬�������ϴη��͵�ACK��ʧ��*/
			URCP_ERROR(("%s: URCP_TFTP_DATA retransmission for block:%d\n", __func__, block));
			db_info(URCP_MODULE_ID_TFTP, "%s: get DATA retry for block:%d %s\n", __func__,block, 
				str_macaddr(tftp_session->mac));
			/*block����ȷ*/
			urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);
		    }
		    else
		    {
			URCP_ERROR(("%s: block num:%d, expected:%d\n", __func__, block, tftp_session->blockct+1u));
			break;
		    }
		    /*��λ*/
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
	case URCP_TFTP_ACK:/*�ظ�*/
	    URCP_DEBUG(("%s: URCP_TFTP_ACK\n", __func__));
	    if(tftp_session!=NULL)/*�Ѿ��ҵ�*/
	    {
		tftp_data = (urcp_tftp_data_t*)(urcp_info->cmd_buff);
		block = ntohl(tftp_data->block_num);
		URCP_DEBUG(("%s: ACK block:%d\n", __func__, block));
		if(block==tftp_session->blockct)/*block��Ч*/
		{
		    /*Ϊ�ȴ�ack״̬*/
		    if(tftp_session->state==WAIT_ACK)
		    {
			tftp_session->retries = 0u;
			tftp_session->timer = 0;
			if(block<tftp_session->block_total)
			{
			    /*������δ�������,��������*/
			    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_DATA);
			}
			else/*�յ����һ������ȷ��*/
			{
			    /*����FIN(ACK)*/
			    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);
			    URCP_DEBUG(("%s: send data over\n", __func__));
			    db_info(URCP_MODULE_ID_TFTP, "%s: get last ack: %d from: %s\n", __func__, block, str_macaddr(tftp_session->mac));

			    /*����ȫ�����ͳɹ���ע���Ự*/
			    urcp_tftp_session_clear(tftp_session, URCP_STATUS_OK);
			}
		    }
		    else if(tftp_session->state==WAIT_OACK_ACK)
		    {
			/*�������ݼ�*/
			urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_DATA);
			if(tftp_session->last_pkt!=NULL)/*����ռ�ɹ�*/
			{
			    /*��λ*/
			    tftp_session->timer = 0;
			    tftp_session->state=WAIT_ACK;
			}
		    }
		    else if(tftp_session->state==WAIT_FIN)
		    {
			tftp_session->retries = 0u;
			/*�Ự�ɹ���������������*/
			URCP_DEBUG(("%s: the session sucessful\n", __func__));
			db_info(URCP_MODULE_ID_TFTP, "%s: the session sucessful\n", __func__);
#if DEBUG_URCP
                        printf_time();
#endif
			/*����ȫ�����ͳɹ���ע���Ự*/
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
	    if(tftp_session!=NULL)/*�Ѿ��ҵ�*/
	    {
		/*Զ��ID*/
                tftp_session->remote_id=ntohs(((urcp_tftp_hdr_t*)(urcp_info->cmd_buff))->lid);
		tftp_oack_hdr = (urcp_tftp_oack_hdr_t*)(urcp_info->cmd_buff);
		tftp_session->block_size = ntohl(tftp_oack_hdr->blksize);
		/*����*/
		len = ntohl(tftp_oack_hdr->length); 
		URCP_DEBUG(("%s: len:%d\n", __func__, len));
		/*�����*/
		tftp_session->block_total=len/(tftp_session->block_size);
		tftp_session->last_block_size=len%(tftp_session->block_size);
		if(tftp_session->last_block_size!=0u)
		{
                    tftp_session->block_total = tftp_session->block_total+1u;
		}
		tftp_session->timer = 0;
		tftp_session->fromL3 = urcp_info->fromL3;
		if(tftp_session->state==WAIT_R_OACK) /*���͹�������*/
		{
		    if(tftp_session->sess_type==TFTP_GET_FILE)/*���ļ�*/
		    {
			/*�����ļ���*/
			set_file_name(urcp_info, tftp_session);
			/*���������ļ�*/
			urcp_open_file(tftp_session->filename, MAX_FILE_NAME_LEN, "wb", &(tftp_session->fd));
			if(tftp_session->fd==NULL)
			{
			    break;
			}
		    }
		    else if(tftp_session->sess_type==TFTP_GET_DATA)/*��ȡ����*/
		    {
			/*�����ڴ�*/
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
		    /*����ack*/
		    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_ACK);
		    /*��λ*/
		    tftp_session->state=WAIT_DATA;
		    URCP_DEBUG(("%s: total:%d\n", __func__, tftp_session->block_total));
		}
		else if(tftp_session->state==WAIT_W_OACK)
		{
		    /*�������ݼ�*/
		    urcp_tftp_send_pkt(tftp_session, urcp_info->vid, URCP_TFTP_DATA);
		    if(tftp_session->last_pkt!=NULL)/*����ռ�ɹ�*/
		    {
			/*��λ*/
			tftp_session->state=WAIT_ACK;
		    }
		}
		else
		{
		    /*�ܵ������ǲ��Ե�*/
		}
	    }
	    break;
	case URCP_TFTP_READ_FILE:/*read file*/
	    URCP_DEBUG(("%s: URCP_TFTP_READ_FILE\n", __func__));
//	    db_info(URCP_MODULE_ID_TFTP, "%s: URCP_TFTP_READ_FILE\n", __func__);
	    if((tftp_session==NULL) /*&& (urcp_info->certified==1)*/)/*�ûỰ������*/
	    {
		/*�ж���û��remote id��ͬ��,�����ڻػ�ʱ��ͣ�����Ự*/
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
	    if((tftp_session==NULL) /*&& (urcp_info->certified==1)*/)/*�ûỰ������*/
	    {
		/*�ж���û��remote id��ͬ��,�����ڻػ�ʱ��ͣ�����Ự*/
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
*������    urcp_tftp_poll
*����      ��ʱ���»Ự״̬,�Գ�ʱ�Ự����ע�����ط�����
*����      ��
*********************************************************/
void urcp_tftp_poll(void)
{
    uint32 i;
    urcp_tftp_session_info_t* sess;

    /*���β�ѯ���лỰ*/
    for(i=0u;i<MAX_URCP_TFTP_SESSION;i++)
    {
	/*�ǿ��лỰ*/
	if(urcp_tftp_session.session[i].state!=IDLE)
	{
	    sess = &urcp_tftp_session.session[i];
	    /*ʱ��*/
	    if(sess->timer>TFTP_MAX_TIMEOUT_MS)/*���س�ʱ*/
	    {
		db_info(URCP_MODULE_ID_TFTP, "%s: max timeout:%s\n", __func__, str_macaddr(sess->mac));
		//URCP_DEBUG(("%s: session max time out, state:%s\n", __func__, statestr[sess->state]));
#if 0
	        switch (sess->state)
		{
		    case WAIT_DATA:/*�ȴ�����*/
		    case WAIT_R_OACK: /*�ȴ��Զ������ȷ��*/
		    case WAIT_W_OACK: /*�ȴ���д�����ȷ��*/
		    case WAIT_OACK_ACK:/*�յ���������Ѿ�������ACK���ȴ�ȷ��*/
		    default:
			break;
		}
#endif
                /*ע���Ự*/
                urcp_tftp_session_clear(sess, URCP_STATUS_TIME_OUT);
	    }
	    else if(sess->timer>TFTP_TIMEOUT_MS)
	    {
		//URCP_ERROR(("%s: session time out, state:%s\n", __func__, statestr[sess->state]));
		/*�жϻỰ״̬*/
		switch(sess->state)
		{
		    case WAIT_ACK:
			if(sess->retries<TFTP_MAX_RETRIES)
			{
			    /*�ش�*/
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
			    /*��������ش�����,ע���Ự*/
			    urcp_tftp_session_clear(sess, URCP_STATUS_TIME_OUT);
			}
			break;
		    case WAIT_FIN:
                        /*ע���Ự*/
                        urcp_tftp_session_clear(sess, URCP_STATUS_OK);
			db_info(URCP_MODULE_ID_TFTP, "%s: wait_fin timeout\n",__func__);
			break;
#if 0
		    case WAIT_R_OACK:
		    case WAIT_W_OACK:
		    case WAIT_OACK_ACK:
                        /*ע���Ự*/
			db_info(URCP_MODULE_ID_TFTP, "%s: wait oack or ack timeout:%s\n",__func__,str_macaddr(sess->mac));
			break;
#endif
		    default:
			URCP_ERROR(("%s: time out default\n", __func__));
			db_info(URCP_MODULE_ID_TFTP, "%s: %s timeout, state:%d, block:%d\n",__func__, 
				str_macaddr(sess->mac), sess->state, sess->blockct);
			/*������1*/
			sess->timer = sess->timer + 1;
			break;
		}
	    }
	    else
	    {
		/*������1*/
                sess->timer = sess->timer + 1;
	    }
	}
    }

    return;
}

