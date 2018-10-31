
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "netkeeper.h"


/**************************************************************************
 * 创建人：         王炳荣
 * 维护人：         王炳荣
 * 创建时间：       2008-11-27
 * 最后修改人：     王炳荣
 * 最后修改时间：   2008-11-27
 * 文件主要功能：   破解江西星空极速客户端拨号
 ***************************************************************************/

/* rdata4 is unk_428cb0 */
unsigned char rdata1[]={0x7,0x0c,0x11,0x16,0x5,0x9,0x0e,0x14,0x4,0x0b,0x10,0x17,0x6,0x0a,0x0f,0x15};
/* rdata2 is unk_428cc0 */
unsigned char rdata2[]= 
{0x78, 0xA4, 0x6A, 0xD7, 0x56, 0xB7, 0xC7, 0xE8, 0xDB, 0x70, 0x20, 0x24, 0xEE, 0xCE, 0xBD, 0xC1, 0xAF, 0x0F, 0x7C, 0xF5, 0x2A, 0xC6, 0x87, 0x47, 0x13, 0x46, 0x30, 0xA8, 0x1, 0x95, 0x46, 0xFD, 0xD8  , 0x98, 0x80, 0x69, 0xAF, 0xF7, 0x44, 0x8B, 0xB1, 0x5B, 0xFF, 0xFF, 0xBE, 0xD7, 0x5C, 0x89, 0x22, 0x11, 0x90, 0x6B, 0x93, 0x71, 0x98, 0xFD, 0x8E, 0x43, 0x79, 0xA6, 0x21, 0x8, 0xB4, 0x49, 0x62, 0x25, 0x1E, 0xF6, 0x40, 0xB3, 0x40, 0xC0, 0x51, 0x5A, 0x5E, 0x26, 0xAA, 0xC7, 0xB6, 0xE9, 0x5D, 0x10, 0x2F, 0xD6, 0x53, 0x14, 0x44, 0x2, 0x81, 0xE6, 0xA1, 0xD8, 0xC8, 0xFB, 0xD3, 0xE7, 0xE6, 0xCD, 0xE1, 0x21, 0xD6, 0x7, 0x37, 0xC3, 0x87, 0x0D, 0xD5, 0xF4, 0xED, 0x14, 0x5A, 0x45, 0x5, 0xE9, 0xE3, 0xA9, 0xF8, 0xA3, 0xEF, 0xFC, 0xD9, 0x2, 0x6F, 0x67, 0x8A, 0x4C, 0x2A, 0x8D, 0x42, 0x39, 0xFA, 0xFF, 0x81, 0xF6, 0x71, 0x87, 0x22, 0x61, 0x9D, 0x6D, 0x0C, 0x38, 0xE5, 0xFD, 0x44, 0xEA, 0xBE, 0xA4, 0xA9, 0xCF, 0xDE, 0x4B, 0x60, 0x4B, 0xBB, 0xF6, 0x70, 0xBC, 0xBF, 0xBE, 0xC6, 0x7E, 0x9B, 0x28, 0xFA, 0x27, 0xA1, 0xEA, 0x85, 0x30, 0xEF, 0xD4, 0x5, 0x1D, 0x88, 0x4, 0x39, 0xD0, 0xD4, 0xD9, 0xE5, 0x99, 0xDB, 0xE6, 0xF8, 0x7C, 0xA2, 0x1F, 0x65, 0x56, 0xAC, 0xC4, 0x44, 0x22, 0x29, 0xF4, 0x97, 0xFF, 0x2A, 0x43, 0xA7, 0x23, 0x94, 0xAB, 0x39, 0xA0, 0x93, 0xFC, 0xC3, 0x59, 0x5B, 0x65, 0x92, 0xCC, 0x0C, 0x8F, 0x7D, 0xF4, 0xEF, 0xFF, 0xD1, 0x5D, 0x84, 0x85, 0x4F, 0x7E, 0xA8, 0x6F, 0xE0, 0xE6, 0x2C, 0xFE, 0x14, 0x43, 0x1, 0xA3, 0xA1, 0x11, 0x8, 0x4E,0x82,0x7e,0x53,0xf7,0x35,0xf2,0x3a,0xbd,0xbb,0xd2,0xd7,0x2a,0x91,0xd3,0x86,0xeb};
unsigned char rdata3[]={0x72,0x61,0x64,0x69,0x75,0x73,0x80};
/* rdata4 is byte_428ca0 */
unsigned char rdata4[]={0x00,0x07,0x0e,0x05,0x0c,0x03,0x0a,0x01,0x08,0x0f,0x06,0x0d,0x04,0x0b,0x02,0x09};
/* rdata5 is byte_428c90 */
unsigned char rdata5[]={0x05,0x08,0x0b,0x0e,0x01,0x04,0x07,0x0a,0x0d,0x00,0x03,0x06,0x09,0x0c,0x0f,0x02};
/* rdata6 is byte_428c80 */
unsigned char rdata6[]={0x01,0x06,0x0b,0x00,0x05,0x0a,0x0f,0x04,0x09,0x0e,0x03,0x08,0x0d,0x02,0x07,0x0c};

static void  getRealNetKeeperName(unsigned char *a,unsigned char last2char,char *out,char* netKeeperName);

static unsigned char last2(unsigned char *a,char *name);

static char realNetKeeperName[10+MAX_UNIT_NAME_LEN + 1];
/***********************************************************************
 * 函 数 名：	jiangxi_netKeeper_main
 * 功能描述：   破解江西星空极速客户端拨号，将用户名进行转化
 * 输入参数:    电信给的pppoe用户名
 * 输出参数：	转换后的用户名
 * 返回值：     转换后的用户名
 * 创建日期：	2008-11-27 
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：   
***********************************************************************/
const char* jiangxi_netKeeper_main(char* netKeeperName)
{
  int tmp2=0x66666667;
  int tmp1 = 0;
  int i=0,numa=3,numb=0;
  unsigned char a[4],last2char;
  unsigned char b[4]={0};
  unsigned char    temp1,temp2,temp3;
  long long imul;
#if 0
  AbsoluteTime myCurrentTime;
  requestAbsoluteTime( &myCurrentTime );
  Pointer	prof;
  MibProfileType proftypeKey = MIB_PROF_SYSTEM;
  prof = _getNvramPointer(proftypeKey, 0);
  if(myCurrentTime < ((SystemProfile*)prof)->netKeeperTime){
      myCurrentTime = ((SystemProfile*)prof)->netKeeperTime + 1;
  }
    ((SystemProfile*)prof)->netKeeperTime += 5;
    if(sysFlashLocked == FALSE)
        writeSystemProfileData((SystemProfile*)prof);
#else
    time_t myCurrentTime;
    time(&myCurrentTime);
#endif

#if NETKEEPER_DEBUG
  for(i =0 ;i< 16;i++){
      printf("%x ",var58h[i]);
  }
  printf("\n");
  printf("netk time:      %x\n",myCurrentTime);
#endif
  myCurrentTime -= 0x7080;

  imul =(long long)myCurrentTime * (long long)tmp2;
#if NETKEEPER_DEBUG
  printf("log long  %x  %x \n",((int*)&imul)[0],((int*)&imul)[1]);
#endif
    tmp1 = imul/0xffffffff;
    tmp1 >>= 1; 
    tmp2 = tmp1;
    tmp2 >>= 31;
    tmp1 += tmp2;
  


    a[0] = (tmp1 >> 24) ;
    a[1] = tmp1 >> 16;
    a[2] = tmp1 >> 8;
    a[3] = tmp1;

/*
a[0] = 0x0e;
a[1] = 0x7f;
a[2] = 0x40;
a[3] = 0x0d;
*/
#if NETKEEPER_DEBUG
     printf("%02x %02x %02x %02x\n",a[0],a[1],a[2],a[3]);
     printf(" name %sbrwang\n",netKeeperName);
#endif
    last2char = last2(a,netKeeperName);    
    for(i = 0;i < 0x20;i++){
        if(i!=0){
            if(i%8 == 0)numa--;
       }
         
        temp3 = temp1 = a[numa];
        temp2 = b[numb];
        temp2 <<= 1;
        temp3 &= 1;
        temp2 |= temp3;
        temp1 >>= 1;
        a[numa] = temp1;
        b[numb++] = temp2;

        if(numb == 4)numb = 0;
    }

#if NETKEEPER_DEBUG
 printf("b:  %02x  %02x  %02x  %02x\n",b[0],b[1],b[2],b[3]);
#endif
    getRealNetKeeperName(b,last2char, realNetKeeperName,netKeeperName);
    
 
    return (const char*) realNetKeeperName;
}

/***********************************************************************
 * 函 数 名：	getRealNetKeeperName
 * 功能描述：   破解江西星空极速客户端拨号，将转换后的字符在转换合并成真正的用户名
 * 输入参数:    电信给的pppoe用户名,转换后的4字符，转换后的最后2字符，需要返回真正名字的地址
 * 输出参数：	转换后的用户名
 * 返回值：     无
 * 创建日期：	2008-11-27 
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：   
***********************************************************************/

static void  getRealNetKeeperName(unsigned char *a,unsigned char last2char,char *realName,char* netKeeperName)
{
    int i=0;
    unsigned char out[8];
    unsigned char temp1,temp2;
    temp1 = a[0];
    temp1 >>= 2;
    out[0] = temp1;

    temp1 = a[0];
    temp1 &= 3;
    temp1 <<= 4;
    out[1] = temp1;

    temp2 = a[1];
    temp2 >>= 4;
    temp2 |= temp1;
    out[1] |= temp2;

    temp1 = a[1];
    temp1 &= 0xf;
    temp1 <<= 2;
    out[2] = temp1;

    temp2 = a[2];
    temp2 >>= 6;
    temp2 |= temp1;
    out[2] = temp2;
     
    temp1 = a[2];
    temp1 &= 0x3f;
    out[3] = temp1;
    
    temp2 = a[3];
    temp2 >>= 2;
    out[4] = temp2;
    
    temp1 = a[3];
    temp1 &= 3;
    temp1 <<= 4;
    out[5] = temp1;

    for(i = 0;i < 6;i++){
       temp2 = out[i]; 
       temp2 += 0x20;
       out[i] = temp2;
       if(temp2 <0x40){
            continue;
       }else{
            temp2++;
            out[i] = temp2;
       }
    }
    sprintf((char*)(out+6),"%02x",last2char);
    realName[0] = 0x0d;
    realName[1] = 0x0a;
    memcpy(realName+2,out,8);
    memcpy(realName+10,netKeeperName,strlen(netKeeperName));

#if NETKEEPER_DEBUG
    for(i = 0;i < strlen(realName);i++)
        printf("%02x",realName[i]);
#endif        
}

/***********************************************************************
 * 函 数 名：	last2
 * 功能描述：   破解江西星空极速客户端拨号，将用户名进行转化,生成最后两个字符
 * 输入参数:    电信给的pppoe用户名,当前的时间
 * 输出参数：	生成最后的2个字符
 * 返回值：     生成最后的2个字符
 * 创建日期：	2008-11-27 
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：   
***********************************************************************/

static unsigned char last2(unsigned char *time,char *name)
{
    unsigned char var58h[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
    unsigned char var58l[64] = {0};
    unsigned int var50[4];
    int var40[16]={0};
    int *var60;
    char *var5c;
    int num2 = 0;
    int num6 = 0,num3 =0,num4 = 0,num5 = 0,num7 = 0,num8 =0;
    int i,j;

    char var58[4],tmp1;


    int namelen;
    char bl;

    namelen= strlen(name);
    memcpy(var58l,time,4);
    memcpy(var58l+4,name,namelen);
    memcpy(var58l+4+namelen,rdata3,7);
    namelen += 10;

#if NETKEEPER_DEBUG
    for(i =0;i <64;i++){
        if(i%8 == 0)printf("\n");
        printf("%x ",var58l[i]);
    }
    printf("\n");
#endif
    num6 = namelen;
    num6 <<= 3;

    for(i = 0;i < 4;i++){
        num3 = i;
        bl = var58l[0x38+i];
        num3 &= 3;
        num4 = num6;
        num3 <<= 3;
        num4 >>= (num3%0xff);
        bl |= (num4%0xff);
        var58l[0x38+i] = bl;
    }

   /*dui chang du chao guo 31 de yao zhu yi te shu chu li*/


#if NETKEEPER_DEBUG
    printf("%x \n",(int)var58h);
    for(i =0;i<16;i++){
    printf("%x ",var58h[i]);
    }
    printf("\n");
#endif
    for (i = 0; i < 4; i++) {
        memcpy(var50+i,var58h+i*4,4);
#if 0
        var50[i] = nk_ntohl(var50[i]);
#else
        var50[i] = ntohl(var50[i]);
#endif
    }
#if NETKEEPER_DEBUG
    for(i = 0;i<4;i++){
    printf("%x ",var50[i]);
    }
#endif

#if 0
    var50[0]=((unsigned int *)var58h)[0];
        var50[0] = nk_ntohl(var50[0]);
    var50[1]=((unsigned int *)var58h)[1];
        var50[1] = nk_ntohl(var50[1]);
    var50[2]=((unsigned int *)var58h)[2];
        var50[2] = nk_ntohl(var50[2]);
    var50[3]=((unsigned int *)var58h)[3];
        var50[3] = nk_ntohl(var50[3]);
#else
    var50[0]=((unsigned int *)var58h)[0];
        var50[0] = ntohl(var50[0]);
    var50[1]=((unsigned int *)var58h)[1];
        var50[1] = ntohl(var50[1]);
    var50[2]=((unsigned int *)var58h)[2];
        var50[2] = ntohl(var50[2]);
    var50[3]=((unsigned int *)var58h)[3];
        var50[3] = ntohl(var50[3]);
#endif


   
    for(i = 0;i < 16;i++){
        memcpy(var40+i,var58l+i*4,4);
#if 0
        var40[i] = nk_ntohl(var40[i]);
#else
        var40[i] = ntohl(var40[i]);
#endif
    }
   /*
     * num2 :eax
     * num3 :ecx
     * num4 :edx
     * num5 :ebp
     * num6 :edi
     * num7 :ebx
     */
    for(i = 0;i < 4;i++){
        var60 = var40;
        num2 = i << 6;
        num2 = num2 + (int)rdata2;
        var5c = (char*)num2;
        for(j = 0;j < 16;j++){
           num2 = (j & 0x80000003); 
           if(num2 < 0){
               num2--;
               num2 |= 0xfffffffc;
               num2++;
           }
           num3 = i;
           num4 = 0 - num2;
           tmp1 = rdata1[num2 + i*4];
           num4 &= 3;
           var58[0] = tmp1;
           num3 = num4 + 1;
           num2 = num4 - 2;
           num3 &= 3;
           num2 &= 3;
           memcpy(&num5,var58h+num3*4,4);
#if 0
           num5 = nk_ntohl(num5);
#else
           num5 = ntohl(num5);
#endif
           memcpy(&num6,var58h+num2*4,4);
#if 0
           num6 = nk_ntohl(num6);
#else
           num6 = ntohl(num6);
#endif
           num2 = i;
           num3 = num4 - 1;
           num3 &= 3;
           memcpy(&num3,var58h+num3*4,4);
#if 0
           num3 = nk_ntohl(num3);
#else
           num3 = ntohl(num3);
#endif

           if(num2 == 0)goto loc_410a8f;
           num2--;
           if(num2 == 0)goto loc_410a77;
           num2--;
           if(num2 == 0)goto loc_410a63;
           num2 = 0;
           num2 = rdata4[j];

           num3 = (~num3);

           num2 = var40[num2];

           num3 |= num5;

           num3 ^= num6;

           num2 += num3;

           goto loc_410aa1;
loc_410a63:
           num2 = 0;
           num3 ^= num6;
           num2 = rdata5[j];
           num3 ^= num5;
           num2 = var40[num2];
           num2 += num3;
           goto loc_410aa1;
loc_410a77:
           num2 = num3;
           num3 &= num5;
           num2 = (~num2);
           num2 &= num6;
           num2 |= num3;  
           num3 = 0;
           num3 = rdata6[j];
           num6 = var40[num3];
           goto loc_410a9f;
loc_410a8f:
           num2 = num5; 
           num6 &= num5;
           num2 = (~num2);
           num2 &= num3;
           num2 |= num6;
           num6 = *var60;
loc_410a9f:
           num2 += num6;
loc_410aa1:
           memcpy(&num6,var58h+num4*4,4);
#if 0
           num6 = nk_ntohl(num6);
#else
           num6 = ntohl(num6);
#endif
           memcpy(&num3,var5c,4);
#if 0
           num3 = nk_ntohl(num3);
#else
           num3 = ntohl(num3);
#endif
           num3 += num6;
           memcpy(&num6,var58,4);
#if 0
           num6 = nk_ntohl(num6);
#else
           num6 = ntohl(num6);
#endif
           num2 += num3;
           num6 &= 0xff;
           num3 = 0x20;
           num7 = num2;
           num3 -= num6;
           num7 = (unsigned int)num7 >> (num3%0xff);/*yihui zi ji ce shi*/
           num3 = num6;
           num6 = 4;
           num2 <<= num3%0xff;
           var5c += 4;

           num7 |= num2;
           num7 += num5;
           num2 = num7;
#if 0
           num2 = nk_ntohl(num2);
#else
           num2 = ntohl(num2);
#endif
           memcpy(var58h+num4*4,&num2,4);
#if 0
           num2 = nk_ntohl(num2);
#else
           num2 = ntohl(num2);
#endif
           
           num4 = (int)var60;
           num4 += 4;
           var60 = (int*)num4;
        }
    }
        for(i = 0;i <4;i ++){
              num8 = var50[i];
              num5 = ((int*)var58h)[i];
#if 0
              num5 = nk_ntohl(num5);
#else
              num5 = ntohl(num5);
#endif
              num5 += num8;
#if 0
              num5 = nk_ntohl(num5);
#else
              num5 = ntohl(num5);
#endif
              ((int*)var58h)[i] = num5;
        }
#if NETKEEPER_DEBUG
        for( i = 0;i < 16;i++){
            printf("%02x",var58h[i]);
        }
        printf("\n");
#endif
        return(var58h[0]);

}
