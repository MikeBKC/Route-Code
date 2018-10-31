#ifndef __CONVER_H
#define __CONVER_H

#define INST	1
#define VALUE   2
#define CONFIGEND {NULL, NULL, 0, 0, 0, 0, 0, 0}

#define OFFANDSIZE(a, b) (Uint32)(&(((a*)0)->b)), sizeof(((a*)0)->b)
#define MAX_VALUE_LEN 0xFF

typedef enum 
{
    TNONE = 0,
    TINT,
    TIPADDR,
    TMACADDR,
    TMACADDR2,
    TSTR,
    TSTRNUM,
    TENUM,
    TGROUP,
    TDATETIME,
    TTIME,
    TENUMINT,	
    TPASSWD,
    THEX,
    TVARDATA,
    TBASESTR,
    TMEMBERS,
    TIPENTRY,
    TPORTENTRY,
    TUINT,
    TTIMESEC,
    TTIMEDATE,
    TITypeMAX
} IType;

struct st_xmlConfig {
    char            *name;
    struct st_xmlConfig *child;
    int             privilege;
    IType           itype;
    Uint32          offset;
    Uint32          size;
    Pointer         enumP;
    MibProfileType  mibtype;
};

typedef struct {        
    const char  *enumName;
    int     enumValue;
} LcdParamEnumValue;

extern void profGetEnumToStr2(Pointer prof, Uint32 offset, Uint32 size, char* buf,
        LcdParamEnumValue *enumPara);
extern void parseProfileValue2(Uint32 offset, Uint32 size, IType itype, char* buf, Pointer prof);
extern int converConfigFromXmlFile(const char *fileName);

#endif
