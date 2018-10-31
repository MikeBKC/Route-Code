#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <string.h>
#include    <dirent.h>
#include    <time.h>
#include    <uttMachine.h>
#include    "webs.h"
#include    "utils.h"
#include    "usb.h"
#include    "uttfunction.h"
#include    <linux/autoconf.h>
#include    "translate.h"
#include    <unistd.h>

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <urcp_ipc.h>
#include    <switch.h>
#include    <sw_cfg.h>
#include    "webmsg.h"
#include    "mibUrcp.h"
#endif

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include     <usb-user.h>
#endif

#if (AP_DRAWING == FYES)

#define IMAGE_SIZE      5000000
#define DRAW_MAGIC          0x554a5047
#define DRAW_VERSION        1

#define DRAW_AP_TYPE        0
#define DRAW_AP_MODULE      0
#define DRAW_AP_STATUS      0

#define DBCKS 0
#if DBCKS
#define DBCK(fmt, args...) \
			{ \
				printf("%s, %d, %s(), "fmt, __FILE__, __LINE__, __func__, ##args); \
				printf("\n"); \
			}
#else
#define DBCK(fmt, args...)
#endif

typedef struct _draw_head {
	unsigned int      magic;
	char              ver;
	unsigned short    hlen;
	unsigned short    length;
	unsigned short    height;
	unsigned short    width;
	char              reserve[4];
} draw_head;

typedef struct _draw_ap_info {
	unsigned short    type;
	unsigned short    length;
	unsigned short    model;
	unsigned short    status;
	unsigned int      sn;
	unsigned short    x;
	unsigned short    y;
	unsigned char     reserve[4];
} draw_ap_info;

#define   JPG_PATH    "/etc_ro/web/images/"
#define   DEFAULT_JPG_PATH "/etc_ro/web/"
static char lastCheckedFileName[50] = "default.jpg";
static int isDefaultJPGExist = 1;
static int isJPG(const char *path);

int draw_head_init(char buf[1024], unsigned short height, unsigned short width)
{
	int ret = 0;
	draw_head *header;

	header = (draw_head *)buf;

	header->magic = htonl(DRAW_MAGIC);
	header->ver = DRAW_VERSION;
	header->hlen = htons(sizeof(draw_head));
	header->length = header->hlen;
	header->height = htons(height);
	header->width = htons(width);

	return ret;
}
/*
 *
 *
 *
 * */
static void setALLDrawingUmanaged(void)
{
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t* tmp = NULL;
    int tb_index,val,i;
    //更新dev_table中AP的管理状态
    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index - 1]);
		val = tmp->dev_type;
		tmp->dev_type = (val&(~DEV_TYPE_DRAWING_MANAGED));
		tb_index = tmp->tb_next;
	    }
	}
    }
}
/*
 *
 *
 *
 * */
void setApDrawingManaged(unsigned int sn,int isManage)
{
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t* tmp = NULL;
    int tb_index,val,i,flag = 0;

    //更新dev_table中AP的管理状态
    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index - 1]);
		if(tmp->serial_no == sn)
		{
		    val = tmp->dev_type;
		    if(isManage)
		    {
			tmp->dev_type = (val|DEV_TYPE_DRAWING_MANAGED);
		    }
		    else
		    {
			tmp->dev_type = (val&(~DEV_TYPE_DRAWING_MANAGED));
		    }
		    flag = 1;//已经找到AP
		    break;
		}
		tb_index = tmp->tb_next;
	    }
	    if(flag)
	    {
		break;
	    }
	}
    }
}
int add_ap_info(char buf[1024], unsigned short x, unsigned short y, unsigned int sn)
{
	int ret = 0;
	draw_head *header;
	draw_ap_info *ap_info;

	header = (draw_head *)buf;
	ap_info = (draw_ap_info *)(&buf[ntohs(header->length)]);

	ap_info->type = htons(DRAW_AP_TYPE);
	ap_info->length = htons(sizeof(draw_ap_info));
	ap_info->model = htons(DRAW_AP_MODULE);
	ap_info->status = htons(DRAW_AP_STATUS);
	ap_info->sn= htonl(sn);
	ap_info->x = htons(x);
	ap_info->y = htons(y);
	
	header->length = ntohs(header->length);
	header->length += ntohs(ap_info->length);
	header->length = htons(header->length);

	return ret;
}
void checkPicBuf(char *path_obj,int isManaged)
{
    int fp, ret,length,flag = 0;
    char str[1024] = {0}, buf[1024] = {0};
    draw_head *header = NULL;
    draw_ap_info *ap_info,*tmp;
    header = (draw_head *)str;
    fp = open(path_obj, O_RDWR);
    if(fp != 0) 
    {
	ret = lseek(fp, -1024, SEEK_END);
	if(ret != -1) 
	{
	    read(fp, str, 1024);
	    if(header->magic != htonl(DRAW_MAGIC)) 
	    {
		close(fp);
		return;
	//	read(fp, buf, 1024);
	    }
	    else 
	    {
		ret = lseek(fp, -1024, SEEK_END);
		read(fp, buf, 1024);
	    }
	}
	close(fp);
    }
    header = (draw_head *)buf;
    length = ntohs(header->length);
    length = length - sizeof(draw_head);
    tmp = (draw_ap_info *)(&buf[sizeof(draw_head)]);
    while(length >= sizeof(draw_ap_info))
    {
	ap_info = (draw_ap_info *)tmp;
	setApDrawingManaged(ntohl(ap_info->sn),isManaged);
	tmp = (draw_ap_info *)(ap_info + 1);
	length = length - sizeof(draw_ap_info);
    }
}
static void formApDrawingEdit(webs_t wp, char * path, char * query)
{
    char *length_str, *width_str;
    unsigned short length = 0;
    unsigned short width  = 0;
    char *file = NULL;
    char *info;
    unsigned int x = 0, y = 0;
    unsigned int sn = 0;
    char buf[1024], str[1024], path_obj[256];
    char *tmp = NULL, *ptr = NULL;
    draw_head *header = NULL;
    int ret = 0,i = 0,j = 0,flag = 0;
    int fp;

    memset(buf, 0, sizeof(buf));
    memset(str, 0, sizeof(str));
    memset(path_obj, 0, sizeof(path_obj));

    length_str = websGetVar(wp, "bgimg_h", T(""));
    width_str = websGetVar(wp, "bgimg_w", T(""));


    file = websGetVar(wp, "path_obj", T(""));
    info = websGetVar(wp, "apsinfo", T(""));
    DBCK("file: %s, info: %s\n", file, info);
    if((file == NULL) || (strcmp(file,"") == 0) || (strcmp(file,"default.jpg") == 0 && (0 == isDefaultJPGExist)))
    {
	websRedirect(wp,"DrawingPreview.asp");
	return;
    }

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(path_obj)) {
	strcat(path_obj, USB_DRAWING_PATH);
    } else
#endif
    {
#if (UTT_WX_2000 == FYES)
	strcat(path_obj, "/ff/");
	strcat(path_obj, USB_DRAWING_PATH);
#else
	strcat(path_obj, RAM_DRAWING_PATH);
#endif
    }

    strcat(path_obj, file);
    length = (Uint16)strtoul(length_str, NULL, 10);
    width = (Uint16)strtoul(width_str, NULL, 10);
    DBCK("path_obj: %s\n", path_obj);

    draw_head_init(buf, length, width);
    checkPicBuf(path_obj,0);//检出图纸的上一个AP管理状态
    tmp = strtok_r(info, ",", &ptr);
    while (tmp != NULL) {
	DBCK("%s\n", tmp);

	x = 0;
	y = 0;
	sn = 0;
	ret = sscanf(tmp, "%u-%u-%u", &x, &y, &sn);
	DBCK("x = %d, y = %d, sn = %d, ret = %d\n", x, y, sn, ret);
	if (ret == 3) {
	    add_ap_info(buf, x, y, sn);
	    setApDrawingManaged(sn,1);//dev_type设置为已被图纸管理
	}

	tmp = strtok_r(NULL, ",", &ptr);
    }
    DBCK("buf: %s\n", buf);

    header = (draw_head *)str;
    fp = open(path_obj, O_RDWR);
    if (fp != 0) {
	ret = lseek(fp, -1024, SEEK_END);
	if (ret != -1) {
	    read(fp, str, 1024);
	    if (header->magic != htonl(DRAW_MAGIC)) {
		write(fp, buf, 1024);
	    } else {
		ret = lseek(fp, -1024, SEEK_END);
		write(fp, buf, 1024);
	    }
	}

	close(fp);
    }

    memset(path_obj, 0 , sizeof(path_obj));
    sprintf(path_obj, "rm -rf %s/%s", JPG_PATH, file);
    DBCK("path_obj: %s", path_obj);
    system(path_obj);

    memset(path_obj, 0 , sizeof(path_obj));
    sprintf(path_obj, "DrawingPreview.asp?oldName=%s", file);
#if (UTT_NV_D908W == FYES) /*D908W不支持USB存储*/
    /*保存AP分布图到 'apDrawing' 分区*/
    apDrawingWriteToFlash();
#endif
out:
    websRedirect(wp, path_obj);
    return;
}
/*****************************************************************
 *功能：检索图纸，将AP位置找到，重定向到"建筑图纸页面"
 *作者：hec
 *日期：2014-07-31
 * ***************************************************************/
static void formFindApInPic(webs_t wp, char * path, char * query)
{
    char serialno[30] = {0}, *serialnoFromWeb = NULL;
    char path_obj[256] = {0}, file_name[128] = {0};
    char tmp_path[256] = {0}, buf[1024] = {0}, str[1024] = {0};
    DIR *dir_info;
    int fp, length, flag = 0, ret = 0, isFindPic = 0;
    struct dirent *dir_entry;
    struct stat buf1;
    const char *ErrMsg = NULL;
    draw_head *header = NULL;
    draw_ap_info *ap_info,*tmp;
    header = (draw_head *)str;
    serialnoFromWeb = websGetVar(wp,"serialNo",T(""));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(path_obj)) {
	strcat(path_obj, USB_DRAWING_PATH);
    } else
#endif
    {
#if (UTT_WX_2000 == FYES)
	strcat(path_obj, "/ff/");
	strcat(path_obj, USB_DRAWING_PATH);
#else
	strcat(path_obj, RAM_DRAWING_PATH);
#endif
    }
    dir_info = opendir(path_obj);
    if(dir_info != NULL)
    {
	while(1)
	{
	    dir_entry = readdir(dir_info);
	    if(dir_entry != NULL)
	    {
		if ((0!=strcmp(dir_entry->d_name,"."))&&(0!=strcmp(dir_entry->d_name,"..")))
		{
		    memset(file_name,0,sizeof(file_name));
		    memset(tmp_path,0,sizeof(tmp_path));
		    strncpy(file_name, dir_entry->d_name, strlen(dir_entry->d_name));
		    strcat(tmp_path,path_obj);
		    strcat(tmp_path,file_name);
		    if(isJPG(tmp_path) != 1)
		    {
			continue;
		    }
		    /*如果是目录下的图片，读出图片中存储的buf*/
		    flag = 0;
		    length = 0;
		    memset(buf,0,sizeof(buf));
		    if((0 == stat(tmp_path, &buf1)) && (S_ISREG(buf1.st_mode)))
		    {
			fp = open(tmp_path, O_RDWR);
			memset(str,0,sizeof(str));
			header = (draw_head *)str;
			if(fp != 0)
			{
			    ret = lseek(fp, -1024, SEEK_END);
			    if(ret != -1) 
			    {
				read(fp, str, 1024);
				if(header->magic == htonl(DRAW_MAGIC)) 
				{
				    ret = lseek(fp, -1024, SEEK_END);
				    read(fp, buf, 1024);
				    flag = 1;
				}
			    }
			    close(fp);
			}
			if(flag == 1)
			{
			    header = (draw_head *)buf;
			    length = ntohs(header->length);
			    length = length - sizeof(draw_head);
			    tmp = (draw_ap_info *)(&buf[sizeof(draw_head)]);
			    while(length >= sizeof(draw_ap_info))
			    {
				ap_info = (draw_ap_info *)tmp;
				memset(serialno,0,sizeof(serialno));
				sprintf(serialno,"%d",ntohl(ap_info->sn));
				if(strcmp(serialno,serialnoFromWeb) == 0)
				{
				    isFindPic = 1;
				    break;
				}
				tmp = (draw_ap_info *)(ap_info + 1);
				length = length - sizeof(draw_ap_info);
			    }            
			}
		    }
		}
		if(isFindPic == 1)
		{
		    break;
		}
	    }
	    else
	    {
		break;
	    }
	}
	closedir(dir_info);
    }
    memset(path_obj, 0 , sizeof(path_obj));
    if(isFindPic == 1)
    {
    sprintf(path_obj, "DrawingPreview.asp?oldName=%s&serialno=%s", file_name, serialnoFromWeb);
    }
    else
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CAN_NOT_LOCATION_AP);
	setTheErrorMsg((char *)ErrMsg);
	sprintf(path_obj, "DrawingPreview.asp?oldName=%s&serialno=%s", "", "");
    }
    websRedirect(wp, path_obj);
    return;
}
/*****************************************************************
 *功能：将未被"建筑图纸管理"的AP信息输出到页面
 *作者：hec
 *日期：2014-07-24
 * ***************************************************************/
static void put_urcpDevTable_unmanaged_ap(webs_t wp)
{
    unsigned int i = 0u,j = 0u,k = 0u;
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t* tmp = NULL;
    int tb_index,val;
    char str[48];
    mem_shadow = get_shm_ptr();
    
    websWrite(wp,"var userialnum = new Array();\n");
    websWrite(wp,"var ustatue = new Array();\n");
    websWrite(wp,"var uapname = new Array();\n");
    websWrite(wp,"var uapmodel = new Array();\n");
    websWrite(wp,"var uapip = new Array();\n");
    websWrite(wp,"var uapmac = new Array();\n");
    websWrite(wp,"var UNMAC = new Array();\n");
    websWrite(wp,"var uclienCt = new Array();\n");
    websWrite(wp,"var umanaged_enable = new Array();\n");
    websWrite(wp,"var uchannel = new Array();\n");
    websWrite(wp,"var ussid = new Array();\n");
    if(mem_shadow != NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index - 1]);
		val = tmp->dev_type;
		memset(str, 0, sizeof(str));
		if ((val & DEV_TYPE_DRAWING_MANAGED) == 0 && tmp->aged != 2) 
		{
		    //向asp写AP信息，这些AP都是未被"建筑图纸管理"的
		    // userialnum 
		    websWrite(wp,"userialnum[%d] = \"%u\";\n",j,tmp->serial_no);
		    // ustatue
		    if(tmp->aged == 2)
		    {
			websWrite(wp,"ustatue[%d] = \"0\";\n",j);
		    }
		    else
		    {
			websWrite(wp,"ustatue[%d] = \"2\";\n",j);
		    }
		    // uapname 
		    websWrite(wp,"uapname[%d] = \"%s\";\n",j,tmp->dev_name);
		    // uapmodel 
		    websWrite(wp,"uapmodel[%d] = \"%s\";\n",j,tmp->dev_model);
		    // uapip
		    websWrite(wp,"uapip[%d] = \"%s\";\n",j,int2ip(tmp->ip,str));
		    // uapmac
		    websWrite(wp,"uapmac[%d] = \"%02X%02X%02X%02X%02X%02X\";\n",j,tmp->wl_info.ssid_info[0].mac[0],tmp->wl_info.ssid_info[0].mac[1],tmp->wl_info.ssid_info[0].mac[2],tmp->wl_info.ssid_info[0].mac[3],tmp->wl_info.ssid_info[0].mac[4],tmp->wl_info.ssid_info[0].mac[5]);
		    websWrite(wp,"UNMAC[%d] = \"%02X%02X%02X%02X%02X%02X\";\n",j,tmp->mac[0],tmp->mac[1],tmp->mac[2],tmp->mac[3],tmp->mac[4],tmp->mac[5]);
		    // uclienCt 
		    websWrite(wp,"uclienCt[%d] = \"%d\";\n",j,tmp->wl_info.clients);// 
		    // umanaged_enable
		    if(val&DEV_FLAG_MANAGE)
		    {
			websWrite(wp,"umanaged_enable[%d] = \"1\";\n",j);
		    }
		    else
		    {
			websWrite(wp,"umanaged_enable[%d] = \"0\";\n",j);
		    }
		    // uchannel
		    if(tmp->wl_info.channel_head)//无效信道
		    {
			websWrite(wp,"uchannel[%d] = \"\";\n",j);
		    }
		    else
		    {
			websWrite(wp,"uchannel[%d] = \"%u\";\n",j,tmp->wl_info.channel_end);
		    }
		    // ussid
		    memset(str, 0, sizeof(str));
		    if(val&DEV_FLAG_MANAGE)
		    {
			 val = tmp->wl_info.joined_sz_num;
			 if(val>MAX_SSID_NUM)
			 {
			    val = MAX_SSID_NUM; 
			 }
		    }
		    else
		    {
			val = 1;	
		    }
		    for(k = 0;k < val;k++)
		    {
			if(strcmp(tmp->wl_info.ssid_info[k].ssid,"") != 0)
			{
			    strcat(str,tmp->wl_info.ssid_info[k].ssid);
			    strcat(str,",");
			}
		    }
		    k = strlen(str);
		    str[k-1] = 0;//去掉末尾的','
		    websWrite(wp,"ussid[%d] = \"%s\";\n",j,str);//

		    j++;
		}
		tb_index = tmp->tb_next;
	    }
	}
    }
}

urcp_dev_t* urcp_dev_table_lookup_by_sn(unsigned int sn)
{
#if 0
    unsigned char std_mac[MAC_LEN];
    urcp_dev_t *tmp = NULL;

	std_mac[0] = 0x00;
	std_mac[1] = 0x22;
	std_mac[2] = 0xaa;
	std_mac[3] = (0xFF & (sn >> 16));
	std_mac[4] = (0xFF & (sn >> 8));
	std_mac[5] = (0xFF & sn);

	DBCK("%x %x %x %x %x %x", std_mac[0], std_mac[1], std_mac[2], std_mac[3], std_mac[4], std_mac[5]);
    tmp = urcp_dev_table_lookup(std_mac, 0);

	return tmp;
#else
    unsigned int i = 0u;
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t* tmp = NULL;
    int tb_index;

    mem_shadow = get_shm_ptr();

	if(mem_shadow != NULL)
	{
		for(i = 0u; i < DEV_HASH_SIZE; i++)
		{
			tb_index = mem_shadow->dev_tb_hash[i];

			while(tb_index!=NONE_NODE)
			{
				tmp = &(mem_shadow->dev_table[tb_index - 1]);

				if (tmp->serial_no == sn) {
					return tmp;
				}

				tb_index = tmp->tb_next;
			}
		}
	}

	return NULL;
#endif
}
/*
 *功能：向页面写已经保存的图纸信息
 *作者：hec
 *日期：2014-07-29
 * **************************************************************************/
static void putAllJPG(webs_t wp, char *tmp_path)
{
    uint32 ii = 0u;
    unsigned char tmpOutSaved[256], file_name[128];
    DIR *dir_info;
    struct dirent *dir_entry;
    struct stat buf1;
    char delStrDotJPG[50] = {0};//*addZero = NULL;
    char tmp_pic[256] = {0};
    
    websWrite(wp, "var drawings = new Array();\n");
    dir_info = opendir(tmp_path);
    if (NULL != dir_info) 
    {
	while (1) 
	{
	    dir_entry = readdir(dir_info);
	    if (NULL != dir_entry) 
	    {
		if ((0!=strcmp(dir_entry->d_name,"."))&&(0!=strcmp(dir_entry->d_name,".."))) 
		{
		    memset(tmpOutSaved, 0, sizeof(tmpOutSaved));
		    memset(file_name, 0, sizeof(file_name));
		    strncpy(file_name, dir_entry->d_name, strlen(dir_entry->d_name));
		    strcat(tmpOutSaved, tmp_path);
		    strcat(tmpOutSaved, dir_entry->d_name);
		    if (1 != isJPG(tmpOutSaved)) 
		    {
			continue;
		    }
		    if ((0 == stat(tmpOutSaved, &buf1)) && (S_ISREG(buf1.st_mode))) 
		    {
			memset(delStrDotJPG,0,sizeof(delStrDotJPG));
			strncpy(delStrDotJPG,dir_entry->d_name,strlen(dir_entry->d_name));
			//addZero = gstrrchr(delStrDotJPG,'.');
			//*(addZero) = 0;
			memset(tmp_pic,0,sizeof(tmp_pic));
			strcpy(tmp_pic,tmp_path);
			strcat(tmp_pic,delStrDotJPG);
			checkPicBuf(tmp_pic,1);
			websWrite(wp,"\ndrawings[%d]=\"%s\";",ii,delStrDotJPG);
			ii = ii + 1u;
			if (ii >= MAX_AP_MANAGED * 2) 
			{
			    break;
			}
		    }
		}
	    }
	    else 
	    {
		break;
	    }
	}
	closedir(dir_info);
    }
    return;
}
extern void apDrawingInit(void)
{
    char *defaultFile = "default.jpg";
    char tmp[1024],tmpDefault[1024],tmpDefault1[1024],tmpCMD[1024];
    FILE *fp;
    memset(tmp,0,sizeof(tmp));
    memset(tmpDefault,0,sizeof(tmpDefault));
    memset(tmpDefault1,0,sizeof(tmpDefault1));
    memset(tmpCMD,0,sizeof(tmpCMD));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(tmp)) 
    {
	strcat(tmp, USB_DRAWING_PATH);
    } 
    else
#endif
    {
#if (UTT_WX_2000 == FYES)
	strcat(tmp, "/ff/");
	strcat(tmp, USB_DRAWING_PATH);
#else
	strcat(tmp, RAM_DRAWING_PATH);
#endif
    }
    /*检查路径下边有没有default.jpg,若没有拷贝过去*/
    strcat(tmpDefault,DEFAULT_JPG_PATH);
    strcat(tmpDefault,defaultFile);
    strcat(tmpDefault1,tmp);
    strcat(tmpDefault1,defaultFile);
    fp = fopen(tmpDefault1,"rb");
    if(NULL == fp)
    {
	fp = fopen(tmpDefault,"rb");
	if(fp != NULL)
	{
	    fclose(fp);
	    sprintf(tmpCMD,"cp -af %s %s",tmpDefault,tmpDefault1);
	    system(tmpCMD);
	}
    }
    else
    {
	fclose(fp);
    }
}
static void isLastcheckedJPGExist(void)
{
    char tmp[1024];
    FILE *fp;
    memset(tmp, 0, sizeof(tmp));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(tmp)) 
    {
	strcat(tmp, USB_DRAWING_PATH);
    } 
    else
#endif
    {
#if (UTT_WX_2000 == FYES)
	strcat(tmp, "/ff/");
	strcat(tmp, USB_DRAWING_PATH);
#else
	strcat(tmp, RAM_DRAWING_PATH);
#endif
    }
    strcat(tmp,lastCheckedFileName);
    fp = fopen(tmp,"rb");
    if(NULL == fp)
    {
	memset(lastCheckedFileName,0,sizeof(lastCheckedFileName));
    }
    else
    {
	fclose(fp);
    }
    return;
}
static int aspOutPutApDrawingEdit(int eid, webs_t wp, int argc, char **argv)
{
    char *filename = NULL,*checkFilename = NULL,MAC[48] = {0};
    draw_head *header;
    draw_ap_info *ap_info;
    char buf[1024], tmp[1024], str[48], tp[256], tp1[256],str1[48];
    int ret = 0;
    unsigned short hlen = 0, length = 0, status = 0;
    urcp_dev_t* tmp_dev = NULL;
    FILE *fp;
    unsigned i = 0;
    int val,k;
    filename = websGetVar(wp, "filename", T(""));
    if((checkFilename = gstrrchr(filename,' ')) != NULL)
    {
	filename = ++checkFilename;
    }
    if(strcmp(filename,"") != 0)
    {
	strcpy(lastCheckedFileName,filename);
    }
    if(strcmp(lastCheckedFileName,"") != 0)
    {
	isLastcheckedJPGExist();
    }
    if((strcmp(filename,"") == 0) && (strcmp(lastCheckedFileName,"") != 0))
    {
	strcpy(filename,lastCheckedFileName);
    }
    DBCK("filename = %s\n", filename);
    memset(tmp, 0, sizeof(tmp));
    memset(buf, 0, sizeof(buf));
    memset(tp, 0, sizeof(tp));
    memset(tp1, 0, sizeof(tp1));

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(tmp)) 
    {
	strcat(tmp, USB_DRAWING_PATH);
    } 
    else
#endif
    {
#if (UTT_WX_2000 == FYES)
	strcat(tmp, "/ff/");
	strcat(tmp, USB_DRAWING_PATH);
#else
	strcat(tmp, RAM_DRAWING_PATH);
#endif
    }
#if 0
    setALLDrawingUmanaged();
#endif
    putAllJPG(wp,tmp);
    strcat(tp, tmp);
    strcat(tp, filename);
    strcat(tp1, JPG_PATH);
    strcat(tp1, filename);
    if(strcmp(filename,"") != 0)
    {
	sprintf(tmp, "cp -af %s %s", tp, tp1);
	system(tmp);
    }
    DBCK("tp = %s, tp1 = %s, tmp = %s\n", tp, tp1, tmp);
    websWrite(wp, "var path_obj = \"%s\";\n", filename);
    websWrite(wp, "var height = %d;\n", 0);
    websWrite(wp, "var width = %d;\n", 0);
    websWrite(wp, "var max_num = \"%d\";\n", MAX_AP_MANAGED);
    websWrite(wp, "var x = new Array();\n");
    websWrite(wp, "var y = new Array();\n");
    websWrite(wp, "var serialnum = new Array();\n");
    websWrite(wp, "var statue = new Array();\n");
    websWrite(wp, "var apname = new Array();\n");
    websWrite(wp, "var apmodel = new Array();\n");
    websWrite(wp, "var apip = new Array();\n");
    websWrite(wp, "var apmac = new Array();\n");
    websWrite(wp, "var clienCt = new Array();\n");
    websWrite(wp, "var managed_enable = new Array();\n");
    websWrite(wp,"var channel = new Array();\n");
    websWrite(wp,"var ssid = new Array();\n");
    websWrite(wp,"var MAC = new Array();\n");

    header = (draw_head *)buf;
    if(strcmp(filename,"") == 0)
    {
	goto nofile;
    }
    memset(tmp, 0, sizeof(tmp));
    fp = fopen(tp, "rb");
    if (fp != NULL) 
    {
	ret = fseek(fp, -1024, SEEK_END);
	if (ret != -1) 
	{
	    fread(buf, 1u, sizeof(buf), fp);
	    hlen = ntohs(header->hlen);
	    length = ntohs(header->length);
	    if (header->magic != htonl(DRAW_MAGIC)) 
	    {
		DBCK("no ap info, do nothing.................");
	    } 
	    else 
	    {
		DBCK("has ap info----------------------------");
		websWrite(wp, "height = %d;\n", ntohs(header->height));
		websWrite(wp, "width = %d;\n", ntohs(header->width));
		while (hlen < length) 
		{
		    memset(str, 0, sizeof(str));
		    memset(MAC,0,sizeof(MAC));
		    ap_info = (draw_ap_info *)(&buf[hlen]);
		    websWrite(wp, "serialnum[%d] = \"%d\";\n", i, ntohl(ap_info->sn));
		    websWrite(wp, "x[%d] = \"%d\";\n", i, ntohs(ap_info->x));
		    websWrite(wp, "y[%d] = \"%d\";\n", i, ntohs(ap_info->y));

		    DBCK("sn = %d", ntohl(ap_info->sn));
		    if (ntohl(ap_info->sn) != 0) 
		    {
			setApDrawingManaged(ntohl(ap_info->sn),1);
			tmp_dev = urcp_dev_table_lookup_by_sn(ntohl(ap_info->sn));
			if (tmp_dev != NULL) 
			{
			    if (tmp_dev->aged == 0) 
			    {
				websWrite(wp, "statue[%d] = \"2\";\n", i);
			    }
			    else if (tmp_dev->aged == 2) 
			    {
				websWrite(wp, "statue[%d] = \"0\";\n", i);
			    } 
			    else 
			    {
				websWrite(wp, "statue[%d] = \"2\";\n", i);
			    }
			    websWrite(wp, "apname[%d] = \"%s\";\n", i, tmp_dev->dev_name);
			    websWrite(wp, "apmodel[%d] = \"%s\";\n", i, tmp_dev->dev_model);
			    websWrite(wp, "apip[%d] = \"%s\";\n", i, int2ip(tmp_dev->ip, str));
			    sprintf(str, "%02X%02X%02X%02X%02X%02X", tmp_dev->wl_info.ssid_info[0].mac[0],tmp_dev->wl_info.ssid_info[0].mac[1],tmp_dev->wl_info.ssid_info[0].mac[2],tmp_dev->wl_info.ssid_info[0].mac[3],tmp_dev->wl_info.ssid_info[0].mac[4],tmp_dev->wl_info.ssid_info[0].mac[5]);
			    sprintf(MAC,"%02X%02X%02X%02X%02X%02X",tmp_dev->mac[0],tmp_dev->mac[1],tmp_dev->mac[2],tmp_dev->mac[3],tmp_dev->mac[4],tmp_dev->mac[5]); 
			    websWrite(wp, "apmac[%d] = \"%s\";\n", i, str);
			    websWrite(wp, "MAC[%d] = \"%s\";\n", i, MAC);
			    websWrite(wp, "clienCt[%d] = \"%d\";\n", i, tmp_dev->wl_info.clients);
			    val = tmp_dev->dev_type;
			    /*是否可管理, 不支持管理协议的设备，未授权的设备*/
			    if(val&DEV_FLAG_MANAGE) 
			    {
				websWrite(wp, "managed_enable[%d] = \"1\";\n", i);
			    }
			    else
			    {
				websWrite(wp, "managed_enable[%d] = \"0\";\n", i);
			    }
			    // channel
			    if(tmp_dev->wl_info.channel_head)//无效信道
			    {
				websWrite(wp,"channel[%d] = \"\";\n",i);
			    }
			    else
			    {
				websWrite(wp,"channel[%d] = \"%u\";\n",i,tmp_dev->wl_info.channel_end);
			    }
			    // ssid
			    memset(str1, 0, sizeof(str1));
			    if(val&DEV_FLAG_MANAGE)
			    {
				 val = tmp_dev->wl_info.joined_sz_num;
				 if(val>MAX_SSID_NUM)
				 {
				    val = MAX_SSID_NUM; 
				 }
			    }
			    else
			    {
				val = 1;
			    }
			    for(k = 0;k < val;k++)
			    {
				if(strcmp(tmp_dev->wl_info.ssid_info[k].ssid,"") != 0)
				{
				    strcat(str1,tmp_dev->wl_info.ssid_info[k].ssid);
				    strcat(str1,",");
				}
			    }
			    k = strlen(str1);
			    str1[k-1] = 0;//去掉末尾的','
			    websWrite(wp,"ssid[%d] = \"%s\";\n",i,str1);//
			} 
			else 
			{
			    websWrite(wp, "statue[%d] = \"1\";\n", i);
			    websWrite(wp, "apname[%d] = \"\";\n", i);
			    websWrite(wp, "apmodel[%d] = \"\";\n", i);
			    websWrite(wp, "apip[%d] = \"\";\n", i);
			    websWrite(wp, "apmac[%d] = \"\";\n", i);
			    websWrite(wp, "MAC[%d] = \"\";\n", i);
			    websWrite(wp, "clienCt[%d] = \"\";\n", i);
			    websWrite(wp, "managed_enable[%d] = \"0\";\n", i);
			    websWrite(wp,"channel[%d] = \"\";\n",i);
			    websWrite(wp,"ssid[%d] = \"\";\n",i);
			}
		    } 
		    else 
		    {
			websWrite(wp, "statue[%d] = \"1\";\n", i);
			websWrite(wp, "apname[%d] = \"\";\n", i);
			websWrite(wp, "apmodel[%d] = \"\";\n", i);
			websWrite(wp, "apip[%d] = \"\";\n", i);
			websWrite(wp, "apmac[%d] = \"\";\n", i);
			websWrite(wp, "MAC[%d] = \"\";\n", i);
			websWrite(wp, "clienCt[%d] = \"\";\n", i);
			websWrite(wp, "managed_enable[%d] = \"0\";\n", i);
			websWrite(wp,"channel[%d] = \"\";\n",i);
			websWrite(wp,"ssid[%d] = \"\";\n",i);
		    }
		    hlen += sizeof(draw_ap_info);
		    i++;
		}
	    }
	}
	fclose(fp);
    }
nofile:
    put_urcpDevTable_unmanaged_ap(wp);
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static int isJPG(const char *path)
{
	int ret = 0;
	char *tmp = path;
	unsigned len = strlen(path);
	FILE *fp = NULL;
	draw_head *header;
	unsigned short hlen = 0, length = 0, status = 0;
	unsigned char buf[1027];

	memset(buf, 0, sizeof(buf));

	/* 文件必须保持JPG后缀，给页面识别用*/
	DBCK("path = %s", path);
	if (0 == strncasecmp(tmp + len - strlen(".jpg"), ".jpg", 4)) {
		fp = fopen(path, "rb");
		if (!fp) {
			return 0;
		}

		fread(buf, 1u, sizeof(buf), fp);
		if ((buf[0] == 0xff) && (buf[1] == 0xd8) && (buf[2] == 0xff) && buf[3] <= 0xef && buf[3] >=0xe0) {
			/*0xffd9 0x0a*/
			fseek(fp, -1027, SEEK_END);
			fread(buf, 1u, sizeof(buf), fp);

			header = (draw_head *)(buf + 3);
			DBCK("buf[0] = %x, buf[1] = %x, buf[2] = %x", buf[0], buf[1], buf[2]);
			DBCK("buf[1024] = %x, buf[1025] = %x buf[1026] = %x", buf[1024], buf[1025], buf[1026]);
			DBCK("header->magic = %x", header->magic);
			if (header->magic == htonl(DRAW_MAGIC)) {
				if (((buf[0] == 0xff) && (buf[1] == 0xd9)) ||
					((buf[1] == 0xff) && (buf[2] == 0xd9))) {
					ret = 1;
				}
			} else {
				if (((buf[1024] == 0xff) && (buf[1024 + 1] == 0xd9)) ||
					((buf[1024 + 1] == 0xff) && (buf[1024 + 2] == 0xd9))) {
					ret = 1;
				}
			}
		}

		fclose(fp);
	}

	return ret;
}

static void aspOutDirDrawing(webs_t wp, const char *path, unsigned int *out_index)
{
	unsigned char tmp[256], file_name[128];
	unsigned char buf[256];
	off_t size1;
	DIR *dir_info;
	FILE *fp = NULL;
	struct dirent *dir_entry;
	struct stat buf1;
	struct tm *t;

	dir_info = opendir(path);
	if (NULL != dir_info) {
		while (1) {
			dir_entry = readdir(dir_info);
			if (NULL != dir_entry) {
				if ((0 != strcmp(dir_entry->d_name, ".")) && (0 != strcmp(dir_entry->d_name, ".."))) {
					memset(tmp, 0, sizeof(tmp));
					memset(file_name, 0, sizeof(file_name));
					strncpy(file_name, dir_entry->d_name, strlen(dir_entry->d_name));
					strcat(tmp, path);
					strcat(tmp, dir_entry->d_name);

					if (1 != isJPG(tmp)) {
						continue;
					}

					if ((0 == stat(tmp, &buf1)) && (S_ISREG(buf1.st_mode))) {
						websWrite(wp, "DrawingNames[%d] = \"%s\";\n", *out_index, dir_entry->d_name);
						size1 = buf1.st_size;

						if (size1 >= 1024) {
							size1 = size1 >> 10;
							websWrite(wp, "DrawingSizes[%d] = \"%dKB\";\n", *out_index, size1);
						} else {
							websWrite(wp, "DrawingSizes[%d] = \"%dB\";\n", *out_index, size1);
						}


						*out_index = *out_index + 1u;
						if (*out_index >= MAX_AP_MANAGED * 2) {
							break;
						}
					}
				}
			}
			else {
				break;
			}
		}

		closedir(dir_info);
	}

	return;
}
static int aspOutPutApPositionDraw(int eid, webs_t wp, int argc, char **argv)
{
    const char *ErrMsg = NULL;
	char tmp_path[256];
    uint32 i=0u;

    websWrite(wp, "var DrawingNames = new Array();\n");
    websWrite(wp, "var DrawingSizes = new Array();\n");

	memset(tmp_path, '\0', sizeof(tmp_path));

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_DRAWING_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_DRAWING_PATH);
#else
		strcat(tmp_path, RAM_DRAWING_PATH);
#endif
	}

    /*读取内存文件*/
    aspOutDirDrawing(wp, tmp_path, &i);

	websWrite(wp, "var totalrecs = \"%d\";\n", i);
#if (UTT_NV_D908W == FYES) /*D908W不支持USB存储*/
/*D908W 仅支持上传一张AP图纸*/
	websWrite(wp, "var max_totalrecs = \"%d\";\n", 2);
#else
	websWrite(wp, "var max_totalrecs = \"%d\";\n", MAX_AP_MANAGED * 2);
#endif

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

static void formApDrawingReName(webs_t wp, char_t *path, char_t *query)
{
    char *old_name, *new_name;
    char buf1[256], buf2[256];
    const char *ErrMsg = NULL;
	char tmp_path[256];

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)){
		strcat(tmp_path, USB_DRAWING_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_DRAWING_PATH);
#else
		strcat(tmp_path, RAM_DRAWING_PATH);
#endif
	}

    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
//	tmp = websGetVar(wp, "statustr", T(""));
    old_name = websGetVar(wp, "oldName", T(""));
    new_name = websGetVar(wp, "newName", T(""));

	if (NULL != tmp_path) {
		strcat(buf1, tmp_path);
		strcat(buf1, old_name);
		strcat(buf2, tmp_path);
		strcat(buf2, new_name);
	}

	if (0 != rename(buf1, buf2)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_RENAME_FAILURE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("rename failure"));
	}

    websRedirect(wp, "DrawingPreview.asp");

    return;
}

static void formApDrawingDelAll(webs_t wp, char_t *path, char_t *query)
{
	char tmp[512];
	char tmp_path[256];

	setALLDrawingUmanaged();
	memset(lastCheckedFileName,0,sizeof(lastCheckedFileName));
	memset(tmp, '\0', sizeof(tmp));
	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_DRAWING_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_DRAWING_PATH);
#else
		strcat(tmp_path, RAM_DRAWING_PATH);
#endif
	}

	sprintf(tmp, "rm -rf %s./*", tmp_path);

	system(tmp);
	wait(NULL);
#if (UTT_NV_D908W == FYES) /*D908W不支持USB存储*/
	/*保存AP分布图到 'apDrawing' 分区*/
	apDrawingWriteToFlash();
#endif
    isDefaultJPGExist = 0;
    websRedirect(wp, "DrawingPreview.asp");

    return;
}

static void formApDrawingDel(webs_t wp, char_t *path, char_t *query)
{
    char tp[256];
    char *filename = NULL, *isInUSB=tp;
    char *tmp, *ptr;
    unsigned int i=0u, len, out=0u;
    const char *ErrMsg = NULL;
	char tmp_path[256],tmp_path_free[256];

	memset(tmp_path, '\0', sizeof(tmp_path));
	memset(tmp_path_free,0,sizeof(tmp_path_free));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_DRAWING_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_DRAWING_PATH);
#else
		strcat(tmp_path, RAM_DRAWING_PATH);
#endif
	}
    strcat(tmp_path_free,tmp_path);
    filename = websGetVar(wp, "delstr", T("")); /*要删除的文件名*/
    if((filename == NULL) || (strcmp(filename,"") == 0))
    {
	websRedirect(wp, "DrawingPreview.asp");
	return;
    }
    if(strcmp(filename,"default.jpg") == 0)
    {
	isDefaultJPGExist = 0;	
    }
    isInUSB = websGetVar(wp, "statustr", T("")); /*是否存在u盘中的文件*/
    tmp = strtok_r(filename, UTT_SPLIT_SIGN_STR, &ptr);/*可能删除多条映射*/
    strcat(tmp_path_free,tmp);
    checkPicBuf(tmp_path_free,0);
    if(strcmp(lastCheckedFileName,filename) == 0)
    {
	memset(lastCheckedFileName,0,sizeof(lastCheckedFileName));
	if(isDefaultJPGExist)
	{
	    strcpy(lastCheckedFileName,"default.jpg");
	}
    }
    while(tmp!=NULL)
    {
	    memset(tp, 0, sizeof(tp));
		len = sizeof(tp) - 1u - (unsigned int)strlen(RAM_DRAWING_PATH);
		strcat(tp, tmp_path);
	    strncat(tp, tmp, len);
	    if(unlink(tp)!=0)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("操作失败"));
			out=1u;
		}

		if(out==1u)
		{
		    break;
		}

		i++;
	/*继续*/
	tmp = strtok_r((char*)NULL, UTT_SPLIT_SIGN_STR, &ptr);
    }

#if (UTT_NV_D908W == FYES) /*D908W不支持USB存储*/
    /*保存AP分布图到 'apDrawing' 分区*/
    apDrawingWriteToFlash();
#endif
    websRedirect(wp, "DrawingPreview.asp");

    return;
}

static int aspOutPutUploadDrawing(int eid, webs_t wp, int argc, char_t **argv)
{
//    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

static void formUploadDrawing(webs_t wp, char * path, char * query)
{
	char *     fn;
	char *picSaveName = NULL;
	char *     bn = NULL;
	unsigned char *postData;
	int fnlen, len, count = 0, ret = 0, isUsb = 0; 
	image_header_t *head;
	FILE *p = NULL;
	int fd;
	char tp[128];
	const char *ErrMsg = NULL;
	char tmp_path[256];
	long long freeDisk = 0;

	a_assert(websValid(wp));

	fnlen = wp->lenPostData;
	postData = wp->postData;
	wp->postData = NULL;
	wp->lenPostData = 0;

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		if (-1 != storageFree(tmp_path, NULL, &freeDisk)) {
			if (freeDisk > IMAGE_SIZE) {
				isUsb = 1;
			} else {
				ErrMsg = getTransDataForC(C_LANG_INDEX_FLASH_SPACE_NOT_ENOUGH);
				setTheErrorMsg((char *)ErrMsg);
				goto OUT;
			}
		}
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		isUsb = 0;
#else
		strcat(tmp_path, "/tmp/urcp/");
		isUsb = 0;
#endif
	}
#if (UTT_NV_D908W == FYES) /*D908W不支持USB存储*/
	/*保存AP分布图，检测上传文件大小，限制为60KB*/
	if( fnlen > 60*1024 ) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_AP_DRAWING_FILE_LIMIT_D908W);
		setTheErrorMsg((char *)ErrMsg);
		goto OUT;
	}
#endif

	ramFree(&freeDisk);
	if (freeDisk <  IMAGE_SIZE) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_MEMERY_SPACE_NOT_ENOUGH);
		setTheErrorMsg((char *)ErrMsg);
		goto OUT;
	}

	fn = websGetVar(wp, T("filename"), T(""));
	picSaveName = websGetVar(wp, T("drawingName"), T(""));
	strcat(picSaveName,".jpg");
	if(strcmp(picSaveName,"default.jpg") == 0)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_CAN_NOT_UPLOAD_DEFAULT_JPG);
	    setTheErrorMsg((char *)ErrMsg);
	    goto OUT;
	}
	if (fn != NULL && *fn != '\0') {        
		//if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
		if ((bn = gstrrchr(fn, '\\')) != NULL) {
			fn = bn+1;
		}
		//}
	}

	DBCK("fn = %s", fn);

	if (0 != strncasecmp(fn + strlen(fn) - strlen(".jpg"), ".jpg", 4)) {
		ErrMsg=getTransDataForC(C_LANG_INDEX_CONFIG_FILE_FORMAT);
		setTheErrorMsg((char *)ErrMsg);
		goto OUT;
	}
//	strncpy(fn + strlen(fn) - strlen(".jpg"), ".jpg", 4);//替换扩展名为小写

	DBCK("%x, %x, %x, %x", postData[0], postData[1], postData[2], postData[3]);
	if ((postData[0] != 0xff) || (postData[1] != 0xd8) || (postData[2] != 0xff) || postData[3] > 0xef || postData[3] <0xe0) {
		ErrMsg=getTransDataForC(C_LANG_INDEX_CONFIG_FILE_FORMAT);
		setTheErrorMsg((char *)ErrMsg);
		goto OUT;
	}
	else if((picSaveName != NULL) && (strcmp(picSaveName,"") != 0))
	{
	    memset(lastCheckedFileName,0,sizeof(lastCheckedFileName));
	    strcpy(lastCheckedFileName,picSaveName);
	}
	
	if (isUsb == 0) {
		// 没有U盘的情况
		// 这个条件语句的前一段代码可以合并啊
		memset(tp, 0, sizeof(tp));
		strcat(tp, tmp_path);
		strcat(tp, USB_DRAWING_PATH);
//		strcat(tp, fn);
		strcat(tp,picSaveName);
		p = fopen(tp, "wb");
		if(p != NULL)
		{
			if(fwrite(postData, fnlen, 1, p) != 1)
			{
				printf("write file error !!!\n");
			}
			fclose(p);

		}
		else
		{
			printf("open file error !!!");
			goto OUT;
		}
	} else {
		// 有U盘的情况
		memset(tp, 0, sizeof(tp));
		strcat(tp, tmp_path);
		strcat(tp, USB_DRAWING_PATH);
//		strcat(tp, fn);
		strcat(tp,picSaveName);
		fd = open(tp, O_WRONLY | O_CREAT);
		if (fd != -1) {
			len = write(fd, postData, fnlen);
			if (len != fnlen) {
				close(fd);
				unlink(tmp_path);
			}

			close(fd);

		}
	}

OUT:
	sync();//将缓存的数据写入存储区，防止程序异常结束损坏文件
	doSystem("echo 3 > /proc/sys/vm/drop_caches");
	if(postData) {
		bfree(B_L, postData);
	}
#if (UTT_NV_D908W == FYES) /*D908W不支持USB存储*/
	/*保存AP分布图到 'apDrawing' 分区*/
	apDrawingWriteToFlash();
#endif
	websRedirect(wp, "ImportDrawing.asp");
	return;
}

extern void formDefineApDrawing(void)
{
    websAspDefine(T("aspOutPutApPositionDraw"), aspOutPutApPositionDraw);
    websFormDefine(T("formApDrawingDel"), formApDrawingDel);
    websFormDefine(T("formApDrawingDelAll"), formApDrawingDelAll);
    websFormDefine(T("formApDrawingReName"), formApDrawingReName);
    websAspDefine(T("aspOutPutUploadDrawing"), aspOutPutUploadDrawing);
    websFormDefine(T("formUploadDrawing"), formUploadDrawing);
    websAspDefine(T("aspOutPutApDrawingEdit"), aspOutPutApDrawingEdit);
    websFormDefine(T("formApDrawingEdit"), formApDrawingEdit);
    websFormDefine(T("formFindApInPic"), formFindApInPic);
}
#endif
