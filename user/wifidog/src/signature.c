#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <pthread.h>
#include "signature.h"
#include "uttMachine.h"
#include "mib.h"
#include "conf.h"
#include <linux/netfilter_ipv4/utt_netfilter.h>
/*
 * the signatureof wifidog's parameter to verify
 * 
 * 此文件主要是实现参数的签名
 * 主要定义一些签名中用到的结构体定义和函数接口声明
 * guo.deyuan
 * */


#ifdef PARAM_SIGN

pthread_mutex_t param_sign_mutex = PTHREAD_MUTEX_INITIALIZER;
#define EMPTY_ENTRY {"", "", 0}
/*
 * wifidog 参数添加，用于参数签名时使用
 * 与服务器的交互中用到的所有参数都需要在此添加
 * 添加方法:
 * 需要按照字符的ASCII比较由小到大按顺序添加，
 * 否则加密后的结果服务端无法验证通过
 * */
struct sign_t tmpSign[]= {
    {"1111","",0},
    {"2222","",0},
    {"deviceid", "", 0},
    {"expire", "", 0},
    {"gwaddress", "", 0},
    {"gwport", "", 0},
    {"ip", "", 0},
    {"mac", "", 0},
    {"sys_load", "", 0},
    {"sys_memfree", "", 0},
    {"sys_uptime", "", 0},
    {"timestamp", "", 0},
    {"token", "", 0},
    {"url","",0},
    {"v","",0},
    {"wifi_uptime", "", 0},
    EMPTY_ENTRY
};

extern unsigned char sha1InData[];
extern unsigned char sha1Data[];
extern unsigned char hexData[];

/*
 * findParamExist 
 * 查找需要验证的参数是否存在
 * name - 参数名
 *
 **/
static int findParamExist(char *name)
{
    int index = -1;
    int entrySize = 0;
    int i = 0;
    if(name != NULL) {
	entrySize = sizeof(tmpSign)/sizeof(struct sign_t);
	for(i=0;i<entrySize;i++) 
	{
	    if(strcmp(tmpSign[i].param, name) == 0) {
		index = i;
	    }
	}
    }
    return index;
}

/*
 * 初始化机密结构体，每次需要加密之前都需要调用此函数进行初始化
 * */
void initParamval(void)
{
    int entrySize = 0;
    int i=0;
    entrySize = sizeof(tmpSign)/sizeof(struct sign_t);
    for(i=0;i<entrySize;i++) 
    {
	memset(tmpSign[i].value, 0, SIGN_PARAM_VALUE_LEN);
	tmpSign[i].mark = 0;
    }
    return;
}

/*
 * 添加需要进行验证的参数值
 */
int addParamValue(char *param, char *value)
{
    int findindex = -1;
    
    if((param != NULL) && (value != NULL)) {
	findindex = findParamExist(param);
	if(findindex != -1) {
	    strncpy(tmpSign[findindex].value,value,SIGN_PARAM_VALUE_LEN-1);
	    tmpSign[findindex].mark = 1;
	    //debug(LOG_DEBUG, "%s: add param %s, value %s, mark%d, index %d",__func__,tmpSign[findindex].param,tmpSign[findindex].value,tmpSign[findindex].mark,findindex);
	    return 1;
	}
	debug(LOG_DEBUG, "%s: not find param \"%s\"",__func__,param);
    }
    debug(LOG_DEBUG, "%s: param or value is null.\n",__func__);

    return 0;
}

int delParamValue(char *param)
{
    int findindex = -1;
    
    if((param != NULL)) {
	findindex = findParamExist(param);
	if(findindex != -1) {
	    memset(tmpSign[findindex].value, 0, SIGN_PARAM_VALUE_LEN);
	    tmpSign[findindex].mark = 0;
	    //debug(LOG_DEBUG, "%s: add param %s, mark%d, index %d",__func__,tmpSign[findindex].param,tmpSign[findindex].mark,findindex);
	    return 1;
	}
	debug(LOG_DEBUG, "%s: not find param \"%s\"",__func__,param);
    }
    debug(LOG_DEBUG, "%s: param or value is null.\n",__func__);

    return 0;
}


#if 0
int upperCaseVal(void)
{
    int i = 0;
    for(i=0;i<40;i++)
    {
	if((hexData[i] >= 'a') && hexData[i] <= 'f')
	    hexData[i] = hexData[i]-32;
    }

    return 1;
}

int byte2hex(void)
{
    int len = 0;
    int i = 0;
    memset(hexData, 0, sizeof(hexData));

#if 0
    len = strlen(sha1Data);
    debug(LOG_DEBUG, "%s: sha1 data len =%d",__func__,len);
    if(len == 20) {
#endif
	for(i=0;i<20;i++) {
	sprintf((&(hexData[2*i])), "%02x", (unsigned char)sha1Data[i]);
	}
	debug(LOG_DEBUG, "%s: hex Data =%s",__func__,hexData);
	upperCaseVal();
	return 1;
#if 0
    }
    fprintf(stderr, "Error: [SHA1] byte to hex string error.\n");
    return 0;
#endif
}

int stringConcatenate(void)
{
    int i = 0;
    int entrySize = 0;
    int shaLen = 0;

    memset(sha1InData, 0, sizeof(sha1InData));
    entrySize = sizeof(tmpSign)/sizeof(struct sign_t);
    snprintf(sha1InData,1024,"%s",SECRET_KEY_VALUE);
    debug(LOG_DEBUG, "%s: entrySize =%d",__func__,entrySize);
    for(i=0;i<entrySize;i++) 
    {
	shaLen = strlen(sha1InData);
	if(tmpSign[i].mark == 1)
	{
	    if((strcmp(tmpSign[i].param, "1111")==0) || (strcmp(tmpSign[i].param, "2222")==0)) {
	    snprintf(sha1InData+shaLen, (1024-shaLen), "%s",tmpSign[i].value);
	    } else {
	    snprintf(sha1InData+shaLen, (1024-shaLen), "%s%s",tmpSign[i].param,tmpSign[i].value);
	    }
	}
    }
    shaLen = strlen(sha1InData);
    snprintf(sha1InData+shaLen, (1024-shaLen), "%s",SECRET_KEY_VALUE);
    debug(LOG_DEBUG, "%s: sha1InData =%s",__func__,sha1InData);

    return 1;
}

int getSha1Result(void)
{
    SHA1_CTX s;

    memset(sha1Data, 0, sizeof(sha1Data));
    stringConcatenate();
    if(strlen(sha1InData) > 0) {
    debug(LOG_DEBUG, "%s: sha1InData =%s",__func__,sha1InData);
    SHA1_Init(&s);
    SHA1_Update(&s, sha1InData, strlen(sha1InData));
    SHA1_Final(sha1Data, &s);
    //debug(LOG_DEBUG, "%s: sha1 data =%s",__func__,sha1Data);
    byte2hex();
    debug(LOG_DEBUG, "%s: hex and uppercase sha1 data =%s",__func__,hexData);
    return 1;
    }

    fprintf(stderr, "Error: [SHA1] get sha1 result data error.\n");
    return 0;
}

#else
int byte2hex(char *inData, char *outData)
{
    int len = 0;
    int i = 0;
    //memset(outData, 0, sizeof(outData));
    if(inData!=NULL && outData!=NULL) {
#if 0
    len = strlen(inData);
    debug(LOG_DEBUG, "%s: sha1 data len =%d",__func__,len);
    if(len == 20) {
#endif
	for(i=0;i<20;i++) {
	sprintf((&(outData[2*i])), "%02x", (unsigned char)inData[i]);
	}
	//debug(LOG_DEBUG, "%s: hex Data =%s",__func__,outData);
	//upperCaseVal();
	for(i=0;i<40;i++)
	{
	    if((outData[i] >= 'a') && outData[i] <= 'f')
		outData[i] = outData[i]-32;
	}
	return 1;
#if 0
    }
    fprintf(stderr, "Error: [SHA1] byte to hex string error.\n");
    return 0;
#endif
    }
    return 0;
}

/*
 * 将有函数addParamValue函数添加的参数值组成一个长字符串作为加密的明文
 * */
int stringConcatenate(char *sha1InData)
{
    int i = 0;
    int entrySize = 0;
    int shaLen = 0;
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    s_config *config = config_get_config();
#endif

    entrySize = sizeof(tmpSign)/sizeof(struct sign_t);
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    snprintf(sha1InData,1024,"%s",config->secret_key);
#else
    snprintf(sha1InData,1024,"%s",SECRET_KEY_VALUE);
#endif
    //debug(LOG_DEBUG, "%s: entrySize =%d",__func__,entrySize);
    for(i=0;i<entrySize;i++) 
    {
	shaLen = strlen(sha1InData);
	if(tmpSign[i].mark == 1)
	{
	    if((strcmp(tmpSign[i].param, "1111")==0) || (strcmp(tmpSign[i].param, "2222")==0)) {
	    snprintf(sha1InData+shaLen, (1024-shaLen), "%s",tmpSign[i].value);
	    } else {
	    snprintf(sha1InData+shaLen, (1024-shaLen), "%s%s",tmpSign[i].param,tmpSign[i].value);
	    }
	}
    }
    shaLen = strlen(sha1InData);
#if (WIFIDOG_USER_SECRET_KEY == FYES)
    snprintf(sha1InData+shaLen, (1024-shaLen), "%s",config->secret_key);
#else
    snprintf(sha1InData+shaLen, (1024-shaLen), "%s",SECRET_KEY_VALUE);
#endif
    //debug(LOG_DEBUG, "%s: sha1InData =%s",__func__,sha1InData);

    return 1;
}

/*
 * 获取最后sha1后的结果，取前40个字符
 * */
int getSha1Result(unsigned char *outData)
{
    unsigned char InData[1024];
    unsigned char Data[21];
    SHA1_CTX s;

    if(outData!=NULL) {
    memset(InData, 0, sizeof(InData));
    memset(Data, 0, sizeof(Data));
    stringConcatenate(InData);
    if(strlen(InData) > 0) {
    //debug(LOG_DEBUG, "%s: InData =%s",__func__,InData);
    SHA1_Init(&s);
    SHA1_Update(&s, InData, strlen(InData));
    SHA1_Final(Data, &s);
    //debug(LOG_DEBUG, "%s: sha1 data =%s",__func__,Data);
    byte2hex(Data, outData);
    debug(LOG_DEBUG, "%s: hex and uppercase sha1 data =%s",__func__,outData);
    return 1;
    }
    
    }
    fprintf(stderr, "Error: [SHA1] get sha1 result data error.\n");
    return 0;
}
#endif
#endif
