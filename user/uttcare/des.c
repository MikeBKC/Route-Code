#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include "uttMachine.h"
#include "des.h"

#define INPUT_LEN 500
#define ENCRYPT_LEN 2000
#define KEY_LEN 8

#define DEBUG 0

char * DES_Encrypt(char *plain, char *keyStr,int *cipherlen);
int DES_Decrypt(char *cipherFile, char *keyStr,char *plainFile,long cipherLen);
#if 0
static void getSoftVersion(char *softVer);
#endif

#if 0
int main(int argc, char **argv)
{
    char *cipherStr = NULL;
    int cipherLen = 0;
    char plainStr[200]={0};
    char str[9];
    int num = 0, till = 0;
    if(argc==2)
    {
	num = atoi(argv[1]);

	if((cipherStr = DES_Encrypt("18726035908", "12345678", &cipherLen)) == NULL) {
	    printf("des decrypt error.\n");
	    return -1;
	}
	if(cipherStr != NULL) {
	    if(DES_Decrypt(cipherStr,"12345678",plainStr) == -1) {
		printf("des decrypt error.\n");
		return -1;
	    }
	    printf("%s-%d: plainStr =%s\n",__func__,__LINE__,plainStr);
	    free(cipherStr);
	}
	printf("%s-%d: plainStr =%s\n",__func__,__LINE__,plainStr);
	if(plainStr[0]!='\0') {
	    printf("%s-%d: plainStr =%s\n",__func__,__LINE__,plainStr);
	}
    }
    return 0;
}
#else
void usagePrintf()
{
    printf("Usage:desTool -i [inputstr] -k [keystr] -t [0/1]\n"
	    "	    -i:the input str\n"
	    "	    -k:key str\n"
	    "	    -t:type enStr or deStr\n");
}

#if 0
int main(int argc, char **argv)
{
    char ch;
    char inputStr[INPUT_LEN+1];
    char key[KEY_LEN+1];
    int type;
    if(argc != 7){
	printf("error\n");
	usagePrintf();
	return 0;
    }
    while ((ch = getopt(argc, (char * const *)argv, "i:k:t:")) != -1) 
    {   
        switch(ch){
            case 'i':
		strncpy(inputStr,optarg,INPUT_LEN);
                break;    
            case 'k':
		strncpy(key,optarg,KEY_LEN);
		break;
	    case 't':
		type=atoi(optarg);
		break;

	    case '?':
                usagePrintf();
                exit(0);
                break;
        }   
    }   
    if(type == 0){
	encrypt(inputStr,key);    
    }else{
	decrypt(inputStr,key);
    }
}
#endif
#endif
static void KeyInitialize(int *key, int keyarray[16][48]);
static int Encrypt(int *timeData, int flag, int keyarray[16][48], char *result);
static void LeftBitMove(int *k, int offset);
static void LoopF(int *M, int times, int flag, int keyarray[16][48]);
static void GetEncryptResultOfByteArray(int *data, char *value);
static void ReadDataToBirnaryIntArray(char *intdata,int *retdata);
char key[8+1];    
int KeyArray[16][48];        

static int UnitDes(char *des_key, char *des_data, int flag, char *result) {
    // 检测输入参数格式是否正确，错误直接返回空值（null） 
    if (((flag != 1) && (flag != 0))) { 
	printf("flag %d errot.\n",flag);
	return -1;
    } 

    int flags = flag;
    // 二进制加密密钥   
    int keydata[64];
    // 二进制加密数据    
    int encryptdata[64]; 
    // 加密操作完成后的字节数组   
    // 密钥初试化成二维数组   
    int keyArray[16][48];
    memset(keydata, 0U, sizeof(keydata));
    memset(encryptdata, 0U, sizeof(encryptdata));
    memset(keyArray, 0U, sizeof(keyArray));
    // 将密钥字节数组转换成二进制字节数组
#if 1
    ReadDataToBirnaryIntArray(des_key,keydata);
#else
    //memcpy(keydata, ReadDataToBirnaryIntArray(des_key), 64);
#endif
#if DEBUG
    printf("%s-%d: des_key =%s\n",__func__,__LINE__,des_key);
    int i=0;
    printf("keydata =");
    for(i=0;i<64;i++)
	printf(" %d ",keydata[i]);
    printf("\n");
#endif
    // 将加密数据字节数组转换成二进制字节数组  
#if 1
    ReadDataToBirnaryIntArray(des_data,encryptdata);
#else
    //memcpy(encryptdata, ReadDataToBirnaryIntArray(des_data), 64);
#endif
#if DEBUG
    printf("encryptdata =");
    for(i=0;i<64;i++)
	printf(" %d ",encryptdata[i]);
    printf("\n");
#endif
    // 初试化密钥为二维密钥数组   
    KeyInitialize(keydata, keyArray); 
    //printf("%s-%d: keydata %d %d %d %d\n",__func__,__LINE__,keydata[0],keydata[1],keydata[2],keydata[3]);
    //printf("%s-%d: KeyArray %d %d %d %d\n",__func__,__LINE__,KeyArray[0][0],KeyArray[0][1],KeyArray[0][2],KeyArray[0][3]);
    // 执行加密解密操作 
    Encrypt(encryptdata, flags, keyArray,result);
    return 0;

}// 初试化密钥数组  

static void KeyInitialize(int *key, int keyarray[16][48]) {
    int i;
    int j;
    int K0[56];// 特别注意：xxx[IP[i]-1]等类似变换   
    memset(K0, 0U, sizeof(K0));
    for (i = 0; i < 56; i++) {     
	K0[i] = key[PC_1[i] - 1]; // 密钥进行PC-1变换 
    } 
    for (i = 0; i < 16; i++) { 
	LeftBitMove(K0, LeftMove[i]); // 特别注意：xxx[IP[i]-1]等类似变换    
	for (j = 0; j < 48; j++) {
	    KeyArray[i][j] = K0[PC_2[j] - 1]; // 生成子密钥keyarray[i][j]  
	}  
    }   
} // 执行加密解密操作


static int Encrypt(int *timeData, int flag, int keyarray[16][48], char *result) {
    int i;
    char encrypt[8];
    int flags = flag;
    int M[64];
    int MIP_1[64]; // 特别注意：xxx[IP[i]-1]等类似变换  
    memset(encrypt, 0U, 8);
    memset(M, 0U, 64);
    memset(MIP_1, 0U, 64);

    for (i = 0; i < 64; i++) {   
	M[i] = timeData[IP[i] - 1]; // 明文IP变换   
    }   
    if (flags == 1) { // 加密
	for (i = 0; i < 16; i++) {     
	    LoopF(M, i, flags, KeyArray); 
	}    
    } else if (flags == 0) { // 解密    
	for (i = 15; i > -1; i--) {
	LoopF(M, i, flags, KeyArray);  
	}  
    }  
    for (i = 0; i < 64; i++) { 
	MIP_1[i] = M[IP_1[i] - 1]; // 进行IP-1运算
    }   
    GetEncryptResultOfByteArray(MIP_1, result);// 返回加密数据   
    return 0; 
}

static void ReadDataToBirnaryIntArray(char *intdata, int *retdata) { 
    int i; 
    int j;    // 将数据转换为二进制数，存储到数组  
    int IntDa[8];  
    memset(IntDa, 0U, 8);
    for (i = 0; i < 8; i++) {   
	IntDa[i] = intdata[i];   
	if (IntDa[i] < 0) {     
	    IntDa[i] += 256;  
	    IntDa[i] %= 256;
	}

    }

    int IntVa[64]; 
    memset(IntVa, 0U, 64);
    for (i = 0; i < 8; i++) {   
	for (j = 0; j < 8; j++) {    
	    IntVa[((i * 8) + 7) - j] = IntDa[i] % 2;   
	    IntDa[i] = IntDa[i] / 2;   
	}   
    }
#if DEBUG
    printf("IntDa =");
#endif
    for(i=0;i<64;i++) {
	retdata[i] = IntVa[i];
#if DEBUG
	printf(" %d ",IntVa[i]);
#endif
    }
#if DEBUG
    printf("\n");
#endif
    return ;
}
static void LeftBitMove(int *k, int offset) { 
    int i;    // 循环移位操作函数  
    int c0[28];
    int d0[28];
    int c1[28];
    int d1[28]; 
    memset(c0, 0U, sizeof(c0));
    memset(d0, 0U, sizeof(d0));
    memset(c1, 0U, sizeof(c1));
    memset(d1, 0U, sizeof(d1));
    for (i = 0; i < 28; i++) {
	c0[i] = k[i];
	d0[i] = k[i + 28];
    }
    if (offset == 1) {
	for (i = 0; i < 27; i++) { // 循环左移一位 
	    c1[i] = c0[i + 1];
	    d1[i] = d0[i + 1];
	} 
	c1[27] = c0[0];
	d1[27] = d0[0]; 
    } else if (offset == 2) {
	for (i = 0; i < 26; i++) { // 循环左移两位 
	    c1[i] = c0[i + 2];
	    d1[i] = d0[i + 2]; 
	} 
	c1[26] = c0[0];
	d1[26] = d0[0];
	c1[27] = c0[1];
	d1[27] = d0[1]; 
    } 
    for (i = 0; i < 28; i++) { 
	k[i] = c1[i]; 
	k[i + 28] = d1[i]; 
    } 
} 
static void LoopF(int *M, int times, int flag, int keyarray[16][48]) { 
    int i; 
    int j; 
    int L0[32];
    int R0[32];
    int L1[32];
    int R1[32];
    int RE[48];
    int S[8][6];
    int sBoxData[8];
    int sValue[32];
    int RP[32]; 
    memset(L0, 0U, sizeof(L0));
    memset(R0, 0U, sizeof(R0));
    memset(L1, 0U, sizeof(L1));
    memset(R1, 0U, sizeof(R1));
    memset(RE, 0U, sizeof(RE));
    memset(S, 0U, sizeof(S));
    memset(sBoxData, 0U, sizeof(sBoxData));
    memset(sValue, 0U, sizeof(sValue));
    memset(RP, 0U, sizeof(RP));
    for (i = 0; i < 32; i++) {
	L0[i] = M[i]; // 明文左侧的初始化 
	R0[i] = M[i + 32]; // 明文右侧的初始化 
    } 
    for (i = 0; i < 48; i++) { 
	RE[i] = R0[E[i] - 1]; // 经过E变换扩充，由32位变为48位 
	RE[i] = RE[i] + keyarray[times][i]; // 与KeyArray[times][i]按位作不进位加法运算 
	if (RE[i] == 2) { 
	    RE[i] = 0; 
	} 
    } 
    for (i = 0; i < 8; i++) { // 48位分成8组 
	for (j = 0; j < 6; j++) {
	    S[i][j] = RE[(i * 6) + j];
	}
	// 下面经过S盒，得到8个数   
	sBoxData[i] = S_Box[i][(S[i][0] << 1) + S[i][5]][(S[i][1] << 3) + (S[i][2] << 2) + (S[i][3] << 1) + S[i][4]];    // 8个数变换输出二进制
	for (j = 0; j < 4; j++) { 
	    sValue[((i * 4) + 3) - j] = sBoxData[i] % 2; 
	    sBoxData[i] = sBoxData[i] / 2; 
	}  
    }

    for (i = 0; i < 32; i++) { 
	RP[i] = sValue[P[i] - 1]; // 经过P变换
	L1[i] = R0[i]; // 右边移到左边  
	R1[i] = L0[i] + RP[i]; 
	if (R1[i] == 2) {
	    R1[i] = 0; 
	} 
	// 重新合成M，返回数组M

	// 最后一次变换时，左右不进行互换。此处采用两次变换实现不变 
	if (((flag == 0) && (times == 0)) || ((flag == 1) && (times == 15))) { 
	    M[i] = R1[i]; 
	    M[i + 32] = L1[i]; 
	} else { 
	    M[i] = L1[i]; 
	    M[i + 32] = R1[i];
	} 
    }
}
static void GetEncryptResultOfByteArray(int *data, char *value) { 
    int i;  
    int j; // 将存储64位二进制数据的数组中的数据转换为八个整数（byte） 
    for (i = 0; i < 8; i++) {  
	for (j = 0; j < 8; j++) { 
	    value[i] += (data[(i << 3) + j] << (7 - j)); 
	}  
    }   
    for (i = 0; i < 8; i++) {   
	value[i] %= 256;
	if (value[i] > 128) {    
	    value[i] -= 255;   
	}  
    } 
}  
static char* ByteDataFormat(char *data, int datalen, int *newdatalen) {
    int i = 0;
    int padlen = 8 - (datalen % 8);
    int newlen = datalen + padlen;
    char *newdata;
    newdata = (char *)malloc(newlen+1);
    if(newdata == NULL) {
	printf("malloc newdata error.\n");
	return NULL;
    }
    memset(newdata, 0U,newlen+1);
    memcpy(newdata, data, datalen);
    for (i = datalen; i < newlen; i++)
	newdata[i] = (char) padlen;
    *newdatalen = newlen;
#if DEBUG
    printf("%s-%d: newdatalen =%d newdata = \n",__func__,__LINE__,*newdatalen);
    for(i=0;i<*newdatalen;i++) {
	printf(" %d ",newdata[i]);
	if((i+1)%8==0)
	    printf("\n");
    }
#endif
    return newdata;
}
static char* DesEncrypt(char *des_data, int des_datalen, int flag, int *retlen) {  
    int i = 0;
    int newlen = 0;   
    char *format_data=NULL;
    char tmpresult[8];
    format_data = ByteDataFormat(des_data, des_datalen,  &newlen);  
    if(format_data == NULL) {
	printf("%s-%d: format data error.\n",__func__,__LINE__);
	return NULL;
    }
#if DEBUG
    printf("%s-%d: newdatalen =%d format_data = \n",__func__,__LINE__,newlen);
    for(i=0;i<newlen;i++) {
	printf(" %d ",format_data[i]);
	if((i+1)%8==0)
	    printf("\n");
    }
    printf("--------------------\n");
#endif
    int unitcount = newlen / 8;  
    char *result_data = NULL;
    result_data = (char*)malloc(newlen+1);   
    if(result_data == NULL) {
	printf("malloc result_data error. \n");
	return NULL;
    }
    for (i = 0; i < unitcount; i++) {
	char tmpkey[9];
	char tmpdata[9];
	memset(tmpkey, 0U, sizeof(tmpkey)); 
	memset(tmpdata, 0U, sizeof(tmpdata)); 
	memset(tmpresult, 0U, sizeof(tmpresult));
	memcpy(tmpdata, format_data+i*8, 8);
	UnitDes(key, tmpdata, flag, tmpresult);
#if DEBUG
	printf("%s-%d: i =%d, tmpdata %d %d %d %d %d %d %d %d\n",__func__,__LINE__,i,tmpdata[0],tmpdata[1],tmpdata[2],tmpdata[3],tmpdata[4],tmpdata[5],tmpdata[6],tmpdata[7]);
	printf("%s-%d: i =%d, tmpresult %d %d %d %d %d %d %d %d\n",__func__,__LINE__,i,tmpresult[0],tmpresult[1],tmpresult[2],tmpresult[3],tmpresult[4],tmpresult[5],tmpresult[6],tmpresult[7]);
#endif
	memcpy(result_data+i*8, tmpresult, 8);
    }
    *retlen=newlen;
    // 当前为解密过程，去掉加密时产生的填充位
    //char *decryptbytearray = NULL;
#if 1
    if (flag == 0) {
	int k = 0;
	int total_len = newlen;
	int delete_len = result_data[total_len - 8 - 1];
	delete_len = ((delete_len >= 1) && (delete_len <= 8)) ? delete_len : 0;
	//decryptbytearray = new byte[total_len - delete_len - 8];
	int del_flag = 1;
	for (k = 0; k < delete_len; k++) {

	    if (delete_len != result_data[total_len - 8 - (k + 1)])
		del_flag = 0;
	}
	if (del_flag == 1) {
	//System.arraycopy(result_data, 0, decryptbytearray, 0, total_len- delete_len - 8);
	    *retlen = total_len-delete_len-8;
	    memset(result_data+(*retlen), 0U,delete_len+8);
	}
    }
#endif
    //return (flag == 1) ? result_data : decryptbytearray;
    free(format_data);
    return result_data;
}

#if 0
       public static String parseByte2HexStr(byte buf[]) {
           StringBuffer sb = new StringBuffer();
           for (int i = 0; i < buf.length; i++) {
               String hex = Integer.toHexString(buf[i] & 0xFF);
               if (hex.length() == 1) {
                   hex = '0' + hex;
               }
               sb.append(hex.toUpperCase());
           }
           return sb.toString();
       }               
#endif
static char *getEncString(char *data, int datalen, int *retlen){
    char *result = NULL;
    result = DesEncrypt(data, datalen, 1, retlen);
    if(result == NULL) {
	printf("%s-%d: des encrypt error.\n",__func__,__LINE__);
	return NULL;
    }
#if 0
    int i = 0;
    printf("*retlen =%d, encrypt result = \n", *retlen);
    for(i=0;i<*retlen;i++){
	printf(" %.2x ",result[i]&0xFF);
	if((i+1)%8==0) {
	    printf("\n");
	}
    }
#endif
    //free(result);
    return result;
#if 0
    System.out.println("result length:"+result.length);
    BASE64Encoder base64en = new BASE64Encoder();
    System.out.println("base64en.encode(result) length:"+base64en.encode(result).length());
    return base64en.encode(result);
#endif
}       

static char* getDesString(char *encStr, int encStrlen, int *retlen) {
#if 0
BASE64Decoder base64De = new BASE64Decoder();
byte[] byteMi = base64De.decodeBuffer(encStr);
#endif                          
    char *result = NULL;
    result = DesEncrypt(encStr, encStrlen, 0, retlen);
    if(result == NULL) {
	printf("%s-%d: des decrypt error.\n",__func__,__LINE__);
	return NULL;
    }
#if DEBUG
    printf("%s-%d: *retlen =%d, result =%s\n",__func__,__LINE__,*retlen,result);
#endif
    return result;
#if 0
String string= new String(desbyte);
return string;
#endif
}

//const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const char base[] = "ANBOCPDQERFSGTHUIVJWKXLYMZagmsbhntcioudjpvekqwflrxyz0963852741+/";
/* */ 
char *base64_encode(const char* data, int data_len, int *len) 
{     
    //int data_len = strlen(data);     
    int prepare = 0;
    int ret_len;
    int temp = 0;
    char *ret = NULL;
    char *f = NULL;
    int tmp = 0;
    char changed[4];
    int i = 0; 
    int count = 0;
    ret_len = data_len / 3; 
    temp = data_len % 3;     
    if (temp > 0)     {         
	ret_len += 1;     
    }     
    ret_len = ret_len*4 + 1;     
    ret = (char *)malloc(ret_len); 
    if ( ret == NULL)     { 
	printf("No enough memory.\n"); 
	return 0; 
    }     
    memset(ret, 0, ret_len);     
    f = ret;     
    while (tmp < data_len)     {         
	temp = 0;         
	prepare = 0;         
	memset(changed, '\0', 4);         
	while (temp < 3)         {             
	    //printf("tmp = %d\n", tmp);             
	    if (tmp >= data_len)             
	    {                 
		break;             
	    }             
	    prepare = ((prepare << 8) | (data[tmp] & 0xFF));             
	    tmp++;             
	    temp++;         
	}         
	prepare = (prepare<<((3-temp)*8));         
	//printf("before for : temp = %d, prepare = %d\n", temp, prepare);         
	for (i = 0; i < 4 ;i++ ) {             
	    if (temp < i) { 
		changed[i] = 0x40;             
	    } else {                 
		changed[i] = (prepare>>((3-i)*6)) & 0x3F;             
	    } 
	    *f = base[changed[i]];             
	    //printf("%.2X", changed[i]);             
	    f++;  
	    count++;
	}     
    }     
    *f = '\0';
#if DEBUG
    printf("%s-%d: count =%d\n",__func__,__LINE__,count);
#endif
    *len = ret_len-1;
    return ret;       
} 

/* */
static char find_pos(char ch)
{
char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[]     
return (ptr - base);
}
/* */
char *base64_decode(const char *data, int data_len, int *len)
{
   int ret_len = (data_len / 4) * 3;
   int equal_count = 0;
   char *ret = NULL;
   char *f = NULL;
   int tmp = 0;
   int temp = 0;
   char need[3];
   int prepare = 0;
   int i = 0;
   int count = 0;
   if (*(data + data_len - 1) == '=') {
       equal_count += 1;
   }
   if (*(data + data_len - 2) == '=') {
       equal_count += 1;
   }
   if (*(data + data_len - 3) == '=') {//seems impossible         
       equal_count += 1;
   }
   switch (equal_count) {
       case 0:
           ret_len += 4;//3 + 1 [1 for NULL]         
           break;
       case 1:
           ret_len += 4;//Ceil((6*3)/8)+1         
           break;
       case 2:
           ret_len += 3;//Ceil((6*2)/8)+1         
           break;
       case 3:
           ret_len += 2;//Ceil((6*1)/8)+1         
           break;
   }
#if DEBUG
   printf("%s-%d: ret_len =%d,data_len =%d,equal_count =%d\n",__func__,__LINE__,ret_len,data_len,equal_count);
#endif
   ret = (char *)malloc(ret_len);
   if (ret == NULL)     {
       printf("No enough memory.\n");
       return 0;;
   }
   memset(ret, 0, ret_len);
   f = ret;
   while (tmp < (data_len - equal_count)) {
       temp = 0;
       prepare = 0;
       memset(need, 0, 4);
       while (temp < 4) {
           if (tmp >= (data_len - equal_count)) {
               break;
           }
           prepare = (prepare << 6) | (find_pos(data[tmp]));
           temp++;
           tmp++;
       }
       prepare = prepare << ((4-temp) * 6);
       for(i=0; i<3 ;i++ ) {
           if (i == temp) {
               break;
           }
           *f = (char)((prepare>>((2-i)*8)) & 0xFF);
           f++;
           count++;
       }
   }
   *f = '\0';
   *len = count-equal_count;
#if DEBUG
   printf("%s-%d: count =%d,*(f-1)=%d\n",__func__,__LINE__,count,*(f-1));
#endif
   return ret;
}
//加密文件   
char* DES_Encrypt(char *plain, char *keyStr,int *cipherlen)
{
    int plainLen = 0;
#if DEBUG
    printf("Encrypt data: \n");
    printf("plain str: %s, len: %d\n",plain,strlen(plain));
    printf("key str: %s\n",keyStr);
#endif
#if 1
    //设置密钥   
    char *formatkey=NULL;
    int newkeylen = 0;
    memset(KeyArray, 0U, sizeof(KeyArray));
    memset(key, 0, sizeof(key));
    if(strlen(keyStr)>0) {
	formatkey = ByteDataFormat(keyStr, strlen(keyStr), &newkeylen);
    } else {
	printf("Key string error.\n");
	return NULL;
    }
    if(formatkey != NULL) {
	memcpy(key, formatkey, 8);  
	free(formatkey);
    } else {
	printf("%s-%d: format key error.\n",__func__,__LINE__);
	return NULL;
    }
#if DEBUG
    printf("%s-%d: key %d %d %d %d %d %d %d %d\n",__func__,__LINE__,key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7]);
#endif
#endif

#if 1
    char *dst1 = NULL;
#endif
#if 1
    char *mistr = NULL;
    int encryptlen=0;
    int baseLen = 0;
    plainLen = strlen(plain);
    mistr = getEncString(plain,plainLen,cipherlen);
#if 0
    printf("%s-%d: cipherlen =%d, mistr =%s\n",__func__,__LINE__,*cipherlen,mistr);
#endif
#endif
#if 0
    if(mistr != NULL) {
	dst1=base64_encode(mistr, encryptlen, &baseLen); /* the second parameter must accord with the first one */
#if DEBUG
	printf("%s-%d: strlen(dst1) =%d, baseLen =%d, dst1 =%s\n",__func__,__LINE__,strlen(dst1),baseLen,dst1);
#endif
    }
#if DEBUG
    printf("End encrypt.\n");
#endif
    if(dst1 != NULL) {
	*cipherlen = strlen(dst1);
	return dst1;
    }
    //free(dst1);
#endif
    return mistr;
}

//解密文件
int DES_Decrypt(char *cipher, char *keyStr,char *plain,long cipherLen)
{
//    long cipherLen = 0;
#if DEBUG
    printf("Decrypt data: \n");
    printf("cipher str: %s, len: %d",cipher,strlen(cipher));
    printf("key str: %s \n",keyStr);
#endif
    //设置密钥   
    char *formatkey=NULL;
    int newkeylen = 0;
    memset(KeyArray, 0U, sizeof(KeyArray));
    memset(key, 0, sizeof(key));
    if(strlen(keyStr)>0) {
	formatkey = ByteDataFormat(keyStr, strlen(keyStr), &newkeylen);
    } else {
	printf("Key string error.\n");
	return -1;
    }
    if(formatkey != NULL) {
	memcpy(key, formatkey, 8);  
	free(formatkey);
    } else {
	printf("%s-%d: format key error.\n",__func__,__LINE__);
	return -1;
    }
#if DEBUG
    printf("%s-%d: key %d %d %d %d %d %d %d %d\n",__func__,__LINE__,key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7]);
#endif

    //取文件长度    
//    cipherLen = strlen(cipher);    //取文件指针当前位置   
#if DEBUG
    printf("cipherLen =%d\n",cipherLen);
#endif
    if(cipherLen%4 != 0) {
	printf("%s-%d: cipher len error.\n",__func__,__LINE__);
	return -1;
    }
    int len=0;
    char *dst2=NULL;
//    dst2=base64_decode(cipher,cipherLen, &len);
#if 0
    while(dst2[len-1] == '=') {
	len -= 1;
    }
#endif
#if DEBUG
    printf("%s-%d:base64 decode %d, %d,len =%d\n",__func__,__LINE__,strlen(dst2),sizeof(dst2),len);
#endif
    char *mingstr = NULL;
    int decryptlen=0;
    mingstr = getDesString(cipher,cipherLen,&decryptlen);
    if(mingstr != NULL) {
	sprintf(plain,"%s",mingstr);
	return decryptlen;
    }
    free(dst2);
#if DEBUG
    printf("End decrypt.\n");
#endif
    return -1;
}

#if defined(CONFIG_BOARD_P1010_C)
void desEncrypt(char *inputStr,char *outputStr,char *key)
{
    int i,cipherLen=0;
    char *cipherStr=NULL;
    if((cipherStr = DES_Encrypt(inputStr, key, &cipherLen)) != NULL) {
	printf("len=%d\n",cipherLen);
	while(i < cipherLen ){
	    sprintf(outputStr+i*2,"%02X",cipherStr[i]&0xFF);
	    printf("output=%02X\n",cipherStr[i]&0xFF);
	    i++;
	}
    }
}
void desDecrypt(char *inputStr,char *deStr,char *key)
{
    int i,hex;
    int iLen=strlen(inputStr);
    char hexStr[3]={0},outputStr[1000]={0};
    for(i=0;i<iLen/2;i++){
	sscanf(inputStr+2*i,"%2s",hexStr);
	hex=strtol(hexStr,NULL,16);
	outputStr[i]=hex;
    }
    DES_Decrypt(outputStr,key,deStr,iLen/2);

}
#else
char *utHex2str(unsigned char *in, int iLen, unsigned char *out)
{
    int i;

    for (i=0; i<iLen; i++)
    {
    sprintf(out+i*2, "%02X", in[i]);
    }

    return out;
}
char *utstr2Hex(unsigned char *in, int iLen, unsigned char *out)
{
    int n=0,i=0;
    char tStr[3];            //临时字串     
    char hexStr[3];                 //转变后字符 
   for(i=0;i<iLen/2;i++){
	sscanf(in+2*i,"%2s",hexStr);
	sscanf(hexStr,"%x",&out[i]);
    }
    return out;
}
void desEncrypt(char *inputStr,char *outputStr,char *key)
{
//    unsigned char outputStr[ENCRYPT_LEN+1];
    char *cipherStr = NULL;
//    int iLen;
    int cipherLen=0;
//    iLen = utDesEncrypt(inputStr, strlen(inputStr), output, key);
    if((cipherStr = DES_Encrypt(inputStr, key, &cipherLen))==NULL)
    {
	printf("des error\n");
	exit(-1);
    }
    utHex2str(cipherStr, cipherLen, outputStr);
    return;
}
void desDecrypt(char *inputStr,char *deStr,char *key)
{
    unsigned char input[ENCRYPT_LEN+1];
//    unsigned char deStr[INPUT_LEN+1];
    long len=strlen(inputStr);
    utstr2Hex(inputStr,len,input);
//    utDesDecrypt(input,strlen(input),deStr,key);
     if(DES_Decrypt(input,key,deStr,len/2) == -1) {
	 printf("des decrypt error.\n");
	 exit(-1);
     }
    return deStr;
}
#endif
