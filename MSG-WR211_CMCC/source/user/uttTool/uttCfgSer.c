
/******************************************************服务端程序****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/switch/switch.h>
#include <net/if.h>
#include <sys/utsname.h>
#include <linux/sockios.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include "cJSON.h"
#include<string.h>
#include"uttMachine.h"
#include"mib.h"
#include"profacce.h"
#include"comExeFun.h"


#define AP_IF_NAME      "br0"
#define ETH_P_ALL       0x003
#define URCP_BUF_SIZE          1522u
#define MAC_LEN             6u
#define ETH_URCP_OFFSET        14u
#define ETH_P_URCP            0x9988
#define MAX_FRAME_LENGTH         1518u
#define TimeOutCount 60u

#define MALLOC_DES_SIZE 1024u


unsigned char DES_KEY_RPT[9] ;
static unsigned int uttSendLen = 0;
static unsigned int exchCount = 0;
static unsigned int client_sn = 0;
uchar mac_addr_broadcast[MAC_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
typedef struct _eth_hdr{
    uchar   dst_addr[MAC_LEN];
    uchar   src_addr[MAC_LEN];
    uint16  protocol;
}eth_hdr_t;

static void generate_sn_key(unsigned int sn,unsigned char *des,unsigned char *key)
{
    unsigned char snsn[4] = {0};
    int i;
    memcpy(snsn,&sn,4);
    for(i = 0;i<8;i++)
    {
	key[i] = ((snsn[i%4]) ^ (des[i]));
    }
    return ;
}
static int get_ip(char *ifname, uint32 *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("getIfIp: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
#if 0
		printf("getIfIp: ioctl SIOCGIFADDR error for %s", ifname);
#endif
		return -1;
	}

	*if_addr = (uint32)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);

	close(skfd);
	return 0;
}
static int get_mac(char *ifname, unsigned char *if_hw)
{
    struct ifreq ifr;
    int skfd;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("getIfMac: open socket error");
	return -1;
    }

    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
	close(skfd);
	printf("getIfMac: ioctl SIOCGIFHWADDR error for %s", ifname);
	return -1;
    }
    memcpy(if_hw, ifr.ifr_hwaddr.sa_data, MAC_LEN);
    close(skfd);
    return 0;
}

int urcp_buid_header(uchar buffer[URCP_BUF_SIZE],uchar dst_mac[MAC_LEN])
{
    uchar mac[MAC_LEN] ;
    int ret = 0;
    char tmp[12];
    memset(mac, 0, sizeof(mac));
    get_mac(AP_IF_NAME, mac);
    /*指向包头*/
    memcpy(((eth_hdr_t*)buffer)->dst_addr, dst_mac, MAC_LEN);/*DMAC*/    
    memcpy(((eth_hdr_t*)buffer)->src_addr, mac, MAC_LEN);/*SMAC*/
    ((eth_hdr_t*)buffer)->protocol = htons((uint16)ETH_P_URCP);
    sprintf(tmp, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", dst_mac[0], dst_mac[1],dst_mac[2],dst_mac[3],dst_mac[4],dst_mac[5],mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],buffer[12],buffer[13]);
    printf("tmp mac =%s\n",tmp);
    return ret;
}

int urcp_header_isvalue(uchar buffer[MAX_FRAME_LENGTH])
{
    int ret = 0 ;
    if(((eth_hdr_t*)buffer)->protocol == htons((uint16)ETH_P_URCP))
    {
	printf("xrgadd  line = %d , func = %s \n",__LINE__,__func__);
	ret = 1 ; 
    }
    return ret;
}

static int get_server_wifi_info(cJSON *json)
{
    MibProfileType mibType = MIB_PROF_WIRELESS;
    WirelessProfile *prof= NULL;
    prof = (WirelessProfile*)ProfGetInstPointByIndex(mibType, 0);

    cJSON *root , *fld;
    uchar version[4];
	char ssid[33],encrypt[32],passwd[64],autoMode[24],encodeType[8],wepkey1[64],wepkey2[64],wepkey3[64],wepkey4[64]; 
	int wepKeyType,wepkeyLength1,wepkeyLength2,wepkeyLength3,wepkeyLength4,wepKeyID;
    int ch;

    memset(ssid,0,sizeof(ssid));
    memset(encrypt,0,sizeof(encrypt));
    memset(encodeType,0,sizeof(encodeType));
    memset(autoMode,0,sizeof(autoMode));
    memset(passwd,0,sizeof(passwd));
    memset(version,0,sizeof(version));
	memset(wepkey1,0,sizeof(wepkey1));
	memset(wepkey2,0,sizeof(wepkey2));
	memset(wepkey3,0,sizeof(wepkey3));
	memset(wepkey4,0,sizeof(wepkey4)); 


    strcpy(version,"1.0");
    strcpy(passwd,prof->mSafeCf.ap.AuthMode.pskPsswd); 
    strcpy(autoMode,prof->mSafeCf.SelAuthMode);
    strcpy(encrypt,prof->mSafeCf.ap.AuthMode.EncrypType);
    strcpy(encodeType,prof->mBaseCf.encodeType);
    ch = prof->mBaseCf.Channel ;
    strcpy(ssid,prof->mBaseCf.SSID1);//修改无线ssid与对端一致
	strcpy(wepkey1,prof->mSafeCf.ap.AuthMode.wepkey1);
	strcpy(wepkey2,prof->mSafeCf.ap.AuthMode.wepkey2);
	strcpy(wepkey3,prof->mSafeCf.ap.AuthMode.wepkey3);
	strcpy(wepkey4,prof->mSafeCf.ap.AuthMode.wepkey4);
	wepkeyLength1 =  prof->mSafeCf.ap.AuthMode.wepkeyLength1;
	wepkeyLength2 =  prof->mSafeCf.ap.AuthMode.wepkeyLength2;
	wepkeyLength3 =  prof->mSafeCf.ap.AuthMode.wepkeyLength3;
	wepkeyLength4 =  prof->mSafeCf.ap.AuthMode.wepkeyLength4;
	wepKeyType = prof->mSafeCf.ap.AuthMode.wepKeyType;/* 秘钥格式*/
	wepKeyID = prof->mSafeCf.ap.AuthMode.wepKeyID;/*秘钥序号*/


    cJSON_AddStringToObject(json,"v",version);
    root = cJSON_CreateArray();
    cJSON_AddItemToArray(root,fld = cJSON_CreateObject());
    cJSON_AddStringToObject(fld,"ssid",ssid);
    cJSON_AddStringToObject(fld,"passwd",passwd);
    cJSON_AddStringToObject(fld,"autoMode",autoMode);
    cJSON_AddNumberToObject(fld,"ch",ch);
    cJSON_AddStringToObject(fld,"encrypt",encrypt);
    cJSON_AddStringToObject(fld,"encodeType",encodeType);
    
    cJSON_AddItemToObject(json,"wifi",root);/*加密方式*/
	cJSON_AddNumberToObject(fld,"wepKeyType" ,wepKeyType);
	cJSON_AddNumberToObject(fld,"wepKeyID" ,wepKeyID);
	cJSON_AddNumberToObject(fld,"wepkeyLength1" ,wepkeyLength1);
	cJSON_AddNumberToObject(fld,"wepkeyLength2" ,wepkeyLength2);
	cJSON_AddNumberToObject(fld,"wepkeyLength3" ,wepkeyLength3);
	cJSON_AddNumberToObject(fld,"wepkeyLength4" ,wepkeyLength4);
	cJSON_AddStringToObject(fld,"wepkey1",wepkey1);
	cJSON_AddStringToObject(fld,"wepkey2",wepkey2);
	cJSON_AddStringToObject(fld,"wepkey3",wepkey3);
	cJSON_AddStringToObject(fld,"wepkey4",wepkey4);
}


/*解析从客户端发过来的第一个数据包*/
static int parse_server_packet(uchar buffer[MAX_FRAME_LENGTH],uchar *mac_addr)
{
    uchar version[4] , cmd[8] , ip_char[10];

    int ret = 0 , result = 0 , sn = 0 ; 
    uchar buf[MAX_FRAME_LENGTH-ETH_URCP_OFFSET+1];
    memset(buf,0,sizeof(buf));

    ret = urcp_header_isvalue(buffer); 
    if(ret == 0)
    {
	//printf("xrgadd not need packet line = %d , func = %s \n",__LINE__,__func__);
	return ret;
    }
    
    desDecrypt(&buffer[ETH_URCP_OFFSET],buf,DES_KEY_RPT);/*解密文件到buf中*/
  /*  strcpy(buf,&buffer[ETH_URCP_OFFSET]);*/
    printf("\n\nbuf = %s \n",buf); 
   
    cJSON *json = cJSON_Parse(buf);
   
    memset(version,0,sizeof(version));
    memset(cmd,0,sizeof(cmd));
    memset(ip_char,0,sizeof(ip_char));

    strcpy(version,cJSON_GetObjectItem(json,"v")->valuestring);      
    strcpy(cmd,cJSON_GetObjectItem(json,"ct")->valuestring);      
    sn = cJSON_GetObjectItem(json,"sn")->valueint;     
    strcpy(mac_addr,cJSON_GetObjectItem(json,"mac")->valuestring);     
    strcpy(ip_char,cJSON_GetObjectItem(json,"ip")->valuestring) ;    

    if(strncmp(version,"1.0",3)==0)
    {
	client_sn = sn ;
	result = 1;
    }
    printf("\n\n version = %s,cmd = %s,sn_char = %d ,mac_addr = %s,ip_char = %s\n\n",version,cmd,sn,mac_addr,ip_char);
    return result;
}
static int  generate_reply_packet(uchar buffer[URCP_BUF_SIZE],uchar *mac_addr)
{
    char *respStr;
    cJSON *json;
    char DesBuffer[MALLOC_DES_SIZE] ;/*des加密后的buf*/ 
    unsigned char des_key[9];

    memset(DesBuffer,0,sizeof(DesBuffer));
    memset(des_key,0,sizeof(des_key));

    urcp_buid_header(buffer,mac_addr);/*建立发送帧头*/
    json = cJSON_CreateObject();/*创建json对象*/
    get_server_wifi_info(json);
    respStr = cJSON_Print(json);/*获取json对象封装的信息*/
    
    generate_sn_key(client_sn,DES_KEY_RPT,des_key);
    printf("sn = %x....the decrypt key = %2x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",client_sn,des_key[0],des_key[1],des_key[2],des_key[3],des_key[4],des_key[5],des_key[6],des_key[7]);
    desEncrypt(respStr,DesBuffer,des_key);/*des加密发送文件*/

    uttSendLen = strlen(DesBuffer)+14;
    printf("uttSendLen = %d\n",uttSendLen);
    cJSON_Delete(json);/*销毁json对象*/   
    
    strcpy(&buffer[ETH_URCP_OFFSET],DesBuffer);
}
int main(int argc ,char *argv[])
{

    int fd, ret , size , error , result = 0;
    struct ifreq req;
    struct sockaddr_ll myaddr_ll;
    uchar buffer[URCP_BUF_SIZE],mac_addr[MAC_LEN];
    char frame[MAX_FRAME_LENGTH+1]; 
    char *respStr;
    cJSON *json;

    int nNetTimeout = 5;

    memset(DES_KEY_RPT,0,sizeof(DES_KEY_RPT));
    memset(buffer,0,sizeof(buffer));
    memset(frame,0,sizeof(frame));
    memset(&req,0,sizeof(req));
  
    ProfInit();/*初始化来读取prof*/ 

    /*优化密码，将密码赋值放到代码段,且赋值打乱顺序*/
    DES_KEY_RPT[2] = 0x27 ;
    DES_KEY_RPT[4] = 0x52 ;
    DES_KEY_RPT[0] = 0xea ;
    DES_KEY_RPT[6] = 0x8f ;
    DES_KEY_RPT[3] = 0xb5 ;
    DES_KEY_RPT[7] = 0xf9 ;
    DES_KEY_RPT[1] = 0x56 ;
    DES_KEY_RPT[5] = 0x77 ;

    strcpy(req.ifr_name, AP_IF_NAME);

    fd = socket(AF_INET, SOCK_DGRAM,0);/*通过设备名称获取index*/ 
    if(fd<0)
    {
	perror("create SOCK_DGRAM error");
	return -1;
    }


    ret = ioctl(fd, SIOCGIFINDEX, &req);
    if (ret < 0)
    {
	perror("Get ifindex err");
	close(fd);
	return -1;
    }
    close(fd);

    /*set addr_ll*/
    memset(&myaddr_ll, 0, sizeof(myaddr_ll));
    myaddr_ll.sll_family  = PF_PACKET;
    myaddr_ll.sll_protocol = htons(ETH_P_URCP);
    myaddr_ll.sll_ifindex = req.ifr_ifindex;

    /*create raw socket*/
    if( (fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_URCP))) < 0)
    {
	perror("create raw socket error");
	return -1;
    }
    /*bind*/
    if (bind(fd, (struct sockaddr*)&myaddr_ll, sizeof(myaddr_ll)) < 0)
    {
	perror("bind socketaddr_ll error");
	return -1;
    }
    size = 300 * 1024 ;

    error = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int)); 
    if(error<0)
    { 
	perror("set socket buffer "); 
	return 0;
    }

    while(1)
    {
	memset(frame,0,sizeof(frame));
	ret = recv(fd, frame, MAX_FRAME_LENGTH, MSG_DONTWAIT);
	if(ret<0)
	{
	//    perror("recv");
	}	
//	printf("\n\nret = %d  fram= %s exchCount = %u\n\n",ret,frame,exchCount);

	if((result = parse_server_packet(frame,mac_addr))!=0)
	{
	    generate_reply_packet(buffer,mac_addr);

	    printf("\nsend long  = %u,,,buf = %s\n",uttSendLen,&buffer[14]);
	    ret = send(fd, buffer, uttSendLen, 0);
	    if(ret<0)
	    {
		perror("send");
	    }
	}
	if(++exchCount == TimeOutCount)
	{
	    break;
	}
	usleep(500000);
    }
    return 0;
}
/*************************************************e n d**************服务端程序************************************************/

