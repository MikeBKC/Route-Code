#ifndef _SIGNATURE_H
#define _SIGNATURE_H
/*
 * the signatureof wifidog's parameter to verify
 *
 * 此文件主要是实现参数的签名
 * 主要定义一些签名中用到的结构体定义和函数接口声明
 *
 * 如果开启参数签名功能，打开宏PARAM_SIGN(mibWebAuthShare.h)
 * guo.deyuan
 * */

#include "debug.h"
#include "sha1.h"
#include "conf.h"

#define SECRET_KEY_VALUE "1623`@wifi"
#if 0
unsigned char sha1InData[1024];
unsigned char sha1Data[21];
unsigned char hexData[41];
#endif

#define SIGN_PARAM_NAME_LEN 31
#define SIGN_PARAM_VALUE_LEN ((512>(MAX_MACLIST_LEN))?(512):(MAX_MACLIST_LEN))
/*
 * param    -  参数名
 * value    -  需要传输或获取的参数值
 * mark     -  此参数是否需要验证
 * */
typedef struct sign_t {
    char param[SIGN_PARAM_NAME_LEN];
    char value[SIGN_PARAM_VALUE_LEN];
    int mark;
} sign; 

extern void initParamval(void);
extern int addParamValue(char *param, char *value);
extern int delParamValue(char *param);
#if 0
extern int getSha1Result(void);
#else
extern int getSha1Result(unsigned char *outData);
#endif

#define LOCK_PARAM_SIGN() do { \
	debug(LOG_DEBUG, "Locking param sign"); \
	pthread_mutex_lock(&param_sign_mutex); \
	debug(LOG_DEBUG, "Param sign locked"); \
} while (0)

#define UNLOCK_PARAM_SIGN() do { \
	debug(LOG_DEBUG, "Unlocking param sign"); \
	pthread_mutex_unlock(&param_sign_mutex); \
	debug(LOG_DEBUG, "Param sign unlocked"); \
} while (0)

#endif /*_SIGNATURE_H*/
