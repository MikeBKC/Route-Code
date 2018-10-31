#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include <user_oid.h>
#include <mibUrcp.h>
#include "common.h"
#include "urcp.h"
#include "urcp_cmd.h"
#include "hash.h"
#include "trap_sta.h"
#include "urcp_ap.h"
#include "urcp_debug.h"
#ifdef URCP_AC
#include <switch.h>
#include <sw_cfg.h>
#include "urcp_ap.h"
#include "urcp_sta_log.h"
#endif

#ifdef URCP_AC
extern urcp_shm_t* shm_ptr;
#if (STA_ONOFF_LOG == FYES)
hash_table_info_t sta_table_log, *pStaTb_log=NULL;
log_pool_t *log_pool = NULL;
static uint32 today_time = 0u;	/*�����ʱ��*/
static uint32 max_file_size = 0u;   /*�ļ���С������*/
static uint32 time_mark = 0u;	   /*ʱ���ǣ����ڱȽϽڵ�ʱ������д�ļ�*/ 
static mtimer_t ac_sta_table_log_timer;

void ac_sta_table_log_init(void)
{
    db_info( URCP_MODULE_ID_STA_LOG, "%s, init\n", __func__);
    int i = 0;
    /*��ȡtableָ��*/
    pStaTb_log = &sta_table_log;
    log_pool = &(shm_ptr->logPool); 
    /*����*/
    memset(pStaTb_log, 0, sizeof(hash_table_info_t));
    pStaTb_log->pHash = (hash_entry_hdr_t*)(shm_ptr -> sta_log_hash);
    pStaTb_log->pTb = (tb_entry_hdr_t*)(shm_ptr -> sta_log);
    /*��ʼ���ڵ��С�ͱ��С*/
    pStaTb_log->unit = sizeof(sta_log_t);
    pStaTb_log->hash_size = MAX_STA_LOG_HASH_SIZE;
    pStaTb_log->table_size = MAX_STA_LOG_TB_SIZE;
    /*����*/
    memset(shm_ptr->sta_log_hash, 0, sizeof(shm_ptr->sta_log_hash));
    memset(shm_ptr->sta_log, 0, sizeof(shm_ptr->sta_log));
    memset(&(shm_ptr->logPool), 0, sizeof(shm_ptr->logPool));
    /*��ʾ���нڵ㣬��ʼ��ʱ�ӵ�һ���ڵ㿪ʼ, 0��ʾû�п��нڵ�, ���������еĽڵ��Ϊ��ʵ�����е��±꣬��Ϊ������0�Žڵ�*/
    pStaTb_log->first_free = 1;	
    for(i = 0; i < MAX_STA_LOG_TB_SIZE - 1; i++)
    {
	shm_ptr->sta_log[i].hdr.tb_next = i + 1;	
    }
    /*����ͬ��*/
    shm_ptr->logPool.firstFree = 1;
    for (i = 0; i < MAX_STA_LOG_POOL - 1; i++)
    {
	shm_ptr->logPool.logBuffer[i].next = i + 1;    
    }
    today_time = 0u;
    time_mark = 0u;
    /*������ʱ��*/
    MTIMER_START(&ac_sta_table_log_timer, AC_STA_TABLE_LOG_TIME_MS);
}

static void set_file_head(log_file_head_t *head)
{
    memset(head, 0, sizeof(log_file_head_t));
    head->head = 0x4C4F4746;
    head->flag = head->flag|STA_LOG_FLAG;
    head->head_len = sizeof(log_file_head_t);
    head->one_log_size = sizeof(log_file_info_t);
    head->head = htonl(head->head);
    head->flag = htons(head->flag);
    head->head_len = htons(head->head_len);
    head->one_log_size = htons(head->one_log_size);
}
/*�½��ļ�*/
static int create_new_log(char *path)
{
    char buff[256] = "";
    time_t timep;
    struct tm *p;
    FILE *log_fp = NULL;
    log_file_head_t head;
    
    memset(&head, 0, sizeof(head));
    db_info(URCP_MODULE_ID_STA_LOG, "%s, mkdir new:%s\n", __func__, path);
    time(&timep);
    p=localtime(&timep); /*ȡ�õ���ʱ��*/
    sprintf(buff,"staLog%d%d%d", (1900+p->tm_year),(1 + p->tm_mon), p->tm_mday);
    strcat(path, buff);

    log_fp = fopen(path, "wb");
    if (log_fp != NULL)
    {
	/*������־�ļ�,��д�ļ�ͷ*/
	db_info(URCP_MODULE_ID_STA_LOG, "%s, create new log_file\n", __func__);
	set_file_head(&head);
	fwrite(&head, sizeof(head), 1, log_fp);
	fclose(log_fp);
    }
    return 0;
}

static int get_usb_sta_log_file_name(char *path)
{
    int ret = -1;
    int size = 0;
    struct dirent* ent = NULL;
    DIR *pDir = NULL;
    char buff[256] = "";
    FILE *log_fp = NULL;
    log_file_head_t head;
    memset(&head, 0, sizeof(head));
    /*����ָ��·���µ������ļ������ҵ����������ļ�*/
//    db_info(URCP_MODULE_ID_STA_LOG, "%s, open dir :%s\n", __func__, path);
    pDir=opendir(path);
    if (pDir == NULL)
    {
	/*U������δ�и�Ŀ¼, �½���·�����ļ�*/
	if (mkdir(path, 0777) == 0)
	{
	    create_new_log(path);
	    time_mark = 0u;
	    ret = 0;
	}
    }
    else
    {
	while ((ent = readdir(pDir)) != NULL)
	{ 
	    if ((strcmp(ent->d_name,".") != 0) && (strcmp(ent->d_name, "..") != 0))
	    {
		db_info(URCP_MODULE_ID_STA_LOG, "%s, name:%s, %d\n", __func__, ent->d_name, ent->d_type);
		memset(buff,0, sizeof(buff));
		strncpy(buff, path, sizeof(buff));
		strcat(buff, ent->d_name);
		log_fp = fopen(buff, "rb");
		if (log_fp != NULL)
		{
		    /*��ȡ��ͷ*/
		    size = fread(&(head), sizeof(head), 1, log_fp);
		    if ( size == 1)
		    {
			if ((ntohl(head.head) == 0x4C4F4746) && (((ntohs(head.flag))&0x0F) == STA_LOG_FLAG))
			{
			    /*�����������ļ�*/
			    strcat(path, ent->d_name);
			    fclose(log_fp);
			    ret = 0;
			    db_info(URCP_MODULE_ID_STA_LOG, "%s, find file path:%s\n", __func__, path);
			    break;
			}
		    }
		    fclose(log_fp);
		    log_fp = NULL;
		}
	    }
	}
	if (ret != 0)
	{
	    /*�ļ����ܱ�ɾ�����½����ļ�*/
	    create_new_log(path);
	    time_mark = 0u;
	    ret = 0;
	}

    }
    if (pDir != NULL)
    {
	closedir(pDir);
    }
    return ret;
}

static int change_log_info_to_file(uint32 node, log_file_info_t *file_log)
{
    int ret = -1;
    log_info_t *sta_log = NULL;
    if (node != 0)
    {
	sta_log = &(log_pool->logBuffer[node]);
	if (sta_log->used == 2)
	{
	    /*�Ѿ����ߵ���־�ڵ�*/
	    memset(file_log, 0, sizeof(log_file_info_t));
	    memcpy(file_log->ap_mac, sta_log->apMac, MAC_LEN);
	    memcpy(file_log->sta_mac, shm_ptr->sta_log[sta_log->staIdx].hdr.mac_addr, MAC_LEN);
	    strncpy(file_log->staAlias, shm_ptr->sta_log[sta_log->staIdx].staAlias, sizeof(file_log->staAlias));
	    file_log->up_time = htonl(sta_log->upTime);
	    file_log->duration = htonl(sta_log->duration);
	    file_log->upload = htons(sta_log->upload);
	    file_log->download = htons(sta_log->download);
	    ret = 0;
	}
    }
    return ret;
}

static uint32 get_update_start_node(uint32 node, uint16 *num)
{
    uint16 need_num = 0u;	    /*ɸ����Ҫ���µĽڵ�����*/
    int ret = 0;
    /*������־��Ϣ*/
    if (node != 0)
    {
	if (time_mark != 0)
	{
	    while(log_pool->logBuffer[node].upTime > time_mark)
	    {
		if (2 == log_pool->logBuffer[node].used)
		{
		    need_num++;
		}
		else
		{
		    need_num = 0;
		}
		node = log_pool->logBuffer[node].earlierIdx;
	    }
	    node = log_pool->logBuffer[node].newIdx;
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, add to log_file\n", __func__);
	}
	else
	{
	    node = log_pool->earliestIdx;
	    while(node != 0)
	    {
		if (2 == log_pool->logBuffer[node].used)
		{
		    need_num++;
		}
		else
		{
		    break;
		}
		node = log_pool->logBuffer[node].newIdx;
	    }
	    node = log_pool->earliestIdx;
	    db_info(URCP_MODULE_ID_STA_LOG,"%s, first add log to file\n", __func__);
	}
    }
    *num = need_num;
    db_info(URCP_MODULE_ID_STA_LOG, "%s, need up log num:%u, start log node=%u\n", __func__, need_num, node);
    return node;
}

static uint16 get_file_log_num(char *path)
{
    FILE * log_fp = NULL;
    log_file_head_t head;
    memset(&head, 0, sizeof(head));
    log_fp = fopen(path, "rb");
    if (log_fp != NULL)
    {
	fread(&head, sizeof(head), 1, log_fp);
	fclose(log_fp);
	return ntohs(head.log_num);
    }
    return 0;
}

static void set_file_log_num(char *path, uint16 num)
{
    FILE * log_fp = NULL;
    log_file_head_t head;
    memset(&head, 0, sizeof(head));
    log_fp = fopen(path, "rb+");
    if (log_fp != NULL)
    {
	memset(&head, 0, sizeof(head));
	fread(&head, sizeof(head), 1, log_fp);
	head.log_num = htons(num);
	rewind(log_fp);
	fwrite(&head, sizeof(head), 1, log_fp);
	fclose(log_fp);
	db_info(URCP_MODULE_ID_STA_LOG, "%s, change file head ok ,log_num = %u\n", __func__, ntohs(head.log_num));
    }
}

static void update_sta_log_to_file(char *path)
{
    FILE *log_fp = NULL;
    log_file_head_t head;
    log_file_info_t *file_log = NULL;
    uint32 node = 0u;
    uint16 i = 0u;
    uint16 old_num = 0u;
    uint16 need_num = 0u;
    uint16 rel_num = 0u;
    char *new_log_buff = NULL;
    char *old_log_buff = NULL;

    db_info(URCP_MODULE_ID_STA_LOG, "%s, time_mark:%u\n", __func__, time_mark);
    node = log_pool->lastestIdx;
    node = get_update_start_node(node , &need_num);

    if((node != 0) && (need_num > 0))
    {
	new_log_buff = malloc(sizeof(log_file_info_t) * need_num);	/*�����ڴ�*/
	file_log = (log_file_info_t *)new_log_buff;
	i = need_num;
	if (new_log_buff != NULL)
	{
	    while( (node != 0) && (i != 0))
	    {
		if (change_log_info_to_file(node, file_log) == 0)
		{
		    //		fwrite(file_log, sizeof(file_log), 1, log_fp);	/*��ʼд��־*/
		    file_log++;
		    i--;
		    time_mark = log_pool->logBuffer[node].upTime;	/*����ʱ����*/
		}
		node = log_pool->logBuffer[node].newIdx;
	    }
	    need_num = need_num - i;
	}

	old_num = get_file_log_num(path);
	if ((old_num + need_num)*sizeof(log_file_info_t) <= max_file_size)
	{
	    /*���ļ������С��Χ֮��*/
	    log_fp = fopen(path, "ab");
	    if (log_fp != NULL)
	    {
		db_info(URCP_MODULE_ID_STA_LOG ,"%s, add %d log to file end\n", __func__, need_num);
		fwrite(new_log_buff, sizeof(log_file_info_t), need_num, log_fp);
		fclose(log_fp);
		set_file_log_num(path, old_num + need_num);
	    }
	}
	else if(need_num * sizeof(log_file_info_t) >= max_file_size)
	{
	    /*������־�������ļ��涨��С*/
	    log_fp = fopen(path, "wb");
	    if (log_fp != NULL)
	    {
		rel_num = max_file_size / sizeof(log_file_info_t);
		set_file_head(&head);
		head.log_num = htons(rel_num);
		fwrite(&head, sizeof(head), 1, log_fp);
		fwrite(new_log_buff + (need_num - rel_num)*sizeof(log_file_info_t), sizeof(log_file_info_t), rel_num, log_fp);
		fclose(log_fp);
		db_info(URCP_MODULE_ID_STA_LOG, "%s, restart file ,input log num:%d\n", __func__, rel_num);
	    }
	}
	else
	{
	    /*��������ԭ�еĳ����ļ���С����*/
	    old_log_buff = malloc(old_num * sizeof(log_file_info_t));
	    if (old_log_buff != NULL)
	    {
		log_fp = fopen(path, "rb+");
		if (log_fp != NULL)
		{
		    memset(&head, 0, sizeof(log_file_head_t));
		    fread(&head, sizeof(log_file_head_t), 1, log_fp);
		    fread(old_log_buff, sizeof(log_file_info_t), old_num, log_fp);
		    rewind(log_fp);
		    ftruncate(fileno(log_fp), 0);
		    rewind(log_fp);
		    rel_num = max_file_size / sizeof(log_file_info_t);
		    head.log_num = htons(rel_num);
		    fwrite(&head, sizeof(log_file_head_t), 1, log_fp);
		    fwrite(old_log_buff + (old_num-(rel_num-need_num))*sizeof(log_file_info_t), sizeof(log_file_info_t), rel_num - need_num, log_fp);
		    fwrite(new_log_buff, sizeof(log_file_info_t), need_num, log_fp);
		    fclose(log_fp);
		    db_info(URCP_MODULE_ID_STA_LOG, "%s, add old_log:%d and add new_log:%d\n", __func__,\
			    old_num-(rel_num-need_num), need_num);
		}
	    }
	    free(old_log_buff);
	}
	free(new_log_buff);	    /*�ͷ��ڴ�*/
    } 
}

static void sta_log_write(void)
{
    /*����Ƿ���U��*/
    char file_path[256] = "";
    long long total_size = 0, free_size = 0;
    int ret = -1;

#if (UTT_WX_2000 == FYES)
    /*2000����TF��*/
    strcpy(file_path, "/ff/");
    strcat(file_path, USB_STA_LOG_PATH);
    max_file_size = USB_STA_LOG_SIZE;
    ret = get_usb_sta_log_file_name(file_path);
#else
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    getOuterFlashStat(&total_size, &free_size);
#endif
    db_info(URCP_MODULE_ID_STA_LOG, "%s, U total:%lld, free:%lld\n", __func__, total_size, free_size);
    if (total_size != 0)
    {
	if (free_size > USB_STA_LOG_SIZE )
	{
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	    isUSBStorageExist(file_path);
#endif
	    strcat(file_path, USB_STA_LOG_PATH);
	    /*ȷ��U���е��ļ�·��*/
	    max_file_size = USB_STA_LOG_SIZE;
	    ret = get_usb_sta_log_file_name(file_path);
	}
	else
	{
	    max_file_size = free_size - sizeof(log_file_head_t);
	    /*�ռ䲻��*/
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, external memory out\n", __func__);
	}
    }
#endif
   
    if (ret == 0)
    {
	/*��ȡ�������ļ�·��*/
	db_info(URCP_MODULE_ID_STA_LOG, "%s, path:%s\n", __func__, file_path);
	update_sta_log_to_file(file_path);
    }
}

void ac_sta_log_timer()
{
    log_info_t *log = NULL;
    uint32 node = 0, i = 0;
    if (MTIMER_TIMEOUT(&ac_sta_table_log_timer))
    {
	db_info(URCP_MODULE_ID_STA_LOG, "%s, test\n", __func__);
	node = log_pool->earliestIdx;
	while(node != 0)
	{
	    log = &(log_pool->logBuffer[node]);
	    db_info(URCP_MODULE_ID_STA_LOG, "%s,TIME->>%d>>> used:%u, apMac:%s", __func__, i++ ,log->used, str_macaddr(log->apMac));
	    db_info(URCP_MODULE_ID_STA_LOG, " staMac:%s, uptime:%u, duration:%u, staIdx:%u, earlierIdx:%u, new:%u, next:%u, myself:%u\n",\
		     str_macaddr(shm_ptr->sta_log[log->staIdx].hdr.mac_addr),log->upTime, log->duration,log->staIdx, \
		     log->earlierIdx, log->newIdx, log->next, node);
	    node = log->newIdx;
	}
	
	sta_log_write();
	/*��ӡ��־�ļ�����*/
	MTIMER_START(&ac_sta_table_log_timer, AC_STA_TABLE_LOG_TIME_MS);
    }
}

static int log_change_node(uint32 node)
{
    log_info_t *log = NULL;
    uint32 new_node = 0u, ear_node = 0u;
    if (0 == node)
    {
	return -1;
    }

    log = &(log_pool->logBuffer[node]);

    if ((node == log_pool->lastestIdx) && (node == log_pool->earliestIdx))
    {
	/*��һ��Ҳ�����һ����־�����ı�*/
	log_pool->lastestIdx = 0;	
    }
    else 
    {
	/*�жϺ͸�����ڵ��Ƿ��ͻ*/
	if (node == log_pool->lastestIdx)
	{
	    log_pool->lastestIdx = log->earlierIdx;
	}
	if ( node == log_pool->earliestIdx)
	{
	    log_pool->earliestIdx = log->newIdx;
	}
	if (node == log_pool->ago1day)
	{
	    log_pool->ago1day = log->newIdx;
	}
	if (node == log_pool->ago3day)
	{	
	    log_pool->ago3day = log->newIdx;
	}
	if (node == log_pool->ago7day)
	{	
	    log_pool->ago7day = log->newIdx;
	}
	if (node == log_pool->ago1month)
	{	
	    log_pool->ago1month = log->newIdx;
	}
	if (node == log_pool->ago2month)
	{	
	    log_pool->ago2month = log->newIdx;
	}
	if (node == log_pool->ago1year)
	{
	    log_pool->ago1year = log->newIdx;
	}
    
	/*ɾ���ýڵ�*/	
	new_node = log->newIdx;
	ear_node = log->earlierIdx;
	if (new_node != 0u)
	{
	    log_pool->logBuffer[new_node].earlierIdx = ear_node;
	}
	if (ear_node != 0u)
	{
	    log_pool->logBuffer[ear_node].newIdx = new_node;
	}
    }
    db_info(URCP_MODULE_ID_STA_LOG,"%s, last:%u, earlist:%u, 1day:%u, 3day:%u, 7day:%u, 1month:%u, 2month:%u, 1year:%u\n",__func__,\
	   log_pool->lastestIdx, log_pool->earliestIdx, log_pool->ago1day, log_pool->ago3day, log_pool->ago7day,\
	   log_pool->ago1month, log_pool->ago2month, log_pool->ago1year);
    return 0;
}

/*����ָ���ڵ����һ���ڵ��nextΪ0*/
static void set_node_prev_next_zero(uint32 node)
{
    uint32 first = 0u;
    uint32 ret = 0;
    sta_log_t *sta = NULL;
    sta = &(shm_ptr->sta_log[log_pool->logBuffer[node].staIdx]);
    first = sta->logPoolIdx;
    db_info(URCP_MODULE_ID_STA_LOG, "%s, the sta :%s, first log:%d\n", __func__, str_macaddr(sta->hdr.mac_addr),first);
    if (first == node)
    {
	sta->count = 0;
	sta->logPoolIdx = 0;
    }
    else
    {
	ret = first;
	while(log_pool->logBuffer[ret].next != node)
	{
	    ret = log_pool->logBuffer[ret].next;
	}
	log_pool->logBuffer[ret].next = 0;
	sta->count -= 1;
    }
    db_info(URCP_MODULE_ID_STA_LOG, "%s, the sta :%s, count:%d\n", __func__, str_macaddr(sta->hdr.mac_addr), sta->count);
}

static uint32 get_log_free()
{
    uint32 ret = 0;
    if ((log_pool->firstFree > 0 ) && (log_pool->logBuffer[log_pool->firstFree].used == 0))
    {
	ret = log_pool->firstFree;
	log_pool->firstFree = log_pool->logBuffer[ret].next;
	memset((char *)&(log_pool->logBuffer[ret]), 0, sizeof(log_info_t));
    }
    else
    {
#if 0
	/*����,д�ļ���յ�ǰ����*/
		log_write();	
#endif 
	/*ɾ������һ����¼*/
	db_info(URCP_MODULE_ID_STA_LOG, "%s, log full\n", __func__);
	ret = log_pool->earliestIdx;
	log_change_node(log_pool->earliestIdx);
	set_node_prev_next_zero(ret);
    }
    return ret;
}

static uint32 get_log_last(uint32 node)
{
    uint32 ret = node;
    while(log_pool->logBuffer[ret].next != 0)
    {
	ret = log_pool->logBuffer[ret].next;
    }
    return ret;
}

/*��ȡ��������ʱ��*/
static uint32 get_today_zero(uint32 time)
{
    struct tm *p;
    p = gmtime((time_t *)&time);
    if (p != NULL)
    {
	time -= (p->tm_hour * 3600 + p->tm_min * 60 + p->tm_sec );
    }
    return time;
}

/*���ø���ʱ���Ľڵ�*/
static void set_log_date_data(void)
{
    uint32 ret = 0u;
    int flag_old = 0u, flag_now = 0u;
    log_info_t *p = NULL;
    ret = log_pool->lastestIdx;
    /*��ʱ��˳�����*/
    db_info(URCP_MODULE_ID_STA_LOG,"%s, end\n", __func__);
    do
    {
	p = &(log_pool->logBuffer[ret]);
	if ((p->upTime >= today_time))
	{
	    /*����*/
	    flag_now = flag_old;
	}
	else if ((p->upTime < today_time) && (p->upTime >= (today_time - LOG_3_DAY)) )
	{
	    /*3������*/
	    flag_now = 1;
	    if (flag_now != flag_old)
	    {
		/*���׶ε�һ������*/
		log_pool->ago1day = p->newIdx;
		flag_old = flag_now;
	    }
	}
	else if ( (p->upTime < (today_time - LOG_3_DAY)) && (p->upTime >= (today_time - LOG_7_DAY)))
	{
	    /*7������*/
	    flag_now = 2;
	    if (flag_now != flag_old)
	    {
		log_pool->ago3day = p->newIdx;
		flag_old = flag_now;
	    }
	}
	else if ( (p->upTime < (today_time - LOG_7_DAY)) && (p->upTime >= (today_time- LOG_1_MONTH)))
	{
	    flag_now = 3;
	    if (flag_now != flag_old)
	    {
		log_pool->ago7day = p->newIdx;
		flag_old = flag_now;
	    }
	}
	else if ( (p->upTime < (today_time- LOG_1_MONTH)) && (p->upTime >= (today_time- LOG_2_MONTH)))
	{
	    flag_now = 4;
	    if (flag_now != flag_old)
	    {
		log_pool->ago1month = p->newIdx;
		flag_old = flag_now;
	    }
	}
	else if ( (p->upTime < (today_time- LOG_2_MONTH)) && (p->upTime >= (today_time- log_1_YEAR)))
	{
	    flag_now = 5;
	    if (flag_now != flag_old)
	    {
		log_pool->ago2month = p->newIdx;
		flag_old = flag_now;
	    }
	}
	else
	{
	    flag_now = 6;
	    if (flag_now != flag_old)
	    {
		log_pool->ago1year = p->newIdx;
		flag_old = flag_now;
	    }
	}

	ret = p->earlierIdx;
    }while(ret != 0);

    db_info(URCP_MODULE_ID_STA_LOG,"%s, end\n", __func__);
}

static void change_log_order(uint32 node, uint32 time)
{
    log_info_t *old_log = NULL;
    log_info_t *new_log = NULL;
    db_info(URCP_MODULE_ID_STA_LOG, "%s, node = %d\n", __func__,node);
    if (0 == log_pool->lastestIdx)
    {
	/*ϵͳ��ʼ�����һ��*/
	db_info(URCP_MODULE_ID_STA_LOG, "%s, first sta first log\n", __func__);
	log_pool->lastestIdx = node;
	log_pool->earliestIdx = node;
	log_pool->ago1day = node;
	log_pool->ago3day = node;
	log_pool->ago7day = node;
	log_pool->ago1month = node;
	log_pool->ago2month = node;
	log_pool->ago1year = node;
	log_pool->logBuffer[node].earlierIdx = 0;
	log_pool->logBuffer[node].newIdx = 0;
	log_pool->logBuffer[node].upTime = time;
	today_time = get_today_zero(time);  /*��ȡtime��Ӧ�ĵ�������ʱ��*/
    }
    else
    {
	old_log = &(log_pool->logBuffer[log_pool->lastestIdx]);
	new_log = &(log_pool->logBuffer[node]); 
	old_log->newIdx = node;
	new_log->earlierIdx = log_pool->lastestIdx;
	new_log->newIdx = 0u;
	
	log_pool->lastestIdx = node;	    /*���½ڵ�*/
	new_log->upTime = time;
	if ((time - today_time) > LOG_1_DAY)
	{
	    db_info(URCP_MODULE_ID_STA_LOG, "%s day changed today_time:%u, now_time:%u\n ", __func__, today_time, time);
	   /*����ʱ�䷢���ı�*/
	    today_time = get_today_zero(time);
	    set_log_date_data();
	}
    }

}

static sta_log_t * ac_sta_table_log_lookup(char *mac)
{
    tb_entry_hdr_t tp;
    sta_log_t *entry_log = NULL;
    memset(&tp, 0, sizeof(tb_entry_hdr_t));
    memcpy(tp.mac_addr, mac, MAC_LEN);
    
    entry_log = (sta_log_t*)hash_lookup_entry(pStaTb_log, &tp);
    
    return entry_log;
}

static void set_up_down_num(uint16 *num, unsigned long long data)
{
    if (data < MAX_UP_DOWN_SIZE)
    {
	/*ԭ����*/
	*num = (uint16)data;
    }
    else if ((data / 1000ull) < MAX_UP_DOWN_SIZE)
    {
	*num = (uint16)(data/1000ull);
	*num = (*num) | (0x01 << 14);
    }
    else if ((data / 1000000ull) < MAX_UP_DOWN_SIZE)
    { 
	*num = (uint16)(data/1000000ull);
	*num = (*num) | (0x02 << 14);
    }
    else 
    { 
	*num = (uint16)(data/1000000000ull);
	*num = (*num) | (0x03 << 14);
    }
}

int ac_sta_add_down_log(char *mac , unsigned long long upload, unsigned long long download)
{
    uint32 node = 0u;
    sta_log_t *entry_log = NULL;
    log_info_t *log = NULL;
    db_info(URCP_MODULE_ID_STA_LOG,"%s, sta mac:%s, down\n", __func__, str_macaddr(mac));
    entry_log = ac_sta_table_log_lookup(mac);
    if ( (entry_log != NULL) && (entry_log->logPoolIdx != 0))
    {
	db_info(URCP_MODULE_ID_STA_LOG, "%s, find sta and logPoolIdx= %u\n", __func__, entry_log->logPoolIdx);
	node = entry_log->logPoolIdx;
	if (node != 0)
	{
	    log = &(log_pool->logBuffer[node]);
	    if (log->used == 1)
	    {
		log->used = 2;	/*2���������ˣ��ýڵ���������*/ 
		set_up_down_num(&(log->upload), upload);	    /**/
		set_up_down_num(&(log->download), download);    /**/
		log->duration = (uint32)time(NULL) - log->upTime;	/*��¼����ʱ��*/
		/*����������ʱ���������*/
		change_log_order(node, (uint32)time(NULL));
	    }
	    /*��ӡ�ýڵ�sta��������Ϣ*/
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, used:%u, apMac:%s, uptime:%u, duration:%u, staIdx:%u, earlierIdx:%u, new:%u, next:%u\n",\
		    __func__,log->used, str_macaddr(log->apMac), log->upTime, log->duration, log->staIdx, log->earlierIdx, log->newIdx, log->next);
	}
    }
    return 0;
}


static int ac_add_up_log(sta_log_t *sta_log, log_info_t *entry_log) 
{
    log_info_t *log = NULL;
    uint32 node = 0, old_node = 0;
    uint32 now_time = 0;
    if (sta_log->count > 0)
    {
	/*���нڵ�,����һ���ڵ�*/
	if (sta_log->logPoolIdx > 0)
	{
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, this start log:%d\n", __func__, sta_log->logPoolIdx);
	    old_node = sta_log->logPoolIdx;
	    if ((log_pool->logBuffer[old_node].duration < LOG_UP_DOWN_LIMIT) && (log_pool->logBuffer[old_node].used == 2))
	    {
		db_info(URCP_MODULE_ID_STA_LOG, "%s, find log:%d duration:%d\n", __func__, old_node, log_pool->logBuffer[old_node].duration);
		/*�ϴ�����ʱ�����,����ʹ�øýڵ�*/
//		log_pool->logBuffer[old_node].used = 1;
		log = &(log_pool->logBuffer[old_node]);
		log->duration = 0u;
		node = old_node;
		/*������ʱnode�ڱ������е�˳��, ��Ϊold_node�������ʱ�������µ�һ���ڵ�*/
		log_change_node(old_node);
	    }
	    else if ((log_pool->logBuffer[old_node].duration == 0) && (log_pool->logBuffer[old_node].used == 1) )
	    {
		/*�ظ�����,����һ��ʱ��*/
		node = sta_log->logPoolIdx;
		log =  &(log_pool->logBuffer[node]);
		log->upTime = (uint32)time(NULL);
		log = NULL;
	    }
	    else
	    {
		if ((node = get_log_free()) > 0)
		{
		    db_info(URCP_MODULE_ID_STA_LOG, "%s, online get free node:%d\n", __func__, node);
		    log = &(log_pool->logBuffer[node]); 
		    log->next = old_node;
		    sta_log->count += 1;
		    sta_log->logPoolIdx = node;
//		    log->used = 1;	/*ʹ�øýڵ�*/
		}
	    }
	}
    }
    else
    {
	/*��sta��δ������־�ڵ㣬��ȡ����־�ڵ㣬��ʼ����*/
	if ((node = get_log_free()) > 0)
	{
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, first add log:%d\n", __func__, node);
	    sta_log->logPoolIdx = node;
	    sta_log->count = 1;
	    log = &(log_pool->logBuffer[node]); 
//	    log->used = 1;	/*ʹ�øýڵ�*/
	    log->next = 0;
	}
    }

    if (log != NULL)
    {
	if ( 0 == entry_log->used)
	{
	    now_time = (uint32)time(NULL);
	    log->upTime = now_time;
	    log->used = 1;
	}
	else
	{
	    /*��entry_log�ڵ��������ڵ�,�ļ�����ȡ�������ڵ�,��������Ϣ�����*/
	    log->used = entry_log->used;
	    log->upTime = entry_log->upTime;
	    now_time = entry_log->upTime;
	    log->duration = entry_log->duration;
	    log->upload = entry_log->upload;
	    log->download = entry_log->download;
	}
	memcpy(log->apMac, entry_log->apMac, MAC_LEN);
	log->staIdx = (((char *)sta_log - (char *)(pStaTb_log->pTb))/sizeof(sta_log_t));
	db_info(URCP_MODULE_ID_STA_LOG, "%s, uptime:%u, staIdx:%u, ApMac:%s\n", __func__, log->upTime, log->staIdx, str_macaddr(log->apMac));
	/*�����*/
//	change_log_order(node, now_time);
    }

    return 0;
}

int ac_sta_add_up_log(char *sta_mac, char *ap_mac)
{
    sta_log_t *entry_log = NULL;
    tb_entry_hdr_t tp;
    int ret = 0;
    log_info_t log;

    memset((char *)&log, 0, sizeof(log_info_t));
    memset(&tp, 0, sizeof(tb_entry_hdr_t));
    memcpy(tp.mac_addr, sta_mac, MAC_LEN);
    ret = hash_add_entry(pStaTb_log, &tp);
    if (ret != HASH_RET_FAILED)
    {
	db_info(URCP_MODULE_ID_STA_LOG, "%s, add sta ok\n", __func__);
	/*���Ҷ�Ӧ��STA�ڵ�*/
	entry_log = (sta_log_t *)(pStaTb_log->pDst);
	memcpy(log.apMac, ap_mac, MAC_LEN);
	ac_add_up_log(entry_log, &log);
    }
    return 0;
}

int get_sta_log_file_name(char *path)
{
    char buff[256] = "";
    log_file_head_t head;
    struct dirent* ent = NULL;
    DIR *pDir = NULL;
    int ret = -1;
    FILE *log_fp = NULL;

#if (UTT_WX_2000 == FYES)
    /*2000����TF��*/
    strcpy(path, "/ff/");
    strcat(path, USB_STA_LOG_PATH);
    ret = 0;
#else
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (isUSBStorageExist(path))
    {
	/*�ҵ�U��*/
	strcat(path, USB_STA_LOG_PATH);
	ret = 0;
    }
#endif
#endif

    if (0 == ret)
    {
	ret = -1;
	pDir = opendir(path);
	if (pDir != NULL)
	{
	    while ((ent = readdir(pDir)) != NULL)
	    { 
		if ((strcmp(ent->d_name,".") != 0) && (strcmp(ent->d_name, "..") != 0))
		{
		    memset(buff,0, sizeof(buff));
		    strncpy(buff, path, sizeof(buff));
		    strcat(buff, ent->d_name);
		    log_fp = fopen(buff, "rb");
		    if (log_fp != NULL)
		    {
			/*��ȡ��ͷ*/
			memset(&head, 0, sizeof(head));
			if ( fread(&(head), sizeof(head), 1, log_fp) == 1)
			{
			    if ((ntohl(head.head) == 0x4C4F4746) && (((ntohs(head.flag))&0x0F) == STA_LOG_FLAG))
			    {
				/*�����������ļ�*/
				strcat(path, ent->d_name);
				fclose(log_fp);
				ret = 0;
				db_info(URCP_MODULE_ID_STA_LOG, "%s, find file path:%s\n", __func__, path);
				break;
			    }
			}
			fclose(log_fp);
			log_fp = NULL;
		    }
		}
	    }
	    closedir(pDir);
	}
    }
    return ret;
}

void urcp_msg_del_log( struct msgbuf_t *msgbuf )
{
    urcp_del_log_msg_t *msg;
    long long total_size = 0, free_size = 0;
    char file_path[256] = "";
    char buff[256] = "";

    msg = (urcp_del_log_msg_t *)(msgbuf->sw_msg.buffer);
    if (msg->mode == 0)
    {
#if (UTT_WX_2000 == FYES)
	/*2000����TF��*/
	strcpy(file_path, "/ff/");
	strcat(file_path, USB_STA_LOG_PATH);
	sprintf(buff, "rm -rf %s", file_path);
	system(buff);
	if (mkdir(file_path, 0777) == 0)
	{
	    create_new_log(file_path);
	}
	db_info(URCP_MODULE_ID_STA_LOG, "%s, del all log, %s\n\n", __func__, file_path);
#else
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	getOuterFlashStat(&total_size, &free_size);
#endif
	if (total_size != 0)
	{
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	    isUSBStorageExist(file_path);
#endif
	    strcat(file_path, USB_STA_LOG_PATH);
	    sprintf(buff, "rm -rf %s", file_path);
	    system(buff);
	    if (mkdir(file_path, 0777) == 0)
	    {
		create_new_log(file_path);
	    }
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, del all log, %s\n", __func__, file_path);
	}
	else
	{
	    db_info(URCP_MODULE_ID_STA_LOG, "%s, reinit sta_log\n", __func__);
	    /*û��USB�ļ������³�ʼ��*/
	    ac_sta_table_log_init();
	}
#endif
    }
}

#endif
#endif
