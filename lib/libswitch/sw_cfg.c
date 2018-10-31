#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <switch.h>
#include <urcp_ipc.h>
#include "sw_cfg.h"


#define SW_SHM_SIZE    sizeof(urcp_shm_t)


#define  DEBUG_CFG 0
#if DEBUG_CFG
#define CFG_DEBUG(fmt, agrx...)   printf(fmt, ##agrx);
#else
#define CFG_DEBUG(fmt, agrx...)   
#endif
#define SM_DEV_TB   (shm_ptr->dev_table)

struct semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
static urcp_shm_t *shm_ptr=NULL;

#define LOOKUP_BY_IP  1u  /*根据user_ip搜索*/
#define LOOKUP_BY_ID  2u  /*根据user_ip搜索*/

typedef struct _misc_lookup_opt_t{
   uint32 opt_type;
   uint32 mask;
   uint32 ip;
   uint16 reqId;
}misc_lookup_opt_t;

static uint32 urcp_misc_get_next(uchar* misc_addr, uint32 misc_buffer_size);
static int32 urcp_misc_set_status(uchar *miscaddr, uint16 id, urcp_status_t status, uint32 unit_size, uint32 misc_buffer_size);
static uint32 urcp_misc_read(uchar *misc_addr, uchar mac[MAC_LEN], misc_lookup_opt_t *opt, uint32 unit_size, uint32 misc_buffer_size);
static uint32 urcp_create_misc_record(uchar* misc_addr, uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type, uint32 unit_size, uint32 misc_buffer_size);

/*
 *初始化共享内存
 * KeHuatao
 */
void shm_init(void)
{
    memset(shm_ptr, 0, sizeof(urcp_shm_t));

    return;
}
urcp_shm_t* get_shm_ptr(void)
{
    return shm_ptr;
}
/*
 * 映射共享内存
 * 
 **/
int sw_shm_init(void)    
{
    int shm_id ;
    char *ptr;

    /*get the shared memery*/ 
    shm_id = shmget(SW_SHM_KEY, SW_SHM_SIZE, IPC_CREAT | 0666);
    if(shm_id == -1)          
    {  
	perror("switch shmget error"); 
	return -1;            
    }  
    /*get the pointer*/       
    ptr = shmat(shm_id, NULL, 0); 
    shm_ptr = (urcp_shm_t*)ptr; 
    if((int)ptr == -1)        
    {  
	perror("switch shmmat err");   
	return -1;            
    }  
    /*return the shm_id*/
    return shm_id;
}


/*
 *
 * initial the message queue  
 *     
 ***/   
int sw_msg_init(void)         
{
    int msg_id;               

    /*create the msg queue*/  
    msg_id = msgget(SW_MSG_KEY, IPC_CREAT | 0666);
    if(msg_id == -1)          
    {  
	perror("switch msgget error");
    }  
    CFG_DEBUG(("msgid:%d\n", msg_id));

    return msg_id;            
}

/*
 *
 * initial sem
 * 
 **/
int sw_sem_init(void)
{
    struct semun sem;
    int semid;

    sem.val = 0;
    /*创建信号量*/
    semid = semget(SW_SEM_KEY, 1, IPC_CREAT|0x1B6u);
    
    CFG_DEBUG(("semid %d\n", semid));

    if (semid == -1)
    {
	perror("switch create semaphore error\n");
    }
    else
    {
	/*清零*/
	semctl(semid, 0, SETVAL, sem);
    }

    return semid;
}



/*
 *
 * 信号量加1
 *
 **/ 
int sw_sem_p(int semid, int no_wait)
{
    struct sembuf sops = {0, +1, 0};

    if(no_wait > 0){
        sops.sem_flg = IPC_NOWAIT;
    }

    return (semop(semid, &sops, 1));                                                                                                
}

/*
 * 
 *信号量减1
 *  
 **/
int sw_sem_v(int semid, int no_wait)
{
    //struct semun sem;
    struct sembuf sops = {0, -1, 0};
#if 0   
    /*清零*/
    sem.val = 0;
    semctl(semid, 0, SETVAL, sem);
#endif

    if(no_wait > 0){
        sops.sem_flg = IPC_NOWAIT;
    }

    return (semop(semid, &sops, 1));
}
/*
 * 写联动设置交换空间数据结构
 * bhou
 * */
extern void cfg_write_urcp_swap_conf(char *conf_data, uint32 ulen)
{
    if(ulen >= URCP_SWAP_CONF_SIZE)
    {
	ulen = URCP_SWAP_CONF_SIZE - 1u;
    }
    memset(shm_ptr->urcp_swap_buf.urcp_conf, 0, URCP_SWAP_CONF_SIZE);
    memcpy(shm_ptr->urcp_swap_buf.urcp_conf, conf_data, ulen);
    return;
}
/**读取联动设置交换空间数据结构
 * bhou
 */
extern void cfg_read_urcp_swap_conf(char *dst_conf, uint32 ulen)
{
    if(ulen >= URCP_SWAP_CONF_SIZE)
    {
	ulen = URCP_SWAP_CONF_SIZE - 1u;
    }
    memcpy(dst_conf,shm_ptr->urcp_swap_buf.urcp_conf, ulen);
    return;
}


/*
 *  *
 *   *计算设备hash表的key
 *    *
 *     */
uint32 urcp_dev_table_hash(uchar* mac, uint32 ip)
{
    uint32 key;

    /*所有字节异或*/
    key = (mac[0] ^ mac[1] ^ mac[2] ^ mac[3] ^
	    mac[4]) ^ mac[5];
    /*高位和低位异或*/
    key = ((key >> 4) | key) % DEV_HASH_SIZE;
#if  DEV_HASH_USE_IP  /*not use ip,keHuatao*/
    /*于ip异或*/
    key = (ip & 0x1fu) ^ ((ip >> 5) & 0x1fu);
#endif

    return key;
}

/*
 *
 *在设备表中搜索某设备
 *KeHuatao
 **/ 
urcp_dev_t* urcp_dev_table_lookup(uchar* mac, uint32 ip)
{
    uint32  key;
    int32 index;
    urcp_dev_t* tmp=NULL;

    /*calcu key*/
    key = urcp_dev_table_hash(mac, ip);
    CFG_DEBUG("%s: key:%d\n", __func__, key);
    CFG_DEBUG("%s: mac:%s, ip: 0x%08x\n", __func__, str_macaddr(mac), ip);
    index = shm_ptr->dev_tb_hash[key];
    if(index != NONE_NODE)
    {
	/*不为空*/
	tmp = &SM_DEV_TB[index-1]; 
	CFG_DEBUG("tb_index:%d\n", index);
	while(index != NONE_NODE)
	{ 
	    CFG_DEBUG("mac:%s\n", str_macaddr(SM_DEV_TB[index-1].mac));
	    /*compare*/
	    if ((mac_cmp(SM_DEV_TB[index -1].mac, mac) == 0)
#if DEV_HASH_USE_IP  
		    && ((SM_DEV_TB[index -1]).ip==ip)
#endif
		    )
	    {
		/*get it*/
#if (DEV_HASH_USE_IP!=1)
			if (ip != 0) {
				SM_DEV_TB[index -1].ip=ip;
			}
#endif
		return &SM_DEV_TB[index - 1];
	    }
	    index = SM_DEV_TB[index-1].tb_next;
	}
    }
    CFG_DEBUG("lookup failed\n");
    /*没找到*/
    return NULL;
}
/*
 *
 *返回下一个可用节点号
 *KeHuatao
 *
 */
static uint32 urcp_misc_get_next(uchar* misc_addr, uint32 misc_buffer_size)
{
    uint32 index;

    /*第一个字节为next_index*/
    index = *((uint32*)misc_addr); 
    if(index+1u>=misc_buffer_size)
    {
	/*返回到开头*/
	*((uint32*)misc_addr) = 0u;
    }
    else
    {
	/*下一个*/
	*((uint32*)misc_addr) = index + 1u;
    }

    return index;
}
/*
 *在misc buffer 中创建一条新记录
 *KeHuatao
 */
static uint32  urcp_create_misc_record(uchar misc_addr[8], uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type, uint32 unit_size, uint32 misc_buffer_size)
{
    uchar *ptr;
    uint32 i;
    misc_header_t *buf;

    /*下一个节点*/
    ptr = misc_addr; /*定位到header 处*/
    i = urcp_misc_get_next(ptr, misc_buffer_size);

    /*跳过next_index*/
    ptr = &(ptr[sizeof(uint32)]);
    /*定位到第i个位置*/
    ptr = &ptr[unit_size * i];
    /*转换类型*/
    buf = (misc_header_t*)ptr;
    /*清空*/
    memset(buf, 0, sizeof(misc_header_t));
    /*初始化header结构*/
    memcpy(buf->mac, macaddr, MAC_LEN);
    buf->user_ip = userIp;
    buf->req_id = id;
    buf->data_type = type;
    /*status*/
    buf->status = URCP_STATUS_WAIT;
    buf->valid = URCP_DATA_INVALID;

    return i;
}

/*
 *往miscbuffer中写入error msg
 *根据req id搜索目的记录
 *KeHuatao
 */
#if 1
static int32 urcp_misc_set_status(uchar miscaddr[8], uint16 id, urcp_status_t error_status, uint32 unit_size, uint32 misc_buffer_size)
#else
static int32 urcp_misc_set_errmsg(uchar miscaddr[8], uint16 id, const char *msg, uint32 unit_size, uint32 misc_buffer_size)
#endif
{
    uint32 i;
    uchar *ptr;
    misc_header_t* header;

    CFG_DEBUG("%s, %d\n", __func__, error_status);
    ptr = miscaddr;
    /*table起始地址*/
    ptr = &(ptr[sizeof(uint32)]);
    for(i=0u;i<misc_buffer_size;i++)
    {
	/*定位到header处*/
	header = (misc_header_t*)(&ptr[unit_size*i]);
	if(header->req_id==id)
	{
	    /*找到*/
#if 1
	    header->status = error_status;
#else
	    if(msg!=NULL)
	    {
		/*不为空*/
		memset(header->error_msg, 0, MISC_MSG_LEN);
		strncpy(header->error_msg, msg, MISC_MSG_LEN-1u);
		header->status = URCP_OPT_FAIL;
	    }
	    else
	    {
		/*成功*/
		header->status = URCP_OPT_OK;
	    }
#endif
	    header->valid = URCP_DATA_VALID;
	    CFG_DEBUG("%s: found index:%d, id:%d\n", __func__, i, id);
	    /*退出*/
	    break;
	}
    }

    return 0;
}
/*
 *在misc表中查找目标项
 *KeHuatao
 */
static uint32 urcp_misc_read(uchar misc_addr[8], uchar mac[MAC_LEN], misc_lookup_opt_t *opt, uint32 unit_size, uint32 misc_buffer_size)
{
    uchar *ptr;
    uint32 i, j, found;
    misc_header_t* header;

    found=0u;
    /*加偏移*/
    ptr = misc_addr;
    /*第一个字节为next_index*/
    j = *((uint32*)ptr);
    //printf("\n%s: next index:%d, %s, %08x, id:%u, mask:%08x, type:%d\n", __func__, j, str_macaddr(mac), opt->ip, opt->reqId, opt->mask, opt->opt_type);
#if 0
    if(j<misc_buffer_size)
#endif
    {
	/*table起始地址*/
	ptr = &(ptr[sizeof(uint32)]);
	/*查找所有buffer*/
	for(i=0u;i<misc_buffer_size;i++)
	{
	    if(j==0u)
	    {
		/*跳到末尾*/
		j=misc_buffer_size-1u;
	    }
	    else/*前一个*/
	    {
		j--;
	    }

	    header = (misc_header_t*)(&ptr[unit_size*j]);
	    /*定位到下一条记录*/
	    //printf("\n%s: tb[%d]: %s, ip:%08x, id:%d, type:%d, valid:%d\n",__func__, j, str_macaddr(header->mac), header->user_ip, header->req_id, header->data_type, header->valid);
	    if(
		    ((mac_cmp(mac, header->mac)==0)/*mac相同*/
		     && (opt->opt_type==header->data_type)
		     && ((opt->mask&LOOKUP_BY_IP)>0u) && (opt->ip==header->user_ip))/*ip相同*/
		    || (((opt->mask&LOOKUP_BY_ID)>0u) && (opt->reqId==header->req_id))/*id相同*/
	      )
	    {
		/*找到了*/
		found=1u;
		break;
	    }
	}
    }

    /*找到了*/
    if(found==1u)
    {
	    //printf("\n< %s %d > found\n",__func__, __LINE__);
	/*表长度不可能为0xFFFFFFFF*/
	j++;
    }
    else/*没找到*/
    {
	    //printf("\n< %s %d > Not found\n",__func__, __LINE__);
	j=0u;
    }

    return j;
}

/**
 *初始化misc_opt_assist结构
 *bhou
 */
void init_misc_opt_assist(misc_opt_assist*misc_buff,uchar* misc_addr,uint32 u_unit_size,uint32 u_misc_buffer_size)
{
    misc_buff->misc_buffer_header = misc_addr;/*初始化头部*/
    misc_buff->unit_size = u_unit_size;/*初始化单个大小*/
    misc_buff->misc_buffer_size = u_misc_buffer_size ; /*初始化buffer长度*/
}

/*
 *在misc buffer中添加一条记录
 *bhou
 */
void urcp_create_general_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,misc_opt_assist* misc_buff,uchar*  config)
{
    uint32 i;
    uchar tmp[32];
    uchar* ptr=tmp;

    i = urcp_create_misc_record(misc_buff->misc_buffer_header, macaddr, userIp, id, type,misc_buff->unit_size , misc_buff->misc_buffer_size);/*创建记录，并返回索引*/
    //printf("\n< %s %d > i:%d\n",__func__,__LINE__,i);
    if(config!=NULL)
    {
	/*复制信息*/
	ptr = misc_buff->misc_buffer_header;/*定位到buffer头*/
	ptr = &ptr[sizeof(uint32)];/*跳过next_index*/
	ptr = &ptr[misc_buff->unit_size * i];/*定位到第i个结构体*/
	ptr = &ptr[sizeof(misc_header_t)];/*跳过misc_header*/
	memcpy(ptr, config, misc_buff->unit_size-sizeof(misc_header_t));/*信息拷贝*/
    }

    return;
}
/*
 *将接受到的信息写入misc buf
 *bhou
 */
int32 urcp_rev_general_get_reply(uchar macaddr[MAC_LEN], uint16 id,misc_opt_assist* misc_buff,uchar*conf)
{
    uint32 i=0u;
    int32 j=0;
    uchar tmp[32];
    uchar* ptr=tmp;

    misc_header_t* misc_header=NULL;
    misc_lookup_opt_t opt;

    CFG_DEBUG("%s\n",__func__);
    /*设置参数*/
    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_ID;
    /*根据req id查找*/
    opt.reqId = id;
    /*读取节点信息*/
    i = urcp_misc_read(misc_buff->misc_buffer_header, macaddr, &opt, misc_buff->unit_size , misc_buff->misc_buffer_size);
    if(i>0u)
    {
	i= i-1u;
	/*写该节点数据*/
	ptr = misc_buff->misc_buffer_header;/*定位到buffer头*/
	ptr = &ptr[sizeof(uint32)];/*跳过next_index*/
	ptr = &ptr[misc_buff->unit_size * i];/*定位到第i个结构体*/
	misc_header = (misc_header_t*)ptr;/*记录misc_header地址*/
	ptr = &ptr[sizeof(misc_header_t)];/*跳过misc_header*/
	memcpy(ptr, conf, misc_buff->unit_size - sizeof(misc_header_t));/*信息拷贝*/

	/*更新头部信息*/
	misc_header->status = URCP_STATUS_OK;
	/*置为有效*/
	misc_header->valid = URCP_DATA_VALID;

	j=0;
    }
    else
    {
	j=(int32)(-1);
    }

    return j;
}

/*
 *读取misc buf中的数据
 *并返回其状态
 * */
urcp_status_t urcp_read_general_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type, misc_opt_assist* misc_buff ,uchar* conf)
{
    uint32 i = 0u ;
    uchar tmp[32];
    uchar* ptr=tmp;
    misc_header_t* misc_header=NULL;
    misc_lookup_opt_t opt;
    urcp_status_t ret;

    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_IP;
    opt.ip = userIp;
    opt.opt_type = type;
    /*差找该节点*/
    i = urcp_misc_read(misc_buff->misc_buffer_header, mac, &opt, misc_buff->unit_size , misc_buff->misc_buffer_size);
    //printf("\n< %s %d > i:%d\n",__func__,__LINE__,i);
    if(i>0u)
    {
	/*找到*/
	i = i-1u;
	/*读取该节点数据*/
	ptr = misc_buff->misc_buffer_header;/*定位到buffer头*/
	ptr = &ptr[sizeof(uint32)];/*跳过next_index*/
	ptr = &ptr[misc_buff->unit_size * i];/*定位到第i个结构体*/
	misc_header = (misc_header_t*)ptr;/*记录misc_header地址*/
	ptr = &ptr[sizeof(misc_header_t)];/*跳过misc_header*/
	/*状态*/
	ret = misc_header->status;

	if( ( ret == URCP_STATUS_OK) && (conf != NULL))
	{
	    memcpy(conf, ptr ,misc_buff->unit_size-sizeof(misc_header_t));/*信息拷贝*/
	    /*拷贝长度为conf长度不包含header*/
	}
    //printf("\n< %s %d > ptr:%s\n",__func__,__LINE__,ptr);
    }
    else
    {
	/*查找失败*/
	ret = URCP_STATUS_FOUND_NONE;
    }

    return ret;
}
/*
 *在misc buffer中添加一条记录
 *KeHuatao
 */
void urcp_create_mirror_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type, urcp_mirror_conf_t* mirror_config)
{
    uint32 i, j;

    CFG_DEBUG("%s\n", __func__);
    j = sizeof(urcp_mirror_buf_t);
    i = urcp_create_misc_record((uchar*)(&(shm_ptr->urcp_misc.urcp_mirror_struct)), macaddr, userIp, id, type, j, URCP_MISC_MIRROR_SIZE);
    if((type==MISC_SET_GENERAL) && (mirror_config!=NULL))
    {
	/*复制镜像信息*/
	memcpy(&(shm_ptr->urcp_misc.urcp_mirror_struct.urcp_mirror_buf[i].mirror_conf), mirror_config, sizeof(urcp_mirror_conf_t));
    }

    return;
}
/*
 *读取镜像端口号
 *KeHuatao
 */
urcp_status_t urcp_read_mirror_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type, urcp_mirror_conf_t* conf)
{
    uint32 i, j;
    urcp_mirror_buf_t* buf;
    misc_lookup_opt_t opt;
    urcp_status_t ret;

    CFG_DEBUG("%s\n",__func__);
    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_IP;
    opt.ip = userIp;
    opt.opt_type = type;
    /*差找该节点*/
    j = sizeof(urcp_mirror_buf_t);
    i = urcp_misc_read((uchar*)(&(shm_ptr->urcp_misc.urcp_mirror_struct)), mac, &opt, j, URCP_MISC_MIRROR_SIZE);
    if(i>0u)
    {
	/*找到*/
	i--;
	buf = &(shm_ptr->urcp_misc.urcp_mirror_struct.urcp_mirror_buf[i]);
	memcpy(conf, &(buf->mirror_conf), sizeof(struct _urcp_mirror_conf_t));
	/*状态*/
	ret = buf->mirror_header.status;
    }
    else
    {
	/*查找失败*/
	ret = URCP_STATUS_FAIL;
    }
    CFG_DEBUG("%s: ret:%d\n",__func__, ret);

    return ret;
}

#if 0
/*
 *设置联动镜像端口
 *KeHuatao
 */
int32 urcp_rev_mirror_set_reply(uchar macaddr[MAC_LEN], uint16 id, uchar *errmsg)
{
    uint32 i;
    int32 j;
    urcp_mirror_buf_t *buf;
    misc_lookup_opt_t opt;

    CFG_DEBUG("%s\n",__func__);
    /*设置操作参数*/
    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_ID;
    opt.opt_type = MISC_SET;
    /*根据req id查找*/
    opt.reqId = id;
    /*读取节点信息*/
    i = urcp_misc_read((uchar*)(&(shm_ptr->urcp_misc.urcp_mirror_struct)), sizeof(urcp_mirror_buf_t), macaddr, &opt);
    if(i>0u)
    {
	i--;
	/*读取该节点数据*/
	buf = &(shm_ptr->urcp_misc.urcp_mirror_struct.urcp_mirror_buf[i]);
	if(errmsg==NULL)
	{
	    /*成功*/
	    buf->mirror_header.status = URCP_OPT_OK;
	}
	else
	{
	    /*失败*/
	    buf->mirror_header.status = URCP_OPT_FAIL;
	    /*保存错误信息*/
	    memset(buf->mirror_header.error_msg, 0, MISC_REMARK_LEN);
	    strncpy(buf->mirror_header.error_msg, errmsg, MISC_REMARK_LEN-1u);
	}
	/*该记录有效*/
	buf->mirror_header.valid = URCP_DATA_VALID;
	j=0;
    }
    else
    {
	/*没找到*/
	j=-1;
    }

    return j;
}
#endif

/*
 *收到联动包，将数据写入misc buffer
 *如果失败，则将错误信息写入buffer
 *KeHuatao
 */
int32 urcp_rev_mirror_get_reply(uchar macaddr[MAC_LEN], uint16 id, urcp_mirror_conf_t *conf)
{
    uint32 i, k;
    int32 j=0;
    urcp_mirror_buf_t *buf;
    misc_lookup_opt_t opt;

    CFG_DEBUG("%s\n",__func__);
    /*设置参数*/
    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_ID;
    /*根据req id查找*/
    opt.reqId = id;
    /*读取节点信息*/
    k = sizeof(urcp_mirror_buf_t);
    i = urcp_misc_read((uchar*)(&(shm_ptr->urcp_misc.urcp_mirror_struct)), macaddr, &opt, k, URCP_MISC_MIRROR_SIZE);
    if(i>0u)
    {
	i--;
	/*读取该节点数据*/
	buf = &(shm_ptr->urcp_misc.urcp_mirror_struct.urcp_mirror_buf[i]);
	/*成功*/
	memcpy(&(buf->mirror_conf), conf, sizeof(urcp_mirror_conf_t));
	buf->mirror_header.status = URCP_STATUS_OK;
	/*置为有效*/
	buf->mirror_header.valid = URCP_DATA_VALID;
	j=0;
    }
    else
    {
	j=(int32)(-1);
    }

    return j;
}

/*
 *根据req id搜索miscbuffer
 *KeHuatao
 */
int32 urcp_mirror_set_status(uint16 id, urcp_status_t status1)
{
    int32 ret;
    uint32 i;

    CFG_DEBUG("%s: %d\n", __func__, status1);
    /*写入 */
    i = sizeof(urcp_mirror_buf_t);
    ret = urcp_misc_set_status((uchar*)(&shm_ptr->urcp_misc.urcp_mirror_struct), id, status1, i, URCP_MISC_MIRROR_SIZE);
    /*返回*/
    return ret;
}

/*
 *根据req id搜索miscbuffer
 *KeHuatao
 */
int32 urcp_passwd_auth_set_status(uint16 id, urcp_status_t status1)
{
    int32 ret;
    uint32 i;    

    CFG_DEBUG("%s: %d\n", __func__, status1);
    /*写入 */
    i = sizeof(urcp_passwd_auth_buf_t);
    ret = urcp_misc_set_status((uchar*)(&shm_ptr->urcp_misc.urcp_passwd_auth_struct), id, status1, i, URCP_MISC_PASSWD_SIZE);
    /*返回*/
    return ret;
}

/*
 *创建密码验证操作记录
 *KeHuatao
 */
void urcp_create_passwd_auth_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type)
{
    uint32 i;

    CFG_DEBUG("%s\n", __func__);
    i = sizeof(urcp_passwd_auth_buf_t);
    urcp_create_misc_record((uchar*)(&(shm_ptr->urcp_misc.urcp_passwd_auth_struct)), macaddr, userIp, id, type, i, URCP_MISC_PASSWD_SIZE);

    return;
}
/*
 *读取镜像端口号
 *KeHuatao
 */
urcp_status_t urcp_read_passwd_auth(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    uint32 i, j;
    urcp_passwd_auth_buf_t* buf;
    misc_lookup_opt_t opt;
    urcp_status_t ret;

    CFG_DEBUG("%s\n",__func__);
    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_IP;
    opt.ip = userIp;
    opt.opt_type = type;
    /*差找该节点*/
    j = sizeof(urcp_passwd_auth_buf_t);
    i = urcp_misc_read((uchar*)(&(shm_ptr->urcp_misc.urcp_passwd_auth_struct)), mac, &opt, j, URCP_MISC_PASSWD_SIZE);
    if(i>0u)
    {
	/*找到*/
	i--;
	/*地址*/
	buf = &(shm_ptr->urcp_misc.urcp_passwd_auth_struct.urcp_passwd_auth_buf[i]);
	/*操作结果*/
	ret = buf->passwd_auth_header.status;
#if DEBUG_CFG
	if(ret==URCP_STATUS_OK)
	{
	    /*成功*/
	    CFG_DEBUG("%s: passwd auth sucessed id:%d\n",__func__, buf->passwd_auth_header.req_id);
	}
#endif
    }
    else
    {
	/*查找失败*/
	ret = URCP_STATUS_FAIL;
    }

    return ret;
}

/***
 *初始化 aggr_misc_opt_assist结构
 *bhou
 */
void init_aggr_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_aggr_struct));
    misc_buff->unit_size = sizeof(urcp_aggr_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_AGGR_SIZE;
}
/*
 *在misc buffer中添加一条记录
 *bhou
 */
void urcp_create_aggr_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,urcp_aggr_conf_t* aggr_config)
{
    misc_opt_assist misc_buff;
    init_aggr_misc_opt_assist(&misc_buff);

    CFG_DEBUG("%s\n", __func__);
    urcp_create_general_record(macaddr, userIp,id,type,&misc_buff, (uchar*) aggr_config);

    return;
}
/*
 *将接受到的信息写入misc buf
 *bhou
 */
int32 urcp_rev_aggr_get_reply(uchar macaddr[MAC_LEN], uint16 uid, urcp_aggr_conf_t *conf)
{
    misc_opt_assist misc_buff;
    init_aggr_misc_opt_assist(&misc_buff);
    CFG_DEBUG("%s\n",__func__);
    return urcp_rev_general_get_reply(macaddr,uid,&misc_buff, (uchar*)conf);
}

/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_aggr_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_aggr_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/*
 *读取misc buf中的数据
 *并返回其状态
 * */
urcp_status_t urcp_read_aggr_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type, urcp_aggr_conf_t* conf)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;
    CFG_DEBUG("%s\n", __func__);
    init_aggr_misc_opt_assist(&misc_buff);
    ret= urcp_read_general_conf(mac,userIp,type, &misc_buff,(uchar*) conf) ;
    return ret;
}
/***
 *初始化 vlan_misc_opt_assist结构
 *bhou
 */
void init_vlan_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_vlan_struct));
    misc_buff->unit_size = sizeof(urcp_vlan_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_VLAN_SIZE;
}
/*
 *在misc buffer中添加一条记录
 *bhou
 */
void urcp_create_vlan_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,urcp_vlan_conf_t* vlan_config)
{
    misc_opt_assist misc_buff;
    init_vlan_misc_opt_assist(&misc_buff);

    CFG_DEBUG("%s\n", __func__);
    urcp_create_general_record(macaddr, userIp,id,type,&misc_buff, (uchar*) vlan_config);

    return;
}
/*
 *将接受到的信息写入misc buf
 *bhou
 */
int32 urcp_rev_vlan_get_reply(uchar macaddr[MAC_LEN], uint16 uid, urcp_vlan_conf_t *conf)
{
    misc_opt_assist misc_buff;
    init_vlan_misc_opt_assist(&misc_buff);
    CFG_DEBUG("%s\n",__func__);
    return urcp_rev_general_get_reply(macaddr,uid,&misc_buff, (uchar*)conf);
}

/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_vlan_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_vlan_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/*
 *读取misc buf中的数据
 *并返回其状态
 * */
urcp_status_t urcp_read_vlan_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type, urcp_vlan_conf_t* conf)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;
    CFG_DEBUG("%s\n", __func__);
    init_vlan_misc_opt_assist(&misc_buff);
    ret= urcp_read_general_conf(mac,userIp,type, &misc_buff,(uchar*) conf) ;
    return ret;
}
/**
 *初始化 qvlan_pconf_misc_opt_assist
 *bhou
 */
void init_qvlan_pconf_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_qvlan_pconf_struct));
    misc_buff->unit_size = sizeof(urcp_qvlan_pconf_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_QVLAN_PCONF_SIZE;
}
/*
 *在misc buffer中添加一条记录
 *bhou
 */
void urcp_create_qvlan_pconf_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,urcp_qvlan_pconf_conf_t* qvlan_pconf_config)
{
    misc_opt_assist misc_buff;
    init_qvlan_pconf_misc_opt_assist(&misc_buff);

    CFG_DEBUG("%s\n", __func__);
    urcp_create_general_record(macaddr, userIp,id,type,&misc_buff, (uchar*) qvlan_pconf_config);

    return;
}
/*
 *将接受到的信息写入misc buf
 *bhou
 */
int32 urcp_rev_qvlan_pconf_get_reply(uchar macaddr[MAC_LEN], uint16 id, urcp_qvlan_pconf_conf_t *conf)
{
    misc_opt_assist misc_buff;
    init_qvlan_pconf_misc_opt_assist(&misc_buff);
    CFG_DEBUG("%s\n",__func__);
    return urcp_rev_general_get_reply(macaddr,id,&misc_buff, (uchar*)conf);
}

/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_qvlan_pconf_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_qvlan_pconf_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/*
 *读取misc buf中的数据
 *并返回其状态
 * */
urcp_status_t urcp_read_qvlan_pconf_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type, urcp_qvlan_pconf_conf_t* conf)
{
    misc_opt_assist misc_buff;
    init_qvlan_pconf_misc_opt_assist(&misc_buff);
    return urcp_read_general_conf(mac,userIp,type, &misc_buff,(uchar*) conf) ;
}
/**
 *初始化 rstp_lacp_misc_opt_assist
 *bhou
 */
void init_rstp_lacp_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_rstp_lacp_struct));
    misc_buff->unit_size = sizeof(urcp_rstp_lacp_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_RSTP_LACP_SIZE;
}
/*
 *在misc buffer中添加一条记录
 *bhou
 */
void urcp_create_rstp_lacp_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,urcp_rstp_lacp_conf_t* rstp_lacp_config)
{
    misc_opt_assist misc_buff;
    init_rstp_lacp_misc_opt_assist(&misc_buff);

    CFG_DEBUG("%s\n", __func__);
    urcp_create_general_record(macaddr, userIp,id,type,&misc_buff, (uchar*) rstp_lacp_config);

    return;
}
/*
 *将接受到的信息写入misc buf
 *bhou
 */
int32 urcp_rev_rstp_lacp_get_reply(uchar macaddr[MAC_LEN], uint16 id, urcp_rstp_lacp_conf_t *conf)
{
    misc_opt_assist misc_buff;
    init_rstp_lacp_misc_opt_assist(&misc_buff);
    CFG_DEBUG("%s\n",__func__);
    return urcp_rev_general_get_reply(macaddr,id,&misc_buff, (uchar*)conf);
}

/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_rstp_lacp_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_rstp_lacp_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/*
 *读取misc buf中的数据
 *并返回其状态
 * */
urcp_status_t urcp_read_rstp_lacp_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type, urcp_rstp_lacp_conf_t* conf)
{
    misc_opt_assist misc_buff;
    init_rstp_lacp_misc_opt_assist(&misc_buff);
    return urcp_read_general_conf(mac,userIp,type, &misc_buff,(uchar*) conf) ;
}

/***
 *初始化 port_conf_misc_opt_assist结构
 *bhou
 */
void init_port_conf_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_port_conf_struct));
    misc_buff->unit_size = sizeof(urcp_port_conf_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_PORT_CONF_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_port_conf_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_port_conf_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}


/***
 *初始化 misc_opt_assist结构
 *bhou
 */
void init_qos_rate_limit_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_qos_rate_limit_struct));
    misc_buff->unit_size = sizeof(urcp_qos_rate_limit_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_QOS_RATE_LIMIT_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_qos_rate_limit_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/***
 *初始化 misc_opt_assist结构
 *bhou
 */
void init_storm_rate_limit_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_storm_rate_limit_struct));
    misc_buff->unit_size = sizeof(urcp_storm_rate_limit_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_STORM_RATE_LIMIT_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_storm_rate_limit_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/***
 *初始化 misc_opt_assist结构
 *bhou
 */
void init_sys_conf_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_sys_conf_struct));
    misc_buff->unit_size = sizeof(urcp_sys_conf_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_SYS_CONF_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_sys_conf_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_sys_conf_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/***
 *初始化 misc_opt_assist结构
 *bhou
 */
void init_ip_filter_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_ip_filter_struct));
    misc_buff->unit_size = sizeof(urcp_ip_filter_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_IP_FILTER_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_ip_filter_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_ip_filter_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/***
 *初始化 misc_opt_assist结构
 *bhou
 */
void init_sys_info_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_sys_info_struct));
    misc_buff->unit_size = sizeof(urcp_sys_info_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_SYS_INFO_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_sys_info_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_sys_info_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
/***
 *初始化 misc_opt_assist结构
 *bhou
 */
void init_cmd_alone_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_cmd_alone_struct));
    misc_buff->unit_size = sizeof(urcp_cmd_alone_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_CMD_ALONE_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *bhou
 */
int32 urcp_cmd_alone_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_cmd_alone_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

/*
 *读取misc buf中的数据
 *并返回其状态
 *bhou
 **/
urcp_status_t urcp_read_cmd_alone_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 utype)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;

    init_cmd_alone_misc_opt_assist(&misc_buff);
    /*读取结果*/
    ret= urcp_read_general_conf(mac, userIp, utype, &misc_buff,(uchar*)NULL) ;

    return ret;
}

/***
 *初始化 misc_opt_assist结构
 *ChenShiyu
 */
void init_password_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_passwd_struct));
    misc_buff->unit_size = sizeof(urcp_passwd_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_PASSWD_SIZE;
}
/*
 *根据req id搜索miscbuffer，并设置相应的buf状态
 *ChenShiyu
 */
int32 urcp_password_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;

    misc_opt_assist misc_buff;
    init_password_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

/*
 *在misc buffer中添加一条记录
 *ChenShiyu
 */
void urcp_create_password_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type,uchar *passwd_config)
{
    uint32 i, j; 

    j = sizeof(urcp_passwd_buf_t);
    i = urcp_create_misc_record((uchar*)(&(shm_ptr->urcp_misc.urcp_passwd_struct)), macaddr, userIp, id, type, j, URCP_MISC_PASSWD_SIZE);	
    CFG_DEBUG("%s\n", __func__);

    return;
}
/*
 *读取密码信息
 *ChenShiyu
 */
urcp_status_t urcp_read_password(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    uint32 i, j;
    urcp_passwd_buf_t* buf;
    misc_lookup_opt_t opt;
    urcp_status_t ret;

    CFG_DEBUG("%s\n",__func__);
    memset(&opt, 0, sizeof(misc_lookup_opt_t));
    opt.mask |= LOOKUP_BY_IP;
    opt.ip = userIp;
    opt.opt_type = type;
    /*差找该节点*/
    j = sizeof(urcp_passwd_buf_t);
    i = urcp_misc_read((uchar*)(&(shm_ptr->urcp_misc.urcp_passwd_struct)), mac, &opt, j, URCP_MISC_PASSWD_SIZE);
    if(i>0u)
    {
	/*找到*/
	i--;
	/*地址*/
	buf = &(shm_ptr->urcp_misc.urcp_passwd_struct.urcp_passwd_buf[i]);
	/*操作结果*/
	ret = buf->passwd_header.status;
#if DEBUG_CFG
	if(ret==URCP_STATUS_OK)
	{
	    /*成功*/
	    CFG_DEBUG("%s: password successed id:%d\n",__func__, buf->passwd_header.req_id);
	}
#endif
    }
    else
    {
	/*查找失败*/
	ret = URCP_STATUS_FAIL;
    }

    return ret;
}
/***
 *初始化 config_misc_opt_assist结构
 *keHuatao
 */
static void init_config_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_config_struct));
    misc_buff->unit_size = sizeof(urcp_config_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_CONFIG_SIZE;
}

/*
 *在misc buffer中添加一条记录
 *KeHuatao
 */
void urcp_create_config_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type)
{
    misc_opt_assist misc_buff;

    CFG_DEBUG("%s\n", __func__);
    init_config_misc_opt_assist(&misc_buff);
    urcp_create_general_record(macaddr, userIp, id, type, &misc_buff, NULL);

    return;
}

/*
 *读取misc buf中的数据
 *并返回其状态
 *KeHuatao
 **/
urcp_status_t urcp_read_config_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;

    init_config_misc_opt_assist(&misc_buff);
    /*读取结果*/
    ret= urcp_read_general_conf(mac, userIp, type, &misc_buff, NULL) ;

    return ret;
}
/*
 *根据req id搜索miscbuffer，并设置相信的 buf状态
 *KeHuatao
 */
int32 urcp_config_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;
    misc_opt_assist misc_buff;

    init_config_misc_opt_assist(&misc_buff);/*初始化*/

    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status(misc_buff.misc_buffer_header, uid, error_status, misc_buff.unit_size, misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

void init_ap_basic_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_ap_basic_conf_struct));
    misc_buff->unit_size = sizeof(urcp_ap_basic_conf_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_AP_BASIC_CONF_SIZE;
}

void urcp_create_ap_basic_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type)
{
    misc_opt_assist misc_buff;

    CFG_DEBUG("%s\n", __func__);
    init_ap_basic_misc_opt_assist(&misc_buff);
    urcp_create_general_record(macaddr, userIp, id, type, &misc_buff, NULL);

    return;
}
urcp_status_t urcp_read_ap_basic_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;

    init_ap_basic_misc_opt_assist(&misc_buff);
    /*读取结果*/
    ret= urcp_read_general_conf(mac, userIp, type, &misc_buff, NULL) ;

    return ret;
}
int32 urcp_ap_basic_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;
    misc_opt_assist misc_buff;

    init_ap_basic_misc_opt_assist(&misc_buff);/*初始化*/
    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

void init_mac_filter_misc_opt_assist(misc_opt_assist *misc_buff)
{
	misc_buff->misc_buffer_header = (uchar *)(&(shm_ptr->urcp_misc.urcp_mac_filter_struct));
	misc_buff->unit_size = sizeof(urcp_wl_mac_filter_t);
	misc_buff->misc_buffer_size = URCP_MISC_WL_MAC_FILTER_SIZE;
}

int32 urcp_wl_mac_filter_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;
    misc_opt_assist misc_buff;

    init_mac_filter_misc_opt_assist(&misc_buff);/*初始化*/
    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

/*
 * 初始化 config_misc_opt_assist结构
 * added by Shiyu.Chen, 2012-07-22
 */
void init_ap_conf_temp_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_ap_conf_temp_struct));
    misc_buff->unit_size = sizeof(urcp_ap_conf_temp_conf_t);
    misc_buff->misc_buffer_size = URCP_MISC_AP_CONF_TEMP_SIZE;
}


/*
 * 在misc buffer中添加一条记录
 * added by Shiyu.Chen, 2012-07-22
 */
void urcp_create_ap_conf_temp_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type)
{
    misc_opt_assist misc_buff;

    CFG_DEBUG("%s\n", __func__);
    init_ap_conf_temp_misc_opt_assist(&misc_buff);
    urcp_create_general_record(macaddr, userIp, id, type, &misc_buff, NULL);

    return;
}

/*
 * 读取misc buf中的数据
 * 并返回其状态
 * added by Shiyu.Chen, 2012-07-22
 */
urcp_status_t urcp_read_ap_conf_temp(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;

    init_ap_conf_temp_misc_opt_assist(&misc_buff);
    /*读取结果*/
    ret= urcp_read_general_conf(mac, userIp, type, &misc_buff, NULL) ;

    return ret;
}

/*
 * 根据req id搜索miscbuffer，并设置相信的 buf状态
 * added by Shiyu.Chen, 2012-07-22
 */
int32 urcp_ap_conf_temp_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;
    misc_opt_assist misc_buff;

    init_ap_conf_temp_misc_opt_assist(&misc_buff);/*初始化*/
    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status( misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

/* AP join servicezone */
void init_ap_join_sz_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_ap_join_sz_struct));
    misc_buff->unit_size = sizeof(urcp_ap_join_sz_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_AP_JOIN_SZ_SIZE;
}

void urcp_create_ap_join_sz_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type)
{
    misc_opt_assist misc_buff;

    CFG_DEBUG("%s\n", __func__);
    init_ap_join_sz_misc_opt_assist(&misc_buff);
    urcp_create_general_record(macaddr, userIp, id, type, &misc_buff, NULL);

    return;
}
urcp_status_t urcp_read_ap_join_sz_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;

    init_ap_join_sz_misc_opt_assist(&misc_buff);
    /*读取结果*/
    ret= urcp_read_general_conf(mac, userIp, type, &misc_buff, NULL) ;

    return ret;
}
int32 urcp_ap_join_sz_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;
    misc_opt_assist misc_buff;

    init_ap_join_sz_misc_opt_assist(&misc_buff);/*初始化*/
    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status(misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}

/* AP mac filter */
void init_ap_mac_filter_misc_opt_assist(misc_opt_assist* misc_buff)
{
    misc_buff->misc_buffer_header = (uchar*)(&(shm_ptr->urcp_misc.urcp_ap_mac_filter_struct));
    misc_buff->unit_size = sizeof(urcp_ap_mac_filter_buf_t);
    misc_buff->misc_buffer_size = URCP_MISC_AP_MAC_FILTER_SIZE;
}

void urcp_create_ap_mac_filter_record(uchar macaddr[MAC_LEN], uint32 userIp, uint16 id, uint32 type)
{
    misc_opt_assist misc_buff;

    CFG_DEBUG("%s\n", __func__);
    init_ap_mac_filter_misc_opt_assist(&misc_buff);
    urcp_create_general_record(macaddr, userIp, id, type, &misc_buff, NULL);

    return;
}
urcp_status_t urcp_read_ap_mac_filter_conf(uchar mac[MAC_LEN], uint32 userIp, uint32 type)
{
    urcp_status_t ret;
    misc_opt_assist misc_buff;

    init_ap_mac_filter_misc_opt_assist(&misc_buff);
    /*读取结果*/
    ret= urcp_read_general_conf(mac, userIp, type, &misc_buff, NULL) ;

    return ret;
}
int32 urcp_ap_mac_filter_set_status(uint16 uid, urcp_status_t error_status)
{
    int32 ret;
    misc_opt_assist misc_buff;

    init_ap_mac_filter_misc_opt_assist(&misc_buff);/*初始化*/
    CFG_DEBUG("%s: %d\n", __func__, error_status);
    /*写入 */
    ret = urcp_misc_set_status(misc_buff.misc_buffer_header, uid, error_status,misc_buff.unit_size ,misc_buff.misc_buffer_size);
    /*返回*/
    return ret;
}
