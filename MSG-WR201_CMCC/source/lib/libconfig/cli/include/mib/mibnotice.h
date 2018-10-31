#if !defined(MIB_NOTICE_H)
#define MIB_NOTICE_H

#define MAX_LINE_LEN 70
#define MAX_NOTICE_URL_LEN 63
#define MAX_TITLE_LEN 20
#define MAX_PRETIME_LEN 3

/*
 *NoticeProfile
 *
 * 通告功能
 *
 *  indexName	实例名
 *
 *  enable
 *
 *  line1 ~ line13  用户填写的html内容,因为html内容太长，目前为910个字节，所以分成13段，每段70
 *
 *  urlContent	用户填写的url内容
 *
 *  urlorhtml	是url转向还是推送指定html页面
 *
 *  IpGroupName 推送的ip地址组
 *
 *  lively	此推送功能是否启用
 *
 *  time	    自动推送时用户第一次上网flag的更新时间
 *
 *  pppoeflow	pppoe账号推送时流量值
 *
 *  pppoeday	pppow账号推送时剩余天数
 *
 *  pppoehour	pppoe账号推送时剩余小时数
 */

typedef enum{
    URL,
    HTML
}UrlOrHtml;

typedef struct noticeprofile{
    char	    instName[MAX_PROFILE_NAME_LENGTH + 1];
    Boolean	    enable;
    char	    line1[MAX_LINE_LEN + 1];
    char	    line2[MAX_LINE_LEN + 1];
    char	    line3[MAX_LINE_LEN + 1];
    char	    line4[MAX_LINE_LEN + 1];
    char	    line5[MAX_LINE_LEN + 1];
    char	    line6[MAX_LINE_LEN + 1];
    char	    line7[MAX_LINE_LEN + 1];
    char	    line8[MAX_LINE_LEN + 1];
    char	    line9[MAX_LINE_LEN + 1];
    char	    line10[MAX_LINE_LEN + 1];
    char	    line11[MAX_LINE_LEN + 1];
    char	    line12[MAX_LINE_LEN + 1];
    char	    line13[MAX_LINE_LEN + 1];
    char	    htmlTitle[MAX_TITLE_LEN + 1];
    char	    manager[MAX_TITLE_LEN + 1];
    char	    urlContent[MAX_NOTICE_URL_LEN + 1];
    UrlOrHtml	    urlorhtml;
    char	    IpGroupName[MAX_IPGROUP_NAME_LENGTH + 1];
    char	    preTime[MAX_PRETIME_LEN + 1];
    Boolean	    lively;
    Uint32	    time;
#if (PPPOE_SRV == FYES)
    Uint32	    pppoerxflow;
    Uint32	    pppoetxflow;
    Uint32	    pppoeday;
    Uint32	    pppoehour;
#endif
}NoticeProfile;


#endif
