/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/

#if (UTTPDB == FYES)
#ifndef MIBPDB_SHARE_H
#define MIBPDB_SHARE_H

#define MAX_PDB_LINE_COUNT 30	
#define MAX_PDB_LINE_LENGTH 128

extern  char *pdb_webQQ[]; 
extern  char *pdb_GAMEURL[]; 
extern  char *pdb_STOCKURL[];
extern  char *pdb_STOCKSW[];
extern  char *pdb_ADVERURL[];
extern  char *pdb_SHOPURL[];
extern  char *pdb_SOCIALURL[];
#if 0
#define MAX_PDB_WEBQQ_LINES	    2
#define MAX_PDB_PORTPDB_LINES	    1
#define MAX_PDB_GAMEURL_LINES	    3
#define MAX_PDB_STOCKURL_LINES	    5
#define MAX_PDB_SHOPURL_LINES	    3
#define MAX_PDB_SOCIALURL_LINES	    2
#endif
#define MAX_PDB_QQ_LINES	    1
#define	MAX_PDB_WLMESSENGER_LINES   1
#define MAX_PDB_ALIIM_LINES	    1
#define MAX_PDB_FETION_LINES	    1
#define MAX_PDB_WEXIN_LINES	    2
#define MAX_PDB_BITTORRENT_LINES    1
#define MAX_PDB_THUNDER_LINES	    2
#define MAX_PDB_QQLIVE_LINES	    1
#define MAX_PDB_PPSTREAM_LINES	    1
#define MAX_PDB_KUGOU_LINES	    1
#define MAX_PDB_PPTV_LINES	    1
#define MAX_PDB_QVOD_LINES	    1
#define MAX_PDB_BAIDUPLAYER_LINES   1
#define MAX_PDB_YOUKU_LINES	    1
#define MAX_PDB_TUDOU_LINES	    1
#define MAX_PDB_AIQI_LINES	    1
#define MAX_PDB_SOUHUVIDEO_LINES    1
#define MAX_PDB_THUNDERKANKAN_LINES 1
#define MAX_PDB_KU6_LINES	    1
#define MAX_PDB_NET56_LINES	    1
#define MAX_PDB_SINAVIDEO_LINES	    1
#define MAX_PDB_TECENTVIDEO_LINES   1
#define MAX_PDB_CNTV_LINES	    1
#define MAX_PDB_IFENGVIDEO_LINES    1
#define MAX_PDB_BAOFENG_LINES	    1
#define MAX_PDB_JOY_LINES	    1
#define MAX_PDB_PPS_LINES	    1
#define MAX_PDB_FUNSHION_LINES	    1
#define MAX_PDB_WEBQQ_LINES	    2
#define MAX_PDB_PORTPDB_LINES	    1
#define MAX_PDB_QQGAME_LINES	    1
#define MAX_PDB_ZHENGTU_LINES	    1
#define MAX_PDB_WANMEI_LINES	    1
#define MAX_PDB_XYQ_LINES	    1
#define MAX_PDB_JWT_LINES	    1
#define MAX_PDB_HFANG_LINES	    1
#define MAX_PDB_POPCAR_LINES	    1
#define MAX_PDB_QQCF_LINES	    1
#define MAX_PDB_QQDNF_LINES	    1
#define MAX_PDB_QQLOL_LINES	    1
#define MAX_PDB_QQSPEED_LINES	    1
#define MAX_PDB_QQX5_LINES	    1
#define MAX_PDB_SDODN_LINES	    1
#define MAX_PDB_THE9SXZ_LINES	    1
#define MAX_PDB_SDOMIR_LINES	    1
#define MAX_PDB_TL3_LINES	    1
#define MAX_PDB_CSOL_LINES	    1
#define MAX_PDB_QQC9_LINES	    1
#define MAX_PDB_SDOMXD_LINES	    1
#define MAX_PDB_QN163_LINES	    1
#define MAX_PDB_WD_LINES	    1
#define MAX_PDB_M3GUO_LINES	    1
#define MAX_PDB_WOT_LINES	    1
#define MAX_PDB_TAOBAO_LINES	    1
#define MAX_PDB_TMALL_LINES	    1
#define MAX_PDB_BUY360_LINES	    1
#define MAX_PDB_DANGDANG_LINES	    1
#define MAX_PDB_AMAZON_LINES	    1
#define MAX_PDB_PAIPAI_LINES	    1
#define MAX_PDB_ALIBABA_LINES	    1
#define MAX_PDB_M18_LINES	    1
#define MAX_PDB_HAO123_LINES	    1
#define MAX_PDB_VANCL_LINES	    1
#define MAX_PDB_VIPSHOP_LINES	    1
#define MAX_PDB_FCLUB_LINES	    1
#define MAX_PDB_XIU_LINES	    1
#define MAX_PDB_SSQY_LINES	    1
#define MAX_PDB_MOONBASA_LINES	    1
#define MAX_PDB_YINTAI_LINES	    1
#define MAX_PDB_VJIA_LINES	    1
#define MAX_PDB_MASAMASO_LINES	    1
#define MAX_PDB_PD89_LINES	    1
#define MAX_PDB_SUNING_LINES	    1
#define MAX_PDB_GOME_LINES	    1
#define MAX_PDB_BUY51_LINES	    1
#define MAX_PDB_COO8_LINES	    1
#define MAX_PDB_NEWEGG_LINES	    1
#define MAX_PDB_LUSEN_LINES	    1
#define MAX_PDB_OKBUY_LINES	    1
#define MAX_PDB_LETAO_LINES	    1
#define MAX_PDB_YOUGOU_LINES	    1
#define MAX_PDB_SCN_LINES	    1
#define MAX_PDB_MBAOBAO_LINES	    1
#define MAX_PDB_LAFASO_LINES	    1
#define MAX_PDB_TIANTIAN_LINES	    1
#define MAX_PDB_SEPHORA_LINES	    1
#define MAX_PDB_DHC_LINES	    1
#define MAX_PDB_JXDYF_LINES	    1
#define MAX_PDB_STRAW_LINES	    1
#define MAX_PDB_HITAO_LINES	    1
#define MAX_PDB_GUOPI_LINES	    1
#define MAX_PDB_CAOMEIPAI_LINES	    1
#define MAX_PDB_REDBABY_LINES	    1
#define MAX_PDB_MUYINGZHIJIA_LINES  1
#define MAX_PDB_YIHAODIAN_LINES	    1
#define MAX_PDB_WOMAI_LINES	    1
#define MAX_PDB_LASHOU_LINES	    1
#define MAX_PDB_NUOMI_LINES	    1
#define MAX_PDB_MEITUAN_LINES	    1
#define MAX_PDB_GAOPENG_LINES	    1
#define MAX_PDB_TUAN55_LINES	    1
#define MAX_PDB_QQZONE_LINES	    1
#define MAX_PDB_RENREN_LINES	    1
#define MAX_PDB_PENGYOU_LINES	    1
#define MAX_PDB_WEIBOSINA_LINES	    1
#define MAX_PDB_KAIXIN001_LINES	    1
#define MAX_PDB_COM51_LINES	    1
#define MAX_PDB_DOUBAN_LINES	    1
#define MAX_PDB_TAOMI_LINES	    1
#define MAX_PDB_WEIBOTECENT_LINES   1
#define MAX_PDB_JIAYUAN_LINES	    1
#define MAX_PDB_JIANGHU_LINES	    1
#define MAX_PDB_BAISOHU_LINES	    1
#define MAX_PDB_ZHENAI_LINES	    1
#define MAX_PDB_BAIHE_LINES	    1
#define MAX_PDB_IPART_LINES	    1
#define MAX_PDB_DUOWAN_LINLES	    1
#define MAX_PDB_BAIDUGAME_LINES	    1
#define MAX_PDB_WAN37_LINES	    1
#define MAX_PDB_G265_LINES	    1
#define MAX_PDB_EEYY_LINES	    1
#define MAX_PDB_KUNLUN_LINES	    1
#define MAX_PDB_GAME4399_LINES	    1
#define MAX_PDB_GAMEPPS_LINES	    1
#define MAX_PDB_GAME7K7K_LINES	    1
#define MAX_PDB_WAN05_LINES	    1
#define MAX_PDB_MAILSINA_LINES	    1
#define MAX_PDB_MAIL126_LINES	    1
#define MAX_PDB_MAIL163_LINES	    1
#define MAX_PDB_MAILQQ_LINES	    1
#define MAX_PDB_MOP_LINES	    1
#define MAX_PDB_TIANYA_LINES	    1
#define MAX_PDB_LIBA_LINES	    1
#define MAX_PDB_KDS_LINES	    1
#define MAX_PDB_TIEXUE_LINES	    1
#define MAX_PDB_IFENGBBS_LINES	    1
#define MAX_PDB_XICI_LINES	    1
#define MAX_PDB_HTTPPROXY_LINES	    1
#define MAX_PDB_SOCKS_LINES	    1
#define MAX_PDB_FILETYPE_LINES	    1
#define MAX_PDB_WEBPOST_LINES	    1
#define MAX_PDB_GAMEURL_LINES	    3
#define MAX_PDB_STOCKURL_LINES	    5
#define MAX_PDB_STOCKSW_LINES       1
#define MAX_PDB_ADVERURL_LINES      6
#define MAX_PDB_SHOPURL_LINES	    3
#define MAX_PDB_SOCIALURL_LINES	    2
#if YAHOO_EN
#define MAX_PDB_YAHOOTONG_LINES	    1
#define MAX_PDB_facebook_LINES	    1
#define MAX_PDB_youtobe_LINES	    1
#define MAX_PDB_twitter_LINES	    1
#endif
#define MAX_PDB_ENTERPRISEQQ_LINES  1
#if (GAME_BOOST == FYES)
#define MAX_PDB_LOL_LINES	    10
#define MAX_PDB_DNF_LINES	    10
#define MAX_PDB_CF_LINES	    10
#define MAX_PDB_WOW_LINES	    10
#define MAX_PDB_MHXY_LINES	    10
#define MAX_PDB_QQYY_LINES	    4
#define MAX_PDB_CCTV_LINES	    1
#define MAX_PDB_SINA_LINES	    1
#define MAX_PDB_QQZB_LINES	    1
#define MAX_PDB_JL_LINES	    10
#define MAX_PDB_DOTA2_LINES	    10
#define MAX_PDB_TKSJ_LINES	    10
#define MAX_PDB_LZG_LINES	    10
#define MAX_PDB_YXSG_LINES	    10
#define MAX_PDB_FKJYOL_LINES	    10
#define MAX_PDB_QQXW_LINES	    10
#define MAX_PDB_WMSJ_LINES	    10
#define MAX_PDB_LSCS_LINES	    10
#define MAX_PDB_QQFC_LINES	    10
#endif
#if (WIFIDOG_AUTH == FYES)
/*微信策略更新*/
#define MAX_PDB_WECHAT_LINES	    3
#endif

extern  char defaultPdbName1[];
extern  char defaultPdbName2[];
extern  char defaultPdbName3[];
extern  char defaultPdbName4[];
extern  char defaultPdbName5[];
extern  char defaultPdbName6[];
extern  char defaultPdbName7[];
extern  char defaultPdbName8[];
extern  char defaultPdbName9[];
extern  char defaultPdbName10[];
extern  char defaultPdbName11[];
extern  char defaultPdbName12[];
extern  char defaultPdbName13[];
extern  char defaultPdbName14[];
extern  char defaultPdbName15[];
extern  char defaultPdbName16[];
extern  char defaultPdbName17[];
extern  char defaultPdbName18[];
extern  char defaultPdbName19[];
extern  char defaultPdbName20[];
extern  char defaultPdbName21[];
extern  char defaultPdbName22[];
extern  char defaultPdbName23[];
extern  char defaultPdbName24[];
extern  char defaultPdbName25[];
extern  char defaultPdbName26[];
extern  char defaultPdbName27[];
extern  char defaultPdbName28[];
extern  char defaultPdbName29[];
extern  char defaultPdbName30[];
extern  char defaultPdbName31[];
extern  char defaultPdbName32[];


#if 0
typedef enum {
    DEFAULT_PDB_QQ = 0,	    /* index = 0 */
    DEFAULT_PDB_MSN,	    /* index =  */
    DEFAULT_PDB_ALI,	    /* index =  */
    DEFAULT_PDB_WEBQQ,      /* index =  */
    DEFAULT_PDB_FETION,	    /* index =  */
    DEFAULT_PDB_BT,	    /* index =  */
    DEFAULT_PDB_THUNDER,    /* index =  */
    DEFAULT_PDB_QQLIVE,	    /* index =  */
    DEFAULT_PDB_PPSTREAM,   /* index =  */
    DEFAULT_PDB_KUGOO,      /* index =  */
    DEFAULT_PDB_PPLIVE,	    /* index =  */
    DEFAULT_PDB_QVOD,	    /* index =  */
    DEFAULT_PDB_QQGAME,     /* index =  */
    DEFAULT_PDB_PAOPAO,     /* index =  */
    DEFAULT_PDB_ZHENGTU,    /* index =  */
    DEFAULT_PDB_WANMEI,     /* index =  */
    DEFAULT_PDB_XIYOU,	    /* index =  */
    DEFAULT_PDB_JWT,        /* index =  */
    DEFAULT_PDB_HFANG,	    /* index =  */
    DEFAULT_PDB_WOW,        /* index =  */
    DEFAULT_PDB_AION,	    /* index =  */
    DEFAULT_PDB_POPCAR,     /* index =  */
    DEFAULT_PDB_GAMEURL,    /* index =  */
    DEFAULT_PDB_STOCKURL,   /* index =  */
    DEFAULT_PDB_SOCIALURL,  /* index =  */
    DEFAULT_PDB_SHOPURL,    /* index =  */
    DEFAULT_PDB_HTTPPROXY,  /* index =  */
    DEFAULT_PDB_SOCKS4,	    /* index =  */
    DEFAULT_PDB_SOCKS5,     /* index =  */
    DEFAULT_PDB_FILETYPE,   /* index =  */
    DEFAULT_PDB_POST,       /* index =  */ 
    DEFAULT_PDB_10000,	    /* index = 31,电信 */
    DEFAULT_PDB_10010,	    /* index = 32,联通 */
    DEFAULT_PDB_10086,	    /* index = 33,移动 */

    /*
     *      * pleaes add above.....
     *           */
    DEFAULT_PDB_COUNT

} DEFAULT_PDB_TYPE;
#endif
typedef enum {
    DEFAULT_PDB_QQ = 1,		/* index = 0  */
    DEFAULT_PDB_MSN,		/* index = 1  */
    DEFAULT_PDB_ALI,		/* index = 2  */
    DEFAULT_PDB_WEBQQ,		/* index = 3  */
    DEFAULT_PDB_FETION,		/* index = 4  */
    DEFAULT_PDB_BT,		/* index = 5  */
    DEFAULT_PDB_THUNDER,	/* index = 6  */
    DEFAULT_PDB_QQLIVE,		/* index = 7  */
    DEFAULT_PDB_PPSTREAM,	/* index = 8  */
    DEFAULT_PDB_KUGOO,		/* index = 9  */
    DEFAULT_PDB_PPLIVE,		/* index = 10 */
    DEFAULT_PDB_QVOD,		/* index = 11 */
    DEFAULT_PDB_BAIDUPLAYER,	/* index = 12 */  
    DEFAULT_PDB_STOCKURL,	/* index = 13 */
    DEFAULT_PDB_YOUKU,		/* index = 14 */
    DEFAULT_PDB_TUDOU,		/* index = 15 */
    DEFAULT_PDB_AIQI,		/* index = 16 */
    DEFAULT_PDB_SOUHUVIDEO,	/* index = 17 */
    DEFAULT_PDB_THUNDERKANKAN,	/* index = 18 */
    DEFAULT_PDB_KU6,		/* index = 19 */
    DEFAULT_PDB_NET56,		/* index = 20 */
    DEFAULT_PDB_SINAVIDEO,	/* index = 21 */
    DEFAULT_PDB_TECENTVIDEO,	/* index = 22 */
    DEFAULT_PDB_CNTV,		/* index = 23 */
    DEFAULT_PDB_IFENGVIDEO,	/* index = 24 */
    DEFAULT_PDB_BAOFENG,	/* index = 25 */
    DEFAULT_PDB_JOY,		/* index = 26 */
    DEFAULT_PDB_PPS,		/* index = 27 */
    DEFAULT_PDB_FUNSHION,	/* index = 28 */
    DEFAULT_PDB_GAMEURL,	/* index = 29 */
    DEFAULT_PDB_QQGAME,		/* index = 30 */
    DEFAULT_PDB_PAOPAO,		/* index = 31 */
    DEFAULT_PDB_ZHENGTU,	/* index = 32 */
    DEFAULT_PDB_WANMEI,		/* index = 33 */
    DEFAULT_PDB_XIYOU,		/* index = 34 */
    DEFAULT_PDB_JWT,		/* index = 35 */
    DEFAULT_PDB_HFANG,		/* index = 36 */
    DEFAULT_PDB_WOW,		/* index = 37 */
    DEFAULT_PDB_AION,		/* index = 38 */
    DEFAULT_PDB_POPCAR,		/* index = 39 */
    DEFAULT_PDB_QQCF,		/* index = 40 */
    DEFAULT_PDB_SDODN ,	        /* index = 45 */
    DEFAULT_PDB_THE9SXZ,	/* index = 46 */
    DEFAULT_PDB_SDOMIR,		/* index = 47 */
    DEFAULT_PDB_TL3,		/* index = 48 */
    DEFAULT_PDB_CSOL,		/* index = 49 */
    DEFAULT_PDB_SDOMXD,		/* index = 51 */
    DEFAULT_PDB_QN163,		/* index = 52 */
    DEFAULT_PDB_WD,		/* index = 53 */
    DEFAULT_PDB_M3GUO,		/* index = 54 */
    DEFAULT_PDB_WOT,		/* index = 55 */
    DEFAULT_PDB_TAOBAO,		/* index = 56 */
    DEFAULT_PDB_TMALL,		/* index = 57 */
    DEFAULT_PDB_BUY360,		/* index = 58 */
    DEFAULT_PDB_DANGDANG,	/* index = 59 */
    DEFAULT_PDB_AMAZON,		/* index = 60 */
    DEFAULT_PDB_PAIPAI,		/* index = 61 */
    DEFAULT_PDB_ALIBABA,	/* index = 62 */
    DEFAULT_PDB_M18,		/* index = 63 */
    DEFAULT_PDB_HAO123,		/* index = 64 */
    DEFAULT_PDB_VANCL,		/* index = 65 */
    DEFAULT_PDB_VIPSHOP,	/* index = 66 */
    DEFAULT_PDB_FCLUB,		/* index = 67 */
    DEFAULT_PDB_XIU,		/* index = 68 */
    DEFAULT_PDB_SSQY,		/* index = 69 */
    DEFAULT_PDB_MOONBASA,	/* index = 70 */
    DEFAULT_PDB_YINTAI,		/* index = 71 */
    DEFAULT_PDB_VJIA,		/* index = 72 */
    DEFAULT_PDB_MASAMASO,	/* index = 73 */
    DEFAULT_PDB_PD89,		/* index = 74 */
    DEFAULT_PDB_SUNING,		/* index = 75 */
    DEFAULT_PDB_GOME,		/* index = 76 */
    DEFAULT_PDB_BUY51,		/* index = 77 */
    DEFAULT_PDB_COO8,		/* index = 78 */
    DEFAULT_PDB_NEWEGG,		/* index = 79 */
    DEFAULT_PDB_LUSEN,		/* index = 80 */
    DEFAULT_PDB_OKBUY,		/* index = 81 */
    DEFAULT_PDB_LETAO,		/* index = 82 */
    DEFAULT_PDB_YOUGOU,		/* index = 83 */
    DEFAULT_PDB_SCN,		/* index = 84 */
    DEFAULT_PDB_MBAOBAO,	/* index = 85 */
    DEFAULT_PDB_LAFASO,		/* index = 86 */
    DEFAULT_PDB_TIANTIAN,	/* index = 87 */
    DEFAULT_PDB_SEPHORA,	/* index = 88 */
    DEFAULT_PDB_DHC,		/* index = 89 */
    DEFAULT_PDB_JXDYF,		/* index = 90 */
    DEFAULT_PDB_STRAW,		/* index = 91 */
    DEFAULT_PDB_HITAO,		/* index = 92 */
    DEFAULT_PDB_GUOPI,		/* index = 93 */
    DEFAULT_PDB_CAOMEIPAI,	/* index = 94 */
    DEFAULT_PDB_REDBABY,	/* index = 95 */
    DEFAULT_PDB_MUYINGZHIJIA,	/* index = 96 */
    DEFAULT_PDB_YIHAODIAN,	/* index = 97 */
    DEFAULT_PDB_WOMAI,		/* index = 98 */
    DEFAULT_PDB_LASHOU,		/* index = 99 */
    DEFAULT_PDB_NUOMI,		/* index = 100*/
    DEFAULT_PDB_MEITUAN,	/* index = 101*/
    DEFAULT_PDB_GAOPENG,	/* index = 102*/
    DEFAULT_PDB_TUAN55,		/* index = 103*/
    DEFAULT_PDB_QQZONE,		/* index = 104*/
    DEFAULT_PDB_RENREN,		/* index = 105*/
    DEFAULT_PDB_PENGYOU,	/* index = 106*/
    DEFAULT_PDB_WEIBOSINA,	/* index = 107*/
    DEFAULT_PDB_KAIXIN,		/* index = 108*/
    DEFAULT_PDB_COM51,		/* index = 109*/
    DEFAULT_PDB_DOUBAN,		/* index = 110*/
    DEFAULT_PDB_TAOMI,		/* index = 111*/
    DEFAULT_PDB_WEIBOTECENT,	/* index = 112*/
    DEFAULT_PDB_JIAYUAN,	/* index = 113*/
    DEFAULT_PDB_JIANGHU,	/* index = 114*/
    DEFAULT_PDB_BAISOHU,	/* index = 115*/
    DEFAULT_PDB_ZHENAI,		/* index = 116*/
    DEFAULT_PDB_BAIHE,		/* index = 117*/
    DEFAULT_PDB_IPART,		/* index = 118*/
    DEFAULT_PDB_DUOWAN,		/* index = 119*/
    DEFAULT_PDB_BAIDUGAME,	/* index = 120*/
    DEFAULT_PDB_WAN37,		/* index = 121*/
    DEFAULT_PDB_G256,		/* index = 122*/
    DEFAULT_PDB_EEYY,		/* index = 123*/
    DEFAULT_PDB_KUNLUN,		/* index = 124*/
    DEFAULT_PDB_GAME4399,	/* index = 125*/
    DEFAULT_PDB_GAMEPPS,	/* index = 126*/
    DEFAULT_PDB_GAME7K7K,	/* index = 127*/
    DEFAULT_PDB_WAN05,		/* index = 128*/
    DEFAULT_PDB_MAILSINA,	/* index = 129*/
    DEFAULT_PDB_MAIL126,	/* index = 130*/
    DEFAULT_PDB_MAIL163,	/* index = 131*/
    DEFAULT_PDB_MAILQQ,		/* index = 132*/
    DEFAULT_PDB_MOP,		/* index = 133*/
    DEFAULT_PDB_TIANYA,		/* index = 134*/
    DEFAULT_PDB_LIBA,		/* index = 135*/
    DEFAULT_PDB_KDS,		/* index = 136*/
    DEFAULT_PDB_TIEXUE,		/* index = 137*/
    DEFAULT_PDB_IFENGBBS,	/* index = 138*/
    DEFAULT_PDB_XICI,		/* index = 139*/
    
    //DEFAULT_PDB_SOCIALURL,	/* index =  */
    //DEFAULT_PDB_SHOPURL,	/* index =  */
    DEFAULT_PDB_HTTPPROXY,	/* index = 140*/
    //DEFAULT_PDB_SOCKS4,	/* index =  */
    DEFAULT_PDB_SOCKS5,		/* index = 141*/
    DEFAULT_PDB_FILETYPE,	/* index = 142*/
    DEFAULT_PDB_POST,		/* index = 143*/ 
    DEFAULT_PDB_10000,		/* index = 144,电信 */
    DEFAULT_PDB_10010,		/* index = 145,联通 */
    DEFAULT_PDB_10086,		/* index = 146,移动 */

    DEFAULT_PDB_ADVERURL,       /* index = 147,视频广告 */

    DEFAULT_PDB_yahootong,		/* index = 148,雅虎通 */
    DEFAULT_PDB_facebook,		/* index = 149,facebook */
    DEFAULT_PDB_youtobe,		/* index = 150,youtobe */
    DEFAULT_PDB_twitter,		/* index = 151,推特 */

    DEFAULT_PDB_ENTERPRISE_QQ,	/* index = 152,企业QQ */

    DEFAULT_PDB_LOL,/* index = 153,英雄联盟 */
    DEFAULT_PDB_DNF,/* index = 154,地下城与勇士 */
    DEFAULT_PDB_CF,/* index = 155,穿越火线 */
    DEFAULT_PDB_WOW1,/* index = 156,魔兽世界 */
    DEFAULT_PDB_MHXY,/* index = 157,梦幻西游 */
    DEFAULT_PDB_QQYY,/* index = ,QQ语音 */
    DEFAULT_PDB_CCTV,/* index = ,CCTV直播*/
    DEFAULT_PDB_SINA,/* index = ,SINA直播*/
    DEFAULT_PDB_QQZB,/* index = ,QQ直播*/
    DEFAULT_PDB_JL,/*index = ,剑灵*/
    DEFAULT_PDB_DOTA2,/*index = ,Dota2*/
    DEFAULT_PDB_TKSJ,/*index = ,坦克世界*/
    DEFAULT_PDB_LZG,/*index = ,龙之谷*/
    DEFAULT_PDB_YXSG,/*index = ,英雄三国*/
    DEFAULT_PDB_FKJYOL,/*index = ,反恐精英OL*/
    DEFAULT_PDB_QQXW,/*index = ,QQ炫舞*/
    DEFAULT_PDB_WMSJ,/*index = ,完美世界*/
    DEFAULT_PDB_LSCS,/*index = ,炉石传说*/
    DEFAULT_PDB_QQFC,/*index = ,QQ飞车*/
    DEFAULT_PDB_STOCKSW, /*index = 172,股票软件*/
    DEFAULT_PDB_WECHAT,	    /*index = , wifidog微信策略*/
    DEFAULT_PDB_WEXIN,	    /*index = , 禁止微信*/
    /*
     *      * pleaes add above.....
     *           */
    DEFAULT_PDB_COUNT

} DEFAULT_PDB_TYPE;


typedef enum PdbMode {
    Predefined=0,
    Userdefined
}PdbMode;

typedef enum PdbType {
    L7Pdb=0,
    DnsPdb,
    PortPdb,
    RoutePdb,
    EnterpriseQQPdb,
#if (GAME_BOOST == FYES)
    SpeedupPdb,
#endif
#if (WIFIDOG_AUTH == FYES)
    wifidogUp,
#endif
}PdbType;


#if 0
typedef enum  pdbCategory{
    PDB_IM=0,
    PDB_P2P,
    PDB_GAME,
    PDB_DNS,
    PDB_OTHER
}PdbCategory;
#endif
typedef enum  pdbCategory{
    PDB_IM=1,
    PDB_P2P,
    PDB_STOCK,
    PDB_VIDEO,
    PDB_GAME,
    PDB_SHOP,
    PDB_SNS,
    PDB_WEBGAME,
    PDB_MAIL,
    PDB_FORUM,  
    PDB_OTHER
}PdbCategory;

typedef struct st_pdb_profile {
    mibHead head;
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    int policyVersion;/*用于检测更新*/
#endif
    PdbMode     mode;
    PdbType     type;
    PdbCategory category;
    int         index;
    char        description[128];
    char        line[MAX_PDB_LINE_COUNT][MAX_PDB_LINE_LENGTH+1];
} PdbProfile;
extern PdbProfile *pdbProfile;

#endif
#endif
