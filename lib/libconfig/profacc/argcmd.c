/*
 * 说明：
 *  本文件包含一些对profile参数处理得函数
 */
#include "uttMachine.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "string.h"

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "daughter.h"
#include "argcmd.h"

/*
 * 根据字符串获得枚举结构中的数值
 * 输入：
 *  enumPara	    -	    枚举结构数组
 *  value	    -	    对应value值
 * 返回值：
 *  如果找到返回对应字符串，否则返回NULL
 */
extern const char *getEnumString(const LcdParamEnumValue *enumPara, int value) {

    while((enumPara != NULL) && (enumPara->enumName != NULL)) {
	if(enumPara->enumValue == value) {
	    return enumPara->enumName;
	}
    	enumPara++;
    }

    return NULL;
}

/*
 * 根据枚举结构中的数值获得对应字符串
 * 输入：
 *  enumPara	    -	    枚举结构数组
 *  string	    -	    对应字符串值
 * 返回值：
 *  如果找到返回对应枚举值，否则返回PROFFAIL(-1)
 */
extern int getEnumValue(const LcdParamEnumValue *enumPara, char *string) {

    while((enumPara != NULL) && (enumPara->enumName != NULL)) {
	if(strcmp(enumPara->enumName, string) == 0) {
	    return enumPara->enumValue;
	}
    	enumPara++;
    }

    return PROFFAIL;
}
