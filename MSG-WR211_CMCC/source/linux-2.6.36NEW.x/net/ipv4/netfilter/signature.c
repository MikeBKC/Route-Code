//#include <stdio.h>
//#include <stdlib.h>
//#include <syslog.h>
#include <linux/string.h>
#include <linux/kernel.h>
//#include <pthread.h>
#include "signature.h"


//pthread_mutex_t param_sign_mutex = PTHREAD_MUTEX_INITIALIZER;
#define EMPTY_ENTRY {"", "", 0}
char * secret_key;
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

int findParamExist(char *name)
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
	wifi_debug_print("%s: not find param \"%s\"\n",__func__,param);
    }
    wifi_debug_print("%s: param or value is null.\n",__func__);

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
#if 0
    int len = 0;
#endif
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

int stringConcatenate(char *sha1InData)
{
    int i = 0;
    int entrySize = 0;
    int shaLen = 0;

    entrySize = sizeof(tmpSign)/sizeof(struct sign_t);
    snprintf(sha1InData,1024,"%s",secret_key);
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
    snprintf(sha1InData+shaLen, (1024-shaLen), "%s",secret_key);
    //debug(LOG_DEBUG, "%s: sha1InData =%s",__func__,sha1InData);

    return 1;
}

int getSha1Result(char *outData)
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
    wifi_debug_print("%s: hex and uppercase sha1 data =%s\n",__func__,outData);
    return 1;
    }
    
    }
    wifi_debug_print("Error: [SHA1] get sha1 result data error.\n");
    return 0;
}
#endif

char *_httpd_escape(const char *str)
{
    unsigned char mask = URL_XPALPHAS;
    const char * p;
    char * q;
    char * result;
    int unacceptable = 0;
    for(p=str; *p; p++)
        if (!ACCEPTABLE((unsigned char)*p))
                unacceptable +=2;
wifi_debug_print("%s-%d: ---------------\n",__func__,__LINE__);
    result = (char *)kmalloc((p-str + unacceptable + 1), GFP_ATOMIC);
wifi_debug_print("%s-%d: ---------------\n",__func__,__LINE__);
    memset(result,0,(p-str + unacceptable + 1));

    if (result == NULL)
    {
	return(NULL);
    }
    for(q=result, p=str; *p; p++) {
        unsigned char a = *p;
        if (!ACCEPTABLE(a)) {
            *q++ = '%';  /* Means hex commming */
            *q++ = hex_wifi[a >> 4];
            *q++ = hex_wifi[a & 15];
        }
        else *q++ = *p;
    }
    *q++ = 0;                   /* Terminate */
    return result;
}

char *httpdUrlEncode(const char *str)
{
        char    *new, *cp;

        new = (char *)_httpd_escape(str);
	if (new == NULL)
	{
		return(NULL);
	}
        cp = new;
        while(*cp)
        {
                if (*cp == ' ')
                        *cp = '+';
                cp++;
        }
	return(new);
}
char * set_secret_key(char * value)
{
    if(value != NULL && strlen(value) > 0)
    {   
        secret_key = value;
    }else{
        secret_key = SECRET_KEY_VALUE;
    }   
    wifi_debug_print("%s:  secret_key =  %s\n",__func__,secret_key);
}
