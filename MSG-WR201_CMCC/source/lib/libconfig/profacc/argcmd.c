/*
 * ˵����
 *  ���ļ�����һЩ��profile��������ú���
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
 * �����ַ������ö�ٽṹ�е���ֵ
 * ���룺
 *  enumPara	    -	    ö�ٽṹ����
 *  value	    -	    ��Ӧvalueֵ
 * ����ֵ��
 *  ����ҵ����ض�Ӧ�ַ��������򷵻�NULL
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
 * ����ö�ٽṹ�е���ֵ��ö�Ӧ�ַ���
 * ���룺
 *  enumPara	    -	    ö�ٽṹ����
 *  string	    -	    ��Ӧ�ַ���ֵ
 * ����ֵ��
 *  ����ҵ����ض�Ӧö��ֵ�����򷵻�PROFFAIL(-1)
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
