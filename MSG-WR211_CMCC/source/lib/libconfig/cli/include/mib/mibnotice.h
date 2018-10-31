#if !defined(MIB_NOTICE_H)
#define MIB_NOTICE_H

#define MAX_LINE_LEN 70
#define MAX_NOTICE_URL_LEN 63
#define MAX_TITLE_LEN 20
#define MAX_PRETIME_LEN 3

/*
 *NoticeProfile
 *
 * ͨ�湦��
 *
 *  indexName	ʵ����
 *
 *  enable
 *
 *  line1 ~ line13  �û���д��html����,��Ϊhtml����̫����ĿǰΪ910���ֽڣ����Էֳ�13�Σ�ÿ��70
 *
 *  urlContent	�û���д��url����
 *
 *  urlorhtml	��urlת��������ָ��htmlҳ��
 *
 *  IpGroupName ���͵�ip��ַ��
 *
 *  lively	�����͹����Ƿ�����
 *
 *  time	    �Զ�����ʱ�û���һ������flag�ĸ���ʱ��
 *
 *  pppoeflow	pppoe�˺�����ʱ����ֵ
 *
 *  pppoeday	pppow�˺�����ʱʣ������
 *
 *  pppoehour	pppoe�˺�����ʱʣ��Сʱ��
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
