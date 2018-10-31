#ifndef BASE_64_H
#define BASE_64_H
/*Base64 编码*/
//typedef enum {false = 0, true = 1} bool;
extern int Base64Encode(char *OrgString, char *Base64String, int OrgStringLen );
/*Base64 解码*/
extern char GetBase64Value(char ch); //得到编码值
/* 解码函数*/
extern int Base64Decode(char *OrgString, char *Base64String, int Base64StringLen, bool bForceDecode );//解码函数
#endif
