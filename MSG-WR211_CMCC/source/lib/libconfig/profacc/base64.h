#ifndef BASE_64_H
#define BASE_64_H
/*Base64 ����*/
//typedef enum {false = 0, true = 1} bool;
extern int Base64Encode(char *OrgString, char *Base64String, int OrgStringLen );
/*Base64 ����*/
extern char GetBase64Value(char ch); //�õ�����ֵ
/* ���뺯��*/
extern int Base64Decode(char *OrgString, char *Base64String, int Base64StringLen, bool bForceDecode );//���뺯��
#endif
