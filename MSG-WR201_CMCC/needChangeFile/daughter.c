/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be  copied,    ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1990-1997 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

#include <string.h>
#include "uttMachine.h"
#include <stdlib.h>
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include <netinet/in.h> /* htonl() */
#include "daughter.h"
#if (UTTPDB == FYES)
#if 0
/*
 * domainÀàÐÍµÄpdbÄÚÈÝ   ¿ÉÒÔÌîÐ´¶àÐÐ
 */
char *pdb_webQQ[] = {
    "web.qq.com,w.qq.com,wed.qq.com,webqq.qq.com",
    "web2.qq.com"
};   
/*
 *portÀàÐÍµÄ¶Ë¿ÚºÅ£¬ÒÔ·ÖºÅ¸ô¿ª£¬Ö»ÄÜÌîÐ´ÔÚÒ»ÐÐ
 *
 */
char *pdb_paopaotang[] = {
    "3838,14392,5858,4848",
};   
char *pdb_zhengtu[] = {
    "6020",
};   
char *pdb_wanmei[] = {
    "29000",
};   
char *pdb_JWT[] = {
    "25520,25510,25511",
};   
char *pdb_HFANG[] = {
    "33000,1201,1203",
};   
char *pdb_WOW[] = {
    "1119,3724",
};   
char *pdb_AION[] = {
    "27500,7777,2106",
};   
char *pdb_POPCAR[] = {
    "36567,39311",
};   
char *pdb_GAMEURL[] = {
    "games.sina.com.cn,sgamer.com,17173.com,games.tom.com,chinagames.net,ourgame.com",
    "game.qq.com,game.com.cn,ali213.com,games.sohu.com,nie.163.com,pcpop.com,yx99.com",
    "wanyouxi.com,yx007.cn,game.21cn.com",
};   
char *pdb_STOCKURL[] = {
    "finance.sina.com.cn,sse.com.cn,szse.cn,cnstock.com,cs.com.cn,f10.com.cn,gtja.com",
    "sw108.com,htsec.com,guosen.com,gf.com.cn,newone.com.cn,htsc.com.cn,cs.ecitic.com",
    "dfzq.com.cn,cjis.cn,lhzq.com,95579.com,bigsun.com.cn,stock.eastmoney.com",
    "stock2000.com.cn,qlzq.com.cn,stock.cnfol.com,stock01.com,gsstock.com,chinastock.com.cn",
    "cc168.com.cn,ebscn.com,bocionline.com,skyone.com.cn,xyzq.com.cn",
};   
char *pdb_SHOPURL[] = {
    "taobao.com,tmall.com,amazon.cn,dangdang.com,360buy.com,vancl.com,newegg.com.cn",
    "yihaodian.com,paipai.com,icson.com,lashou.com,meituan.com,nuomi.com,55tuan.com",
    "gaopeng.com,jumei.com",
};
char *pdb_SOCIALURL[]={
    "renren.com,kaixin001.com,51.com,xiaonei.com,hainei.com,xiaoyou.qq.com,5gme.com",
    "luobohua.com,weibo.com,t.sina.com.cn,t.qq.com,t.sohu.com,t.163.com,fanfou.com",
};

/* 
 * µçÐÅ¾²Ì¬Â·ÓÉpdb²ßÂÔ 
 * ¸ñÊ½£º1122334404ÓÃ×Ö½ÚÀ´±íÊ¾hex,Ðè×ª»»³Éhex±íÊ¾ÐÎÊ½0x1122334404¡£ 11223344±íÊ¾Ä¿µÄipµØÖ·£¬04±íÊ¾ÑÚÂë¡£
 * pdbÒ»ÐÐ´æ12¸öÂ·ÓÉ²ßÂÔ£¬¼´12¸ö1122334404£¬Õ¼120¸ö×Ö½Ú
 * ×ÜÐÐÊý²»ÄÜ³¬¹ý30ÐÐ
 */
char *pdb_10000[] = {
"013000000e014400000e015000000d01b400000e01c000000c0e1000000c0e6800000d0e7000000c0e8600000f0e9000000c0ed000000c1b1000000c",
"1b8000000f1b9400000e1b9800000d1bb800000d1be000000e314000000b317000000d3a2000000b3ad000000c3b2000000b3bac00000e3ca000000b",
"3ceb0000103d8000000e3d8400000f3d860000123d864000133d888000113d890000113d8ac000123d8b0000113d8bc000123d8c00000e3d9000000e",
"3d9600000f3d9800000e3d9d0000103d9f4000123d9f8000113da00000103da14000123da400000f3da60000103da90000103daa00000f3dac00000e",
"3db10000103db20000103db40000113db70000103db800000e3dbc0000103dbd8000113dbe00000f6e5000000c6e9800000e6e9c00000f6ea600000f",
"6ead6000136eb000000d6f4800000d6f7000000f6f7800000d6faa0000106fac00000e6fb000000d6fe000000e704200000f706200000f706400000e",
"707000000e707400000f710c00000e711000000f711800000e713e00000f714000000a718000000f718400000e718800000d71da00000f71dc00000e",
"71f000000d71f800000e725000000c726000000d726800000e7287000010728a00000f72d800000d72e000000c739400000e739800000f73a800000e",
"73c000000a7401000010740400000e740800000e741000000c743400000e74c000001074cf00001074d000000e74d4a0001474d700001074e000000c",
"74f600000f74f800000f74fc00000f751000000c752000000c7539000010753c00000e754000000d755000000c765400000f767000000c76b400000e",
"76d500001076ef00001076f800000d770000000f7729000010773c000010775400000e776000000d777800000d778000000c779000000e782000000d",
"782800000e784400000e7858080015790800000d792000000e793800000f793a000011793b000010793c00000e79cc00000e79e000000c7a0400000e",
"7a665000147ae000000c7af000000d7b3400000e7b6000000f7b650000107b670000117b950000107b9600000f7ba000000c7bb10000107bb200000f",
"7bb400000e7bb800000e7bf400000e7c1f0000107c4800000d7c7000000d7ce000000c7d4000000b7d4800000d7d5000000c7d6800000d7d7000000c",
"af0000000caf1e00000fb46000000bb48800000db49800000db4a000000cb4d400000fb62000000cb65400000eb66000000cb68000000cb69000000d",
"b6c800000db6f000000db70000000ab78000000bb7a000000dca4bd00014ca60600013ca60800011ca61000011ca62200013ca62400012ca62800011",
"ca6400000fca66000011ca66c00013ca67000010ca6800000fca6b800011ca6d000010ca6e800012ca6f000011ca6fa00013ca6fc00012caade00013",
"cb6ea00013cb82200013cbd4000014d2c0600013da0000000eda0400000fda06000010da0d000010da0e00000fda1000000eda14000010da15000011",
"da1600000fda1e00000fda20000010da3e800011da3f000010da4000000fda42000010da43000011da4600000fda4800000dda5000000cdaf4400013",
"db8000000cdb9000000ddb9800000fdb9f400012db9f800011dca000000bdde000000ddde800000eddec00000fddee000010ddef000011ddef800011",
"de4000000dde4800000dde5000000cdea800000ddeb000000cded000000cdef000000ddf0800000ddf9000000cdfd600000fdfdc00000fdff000000d", /* ¹²21ÐÐ */
};

/*
 * ÁªÍ¨¾²Ì¬Â·ÓÉ²ßÂÔ£¬¸ñÊ½Í¬µçÐÅ
 */
char *pdb_10010[] = {
"01bc00000e1b0800000d1b2400000e1b2800000d3a1000000d3af000000c3c0000000b3cce0000103cd000000c3d3000000d3d850000113d86600013",
"3d868000113d870000103d880000123d884000123d898000113d8a0000113d8a8000123d8b8000123d9400000f3d9c0000103d9e0000103d9f000012",
"3da10000123da18000113da20000103da30000103da70000103da80000103db00000103db30000103db48000113db50000103db60000103dbd000011",
"3df000000e6e0600000f6e1000000e6e3400000f6e4800000f704000000f705000000d705800000d706000000f706d800011706f000010707a00000f",
"708400001070c000000e70e000000b710000000d710800000f713800000f713a000010713b00001171c200000f71c800000f71cc00000e71e000000c",
"72f000000c732e000010733000000c7355c00012740200000f745f000010747000000e747400000f750800000d764800000d765000000f769000000e",
"76d4000010770400000e77240000107727000010773000000d773e000010776c00000f777000000d77a200000f77a400000e77b000000c77f800000e",
"780000000c785000000d791000000d791800000e791c00000f791e000010791f0000107a6000000f7a8800000d7a9c00000e7ac000000e7b0400000e",
"7b0800000d7b7000000c7b8000000d7b890000107b8a00000f7b9000000e7b940000107b9800000d7bbc00000e7be800000e7c4000000d7c5800000d",
"7c8000000d7ca000000d7d2000000c7dd200000fca60000012ca60400013ca61800011ca62000013ca63000010ca65800013ca66800012ca66e00013",
"ca6a000010ca6b000011ca6c000010ca6e000011ca6ec00012ca6f800012ca7f700014ca82e00013ca8e100014ca98b00014caa5b00014cb5d000010",
"cba6a00013d20c00000fd20ea00013d20ec00012d20f000011d20f800012d215000010d216000010d233000010d23400000fd24a600013d24a800013",
"d24e000013d25200000fd35a00000fd35c00000ed36000000fd390000014d39d600013d39e000010d3a3000010da07000010da0800000eda0c000010",
"da15800011da1800000eda1c00000fda3800000eda3c00000fda3e000011da43800011da4400000fda6800000eda6c00000fdaf4200013daf4800015",
"db9a00000fdb9c00000fdb9e000010db9f000012dcc000000cdcf800000edcfc000010dd0000000cddc000000bde8000000cdea000000e", /* ¹²17ÐÐ */
};

/*
 * ÒÆ¶¯¾²Ì¬Â·ÓÉ²ßÂÔ£¬¸ñÊ½Í¬µçÐÅ
 */
char *pdb_10086[] = {
"758000000ad367000011d38800000ddac800000ddd8200000fddb000000d", /* ¹²1ÐÐ */
};
#endif
char *pdb_QQ[] = {
    "^([!\\xc8]?[!\\xc8]?\\x02.+\\x03$|[\\x01\\x02]?.[\\x01\\x03][\\x01\\x02][\\x2c\\x04])",
};
char *pdb_enterpriseQQ[] = {
    "2b1e,2b24,2b2a,2b30,3112,3118",
};
char *pdb_WLMessenger[] = {
    "(usr\\x20[0-9]+\\x20(twn|sso)\\x20\\x49\\x20.+\\x0d\\x0a$|^RASI |^RAT )",
};
char *pdb_AliIM[] = {
    "^\\x88\\x06",
};
char *pdb_Fetion[] = {
    "^R fetion\\.com\\.cn",
};

char *pdb_Wechat[] = {
    "(^.\\x10\\x01)|(^GET /mmhead/.+gzip\\x0d\\x0a\\x0d\\x0a$)|(^POST http://short.weixin.qq.com)|(^POST ",
    "http://long.weixin.qq.com)|(^POST http://szshort.weixin.qq.com)|(^POST http://szlong.weixin.qq.com)",
};

char *pdb_BitTorrent[] = {
    "^(\\x13bittorrent protocol|azver\\x01$|get /scrape\\?info_hash=get /announce\\?info_hash=|get /client/bitcomet/|GET /data\\?fid=)|d1:ad2:id20:|\\x08'7P\\)[RP]",
};
char *pdb_Thunder[] = {
    "^((POST / HTTP/1\\.1.+)?\\x65..\\x01|\\xe4(\\x01|.+\\x9b\\x49\\x4d\\+)$|\\x13BitTorrent protocol|",
    "POST /UPVP?gs=Xunlei|.+\\x06sandai\\x03net\\x01\\x01$)"
};
/*char *pdb_QQLive[] = {
    "^\\xfe..\\xde|(^get .+qq\\.com.+\\.(mp4|flv)) |(^get .+\\.(mp4|flv).+qq\\.com)",
};*/
char *pdb_QQLive[] = {
    "*.p.qq.com,*.tv.qq.com,*.tc.qq.com,*.p2p.qq.com,*.video.qq.com,*.qqlive.qq.com",
};
/*char *pdb_PPStream[] = {
    "^(.\\x43.+\\x70\\x70\\x73|get.+User-Agent:PPStream\\-Client/1\\.0)"
};*/
char *pdb_PPStream[] = {
    "*.video.iqiyi.com,*.video.qiyi.com,*.dns.iqiyi.com",
};
/*char *pdb_KuGou[] = {
    "^\\x64.....\\x70....\\x50\\x37",
};*/
char *pdb_KuGou[] = {
    "*.kugou.com,*.kugou.net",
};
/*char *pdb_PPTV[] = {
    "(^....\\x52..?[\\x01-\\x0f]\\x01)|(client$)|^GET.+((/live/)|(\\.mp4)).+=ppap",
};*/
char *pdb_PPTV[] = {
    "*.pptv.com,*.pplive.cn,*.pplive.com",
};
char *pdb_Qvod[] = {
    "^........?.?\\x30\\x31\\x30\\x31|^..?.?.?.?QVOD00|^\\x05\\x01\\x01\\x22\\x01\\x10",
};
char *pdb_QQGame[] = {
    "(^.?.?\\x02\\x50[\\x05\\x06].+\\x03$)",
};
/*
 * domainÀàÐÍµÄpdbÄÚÈÝ   ¿ÉÒÔÌîÐ´¶àÐÐ
 */
char *pdb_webQQ[] = {
    "web.qq.com,w.qq.com,wed.qq.com,webqq.qq.com",
    "web2.qq.com,web3.qq.com"
}; 
char *pdb_BaiduPlayer[] = {
    "*.p2sp.baidu.com,*player.baidu.com,stat.client.baidu.com,*ikan.baidu.com,baoku.baidu.com,*.v.baidu.com,*.hiphotos.baidu.com",
};
char *pdb_YouKu[] = {
    "*.youku.com",
};
char *pdb_TuDou[] = {
    "*.tudou.com",
};
char *pdb_AiQi[] = {
    "*.iqiyi.com",
};
char *pdb_SouHuVideo[] = {
    "tv.sohu.com",
};
char *pdb_ThunderKanKan[] = {
    "*.xunlei.com,*.kankan.com,*.sandai.net",
};
char *pdb_Ku6[] = {
    "*.ku6.com",
};
char *pdb_Net56[] = {
    "*.56.com",
};
char *pdb_SinaVideo[] = {
    "video.sina.com.cn",
};
char *pdb_TencentVideo[] = {
    "v.qq.com",
};
char *pdb_CNTV[] = {
    "*.cntv.com,*.cntv.cn",
};
char *pdb_iFengVideo[] = {
    "v.ifeng.com",
};
char *pdb_BaoFeng[] = {
    "*.baofeng.com",
};
char *pdb_Joy[] = {
    "*.joy.cn",
};
char *pdb_PPS[] = {
    "*.pps.tv",
};
char *pdb_FunShion[] = {
    "*.funshion.com,*.fun.tv",
};
/*
 *portÀàÐÍµÄ¶Ë¿ÚºÅ£¬ÒÔ·ÖºÅ¸ô¿ª£¬Ö»ÄÜÌîÐ´ÔÚÒ»ÐÐ
 *
 */
char *pdb_paopaotang[] = {
    "3838",
};   
char *pdb_zhengtu[] = {
    "^\\x67\\x9d\\xfc\\x9b\\x0d\\x74\\xc1\\x20",
};   
char *pdb_wanmei[] = {
    "^\\x01.\\x10.+\\x03\\x04$",
};
char *pdb_xyq[] = {
    "^.?.?\\x10\\x80\\xf1....?.?.\\x37\\xc6\\x97",
};
char *pdb_JWT[] = {
    "^.?.?.?.\\xac\\xac.?.\\xac\\xac\\xac",
};   
char *pdb_HFANG[] = {
    "^\\xaf\\x01\\x31\\x03.+\\x04\\x0c\\x69\\xba\\xe7\\x3a\\xa5\\x85\\xda\\x52\\x4e\\xc5",
};   
char *pdb_WOW[] = {
    "1119,3724",
};   
char *pdb_AION[] = {
    "27500,7777,2106",
};   
char *pdb_POPCAR[] = {
    "^GET /kartclient/kartlogin.aspx.+FCQ.+EXT",
}; 
char *pdb_QQCF[] = {
    "^(\\x40|\\x6a)\\x02\\x0b\\x82\\x03.+\\x03$",
};
char *pdb_QQDNF[] = {
    "^GET..version\\.ini.+dnfdl.qq.com",
};
char *pdb_QQLOL[] = {
    "^.?.?\\x02\\x50\\x06.+\\x03$",
};
char *pdb_QQSPEED[] = {
    "^.?\\x02\\x50\\x06.+\\x03$",
};
char *pdb_QQX5[] = {
    "^.?\\x02\\x50\\x06.+\\x03$",
};
char *pdb_SDODN[] = {
    "^get /dn/sndalist/sndalist.xml",
};
char *pdb_THE9SXZ[] = {
    "^\\x27\\x72\\x01..\\x42\\x06$",
};
char *pdb_SDOMIR[] = {
    "^\\x44\\xff\\x44\\xff",
};
char *pdb_TL3[] = {
    "^\\x63\\x01.\\x7b",
};
char *pdb_CSOL[] = {
    "^.\\x33\\x18.\\x02.....\\x50\\x6a\\x20\\xe5",
};
char *pdb_QQC9[] = {
    "^.?\\x02\\x50\\x06.+\\x03$",
};
char *pdb_SDOMXD[] = {
    "^\\x0e\\x66\\x01.+\\x04$",
};
char *pdb_QN163[] = {
    "^\\x80\\x7a\\x01\\x03\\x01\\x51\\x20\\x39",
};
char *pdb_WD[] = {
    "^\\x4d\\x5a.+\\x33\\x46\\x44\\x42\\x41$",
};
char *pdb_M3GUO[] = {
    "^get /ClientCY.spl HTTP/1.1",
};
char *pdb_WOT[] = {
    "^\\x01\\x08\\x01.?..\\x3a.+\\x02.?$",
};

char *pdb_TaoBao[] = {
    "*.taobao.com",
};
#if YAHOO_EN
char *pdb_facebook[] = {
    "*.facebook.com",
};
char *pdb_youtobe[] = {
    "*.youtube.com",
};
char *pdb_twitter[] = {
    "*.twitter.com",
};
#endif
char *pdb_TMall[] = {
    "*.tmall.com",
};
char *pdb_buy360[] = {
    "*.360buy.com,*.jd.com",
};
char *pdb_DangDang[] = {
    "*.dangdang.com",
};
char *pdb_Amazon[] = {
    "*.amazon.cn",
};
char *pdb_PaiPai[] = {
    "*.paipai.com",
};
char *pdb_Alibaba[] = {
    "*.1688.com,*.alibaba.com",
};
char *pdb_M18[] = {
    "*.m18.com",
};
char *pdb_hao123[] = {
    "zhekou.hao123.com",
};
char *pdb_Vancl[] = {
    "*.vancl.com",
};
char *pdb_VipShop[] = {
    "*.vipshop.com,*.vip.com",
};
char *pdb_Fclub[] = {
    "www.fclub.cn",
};
char *pdb_Xiu[] = {
    "*.xiu.com",
};
char *pdb_SSQY[] = {
    "*.shishangqiyi.com",
};
char *pdb_MoonBasa[] = {
    "*.moonbasa.com",
};
char *pdb_YinTai[] = {
    "*.yintai.com",
};
char *pdb_Vjia[] = {
    "*.vjia.com",
};
char *pdb_MasaMaso[] = {
    "*.masamaso.com",
};
char *pdb_pb89[] = {
    "*.pb89.com,*.peacebird.com.cn",
};
char *pdb_SuNing[] = {
    "www.suning.com",
};
char *pdb_Gome[] = {
    "www.gome.com.cn",
};
char *pdb_buy51[] = {
    "www.51buy.com,www.yixun.com",
};
char *pdb_Coo8[] = {
    "*.coo8.com",
};
char *pdb_NewEgg[] = {
    "*.newegg.com.cn",
};
char *pdb_LuSen[] = {
    "*.lusen.com",
};
char *pdb_okBuy[] = {
    "*.okbuy.com",
};
char *pdb_LeTao[] = {
    "*.letao.com",
};
char *pdb_YouGou[] = {
    "*.yougou.com",
};
char *pdb_scn[] = {
    "*.s.cn",
};
char *pdb_mBaoBao[] = {
    "*.mbaobao.com",
};
char *pdb_Lafaso[] = {
    "*.lafaso.com,*.lefeng.com",
};
char *pdb_TianTian[] = {
    "*.tiantian.com",
};
char *pdb_Sephora[] = {
    "*.sephora.cn",
};
char *pdb_DHC[] = {
    "*.dhc.net.cn",
};
char *pdb_jxdyf[] = {
    "*.jxdyf.com",
};
char *pdb_straw[] = {
    "*.strawberrynet.com",
};
char *pdb_hiTao[] = {
    "*.hitao.com",
};
char *pdb_guopi[] = {
    "*.guopi.com",
};
char *pdb_caomeipai[] = {
    "*.caomeipai.com",
};
char *pdb_redBaby[] = {
    "*.redbaby.com.cn,redbaby.suning.com",
};
char *pdb_muyingzhijia[] = {
    "*.muyingzhijia.com",
};
char *pdb_yihaodian[] = {
    "*.yihaodian.com,*.yhd.com",
};
char *pdb_womai[] = { 
    "*.womai.com",
};
char *pdb_lashou[] = {
    "*.lashou.com",
};
char *pdb_nuomi[] = {
    "*.nuomi.com",
};
char *pdb_meituan[] = {
    "*.meituan.com",
};
char *pdb_gaopeng[] = {
    "*.gaopeng.com",
};
char *pdb_tuan55[] = {
    "*.55tuan.com",
};
char *pdb_QQZone[] = {
    "qzone.qq.com,*.qzone.qq.com",
};
char *pdb_RenRen[] = {
    "*.renren.com",
};
char *pdb_PengYou[] = {
    "*.pengyou.com",
};
char *pdb_weiboSina[] = {
    "weibo.com,*.weibo.com",
};
char *pdb_kaixin001[] = {
    "*.kaixin001.com",
};
char *pdb_com51[] = {
    "*.51.com",
};
char *pdb_douban[] = {
    "*.douban.com",
};
char *pdb_taomi[] = {
    "*.61.com",
};
char *pdb_weiboTencent[] = {
    "t.qq.com",
};
char *pdb_jiayuan[] = {
    "*.jiayuan.com",
};
char *pdb_jianghu[] = {
    "*.taobaos.org",
};
char *pdb_baisohu[] = {
    "bai.sohu.com",
};
char *pdb_zhenai[] = {
    "*.zhenai.com",
};
char *pdb_baihe[] = {
    "*.baihe.com",
};
char *pdb_ipart[] = {
    "www.i-part.com.cn,*.ipart.cn",
};
char *pdb_DuoWan[] = {
    "*.duowan.com",
};
char *pdb_BaiduGame[] = {
    "youxi.baidu.com,iwan.baidu.com",
};
char *pdb_Wan37[] = {
    "*.37wan.com",
};
char *pdb_G265[] = {
    "*.265g.com",
};
char *pdb_eeyy[] = {
    "*.eeyy.com",
};
char *pdb_kunlun[] = {
    "*.kunlun.com",
};
char *pdb_Game4399[] = {
    "*.4399.com",
};
char *pdb_Gamepps[] = {
    "g.pps.tv",
};
char *pdb_Game7k7k[] = {
    "*.7k7k.com",
};
char *pdb_Wan05[] = {
    "*.05wan.com",
};
char *pdb_MailSina[] = {
    "mail.sina.com.cn",
};
char *pdb_Mail126[] = {
    "*.126.com",
};
char *pdb_Mail163[] = {
    "mail.163.com,email.163.com,entry.mail.163.com",
};
char *pdb_MailQQ[] = {
    "mail.qq.com",
};
char *pdb_Mop[] = {
    "*.mop.com",
};
char *pdb_TianYa[] = {
    "*.tianya.cn",
};
char *pdb_LiBa[] = {
    "*.liba.com",
};
char *pdb_Kds[] = {
    "*.kdslife.com,club.pchome.net",
};
char *pdb_TieXue[] = {
    "*.tiexue.net",
};
char *pdb_iFengbbs[] = {
    "bbs.ifeng.com",
};
char *pdb_XiCi[] = {
    "*.xici.net",
};

char *pdb_GAMEURL[] = {
    "games.sina.com.cn,*.sgamer.com,*.17173.com,games.tom.com,*.chinagames.net,*.ourgame.com",
    "game.qq.com,game.com.cn,*.ali213.com,games.sohu.com,nie.163.com,*.pcpop.com,*.yx99.com",
    "*.wanyouxi.com,*.yx007.cn,game.21cn.com,www.game.com.cn,www.a10.com",
};   
char *pdb_STOCKURL[] = {
    "finance.sina.com.cn,*.sse.com.cn,*.szse.cn,*.cnstock.com,*.cs.com.cn,*.f10.com.cn,*.gtja.com",
    "*.sw108.com,*.htsec.com,*.guosen.com,*.gf.com.cn,*.newone.com.cn,*.htsc.com.cn,*.cs.ecitic.com",
    "*.dfzq.com.cn,*.cjis.cn,*.lhzq.com,*.95579.com,*.bigsun.com.cn,stock.eastmoney.com",
    "*.stock2000.net,*.qlzq.com.cn,*.cnfol.com,*.stock01.com,*.gsstock.com,*.chinastock.com.cn",
    "*.cc168.com.cn,*.ebscn.com,*.bocionline.com,*.skyone.com.cn,*.xyzq.com.cn,*.china-invs.cn",
};   
char *pdb_ADVERURL[] = {
    "data.vod.itc.cn,show.cupid.qiyi.com,player.as.pptv.com,asimgs.pplive.cn,1.allyes.com.cn",
    "acs.56.com,acs.agent.56.com,acs.agent.v-56.com,bill.agent.56.com.bill.agent.v-56.com",
    "stat2.corp.56.com,union.56.com,v16.56.com,pole.6rooms.com,shrek.6.cn,simba.6.cn",
    "at-img1.tdimg.com,at-img2.tdimg.com,at-img3.tdimg.com,adguanggao.eee114.com,server1.adpolestar.net",
    "casting.openv.com,m.openv.tv,uniclick.openv.com,mcfg.sandai.net,biz5.sandai.net,advstat.xunlei.com,86log.joy.cn",
    "*.l.qq.com,86file.megajoy.com",
};
char *pdb_STOCKSW[] = {
    "5188,6000,8601,9000,9150,9190,9010,4010,5401,5403,5410,5360,5091,8887,5062",
};
char *pdb_HttpProxy[] = {
    "\\x50\\x72\\x6f\\x78\\x79\\x2d\\x43\\x6f\\x6e\\x6e\\x65\\x63\\x74\\x69\\x6f\\x6e\\x3a\\x20",
};
char *pdb_Socks[] = {
    "\\x05[\\x01-\\x08]*\\x05[\\x01-\\x08]?.*\\x05[\\x01-\\x03][\\x01\\x03].*\\x05[\\x01-\\x08]?[\\x01\\x03]",
};
char *pdb_FileType[] = {
    "^GET \\/.[^=]+(\\.exe|\\.dll|\\.vbs|\\.bat|\\.sys) HTTP/(0\\.9|1\\.0|1\\.1)",
};
char *pdb_WebPost[] = {
    "post .+ http/(0\\.9|1\\.0|1\\.1)\\x0d\\x0a",
};

/* 
 * µçÐÅ¾²Ì¬Â·ÓÉpdb²ßÂÔ 
 * ¸ñÊ½£º1122334404ÓÃ×Ö½ÚÀ´±íÊ¾hex,Ðè×ª»»³Éhex±íÊ¾ÐÎÊ½0x1122334404¡£ 11223344±íÊ¾Ä¿µÄipµØÖ·£¬04±íÊ¾ÑÚÂë¡£
 * pdbÒ»ÐÐ´æ12¸öÂ·ÓÉ²ßÂÔ£¬¼´12¸ö1122334404£¬Õ¼120¸ö×Ö½Ú
 * ×ÜÐÐÊý²»ÄÜ³¬¹ý30ÐÐ
 */
char *pdb_10000[] = {
"013000000e014400000e015000000d01b400000e01c000000c0e1000000c0e6800000d0e7000000c0e8600000f0e9000000c0ed000000c1b1000000c",
"1b8000000f1b9400000e1b9800000d1bb800000d1be000000e314000000b317000000d3a2000000b3ad000000c3b2000000b3bac00000e3ca000000b",
"3ceb0000103d8000000e3d8400000f3d860000123d864000133d888000113d890000113d8ac000123d8b0000113d8bc000123d8c00000e3d9000000e",
"3d9600000f3d9800000e3d9d0000103d9f4000123d9f8000113da00000103da14000123da400000f3da60000103da90000103daa00000f3dac00000e",
"3db10000103db20000103db40000113db70000103db800000e3dbc0000103dbd8000113dbe00000f6e5000000c6e9800000e6e9c00000f6ea600000f",
"6ead6000136eb000000d6f4800000d6f7000000f6f7800000d6faa0000106fac00000e6fb000000d6fe000000e704200000f706200000f706400000e",
"707000000e707400000f710c00000e711000000f711800000e713e00000f714000000a718000000f718400000e718800000d71da00000f71dc00000e",
"71f000000d71f800000e725000000c726000000d726800000e7287000010728a00000f72d800000d72e000000c739400000e739800000f73a800000e",
"73c000000a7401000010740400000e740800000e741000000c743400000e74c000001074cf00001074d000000e74d4a0001474d700001074e000000c",
"74f600000f74f800000f74fc00000f751000000c752000000c7539000010753c00000e754000000d755000000c765400000f767000000c76b400000e",
"76d500001076ef00001076f800000d770000000f7729000010773c000010775400000e776000000d777800000d778000000c779000000e782000000d",
"782800000e784400000e7858080015790800000d792000000e793800000f793a000011793b000010793c00000e79cc00000e79e000000c7a0400000e",
"7a665000147ae000000c7af000000d7b3400000e7b6000000f7b650000107b670000117b950000107b9600000f7ba000000c7bb10000107bb200000f",
"7bb400000e7bb800000e7bf400000e7c1f0000107c4800000d7c7000000d7ce000000c7d4000000b7d4800000d7d5000000c7d6800000d7d7000000c",
"ab2800000daf0000000caf1e00000fb46000000bb48800000db49800000db4a000000cb4d400000fb62000000cb65400000eb66000000cb68000000c",
"b69000000db6c800000db6f000000db70000000ab78000000bb7a000000dca4bd00014ca60600013ca60800011ca61000011ca62200013ca62400012",
"ca62800011ca6400000fca66000011ca66c00013ca67000010ca6800000fca6b800011ca6d000010ca6e800012ca6f000011ca6fa00013ca6fc00012",
"caade00013cb6ea00013cb82200013cbd4000014d2c0600013da0000000eda0400000fda06000010da0d000010da0e00000fda1000000eda14000010",
"da15000011da1600000fda1e00000fda20000010da3e800011da3f000010da4000000fda42000010da43000011da4600000fda4800000dda5000000c",
"daf4400013db8000000cdb9000000ddb9800000fdb9f400012db9f800011dca000000bdde000000ddde800000eddec00000fddee000010ddef000011",
"ddef800011de4000000dde4800000dde5000000cdea800000ddeb000000cded000000cdef000000ddf0800000ddf9000000cdfd600000fdfdc00000f",
"dff000000d", /* ¹²22ÐÐ */
};

/*
 * ÁªÍ¨¾²Ì¬Â·ÓÉ²ßÂÔ£¬¸ñÊ½Í¬µçÐÅ
 */
char *pdb_10010[] = {
"01bc00000e1b0800000d1b2400000e1b2800000d3a1000000d3af000000c3c0000000b3cce0000103cd000000c3d3000000d3d850000113d86600013",
"3d868000113d870000103d880000123d884000123d898000113d8a0000113d8a8000123d8b8000123d9400000f3d9c0000103d9e0000103d9f000012",
"3da10000123da18000113da20000103da30000103da70000103da80000103db00000103db30000103db48000113db50000103db60000103dbd000011",
"3df000000e6e0600000f6e1000000e6e3400000f6e4800000f704000000f705000000d705800000d706000000f706d800011706f000010707a00000f",
"708400001070c000000e70e000000b710000000d710800000f713800000f713a000010713b00001171c200000f71c800000f71cc00000e71e000000c",
"72f000000c732e000010733000000c7355c00012740200000f745f000010747000000e747400000f750800000d764800000d765000000f769000000e",
"76d4000010770400000e77240000107727000010773000000d773e000010776c00000f777000000d77a200000f77a400000e77b000000c77f800000e",
"780000000c785000000d791000000d791800000e791c00000f791e000010791f0000107a6000000f7a8800000d7a9c00000e7ac000000e7b0400000e",
"7b0800000d7b7000000c7b8000000d7b890000107b8a00000f7b9000000e7b940000107b9800000d7bbc00000e7be800000e7c4000000d7c5800000d",
"7c8000000d7ca000000d7d2000000c7dd200000f8cce00000fca60000012ca60400013ca61800011ca62000013ca63000010ca65800013ca66800012",
"ca66e00013ca6a000010ca6b000011ca6c000010ca6e000011ca6ec00012ca6f800012ca7f700014ca82e00013ca8e100014ca98b00014caa5b00014",
"cb5d000010cba6a00013d20c00000fd20ea00013d20ec00012d20f000011d20f800012d215000010d216000010d233000010d23400000fd24a600013",
"d24a800013d24e000013d25200000fd35a00000fd35c00000ed36000000fd390000014d39d600013d39e000010d3a3000010da07000010da0800000e",
"da0c000010da15800011da1800000eda1c00000fda3800000eda3c00000fda3e000011da43800011da4400000fda6800000eda6c00000fdaf4200013",
"daf4800015db9a00000fdb9c00000fdb9e000010db9f000012dcc000000cdcf800000edcfc000010dd0000000cddc000000bde8000000cdea000000e", /* ¹²17ÐÐ */
};

/*
 * ÒÆ¶¯¾²Ì¬Â·ÓÉ²ßÂÔ£¬¸ñÊ½Í¬µçÐÅ
 */
char *pdb_10086[] = {
"758000000ad367000011d38800000ddac800000ddd8200000fddb000000d", /* ¹²1ÐÐ */
};

#if YAHOO_EN
/*
* ÑÅ»¢Í¨
*/
char *pdb_yahootong[] = {
"^\\x59\\x4d\\x53\\x47.+\\xc0\\x80$",
};
#endif

#if (GAME_BOOST == FYES) 
char *pdb_LOL[] = { 
	"name|LOL1_CF1;protocol|udp;dport|9030;l7filter|^\\x02\\x01....?\\x01$", 
	"name|LOL2;protocol|udp;l7filter|^.\\xff..?\\x82\\xff\\x01", 
	"name|LOL3;protocol|tcp;dport|1025:65535;l7filter|^\\x16\\x03\\x01.+\\x02\\x01$", 
	"", 
	"", 
	"", 
	"", 
	"", 
	"",
	"",
};
char *pdb_DNF[] = {
	"name|DNF1;protocol|udp;dport|7001;l7filter|^\\x02\\x01.+\\x01$",
	"name|DNF2;protocol|tcp;dport|7000:7001;l7filter|^\\x0b\\+\\x01$",
	"name|DNF3;protocol|tcp;dport|10001:65535;l7filter|^\\x01.\\x01.+\\xde\\xd6\\xd6$",
	"name|DNF4_CF4;protocol|tcp;dport|80;l7filter|^\\x01\\*\\x07.+\\x52...$",
	"name|DNF5;protocol|tcp;dport|7101;l7filter|^\\x0c\\xad\\x98\\xdb.+\\x10$",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_CF[] = {
	"name|CF2;protocol|udp;l7filter|^\\x02\\x50\\x06.+\\x03$",
	"name|CF3;protocol|tcp;sport|10008;l7filter|^\\xf1....\\xf2$",
	"name|CF5;protocol|udp;l7filter|^\\xc7\\xd9\\x19\\x99\\x02\\x7d$",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_WOW1[] = {
	"name|WOW1;protocol|tcp;dport|1119;l7filter|^.\\x0a\\xed\\xea\\x57",
	"name|WOW2;protocol|tcp;sport|3724;l7filter|^\\x30\\x57\\x4f\\x52\\x4c\\x44",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_MHXY[] = {
	"name|MHXY1;protocol|tcp;dport|1025:65535;l7filter|^\\x10\\xae\\xf3....\\.\\x37\\x96",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_QQYY[] = {
        "name|QQYUYIN;l7filter|^\\x04.+\\x03$",
        "name|QQYUYIN1;l7filte|^\\x28.+\\x29$",
        "name|QQYUYIN2;l7filte|^\\x5b..\\x01\\x01",
        "name|QQYUYIN3;l7filte|^\\x05...\\x2d[\\x67\\x68]",
};
char *pdb_CCTV[] = {
        "name|CCTV;protocol|udp;sport|8091;l7filter|^....?[\\x35\\x36]\\x31",
};        
char *pdb_SINA[] = {
        "name|SINA;protocol|udp;sport|50001;l7filter|^.[\\xe1\\xdf]\\x0b",
};        
char *pdb_QQZB[] = {
//        "name|QQZB;protocol|tcp;dport|1863;l7filter|(^(GET|get) \\/\\d\\d\\d\\d\\d\\d\\d\\d\\d\\d?\\.flv)|^....",
        "name|QQZB;protocol|tcp;dport|1863;l7filter|^GET .+\\.flv",
};        
char *pdb_JL[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_DOTA2[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_TKSJ[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_LZG[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_YXSG[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_FKJYOL[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_QQXW[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_WMSJ[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_LSCS[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
char *pdb_QQFC[] = {
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};
#endif
#if (WIFIDOG_AUTH == FYES)
/*Î¢ÐÅ²ßÂÔ¸üÐÂ*/
char *pdb_WECHAT[] = {
	/*×î´ó³¤¶ÈÎª128*/
	/* "name|Wechat1;protocol|;dport|;l7filter|(^\\x10\\x10\\x01.+\\x06\\x01$)|(^.\\x10\\x01.+\\x5f\\x43\\x4e$)" */
	"name|Wechat1;l7filter|(^\\x10\\x10\\x01.+\\x06\\x01$)|(^.\\x10\\x01.+\\x5f\\x43\\x4e$)",
	"name|Wechat2;l7filter|(^.\\x10\\x01)|(^GET \/mmhead\/.+gzip\\x0d\\x0a\\x0d\\x0a$)|(^POST http\:\/\/.*.weixin.qq.com)",
	/*ÏÂÃæÒÔDNS¿ªÍ·µÄ²¿·ÖÊÇÎ¢ÐÅÓòÃûÇÒÒÔ·ÖºÅ½áÊø£¬ÓòÃûÖ®¼äµÄ·Ö¸ô·ûÎª·ÖºÅ*/
	"DNS|;",
};
#endif

#if 0
char defaultPdbName1[] = "QQ";
char defaultPdbName2[] = "WLMessenger";
char defaultPdbName3[] = "AliIM";
char defaultPdbName4[] = "WebQQ";
char defaultPdbName5[] = "Fetion";
char defaultPdbName6[] = "BitTorrent";
char defaultPdbName7[] = "Thunder";
char defaultPdbName8[] = "QQLive";
char defaultPdbName9[] = "PPStream";
char defaultPdbName10[] = "KuGou";
char defaultPdbName11[] = "PPTV";
char defaultPdbName12[] = "Qvod";
char defaultPdbName13[] = "BaiduPlayer";
char defaultPdbName14[] = "QQGame";
char defaultPdbName15[] = "bnb";
char defaultPdbName16[] = "zhengtu";
char defaultPdbName17[] = "wanmei";
char defaultPdbName18[] = "xyq";
char defaultPdbName19[] = "au9you";
char defaultPdbName20[] = "HaoFang";
char defaultPdbName21[] = "WOW";
char defaultPdbName22[] = "AION";
char defaultPdbName23[] = "PopKart";
char defaultPdbName24[] = "GAMEURL";
char defaultPdbName25[] = "STOCKURL";
char defaultPdbName26[] = "SHOPURL";
char defaultPdbName27[] = "SOCIALURL";
char defaultPdbName28[] = "HttpProxy";
char defaultPdbName29[] = "Socks";
char defaultPdbName30[] = "FileType";
char defaultPdbName31[] = "Post";
char defaultPdbName32[] = "10000"; /* µçÐÅ¾²Ì¬Â·ÓÉpdbÊµÀýÃû */
char defaultPdbName33[] = "10010"; /* ÁªÍ¨ */
char defaultPdbName34[] = "10086"; /* ÒÆ¶¯ */
#endif
char defaultPdbName1[] = "QQ";
char defaultPdbName2[] = "WLMessenger";
char defaultPdbName3[] = "AliIM";
char defaultPdbName4[] = "WebQQ";
char defaultPdbName5[] = "Fetion";
char defaultPdbName6[] = "BitTorrent";
char defaultPdbName7[] = "Thunder";
char defaultPdbName8[] = "QQLive";
char defaultPdbName9[] = "PPStream";
char defaultPdbName10[] = "KuGou";
char defaultPdbName11[] = "PPTV";
char defaultPdbName12[] = "Qvod";
char defaultPdbName13[] = "BaiduPlayer";
char defaultPdbName14[] = "STOCKURL";
char defaultPdbName15[] = "YouKu";
char defaultPdbName16[] = "TuDou";
char defaultPdbName17[] = "AiQi";
char defaultPdbName18[] = "SouHuVideo";
char defaultPdbName19[] = "KanKan";
char defaultPdbName20[] = "Ku6";
char defaultPdbName21[] = "Net56";
char defaultPdbName22[] = "SinaVideo";
char defaultPdbName23[] = "TencentVideo";
char defaultPdbName24[] = "CNTV";
char defaultPdbName25[] = "iFengVideo";
char defaultPdbName26[] = "BaoFeng";
char defaultPdbName27[] = "Joy";
char defaultPdbName28[] = "PPS";
char defaultPdbName29[] = "FunShion";
char defaultPdbName30[] = "GAMEURL";
char defaultPdbName31[] = "QQGame";
char defaultPdbName32[] = "bnb";
char defaultPdbName33[] = "zhengtu";
char defaultPdbName34[] = "wanmei";
char defaultPdbName35[] = "xyq";
char defaultPdbName36[] = "au9you";
char defaultPdbName37[] = "HaoFang";
char defaultPdbName38[] = "WOW";
char defaultPdbName39[] = "AION";
char defaultPdbName40[] = "PopKart";
char defaultPdbName41[] = "QQCF";
#if 0
char defaultPdbName42[] = "QQDNF";
char defaultPdbName43[] = "QQLOL";
char defaultPdbName44[] = "QQSPEED";
char defaultPdbName45[] = "QQX5";
#endif
char defaultPdbName46[] = "SDODN";
char defaultPdbName47[] = "THE9SXZ";
char defaultPdbName48[] = "SDOMIR";
char defaultPdbName49[] = "TL3";
char defaultPdbName50[] = "CSOL";
#if 0
char defaultPdbName51[] = "QQC9";
#endif
char defaultPdbName52[] = "SDOMXD";
char defaultPdbName53[] = "QN163";
char defaultPdbName54[] = "WD";
char defaultPdbName55[] = "M3GUO";
char defaultPdbName56[] = "WOT";
char defaultPdbName57[] = "TaoBao";
char defaultPdbName58[] = "TMall";
char defaultPdbName59[] = "buy360";
char defaultPdbName60[] = "DangDang";
char defaultPdbName61[] = "Amazon";
char defaultPdbName62[] = "PaiPai";
char defaultPdbName63[] = "Alibaba";
char defaultPdbName64[] = "M18";
char defaultPdbName65[] = "hao123";
char defaultPdbName66[] = "Vancl";
char defaultPdbName67[] = "VipShop";
char defaultPdbName68[] = "Fclub";
char defaultPdbName69[] = "Xiu";
char defaultPdbName70[] = "SSQY";
char defaultPdbName71[] = "MoonBasa";
char defaultPdbName72[] = "YinTai";
char defaultPdbName73[] = "Vjia";
char defaultPdbName74[] = "MasaMaso";
char defaultPdbName75[] = "pb89";
char defaultPdbName76[] = "SuNing";
char defaultPdbName77[] = "Gome";
char defaultPdbName78[] = "buy51";
char defaultPdbName79[] = "Coo8";
char defaultPdbName80[] = "NewEgg";
char defaultPdbName81[] = "LuSen";
char defaultPdbName82[] = "okBuy";
char defaultPdbName83[] = "LeTao";
char defaultPdbName84[] = "YouGou";
char defaultPdbName85[] = "scn";
char defaultPdbName86[] = "mBaoBao";
char defaultPdbName87[] = "Lafaso";
char defaultPdbName88[] = "TianTian";
char defaultPdbName89[] = "Sephora";
char defaultPdbName90[] = "DHC";
char defaultPdbName91[] = "jxdyf";
char defaultPdbName92[] = "straw";
char defaultPdbName93[] = "hiTao";
char defaultPdbName94[] = "guopi";
char defaultPdbName95[] = "caomeipai";
char defaultPdbName96[] = "redBaby";
char defaultPdbName97[] = "muyingzhijia";
char defaultPdbName98[] = "yihaodian";
char defaultPdbName99[] = "womai";
char defaultPdbName100[] = "lashou";
char defaultPdbName101[] = "nuomi";
char defaultPdbName102[] = "meituan";
char defaultPdbName103[] = "gaopeng";
char defaultPdbName104[] = "tuan55";
char defaultPdbName105[] = "QQZone";
char defaultPdbName106[] = "RenRen";
char defaultPdbName107[] = "PengYou";
char defaultPdbName108[] = "weiboSina";
char defaultPdbName109[] = "kaixin001";
char defaultPdbName110[] = "com51";
char defaultPdbName111[] = "douban";
char defaultPdbName112[] = "taomi";
char defaultPdbName113[] = "weiboTencent";
char defaultPdbName114[] = "jiayuan";
char defaultPdbName115[] = "jianghu";
char defaultPdbName116[] = "baisohu";
char defaultPdbName117[] = "zhenai";
char defaultPdbName118[] = "baihe";
char defaultPdbName119[] = "ipart";
char defaultPdbName120[] = "DuoWan";
char defaultPdbName121[] = "BaiduGame";
char defaultPdbName122[] = "Wan37";
char defaultPdbName123[] = "G265";
char defaultPdbName124[] = "eeyy";
char defaultPdbName125[] = "kunlun";
char defaultPdbName126[] = "Game4399";
char defaultPdbName127[] = "Gamepps";
char defaultPdbName128[] = "Game7k7k";
char defaultPdbName129[] = "Wan05";
char defaultPdbName130[] = "MailSina";
char defaultPdbName131[] = "Mail126";
char defaultPdbName132[] = "Mail163";
char defaultPdbName133[] = "MailQQ";
char defaultPdbName134[] = "Mop";
char defaultPdbName135[] = "TianYa";
char defaultPdbName136[] = "LiBa";
char defaultPdbName137[] = "Kds";
char defaultPdbName138[] = "TieXue";
char defaultPdbName139[] = "iFengbbs";
char defaultPdbName140[] = "XiCi";

//char defaultPdbName24[] = "GAMEURL";
//char defaultPdbName25[] = "STOCKURL";
//char defaultPdbName26[] = "SHOPURL";
//char defaultPdbName27[] = "SOCIALURL";
char defaultPdbName141[] = "HttpProxy";
char defaultPdbName142[] = "Socks";
char defaultPdbName143[] = "FileType";
char defaultPdbName144[] = "WebPost";
char defaultPdbName145[] = "10000"; /* µçÐÅ¾²Ì¬Â·ÓÉpdbÊµÀýÃû */
char defaultPdbName146[] = "10010"; /* ÁªÍ¨ */
char defaultPdbName147[] = "10086"; /* ÒÆ¶¯ */
char defaultPdbName148[] = "Advertise"; /* ¹ã¸æ */

#if YAHOO_EN
char defaultPdbName149[] = "yahoo messenger"; /* ÑÅ»¢Í¨ */
char defaultPdbName150[] = "facebook";
char defaultPdbName151[] = "youtube";
char defaultPdbName152[] = "twitter";
#endif

char defaultPdbName153[] = "enterQQ";
#if (GAME_BOOST == FYES)
char defaultPdbName154[] = "LOL";
char defaultPdbName155[] = "DNF";
char defaultPdbName156[] = "CF";
char defaultPdbName157[] = "WOW1";
char defaultPdbName158[] = "MHXY";
char defaultPdbName159[] = "JL";
char defaultPdbName160[] = "DOTA2";
char defaultPdbName161[] = "TKSJ";
char defaultPdbName162[] = "LZG";
char defaultPdbName163[] = "YXSG";
char defaultPdbName164[] = "FKJYOL";
char defaultPdbName165[] = "QQXW";
char defaultPdbName166[] = "WMSJ";
char defaultPdbName167[] = "LSCS";
char defaultPdbName168[] = "QQFC";
char defaultPdbName169[] = "QQYY";
char defaultPdbName170[] = "CCTV";
char defaultPdbName171[] = "SINA";
char defaultPdbName172[] = "QQZB";
#endif
char defaultPdbName173[] = "STOCKSW";
#if (WIFIDOG_AUTH == FYES)
/*Î¢ÐÅ²ßÂÔ¸üÐÂ*/
char defaultPdbName174[] = "WECHAT";
#endif
char defaultPdbName175[] = "Wechat";

extern void pdb_load_content(PdbProfile *prof, int num, char **content)
{
    int i;
    if (!prof || !content) {
	return;
    }

    for (i = 0; i < num; i++) {
	strcpy(prof->line[i], content[i]);
    }
}
#endif

#if (ADFILTER == FYES)
static void adfilter_rule_set(AdfilterProfile *prof, int num, char **content)
{
    int i;
    unsigned char *tmp = NULL, *tmpData = NULL;
    int strLen;

    if (!prof || !content) {
	return;
    }

    for (i = 0; i < num; i++) {
	if (content[i][0] == '\0') {
	    continue;
	}
	tmpData = content[i];
	if ((tmp = strchr(tmpData,',')) == NULL) {
	    continue;
	} else {
	    strLen = tmp  - tmpData;
	    strncpy(prof->originalHost[i], tmpData,strLen);
	}
	tmpData = tmp + 1;
	if ((tmp = strchr(tmpData,',')) == NULL) {
	    continue;
	} else {
	    strLen = tmp  - tmpData;
	    strncpy(prof->originalUri[i], tmpData,strLen);
	}
	tmpData = tmp + 1;
	if ((tmp = strchr(tmpData,',')) == NULL) {
	    continue;
	} else {
	    strLen = tmp  - tmpData;
	    strncpy(prof->host[i], tmpData,strLen);
	}
	tmpData = tmp + 1;
	if ((tmp = strchr(tmpData,',')) == NULL) {
	    continue;
	} else {
	    strLen = tmp  - tmpData;
	    strncpy(prof->uri[i], tmpData,strLen);
	}
	tmpData = tmp + 1;
	if (*tmpData != '\0') {
	    strcpy(prof->normalHostIp[i],tmpData);
	}
    }
}
/*Ã¿Ò»ÐÐ´ú±íÒ»¸öÔª×é£¬±íÊ¾Ò»Ìõ¹ýÂË¹æÔò£¬´Ó×óµ½ÓÒÒÀ´ÎÊÇget°üÖÐµÄHost£¬uri£¬ºÍÒªÌæ»»³ÉµÄhost£¬uri
 *ÆäÖÐhostºÍuri¶¼ÓÐ³¤¶ÈÏÞÖÆ£¬Ìí¼ÓÒ»ÌõÐÂ¹æÔòµÄÊ±ºòÐèÒª×¢ÒâÊÇ·ñ³¬¹ýÁË×ÖÊýÏÞÖÆ
 */
unsigned char *adfilter_tuple[] = {
"valf.atm.youku.com,vf?,116.251.217.202,vf.php,220.181.185.146",/*youku,tudou*/
"api.cupid.qiyi.com,policy2,116.251.217.202,policy2.php,101.227.14.81",/*iqiyi,pps*/
#if 0
"js.tudouui.com,PortalPlayer_105.swf,opengg.ikcd.net,Tudou_PortalPlayer.swf",/*tudou*/
#endif
"v.aty.sohu.com,v?,drop,v,220.181.2.4",/*sohu*/
"ark.letv.com,s?,127.0.0.1,s,220.181.153.170",/*letv*/
"ad.api.3g.youku.com,adv?,127.0.0.1,adv,106.38.248.31",
"m.aty.sohu.com,m?,127.0.0.1,m,101.277.172.24",
"data.vod.itc.cn,?,127.0.0.1,?,220.181.61.213",
#if 0
"g3.letv.cn,vod.v,127.0.0.1,vod.v,220.181.117.237",
"g3.letv.com,vod.v,127.0.0.1,vod.v,220.181.117.237",
"g3.letv.cn,letv-gug,127.0.0.1,letv-gug,220.181.117.237",
"g3.letv.com,letv-gug,127.0.0.1,letv-gug,220.181.117.237",
"data.video.iqiyi.com,videos.other,127.0.0.1,qiyi.ts,58.215.126.147",
#endif
};
#endif
void ProfInstLowHigh(MibProfileType profileType, int *max, int *min)
{

    int instMax = 0, instMin = 0;
    switch ( profileType ) {
        case MIB_PROF_INTERFACE:
            instMax = MAX_INTERFACE_PROFILES;
            instMin = 0;
            break;
#if (IPV6_INTERFACE == FYES)
        case MIB_PROF_IPV6_INTERFACE:
            instMax = MAX_IPV6_INTERFACE_PROFILES;
            instMin = 0; 
            break;
#endif

#if (IPV6_RADVD == FYES)
        case MIB_PROF_RADVD:
            instMax = MAX_IPV6_RADVD_PROFILES;
            instMin = 0; 
            break;
#endif

#if(IPV6_NOTLISTPREFIXS == FYES)
        case MIB_PROF_IPV6_NOTLISTPREFIXS:
            instMax = MAX_IPV6_NOTLISTPREFIXS_PROFILES;
            instMin = 0; 
            break;
#endif
#if(IPV6_ROUTE ==FYES)
    case  MIB_PROF_IPV6_ROUTECONF:
         instMax = MAX_IPV6_ROUTE_PROFILES; 
         instMin = 0; 
        break;
#endif
#if (IPV6_DHCPV6 == FYES)
        case MIB_PROF_DHCP6:
            instMax = MAX_IPV6_DHCPV6_PROFILES;
            instMin = 0; 
            break;
#endif
#if (IPV6_TUNNEL == FYES)
        case MIB_PROF_TUNNEL:
            instMax = 1; 
            instMin = 0; 
            break;
#endif

#if (IPV6_ISATAP == FYES)
        case MIB_PROF_ISATAP:
            instMax = 1;
            instMin = 0;
            break;
#endif
#if (IP_GRP == FYES)
	case MIB_PROF_IPGRP:
            instMax = MAX_IPGRP_PROFILES;
            instMin = 0;
            break;
#endif
#if (PORT_GRP == FYES)
	case MIB_PROF_PORTGRP:
            instMax = MAX_PORTGRP_PROFILES;
            instMin = 0;
            break;
#endif
#if (TIME_GRP == FYES)
	case MIB_PROF_TIMEGRP:
            instMax = MAX_TIMEGRP_PROFILES;
            instMin = 0;
            break;
#endif
#if (DHCP_SERVER == FYES)
        case MIB_PROF_DHCP_SERVER:
            instMax = 1;
            instMin = 0;
            break;
#endif
        case MIB_PROF_USER:
            instMax = MAX_USER_PROFILES;
            instMin = 0;
            break;

#if (DHCP_POOL == FYES)
        case MIB_PROF_DHCP_POOL:
            instMax = MAX_DHCP_POOL_PROFILES;
            instMin = 0;
            break;
#endif
#if (VLAN_CONFIG == FYES)
        case MIB_PROF_VLAN_CONFIG:
            instMax = MAX_VLAN_CONFIG_PROFILES;
            instMin = 0;
            break;
#endif

#if (DNS_AGENCY == FYES)
		case MIB_PROF_DNS_AGENCY:
			instMax = 1;
			instMin = 0;
			break;
#endif

#if (AP_PERMISSION == FYES)
		case MIB_PROF_AP_PERMISSION:
			instMin = 0;
			instMax = MAX_AP_PERMISSION_PROFILES;
			break;
#endif

#if (AP_CONF == FYES)
		case MIB_PROF_AP_CONF:
			instMax = 1;
			instMin = 0;
			break;
#endif

#if (SERVICE_ZONE == FYES)
		case MIB_PROF_SERVICE_ZONE:
			instMin = 0;
			instMax = 10;
			break;
#endif

#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
		case MIB_PROF_AP_MAC_FILTER:
			instMin = 0;
			instMax = MAX_AP_MAC_FILTER_PROFILES;
			break;
#endif

#if (TAG_VLAN == FYES)
		case MIB_PROF_TAG_VLAN:
			instMin = 0;
			instMax = MAX_TAG_VLAN_PROFILES;
			break;
#if (WAN_TAG_VLAN == FYES)
		case MIB_PROF_WAN_TAG_VLAN:
			instMin = 0;
			instMax = MAX_WAN_TAG_VLAN_PROFILES;
			break;
#endif
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
	case MIB_PROF_SW_PORT_CONF:
	    instMax = MAX_SW_PORT_CONF_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_PORT_VLAN == FYES)
	case MIB_PROF_PORT_VLAN:
            instMax = MAX_PORT_VLAN_PROFILES;
            instMin = 0;
	    break;
#endif
#if (PVLAN_REDUCED == FYES)
	case MIB_PROF_RDD_PORT_VLAN:
            instMax = MAX_LAN_PORT_COUNT;
            instMin = 0;
	    break;
#endif
#if (L2TP_SERVER == FYES)
	case MIB_PROF_L2TP_SRV_GLOBAL:
	    instMax = MAX_L2TP_SRV_GLOBAL_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_L2TP_SRV_ACCOUNT:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_L2TP_SRV_ACCOUNT_PROFILES;
#endif
	    instMin = 0;
	    break;
#endif

#if (L2TP_CLIENT == FYES)
	case MIB_PROF_L2TP_CLIENT:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_L2TP_CLIENT_PROFILES;
#endif
	    instMin = 0;
	    break;
#endif

#if (PPTP_SERVER == FYES)
	case MIB_PROF_PPTP_SRV_GLOBAL:
	    instMax = MAX_PPTP_SRV_GLOBAL_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_PPTP_SRV_ACCOUNT:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_PPTP_SRV_ACCOUNT_PROFILES;
#endif
	    instMin = 0;
	    break;
#endif

#if (PPTP_CLIENT == FYES)
	case MIB_PROF_PPTP_CLI:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_PPTP_CLI_PROFILES;
#endif
	    instMin = 0;
	    break;
#endif

#if (STATIC_ROUTE == FYES)
	case MIB_PROF_STATIC_ROUTE:
	    instMax = MAX_STATIC_ROUTE_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (DDNS == FYES)
	case MIB_PROF_DDNS:
	    instMax = MAX_DDNS_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (PPPOE_SERVER == FYES)
	case MIB_PROF_PPPOE_SRV_GLO:
	    instMax = MAX_PPPOE_SRV_GLO_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_PPPOE_SRV_ACC:
	    instMax = MAX_PPPOE_SRV_ACC_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (GROUP_BASE == FYES)
	case MIB_PROF_GROUP_BASE:
	    instMax = MAX_GROUP_BASE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
		case MIB_PROF_MIRROR:
			instMax = MAX_MIRROR_PROFILES;
			instMin = 0;
			break;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
		case MIB_PROF_TRUNK:
			instMax = MAX_TRUNK_PROFILES;
			instMin = 0;
			break;
#endif
#if (FEATURE_SW_SNMP == FYES)
		case MIB_PROF_SNMP:
			instMax = MAX_SNMP_PROFILES;
			instMin = 0;
			break;
#endif
#if (SNMP == FYES)
              case MIB_PROF_SNMP_INFO:
                      instMax = MAX_SNMP_INFO_PROFILES;
                      instMin = 0;
                      break;
#endif
#if (FEATURE_ACL == FYES)
        case MIB_PROF_ACL:
            instMax = MAX_ACL_PROFILES;
            instMin = 0;
            break;
        case MIB_PROF_ACL_STATUS:
            instMax = 1;
            instMin = 0;
            break;
#endif
#if (UTTPDB == FYES)
		case MIB_PROF_PDB_GLOBAL:
			instMax = MAX_PDB_PROFILES;
			instMin = 0;
			break;
#endif
#if (STATIC_NAT == FYES)
		case MIB_PROF_STATIC_NAT:
			instMax = MAX_STATIC_NAT_PROFILES;
			instMin = 0;
			break;
#endif
#if (NAT_RULE == FYES)
		case MIB_PROF_NAT_RULE:
			instMax = MAX_NAT_RULE_PROFILES;
			instMin = 0;
			break;
#endif
#if (DMZ == FYES)
		case MIB_PROF_DMZ_NAT:
			instMax = 1;
			instMin = 0;
			break;
#endif
#if (ADMIN_CONF == FYES)  
		case MIB_PROF_ADMIN_CONF:
			instMax = MAX_ADMIN_CONF_PROFILES;
			instMin = 0;
			break;
#endif
#if (DNS_FILTER == FYES)  
		case MIB_PROF_DNS_FILTER:
			instMax = MAX_DNS_FILTER_PROFILES;
			instMin = 0;
			break;
#endif
#if (CLI_FOR_ENGINEER_ONLY == FYES)
        case MIB_PROF_CLI_FOR_ENGINEER_ONLY:
            instMax = MAX_CLI_FOR_ENGINEER_ONLY_PROFILES;
            instMin = 0;
            break;
#endif
#if (WIRELESS == FYES)  
		case MIB_PROF_WIRELESS:
			instMax = MAX_WIRELESS_PROFILES;
			instMin = 0;
			break;
		case MIB_PROF_WIRELESS_MAC_FILTER:
			instMax = MAX_WIRELESS_MAC_FILTER_PROFILES;
			instMin = 0;
			break;
#if (WIRELESS_5G == FYES)
		case MIB_PROF_WIRELESS_5G_MAC_FILTER:
			instMax = MAX_WIRELESS_5G_MAC_FILTER_PROFILES;
			instMin = 0;
			break;
#endif
#endif
#if (FEATURE_NTP == FYES)  
		case MIB_PROF_NTP:
			instMax = 1;
			instMin = 0;
			break;
#endif
#if (TASK_SCHEDULER == FYES)
		case MIB_PROF_TASK_SCHED:
			instMax = MAX_TASK_SCHED_PROFILES;
			instMin = 0;
			break;
#endif
#if (FIREWALL == FYES)
		case MIB_PROF_FIRE_WALL:
			instMax = MAX_FIREWALL_FILTER_PROFILES;
			instMin = 0;
			break;
#endif
#if (NOTICE == FYES)
		case MIB_PROF_NOTICE:
			instMax = MAX_NOTICE_PROFILES;
			instMin = 0;
			break;
#endif
#if (CWMP == FYES)
        case MIB_PROF_CWMP:
			instMax = 1;
			instMin = 0;
			break;
#endif
#if (WEB_AUTH == FYES)
		case MIB_PROF_WEB_AUTH_GLOBAL:
			instMax = MAX_WEB_AUTH_GLOBAL_PROFILES;
			instMin = 0;
			break;
		case MIB_PROF_WEB_AUTH:
			instMax = MAX_WEB_AUTH_PROFILES;
			instMin = 0;
			break;
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
		case MIB_PROF_WEB_AUTH_CPT:
			instMax = MAX_WEB_AUTH_PROFILES;
			instMin = 0;
			break;
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
		case MIB_PROF_WHITE_LIST:
			instMax = MAX_WHITE_LIST;
			instMin = 0;
			break;
#endif
#endif
#if (EXCEPT_QQ == FYES)
		case MIB_PROF_EXCEPT_QQ:
			instMax = MAX_EXCEPT_QQ_PROFILES;
			instMin=0;
			break;
#endif
#if (EXCEPT_MSN == FYES)
		case MIB_PROF_EXCEPT_MSN:
			instMax = MAX_EXCEPT_MSN_PROFILES;
			instMin = 0;
			break;
#endif
#if (EXCEPT_ALI == FYES)
		case MIB_PROF_EXCEPT_ALI:
			instMax = MAX_EXCEPT_ALI_PROFILES;
			instMin = 0;
			break;
#endif
#if (LOG_MANAGE == FYES)
		case MIB_PROF_LOG_MANAGE:
			instMax = MAX_LOG_MANAGE_PROFILES;
			instMin = 0;
			break;
#endif
#if (AP_LOG_MANAGE == FYES)
		case MIB_PROF_AP_LOG_MANAGE:
			instMax = MAX_AP_LOG_MANAGE_PROFILES;
			instMin = 0;
			break;
#endif
#if (IP_SEC == FYES)
		case MIB_PROF_IPSEC:
#if (VPN_ITEM_CONTROL == FYES)
			instMax = MAX_VPN_ITEMS_NUM;
#else
			instMax = MAX_IPSEC_PROFILES;
#endif
			instMin = 0;
			break;
		case MIB_PROF_TRANSFORMSET:
			instMax = MAX_TRANSFORMSET_PROFILES;
			instMin = 0;
			break;
		case MIB_PROF_ISAKMP:
#if (VPN_ITEM_CONTROL == FYES)
			instMax = MAX_VPN_ITEMS_NUM;
#else
			instMax = MAX_ISAKMP_PROFILES;
#endif
			instMin = 0;
			break;
		case MIB_PROF_ISAKMP_POLICY:
			instMax = MAX_ISAKMP_POLICY_PROFILES;
			instMin = 0;
			break;
#endif
#if (FAT_FIT_SWITCH == FYES)  
		case MIB_PROF_FAT_FIT_SWITCH:
			instMax = MAX_FAT_FIT_SWITCH_PROFILES;
			instMin = 0;
			break;
#endif
#if (AC_WORK_MODE == FYES)  
		case MIB_PROF_AC_WORK_MODE:
			instMax = 1;
			instMin = 0;
			break;
#endif

#if (AP_LOAD_BALANCE == FYES)
        case MIB_PROF_AP_LOAD_BALANCE:
            instMax = MAX_AP_LOAD_BL_PROFILES;
            instMin = 0;
            break;
#endif
#if (FEATURE_AP_MAIL == FYES)  
		case MIB_PROF_AP_MAIL:
			instMax = MAX_AP_MAIL_PROFILES;
			instMin = 0;
			break;
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)  
		case MIB_PROF_AP_CONF_TEMP:
			instMax = MAX_AP_CONF_TEMP_PROFILES;
			instMin = 0;
			break;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
		case MIB_PROF_AP_SW_MNG:
			instMax = MAX_AP_SW_MNG_PROFILES;
			instMin = 0;
			break;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)  
		case MIB_PROF_AP_BASIC_CONF:
			instMax = MAX_AP_BASIC_CONF_PROFILES;
			instMin = 0;
			break;
#endif
#if (CAPWAP_CONFIG_AP == FYES)
		case MIB_PROF_AP_CAPWAP_CONFIG:
			instMax = MAX_AP_CAPWAP_CONFIG;
			instMin = 0;
			break;
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)  
		case MIB_PROF_AP_EFFECT_CONF:
			instMax = MAX_AP_EFFECT_CONF_PROFILES;
			instMin = 0;
			break;
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)  
		case MIB_PROF_FIT_AP_SZ:
			instMax = MAX_FIT_AP_SERVICE_ZONE_PROFILES;
			instMin = 0;
			break;
#if (WIRELESS_5G == FYES)
		case MIB_PROF_FIT_AP_SZ_5G:
			instMax = MAX_FIT_AP_SERVICE_ZONE_PROFILES;
			instMin = 0; 
			break;
#endif
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
		case MIB_PROF_AP_BLACK_WHITE_LIST:
			instMax = MAX_AP_BW_LIST_PROFILES;
			instMin = 0;
			break;
#endif
#if (UTT_LICENSE == FYES)
		case MIB_PROF_LICENSE:
			instMax = MAX_LICENSE_PROFILES;
			instMin = 0;
			break;
#endif
#if (POLICY_ROUTE == FYES)
		case MIB_PROF_POLICY_ROUTE:
			instMax = MAX_POLICY_ROUTE_PROFILES;
			instMin = 0;
			break;
#endif
#if (PARENTS_CONTROL == FYES)
		case MIB_PROF_PARENTS_CONTROL:
			instMax = MAX_PARENTS_CONTROL_PROFILES;
			instMin = 0;
			break;
#endif
#if (HTTP_CACHE == FYES)
		case MIB_PROF_CACHE:
			instMax = MAX_CACHE_PROFILES;
			instMin = 0; 
			break;
#endif
#if (NET_MODE_SWITCH == FYES)
		case MIB_PROF_NET_MODE_SWITCH:
			instMax = MAX_NET_MODE_PROFILES;
			instMin = 0; 
			break;
#endif
#if (BRIDGE == FYES)
	case MIB_PROF_BRIDGE:
		instMax = MAX_BRIDGE_PROFILES;
		instMin = 0; 
		break;
#endif
#if (MAC_FILTER == FYES)
		case MIB_PROF_MAC_FILTER_GLOBAL:
			instMax = MAX_MAC_FILTER_GLOBAL_PROFILES;
			instMin = 0;
			break;
		case MIB_PROF_MAC_FILTER:
			instMax = MAX_MAC_FILTER_PROFILES;
			instMin = 0;
			break;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
		case MIB_PROF_SHORTCUT_MENU:
			instMax = MAX_SHORTCUT_MENU_PROFILES;
			instMin = 0;
			break;
#endif
#endif
#if (NET_SHARE_MANAGE == FYES)
		case MIB_PROF_NET_SHARE_MANAGE:
			instMax = 1;
			instMin = 0;
			break;
#endif
#if (FTP_SERVER == FYES)
        case MIB_PROF_FTP_SERVER:
			instMax = 1;
			instMin = 0;
			break;
        case MIB_PROF_FTP_SHARE_DIR:
			instMax = MAX_FTP_SHARE_DIR;
			instMin = 0;
			break;
#endif
#if (NET_SHARE_USER == FYES)
        case MIB_PROF_NET_SHARE_USER:
			instMax = MAX_NET_SHARE_USER;
			instMin = 0;
			break;
#endif
#if (ADFILTER == FYES)
        case MIB_PROF_ADFILTER:
			instMax = MAX_ADFILTER_PROFILES;
			instMin = 0;
			break;
#endif
#if (MAKE_MONEY == FYES)
        case MIB_PROF_MAKE_MONEY:
                        instMax = 1; 
                        instMin = 0; 
                        break;
#endif
#if (CLOUD_MANAGE == FYES)
        case MIB_PROF_CLOUD_MANAGE:
                        instMax = 1; 
                        instMin = 0; 
                        break;
#endif
#if (CLOUD_CONFIG == FYES)
        case MIB_PROF_CLOUD_CONFIG:
                        instMax = 1; 
                        instMin = 0; 
                        break;
#endif
#if (UTT_KNOWIFI == FYES)
	case MIB_PROF_KNOWIFI_DOMAIN:
			instMax = MAX_KNOWIFI_DOMAIN_COUNT;
			instMin = 0;
			break;
#endif
        default:
            instMax = 1;
            instMin = 0;
            break;
    }
    if(max != NULL) {
        *max = instMax;
    }

    if(min != NULL) {
        *min = instMin;
    }
    return;
}

Pointer _getProfilePointer(SlotNumber		slot,
		   MibProfileType	profileType,
		   Word			index,
                   Uint			*length,
		   Uint			*maxInst,
		   Uint			*minInst)
{
    Pointer dest = NULL;
    Uint size = 0;

    switch ( profileType ) {
        case MIB_PROF_SYSTEM:
            dest = &(nvramProfile->systemProfile);
            size = sizeof(SystemProfile);
            break;
        case MIB_PROF_INTERFACE:
            dest = &(nvramProfile->interfaceProfile[index]);
            size = sizeof(InterfaceProfile);
            break;
#if (IPV6_INTERFACE == FYES)
   case MIB_PROF_IPV6_INTERFACE:
            dest = &(nvramProfile->ipv6interfaceProfile[index]);
            size = sizeof(Ipv6InterfaceProfile);
            break;
#endif
#if (IPV6_RADVD == FYES)
   case MIB_PROF_RADVD:
            dest = &(nvramProfile->radvdProfile);
            size = sizeof(RadvdProfile);
            break;
#endif
#if(IPV6_NOTLISTPREFIXS == FYES)
    case MIB_PROF_IPV6_NOTLISTPREFIXS:
            dest = &(nvramProfile->ipv6PrefixListProfile[index]);
            size = sizeof(Ipv6PrefixListProfile);
            break;
#endif
#if (IPV6_ROUTE == FYES)
    case MIB_PROF_IPV6_ROUTECONF:
            dest = &(nvramProfile->ipv6RoutConfProfile[index]);
            size= sizeof(Ipv6RoutConfProfile);
            break;
#endif
#if (IPV6_DHCPV6 == FYES)
   case MIB_PROF_DHCP6:
            dest = &(nvramProfile->dhcp6Profile);
            size = sizeof(Dhcp6Profile);
            break;
#endif
#if (IPV6_TUNNEL == FYES)
   case MIB_PROF_TUNNEL:
            dest = &(nvramProfile->tunnelProfile);
            size = sizeof(TunnelProfile);
            break;
#endif
#if (IPV6_ISATAP == FYES)
   case MIB_PROF_ISATAP:
            dest = &(nvramProfile->isatapProfile);
            size = sizeof(IsatapProfile);
            break;
#endif

#if (IP_GRP == FYES)
       case MIB_PROF_IPGRP:
            dest = &(nvramProfile->ipGrpProfile[index]);
            size = sizeof(IpGrpProfile);
            break;
#endif 
#if (PORT_GRP == FYES)
       case MIB_PROF_PORTGRP:
            dest = &(nvramProfile->portGrpProfile[index]);
            size = sizeof(PortGrpProfile);
            break;
#endif 
#if (TIME_GRP == FYES)
       case MIB_PROF_TIMEGRP:
            dest = &(nvramProfile->timeGrpProfile[index]);
            size = sizeof(TimeGrpProfile);
            break;
#endif
#if (DHCP_SERVER == FYES)  
        case MIB_PROF_DHCP_SERVER:
            dest = &(nvramProfile->dhcpServerProfile);
            size = sizeof(DhcpServerProfile);
            break;
#endif
#if (DHCP_POOL == FYES)
        case MIB_PROF_DHCP_POOL:
            dest = &(nvramProfile->dhcpPoolProfile[index]);
            size = sizeof(DhcpPoolProfile);
            break;
#endif
#if (VLAN_CONFIG == FYES)
        case MIB_PROF_VLAN_CONFIG:
            dest = &(nvramProfile->vlanConfigProfile[index]);
            size = sizeof(VlanConfigProfile);
            break;
#endif
        case MIB_PROF_USER:
            dest = &(nvramProfile->userProfile[index]);
            size = sizeof(UserProfile);
            break;

#if (DNS_AGENCY == FYES)
		case MIB_PROF_DNS_AGENCY:
			dest = &(nvramProfile->dnsAgencyProfile);
			size = sizeof(DnsAgencyProfile);
			break;
#endif
#if (FEATURE_SW_PORT_CONF == FYES)
	case MIB_PROF_SW_PORT_CONF:
	    dest = &(nvramProfile->swPortConfProfile[index]);
            size = sizeof(SwPortConfProfile);
	    break;
#endif
#if (FEATURE_PORT_VLAN == FYES)
	case MIB_PROF_PORT_VLAN:
            dest = &(nvramProfile->portVlanProfile[index]);
            size = sizeof(PortVlanProfile);
	    break;
#endif

#if (PVLAN_REDUCED == FYES)
	case MIB_PROF_RDD_PORT_VLAN:
            dest = &(nvramProfile->rddPortVlanProfile[index]);
            size = sizeof(RddPortVlanProfile);
	    break;
#endif

#if (AP_PERMISSION == FYES)
	case MIB_PROF_AP_PERMISSION:
		dest = &(nvramProfile->apPermissionProfile[index]);
		size = sizeof(ApPermissionProfile);
		break;
#endif

#if (AP_CONF == FYES)
	case MIB_PROF_AP_CONF:
		dest = &(nvramProfile->apConfProfile);
		size = sizeof(ApConfProfile);
		break;
#endif

#if (SERVICE_ZONE == FYES)
	case MIB_PROF_SERVICE_ZONE:
		dest = &(nvramProfile->serviceZoneProfile[index]);
		size = sizeof(ServiceZoneProfile);
		break;
#endif

#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	case MIB_PROF_AP_MAC_FILTER:
		dest = &(nvramProfile->apMacFilterProfile[index]);
		size = sizeof(ApMacFilterProfile);
		break;
#endif

#if (TAG_VLAN == FYES)
	case MIB_PROF_TAG_VLAN:
		dest = &(nvramProfile->tagVlanProfile[index]);
		size = sizeof(TagVlanProfile);
		break;
#if (WAN_TAG_VLAN == FYES)
	case MIB_PROF_WAN_TAG_VLAN:
		dest = &(nvramProfile->wanTagVlanProfile);
		size = sizeof(WanTagVlanProfile);
		break;
#endif
#endif

#if (L2TP_SERVER == FYES)
	case MIB_PROF_L2TP_SRV_GLOBAL:
	    dest = &(nvramProfile->l2tpSrvGlobalProfile);
	    size = sizeof(L2TPSrvGlobalProfile);
	    break;
	case MIB_PROF_L2TP_SRV_ACCOUNT:
	    dest = &(nvramProfile->l2tpSrvAccProfile[index]);
	    size = sizeof(L2TPSrvAccProfile);
	    break;
#endif

#if (L2TP_CLIENT == FYES)
	case MIB_PROF_L2TP_CLIENT:
	    dest = &(nvramProfile->l2tpCliProfile[index]);
	    size = sizeof(L2TPCliProfile);
	    break;
#endif

#if (PPTP_SERVER == FYES)
	case MIB_PROF_PPTP_SRV_GLOBAL:
	    dest = &(nvramProfile->pptpSrvGlobalProfile);
	    size = sizeof(PPTPSrvGlobalProfile);
	    break;
	case MIB_PROF_PPTP_SRV_ACCOUNT:
	    dest = &(nvramProfile->pptpSrvAccProfile[index]);
	    size = sizeof(PPTPSrvAccProfile);
	    break;
#endif

#if (PPTP_CLIENT == FYES)
	case MIB_PROF_PPTP_CLI:
	    dest = &(nvramProfile->pptpCliProfile[index]);
	    size = sizeof(PPTPCliProfile);
	    break;
#endif

#if (STATIC_ROUTE == FYES)
	case MIB_PROF_STATIC_ROUTE:
	    dest = &(nvramProfile->staticRouteProfile[index]);
	    size = sizeof(StaticRouteProfile);
	    break;
#endif

#if (DDNS == FYES)
	case MIB_PROF_DDNS:
#if (DDNS_ADVANCE == FYES)
	    dest = &(nvramProfile->ddnsProfile[index]);
#else
	    dest = &(nvramProfile->ddnsProfile);
#endif
	    size = sizeof(DDNSProfile);
	    break;
#endif

#if (PPPOE_SERVER == FYES)
	case MIB_PROF_PPPOE_SRV_GLO:
	    dest = &(nvramProfile->pppoeSrvGloProfile);
	    size = sizeof(PPPoESrvGloProfile);
	    break;
	case MIB_PROF_PPPOE_SRV_ACC:
	    dest = &(nvramProfile->pppoeSrvAccProfile[index]);
	    size = sizeof(PPPoESrvAccProfile);
	    break;
#endif

#if (GROUP_BASE == FYES)
	case MIB_PROF_GROUP_BASE:
            dest = &(nvramProfile->groupBaseProfile[index]);
            size = sizeof(GroupBaseProfile);
	    break;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
		case MIB_PROF_MIRROR:
			dest = &(nvramProfile->mirrorProfile);
			size = sizeof(MirrorProfile);
			break;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
		case MIB_PROF_TRUNK:
			dest = &(nvramProfile->trunkProfile[index]);
			size = sizeof(TrunkProfile);
			break;
#endif
#if (FEATURE_SW_SNMP == FYES)
		case MIB_PROF_SNMP:
			dest = &(nvramProfile->snmpProfile);
			size = sizeof(SnmpProfile);
			break;
#endif
#if (SNMP == FYES)
              case MIB_PROF_SNMP_INFO:
                      dest = &(nvramProfile->snmpInfoProfile);
                      size = sizeof(SnmpInfoProfile);
                      break;
#endif
#if (FEATURE_ACL == FYES)
        case MIB_PROF_ACL:
            dest = &(nvramProfile->aclProfile[index]);
            size = sizeof(AclProfile);
            break;
        case MIB_PROF_ACL_STATUS:
            dest = &(nvramProfile->aclStatusProfile);
            size = sizeof(AclStatusProfile);
            break;
#endif

#if (UTTPDB == FYES)
		case MIB_PROF_PDB_GLOBAL:
			dest = &(nvramProfile->pdbProfile[index]);
			size = sizeof(PdbProfile);
			break;
#endif
#if (STATIC_NAT == FYES)
		case MIB_PROF_STATIC_NAT:
			dest = &(nvramProfile->staticNatProfile[index]);
			size = sizeof(StaticNatProfile);
			break;
#endif
#if (NAT_RULE == FYES)
		case MIB_PROF_NAT_RULE:
			dest = &(nvramProfile->natRuleProfile[index]);
			size = sizeof(NatRuleProfile);
			break;
#endif
#if (DMZ  == FYES)
		case MIB_PROF_DMZ_NAT:
			dest = &(nvramProfile->dmzProfile);
			size = sizeof(DMZProfile);
			break;
#endif
#if (ADMIN_CONF == FYES)  
		case MIB_PROF_ADMIN_CONF:
			dest = &(nvramProfile->adminConfProfile[index]);
			size = sizeof(AdminConfProfile);
			break;
#endif
#if (DNS_FILTER == FYES)  
		case MIB_PROF_DNS_FILTER:
			dest = &(nvramProfile->dnsFilterProfile[index]);
			size = sizeof(DnsFilterProfile);
			break;
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
        case MIB_PROF_CLI_FOR_ENGINEER_ONLY:
            dest = &(nvramProfile->cliForEngineerOnlyProfile[index]);
            size = sizeof(CliForEngineerOnlyProfile);
            break;
#endif

#if (WIRELESS == FYES)
	case MIB_PROF_WIRELESS:
            dest = &(nvramProfile->wirelessProfile[index]);
            size = sizeof(WirelessProfile);
            break;

	case MIB_PROF_WIRELESS_MAC_FILTER:
	    dest = &(nvramProfile->wirelessMacFilterProfile[index]);
	    size = sizeof(WirelessMacFilterProfile);
	    break;
#if (WIRELESS_5G == FYES)
	case MIB_PROF_WIRELESS_5G_MAC_FILTER:
	    dest = &(nvramProfile->wireless5gMacFilterProfile[index]);
	    size = sizeof(Wireless5gMacFilterProfile);
	    break;
#endif
	case MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL:
	    dest = &(nvramProfile->wirelessProfile[index].mFltGlb);
	    size = sizeof(WirelessMacFilterGlobalProfile);
	    break;
#endif
#if (FEATURE_NTP == FYES)  
		case MIB_PROF_NTP:
			dest = &(nvramProfile->ntpProfile);
			size = sizeof(NtpProfile);
		    break;
#endif
#if (FEATURE_LANG == FYES)  
        case MIB_PROF_LANG:
	    dest = &(nvramProfile->langProfile);
            size = sizeof(LangProfile);
	    break;
#endif
#if (TASK_SCHEDULER == FYES)
		case MIB_PROF_TASK_SCHED:
			dest = &(nvramProfile->taskSchedProfile[index]);
			size = sizeof(TaskSchedProfile);
			break;
#endif
 #if (FIREWALL == FYES)
		case MIB_PROF_FIRE_WALL:
			dest = &(nvramProfile->fireWallProfile[index]);
			size = sizeof(FireWallProfile);
			break;
#endif
#if (NOTICE == FYES)
		case MIB_PROF_NOTICE:
			dest = &(nvramProfile->noticeProfile[index]);
			size = sizeof(NoticeProfile);
			break;
#endif
#if (CWMP == FYES)
        case MIB_PROF_CWMP:
			dest = &(nvramProfile->cwmpProfile);
			size = sizeof(CwmpProfile);
			break;
#endif
#if (WEB_AUTH == FYES)
		case MIB_PROF_WEB_AUTH_GLOBAL:
			dest = &(nvramProfile->webAuthGlobalProfile[index]);
			size = sizeof(WebAuthGlobalProfile);
			break;
		case MIB_PROF_WEB_AUTH:
			dest = &(nvramProfile->webAuthProfile[index]);
			size = sizeof(WebAuthProfile);
			break;
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
		case MIB_PROF_WEB_AUTH_CPT:
			dest = &(nvramProfile->webAuthProfile_auto[index]);
			size = sizeof(WebAuthProfile);
			break;
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
		case MIB_PROF_WHITE_LIST:
			dest = &(nvramProfile->whiteListProfile[index]);
			size = sizeof(WhiteListProfile);
			break;
#endif
#endif
#if (EXCEPT_QQ == FYES)
		case MIB_PROF_EXCEPT_QQ:
			dest = &(nvramProfile->exceptQQProfile[index]);
			size = sizeof(ExceptQQProfile);
			break;
#endif
#if (EXCEPT_MSN == FYES)
		case MIB_PROF_EXCEPT_MSN:
			dest = &(nvramProfile->exceptMSNProfile[index]);
			size = sizeof(ExceptMSNProfile);
			break;
#endif
#if (EXCEPT_ALI == FYES)
		case MIB_PROF_EXCEPT_ALI:
			dest = &(nvramProfile->exceptAliProfile[index]);
			size = sizeof(ExceptAliProfile);
			break;
#endif
#if (LOG_MANAGE == FYES)
		case MIB_PROF_LOG_MANAGE:
			dest = &(nvramProfile->logManageProfile[index]);
			size = sizeof(LogManageProfile);
			break;
#endif
#if (AP_LOG_MANAGE == FYES)
		case MIB_PROF_AP_LOG_MANAGE:
			dest = &(nvramProfile->apLogManageProfile[index]);
			size = sizeof(ApLogManageProfile);
			break;
#endif
#if (IP_SEC == FYES)
		case MIB_PROF_IPSEC:
			dest = &(nvramProfile->ipsecconfProfile[index]);
			size = sizeof(ipsecConfProfile);
			break;
		case MIB_PROF_TRANSFORMSET:
			dest = &(nvramProfile->ipsectransformsetProfile[index]);
			size = sizeof(ipsecTransformSetProfile);
			break;
		case MIB_PROF_ISAKMP:
			dest = &(nvramProfile->isakmpconfProfile[index]);
			size = sizeof(isakmpConfProfile);
			break;
		case MIB_PROF_ISAKMP_POLICY:
			dest = &(nvramProfile->isakmppolicyProfile[index]);
			size = sizeof(isakmpPolicyProfile);
			break;
#endif
#if (SYSLOG == FYES)
		case MIB_PROF_SYSLOG:
			dest = &(nvramProfile->syslogProfile);
			size = sizeof(SyslogProfile);
			break;
#endif

#if (FAT_FIT_SWITCH == FYES)
	case MIB_PROF_FAT_FIT_SWITCH:
	    dest = &(nvramProfile->fatFitSwitchProfile);
	    size = sizeof(FatFitSwitchProfile);
	    break;
#endif
#if (AC_WORK_MODE == FYES)
	case MIB_PROF_AC_WORK_MODE:
	    dest = &(nvramProfile->acWorkModeProfile);
	    size = sizeof(AcWorkModeProfile);
	    break;
#endif

#if (AP_LOAD_BALANCE == FYES)
    case MIB_PROF_AP_LOAD_BALANCE:
	    dest = &(nvramProfile->apLoadBalanceProfile[index]);
	    size = sizeof(ApLoadBalanceProfile);
	    break;
#endif
#if (FEATURE_AP_MAIL == FYES)
	case MIB_PROF_AP_MAIL:
	    dest = &(nvramProfile->apMailProfile[index]);
	    size = sizeof(ApMailProfile);
	    break;
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
	case MIB_PROF_AP_CONF_TEMP:
	    dest = &(nvramProfile->apConfTempProfile[index]);
	    size = sizeof(ApConfTempProfile);
	    break;
#endif
#if (FEATURE_AP_SCAN == FYES)
	case MIB_PROF_AP_SCAN:
	    dest = &(nvramProfile->apScanProfile);
	    size = sizeof(ApScanProfile);
	    break;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
	case MIB_PROF_AP_SW_MNG:
	    dest = &(nvramProfile->apSwMngProfile[index]);
	    size = sizeof(ApSwMngProfile);
	    break;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
	case MIB_PROF_AP_BASIC_CONF:
	    dest = &(nvramProfile->apBasicConfProfile[index]);
	    size = sizeof(ApBasicConfProfile);
	    break;
#endif

#if (CAPWAP_CONFIG_AP == FYES)
	case MIB_PROF_AP_CAPWAP_CONFIG:
	    dest = &(nvramProfile->apCapwapConfigProfile[index]);
	    size = sizeof(ApCapwapConfigProfile);
	    break;
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
	case MIB_PROF_AP_EFFECT_CONF:
	    dest = &(nvramProfile->apEffectConfProfile);
	    size = sizeof(ApEffectConfProfile);
	    break;
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
	case MIB_PROF_FIT_AP_SZ:
	    dest = &(nvramProfile->fitApSzProfile[index]);
	    size = sizeof(FitApSzProfile);
	    break;
#if (WIRELESS_5G == FYES)
	case MIB_PROF_FIT_AP_SZ_5G:
	    dest = &(nvramProfile->fitAp5gSzProfile[index]);
	    size = sizeof(FitAp5gSzProfile);
	    break;
#endif
#endif

#if (AP_BLACK_WHITE_LIST == FYES)
        case MIB_PROF_AP_BLACK_WHITE_LIST:
	    dest = &(nvramProfile->apBlackWhiteListProfile[index]);
	    size = sizeof(ApBlackWhiteListProfile);
	    break;
#endif

#if (UTT_LICENSE == FYES)
		case MIB_PROF_LICENSE:
			dest = &(nvramProfile->licenseProfile[index]);
			size = sizeof(LicenseProfile);
			break;
#endif
#if (POLICY_ROUTE == FYES)
		case MIB_PROF_POLICY_ROUTE:
			dest = &(nvramProfile->policyrouteProfile[index]);
			size = sizeof(PolicyRouteProfile);
			break;
#endif
#if (PARENTS_CONTROL == FYES)
		case MIB_PROF_PARENTS_CONTROL:
			dest = &(nvramProfile->parentscontrolProfile[index]);
			size = sizeof(ParentsControlProfile);
			break;
#endif
#if (P2P_LIMIT == FYES)
	case MIB_PROF_P2P_LIMIT:
	    dest = &(nvramProfile->p2pLimitProfile);
	    size = sizeof(P2PLimitProfile);
	    break;
#endif
#if (MAC_FILTER == FYES)
	case MIB_PROF_MAC_FILTER_GLOBAL:
		dest = &(nvramProfile->macFilterGlobalProfile);
                size = sizeof(MacFilterGlobalProfile);
                break;
        case MIB_PROF_MAC_FILTER:
                dest = &(nvramProfile->macFilterProfile[index]);
                size = sizeof(MacFilterProfile);
                break;
#endif
#if (SYS_LOG_INFO == FYES)
	case MIB_PROF_SYS_LOG_INFO:
	    dest = &(nvramProfile->sysLogInfoProfile);
	    size = sizeof(SysLogInfoProfile);
	    break;
#endif
#if (EASY_TASK == FYES)
	case MIB_PROF_EASY_TASK:
	    dest = &(nvramProfile->easyTaskProfile);
	    size = sizeof(EasyTaskProfile);
	    break;
#endif
#if (HTTP_CACHE == FYES)
		case MIB_PROF_CACHE:
			dest = &(nvramProfile->cacheProfile[index]);
			size = sizeof(CacheProfile);
			break;
#endif
#if (NET_MODE_SWITCH == FYES)
		case MIB_PROF_NET_MODE_SWITCH:
			dest = &(nvramProfile->netModeSwitchProfile[index]);
			size = sizeof(NetModeSwitchProfile);
			break;
#endif
#if (BRIDGE == FYES)
		case MIB_PROF_BRIDGE:
			dest = &(nvramProfile->bridgeProfile[index]);
			size = sizeof(BridgeProfile);
			break;
#endif
#if (CACHE_SERVER == FYES)
		case MIB_PROF_CACHE_SERVER:
			dest = &(nvramProfile->cacheServerProfile[index]);
			size = sizeof(CacheServerProfile);
			break;
#endif
#if (NET_SNIPER == FYES)
		case MIB_PROF_NET_SNIPER:
			dest = &(nvramProfile->netSniperProfile);
			size = sizeof(NetSniperProfile);
			break;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
	case MIB_PROF_SHORTCUT_MENU:
		dest = &(nvramProfile->shortCutMenuProfile[index]);
                size = sizeof(ShortCutMenuProfile);
                break;
#endif
#endif
#if (NET_SHARE_MANAGE == FYES)
		case MIB_PROF_NET_SHARE_MANAGE:
			dest = &(nvramProfile->netShareManageProfile);
			size = sizeof(NetShareManageProfile);
			break;
#endif
#if (FTP_SERVER == FYES)
		case MIB_PROF_FTP_SERVER:
			dest = &(nvramProfile->ftpServerProfile);
			size = sizeof(FtpServerProfile);
			break;
		case MIB_PROF_FTP_SHARE_DIR:
			dest = &(nvramProfile->ftpShareDirProfile[index]);
			size = sizeof(FtpShareDirProfile);
			break;
#endif
#if (NET_SHARE_USER == FYES)
		case MIB_PROF_NET_SHARE_USER:
			dest = &(nvramProfile->netShareUserProfile[index]);
			size = sizeof(NetShareUserProfile);
			break;
#endif
#if (ADFILTER == FYES)
		case MIB_PROF_ADFILTER:
			dest = &(nvramProfile->adfilterProfile);
			size = sizeof(AdfilterProfile);
			break;
#endif
#if (MAKE_MONEY == FYES)
                case MIB_PROF_MAKE_MONEY:
                        dest = &(nvramProfile->makeMoneyProfile);
                        size = sizeof(MakeMoneyProfile);
                        break;
#endif
#if (CLOUD_MANAGE == FYES)
        case MIB_PROF_CLOUD_MANAGE:
                        dest = &(nvramProfile->cloudManageProfile);
                        size = sizeof(CloudManageProfile);
                        break;
#endif
#if (CLOUD_CONFIG == FYES)
        case MIB_PROF_CLOUD_CONFIG:
                        dest = &(nvramProfile->cloudConfigProfile);
                        size = sizeof(CloudConfigProfile);
                        break;
#endif
#if (UTT_KNOWIFI == FYES)
	case MIB_PROF_KNOWIFI_DOMAIN:
			dest = &(nvramProfile->knowifiDomainProfile[index]);
			size = sizeof(KnowifiDomainProfile);
			break;
#endif

        default:
            break;
    }

    if(length != NULL) {
        *length = size;
    }
    ProfInstLowHigh(profileType, maxInst, minInst);
    return dest;
}

#if 0
#define UTT_OUI_0 0x00
#define UTT_OUI_1 0x22
#define UTT_OUI_2 0xaa
extern Byte* getFactoryEtherAddress(SlotNumber slot)
{
    static Byte EtherAddress[ETHER_ADDR_LEN];

    LongWord	serialNumber = 0, macAddrBase;

    serialNumber = getSystemVersionHandle()->serialNumber;

    macAddrBase = serialNumber + ethSlotIndex(slot)*0xa10;

    EtherAddress[ 0 ] = UTT_OUI_0;
    EtherAddress[ 1 ] = UTT_OUI_1;
    EtherAddress[ 2 ] = UTT_OUI_2;
    
    EtherAddress[ 3 ] = (Byte)(( macAddrBase & 0x00ff0000 ) >> 16);
    EtherAddress[ 4 ] = (Byte)(( macAddrBase & 0x0000ff00 ) >> 8);
    EtherAddress[ 5 ] = (Byte)(( macAddrBase & 0x000000ff ));
    return EtherAddress;
}
#endif

SystemProfile* buildFactorySystemProfile(ProfileUnion *unionProf)
{
    SystemProfile* systemProfile = &(unionProf->systemProfile);
    memset(systemProfile, 0, (size_t)sizeof(SystemProfile));
    return systemProfile;
}

InterfaceProfile* buildFactoryInterfaceProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->interfaceProfile)), 0, (size_t)sizeof(InterfaceProfile));
   return &(unionProf->interfaceProfile);
}

#if (CLOUD_MANAGE == FYES)
CloudManageProfile* buildFactoryCloudManageProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->cloudManageProfile)), 0, sizeof(CloudManageProfile));
    return &(unionProf->cloudManageProfile);
}
#endif
#if (CLOUD_CONFIG == FYES)
CloudConfigProfile* buildFactoryCloudConfigProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->cloudConfigProfile)), 0, sizeof(CloudConfigProfile));
    return &(unionProf->cloudConfigProfile);
}
#endif

#if (IPV6_INTERFACE == FYES)
Ipv6InterfaceProfile* buildFactoryIpv6InterfaceProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->ipv6interfaceProfile)), 0, sizeof(Ipv6InterfaceProfile));
    return &(unionProf->ipv6interfaceProfile);
}
#endif

#if (IPV6_RADVD == FYES)
RadvdProfile* buildFactoryRadvdProfile(ProfileUnion *unionProf)
{
    RadvdProfile *radvdProfile = &(unionProf->radvdProfile);
    memset((void*)(&(unionProf->radvdProfile)), 0, sizeof(RadvdProfile));
    return &(unionProf->radvdProfile);
}
#endif

#if (IPV6_NOTLISTPREFIXS == FYES)
Ipv6PrefixListProfile* buildFactoryIpv6PrefixListProfile(ProfileUnion *unionProf){

        Ipv6PrefixListProfile *ipv6PrefixListProfile = &(unionProf->ipv6PrefixListProfile);
        memset((void*)(&(unionProf->ipv6PrefixListProfile)), 0, sizeof(Ipv6PrefixListProfile));
        return &(unionProf->ipv6PrefixListProfile);
}
#endif
#if(IPV6_ROUTE == FYES)
Ipv6RoutConfProfile * buildFactoryIpv6RoutConfProfile(ProfileUnion *unionProf)
{
    Ipv6RoutConfProfile  *ipv6RoutConfProfile=&(unionProf->ipv6RoutConfProfile);
     memset((void*)(&(unionProf->ipv6RoutConfProfile)), 0, sizeof(Ipv6RoutConfProfile));
    return &(unionProf->ipv6RoutConfProfile);
}
#endif
#if (IPV6_DHCPV6 == FYES)
Dhcp6Profile* buildFactoryDhcp6Profile(ProfileUnion *unionProf)
{
    Dhcp6Profile *dhcp6Profile = &(unionProf->dhcp6Profile);
    memset((void*)(&(unionProf->dhcp6Profile)), 0, sizeof(Dhcp6Profile));
    dhcp6Profile->dhcpPrefixLength = 64;
    dhcp6Profile->staleTime = 3600;
    return &(unionProf->dhcp6Profile);
}
#endif
#if (IPV6_TUNNEL == FYES)
TunnelProfile* buildFactoryTunnelProfile(ProfileUnion *unionProf)
{
    TunnelProfile *tunnelProfile = &(unionProf->tunnelProfile);
    memset((void*)(&(unionProf->tunnelProfile)), 0, sizeof(TunnelProfile));
    return &(unionProf->tunnelProfile);
}
#endif
#if (IPV6_ISATAP == FYES)
IsatapProfile* buildFactoryIsatapProfile(ProfileUnion *unionProf)
{
    IsatapProfile *isatapProfile = &(unionProf->isatapProfile);
    memset((void*)(&(unionProf->isatapProfile)), 0, sizeof(IsatapProfile));
    return &(unionProf->isatapProfile);
}
#endif

#if (IP_GRP == FYES)
IpGrpProfile* buildFactoryIpGrpProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->ipGrpProfile)), 0, (size_t)sizeof(IpGrpProfile));
   return &(unionProf->ipGrpProfile);
}
			 
#endif

#if (PORT_GRP == FYES)
PortGrpProfile* buildFactoryPortGrpProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->portGrpProfile)), 0, (size_t)sizeof(PortGrpProfile));
   return &(unionProf->portGrpProfile);
}
			 
#endif

#if (TIME_GRP == FYES)
TimeGrpProfile* buildFactoryTimeGrpProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->timeGrpProfile)), 0, (size_t)sizeof(TimeGrpProfile));
   return &(unionProf->timeGrpProfile);
}
#endif

UserProfile* buildFactoryUserProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->userProfile)), 0, sizeof(UserProfile));
   return &(unionProf->userProfile);
}

#if (DHCP_SERVER == FYES)
DhcpServerProfile* buildFactoryDhcpServerProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->dhcpServerProfile)), 0, sizeof(DhcpServerProfile));
   return &(unionProf->dhcpServerProfile);
}
#endif

#if (DHCP_POOL == FYES)
DhcpPoolProfile* buildFactoryDhcpPoolProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->dhcpPoolProfile)), 0, sizeof(DhcpPoolProfile));
   return &(unionProf->dhcpPoolProfile);
}
#endif

#if (VLAN_CONFIG == FYES)
VlanConfigProfile* buildFactoryVlanConfigProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->vlanConfigProfile)), 0, sizeof(VlanConfigProfile));
   return &(unionProf->vlanConfigProfile);
}
#endif

#if (DNS_AGENCY == FYES)
DnsAgencyProfile *buildFactoryDnsAgencyProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->dnsAgencyProfile)), 0, sizeof(DnsAgencyProfile));
	return &(unionProf->dnsAgencyProfile);
}
#endif

#if (L2TP_SERVER == FYES)
L2TPSrvGlobalProfile *buildFactoryL2TPSrvGloProfile(ProfileUnion *unionProf) {
   memset((void*)(&(unionProf->l2tpSrvGlobalProfile)), 0, sizeof(L2TPSrvGlobalProfile));
   return &(unionProf->l2tpSrvGlobalProfile);
}

L2TPSrvAccProfile *buildFactoryL2TPSrvAccProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->l2tpSrvAccProfile)), 0, sizeof(L2TPSrvAccProfile));
   return &(unionProf->l2tpSrvAccProfile);
}
#endif

#if (L2TP_CLIENT == FYES)
L2TPCliProfile *buildFactoryL2TPCliProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->l2tpCliProfile)), 0, sizeof(L2TPCliProfile));
    return &(unionProf->l2tpCliProfile);
}
#endif

#if (PPTP_SERVER == FYES)
PPTPSrvGlobalProfile *buildFactoryPPTPSrvGloProfile(ProfileUnion *unionProf) {
   memset((void*)(&(unionProf->pptpSrvGlobalProfile)), 0, sizeof(PPTPSrvGlobalProfile));
   return &(unionProf->pptpSrvGlobalProfile);
}

PPTPSrvAccProfile *buildFactoryPPTPSrvAccProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->pptpSrvAccProfile)), 0, sizeof(PPTPSrvAccProfile));
   return &(unionProf->pptpSrvAccProfile);
}
#endif

#if (PPTP_CLIENT == FYES)
PPTPCliProfile *buildFactoryPPTPCliProfile(ProfileUnion *unionProf)
{
    PPTPCliProfile *pptpCliProfile = &(unionProf->pptpCliProfile);
   memset(pptpCliProfile, 0, sizeof(PPTPCliProfile));
   pptpCliProfile->mtu = 1478U;
   pptpCliProfile->connectStatus = 1;
   return pptpCliProfile;
}
#endif

#if (STATIC_ROUTE == FYES)
StaticRouteProfile *buildFactoryStaticRouteProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->staticRouteProfile)), 0, sizeof(StaticRouteProfile));
   return &(unionProf->staticRouteProfile);
}
#endif

#if (DDNS == FYES)
DDNSProfile *buildFactoryDdnsProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->ddnsProfile)), 0, sizeof(DDNSProfile));
   return &(unionProf->ddnsProfile);
}
#endif

#if (PPPOE_SERVER == FYES)
PPPoESrvGloProfile *buildFactoryPppoeSrvGloProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->pppoeSrvGloProfile)), 0, sizeof(PPPoESrvGloProfile));
   return &(unionProf->pppoeSrvGloProfile);
}

PPPoESrvAccProfile *buildFactoryPppoeSrvAccProfile(ProfileUnion *unionProf)
{
    PPPoESrvAccProfile* pppoeSrvAccProfile = &(unionProf->pppoeSrvAccProfile);
    memset(pppoeSrvAccProfile, 0, sizeof(PPPoESrvAccProfile));
#if 0
    pppoeSrvAccProfile->ConTime = 1;
#endif
    pppoeSrvAccProfile->accmaxconv = 1;
    pppoeSrvAccProfile->sesIndex = -1;
#if (PPPOE_SELF_HELP == FYES)
    pppoeSrvAccProfile->optCnt = 0U;
#endif
    return pppoeSrvAccProfile;
}

#endif

#if (FEATURE_SW_PORT_CONF == FYES)
SwPortConfProfile* buildFactorySwPortConfProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->swPortConfProfile)), 0, sizeof(SwPortConfProfile));
   return &(unionProf->swPortConfProfile);
}
#endif

#if (FEATURE_PORT_VLAN == FYES)
PortVlanProfile* buildFactoryPortVlanProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->portVlanProfile)), 0, sizeof(PortVlanProfile));
   return &(unionProf->portVlanProfile);
}
#endif

#if (PVLAN_REDUCED == FYES)
RddPortVlanProfile* buildFactoryRddPortVlanProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->rddPortVlanProfile)), 0, sizeof(RddPortVlanProfile));
   return &(unionProf->rddPortVlanProfile);
}
#endif

#if (GROUP_BASE == FYES)
GroupBaseProfile* buildFactoryGroupBaseProfile(ProfileUnion *unionProf)
{
    GroupBaseProfile*  groupBaseProfile = &(unionProf->groupBaseProfile);  
    memset(groupBaseProfile, 0, sizeof(GroupBaseProfile));
    groupBaseProfile->head.active = TRUE;
    groupBaseProfile->groupType = GBT_FINE_LIMIT;
    groupBaseProfile->day = 128;
    groupBaseProfile->limitPolicy = EXCLUSIVE_RATE;

    return groupBaseProfile;
}	    
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
MirrorProfile* buildFactoryMirrorProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->mirrorProfile)), 0, sizeof(MirrorProfile));
	return &(unionProf->mirrorProfile);
}
#endif

#if (FEATURE_SW_PORT_TRUNK == FYES)
TrunkProfile* buildFactoryTrunkProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->trunkProfile)), 0, sizeof(TrunkProfile));
	return &(unionProf->trunkProfile);
}
#endif

#if (FEATURE_SW_SNMP == FYES)
SnmpProfile* buildFactorySnmpProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->snmpProfile)), 0, sizeof(SnmpProfile));
	return &(unionProf->snmpProfile);
}
#endif
#if (SNMP == FYES)
SnmpInfoProfile* buildFactorySnmpInfoProfile(ProfileUnion *unionProf)
{
      memset((void*)(&(unionProf->snmpInfoProfile)), 0, sizeof(SnmpInfoProfile));
      return &(unionProf->snmpInfoProfile);
}
#endif
#if (FEATURE_ACL == FYES)
AclProfile* buildFactoryAclProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->aclProfile)), 0, sizeof(AclProfile));
   return &(unionProf->aclProfile);
}
AclStatusProfile* buildFactoryAclStatusProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->aclStatusProfile)), 0, sizeof(AclStatusProfile));
   return &(unionProf->aclStatusProfile);
}
#endif

#if (UTTPDB == FYES)
PdbProfile* buildFactoryPdbProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->pdbProfile)), 0, sizeof(PdbProfile));
	return &(unionProf->pdbProfile);
}
#endif
#if (STATIC_NAT == FYES)
StaticNatProfile* buildFactoryStaticNatProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->staticNatProfile)), 0, sizeof(StaticNatProfile));
	return &(unionProf->staticNatProfile);
}
#endif
#if (NAT_RULE == FYES)
NatRuleProfile* buildFactoryNatRuleProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->natRuleProfile)), 0, sizeof(NatRuleProfile));
	return &(unionProf->natRuleProfile);
}
#endif
#if (DMZ == FYES)
DMZProfile* buildFactoryDMZProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->dmzProfile)), 0, sizeof(DMZProfile));
	return &(unionProf->dmzProfile);
}
#endif
#if (ADMIN_CONF == FYES)
AdminConfProfile* buildFactoryAdminConfProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->adminConfProfile)), 0, sizeof(AdminConfProfile));
	return &(unionProf->adminConfProfile);
}
#endif
#if (DNS_FILTER == FYES)
DnsFilterProfile* buildFactoryDnsFilterProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->dnsFilterProfile)), 0, sizeof(DnsFilterProfile));
	return &(unionProf->dnsFilterProfile);
}
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
CliForEngineerOnlyProfile* buildFactoryCliForEngineerOnlyProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->cliForEngineerOnlyProfile)), 0, sizeof(CliForEngineerOnlyProfile));
    return &(unionProf->cliForEngineerOnlyProfile);
}
#endif

#if (WIRELESS == FYES)
WirelessProfile *buildFactoryWirelessProfile(ProfileUnion *unionProf) {
    memset((void*)(&(unionProf->wirelessProfile)), 0, sizeof(WirelessProfile));
   return &(unionProf->wirelessProfile);
}

WirelessMacFilterProfile *buildFactoryWirelessMacFilterProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->wirelessMacFilterProfile)), 0, sizeof(WirelessMacFilterProfile));
   return &(unionProf->wirelessMacFilterProfile);
}
#if (WIRELESS_5G == FYES)
Wireless5gMacFilterProfile *buildFactoryWireless5gMacFilterProfile(ProfileUnion *unionProf)
{
   memset((void*)(&(unionProf->wireless5gMacFilterProfile)), 0, sizeof(Wireless5gMacFilterProfile));
   return &(unionProf->wireless5gMacFilterProfile);
}
#endif
#endif

#if (FEATURE_NTP == FYES)
NtpProfile* buildFactoryNtpProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->ntpProfile)), 0, sizeof(NtpProfile));
	return &(unionProf->ntpProfile);
}
#endif
#if (FEATURE_LANG == FYES)
LangProfile* buildFactoryLangProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->langProfile)), 0, sizeof(LangProfile));
	return &(unionProf->langProfile);
}
#endif

#if (TASK_SCHEDULER == FYES)
TaskSchedProfile* buildFactoryTaskSchedProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->taskSchedProfile)), 0, sizeof(TaskSchedProfile));
    return &(unionProf->taskSchedProfile);
}
#endif

#if (FIREWALL == FYES)
FireWallProfile* buildFactoryFireWallProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->fireWallProfile)), 0, sizeof(FireWallProfile));
    return &(unionProf->fireWallProfile);
}
#endif

#if (NOTICE == FYES)
NoticeProfile* buildFactoryNoticeProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->noticeProfile)), 0, sizeof(NoticeProfile));
    return &(unionProf->noticeProfile);
}
#endif

#if (CWMP == FYES)
CwmpProfile* buildFactoryCwmpProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->cwmpProfile)), 0, sizeof(CwmpProfile));
    return &(unionProf->cwmpProfile);
}
#endif

#if (WEB_AUTH == FYES)
WebAuthGlobalProfile* buildFactoryWebAuthGlobalProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->webAuthGlobalProfile)), 0, sizeof(WebAuthGlobalProfile));
    return &(unionProf->webAuthGlobalProfile);
}
WebAuthProfile* buildFactoryWebAuthProfile(ProfileUnion *unionProf)
{
    WebAuthProfile *webAuthProfile = &(unionProf->webAuthProfile);
    memset((void*)(&(unionProf->webAuthProfile)), 0, sizeof(WebAuthProfile));
webAuthProfile->maxSession = 1;
webAuthProfile->actualSession = 0U;
webAuthProfile->sesIndex = -1;
    return webAuthProfile;
}
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
WhiteListProfile* buildFactoryWhiteListProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->whiteListProfile)), 0, sizeof(WhiteListProfile));
    return &(unionProf->whiteListProfile);
}
#endif
#endif

#if (EXCEPT_QQ == FYES)
ExceptQQProfile* buildFactoryExceptQQProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->exceptQQProfile)), 0, sizeof(ExceptQQProfile));
	return &(unionProf->exceptQQProfile);
}
#endif

#if (EXCEPT_MSN == FYES)
ExceptMSNProfile* buildFactoryExceptMSNProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->exceptMSNProfile)), 0, sizeof(ExceptMSNProfile));
	return &(unionProf->exceptMSNProfile);
}
#endif

#if (EXCEPT_ALI == FYES)
ExceptAliProfile* buildFactoryExceptAliProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->exceptAliProfile)), 0, sizeof(ExceptAliProfile));
	return &(unionProf->exceptAliProfile);
}
#endif

#if (LOG_MANAGE == FYES)
LogManageProfile* buildFactoryLogManageProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->logManageProfile)), 0, sizeof(LogManageProfile));
	return &(unionProf->logManageProfile);
}
#endif

#if (AP_LOG_MANAGE == FYES)
ApLogManageProfile* buildFactoryApLogManageProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->apLogManageProfile)), 0, sizeof(ApLogManageProfile));
	return &(unionProf->apLogManageProfile);
}
#endif

#if (IP_SEC == FYES)
ipsecConfProfile* buildFactoryipsecConfProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->ipsecconfProfile)), 0, sizeof(ipsecConfProfile));
	return &(unionProf->ipsecconfProfile);
}
ipsecTransformSetProfile* buildFactoryipsectransformSetProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->ipsectransformsetProfile)), 0, sizeof(ipsecTransformSetProfile));
	return &(unionProf->ipsectransformsetProfile);
}
isakmpConfProfile* buildFactoryisakmpConfProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->isakmpconfProfile)), 0, sizeof(isakmpConfProfile));
	return &(unionProf->isakmpconfProfile);
}
isakmpPolicyProfile* buildFactoryisakmpPolicyProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->isakmppolicyProfile)), 0, sizeof(isakmpPolicyProfile));
	return &(unionProf->isakmppolicyProfile);
}
#endif
#if (SYSLOG == FYES)
SyslogProfile* buildFactorySyslogProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->syslogProfile)), 0, sizeof(SyslogProfile));
	return &(unionProf->syslogProfile);
}
#endif
#if (UTT_LICENSE == FYES)
LicenseProfile* buildFactoryLicenseProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->licenseProfile)), 0, sizeof(LicenseProfile));
	return &(unionProf->licenseProfile);
}
#endif
#if (POLICY_ROUTE == FYES)
PolicyRouteProfile* buildFactoryPolicyRouteProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->policyrouteProfile)), 0, sizeof(PolicyRouteProfile));
	return &(unionProf->policyrouteProfile);
}
#endif
#if (PARENTS_CONTROL == FYES)
ParentsControlProfile* buildFactoryParentsControlProfile(ProfileUnion *unionProf)
{

    ParentsControlProfile*  parentConProfile = &(unionProf->parentscontrolProfile);  

    memset(parentConProfile, 0, sizeof(ParentsControlProfile));
    parentConProfile->head.active = TRUE;
#if (UTT_SMART_UI == FYES)
    parentConProfile->day = 128;
#endif
    return parentConProfile;
}
#endif

#if (FAT_FIT_SWITCH == FYES)
FatFitSwitchProfile* buildFactoryFatFitSwitchProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->fatFitSwitchProfile)), 0, sizeof(FatFitSwitchProfile));
	return &(unionProf->fatFitSwitchProfile);
}
#endif
#if (AC_WORK_MODE == FYES)
AcWorkModeProfile* buildFactoryAcWorkModeProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->acWorkModeProfile)), 0, sizeof(AcWorkModeProfile));
	return &(unionProf->acWorkModeProfile);
}
#endif

#if (AP_LOAD_BALANCE == FYES)
ApLoadBalanceProfile* buildFactoryApLoadBalanceProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apLoadBalanceProfile)), 0, sizeof(ApLoadBalanceProfile));
	return &(unionProf->apLoadBalanceProfile);
}
#endif
#if (AP_PERMISSION == FYES)
ApPermissionProfile *buildFactoryApPermissionProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->apPermissionProfile)), 0, sizeof(ApPermissionProfile));
	return &(unionProf->apPermissionProfile);
}
#endif

#if (AP_CONF == FYES)
ApConfProfile *buildFactoryApConfProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->apConfProfile)), 0, sizeof(ApConfProfile));
	return &(unionProf->apConfProfile);
}
#endif

#if (SERVICE_ZONE == FYES)
ServiceZoneProfile *buildFactoryServiceZoneProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->serviceZoneProfile)), 0, sizeof(ServiceZoneProfile));
	return &(unionProf->serviceZoneProfile);
}
#endif

#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
ApMacFilterProfile *buildFactoryApMacFilterProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->apMacFilterProfile)), 0, sizeof(ApMacFilterProfile));
	return &(unionProf->apMacFilterProfile);
}
#endif


#if (TAG_VLAN == FYES)
TagVlanProfile *buildFactoryTagVlanProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->tagVlanProfile)), 0, sizeof(TagVlanProfile));
	return &(unionProf->tagVlanProfile);
}
#if (WAN_TAG_VLAN == FYES)
WanTagVlanProfile *buildFactoryWanTagVlanProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->wanTagVlanProfile)), 0, sizeof(WanTagVlanProfile));
	return &(unionProf->wanTagVlanProfile);
}
#endif
#endif

#if (FEATURE_AP_MAIL == FYES)
ApMailProfile* buildFactoryApMailProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apMailProfile)), 0, sizeof(ApMailProfile));
	return &(unionProf->apMailProfile);
}
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
ApConfTempProfile* buildFactoryApConfTempProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apConfTempProfile)), 0, sizeof(ApConfTempProfile));
	return &(unionProf->apConfTempProfile);
}
#endif

#if (FEATURE_AP_SCAN == FYES)
ApScanProfile* buildFactoryApScanProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apScanProfile)), 0, sizeof(ApScanProfile));
	return &(unionProf->apScanProfile);
}
#endif

#if (FEATURE_AP_SW_MNG == FYES)
ApSwMngProfile* buildFactoryApSwMngProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->apSwMngProfile)), 0, sizeof(ApSwMngProfile));
    return &(unionProf->apSwMngProfile);
}
#endif

#if (FEATURE_AP_BASIC_INFO == FYES)
ApBasicConfProfile* buildFactoryApBasicConfProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apBasicConfProfile)), 0, sizeof(ApBasicConfProfile));
	return &(unionProf->apBasicConfProfile);
}
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
ApEffectConfProfile* buildFactoryApEffectConfProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apEffectConfProfile)), 0, sizeof(ApEffectConfProfile));
	return &(unionProf->apEffectConfProfile);
}
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
FitApSzProfile* buildFactoryFitApSzProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->fitApSzProfile)), 0, sizeof(FitApSzProfile));
	return &(unionProf->fitApSzProfile);
}
#if(WIRELESS_5G == FYES)
FitAp5gSzProfile* buildFactoryFitAp5gSzProfile(ProfileUnion *unionProf)
{
    memset((void*)(&(unionProf->fitAp5gSzProfile)), 0, sizeof(FitAp5gSzProfile));
    return &(unionProf->fitAp5gSzProfile);
}
#endif
#endif

#if (CAPWAP_CONFIG_AP == FYES)
ApCapwapConfigProfile *buildFactoryApCapwapCongfigProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apCapwapConfigProfile)), 0, sizeof(ApCapwapConfigProfile));
	return &(unionProf->apCapwapConfigProfile);
}
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
ApBlackWhiteListProfile* buildFactoryApBWListProfile(ProfileUnion *unionProf)
{
	memset((void*)(&(unionProf->apBlackWhiteListProfile)), 0, sizeof(ApBlackWhiteListProfile));
	return &(unionProf->apBlackWhiteListProfile);
}
#endif

#if (P2P_LIMIT == FYES)
P2PLimitProfile* buildFactoryP2PLimitProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->p2pLimitProfile)), 0, sizeof(P2PLimitProfile));
	return &(unionProf->p2pLimitProfile);
}
#endif
#if (SYS_LOG_INFO == FYES)
SysLogInfoProfile* buildFactorySysLogInfoProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->sysLogInfoProfile)), 0, sizeof(SysLogInfoProfile));
	return &(unionProf->sysLogInfoProfile);
}
#endif
#if (EASY_TASK == FYES)
EasyTaskProfile* buildFactoryEasyTaskProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->easyTaskProfile)), 0, sizeof(EasyTaskProfile));
	return &(unionProf->easyTaskProfile);
}
#endif

#if (HTTP_CACHE == FYES)
CacheProfile* buildFactoryCacheProfile(ProfileUnion *unionProf)
{
	 memset((void *)(&(unionProf->cacheProfile)),0,sizeof(CacheProfile));
	 return &(unionProf->cacheProfile);
}
#endif

#if (NET_MODE_SWITCH == FYES)
NetModeSwitchProfile* buildFactoryNetModeSwitchProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->netModeSwitchProfile)),0,sizeof(NetModeSwitchProfile));
	return &(unionProf->netModeSwitchProfile);
}
#endif
#if (BRIDGE== FYES)
BridgeProfile* buildFactoryBridgeProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->bridgeProfile)),0,sizeof(BridgeProfile));
	return &(unionProf->bridgeProfile);
}
#endif
#if (CACHE_SERVER == FYES)
CacheServerProfile* buildFactoryCacheServerProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->cacheServerProfile)),0,sizeof(CacheServerProfile));
	return &(unionProf->cacheServerProfile);
}
#endif
#if (MAC_FILTER == FYES)
MacFilterGlobalProfile* buildFactoryMacFilterGlobalProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->macFilterGlobalProfile)), 0, sizeof(MacFilterGlobalProfile));
        return &(unionProf->macFilterGlobalProfile);
}
MacFilterProfile* buildFactoryMacFilterProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->macFilterProfile)), 0, sizeof(MacFilterProfile));
        return &(unionProf->macFilterProfile);
}
#endif
#if (NET_SNIPER == FYES)
NetSniperProfile* buildFactoryNetSniperProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->netSniperProfile)),0,sizeof(NetSniperProfile));
	return &(unionProf->netSniperProfile);
}
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
ShortCutMenuProfile* buildFactoryShortCutMenuProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->shortCutMenuProfile)), 0, sizeof(ShortCutMenuProfile));
        return &(unionProf->shortCutMenuProfile);
}
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
NetShareManageProfile* buildFactoryNetShareManageProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->netShareManageProfile)), 0, sizeof(NetShareManageProfile));
        return &(unionProf->netShareManageProfile);
}
#endif
#if (FTP_SERVER == FYES)
FtpServerProfile* buildFactoryFtpServerProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->ftpServerProfile)), 0, sizeof(FtpServerProfile));
        return &(unionProf->ftpServerProfile);
}
FtpShareDirProfile* buildFactoryFtpShareDirProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->ftpShareDirProfile)), 0, sizeof(FtpShareDirProfile));
        return &(unionProf->ftpShareDirProfile);
}
#endif
#if (NET_SHARE_USER == FYES)
NetShareUserProfile* buildFactoryNetShareUserProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->netShareUserProfile)), 0, sizeof(NetShareUserProfile));
        return &(unionProf->netShareUserProfile);
}
#endif

#if (ADFILTER == FYES)
AdfilterProfile* buildFactoryAdfilterProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->adfilterProfile)), 0, sizeof(AdfilterProfile));
        return &(unionProf->adfilterProfile);
}
#endif
#if (MAKE_MONEY == FYES)
MakeMoneyProfile* buildFactoryMakeMoneyProfile(ProfileUnion *unionProf)
{
        memset((void *)(&(unionProf->makeMoneyProfile)), 0, sizeof(MakeMoneyProfile));
        return &(unionProf->makeMoneyProfile);
}
#endif
#if (UTT_KNOWIFI == FYES)
KnowifiDomainProfile* buildFactoryKnowifiDomainProfile(ProfileUnion *unionProf)
{
	memset((void *)(&(unionProf->knowifiDomainProfile)), 0, sizeof(KnowifiDomainProfile));
	return &(unionProf->knowifiDomainProfile);
}
#endif
#if (WIRELESS == FYES)
extern int get3GSlotIndex(void);
extern int getDefAPCliSlotIndex(void);
#endif

extern void getDefaultIfCount(int *wanCountPtr, int *lanCountPtr);

extern unsigned int uttSerioNo;
#if (WRITE_NEWMAC == FYES)
unsigned char newmac[6] = {0};
#define AP_MAC_OFFSET 0x2f880
#define MAC_LEN 6
#endif
bool BuildDefaultFactoryValue(MibProfileType proftype,
        int instindex, SlotNumber slot,Pointer *_prof)
{
    bool rst = FALSE;
    Pointer defprof = NULL;
    *_prof = NULL;
    char tmpStr[64]={0};
    unsigned int tmpsn;
    unsigned int macint;

    SystemProfile *sysProfile;
    InterfaceProfile *interfaceProfile;
#if (CLOUD_MANAGE == FYES)
    CloudManageProfile *cloudManageProfile;
#endif
#if (CLOUD_CONFIG == FYES)
    CloudConfigProfile *cloudConfigProfile;
#endif
#if (IPV6_INTERFACE == FYES)
    Ipv6InterfaceProfile *ipv6interfaceProfile;
#endif

#if (IPV6_RADVD == FYES)
    RadvdProfile *radvdProfile;
#endif
#if (IPV6_NOTLISTPREFIXS == FYES)
    Ipv6PrefixListProfile *ipv6PrefixListProfile;
#endif
#if (IPV6_ROUTE == FYES)
    Ipv6RoutConfProfile *ipv6RoutConfProfile;
#endif
#if (IPV6_DHCPV6 == FYES)
    Dhcp6Profile *dhcp6Profile;
#endif
#if (IPV6_TUNNEL == FYES)
    TunnelProfile *tunnelProfile;
#endif
#if (IPV6_ISATAP == FYES)
    IsatapProfile *isatapProfile;
#endif

#if (DHCP_SERVER == FYES)
    DhcpServerProfile *dhcpSProfile;
#endif

#if (DHCP_POOL == FYES)
	DhcpPoolProfile *dhcpPool;
#endif
#if (VLAN_CONFIG == FYES)
	VlanConfigProfile *vlanConfig;
#endif

#if(DNS_AGENCY == FYES)
	DnsAgencyProfile *dnsAgency;
#endif

#if (L2TP_SERVER == FYES)
    L2TPSrvGlobalProfile *l2tpSProf;
#endif

#if (PPTP_SERVER == FYES)
    PPTPSrvGlobalProfile *pptpSProf;
#endif

#if (UTT_SMART_UI == FYES)
#if (PPTP_CLIENT == FYES)
    PPTPCliProfile *pptpprof;
#endif
#if (L2TP_CLIENT == FYES)
    L2TPCliProfile *l2tpprof;
#endif
#endif
#if (DDNS == FYES)
    DDNSProfile	*ddnsProf;
#endif

#if (PPPOE_SERVER == FYES)
    PPPoESrvGloProfile *pppoeSrvGloProf;
    PPPoESrvAccProfile *pppoeSrvAccProfile;
#endif

#if (TAG_VLAN == FYES)
#if (WAN_TAG_VLAN == FYES)
    WanTagVlanProfile *wanTagVlanProf;
#endif
#endif

#if (FEATURE_PORT_VLAN == FYES)
    PortVlanProfile *portVlanProfile; 
#endif

#if (PVLAN_REDUCED == FYES)
    RddPortVlanProfile *rddPortVlanProfile; 
#endif

#if (FEATURE_SW_PORT_MIRROR == FYES)
	MirrorProfile *mirrorProfileDef;
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
    SwPortConfProfile* swPortConfProfile;
#endif

#if (FEATURE_SW_PORT_TRUNK == FYES)
	TrunkProfile	*trunkProfileDef;
#endif
#if (FEATURE_SW_SNMP == FYES)
    SnmpProfile *snmpProfileDef;
#endif
#if (SNMP == FYES)
    SnmpInfoProfile *snmpInfoProfileDef;
#endif
#if (UTTPDB == FYES)
	PdbProfile  *pdbProfile;
#endif
#if (FEATURE_ACL == FYES)
    AclProfile *aclProfile;
    AclStatusProfile *aclStatusProfile;
#endif
#if (STATIC_NAT == FYES)
    StaticNatProfile *staticNatProfile;
#endif
#if (NAT_RULE == FYES)
    NatRuleProfile *natRuleProfile;
#endif
#if (DMZ == FYES)
    DMZProfile *dmzProfile;
#endif
#if (ADMIN_CONF == FYES)
    AdminConfProfile	*adminConfProfile;
#endif
#if (DNS_FILTER == FYES)
    DnsFilterProfile	*dnsFilterProfile;
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    CliForEngineerOnlyProfile    *cliForEngineerOnlyProfile;
#endif

#if (WIRELESS == FYES)
    WirelessMacFilterProfile	*wirelessMacFilterProfile;
#if (WIRELESS_5G == FYES)
    Wireless5gMacFilterProfile	*wireless5gMacFilterProfile;
#endif
#endif
#if (TASK_SCHEDULER == FYES)
    TaskSchedProfile        *taskSchedProfile;
#endif
#if (FEATURE_NTP == FYES)
    NtpProfile *ntpProfile;
#endif
#if (FEATURE_LANG == FYES)
    LangProfile *langProfile; 
#endif
#if (FIREWALL == FYES)
    FireWallProfile *fireWallProfile; 
#endif
#if (WIRELESS == FYES)
    WirelessProfile *wirelessProfile;
#endif
#if (NOTICE == FYES)
    NoticeProfile *noticeProfile;
#endif
#if (WEB_AUTH == FYES)
    WebAuthProfile *webAuthProfile;
    WebAuthGlobalProfile *webAuthGlobalProfile;
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
    WhiteListProfile *whiteListProfile;
#endif
#endif
#if (EXCEPT_QQ == FYES)
    ExceptQQProfile *exceptQQProfile;
#endif
#if (EXCEPT_MSN == FYES)
    ExceptMSNProfile *exceptMSNProfile;
#endif
#if (EXCEPT_ALI == FYES)
    ExceptAliProfile *exceptAliProfile;
#endif
#if (IP_GRP==FYES)
    IpGrpProfile *ipGrpProfile;
#endif
#if (PORT_GRP==FYES)
    PortGrpProfile *portGrpProfile;
#endif
#if (TIME_GRP == FYES)
    TimeGrpProfile *timeGrpProfile;
#endif
#if (LOG_MANAGE == FYES)
    LogManageProfile *logManageProfile;
#endif
#if (AP_LOG_MANAGE == FYES)
    ApLogManageProfile *apLogManageProfile;
#endif
#if (IP_SEC==FYES)
    ipsecConfProfile* ipsecProfile;
    ipsecTransformSetProfile* ipsectransformsetProfile;
    isakmpConfProfile* isakmpProfile;
    isakmpPolicyProfile* isakmppolicyProfile;
#endif
#if (SYSLOG == FYES)
    SyslogProfile *syslogProfile;
#endif
#if (CWMP == FYES)
    CwmpProfile *cwmpProfile;
#endif
#if (P2P_LIMIT == FYES)
    P2PLimitProfile *p2pLimitProfile;
#endif
#if (SYS_LOG_INFO == FYES)
    SysLogInfoProfile *sysLogInfoProfile;
#endif
#if (EASY_TASK == FYES)
    EasyTaskProfile *easyTaskProfile;
#endif
#if (FAT_FIT_SWITCH == FYES)
    FatFitSwitchProfile	*fatFitSwitchProfile;
#endif
#if (AC_WORK_MODE == FYES)
    AcWorkModeProfile *acWorkModeProfile;
#endif

#if (AP_LOAD_BALANCE == FYES)
    ApLoadBalanceProfile *apLoadBalanceProfile;
#endif
#if (AP_PERMISSION == FYES)
	ApPermissionProfile *apPermissionProfile;
#endif
#if (AP_CONF == FYES)
	ApConfProfile *apConfProfile;
#endif
#if (SERVICE_ZONE == FYES)
	ServiceZoneProfile *serviceZoneProfile;
#endif
#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	ApMacFilterProfile *apMacFilterProfile;
#endif
#if (FEATURE_AP_MAIL == FYES)
    ApMailProfile	*apMailProfile;
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
    ApConfTempProfile	*apConfTempProfile;
#endif
#if (FEATURE_AP_SCAN == FYES)
    ApScanProfile *apScanProfile;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
    ApSwMngProfile *apSwMngProfile;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
    ApBasicConfProfile *apBasicConfProfile;
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
    ApEffectConfProfile	*apEffectConfProfile;
#if (WIRELESS_5G == FYES)
    FitAp5gSzProfile *fitAp5gSzProfile;
#endif
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)
    FitApSzProfile	*fitApSzProfile;
#endif
#if (CAPWAP_CONFIG_AP == FYES)
    ApCapwapConfigProfile * apCapwapConfigProfile;
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
    ApBlackWhiteListProfile  *apBlackWhiteListProfile;
#endif
# if(HTTP_CACHE == FYES)
	 CacheProfile *cacheProfile;
#endif

# if(NET_MODE_SWITCH == FYES)
	NetModeSwitchProfile *netModeSwitchProfile;
#endif
# if(CACHE_SERVER == FYES)
	CacheServerProfile *cacheServerProfile;
#endif
# if(NET_SNIPER == FYES)
	NetSniperProfile *netSniperProfile;
#endif

# if(BRIDGE== FYES)
	BridgeProfile *bridgeProfile;
#endif
#if (UTT_LICENSE == FYES)
    LicenseProfile *licenseProfile;
#endif
#if (POLICY_ROUTE == FYES)
    PolicyRouteProfile *policyrouteProfile;
#endif
#if (MAC_FILTER == FYES)
    MacFilterGlobalProfile *macFilterGlobalProfile;
    MacFilterProfile *macFilterProfile;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
    ShortCutMenuProfile *shortCutMenuProfile;
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
    NetShareManageProfile *netShareManageProfile;
#endif
#if (FTP_SERVER == FYES)
    FtpServerProfile *ftpServerProfile;
    FtpShareDirProfile *ftpShareDirProfile;
#endif
#if (NET_SHARE_USER == FYES)
    NetShareUserProfile *netShareUserProfile;
#endif
#if (ADFILTER == FYES)
    AdfilterProfile *adfilterProfile;
#endif
#if (MAKE_MONEY == FYES)
    MakeMoneyProfile *makeMoneyProfile;
#endif
#if (UTT_KNOWIFI == FYES)
    KnowifiDomainProfile *knowifiDomainProfile;
#endif
    profileBuildFactory(proftype, &defprof);

    if(defprof == NULL) {
        *_prof = NULL;
        return FALSE;
    }

    switch(proftype) {
        case MIB_PROF_SYSTEM:
            sysProfile = (SystemProfile *)defprof;  

	    sysProfile->head.active = TRUE;
	    getDefaultIfCount(&(sysProfile->wanIfCount), &(sysProfile->lanPortCount));
#if (UTT_NV_WX20S == FYES)
            sysProfile->popWelcome = FUN_DISABLE;
#else
            sysProfile->popWelcome = FUN_ENABLE;
#endif
#if (UTT_IPAUTOREP ==FYES)
	    sysProfile->CheckIPEn = FUN_ENABLE;
#endif
#if (PORT_MIRROR == FYES)
	    sysProfile->portMirrorEn = FUN_DISABLE;
#if defined( CONFIG_RTL8367M) || defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RTL8367RB)|| defined(CONFIG_RALINK_MT7621) || defined(CONFIG_RALINK_MT7628)
	    sysProfile->mirror.monitorPort = 1;
	    sysProfile->mirror.sourcePort = 0;
#endif
#endif
#if (EXCEPT_QQ == FYES)
	    sysProfile->exceptQQEn = FUN_DISABLE;
#endif
#if (EXCEPT_MSN == FYES)
	    sysProfile->exceptMSNEn = FUN_DISABLE;
#endif
#if (EXCEPT_ALI == FYES)
	    sysProfile->exceptAliEn = FUN_DISABLE;
#endif
#if (PORT_MIRROR_6530G == FYES)
	    sysProfile->Mirror_6530G_En = FUN_DISABLE;
#endif
#if (UTTPDB == FYES)
	    sysProfile->UttNfIndentEn = FUN_DISABLE;
#endif
#if (GROUP_BASE == FYES)
	    sysProfile->UttNfRuleEn = FUN_DISABLE;
#endif
#if (CONNECT_LIMIT == FYES)
	    /* ³õÊ¼»¯ */
	    sysProfile->connLimit.connLimitEn = FUN_DISABLE;
	    sysProfile->connLimit.totalConnCount = DEF_TOTAL_CONN_COUNT;
	    sysProfile->connLimit.TCPConnCount = DEF_TCP_CONN_COUNT;
	    sysProfile->connLimit.UDPConnCount = DEF_UDP_CONN_COUNT;
	    sysProfile->connLimit.ICMPConnCount = DEF_ICMP_CONN_COUNT;
#endif
#if (TAG_VLAN == FYES)
	    /* Ä¬ÈÏpvid 1 */
	    sysProfile->pvid.pvid1 = 1;
	    sysProfile->pvid.pvid2 = 1;
	    sysProfile->pvid.pvid3 = 1;
	    sysProfile->pvid.pvid4 = 1;
#endif
#if (NET_SECURITY == FYES)
		sysProfile->sysNetSe.lanArpBroadcastEn = FUN_DISABLE;
		sysProfile->sysNetSe.lanArpBroadcastInterval = DEF_LAN_ARP_INTERVAL;
#endif
#if (SSH_CLI == FYES)
            sysProfile->SSH.SSHEn = FUN_DISABLE;
            sysProfile->SSH.Port = SSH_PORT;
#endif
#if (HTTPS == FYES)
	    sysProfile->sysHttps.httpsEn = FUN_DISABLE;
#endif
#if (HOTEL_PNP == FYES)
	    sysProfile->hotelPnpEn = FUN_DISABLE;
#endif
#if (HARDWARE_NAT == FYES)
	    sysProfile->hardwareNATEn = FUN_DISABLE;
#endif
#if (APP_ELINKC == FYES)
        sysProfile -> elinkc_flag = 1;
#endif
#if (WIFI_SLEEP_FLAG == FYES)
        sysProfile -> wifisleep = 1;
#endif
#if (FIREWALL == FYES)
	    sysProfile->firewallEn = FUN_DISABLE;
#endif
#if (POLICY_ROUTE == FYES)
	    sysProfile->policyrouteEn = FUN_DISABLE;
#endif
#if (PARENTS_CONTROL == FYES)
	    sysProfile->parentControlEn.parentsEn = FUN_DISABLE;
#endif
#if (AP_LOAD_BALANCE == FYES)
	    sysProfile->loadBalanceEn= FUN_DISABLE;
#endif
#if (SZONE_AUTO_UPDATA == FYES)
	    sysProfile->szoneAutoUpdata = FUN_DISABLE;
#endif
        sysProfile->brideg_mode_flag = 1;

            ProfSetNameByPoint((void*)sysProfile, "sysConf");
            rst = TRUE;
            break;
        case MIB_PROF_INTERFACE:
            interfaceProfile = (InterfaceProfile *)defprof;  
#if (WIRELESS == FYES)
	    /* ÎÞÏß½Ó¿ÚÏßÂ·(3G apclient)Ä¬ÈÏdownµô */
	    if ((instindex == get3GSlotIndex()) || (instindex == getDefAPCliSlotIndex())) {
		interfaceProfile->head.active = FALSE;
	    } else 
#endif
	    {
		interfaceProfile->head.active = TRUE;
	    }
#if (NAT_GLOBAL_ENABLE == FYES)
            interfaceProfile->NatActive = 1;
#endif
#if (UPNP_GLOBAL_ENABLE == FYES)
            interfaceProfile->UpnpActive= 0;
#endif

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)
	    interfaceProfile->ether.connMode = ETH_CONN_STATIC;
	    interfaceProfile->ether.sta.ip = htonl(0xc0a801fd);
	    interfaceProfile->ether.sta.nm = htonl(0xffffff00);
#if (MULTI_LAN == FYES)
	    interfaceProfile->ether.sta.ip2 = htonl(0x00000000);
	    interfaceProfile->ether.sta.nm2 = htonl(0xffffff00);
	    interfaceProfile->ether.sta.ip2 = htonl(0x00000000);
	    interfaceProfile->ether.sta.nm3 = htonl(0xffffff00);
	    interfaceProfile->ether.sta.ip2 = htonl(0x00000000);
	    interfaceProfile->ether.sta.nm4 = htonl(0xffffff00);
#endif
	    tmpsn = uttSerioNo;
#else
	    if (instindex == 0) {
#if (UTT_LV_N8000 == FYES) || (UTT_NV_WX20S == FYES)
		interfaceProfile->ether.sta.ip = htonl(0xc0a801fc);
#elif (LAN_DEFAULT_IPADDR == FYES)
		interfaceProfile->ether.sta.ip = htonl(0xc0a80a01);
#elif (RAISECOM_A30W == FYES)
		interfaceProfile->ether.sta.ip = htonl(0xc0a80afd);
#elif (SKYWORTH == FYES)
        interfaceProfile->ether.sta.ip = htonl(0xc0a85901);
#else
		interfaceProfile->ether.sta.ip = htonl(0xc0a801fe);
#endif
		interfaceProfile->ether.sta.nm = htonl(0xffffff00);
#if (MULTI_LAN == FYES)
		interfaceProfile->ether.sta.ip2 = htonl(0x00000000);
		interfaceProfile->ether.sta.nm2 = htonl(0xffffff00);
		interfaceProfile->ether.sta.ip2 = htonl(0x00000000);
		interfaceProfile->ether.sta.nm3 = htonl(0xffffff00);
		interfaceProfile->ether.sta.ip2 = htonl(0x00000000);
		interfaceProfile->ether.sta.nm4 = htonl(0xffffff00);
#endif
		tmpsn = uttSerioNo + 0xa10 + 1;
	    } else {
        if(instindex == 1)
        {
            tmpsn = uttSerioNo;
        }
        else
        {
            tmpsn = uttSerioNo + 0xa10 +instindex;
        }
            }
#endif
#if (ARP_BINDS == FYES)
	    interfaceProfile->arpAllowOtherEn = FUN_ENABLE;/*ÔÊÐí·Ç°ó¶¨ÓÃ»§Á¬½Ó*/
#endif
            sprintf(tmpStr, "%d", instindex);
            ProfSetNameByPoint((void*)interfaceProfile, tmpStr);
	    interfaceProfile->detectCount=10u;/*ÏßÂ·¼ì²âÄ¬ÈÏÖµ¸ÄÎª10*/
#if (WRITE_NEWMAC == FYES)
        flash_read_mtdname("Bootloader", newmac, AP_MAC_OFFSET, MAC_LEN);
        if (newmac != NULL) {
            if (newmac[0] == 0xFF && newmac[1] == 0xFF && newmac[2] == 0xFF && newmac[3] == 0xFF && newmac[4] == 0xFF && newmac[5] == 0xFF) {
                interfaceProfile->ether.mac.addr[0] = 0xc8;
                interfaceProfile->ether.mac.addr[1] = 0x50;
                interfaceProfile->ether.mac.addr[2] = 0xe9;
            }
            else {
                macint=*(unsigned int*)(newmac+2);
                macint = ntohl(macint);
                macint += instindex;
                interfaceProfile->ether.mac.addr[0] = newmac[0];
                interfaceProfile->ether.mac.addr[1] = newmac[1];
                interfaceProfile->ether.mac.addr[2] = newmac[2];
                interfaceProfile->ether.mac.addr[3] = (0xFF & (macint >> 16));
                interfaceProfile->ether.mac.addr[4] =  (0xFF & (macint >> 8));
                interfaceProfile->ether.mac.addr[5] = (0xFF & macint);
            }
        } else {
            interfaceProfile->ether.mac.addr[0] = 0xc8;
            interfaceProfile->ether.mac.addr[1] = 0x50;
            interfaceProfile->ether.mac.addr[2] = 0xe9;
        }
#else
#ifdef CONFIG_CHINA_MAC_04714B
	    interfaceProfile->ether.mac.addr[0] = 0x04;
            interfaceProfile->ether.mac.addr[1] = 0x71;
            interfaceProfile->ether.mac.addr[2] = 0x4B;
#else
#ifdef CONFIG_UTT_MAC_FC2FEF
	    interfaceProfile->ether.mac.addr[0] = 0xFC;
            interfaceProfile->ether.mac.addr[1] = 0x2F;
            interfaceProfile->ether.mac.addr[2] = 0xEF;
#elif defined(CONFIG_UTT_MAC_C850E9)
            interfaceProfile->ether.mac.addr[0] = 0xc8;
            interfaceProfile->ether.mac.addr[1] = 0x50;
            interfaceProfile->ether.mac.addr[2] = 0xe9;
#else
            interfaceProfile->ether.mac.addr[0] = 0x00;
            interfaceProfile->ether.mac.addr[1] = 0x22;
            interfaceProfile->ether.mac.addr[2] = 0xaa;
#endif
#endif
            interfaceProfile->ether.mac.addr[3] = (0xFF & (tmpsn >> 16));
            interfaceProfile->ether.mac.addr[4] = (0xFF & (tmpsn >> 8));
            interfaceProfile->ether.mac.addr[5] = (0xFF & tmpsn);
#endif
#if (PORT_CONF_REDUCED == FYES)
	    interfaceProfile->ether.speedMode=PORT_MODE_AUTO;
#endif
            rst = TRUE;
            break;
#if (IPV6_INTERFACE == FYES)
        case MIB_PROF_IPV6_INTERFACE:
            ipv6interfaceProfile = (Ipv6InterfaceProfile *)defprof;
            ipv6interfaceProfile->LanAutoEnabled = 1;
            ipv6interfaceProfile->LANprefixLength = 64;
            ipv6interfaceProfile->WANprefixLength = 64;
            memset(tmpStr, 0, sizeof(tmpStr));
            sprintf(tmpStr, "%d", instindex+1);

            /*ÉèÖÃname¿ÉÊ¹¸÷¶Ë¿Ú¶ÔÓ¦µÄÄÚ´æÅäÖÃË÷Òý²»±ä*/
            ProfSetNameByPoint(ipv6interfaceProfile, tmpStr);
            rst = TRUE;
            break;
#endif
#if (IPV6_RADVD == FYES)
        case MIB_PROF_RADVD:
            radvdProfile = (RadvdProfile *)defprof;
            ProfSetNameByPoint(radvdProfile, "radvd");
            radvdProfile->head.active=1;
            radvdProfile->manageEnabled=1;
            radvdProfile->otherEnabled=1;
            radvdProfile->noticeInterval=30;
            radvdProfile->routerPreferences=1;
            radvdProfile->MTU_config=1500;
            radvdProfile->routerLifetime=3600;
            rst = TRUE;
            break;
#endif
#if( IPV6_NOTLISTPREFIXS == FYES)
        case MIB_PROF_IPV6_NOTLISTPREFIXS:
            ipv6PrefixListProfile = (Ipv6PrefixListProfile *)defprof;
            ProfSetNameByPoint(ipv6PrefixListProfile, "");
            rst = TRUE;
            break;
#endif
#if(IPV6_ROUTE ==FYES)
        case MIB_PROF_IPV6_ROUTECONF:
        ipv6RoutConfProfile = (Ipv6RoutConfProfile *)defprof;
        ProfSetNameByPoint(ipv6RoutConfProfile,"");
        rst = TRUE;
        break;
#endif
#if (IPV6_DHCPV6 == FYES)
        case MIB_PROF_DHCP6:
            dhcp6Profile = (Dhcp6Profile *)defprof;
            ProfSetNameByPoint(dhcp6Profile, "dhcp6");
            rst = TRUE;
            break;
#endif
#if (IPV6_TUNNEL == FYES)
        case MIB_PROF_TUNNEL:
            tunnelProfile = (TunnelProfile *)defprof;
            ProfSetNameByPoint(tunnelProfile, "tunnel");
            rst = TRUE;
            break;
#endif
#if (IPV6_ISATAP == FYES)
        case MIB_PROF_ISATAP:
            isatapProfile = (IsatapProfile *)defprof;
            ProfSetNameByPoint(isatapProfile, "isatap");
            rst = TRUE;
            break;
#endif

#if (IP_GRP == FYES) 
	case MIB_PROF_IPGRP:
            ipGrpProfile = (IpGrpProfile *)defprof;  
	    memset(ipGrpProfile, 0, sizeof(IpGrpProfile));
	    ipGrpProfile->head.active = FALSE;
            rst = TRUE;
            break;
#endif
#if (PORT_GRP == FYES) 
	case MIB_PROF_PORTGRP:
            portGrpProfile = (PortGrpProfile *)defprof;  
	    memset(portGrpProfile, 0, sizeof(PortGrpProfile));
	    portGrpProfile->head.active = FALSE;
            rst = TRUE;
            break;
#endif
#if (TIME_GRP == FYES)
	case MIB_PROF_TIMEGRP:
	    timeGrpProfile = (TimeGrpProfile *)defprof;
	    timeGrpProfile->head.active = FALSE;
	    rst = TRUE;
	    break;
#endif
#if (DHCP_SERVER == FYES)
    DhcpServerProfile *dhcpSProfile;
        case MIB_PROF_DHCP_SERVER:
            if (instindex == 0) {
                dhcpSProfile = (DhcpServerProfile *)defprof;
                ProfSetNameByPoint(dhcpSProfile, "dhcpServer");
                dhcpSProfile->head.active = TRUE;
#if (UTT_NV_1800N == FYES)
                dhcpSProfile->enable = PROF_DISABLE;
#else
                dhcpSProfile->enable = PROF_ENABLE;
#endif
#if (MULTI_SSID_DHCP_POOL == FYES)
                dhcpSProfile->ipStart = htonl(0xc0a80102);
                dhcpSProfile->ipEnd = htonl(0xc0a80133);

                dhcpSProfile->ssid1_dhcp_enable = PROF_DISABLE;
                dhcpSProfile->ssid1_ipStart = htonl(0xc0a80134);
                dhcpSProfile->ssid1_ipEnd = htonl(0xc0a80165);

                dhcpSProfile->ssid2_dhcp_enable = PROF_DISABLE;
                dhcpSProfile->ssid2_ipStart = htonl(0xc0a80166);
                dhcpSProfile->ssid2_ipEnd = htonl(0xc0a80197);
#if (WIRELESS_5G == FYES)
                dhcpSProfile->ssid1_dhcp_enable_5g = PROF_DISABLE;
                dhcpSProfile->ssid1_ipStart_5g = htonl(0xc0a80198);
                dhcpSProfile->ssid1_ipEnd_5g = htonl(0xc0a801C9);

                dhcpSProfile->ssid2_dhcp_enable_5g = PROF_DISABLE;
                dhcpSProfile->ssid2_ipStart_5g = htonl(0xc0a801CA);
                dhcpSProfile->ssid2_ipEnd_5g = htonl(0xc0a801FB);
#endif
#elif (LAN_DEFAULT_IPADDR == FYES)
                dhcpSProfile->ipStart = htonl(0xc0a80a64);
                dhcpSProfile->ipEnd = htonl(0xc0a80ac8);
#elif (RAISECOM_A30W == FYES)
                dhcpSProfile->ipStart = htonl(0xc0a80a64);
                dhcpSProfile->ipEnd = htonl(0xc0a80ac8);
#elif (SKYWORTH == FYES)
                dhcpSProfile->ipStart = htonl(0xc0a85964);
                dhcpSProfile->ipEnd = htonl(0xc0a859c8);
#else
                dhcpSProfile->ipStart = htonl(0xc0a80164);
                dhcpSProfile->ipEnd = htonl(0xc0a801c8);
#endif
                dhcpSProfile->netmask = htonl(0xffffff00);
#if (UTT_NV_1800N != FYES && LAN_DEFAULT_IPADDR != FYES && SKYWORTH != FYES)
		dhcpSProfile->pDns = htonl(0xc0a801fe);;
                dhcpSProfile->gateway = htonl(0xc0a801fe);
#elif (SKYWORTH == FYES)
		dhcpSProfile->pDns = htonl(0xc0a85901);;
                dhcpSProfile->gateway = htonl(0xc0a85901);
#else
		dhcpSProfile->pDns = htonl(0xc0a80a01);;
                dhcpSProfile->gateway = htonl(0xc0a80a01);
#endif
                dhcpSProfile->lease = 3600U;
#if (DHCP_SERVER_OPTION_43 == FYES)
		dhcpSProfile->option43Type = DHCP_OPTION_43_TYPE_DISABLE;
		strcpy(dhcpSProfile->option43Addr, "0.0.0.0");
		memset(dhcpSProfile->option43Addr2, 0, sizeof(dhcpSProfile->option43Addr2));
#endif
#if (UTT_NV_1800N == FYES)
		dhcpSProfile->DnsProxyEn = PROF_DISABLE;
#else
		dhcpSProfile->DnsProxyEn = PROF_ENABLE;
#endif
                rst = TRUE;
            }
            break;
#endif

#if (DHCP_POOL == FYES)
        case MIB_PROF_DHCP_POOL:
            if (instindex == 0) {
                dhcpPool = (DhcpPoolProfile *)defprof;
                ProfSetNameByPoint(dhcpPool, "default");
                dhcpPool->head.active = TRUE;
#if ((UTT_NV_1800N == FYES) || (UTT_NV_WX20S == FYES))
                dhcpPool->dhcpEn = PROF_DISABLE;
#else
                dhcpPool->dhcpEn = PROF_ENABLE;
#endif
                dhcpPool->vid = 0U;
#if (VLAN_CONFIG == FYES)
				strcpy(dhcpPool->interface, "LAN");
#else
				strcpy(dhcpPool->interface, getLanIfName());
#endif
                dhcpPool->ipStart = htonl(0xc0a80164);
                dhcpPool->ipEnd = htonl(0xc0a801c8);
                dhcpPool->netmask = htonl(0xffffff00);
#if (UTT_NV_1800N != FYES)
                dhcpPool->pDns = htonl(0xc0a80101);
                dhcpPool->gateway = htonl(0xc0a80101);
#endif
                dhcpPool->lease = 7200U;
				dhcpPool->sDns = htonl(0x00000000);
#if (DHCP_SERVER_OPTION_43 == FYES)
		dhcpPool->option43Type = DHCP_OPTION_43_TYPE_DISABLE;
		strcpy(dhcpPool->option43Addr, "0.0.0.0");
		memset(dhcpPool->option43Addr2, 0, sizeof(dhcpPool->option43Addr2));
#endif
                rst = TRUE;
            }
            break;
#endif

#if (VLAN_CONFIG == FYES)
        case MIB_PROF_VLAN_CONFIG:
            if (instindex == 0) {
                vlanConfig = (VlanConfigProfile *)defprof;
                vlanConfig->head.active = FALSE;
				memset(vlanConfig->head.name, 0, UTT_INST_NAME_LEN);
                rst = TRUE;
            }
            break;
#endif

#if (DNS_AGENCY == FYES)
		case MIB_PROF_DNS_AGENCY:
			if (instindex == 0) {
				dnsAgency = (DnsAgencyProfile *)defprof;
				ProfSetNameByPoint(dnsAgency, "dnsAgency");
				dnsAgency->head.active = TRUE;
#if (UTT_NV_1800N == FYES)
				dnsAgency->DnsProxyEn = PROF_DISABLE;
#else
				dnsAgency->DnsProxyEn = PROF_ENABLE;
#endif
				dnsAgency->pDns = htonl(0x00000000);
				dnsAgency->sDns = htonl(0x00000000);
				rst = TRUE;
			}
			break;
#endif

#if (L2TP_SERVER == FYES)
	case MIB_PROF_L2TP_SRV_GLOBAL:
	    /*
	     * poolIpStart 192.168.44.40
	     * localIp	   192.168.44.00
	     * listenIp	   192.168.1.1
	     */
	    if (instindex == 0) {
		l2tpSProf = (L2TPSrvGlobalProfile *)defprof;
		ProfSetNameByPoint(l2tpSProf, "l2tpServer");
		l2tpSProf->head.active = FALSE;
		l2tpSProf->pppAuthMode = PPP_AUTH_ANY;
		l2tpSProf->poolIpStart = htonl(0xc0a82c28);
#if (VPN_ITEM_CONTROL == FYES)
		l2tpSProf->poolIpCnt = MAX_VPN_ITEMS_NUM;
#else
		l2tpSProf->poolIpCnt = MAX_L2TP_SRV_ACCOUNT_PROFILES;
#endif
		l2tpSProf->localIp = htonl(0xc0a82c00);
		l2tpSProf->listenIp = htonl(0xc0a80101);
		rst = TRUE;
	    }
	    break;
#endif

#if (UTT_SMART_UI == FYES)
#if (PPTP_CLIENT == FYES)
	case MIB_PROF_PPTP_CLI:
	    if (instindex == 0) {
		pptpprof = (PPTPCliProfile *)defprof;
		ProfSetNameByPoint(pptpprof, "VPN_PPTP");
		pptpprof->head.active = FALSE;
		rst = TRUE;
	    }
	    break;
#endif
#if (L2TP_CLIENT == FYES)
	case MIB_PROF_L2TP_CLIENT:
	    if (instindex == 0) {
		l2tpprof = (PPTPCliProfile *)defprof;
		ProfSetNameByPoint(l2tpprof, "VPN_L2TP");
		l2tpprof->head.active = FALSE;
		rst = TRUE;
	    }
	    break;
#endif
#endif
#if (PPTP_SERVER == FYES)
	case MIB_PROF_PPTP_SRV_GLOBAL:
	    /*
	     * poolIpStart 192.168.55.40
	     * localIp	   192.168.55.0
	     * listenIp    192.168.1.1
	     */
	    if (instindex == 0) {
		pptpSProf = (PPTPSrvGlobalProfile *)defprof;
		ProfSetNameByPoint(pptpSProf, "pptpServer");
		pptpSProf->head.active = FALSE;
		pptpSProf->pppAuthMode = PPP_AUTH_THREE_ANY;
		pptpSProf->poolIpStart = htonl(0xc0a83728);
#if (VPN_ITEM_CONTROL == FYES)
		pptpSProf->poolIpCnt = MAX_VPN_ITEMS_NUM;
#else
		pptpSProf->poolIpCnt = MAX_PPTP_SRV_ACCOUNT_PROFILES;
#endif
		pptpSProf->localIp = htonl(0xc0a83700);
		pptpSProf->listenIp = htonl(0xc0a80101);
#if (PPTP_ENCRYPT_MODE == FYES)
		strcpy(pptpSProf->EncryptionMode, "None");
#endif
		pptpSProf->mtu = 1478U;
		rst = TRUE;
	    }
	    break;
#endif

#if (DDNS == FYES)
	case MIB_PROF_DDNS:
	    if (instindex == 0) {
		ddnsProf = (DDNSProfile *)defprof;
#if (DDNS_ADVANCE != FYES)
		ProfSetNameByPoint(ddnsProf, "ddnsClient");
#endif
		ddnsProf->head.active = TRUE;
		rst = TRUE;
	    }
	    break;
#endif

#if (PPPOE_SERVER == FYES)
	case MIB_PROF_PPPOE_SRV_GLO:
	    if (instindex == 0) {
		pppoeSrvGloProf = (PPPoESrvGloProfile *)defprof;
		ProfSetNameByPoint(pppoeSrvGloProf, "PPPoEServer");
		pppoeSrvGloProf->pppAuthMode = PPP_AUTH_ANY;
		pppoeSrvGloProf->maxSessCnt = MAX_PPPOE_SRV_SESSION_PROFILES;

#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
		pppoeSrvGloProf->ipcount = MAX_PPPOE_SRV_ACC_PROFILES;
#endif

#if (PPPOE_SELF_HELP == FYES)
		pppoeSrvGloProf->self_help = FUN_DISABLE;
#endif
		rst = TRUE;
	    }
	    break;
	case MIB_PROF_PPPOE_SRV_ACC:
	    pppoeSrvAccProfile = (PPPoESrvAccProfile *)defprof;
	    pppoeSrvAccProfile->head.active = FALSE;
#if (PPPOE_SELF_HELP == FYES)
	    pppoeSrvAccProfile->optCnt = 0U;
#endif
	    rst = TRUE;
	    break;
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
	case MIB_PROF_SW_PORT_CONF:
	    swPortConfProfile = (SwPortConfProfile*)defprof;
	    swPortConfProfile->head.active = TRUE;
	    swPortConfProfile->name[0] = '\0';
#ifdef CONFIG_VSC7390
	    swPortConfProfile->max_frame = 1518u;/*max frame*/
#endif
	    swPortConfProfile->port_mode  = PORT_MODE_AUTO;/*auto negotiate*/
	    swPortConfProfile->flow_control = PROF_DISABLE;/*flow control disabled*/
	    memset(tmpStr, 0, sizeof(tmpStr));
	    sprintf(tmpStr, "port%d", instindex+1);

	    /*ÉèÖÃname¿ÉÊ¹¸÷¶Ë¿Ú¶ÔÓ¦µÄÄÚ´æÅäÖÃË÷Òý²»±ä*/
	    ProfSetNameByPoint(swPortConfProfile, tmpStr);
	    rst = TRUE;
	    break;
#endif
#if (FEATURE_PORT_VLAN == FYES)
         case MIB_PROF_PORT_VLAN:
	    portVlanProfile = (PortVlanProfile*)defprof;  
	    memset(portVlanProfile, 0, sizeof(PortVlanProfile));
	    portVlanProfile->head.active = FALSE;
	    memset(tmpStr, 0, sizeof(tmpStr));
	    sprintf(tmpStr, "pvlan%d", instindex+1);
	    strncpy(portVlanProfile->head.name, tmpStr, UTT_INST_NAME_LEN);
	    rst = TRUE;

            break;
#endif
#if (PVLAN_REDUCED == FYES)
         case MIB_PROF_RDD_PORT_VLAN:
	    rddPortVlanProfile = (RddPortVlanProfile*)defprof;  
	    memset(rddPortVlanProfile, 0, sizeof(RddPortVlanProfile));
	    rddPortVlanProfile->head.active = FALSE;
	    memset(tmpStr, 0, sizeof(tmpStr));
	    sprintf(tmpStr, "pvlan%d", instindex+1);
	    strncpy(rddPortVlanProfile->head.name, tmpStr, UTT_INST_NAME_LEN);
	    rst = TRUE;

            break;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
		case MIB_PROF_MIRROR:
			if (instindex == 0) {
				mirrorProfileDef = (MirrorProfile *)defprof;
				strcpy(mirrorProfileDef->head.name, "mirror");
				mirrorProfileDef->head.active = TRUE;
				mirrorProfileDef->mirrorPort = 1u;
				memset(mirrorProfileDef->monitoredPort, 0, sizeof(mirrorProfileDef->monitoredPort));
				mirrorProfileDef->mirrorMode = MIRROR_DISABLE;
				rst = TRUE;
			}
			break;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
		case MIB_PROF_TRUNK:
				trunkProfileDef = (TrunkProfile *)defprof;
				memset(tmpStr, 0, sizeof(tmpStr));
				sprintf(tmpStr, "t%d", instindex+1);
				strncpy(trunkProfileDef->head.name, tmpStr, UTT_INST_NAME_LEN);
				trunkProfileDef->head.active = FALSE;
				trunkProfileDef->tMode = TRUNK_MODE;
				trunkProfileDef->tGroup = instindex+1;
				memset(trunkProfileDef->tName, '\0',sizeof(trunkProfileDef->tName));
				memset(trunkProfileDef->tMembers, 0, sizeof(trunkProfileDef->tMembers));
				rst = TRUE;
				break;
#endif
#if (FEATURE_SW_SNMP == FYES)
		case MIB_PROF_SNMP:
			if (instindex == 0) {
				snmpProfileDef = (SnmpProfile *)defprof;
				snmpProfileDef->head.active = FALSE;
				strcpy(snmpProfileDef->head.name, "snmp");
				snmpProfileDef->snmp_enable = 0u;
				strncpy(snmpProfileDef->trap_ip,"0.0.0.0",MAX_SNMP_COMMUNITY_LEN+4);
				strncpy(snmpProfileDef->r_community,"public",MAX_SNMP_COMMUNITY_LEN);
				strncpy(snmpProfileDef->w_community,"private",MAX_SNMP_COMMUNITY_LEN);
				strncpy(snmpProfileDef->trap_community,"public",MAX_SNMP_COMMUNITY_LEN);
				rst = TRUE;
			}
			break;
#endif
#if (SNMP == FYES)
              case MIB_PROF_SNMP_INFO:
                      if (instindex == 0) {
                              snmpInfoProfileDef = (SnmpInfoProfile *)defprof;
                              snmpInfoProfileDef->head.active = TRUE;
                              strcpy(snmpInfoProfileDef->head.name, "snmp");
                              snmpInfoProfileDef->snmp_enable = 0u;
                              strncpy(snmpInfoProfileDef->community,"uTt22aA",MAX_SNMP_COMMUNITY_LEN);
                              snmpInfoProfileDef->host_manage_enable = 0u;
                              strncpy(snmpInfoProfileDef->host1,"0.0.0.0",16);
                              strncpy(snmpInfoProfileDef->host2,"0.0.0.0",16);
                              strncpy(snmpInfoProfileDef->host3,"0.0.0.0",16);
                              rst = TRUE;
                      }
                      break;
#endif
#if (FEATURE_ACL == FYES)
        case MIB_PROF_ACL:
            aclProfile = (AclProfile *)defprof;  
	    memset(aclProfile, 0, sizeof(AclProfile));
	    aclProfile->head.active = FALSE;
	    memset(tmpStr, 0, sizeof(tmpStr));
	    sprintf(tmpStr, "acl%d", instindex+1);
	    strncpy(aclProfile->head.name, tmpStr, UTT_INST_NAME_LEN);
            rst = TRUE;
            break;
        case MIB_PROF_ACL_STATUS:
            aclStatusProfile = (AclStatusProfile *)defprof;  
	    memset(aclStatusProfile, 0, sizeof(AclStatusProfile));
	    strncpy(aclStatusProfile->head.name, "aclEnable", UTT_INST_NAME_LEN);
            rst = TRUE;
	    break;
#endif

#if 0
	    /*mantis11610*/
#if (STATIC_NAT == FYES)
	case MIB_PROF_STATIC_NAT:
	    if (instindex == 0) {/*³ö³§Ê±Ìí¼ÓÔ¶³Ì¹ÜÀíµÄÓ³Éä£¬Ä¬ÈÏ¹Ø±Õ*/
		staticNatProfile = (StaticNatProfile *)defprof;
		staticNatProfile->head.active = FALSE;
		ProfSetNameByPoint(staticNatProfile, "admin");
		staticNatProfile->OutPort= 8081;
		staticNatProfile->protocol= 1;
		staticNatProfile->IP = htonl(0xc0a80101);
		staticNatProfile->InnerPort= 80;
		staticNatProfile->PortNum= 1;
		strcpy(staticNatProfile->NatBind, "WAN1");
		rst = TRUE;
	    }    
	    break;
#endif
#endif

#if (ADMIN_CONF == FYES)
	case MIB_PROF_ADMIN_CONF:
		if (instindex == 0) {
		adminConfProfile = (AdminConfProfile *)defprof;
		adminConfProfile->head.active = TRUE;
		strcpy(adminConfProfile->head.name, "admin");
#if (RANDOM_PASS == FYES)
        sprintf(adminConfProfile->password,"%02x%02x%02x", (0xFF & (tmpsn >> 16)),(0xFF & (tmpsn >> 8)),(0xFF & (tmpsn)));
#else
        strcpy(adminConfProfile->password, "admin");
#endif
        rst = TRUE;
		}
	break;
#endif

#if (DNS_FILTER == FYES)
	case MIB_PROF_DNS_FILTER:
		if (instindex == 0) {
		dnsFilterProfile = (DnsFilterProfile *)defprof;
		dnsFilterProfile->head.active = 0;
		memset(dnsFilterProfile->domainName, '\0', sizeof(dnsFilterProfile->domainName));
		rst = TRUE;
		}
	break;
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    case MIB_PROF_CLI_FOR_ENGINEER_ONLY:
        if (instindex == 0){
            cliForEngineerOnlyProfile = (CliForEngineerOnlyProfile *)defprof;
            cliForEngineerOnlyProfile->head.active = 0;
            memset(cliForEngineerOnlyProfile->command, 0, sizeof(cliForEngineerOnlyProfile->command));
            rst = TRUE;
        }
        break;
#endif

#if (WIRELESS == FYES)
	case MIB_PROF_WIRELESS:
		if (instindex == 0) {
		    wirelessProfile = (WirelessProfile*)defprof;
		    memset(tmpStr, '\0', sizeof(tmpStr));
#if (FEATURE_AP == FYES)
		    sprintf(tmpStr, "wireless%d", instindex);
		    ProfSetNameByPoint(wirelessProfile,tmpStr);
#else
		    ProfSetNameByPoint((void*)wirelessProfile,"wireless");
#endif
		    /* »ù±¾ÅäÖÃ³õÊ¼»¯ */
#if (WRITE_NEWMAC == FYES)
            sprintf(wirelessProfile->mBaseCf.SSID1,"CMCC-2.4G-%02X%02X", newmac[4],newmac[5]);
#endif
		    wirelessProfile->head.active = TRUE;
		    wirelessProfile->mBaseCf.apWorkMode = 1;
		    wirelessProfile->mBaseCf.WirelessMode = 9;
		    wirelessProfile->mBaseCf.SSIDBroadCastEn = 1;
		    wirelessProfile->mBaseCf.Channel = 0;
#if (FEATURE_AP == FYES)
		    wirelessProfile->mBaseCf.BYPASS = 0;
#endif
		    wirelessProfile->mBaseCf.ChanWidth = 0;
			wirelessProfile->mBaseCf.OperationMode = 1;
		    strcpy(wirelessProfile->mBaseCf.SelAuthMode,"NONE");
		    strcpy(wirelessProfile->mBaseCf.apClient.ApCliSsid,"UTT-HIPER");
		    strcpy(wirelessProfile->mBaseCf.apClient.wdsSwitch,"off");
		    wirelessProfile->mBaseCf.apWds.wdsEnable = 1;
		    strcpy(wirelessProfile->mBaseCf.apWds.wdsEncrypType, "NONE");
		    strcpy(wirelessProfile->mBaseCf.apWds.bridgeSSID1, "");
		    char apClientMac[18]; 
#ifdef CONFIG_UTT_MAC_FC2FEF
		    strcpy(apClientMac,"fc2fef112233");
#else
		    strcpy(apClientMac,"0022aa112233");
#endif
		    converMac12To6Byte(apClientMac, &(wirelessProfile->mBaseCf.apClient.APCliMacs));
#if (FEATURE_AP == FYES)
		    wirelessProfile->fitAp.rate = 0u;
		    wirelessProfile->fitAp.autoPower = 0u;
		    wirelessProfile->fitAp.manPower = 1u;
#endif
		    /* °²È«ÅäÖÃ³õÊ¼Öµ*/
		    wirelessProfile->mSafeCf.ap.AuthMode.RekeyInterval = 3600;
		    memset(wirelessProfile->mSafeCf.ap.AuthMode.pskPsswd,'\0',UTT_PSK_PASSWD_LEN+1);
#if (MSG_WR201 == FYES)
            strcpy(wirelessProfile->mSafeCf.ap.AuthMode.pskPsswd,"88888888");
            strcpy(wirelessProfile->mSafeCf.SelAuthMode,"WPAPSKWPA2PSK");
            strcpy(wirelessProfile->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
#else 
		    strcpy(wirelessProfile->mSafeCf.SelAuthMode,"OPEN");
		    strcpy(wirelessProfile->mSafeCf.ap.AuthMode.EncrypType,"NONE");
#endif
		    wirelessProfile->mSafeCf.ap.AuthMode.wepKeyID = 1;
		    wirelessProfile->mSafeCf.ap.AuthMode.RADIUS_Port = 1812;
#if (UTT_SSID2_ENCRYPTION == FYES)
		    wirelessProfile->mSafeCf.ap.AuthMode_2.RekeyInterval = 3600;
		    memset(wirelessProfile->mSafeCf.ap.AuthMode_2.pskPsswd,'\0',UTT_PSK_PASSWD_LEN+1);
		    strcpy(wirelessProfile->mSafeCf.SelAuthMode_2,"OPEN");
		    strcpy(wirelessProfile->mSafeCf.ap.AuthMode_2.EncrypType,"NONE");
		    wirelessProfile->mSafeCf.ap.AuthMode_2.wepKeyID = 1;
		    wirelessProfile->mSafeCf.ap.AuthMode_2.RADIUS_Port = 1812;
#endif

		    /* ¸ß¼¶ÅäÖÃ³õÊ¼Öµ*/
		    wirelessProfile->advance.rts = 2347;
		    wirelessProfile->advance.section = 2346;
		    wirelessProfile->advance.beacon = 100;
		    wirelessProfile->advance.dtim = 1;
		    wirelessProfile->advance.TxBurst = 1;
		    wirelessProfile->advance.preamble = 1;
		    wirelessProfile->advance.wmm = 1;

		    wirelessProfile->mFltGlb.filterEn = FUN_DISABLE;
		    wirelessProfile->mFltGlb.action = AP_MAC_ALLOW;

		    /* Ralink ÐèÒªµÄÒ»Ð©³ö³§ÅäÖÃ²ÎÊý */
#ifdef CONFIG_USER_GOAHEAD_LANG_EN
		    wirelessProfile->mBaseCf.CountryRegion = 1; /*Ä¬ÈÏÅ·±ê*/
		    wirelessProfile->mBaseCf.CountryRegionABand = 0;/*Ä¬ÈÏÇé¿öÏÂ£¬CountryRegionABand=10´ú±íÃÀ¹ú,Í¬Ò³ÃæÒ»ÖÂ*/
#else
#if(BRAND_INDIA == FYES)
		    wirelessProfile->mBaseCf.CountryRegion = 1;
#elif (MIDDLE_SOFT == FYES)
		    wirelessProfile->mBaseCf.CountryRegion = 0;
#endif
#if (UTT_5G_CHANNEL_149_TO_165 == FYES)
		    wirelessProfile->mBaseCf.CountryRegionABand = 4;
#else
			wirelessProfile->mBaseCf.CountryRegionABand = 10;
#endif
#endif
		    wirelessProfile->mBaseCf.AutoChannelSelect = 2;
#if (WLAN_MULTI_SSID == FYES)
#if (MAKE_MONEY == FYES)
                    wirelessProfile->mBaseCf.BssidNum = 4;
#elif (MIDDLE_SOFT == FYES)
                    wirelessProfile->mBaseCf.BssidNum = 4;
#else
		    wirelessProfile->mBaseCf.BssidNum = 2;
#endif
#else
		    wirelessProfile->mBaseCf.BssidNum = 1;
#endif
		    wirelessProfile->mBaseCf.BasicRate = 15;
#if (WIRELESS_TURBO == FYES)
                    wirelessProfile->mBaseCf.TxPower = 60;
#else
                    wirelessProfile->mBaseCf.TxPower = TXPOWER_VALUE;
#endif
		    wirelessProfile->mBaseCf.PktAggregate = 1;
		    strcpy(wirelessProfile->mBaseCf.APAifsn,"3;7;1;1");
		    strcpy(wirelessProfile->mBaseCf.APCwmin,"4;4;3;2");
		    strcpy(wirelessProfile->mBaseCf.APCwmax ,"6;10;4;3");
		    strcpy(wirelessProfile->mBaseCf.APTxop ,"0;0;94;47");
		    strcpy(wirelessProfile->mBaseCf.APACM ,"0;0;0;0");
		    strcpy(wirelessProfile->mBaseCf.BSSAifsn ,"3;7;2;2");
		    strcpy(wirelessProfile->mBaseCf.BSSCwmin ,"4;4;3;2");
		    strcpy(wirelessProfile->mBaseCf.BSSCwmax ,"10;10;4;3");
		    strcpy(wirelessProfile->mBaseCf.BSSTxop ,"0;0;94;47");
		    strcpy(wirelessProfile->mBaseCf.BSSACM ,"0;0;0;0");
		    strcpy(wirelessProfile->mBaseCf.AckPolicy ,"0;0;0;0");
		    wirelessProfile->mBaseCf.APSDCapable = 0;
		    wirelessProfile->mBaseCf.DLSCapable = 0;
		    wirelessProfile->mBaseCf.NoForwarding = 0;
		    wirelessProfile->mBaseCf.ShortSlot = 1;
		    wirelessProfile->mBaseCf.CSPeriod = 6;
		    wirelessProfile->mBaseCf.PreAuth = 0;
		    strcpy(wirelessProfile->mBaseCf.RekeyMethod,"DISABLE");
		    wirelessProfile->mBaseCf.PMKCachePeriod = 10;
		    wirelessProfile->mBaseCf.HT_HTC = 1;
		    wirelessProfile->mBaseCf.HT_RDG = 1;
		    wirelessProfile->mBaseCf.HT_MpduDensity = 5;
		    wirelessProfile->mBaseCf.HT_EXTCHA = 1;
		    wirelessProfile->mBaseCf.HT_AutoBA = 1;
		    wirelessProfile->mBaseCf.HT_BAWinSize = 64;
		    wirelessProfile->mBaseCf.HT_GI = 1;
		    wirelessProfile->mBaseCf.HT_STBC = 1;
		    wirelessProfile->mBaseCf.HT_MCS = 33;
		    wirelessProfile->mBaseCf.HT_BSSCoexistence = 0;
		    wirelessProfile->mBaseCf.AutoProvisionEn = 0;
#if (UTT_WA_500N==FYES || UTT_WA_501N==FYES || UTT_AP_801 == FYES)
		    wirelessProfile->mBaseCf.HT_TxStream = 1;
		    wirelessProfile->mBaseCf.HT_RxStream = 1;
		    wirelessProfile->mBaseCf.FreqDelta = 0;
		    wirelessProfile->mBaseCf.ITxBfEn = 0;
		    wirelessProfile->mBaseCf.PreAntSwitch = 0;
		    wirelessProfile->mBaseCf.PhyRateLimit = 0;
		    wirelessProfile->mBaseCf.DebugFlags = 0;
		    wirelessProfile->mBaseCf.ETxBfEnCond = 0;
		    wirelessProfile->mBaseCf.ITxBfTimeout = 0;
		    wirelessProfile->mBaseCf.ETxBfTimeout = 0;
		    wirelessProfile->mBaseCf.ETxBfNoncompress = 0;
		    wirelessProfile->mBaseCf.ETxBfIncapable= 0;
		    wirelessProfile->mBaseCf.FineAGC = 0;
		    wirelessProfile->mBaseCf.StreamMode = 0;
		    wirelessProfile->mBaseCf.DfsOutdoor = 0;
		    wirelessProfile->mBaseCf.SymRoundFromCfg = 0;
		    wirelessProfile->mBaseCf.BusyIdleFromCfg = 0;
		    wirelessProfile->mBaseCf.DfsRssiHighFromCfg = 0;
		    wirelessProfile->mBaseCf.DfsRssiLowFromCfg = 0;
		    wirelessProfile->mBaseCf.DFSParamFromConfig = 0;
		    wirelessProfile->mBaseCf.HT_DisallowTKIP = 0;
		    wirelessProfile->mBaseCf.GreenAP = 0;
		    wirelessProfile->mBaseCf.WCNTest = 0;
#else
		    wirelessProfile->mBaseCf.HT_TxStream = 2;
		    wirelessProfile->mBaseCf.HT_RxStream = 2;
#endif
		    wirelessProfile->mBaseCf.HT_PROTECT = 0;
		    wirelessProfile->mBaseCf.RADIUS_Acct_Port = 1813;
#if (UTT_SMART_UI == FYES || WLAN_MULTI_SSID == FYES)
#if (MIDDLE_SOFT == FYES)
		    strncpy(wirelessProfile->mBaseCf.encodeType,"UTF-8",7);
#else
		    strncpy(wirelessProfile->mBaseCf.encodeType,"GBK",7);
#endif
#if (WLAN_MULTI_SSID == FYES)
#if (MIDDLE_SOFT == FYES)
            strncpy(wirelessProfile->mBaseCf.encodeType2,"UTF-8",7);
            strncpy(wirelessProfile->mBaseCf.encodeType3,"UTF-8",7);
            strncpy(wirelessProfile->mBaseCf.encodeType4,"UTF-8",7);
#else
            strncpy(wirelessProfile->mBaseCf.encodeType2,"GBK",7);
#endif
#endif
#endif
#if (WLAN_WORK_TIME == FYES)
#if (UTT_SMART_UI == FYES)
		    wirelessProfile->mBaseCf.WlanTimeEn = FUN_ENABLE;
#endif
		    wirelessProfile->mBaseCf.day = 127U;
		    strcpy(wirelessProfile->mBaseCf.weekdayTimeStart, "00:00");
		    strcpy(wirelessProfile->mBaseCf.weekdayTimeStop, "23:59");
		    strcpy(wirelessProfile->mBaseCf.weekendTimeStart, "00:00");
		    strcpy(wirelessProfile->mBaseCf.weekendTimeStop, "23:59");
#endif
#if (WIRELESS_TURBO == FYES)
                    wirelessProfile->turbo = 0;
#endif
		    rst = TRUE; 
		}

#if (WIRELESS_5G == FYES)
		if (instindex == 1) {
		    wirelessProfile = (WirelessProfile*)defprof;
		    memset(tmpStr, '\0', sizeof(tmpStr));
		    ProfSetNameByPoint((void*)wirelessProfile,"wireless5g");
		    /* »ù±¾ÅäÖÃ³õÊ¼»¯ */
		    wirelessProfile->head.active = TRUE;
		    wirelessProfile->mBaseCf.apWorkMode = 1;
		    wirelessProfile->mBaseCf.WirelessMode = 8;
		    wirelessProfile->mBaseCf.SSIDBroadCastEn = 1;
		    wirelessProfile->mBaseCf.Channel = 0;
#if(BRAND_INDIA == FYES || BRAND_LEVEL_ONE == FYES)
		    wirelessProfile->mBaseCf.Country = 6;/*6ÊÇ¹ú¼ÒÂë£¬Ä¬ÈÏµÂ¹ú*/
#else
		    wirelessProfile->mBaseCf.Country = 0;/*6ÊÇ¹ú¼ÒÂë£¬Ä¬ÈÏµÂ¹ú*/
#endif
#if (AUTO_CHANNEL == FYES)
            wirelessProfile->mBaseCf.ChanWidth = 1;
#else
            wirelessProfile->mBaseCf.ChanWidth = 0;
#endif
            wirelessProfile->mBaseCf.VChanWidth = 0;
		    wirelessProfile->mBaseCf.OperationMode = 1;
		    strcpy(wirelessProfile->mBaseCf.SelAuthMode,"NONE");
		    strcpy(wirelessProfile->mBaseCf.apClient.ApCliSsid,"UTT-HIPER");
		    wirelessProfile->mBaseCf.apWds.wdsEnable = 0;
		    strcpy(wirelessProfile->mBaseCf.apWds.wdsEncrypType, "NONE");
		    strcpy(wirelessProfile->mBaseCf.apWds.bridgeSSID1, "");
		    char apClientMac[18]; 
#ifdef CONFIG_UTT_MAC_FC2FEF
		    strcpy(apClientMac,"fc2fef112233");
#else
		    strcpy(apClientMac,"0022aa112233");
#endif
		    converMac12To6Byte(apClientMac, &(wirelessProfile->mBaseCf.apClient.APCliMacs));
		    /* °²È«ÅäÖÃ³õÊ¼Öµ*/
		    wirelessProfile->mSafeCf.ap.AuthMode.RekeyInterval = 3600;
		    memset(wirelessProfile->mSafeCf.ap.AuthMode.pskPsswd,'\0',UTT_PSK_PASSWD_LEN+1);
#if (MSG_WR201 == FYES)
            strcpy(wirelessProfile->mSafeCf.ap.AuthMode.pskPsswd,"88888888");
            strcpy(wirelessProfile->mSafeCf.SelAuthMode,"WPAPSKWPA2PSK");
            strcpy(wirelessProfile->mSafeCf.ap.AuthMode.EncrypType,"TKIPAES");
#else 
		    strcpy(wirelessProfile->mSafeCf.SelAuthMode,"OPEN");
		    strcpy(wirelessProfile->mSafeCf.ap.AuthMode.EncrypType,"NONE");
#endif
		    wirelessProfile->mSafeCf.ap.AuthMode.wepKeyID = 1;
		    wirelessProfile->mSafeCf.ap.AuthMode.RADIUS_Port = 1812;
#if (UTT_SSID2_ENCRYPTION == FYES)
		    wirelessProfile->mSafeCf.ap.AuthMode_2.RekeyInterval = 3600;
		    memset(wirelessProfile->mSafeCf.ap.AuthMode_2.pskPsswd,'\0',UTT_PSK_PASSWD_LEN+1);
		    strcpy(wirelessProfile->mSafeCf.SelAuthMode_2,"OPEN");
		    strcpy(wirelessProfile->mSafeCf.ap.AuthMode_2.EncrypType,"NONE");
		    wirelessProfile->mSafeCf.ap.AuthMode_2.wepKeyID = 1;
		    wirelessProfile->mSafeCf.ap.AuthMode_2.RADIUS_Port = 1812;
#endif

		    /* ¸ß¼¶ÅäÖÃ³õÊ¼Öµ*/
		    wirelessProfile->advance.rts = 2347;
		    wirelessProfile->advance.section = 2346;
		    wirelessProfile->advance.beacon = 100;
		    wirelessProfile->advance.dtim = 1;
		    wirelessProfile->advance.TxBurst = 1;
		    wirelessProfile->advance.preamble = 1;
		    wirelessProfile->advance.wmm = 1;

		    wirelessProfile->mFltGlb.filterEn = FUN_DISABLE;
		    wirelessProfile->mFltGlb.action = AP_MAC_ALLOW;

		    /* Ralink ÐèÒªµÄÒ»Ð©³ö³§ÅäÖÃ²ÎÊý */
#if(BRAND_INDIA == FYES)           
		    wirelessProfile->mBaseCf.CountryRegion = 1;
#elif (AUTO_CHANNEL == FYES)
            wirelessProfile->mBaseCf.CountryRegion = 4; 
#else
		    wirelessProfile->mBaseCf.CountryRegion = 0;
#endif

#ifdef CONFIG_USER_GOAHEAD_LANG_EN

#if(BRAND_LEVEL_ONE == FYES || BRAND_INDIA == FYES)
		    wirelessProfile->mBaseCf.CountryRegionABand = 6;
#else
		    wirelessProfile->mBaseCf.CountryRegionABand = 10;/*Ä¬ÈÏÇé¿öÏÂ£¬CountryRegionABand=10´ú±íÃÀ¹ú,Í¬Ò³ÃæÒ»ÖÂ*/
#endif

#else
#if (UTT_5G_CHANNEL_149_TO_165 == FYES)
		    wirelessProfile->mBaseCf.CountryRegionABand = 4;
#else
#if(UTT_5G_CHANNEL_52_TO_64 == FYES)
                    wirelessProfile->mBaseCf.CountryRegionABand = 4;
#else
	            wirelessProfile->mBaseCf.CountryRegionABand = 10;
#endif
#endif
#endif
		    wirelessProfile->mBaseCf.AutoChannelSelect = 2;
#if (WLAN_MULTI_SSID == FYES)
#if (MAKE_MONEY == FYES)
                    wirelessProfile->mBaseCf.BssidNum = 4;
#else
		    wirelessProfile->mBaseCf.BssidNum = 2;
#endif
#else
		    wirelessProfile->mBaseCf.BssidNum = 1;
#endif
		    wirelessProfile->mBaseCf.BasicRate = 15;
#if (WIRELESS_TURBO == FYES)
		    wirelessProfile->mBaseCf.TxPower = 60;
#else

			wirelessProfile->mBaseCf.TxPower = TXPOWER_VALUE;
#endif
		    wirelessProfile->mBaseCf.PktAggregate = 1;
		    strcpy(wirelessProfile->mBaseCf.APAifsn,"3;7;1;1");
		    strcpy(wirelessProfile->mBaseCf.APCwmin,"4;4;3;2");
		    strcpy(wirelessProfile->mBaseCf.APCwmax ,"6;10;4;3");
		    strcpy(wirelessProfile->mBaseCf.APTxop ,"0;0;94;47");
		    strcpy(wirelessProfile->mBaseCf.APACM ,"0;0;0;0");
		    strcpy(wirelessProfile->mBaseCf.BSSAifsn ,"3;7;2;2");
		    strcpy(wirelessProfile->mBaseCf.BSSCwmin ,"4;4;3;2");
		    strcpy(wirelessProfile->mBaseCf.BSSCwmax ,"10;10;4;3");
		    strcpy(wirelessProfile->mBaseCf.BSSTxop ,"0;0;94;47");
		    strcpy(wirelessProfile->mBaseCf.BSSACM ,"0;0;0;0");
		    strcpy(wirelessProfile->mBaseCf.AckPolicy ,"0;0;0;0");
		    wirelessProfile->mBaseCf.APSDCapable = 0;
		    wirelessProfile->mBaseCf.DLSCapable = 0;
		    wirelessProfile->mBaseCf.NoForwarding = 0;
		    wirelessProfile->mBaseCf.ShortSlot = 1;
		    wirelessProfile->mBaseCf.CSPeriod = 6;
		    wirelessProfile->mBaseCf.PreAuth = 0;
		    strcpy(wirelessProfile->mBaseCf.RekeyMethod,"DISABLE");
		    wirelessProfile->mBaseCf.PMKCachePeriod = 10;
		    wirelessProfile->mBaseCf.HT_HTC = 1;
		    wirelessProfile->mBaseCf.HT_RDG = 1;
		    wirelessProfile->mBaseCf.HT_MpduDensity = 5;
		    wirelessProfile->mBaseCf.HT_EXTCHA = 1;
		    wirelessProfile->mBaseCf.HT_AutoBA = 1;
		    wirelessProfile->mBaseCf.HT_BAWinSize = 64;
		    wirelessProfile->mBaseCf.HT_GI = 1;
		    wirelessProfile->mBaseCf.HT_STBC = 1;
		    wirelessProfile->mBaseCf.HT_MCS = 33;
#if (AUTO_CHANNEL == FYES)
            wirelessProfile->mBaseCf.HT_BSSCoexistence = 1;
#else
            wirelessProfile->mBaseCf.HT_BSSCoexistence = 0;
#endif
            wirelessProfile->mBaseCf.AutoProvisionEn = 0;
		    wirelessProfile->mBaseCf.FreqDelta = 0;
		    wirelessProfile->mBaseCf.ITxBfEn = 0;
		    wirelessProfile->mBaseCf.PreAntSwitch = 1;
		    wirelessProfile->mBaseCf.PhyRateLimit = 0;
		    wirelessProfile->mBaseCf.DebugFlags = 0;
		    wirelessProfile->mBaseCf.ETxBfEnCond = 0;
		    wirelessProfile->mBaseCf.ITxBfTimeout = 0;
		    wirelessProfile->mBaseCf.ETxBfTimeout = 0;
		    wirelessProfile->mBaseCf.ETxBfNoncompress = 0;
		    wirelessProfile->mBaseCf.ETxBfIncapable= 0;
		    wirelessProfile->mBaseCf.FineAGC = 0;
		    wirelessProfile->mBaseCf.StreamMode = 0;
		    wirelessProfile->mBaseCf.DfsOutdoor = 0;
		    wirelessProfile->mBaseCf.SymRoundFromCfg = 0;
		    wirelessProfile->mBaseCf.BusyIdleFromCfg = 0;
		    wirelessProfile->mBaseCf.DfsRssiHighFromCfg = 0;
		    wirelessProfile->mBaseCf.DfsRssiLowFromCfg = 0;
		    wirelessProfile->mBaseCf.DFSParamFromConfig = 0;
		    wirelessProfile->mBaseCf.HT_DisallowTKIP = 1;
		    wirelessProfile->mBaseCf.WCNTest = 0;
		    wirelessProfile->mBaseCf.GreenAP = 0;

#ifdef CONFIG_RALINK_MT7612E
		    wirelessProfile->mBaseCf.HT_TxStream = 2;
		    wirelessProfile->mBaseCf.HT_RxStream = 2;
#else
		    wirelessProfile->mBaseCf.HT_TxStream = 1;
		    wirelessProfile->mBaseCf.HT_RxStream = 1;
#endif
		    wirelessProfile->mBaseCf.HT_PROTECT = 0;
		    wirelessProfile->mBaseCf.RADIUS_Acct_Port = 1813;
#if (UTT_SMART_UI == FYES || WLAN_MULTI_SSID == FYES)
#if (MIDDLE_SOFT == FYES)
		    strncpy(wirelessProfile->mBaseCf.encodeType,"UTF-8",7);
#else
		    strncpy(wirelessProfile->mBaseCf.encodeType,"GBK",7);
#endif
#if (WLAN_MULTI_SSID == FYES)
#if (MIDDLE_SOFT == FYES)
            strncpy(wirelessProfile->mBaseCf.encodeType2,"UTF-8",7);
#else
		    strncpy(wirelessProfile->mBaseCf.encodeType2,"GBK",7);
#endif
#endif
#endif

#if (WLAN_WORK_TIME == FYES)
#if (UTT_SMART_UI == FYES)
		    wirelessProfile->mBaseCf.WlanTimeEn = FUN_DISABLE;
#endif
		    wirelessProfile->mBaseCf.day = 127U;
		    strcpy(wirelessProfile->mBaseCf.weekdayTimeStart, "00:00");
		    strcpy(wirelessProfile->mBaseCf.weekdayTimeStop, "23:59");
		    strcpy(wirelessProfile->mBaseCf.weekendTimeStart, "00:00");
		    strcpy(wirelessProfile->mBaseCf.weekendTimeStop, "23:59");
#endif

		    rst = TRUE;
		}
#if (FIT_AP_SERVICE_ZONE == FYES)
		if (instindex >= 2)
		{
		    wirelessProfile = (WirelessProfile*)defprof;
		    wirelessProfile->head.active = FALSE;
		    sprintf(tmpStr, "wireless%d", instindex);
		    ProfSetNameByPoint((void*)wirelessProfile, tmpStr);
		    rst = TRUE;
		}
#endif
#endif
#if 0
		else
		{
		    wirelessProfile = (WirelessProfile*)defprof;
		    /* »ù±¾ÅäÖÃ³õÊ¼»¯ */
		    wirelessProfile->head.active = FALSE;
		    wirelessProfile->mBaseCf.apWorkMode = 1;
		    wirelessProfile->mBaseCf.WirelessMode = 9;
		    wirelessProfile->mBaseCf.SSIDBroadCastEn = 1;
		    wirelessProfile->mBaseCf.Channel = 6;
#if (FEATURE_AP == FYES)
		    wirelessProfile->mBaseCf.BYPASS = 0;
#endif
		    wirelessProfile->mBaseCf.ChanWidth = 0;
		    strcpy(wirelessProfile->mBaseCf.SelAuthMode,"NONE");
		    strcpy(wirelessProfile->mBaseCf.apClient.ApCliSsid,"UTT-HIPER");
		    wirelessProfile->mBaseCf.apWds.wdsEnable = 1;
		    strcpy(wirelessProfile->mBaseCf.apWds.wdsEncrypType, "NONE");
		    char apClientMac[18]; 
		    strcpy(apClientMac,"0022aa112233");
		    converMac12To6Byte(apClientMac, &(wirelessProfile->mBaseCf.apClient.APCliMacs));

		    wirelessProfile->fitAp.rate = 0u;
		    wirelessProfile->fitAp.autoPower = 0u;
		    wirelessProfile->fitAp.manPower = 2u;

		    /* °²È«ÅäÖÃ³õÊ¼Öµ*/
		    wirelessProfile->mSafeCf.ap.AuthMode.RekeyInterval = 3600;
		    memset(wirelessProfile->mSafeCf.ap.AuthMode.pskPsswd,'\0',UTT_PSK_PASSWD_LEN+1);
		    strcpy(wirelessProfile->mSafeCf.SelAuthMode,"OPEN");
		    strcpy(wirelessProfile->mSafeCf.ap.AuthMode.EncrypType,"NONE");
		    wirelessProfile->mSafeCf.ap.AuthMode.wepKeyID = 1;
		    wirelessProfile->mSafeCf.ap.AuthMode.RADIUS_Port = 1812;

		    /* ¸ß¼¶ÅäÖÃ³õÊ¼Öµ*/
		    wirelessProfile->advance.rts = 2347;
		    wirelessProfile->advance.section = 2346;
		    wirelessProfile->advance.beacon = 100;
		    wirelessProfile->advance.dtim = 1;
		    wirelessProfile->advance.TxBurst = 1;
		    wirelessProfile->advance.preamble = 1;
		    wirelessProfile->advance.wmm = 1;

		    wirelessProfile->mFltGlb.filterEn = FUN_DISABLE;
		    wirelessProfile->mFltGlb.action = AP_MAC_ALLOW;

		    /* Ralink ÐèÒªµÄÒ»Ð©³ö³§ÅäÖÃ²ÎÊý */
		    wirelessProfile->mBaseCf.CountryRegion = 5;
		    wirelessProfile->mBaseCf.CountryRegionABand = 7;
		    wirelessProfile->mBaseCf.BssidNum = 1;
		    wirelessProfile->mBaseCf.BasicRate = 15;
		    wirelessProfile->mBaseCf.TxPower = 100;
		    wirelessProfile->mBaseCf.PktAggregate = 1;
		    strcpy(wirelessProfile->mBaseCf.APAifsn,"3;7;1;1");
		    strcpy(wirelessProfile->mBaseCf.APCwmin,"4;4;3;2");
		    strcpy(wirelessProfile->mBaseCf.APCwmax ,"6;10;4;3");
		    strcpy(wirelessProfile->mBaseCf.APTxop ,"0;0;94;47");
		    strcpy(wirelessProfile->mBaseCf.APACM ,"0;0;0;0");
		    strcpy(wirelessProfile->mBaseCf.BSSAifsn ,"3;7;2;2");
		    strcpy(wirelessProfile->mBaseCf.BSSCwmin ,"4;4;3;2");
		    strcpy(wirelessProfile->mBaseCf.BSSCwmax ,"10;10;4;3");
		    strcpy(wirelessProfile->mBaseCf.BSSTxop ,"0;0;94;47");
		    strcpy(wirelessProfile->mBaseCf.BSSACM ,"0;0;0;0");
		    strcpy(wirelessProfile->mBaseCf.AckPolicy ,"0;0;0;0");
		    wirelessProfile->mBaseCf.APSDCapable = 0;
		    wirelessProfile->mBaseCf.DLSCapable = 0;
		    wirelessProfile->mBaseCf.NoForwarding = 0;
		    wirelessProfile->mBaseCf.ShortSlot = 1;
		    wirelessProfile->mBaseCf.CSPeriod = 6;
		    wirelessProfile->mBaseCf.PreAuth = 0;
		    strcpy(wirelessProfile->mBaseCf.RekeyMethod,"DISABLE");
		    wirelessProfile->mBaseCf.PMKCachePeriod = 10;
		    wirelessProfile->mBaseCf.HT_HTC = 1;
		    wirelessProfile->mBaseCf.HT_RDG = 1;
		    wirelessProfile->mBaseCf.HT_MpduDensity = 5;
		    wirelessProfile->mBaseCf.HT_EXTCHA = 1;
		    wirelessProfile->mBaseCf.HT_AutoBA = 1;
		    wirelessProfile->mBaseCf.HT_BAWinSize = 64;
		    wirelessProfile->mBaseCf.HT_GI = 1;
		    wirelessProfile->mBaseCf.HT_STBC = 1;
		    wirelessProfile->mBaseCf.HT_MCS = 33;
		    wirelessProfile->mBaseCf.HT_TxStream = 2;
		    wirelessProfile->mBaseCf.HT_RxStream = 2;
		    wirelessProfile->mBaseCf.HT_PROTECT = 0;
		    wirelessProfile->mBaseCf.RADIUS_Acct_Port = 1813;

		    memset(tmpStr, '\0', sizeof(tmpStr));
		    sprintf(tmpStr, "wireless%d", instindex);
		    ProfSetNameByPoint((void*)wirelessProfile,tmpStr);

		    rst = TRUE; 
		
		}
#endif
	    break;
	case MIB_PROF_WIRELESS_MAC_FILTER:
		if (instindex == 0) {
		wirelessMacFilterProfile = (WirelessMacFilterProfile *)defprof;
		wirelessMacFilterProfile->head.active = TRUE;
		//memset(wirelessMacFilterProfile->mac, '\0', sizeof(wirelessMacFilterProfile->mac));
		rst = TRUE;
		}
#if (WIRELESS_5G == FYES)
		if (instindex == 1) {
		wireless5gMacFilterProfile = (Wireless5gMacFilterProfile *)defprof;
		wireless5gMacFilterProfile->head.active = TRUE;
		//memset(wirelessMacFilterProfile->mac, '\0', sizeof(wirelessMacFilterProfile->mac));
		rst = TRUE;
		}
#endif
	break;
#endif

#if (UTTPDB == FYES)
	    /*add by yqyang*/
/*
	case MIB_PROF_PDB_GLOBAL:
	    pdbProfile= (PdbProfile*)defprof;  
	    pdbProfile->head.active = TRUE;
	    if(instindex == DEFAULT_PDB_QQ){
		ProfSetNameByPoint(pdbProfile, defaultPdbName1);
		strcpy(pdbProfile->description, "½ûÖ¹QQ");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_QQ;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_MSN){
		ProfSetNameByPoint(pdbProfile, defaultPdbName2);
		strcpy(pdbProfile->description, "½ûÖ¹MSN");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_MSN;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_ALI){
		ProfSetNameByPoint(pdbProfile, defaultPdbName3);
		strcpy(pdbProfile->description, "½ûÖ¹°¢ÀïÍúÍúµÇÂ½");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_ALI;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_WEBQQ){
		ProfSetNameByPoint(pdbProfile, defaultPdbName4);
		strcpy(pdbProfile->description, "½ûÖ¹ÍøÒ³QQ");
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_WEBQQ;
		pdb_load_content(pdbProfile, MAX_PDB_WEBQQ_LINES, pdb_webQQ);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_FETION){
		ProfSetNameByPoint(pdbProfile, defaultPdbName5);
		strcpy(pdbProfile->description, "½ûÖ¹·ÉÐÅ");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_FETION;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_BT){
		ProfSetNameByPoint(pdbProfile, defaultPdbName6);
		strcpy(pdbProfile->description, "½ûÖ¹±ÈÌØåçÐÇ¡¢¾«Áé");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_BT;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_THUNDER){
		ProfSetNameByPoint(pdbProfile, defaultPdbName7);
		strcpy(pdbProfile->description, "½ûÖ¹Ñ¸À×ËÑË÷×ÊÔ´");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_THUNDER;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_QQLIVE){
		ProfSetNameByPoint(pdbProfile, defaultPdbName8);
		strcpy(pdbProfile->description, "½ûÖ¹QQLive");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_QQLIVE;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_PPSTREAM){
		ProfSetNameByPoint(pdbProfile, defaultPdbName9);
		strcpy(pdbProfile->description, "½ûÖ¹pps²¥·ÅÊÓÆµ");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_PPSTREAM;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_KUGOO){
		ProfSetNameByPoint(pdbProfile, defaultPdbName10);
		strcpy(pdbProfile->description, "½ûÖ¹¿á¹·ËÑË÷×ÊÔ´");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_KUGOO;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_PPLIVE){
		ProfSetNameByPoint(pdbProfile, defaultPdbName11);
		strcpy(pdbProfile->description, "½ûÖ¹pplive²¥·ÅÊÓÆµ");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_PPLIVE;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_QVOD){
		ProfSetNameByPoint(pdbProfile, defaultPdbName12);
		strcpy(pdbProfile->description, "½ûÖ¹¿ì²¥");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_QVOD;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_QQGAME){
		ProfSetNameByPoint(pdbProfile, defaultPdbName14);
		strcpy(pdbProfile->description, "½ûÖ¹QQGame");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_QQGAME;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_PAOPAO){
		ProfSetNameByPoint(pdbProfile, defaultPdbName15);
		strcpy(pdbProfile->description, "½ûÖ¹ÅÝÅÝÌÃÓÎÏ·");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_PAOPAO;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_paopaotang);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_ZHENGTU){
		ProfSetNameByPoint(pdbProfile, defaultPdbName16);
		strcpy(pdbProfile->description, "½ûÖ¹Õ÷Í¾ÓÎÏ·");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_ZHENGTU;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_zhengtu);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_WANMEI){
		ProfSetNameByPoint(pdbProfile, defaultPdbName17);
		strcpy(pdbProfile->description, "½ûÖ¹ÍêÃÀÊÀ½ç£¬ÖïÏÉ");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_WANMEI;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_wanmei);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_XIYOU){
		ProfSetNameByPoint(pdbProfile, defaultPdbName18);
		strcpy(pdbProfile->description, "½ûÖ¹ÃÎ»ÃÎ÷ÓÎÓÎÏ·");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_XIYOU;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_JWT){
		ProfSetNameByPoint(pdbProfile, defaultPdbName19);
		strcpy(pdbProfile->description, "½ûÖ¹¾¢ÎèÍÅÓÎÏ·");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_JWT;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_JWT);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_HFANG){
		ProfSetNameByPoint(pdbProfile, defaultPdbName20);
		strcpy(pdbProfile->description, "½ûÖ¹½øÈëºÆ·½");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_HFANG;
		pdb_load_content(pdbProfile, MAX_PDB_HFANG_LINES, pdb_HFANG);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_WOW){
		ProfSetNameByPoint(pdbProfile, defaultPdbName21);
		strcpy(pdbProfile->description, "½ûÖ¹Ä§ÊÞÊÀ½çÓÎÏ·");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_WOW;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_WOW);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_AION){
		ProfSetNameByPoint(pdbProfile, defaultPdbName22);
		strcpy(pdbProfile->description, "½ûÖ¹ÓÀºãÖ®ËþÓÎÏ·");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_AION;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_AION);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_POPCAR){
		ProfSetNameByPoint(pdbProfile, defaultPdbName23);
		strcpy(pdbProfile->description, "½ûÖ¹ÅÜÅÜ¿¨¶¡³µÓÎÏ·");
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_POPCAR;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_POPCAR);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_GAMEURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName24);
		strcpy(pdbProfile->description, "½ûÖ¹ÓÎÏ·ÍøÕ¾");
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_GAMEURL;
		pdb_load_content(pdbProfile, MAX_PDB_GAMEURL_LINES, pdb_GAMEURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_STOCKURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName25);
		strcpy(pdbProfile->description, "½ûÖ¹Ö¤È¯ÍøÕ¾");
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_STOCKURL;
		pdb_load_content(pdbProfile, MAX_PDB_STOCKURL_LINES, pdb_STOCKURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_SHOPURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName26);
		strcpy(pdbProfile->description, "½ûÖ¹¹ºÎïÍøÕ¾");
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_SHOPURL;
		pdb_load_content(pdbProfile, MAX_PDB_SHOPURL_LINES, pdb_SHOPURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_SOCIALURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName27);
		strcpy(pdbProfile->description, "½ûÖ¹Éç½»ÍøÕ¾");
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_SOCIALURL;
		pdb_load_content(pdbProfile, MAX_PDB_SOCIALURL_LINES, pdb_SOCIALURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_HTTPPROXY){
		ProfSetNameByPoint(pdbProfile, defaultPdbName28);
		strcpy(pdbProfile->description, "½ûÖ¹http´úÀí");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_HTTPPROXY;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_SOCKS5){
		ProfSetNameByPoint(pdbProfile, defaultPdbName29);
		strcpy(pdbProfile->description, "½ûÖ¹SOCKS5´úÀí");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_SOCKS5;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_FILETYPE){
		ProfSetNameByPoint(pdbProfile, defaultPdbName30);
		strcpy(pdbProfile->description, "¹ýÂËÎÄ¼þÀàÐÍ");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_FILETYPE;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_POST){
		ProfSetNameByPoint(pdbProfile, defaultPdbName31);
		strcpy(pdbProfile->description, "½ûÖ¹ÍøÒ³Ìá½»ÊäÈë");
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_POST;
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_10000){
		ProfSetNameByPoint(pdbProfile, defaultPdbName32);
		pdbProfile->head.active = TRUE;
		strcpy(pdbProfile->description, "µçÐÅ¾²Ì¬Â·ÓÉ");
		pdbProfile->type = RoutePdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_10000;
		pdb_load_content(pdbProfile, (sizeof(pdb_10000) / sizeof(pdb_10000[0])), pdb_10000);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_10010){
		ProfSetNameByPoint(pdbProfile, defaultPdbName33);
		pdbProfile->head.active = TRUE;
		strcpy(pdbProfile->description, "ÁªÍ¨¾²Ì¬Â·ÓÉ");
		pdbProfile->type = RoutePdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_10010;
		pdb_load_content(pdbProfile, (sizeof(pdb_10010) / sizeof(pdb_10010[0])), pdb_10010);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_10086){
		ProfSetNameByPoint(pdbProfile, defaultPdbName34);
		pdbProfile->head.active = TRUE;
		strcpy(pdbProfile->description, "ÒÆ¶¯¾²Ì¬Â·ÓÉ");
		pdbProfile->type = RoutePdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_10086;
		pdb_load_content(pdbProfile, (sizeof(pdb_10086) / sizeof(pdb_10086[0])), pdb_10086);
		rst = TRUE;
	    }

	    break;
*/
	case MIB_PROF_PDB_GLOBAL:
	    pdbProfile= (PdbProfile*)defprof;  
	    pdbProfile->head.active = TRUE;
	    if(instindex == 0){
		ProfSetNameByPoint(pdbProfile, defaultPdbName133);
		strcpy(pdbProfile->description, DEFAULTPDBNAME133);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_MAIL;
		pdbProfile->index = DEFAULT_PDB_MAILQQ;
		pdb_load_content(pdbProfile, MAX_PDB_MAILQQ_LINES, pdb_MailQQ);
		rst = TRUE;
	    }
	    else if(instindex == 1){
		ProfSetNameByPoint(pdbProfile, defaultPdbName2);
		strcpy(pdbProfile->description, DEFAULTPDBNAME2);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_MSN;
		pdb_load_content(pdbProfile, MAX_PDB_WLMESSENGER_LINES, pdb_WLMessenger);
		rst = TRUE;
	    }
	    else if(instindex == 2){
		ProfSetNameByPoint(pdbProfile, defaultPdbName3);
		strcpy(pdbProfile->description, DEFAULTPDBNAME3);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_ALI;
		pdb_load_content(pdbProfile, MAX_PDB_ALIIM_LINES, pdb_AliIM);
		rst = TRUE;
	    }
	    else if(instindex == 3){
		ProfSetNameByPoint(pdbProfile, defaultPdbName4);
		strcpy(pdbProfile->description, DEFAULTPDBNAME4);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_WEBQQ;
		pdb_load_content(pdbProfile, MAX_PDB_WEBQQ_LINES, pdb_webQQ);
		rst = TRUE;
	    }
	    else if(instindex == 4){
		ProfSetNameByPoint(pdbProfile, defaultPdbName5);
		strcpy(pdbProfile->description, DEFAULTPDBNAME5);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_FETION;
		pdb_load_content(pdbProfile, MAX_PDB_FETION_LINES, pdb_Fetion);
		rst = TRUE;
	    }
	    else if(instindex == 5){
		ProfSetNameByPoint(pdbProfile, defaultPdbName6);
		strcpy(pdbProfile->description, DEFAULTPDBNAME6);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_BT;
		pdb_load_content(pdbProfile, MAX_PDB_BITTORRENT_LINES, pdb_BitTorrent);
		rst = TRUE;
	    }
	    else if(instindex == 6){
		ProfSetNameByPoint(pdbProfile, defaultPdbName7);
		strcpy(pdbProfile->description, DEFAULTPDBNAME7);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_THUNDER;
		pdb_load_content(pdbProfile, MAX_PDB_THUNDER_LINES, pdb_Thunder);
		rst = TRUE;
	    }
	    else if(instindex == 7){
            ProfSetNameByPoint(pdbProfile, defaultPdbName8);
            strcpy(pdbProfile->description, DEFAULTPDBNAME8);
            pdbProfile->mode = Predefined;
            //pdbProfile->type = L7Pdb;
            pdbProfile->type = DnsPdb;
            pdbProfile->category = PDB_P2P;
            pdbProfile->index = DEFAULT_PDB_QQLIVE;
            pdb_load_content(pdbProfile, MAX_PDB_QQLIVE_LINES, pdb_QQLive);
            rst = TRUE;
	    }
	    else if(instindex == 8){
		ProfSetNameByPoint(pdbProfile, defaultPdbName9);
		strcpy(pdbProfile->description, DEFAULTPDBNAME9);
		pdbProfile->mode = Predefined;
		//pdbProfile->type = L7Pdb;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_PPSTREAM;
		pdb_load_content(pdbProfile, MAX_PDB_PPSTREAM_LINES, pdb_PPStream);
		rst = TRUE;
	    }
	    else if(instindex == 9){
		ProfSetNameByPoint(pdbProfile, defaultPdbName10);
		strcpy(pdbProfile->description, DEFAULTPDBNAME10);
		pdbProfile->mode = Predefined;
		//pdbProfile->type = L7Pdb;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_KUGOO;
		pdb_load_content(pdbProfile, MAX_PDB_KUGOU_LINES, pdb_KuGou);
		rst = TRUE;
	    }
	    else if(instindex == 10){
		ProfSetNameByPoint(pdbProfile, defaultPdbName11);
		strcpy(pdbProfile->description, DEFAULTPDBNAME11);
		pdbProfile->mode = Predefined;
		//pdbProfile->type = L7Pdb;
        pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_PPLIVE;
		pdb_load_content(pdbProfile, MAX_PDB_PPTV_LINES, pdb_PPTV);
		rst = TRUE;
	    }
	    else if(instindex == 11){
		ProfSetNameByPoint(pdbProfile, defaultPdbName12);
		strcpy(pdbProfile->description, DEFAULTPDBNAME12);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_QVOD;
		pdb_load_content(pdbProfile, MAX_PDB_QVOD_LINES, pdb_Qvod);
		rst = TRUE;
	    }
	    else if(instindex == 12){
		ProfSetNameByPoint(pdbProfile, defaultPdbName13);
		strcpy(pdbProfile->description, DEFAULTPDBNAME13);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_P2P;
		pdbProfile->index = DEFAULT_PDB_BAIDUPLAYER;
		pdb_load_content(pdbProfile, MAX_PDB_BAIDUPLAYER_LINES, pdb_BaiduPlayer);
		rst = TRUE;
	    }
	    else if(instindex == 13){
		ProfSetNameByPoint(pdbProfile, defaultPdbName14);
		strcpy(pdbProfile->description, DEFAULTPDBNAME14);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_STOCK;
		pdbProfile->index = DEFAULT_PDB_STOCKURL;
		pdb_load_content(pdbProfile, MAX_PDB_STOCKURL_LINES, pdb_STOCKURL);
		rst = TRUE;
	    }
	    else if(instindex == 14){
		ProfSetNameByPoint(pdbProfile, defaultPdbName15);
		strcpy(pdbProfile->description, DEFAULTPDBNAME15);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_YOUKU;
		pdb_load_content(pdbProfile, MAX_PDB_YOUKU_LINES, pdb_YouKu);
		rst = TRUE;
	    }
	    else if(instindex == 15){
		ProfSetNameByPoint(pdbProfile, defaultPdbName16);
		strcpy(pdbProfile->description, DEFAULTPDBNAME16);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_TUDOU;
		pdb_load_content(pdbProfile, MAX_PDB_TUDOU_LINES, pdb_TuDou);
		rst = TRUE;
	    }
	    else if(instindex == 16){
		ProfSetNameByPoint(pdbProfile, defaultPdbName17);
		strcpy(pdbProfile->description, DEFAULTPDBNAME17);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_AIQI;
		pdb_load_content(pdbProfile, MAX_PDB_AIQI_LINES, pdb_AiQi);
		rst = TRUE;
	    }
	    else if(instindex == 17){
		ProfSetNameByPoint(pdbProfile, defaultPdbName18);
		strcpy(pdbProfile->description, DEFAULTPDBNAME18);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_SOUHUVIDEO;
		pdb_load_content(pdbProfile, MAX_PDB_SOUHUVIDEO_LINES, pdb_SouHuVideo);
		rst = TRUE;
	    }
	    else if(instindex == 18){
		ProfSetNameByPoint(pdbProfile, defaultPdbName19);
		strcpy(pdbProfile->description, DEFAULTPDBNAME19);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_THUNDERKANKAN;
		pdb_load_content(pdbProfile, MAX_PDB_THUNDERKANKAN_LINES, pdb_ThunderKanKan);
		rst = TRUE;
	    }
	    else if(instindex == 19){
		ProfSetNameByPoint(pdbProfile, defaultPdbName20);
		strcpy(pdbProfile->description, DEFAULTPDBNAME20);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_KU6;
		pdb_load_content(pdbProfile, MAX_PDB_KU6_LINES, pdb_Ku6);
		rst = TRUE;
	    }
	    else if(instindex == 20){
		ProfSetNameByPoint(pdbProfile, defaultPdbName21);
		strcpy(pdbProfile->description, DEFAULTPDBNAME21);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_NET56;
		pdb_load_content(pdbProfile, MAX_PDB_NET56_LINES, pdb_Net56);
		rst = TRUE;
	    }
	    else if(instindex == 21){
		ProfSetNameByPoint(pdbProfile, defaultPdbName22);
		strcpy(pdbProfile->description, DEFAULTPDBNAME22);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_SINAVIDEO;
		pdb_load_content(pdbProfile, MAX_PDB_SINAVIDEO_LINES, pdb_SinaVideo);
		rst = TRUE;
	    }
	    else if(instindex == 22){
		ProfSetNameByPoint(pdbProfile, defaultPdbName23);
		strcpy(pdbProfile->description, DEFAULTPDBNAME23);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_TECENTVIDEO;
		pdb_load_content(pdbProfile, MAX_PDB_TECENTVIDEO_LINES, pdb_TencentVideo);
		rst = TRUE;
	    }
	    else if(instindex == 23){
		ProfSetNameByPoint(pdbProfile, defaultPdbName24);
		strcpy(pdbProfile->description, DEFAULTPDBNAME24);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_CNTV;
		pdb_load_content(pdbProfile, MAX_PDB_CNTV_LINES, pdb_CNTV);
		rst = TRUE;
	    }
	    else if(instindex == 24){
		ProfSetNameByPoint(pdbProfile, defaultPdbName25);
		strcpy(pdbProfile->description, DEFAULTPDBNAME25);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_IFENGVIDEO;
		pdb_load_content(pdbProfile, MAX_PDB_IFENGVIDEO_LINES, pdb_iFengVideo);
		rst = TRUE;
	    }
	    else if(instindex == 25){
		ProfSetNameByPoint(pdbProfile, defaultPdbName26);
		strcpy(pdbProfile->description, DEFAULTPDBNAME26);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_BAOFENG;
		pdb_load_content(pdbProfile, MAX_PDB_BAOFENG_LINES, pdb_BaoFeng);
		rst = TRUE;
	    }
	    else if(instindex == 26){
		ProfSetNameByPoint(pdbProfile, defaultPdbName27);
		strcpy(pdbProfile->description, DEFAULTPDBNAME27);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_JOY;
		pdb_load_content(pdbProfile, MAX_PDB_JOY_LINES, pdb_Joy);
		rst = TRUE;
	    }
	    else if(instindex == 27){
		ProfSetNameByPoint(pdbProfile, defaultPdbName28);
		strcpy(pdbProfile->description, DEFAULTPDBNAME28);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_PPS;
		pdb_load_content(pdbProfile, MAX_PDB_PPS_LINES, pdb_PPS);
		rst = TRUE;
	    }
	    else if(instindex == 28){
		ProfSetNameByPoint(pdbProfile, defaultPdbName29);
		strcpy(pdbProfile->description, DEFAULTPDBNAME29);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_FUNSHION;
		pdb_load_content(pdbProfile, MAX_PDB_FUNSHION_LINES, pdb_FunShion);
		rst = TRUE;
	    }
	    else if(instindex == 29){
		ProfSetNameByPoint(pdbProfile, defaultPdbName30);
		strcpy(pdbProfile->description, DEFAULTPDBNAME30);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_GAMEURL;
		pdb_load_content(pdbProfile, MAX_PDB_GAMEURL_LINES, pdb_GAMEURL);
		rst = TRUE;
	    }
	    else if(instindex == 30){
		ProfSetNameByPoint(pdbProfile, defaultPdbName31);
		strcpy(pdbProfile->description, DEFAULTPDBNAME31);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_QQGAME;
		pdb_load_content(pdbProfile, MAX_PDB_QQGAME_LINES, pdb_QQGame);
		rst = TRUE;
	    }
	    else if(instindex == 31){
		ProfSetNameByPoint(pdbProfile, defaultPdbName32);
		strcpy(pdbProfile->description, DEFAULTPDBNAME32);
		pdbProfile->mode = Predefined;
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_PAOPAO;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_paopaotang);
		rst = TRUE;
	    }
	    else if(instindex == 32){
		ProfSetNameByPoint(pdbProfile, defaultPdbName33);
		strcpy(pdbProfile->description, DEFAULTPDBNAME33);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_ZHENGTU;
		pdb_load_content(pdbProfile, MAX_PDB_ZHENGTU_LINES, pdb_zhengtu);
		rst = TRUE;
	    }
	    else if(instindex == 33){
		ProfSetNameByPoint(pdbProfile, defaultPdbName34);
		strcpy(pdbProfile->description, DEFAULTPDBNAME34);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_WANMEI;
		pdb_load_content(pdbProfile, MAX_PDB_WANMEI_LINES, pdb_wanmei);
		rst = TRUE;
	    }
	    else if(instindex == 34){
		ProfSetNameByPoint(pdbProfile, defaultPdbName35);
		strcpy(pdbProfile->description, DEFAULTPDBNAME35);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_XIYOU;
		pdb_load_content(pdbProfile, MAX_PDB_XYQ_LINES, pdb_xyq);
		rst = TRUE;
	    }
	    else if(instindex == 35){
		ProfSetNameByPoint(pdbProfile, defaultPdbName36);
		strcpy(pdbProfile->description, DEFAULTPDBNAME36);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_JWT;
		pdb_load_content(pdbProfile, MAX_PDB_JWT_LINES, pdb_JWT);
		rst = TRUE;
	    }
	    else if(instindex == 36){
		ProfSetNameByPoint(pdbProfile, defaultPdbName37);
		strcpy(pdbProfile->description, DEFAULTPDBNAME37);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_HFANG;
		pdb_load_content(pdbProfile, MAX_PDB_HFANG_LINES, pdb_HFANG);
		rst = TRUE;
	    }
	    else if(instindex == 37){
		ProfSetNameByPoint(pdbProfile, defaultPdbName38);
		strcpy(pdbProfile->description, DEFAULTPDBNAME38);
		pdbProfile->mode = Predefined;
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_WOW;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_WOW);
		rst = TRUE;
	    }
	    else if(instindex == 38){
		ProfSetNameByPoint(pdbProfile, defaultPdbName39);
		strcpy(pdbProfile->description, DEFAULTPDBNAME39);
		pdbProfile->mode = Predefined;
		pdbProfile->type = PortPdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_AION;
		pdb_load_content(pdbProfile, MAX_PDB_PORTPDB_LINES, pdb_AION);
		rst = TRUE;
	    }
	    else if(instindex == 39){
		ProfSetNameByPoint(pdbProfile, defaultPdbName40);
		strcpy(pdbProfile->description, DEFAULTPDBNAME40);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_POPCAR;
		pdb_load_content(pdbProfile, MAX_PDB_POPCAR_LINES, pdb_POPCAR);
		rst = TRUE;
	    }
	    else if(instindex == 40){
		ProfSetNameByPoint(pdbProfile, defaultPdbName41);
		strcpy(pdbProfile->description, DEFAULTPDBNAME41);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_QQCF;
		pdb_load_content(pdbProfile, MAX_PDB_QQCF_LINES, pdb_QQCF);
		rst = TRUE;
	    }
	    else if(instindex == 41){
		ProfSetNameByPoint(pdbProfile, defaultPdbName46);
		strcpy(pdbProfile->description, DEFAULTPDBNAME46);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_SDODN;
		pdb_load_content(pdbProfile, MAX_PDB_SDODN_LINES, pdb_SDODN);
		rst = TRUE;
	    }
	    else if(instindex == 42){
		ProfSetNameByPoint(pdbProfile, defaultPdbName47);
		strcpy(pdbProfile->description, DEFAULTPDBNAME47);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_THE9SXZ;
		pdb_load_content(pdbProfile, MAX_PDB_THE9SXZ_LINES, pdb_THE9SXZ);
		rst = TRUE;
	    }
	    else if(instindex == 43){
		ProfSetNameByPoint(pdbProfile, defaultPdbName48);
		strcpy(pdbProfile->description, DEFAULTPDBNAME48);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_SDOMIR;
		pdb_load_content(pdbProfile, MAX_PDB_SDOMIR_LINES, pdb_SDOMIR);
		rst = TRUE;
	    }
	    else if(instindex == 44){
		ProfSetNameByPoint(pdbProfile, defaultPdbName49);
		strcpy(pdbProfile->description, DEFAULTPDBNAME49);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_TL3;
		pdb_load_content(pdbProfile, MAX_PDB_TL3_LINES, pdb_TL3);
		rst = TRUE;
	    }
	    else if(instindex == 45){
		ProfSetNameByPoint(pdbProfile, defaultPdbName50);
		strcpy(pdbProfile->description, DEFAULTPDBNAME50);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_CSOL;
		pdb_load_content(pdbProfile, MAX_PDB_CSOL_LINES, pdb_CSOL);
		rst = TRUE;
	    }
	    else if(instindex == 46){
		ProfSetNameByPoint(pdbProfile, defaultPdbName52);
		strcpy(pdbProfile->description, DEFAULTPDBNAME52);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_SDOMXD;
		pdb_load_content(pdbProfile, MAX_PDB_SDOMXD_LINES, pdb_SDOMXD);
		rst = TRUE;
	    }
	    else if(instindex == 47){
		ProfSetNameByPoint(pdbProfile, defaultPdbName53);
		strcpy(pdbProfile->description, DEFAULTPDBNAME53);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_QN163;
		pdb_load_content(pdbProfile, MAX_PDB_QN163_LINES, pdb_QN163);
		rst = TRUE;
	    }
	    else if(instindex == 48){
		ProfSetNameByPoint(pdbProfile, defaultPdbName54);
		strcpy(pdbProfile->description, DEFAULTPDBNAME54);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_WD;
		pdb_load_content(pdbProfile, MAX_PDB_WD_LINES, pdb_WD);
		rst = TRUE;
	    }
	    else if(instindex == 49){
		ProfSetNameByPoint(pdbProfile, defaultPdbName55);
		strcpy(pdbProfile->description, DEFAULTPDBNAME55);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_M3GUO;
		pdb_load_content(pdbProfile, MAX_PDB_M3GUO_LINES, pdb_M3GUO);
		rst = TRUE;
	    }
	    else if(instindex == 50){
		ProfSetNameByPoint(pdbProfile, defaultPdbName56);
		strcpy(pdbProfile->description, DEFAULTPDBNAME56);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_GAME;
		pdbProfile->index = DEFAULT_PDB_WOT;
		pdb_load_content(pdbProfile, MAX_PDB_WOT_LINES, pdb_WOT);
		rst = TRUE;
	    }
	    else if(instindex == 51){
		ProfSetNameByPoint(pdbProfile, defaultPdbName57);
		strcpy(pdbProfile->description, DEFAULTPDBNAME57);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_TAOBAO;
		pdb_load_content(pdbProfile, MAX_PDB_TAOBAO_LINES, pdb_TaoBao);
		rst = TRUE;
	    }
	    else if(instindex == 52){
		ProfSetNameByPoint(pdbProfile, defaultPdbName58);
		strcpy(pdbProfile->description, DEFAULTPDBNAME58);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_TMALL;
		pdb_load_content(pdbProfile, MAX_PDB_TMALL_LINES, pdb_TMall);
		rst = TRUE;
	    }
	    else if(instindex == 53){
		ProfSetNameByPoint(pdbProfile, defaultPdbName59);
		strcpy(pdbProfile->description, DEFAULTPDBNAME59);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_BUY360;
		pdb_load_content(pdbProfile, MAX_PDB_BUY360_LINES, pdb_buy360);
		rst = TRUE;
	    }
	    else if(instindex == 54){
		ProfSetNameByPoint(pdbProfile, defaultPdbName60);
		strcpy(pdbProfile->description, DEFAULTPDBNAME60);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_DANGDANG;
		pdb_load_content(pdbProfile, MAX_PDB_DANGDANG_LINES, pdb_DangDang);
		rst = TRUE;
	    }
	    else if(instindex == 55){
		ProfSetNameByPoint(pdbProfile, defaultPdbName61);
		strcpy(pdbProfile->description, DEFAULTPDBNAME61);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_AMAZON;
		pdb_load_content(pdbProfile, MAX_PDB_AMAZON_LINES, pdb_Amazon);
		rst = TRUE;
	    }
	    else if(instindex == 56){
		ProfSetNameByPoint(pdbProfile, defaultPdbName62);
		strcpy(pdbProfile->description, DEFAULTPDBNAME62);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_PAIPAI;
		pdb_load_content(pdbProfile, MAX_PDB_PAIPAI_LINES, pdb_PaiPai);
		rst = TRUE;
	    }
	    else if(instindex == 57){
		ProfSetNameByPoint(pdbProfile, defaultPdbName63);
		strcpy(pdbProfile->description, DEFAULTPDBNAME63);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_ALIBABA;
		pdb_load_content(pdbProfile, MAX_PDB_ALIBABA_LINES, pdb_Alibaba);
		rst = TRUE;
	    }
	    else if(instindex == 58){
		ProfSetNameByPoint(pdbProfile, defaultPdbName64);
		strcpy(pdbProfile->description, DEFAULTPDBNAME64);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_M18;
		pdb_load_content(pdbProfile, MAX_PDB_M18_LINES, pdb_M18);
		rst = TRUE;
	    }
	    else if(instindex == 59){
		ProfSetNameByPoint(pdbProfile, defaultPdbName65);
		strcpy(pdbProfile->description, DEFAULTPDBNAME65);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_HAO123;
		pdb_load_content(pdbProfile, MAX_PDB_HAO123_LINES, pdb_hao123);
		rst = TRUE;
	    }
	    else if(instindex == 60){
		ProfSetNameByPoint(pdbProfile, defaultPdbName66);
		strcpy(pdbProfile->description, DEFAULTPDBNAME66);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_VANCL;
		pdb_load_content(pdbProfile, MAX_PDB_VANCL_LINES, pdb_Vancl);
		rst = TRUE;
	    }
	    else if(instindex == 61){
		ProfSetNameByPoint(pdbProfile, defaultPdbName67);
		strcpy(pdbProfile->description, DEFAULTPDBNAME67);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_VIPSHOP;
		pdb_load_content(pdbProfile, MAX_PDB_VIPSHOP_LINES, pdb_VipShop);
		rst = TRUE;
	    }
	    else if(instindex == 62){
		ProfSetNameByPoint(pdbProfile, defaultPdbName68);
		strcpy(pdbProfile->description, DEFAULTPDBNAME68);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_FCLUB;
		pdb_load_content(pdbProfile, MAX_PDB_FCLUB_LINES, pdb_Fclub);
		rst = TRUE;
	    }
	    else if(instindex == 63){
		ProfSetNameByPoint(pdbProfile, defaultPdbName69);
		strcpy(pdbProfile->description, DEFAULTPDBNAME69);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_XIU;
		pdb_load_content(pdbProfile, MAX_PDB_XIU_LINES, pdb_Xiu);
		rst = TRUE;
	    }
	    else if(instindex == 64){
		ProfSetNameByPoint(pdbProfile, defaultPdbName70);
		strcpy(pdbProfile->description, DEFAULTPDBNAME70);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_SSQY;
		pdb_load_content(pdbProfile, MAX_PDB_SSQY_LINES, pdb_SSQY);
		rst = TRUE;
	    }
	    else if(instindex == 65){
		ProfSetNameByPoint(pdbProfile, defaultPdbName71);
		strcpy(pdbProfile->description, DEFAULTPDBNAME71);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_MOONBASA;
		pdb_load_content(pdbProfile, MAX_PDB_MOONBASA_LINES, pdb_MoonBasa);
		rst = TRUE;
	    }
	    else if(instindex == 66){
		ProfSetNameByPoint(pdbProfile, defaultPdbName72);
		strcpy(pdbProfile->description, DEFAULTPDBNAME72);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_YINTAI;
		pdb_load_content(pdbProfile, MAX_PDB_YINTAI_LINES, pdb_YinTai);
		rst = TRUE;
	    }
	    else if(instindex == 67){
		ProfSetNameByPoint(pdbProfile, defaultPdbName73);
		strcpy(pdbProfile->description, DEFAULTPDBNAME73);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_VJIA;
		pdb_load_content(pdbProfile, MAX_PDB_VJIA_LINES, pdb_Vjia);
		rst = TRUE;
	    }
	    else if(instindex == 68){
		ProfSetNameByPoint(pdbProfile, defaultPdbName74);
		strcpy(pdbProfile->description, DEFAULTPDBNAME74);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_MASAMASO;
		pdb_load_content(pdbProfile, MAX_PDB_MASAMASO_LINES, pdb_MasaMaso);
		rst = TRUE;
	    }
	    else if(instindex == 69){
		ProfSetNameByPoint(pdbProfile, defaultPdbName75);
		strcpy(pdbProfile->description, DEFAULTPDBNAME75);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_PD89;
		pdb_load_content(pdbProfile, MAX_PDB_PD89_LINES, pdb_pb89);
		rst = TRUE;
	    }
	    else if(instindex == 70){
		ProfSetNameByPoint(pdbProfile, defaultPdbName76);
		strcpy(pdbProfile->description, DEFAULTPDBNAME76);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_SUNING;
		pdb_load_content(pdbProfile, MAX_PDB_SUNING_LINES, pdb_SuNing);
		rst = TRUE;
	    }
	    else if(instindex == 71){
		ProfSetNameByPoint(pdbProfile, defaultPdbName77);
		strcpy(pdbProfile->description, DEFAULTPDBNAME77);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_GOME;
		pdb_load_content(pdbProfile, MAX_PDB_GOME_LINES, pdb_Gome);
		rst = TRUE;
	    }
	    else if(instindex == 72){
		ProfSetNameByPoint(pdbProfile, defaultPdbName78);
		strcpy(pdbProfile->description, DEFAULTPDBNAME78);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_BUY51;
		pdb_load_content(pdbProfile, MAX_PDB_BUY51_LINES, pdb_buy51);
		rst = TRUE;
	    }
	    else if(instindex == 73){
		ProfSetNameByPoint(pdbProfile, defaultPdbName79);
		strcpy(pdbProfile->description, DEFAULTPDBNAME79);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_COO8;
		pdb_load_content(pdbProfile, MAX_PDB_COO8_LINES, pdb_Coo8);
		rst = TRUE;
	    }
	    else if(instindex == 74){
		ProfSetNameByPoint(pdbProfile, defaultPdbName80);
		strcpy(pdbProfile->description, DEFAULTPDBNAME80);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_NEWEGG;
		pdb_load_content(pdbProfile, MAX_PDB_NEWEGG_LINES, pdb_NewEgg);
		rst = TRUE;
	    }
	    else if(instindex == 75){
		ProfSetNameByPoint(pdbProfile, defaultPdbName81);
		strcpy(pdbProfile->description, DEFAULTPDBNAME81);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_LUSEN;
		pdb_load_content(pdbProfile, MAX_PDB_LUSEN_LINES, pdb_LuSen);
		rst = TRUE;
	    }
	    else if(instindex == 76){
		ProfSetNameByPoint(pdbProfile, defaultPdbName82);
		strcpy(pdbProfile->description, DEFAULTPDBNAME82);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_OKBUY;
		pdb_load_content(pdbProfile, MAX_PDB_OKBUY_LINES, pdb_okBuy);
		rst = TRUE;
	    }
	    else if(instindex == 77){
		ProfSetNameByPoint(pdbProfile, defaultPdbName83);
		strcpy(pdbProfile->description, DEFAULTPDBNAME83);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_LETAO;
		pdb_load_content(pdbProfile, MAX_PDB_LETAO_LINES, pdb_LeTao);
		rst = TRUE;
	    }
	    else if(instindex == 78){
		ProfSetNameByPoint(pdbProfile, defaultPdbName84);
		strcpy(pdbProfile->description, DEFAULTPDBNAME84);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_YOUGOU;
		pdb_load_content(pdbProfile, MAX_PDB_YOUGOU_LINES, pdb_YouGou);
		rst = TRUE;
	    }
	    else if(instindex == 79){
		ProfSetNameByPoint(pdbProfile, defaultPdbName85);
		strcpy(pdbProfile->description, DEFAULTPDBNAME85);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_SCN;
		pdb_load_content(pdbProfile, MAX_PDB_SCN_LINES, pdb_scn);
		rst = TRUE;
	    }
	    else if(instindex == 80){
		ProfSetNameByPoint(pdbProfile, defaultPdbName86);
		strcpy(pdbProfile->description, DEFAULTPDBNAME86);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_MBAOBAO;
		pdb_load_content(pdbProfile, MAX_PDB_MBAOBAO_LINES, pdb_mBaoBao);
		rst = TRUE;
	    }
	    else if(instindex == 81){
		ProfSetNameByPoint(pdbProfile, defaultPdbName87);
		strcpy(pdbProfile->description, DEFAULTPDBNAME87);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_LAFASO;
		pdb_load_content(pdbProfile, MAX_PDB_LAFASO_LINES, pdb_Lafaso);
		rst = TRUE;
	    }
	    else if(instindex == 82){
		ProfSetNameByPoint(pdbProfile, defaultPdbName88);
		strcpy(pdbProfile->description, DEFAULTPDBNAME88);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_TIANTIAN;
		pdb_load_content(pdbProfile, MAX_PDB_TIANTIAN_LINES, pdb_TianTian);
		rst = TRUE;
	    }
	    else if(instindex == 83){
		ProfSetNameByPoint(pdbProfile, defaultPdbName89);
		strcpy(pdbProfile->description, DEFAULTPDBNAME89);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_SEPHORA;
		pdb_load_content(pdbProfile, MAX_PDB_SEPHORA_LINES, pdb_Sephora);
		rst = TRUE;
	    }
	    else if(instindex == 84){
		ProfSetNameByPoint(pdbProfile, defaultPdbName90);
		strcpy(pdbProfile->description, DEFAULTPDBNAME90);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_DHC;
		pdb_load_content(pdbProfile, MAX_PDB_DHC_LINES, pdb_DHC);
		rst = TRUE;
	    }
	    else if(instindex == 85){
		ProfSetNameByPoint(pdbProfile, defaultPdbName91);
		strcpy(pdbProfile->description, DEFAULTPDBNAME91);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_JXDYF;
		pdb_load_content(pdbProfile, MAX_PDB_JXDYF_LINES, pdb_jxdyf);
		rst = TRUE;
	    }
	    else if(instindex == 86){
		ProfSetNameByPoint(pdbProfile, defaultPdbName92);
		strcpy(pdbProfile->description, DEFAULTPDBNAME92);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_STRAW;
		pdb_load_content(pdbProfile, MAX_PDB_STRAW_LINES, pdb_straw);
		rst = TRUE;
	    }
	    else if(instindex == 87){
		ProfSetNameByPoint(pdbProfile, defaultPdbName93);
		strcpy(pdbProfile->description, DEFAULTPDBNAME93);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_HITAO;
		pdb_load_content(pdbProfile, MAX_PDB_HITAO_LINES, pdb_hiTao);
		rst = TRUE;
	    }
	    else if(instindex == 88){
		ProfSetNameByPoint(pdbProfile, defaultPdbName94);
		strcpy(pdbProfile->description, DEFAULTPDBNAME94);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_GUOPI;
		pdb_load_content(pdbProfile, MAX_PDB_GUOPI_LINES, pdb_guopi);
		rst = TRUE;
	    }
	    else if(instindex == 89){
		ProfSetNameByPoint(pdbProfile, defaultPdbName95);
		strcpy(pdbProfile->description, DEFAULTPDBNAME95);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_CAOMEIPAI;
		pdb_load_content(pdbProfile, MAX_PDB_CAOMEIPAI_LINES, pdb_caomeipai);
		rst = TRUE;
	    }
	    else if(instindex == 90){
		ProfSetNameByPoint(pdbProfile, defaultPdbName96);
		strcpy(pdbProfile->description, DEFAULTPDBNAME96);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_REDBABY;
		pdb_load_content(pdbProfile, MAX_PDB_REDBABY_LINES, pdb_redBaby);
		rst = TRUE;
	    }
	    else if(instindex == 91){
		ProfSetNameByPoint(pdbProfile, defaultPdbName97);
		strcpy(pdbProfile->description, DEFAULTPDBNAME97);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_MUYINGZHIJIA;
		pdb_load_content(pdbProfile, MAX_PDB_MUYINGZHIJIA_LINES, pdb_muyingzhijia);
		rst = TRUE;
	    }
	    else if(instindex == 92){
		ProfSetNameByPoint(pdbProfile, defaultPdbName98);
		strcpy(pdbProfile->description, DEFAULTPDBNAME98);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_YIHAODIAN;
		pdb_load_content(pdbProfile, MAX_PDB_YIHAODIAN_LINES, pdb_yihaodian);
		rst = TRUE;
	    }
	    else if(instindex == 93){
		ProfSetNameByPoint(pdbProfile, defaultPdbName99);
		strcpy(pdbProfile->description, DEFAULTPDBNAME99);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_WOMAI;
		pdb_load_content(pdbProfile, MAX_PDB_WOMAI_LINES, pdb_womai);
		rst = TRUE;
	    }
	    else if(instindex == 94){
		ProfSetNameByPoint(pdbProfile, defaultPdbName100);
		strcpy(pdbProfile->description, DEFAULTPDBNAME100);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_LASHOU;
		pdb_load_content(pdbProfile, MAX_PDB_LASHOU_LINES, pdb_lashou);
		rst = TRUE;
	    }
	    else if(instindex == 95){
		ProfSetNameByPoint(pdbProfile, defaultPdbName101);
		strcpy(pdbProfile->description, DEFAULTPDBNAME101);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_NUOMI;
		pdb_load_content(pdbProfile, MAX_PDB_NUOMI_LINES, pdb_nuomi);
		rst = TRUE;
	    }
	    else if(instindex == 96){
		ProfSetNameByPoint(pdbProfile, defaultPdbName102);
		strcpy(pdbProfile->description, DEFAULTPDBNAME102);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_MEITUAN;
		pdb_load_content(pdbProfile, MAX_PDB_MEITUAN_LINES, pdb_meituan);
		rst = TRUE;
	    }
	    else if(instindex == 97){
		ProfSetNameByPoint(pdbProfile, defaultPdbName103);
		strcpy(pdbProfile->description, DEFAULTPDBNAME103);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_GAOPENG;
		pdb_load_content(pdbProfile, MAX_PDB_GAOPENG_LINES, pdb_gaopeng);
		rst = TRUE;
	    }
	    else if(instindex == 98){
		ProfSetNameByPoint(pdbProfile, defaultPdbName104);
		strcpy(pdbProfile->description, DEFAULTPDBNAME104);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SHOP;
		pdbProfile->index = DEFAULT_PDB_TUAN55;
		pdb_load_content(pdbProfile, MAX_PDB_TUAN55_LINES, pdb_tuan55);
		rst = TRUE;
	    }
	    else if(instindex == 99){
		ProfSetNameByPoint(pdbProfile, defaultPdbName105);
		strcpy(pdbProfile->description, DEFAULTPDBNAME105);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_QQZONE;
		pdb_load_content(pdbProfile, MAX_PDB_QQZONE_LINES, pdb_QQZone);
		rst = TRUE;
	    }
	    else if(instindex == 100){
		ProfSetNameByPoint(pdbProfile, defaultPdbName106);
		strcpy(pdbProfile->description, DEFAULTPDBNAME106);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_RENREN;
		pdb_load_content(pdbProfile, MAX_PDB_RENREN_LINES, pdb_RenRen);
		rst = TRUE;
	    }
	    else if(instindex == 101){
		ProfSetNameByPoint(pdbProfile, defaultPdbName107);
		strcpy(pdbProfile->description, DEFAULTPDBNAME107);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_PENGYOU;
		pdb_load_content(pdbProfile, MAX_PDB_PENGYOU_LINES, pdb_PengYou);
		rst = TRUE;
	    }
	    else if(instindex == 102){
		ProfSetNameByPoint(pdbProfile, defaultPdbName108);
		strcpy(pdbProfile->description, DEFAULTPDBNAME108);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_WEIBOSINA;
		pdb_load_content(pdbProfile, MAX_PDB_WEIBOSINA_LINES, pdb_weiboSina);
		rst = TRUE;
	    }
	    else if(instindex == 103){
		ProfSetNameByPoint(pdbProfile, defaultPdbName109);
		strcpy(pdbProfile->description, DEFAULTPDBNAME109);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_KAIXIN;
		pdb_load_content(pdbProfile, MAX_PDB_KAIXIN001_LINES, pdb_kaixin001);
		rst = TRUE;
	    }
	    else if(instindex == 104){
		ProfSetNameByPoint(pdbProfile, defaultPdbName110);
		strcpy(pdbProfile->description, DEFAULTPDBNAME110);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_COM51;
		pdb_load_content(pdbProfile, MAX_PDB_COM51_LINES, pdb_com51);
		rst = TRUE;
	    }
	    else if(instindex == 105){
		ProfSetNameByPoint(pdbProfile, defaultPdbName111);
		strcpy(pdbProfile->description, DEFAULTPDBNAME111);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_DOUBAN;
		pdb_load_content(pdbProfile, MAX_PDB_DOUBAN_LINES, pdb_douban);
		rst = TRUE;
	    }
	    else if(instindex == 106){
		ProfSetNameByPoint(pdbProfile, defaultPdbName112);
		strcpy(pdbProfile->description, DEFAULTPDBNAME112);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_TAOMI;
		pdb_load_content(pdbProfile, MAX_PDB_TAOMI_LINES, pdb_taomi);
		rst = TRUE;
	    }
	    else if(instindex == 107){
		ProfSetNameByPoint(pdbProfile, defaultPdbName113);
		strcpy(pdbProfile->description, DEFAULTPDBNAME113);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_WEIBOTECENT;
		pdb_load_content(pdbProfile, MAX_PDB_WEIBOTECENT_LINES, pdb_weiboTencent);
		rst = TRUE;
	    }
	    else if(instindex == 108){
		ProfSetNameByPoint(pdbProfile, defaultPdbName114);
		strcpy(pdbProfile->description, DEFAULTPDBNAME114);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_JIAYUAN;
		pdb_load_content(pdbProfile, MAX_PDB_JIAYUAN_LINES, pdb_jiayuan);
		rst = TRUE;
	    }
	    else if(instindex == 109){
		ProfSetNameByPoint(pdbProfile, defaultPdbName115);
		strcpy(pdbProfile->description, DEFAULTPDBNAME115);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_JIANGHU;
		pdb_load_content(pdbProfile, MAX_PDB_JIANGHU_LINES, pdb_jianghu);
		rst = TRUE;
	    }
	    else if(instindex == 110){
		ProfSetNameByPoint(pdbProfile, defaultPdbName116);
		strcpy(pdbProfile->description, DEFAULTPDBNAME116);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_BAISOHU;
		pdb_load_content(pdbProfile, MAX_PDB_BAISOHU_LINES, pdb_baisohu);
		rst = TRUE;
	    }
	    else if(instindex == 111){
		ProfSetNameByPoint(pdbProfile, defaultPdbName117);
		strcpy(pdbProfile->description, DEFAULTPDBNAME117);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_ZHENAI;
		pdb_load_content(pdbProfile, MAX_PDB_ZHENAI_LINES, pdb_zhenai);
		rst = TRUE;
	    }
	    else if(instindex == 112){
		ProfSetNameByPoint(pdbProfile, defaultPdbName118);
		strcpy(pdbProfile->description, DEFAULTPDBNAME118);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_BAIHE;
		pdb_load_content(pdbProfile, MAX_PDB_BAIHE_LINES, pdb_baihe);
		rst = TRUE;
	    }
	    else if(instindex == 113){
		ProfSetNameByPoint(pdbProfile, defaultPdbName119);
		strcpy(pdbProfile->description, DEFAULTPDBNAME119);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_SNS;
		pdbProfile->index = DEFAULT_PDB_IPART;
		pdb_load_content(pdbProfile, MAX_PDB_IPART_LINES, pdb_ipart);
		rst = TRUE;
	    }
	    else if(instindex == 114){
		ProfSetNameByPoint(pdbProfile, defaultPdbName120);
		strcpy(pdbProfile->description, DEFAULTPDBNAME120);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_DUOWAN;
		pdb_load_content(pdbProfile, MAX_PDB_DUOWAN_LINLES, pdb_DuoWan);
		rst = TRUE;
	    }
	    else if(instindex == 115){
		ProfSetNameByPoint(pdbProfile, defaultPdbName121);
		strcpy(pdbProfile->description, DEFAULTPDBNAME121);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_BAIDUGAME;
		pdb_load_content(pdbProfile, MAX_PDB_BAIDUGAME_LINES, pdb_BaiduGame);
		rst = TRUE;
	    }
	    else if(instindex == 116){
		ProfSetNameByPoint(pdbProfile, defaultPdbName122);
		strcpy(pdbProfile->description, DEFAULTPDBNAME122);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_WAN37;
		pdb_load_content(pdbProfile, MAX_PDB_WAN37_LINES, pdb_Wan37);
		rst = TRUE;
	    }
	    else if(instindex == 117){
		ProfSetNameByPoint(pdbProfile, defaultPdbName123);
		strcpy(pdbProfile->description, DEFAULTPDBNAME123);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_G256;
		pdb_load_content(pdbProfile, MAX_PDB_G265_LINES, pdb_G265);
		rst = TRUE;
	    }
	    else if(instindex == 118){
		ProfSetNameByPoint(pdbProfile, defaultPdbName124);
		strcpy(pdbProfile->description, DEFAULTPDBNAME124);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_EEYY;
		pdb_load_content(pdbProfile, MAX_PDB_EEYY_LINES, pdb_eeyy);
		rst = TRUE;
	    }
	    else if(instindex == 119){
		ProfSetNameByPoint(pdbProfile, defaultPdbName125);
		strcpy(pdbProfile->description, DEFAULTPDBNAME125);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_KUNLUN;
		pdb_load_content(pdbProfile, MAX_PDB_KUNLUN_LINES, pdb_kunlun);
		rst = TRUE;
	    }
	    else if(instindex == 120){
		ProfSetNameByPoint(pdbProfile, defaultPdbName126);
		strcpy(pdbProfile->description, DEFAULTPDBNAME126);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_GAME4399;
		pdb_load_content(pdbProfile, MAX_PDB_GAME4399_LINES, pdb_Game4399);
		rst = TRUE;
	    }
	    else if(instindex == 121){
		ProfSetNameByPoint(pdbProfile, defaultPdbName127);
		strcpy(pdbProfile->description, DEFAULTPDBNAME127);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_GAMEPPS;
		pdb_load_content(pdbProfile, MAX_PDB_GAMEPPS_LINES, pdb_Gamepps);
		rst = TRUE;
	    }
	    else if(instindex == 122){
		ProfSetNameByPoint(pdbProfile, defaultPdbName128);
		strcpy(pdbProfile->description, DEFAULTPDBNAME128);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_GAME7K7K;
		pdb_load_content(pdbProfile, MAX_PDB_GAME7K7K_LINES, pdb_Game7k7k);
		rst = TRUE;
	    }
	    else if(instindex == 123){
		ProfSetNameByPoint(pdbProfile, defaultPdbName129);
		strcpy(pdbProfile->description, DEFAULTPDBNAME129);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_WEBGAME;
		pdbProfile->index = DEFAULT_PDB_WAN05;
		pdb_load_content(pdbProfile, MAX_PDB_WAN05_LINES, pdb_Wan05);
		rst = TRUE;
	    }
	    else if(instindex == 124){
		ProfSetNameByPoint(pdbProfile, defaultPdbName130);
		strcpy(pdbProfile->description, DEFAULTPDBNAME130);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_MAIL;
		pdbProfile->index = DEFAULT_PDB_MAILSINA;
		pdb_load_content(pdbProfile, MAX_PDB_MAILSINA_LINES, pdb_MailSina);
		rst = TRUE;
	    }
	    else if(instindex == 125){
		ProfSetNameByPoint(pdbProfile, defaultPdbName131);
		strcpy(pdbProfile->description, DEFAULTPDBNAME131);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_MAIL;
		pdbProfile->index = DEFAULT_PDB_MAIL126;
		pdb_load_content(pdbProfile, MAX_PDB_MAIL126_LINES, pdb_Mail126);
		rst = TRUE;
	    }
	    else if(instindex == 126){
		ProfSetNameByPoint(pdbProfile, defaultPdbName132);
		strcpy(pdbProfile->description, DEFAULTPDBNAME132);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_MAIL;
		pdbProfile->index = DEFAULT_PDB_MAIL163;
		pdb_load_content(pdbProfile, MAX_PDB_MAIL163_LINES, pdb_Mail163);
		rst = TRUE;
	    }
	    else if(instindex == 127){
		ProfSetNameByPoint(pdbProfile, defaultPdbName1);
		strcpy(pdbProfile->description, DEFAULTPDBNAME1);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_QQ;
		pdb_load_content(pdbProfile, MAX_PDB_QQ_LINES, pdb_QQ);
		rst = TRUE;
	    }
	    else if(instindex == 128){
		ProfSetNameByPoint(pdbProfile, defaultPdbName134);
		strcpy(pdbProfile->description, DEFAULTPDBNAME134);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_MOP;
		pdb_load_content(pdbProfile, MAX_PDB_MOP_LINES, pdb_Mop);
		rst = TRUE;
	    }
	    else if(instindex == 129){
		ProfSetNameByPoint(pdbProfile, defaultPdbName135);
		strcpy(pdbProfile->description, DEFAULTPDBNAME135);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_TIANYA;
		pdb_load_content(pdbProfile, MAX_PDB_TIANYA_LINES, pdb_TianYa);
		rst = TRUE;
	    }
	    else if(instindex == 130){
		ProfSetNameByPoint(pdbProfile, defaultPdbName136);
		strcpy(pdbProfile->description, DEFAULTPDBNAME136);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_LIBA;
		pdb_load_content(pdbProfile, MAX_PDB_LIBA_LINES, pdb_LiBa);
		rst = TRUE;
	    }
	    else if(instindex == 131){
		ProfSetNameByPoint(pdbProfile, defaultPdbName137);
		strcpy(pdbProfile->description, DEFAULTPDBNAME137);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_KDS;
		pdb_load_content(pdbProfile, MAX_PDB_KDS_LINES, pdb_Kds);
		rst = TRUE;
	    }
	    else if(instindex == 132){
		ProfSetNameByPoint(pdbProfile, defaultPdbName138);
		strcpy(pdbProfile->description, DEFAULTPDBNAME138);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_TIEXUE;
		pdb_load_content(pdbProfile, MAX_PDB_TIEXUE_LINES, pdb_TieXue);
		rst = TRUE;
	    }
	    else if(instindex == 133){
		ProfSetNameByPoint(pdbProfile, defaultPdbName139);
		strcpy(pdbProfile->description, DEFAULTPDBNAME139);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_IFENGBBS;
		pdb_load_content(pdbProfile, MAX_PDB_IFENGBBS_LINES, pdb_iFengbbs);
		rst = TRUE;
	    }
	    else if(instindex == 134){
		ProfSetNameByPoint(pdbProfile, defaultPdbName140);
		strcpy(pdbProfile->description, DEFAULTPDBNAME140);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_FORUM;
		pdbProfile->index = DEFAULT_PDB_XICI;
		pdb_load_content(pdbProfile, MAX_PDB_XICI_LINES, pdb_XiCi);
		rst = TRUE;
	    }
#if 0
	    else if(instindex == DEFAULT_PDB_GAMEURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName24);
		strcpy(pdbProfile->description, "½ûÖ¹ÓÎÏ·ÍøÕ¾");
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_GAMEURL;
		pdb_load_content(pdbProfile, MAX_PDB_GAMEURL_LINES, pdb_GAMEURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_STOCKURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName25);
		strcpy(pdbProfile->description, "½ûÖ¹Ö¤È¯ÍøÕ¾");
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_STOCKURL;
		pdb_load_content(pdbProfile, MAX_PDB_STOCKURL_LINES, pdb_STOCKURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_SHOPURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName26);
		strcpy(pdbProfile->description, "½ûÖ¹¹ºÎïÍøÕ¾");
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_SHOPURL;
		pdb_load_content(pdbProfile, MAX_PDB_SHOPURL_LINES, pdb_SHOPURL);
		rst = TRUE;
	    }
	    else if(instindex == DEFAULT_PDB_SOCIALURL){
		ProfSetNameByPoint(pdbProfile, defaultPdbName27);
		strcpy(pdbProfile->description, "½ûÖ¹Éç½»ÍøÕ¾");
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_DNS;
		pdbProfile->index = DEFAULT_PDB_SOCIALURL;
		pdb_load_content(pdbProfile, MAX_PDB_SOCIALURL_LINES, pdb_SOCIALURL);
		rst = TRUE;
	    }
#endif
	    else if(instindex == 135){
		ProfSetNameByPoint(pdbProfile, defaultPdbName141);
		strcpy(pdbProfile->description, DEFAULTPDBNAME141);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_HTTPPROXY;
		pdb_load_content(pdbProfile, MAX_PDB_HTTPPROXY_LINES, pdb_HttpProxy);
		rst = TRUE;
	    }
	    else if(instindex == 136){
		ProfSetNameByPoint(pdbProfile, defaultPdbName142);
		strcpy(pdbProfile->description, DEFAULTPDBNAME142);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_SOCKS5;
		pdb_load_content(pdbProfile, MAX_PDB_SOCKS_LINES, pdb_Socks);
		rst = TRUE;
	    }
	    else if(instindex == 137){
		ProfSetNameByPoint(pdbProfile, defaultPdbName143);
		strcpy(pdbProfile->description, DEFAULTPDBNAME143);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_FILETYPE;
		pdb_load_content(pdbProfile, MAX_PDB_FILETYPE_LINES, pdb_FileType);
		rst = TRUE;
	    }
	    else if(instindex == 138){
		ProfSetNameByPoint(pdbProfile, defaultPdbName144);
		strcpy(pdbProfile->description, DEFAULTPDBNAME144);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_POST;
		pdb_load_content(pdbProfile, MAX_PDB_WEBPOST_LINES, pdb_WebPost);
		rst = TRUE;
	    }
	    else if(instindex == 139){
		ProfSetNameByPoint(pdbProfile, defaultPdbName145);
		pdbProfile->head.active = TRUE;
		strcpy(pdbProfile->description, DEFAULTPDBNAME145);
		pdbProfile->mode = Predefined;
		pdbProfile->type = RoutePdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_10000;
		pdb_load_content(pdbProfile, (sizeof(pdb_10000) / sizeof(pdb_10000[0])), pdb_10000);
		rst = TRUE;
	    }
	    else if(instindex == 140){
		ProfSetNameByPoint(pdbProfile, defaultPdbName146);
		pdbProfile->head.active = TRUE;
		strcpy(pdbProfile->description, DEFAULTPDBNAME146);
		pdbProfile->mode = Predefined;
		pdbProfile->type = RoutePdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_10010;
		pdb_load_content(pdbProfile, (sizeof(pdb_10010) / sizeof(pdb_10010[0])), pdb_10010);
		rst = TRUE;
	    }
	    else if(instindex == 141){
		ProfSetNameByPoint(pdbProfile, defaultPdbName147);
		pdbProfile->head.active = TRUE;
		strcpy(pdbProfile->description, DEFAULTPDBNAME147);
		pdbProfile->mode = Predefined;
		pdbProfile->type = RoutePdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_10086;
		pdb_load_content(pdbProfile, (sizeof(pdb_10086) / sizeof(pdb_10086[0])), pdb_10086);
		rst = TRUE;
	    }
	    else if(instindex == 142){
		ProfSetNameByPoint(pdbProfile, defaultPdbName148);
		strcpy(pdbProfile->description, DEFAULTPDBNAME148);
		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
		pdbProfile->index = DEFAULT_PDB_ADVERURL;
		pdb_load_content(pdbProfile, MAX_PDB_ADVERURL_LINES, pdb_ADVERURL);
		rst = TRUE;
	    }
#if YAHOO_EN
 	    else if(instindex == 143){
 		ProfSetNameByPoint(pdbProfile, defaultPdbName149);
 		pdbProfile->head.active = TRUE;
 		strcpy(pdbProfile->description, DEFAULTPDBNAME149);
		pdbProfile->mode = Predefined;
 		pdbProfile->type = L7Pdb;
 		pdbProfile->category = PDB_IM;
 		pdbProfile->index = DEFAULT_PDB_yahootong;
		pdb_load_content(pdbProfile, MAX_PDB_YAHOOTONG_LINES, pdb_yahootong);
 		rst = TRUE;
 	    }
 	    else if(instindex == 144){
 		ProfSetNameByPoint(pdbProfile, defaultPdbName150);
 		strcpy(pdbProfile->description, DEFAULTPDBNAME150);
 		pdbProfile->mode = Predefined;
 		pdbProfile->type = DnsPdb;
 		pdbProfile->category = PDB_SNS;
 		pdbProfile->index = DEFAULT_PDB_facebook;
 		pdb_load_content(pdbProfile, MAX_PDB_facebook_LINES, pdb_facebook);
 		rst = TRUE;
 	    }
 	    else if(instindex == 145){
 		ProfSetNameByPoint(pdbProfile, defaultPdbName151);
 		strcpy(pdbProfile->description, DEFAULTPDBNAME151);
 		pdbProfile->mode = Predefined;
		pdbProfile->type = DnsPdb;
		pdbProfile->category = PDB_VIDEO;
       	pdbProfile->index = DEFAULT_PDB_youtobe;
 		pdb_load_content(pdbProfile, MAX_PDB_youtobe_LINES, pdb_youtobe);
 		rst = TRUE;
 	    }
 	    else if(instindex == 146){
 		ProfSetNameByPoint(pdbProfile, defaultPdbName152);
 		strcpy(pdbProfile->description, DEFAULTPDBNAME152);
 		pdbProfile->mode = Predefined;
 		pdbProfile->type = DnsPdb;
 		pdbProfile->category = PDB_SNS;
 		pdbProfile->index = DEFAULT_PDB_twitter;
 		pdb_load_content(pdbProfile, MAX_PDB_twitter_LINES, pdb_twitter);
 		rst = TRUE;
 	    }
#endif
	    else if(instindex == 147){
		ProfSetNameByPoint(pdbProfile, defaultPdbName153);
		strcpy(pdbProfile->description, DEFAULTPDBNAME153);
		pdbProfile->mode = Predefined;
		pdbProfile->type = EnterpriseQQPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_ENTERPRISE_QQ;
		pdb_load_content(pdbProfile, MAX_PDB_ENTERPRISEQQ_LINES, pdb_enterpriseQQ);
		rst = TRUE;
	    }
#if (GAME_BOOST == FYES)
	    else if(instindex == 148){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName154);
		strcpy(pdbProfile->description, DEFAULTPDBNAME154);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_LOL;
		pdb_load_content(pdbProfile, MAX_PDB_LOL_LINES, pdb_LOL);
		rst = TRUE;
	    }
	    else if(instindex == 149){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName155);
		strcpy(pdbProfile->description, DEFAULTPDBNAME155);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_DNF;
		pdb_load_content(pdbProfile, MAX_PDB_DNF_LINES, pdb_DNF);
		rst = TRUE;
	    }
	    else if(instindex == 150){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName156);
		strcpy(pdbProfile->description, DEFAULTPDBNAME156);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_CF;
		pdb_load_content(pdbProfile, MAX_PDB_CF_LINES, pdb_CF);
		rst = TRUE;
	    }
	    else if(instindex == 151){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName157);
		strcpy(pdbProfile->description, DEFAULTPDBNAME157);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_WOW1;
		pdb_load_content(pdbProfile, MAX_PDB_WOW_LINES, pdb_WOW1);
		rst = TRUE;
	    }
	    else if(instindex == 152){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName158);
		strcpy(pdbProfile->description, DEFAULTPDBNAME158);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_MHXY;
		pdb_load_content(pdbProfile, MAX_PDB_MHXY_LINES, pdb_MHXY);
		rst = TRUE;
	    }
	    else if(instindex == 153){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName159);
		strcpy(pdbProfile->description, DEFAULTPDBNAME159);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_JL;
		pdb_load_content(pdbProfile, MAX_PDB_JL_LINES, pdb_JL);
		rst = TRUE;
	    }
	    else if(instindex == 154){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName160);
		strcpy(pdbProfile->description, DEFAULTPDBNAME160);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_DOTA2;
		pdb_load_content(pdbProfile, MAX_PDB_DOTA2_LINES, pdb_DOTA2);
		rst = TRUE;
	    }
	    else if(instindex == 155){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName161);
		strcpy(pdbProfile->description, DEFAULTPDBNAME161);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_TKSJ;
		pdb_load_content(pdbProfile, MAX_PDB_TKSJ_LINES, pdb_TKSJ);
		rst = TRUE;
	    }
	    else if(instindex == 156){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName162);
		strcpy(pdbProfile->description, DEFAULTPDBNAME162);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_LZG;
		pdb_load_content(pdbProfile, MAX_PDB_LZG_LINES, pdb_LZG);
		rst = TRUE;
	    }
	    else if(instindex == 157){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName163);
		strcpy(pdbProfile->description, DEFAULTPDBNAME163);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_YXSG;
		pdb_load_content(pdbProfile, MAX_PDB_YXSG_LINES, pdb_YXSG);
		rst = TRUE;
	    }
	    else if(instindex == 158){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName164);
		strcpy(pdbProfile->description, DEFAULTPDBNAME164);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_FKJYOL;
		pdb_load_content(pdbProfile, MAX_PDB_FKJYOL_LINES, pdb_FKJYOL);
		rst = TRUE;
	    }
	    else if(instindex == 159){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName165);
		strcpy(pdbProfile->description, DEFAULTPDBNAME165);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_QQXW;
		pdb_load_content(pdbProfile, MAX_PDB_QQXW_LINES, pdb_QQXW);
		rst = TRUE;
	    }
	    else if(instindex == 160){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName166);
		strcpy(pdbProfile->description, DEFAULTPDBNAME166);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_WMSJ;
		pdb_load_content(pdbProfile, MAX_PDB_WMSJ_LINES, pdb_WMSJ);
		rst = TRUE;
	    }
	    else if(instindex == 161){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName167);
		strcpy(pdbProfile->description, DEFAULTPDBNAME167);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_LSCS;
		pdb_load_content(pdbProfile, MAX_PDB_LSCS_LINES, pdb_LSCS);
		rst = TRUE;
	    }
	    else if(instindex == 162){
#if (UTT_SMART_UI == FYES)
		pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
		ProfSetNameByPoint(pdbProfile, defaultPdbName168);
		strcpy(pdbProfile->description, DEFAULTPDBNAME168);
		pdbProfile->mode = Predefined;
		pdbProfile->type = SpeedupPdb;
		pdbProfile->category = PDB_OTHER;
		pdbProfile->index = DEFAULT_PDB_QQFC;
		pdb_load_content(pdbProfile, MAX_PDB_QQFC_LINES, pdb_QQFC);
		rst = TRUE;
	    }
 else if(instindex == 163){
#if (UTT_SMART_UI == FYES)
                pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
                ProfSetNameByPoint(pdbProfile, defaultPdbName169);
                strcpy(pdbProfile->description, DEFAULTPDBNAME169);
                pdbProfile->mode = Predefined;
                pdbProfile->type = SpeedupPdb;
                pdbProfile->category = PDB_OTHER;
                pdbProfile->index = DEFAULT_PDB_QQYY;
                pdb_load_content(pdbProfile, MAX_PDB_QQYY_LINES, pdb_QQYY);
                rst = TRUE;
            }
 else if(instindex == 164){
#if (UTT_SMART_UI == FYES)
                pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
                ProfSetNameByPoint(pdbProfile, defaultPdbName170);
                strcpy(pdbProfile->description, DEFAULTPDBNAME170);
                pdbProfile->mode = Predefined;
                pdbProfile->type = SpeedupPdb;
                pdbProfile->category = PDB_OTHER;
                pdbProfile->index = DEFAULT_PDB_CCTV;
                pdb_load_content(pdbProfile, MAX_PDB_CCTV_LINES, pdb_CCTV);
                rst = TRUE;
            }
 else if(instindex == 165){
#if (UTT_SMART_UI == FYES)
                pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
                ProfSetNameByPoint(pdbProfile, defaultPdbName171);
                strcpy(pdbProfile->description, DEFAULTPDBNAME171);
                pdbProfile->mode = Predefined;
                pdbProfile->type = SpeedupPdb;
                pdbProfile->category = PDB_OTHER;
                pdbProfile->index = DEFAULT_PDB_SINA;
                pdb_load_content(pdbProfile, MAX_PDB_SINA_LINES, pdb_SINA);
                rst = TRUE;
            }
 else if(instindex == 166){
#if (UTT_SMART_UI == FYES)
                pdbProfile->head.active = FALSE;
#endif
		pdbProfile->head.active = FALSE;
                ProfSetNameByPoint(pdbProfile, defaultPdbName172);
                strcpy(pdbProfile->description, DEFAULTPDBNAME172);
                pdbProfile->mode = Predefined;
                pdbProfile->type = SpeedupPdb;
                pdbProfile->category = PDB_OTHER;
                pdbProfile->index = DEFAULT_PDB_QQZB;
                pdb_load_content(pdbProfile, MAX_PDB_QQZB_LINES, pdb_QQZB);
                rst = TRUE;
            }
#endif
        else if(instindex == 167){
                ProfSetNameByPoint(pdbProfile, defaultPdbName173);
                strcpy(pdbProfile->description, DEFAULTPDBNAME173);
                pdbProfile->mode = Predefined;
                pdbProfile->type = PortPdb;
                pdbProfile->category = PDB_STOCK;
                pdbProfile->index = DEFAULT_PDB_STOCKSW;
                pdb_load_content(pdbProfile, MAX_PDB_STOCKSW_LINES, pdb_STOCKSW);
                rst = TRUE;
        }
#if (WIFIDOG_AUTH == FYES)
/*Î¢ÐÅ²ßÂÔ¸üÐÂ*/
        else if(instindex == 168){
                ProfSetNameByPoint(pdbProfile, defaultPdbName174);
		pdbProfile->head.active = TRUE;
                strcpy(pdbProfile->description, DEFAULTPDBNAME174);
                pdbProfile->mode = Predefined;
                pdbProfile->type = wifidogUp;
                pdbProfile->category = PDB_OTHER;
                pdbProfile->index = DEFAULT_PDB_WECHAT;
                pdb_load_content(pdbProfile, MAX_PDB_WECHAT_LINES, pdb_WECHAT);
                rst = TRUE;
        }
#endif
        else if(instindex == 169){
                ProfSetNameByPoint(pdbProfile, defaultPdbName175);
                strcpy(pdbProfile->description, DEFAULTPDBNAME175);
		pdbProfile->mode = Predefined;
		pdbProfile->type = L7Pdb;
                pdbProfile->category = PDB_IM;
		pdbProfile->index = DEFAULT_PDB_WEXIN;
                pdb_load_content(pdbProfile, MAX_PDB_WEXIN_LINES, pdb_Wechat);
                rst = TRUE;
        }



	    break;
#endif
#if (DMZ == FYES)
	case MIB_PROF_DMZ_NAT:
	    if(instindex == 0){
            dmzProfile = (DMZProfile*)defprof;
            dmzProfile->head.active = FALSE;
            strcpy(dmzProfile->head.name , "DMZ");
            dmzProfile->DMZIP = 0;
            rst = TRUE;
	    }
	    break;
#endif
#if (FEATURE_NTP == FYES)
	case MIB_PROF_NTP:
	    if (instindex == 0) {
		ntpProfile = (NtpProfile *)defprof;  
		ProfSetNameByPoint(ntpProfile, "ntpconf");
                ntpProfile->head.active = TRUE;
                ntpProfile->enable = TRUE;
#if (DST == FYES)
                ntpProfile->dstEnable = FALSE;
#endif
                ntpProfile->NTPServerIP1 = inet_addr(NTP_DEFAULT_SERV1) ;
                ntpProfile->NTPServerIP2 = inet_addr(NTP_DEFAULT_SERV2) ;
		ntpProfile->time_zone = NTP_DEFAULT_TZ;
		rst = TRUE;
	    }
	    break;
#endif
#if (FEATURE_LANG == FYES)
	case MIB_PROF_LANG:
            if (instindex == 0) {
                langProfile = (LangProfile *)defprof;  
                ProfSetNameByPoint(langProfile, "Lang");
                langProfile->head.active = TRUE;
		strcpy(langProfile->lang, LANG_DEFAULT);
		rst = TRUE;
	    }
	    break;
#endif
#if (TASK_SCHEDULER == FYES)
	case MIB_PROF_TASK_SCHED:
	    taskSchedProfile = (TaskSchedProfile*)defprof;
	    memset(taskSchedProfile->head.name, 0, UTT_INST_NAME_LEN);
	    taskSchedProfile->head.active = FALSE;
	    memset(taskSchedProfile->pMin, 0, sizeof(taskSchedProfile->pMin));
	    memset(taskSchedProfile->pHour, 0, sizeof(taskSchedProfile->pHour));
	    memset(taskSchedProfile->pWeek, 0, sizeof(taskSchedProfile->pWeek));
	    memset(taskSchedProfile->pType, 0, sizeof(taskSchedProfile->pType));
	    memset(taskSchedProfile->pCmd, 0, sizeof(taskSchedProfile->pCmd));
	    rst = TRUE;
	    break;
#endif
#if (FIREWALL == FYES)
	case MIB_PROF_FIRE_WALL:
	    fireWallProfile = (FireWallProfile*)defprof;
	    memset(fireWallProfile->head.name, 0, UTT_INST_NAME_LEN);
	    rst = TRUE;
	    break;
#endif
#if (NOTICE == FYES)
	case MIB_PROF_NOTICE:
	    noticeProfile = (NoticeProfile*)defprof;
	    noticeProfile->head.active = FALSE;
	    rst = TRUE;
	    if (instindex == 0) 
	    {
		ProfSetNameByPoint(noticeProfile, "notice");
	    }
	    else if(instindex == 1)
	    {
		ProfSetNameByPoint(noticeProfile, "timenotice");
	    }
	    else if(instindex == 2)
	    {
		ProfSetNameByPoint(noticeProfile, "dnsnotice");
	    }
	    break;
#endif
#if (WEB_AUTH == FYES)
	case MIB_PROF_WEB_AUTH_GLOBAL:
	    webAuthGlobalProfile = (WebAuthGlobalProfile*)defprof;
	    ProfSetNameByPoint(webAuthGlobalProfile,"webauthglobal");
	    webAuthGlobalProfile->head.active = FALSE;
#if (WIFIDOG_AUTH == FYES)
	    webAuthGlobalProfile->authType = 1;	    /*Ä¬ÈÏ¿ªÆô±¾µØwebÈÏÖ¤*/
	    memset(webAuthGlobalProfile->testDomain, 0, sizeof(webAuthGlobalProfile->testDomain));
	    memset(webAuthGlobalProfile->login_url, 0, sizeof(webAuthGlobalProfile->login_url));
	    memset(webAuthGlobalProfile->auth_url, 0, sizeof(webAuthGlobalProfile->auth_url));
	    memset(webAuthGlobalProfile->success_url, 0, sizeof(webAuthGlobalProfile->success_url));
	    memset(webAuthGlobalProfile->ping_url, 0, sizeof(webAuthGlobalProfile->ping_url));
#if (WIFIDOG_MAC_AUTH == FYES)
	    webAuthGlobalProfile->wifiData.macAuthEn = 1;
#endif
#if (SMS_ENABLE ==1)
	    webAuthGlobalProfile->smsEnable = 0;	    /*Ä¬ÈÏ¶ÌÐÅ·¢ËÍ¹Ø±Õ*/
	    webAuthGlobalProfile->cdmaType = 2;	    /*Ä¬ÈÏÔËÓªÉÌÎªÁªÍ¨*/
	    memset(webAuthGlobalProfile->province, 0, sizeof(webAuthGlobalProfile->province));
	    memset(webAuthGlobalProfile->city, 0, sizeof(webAuthGlobalProfile->city));
	    memset(webAuthGlobalProfile->sms_number, 0, sizeof(webAuthGlobalProfile->sms_number));
#endif
#endif
	    webAuthGlobalProfile->enpicture = 0;
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	    webAuthGlobalProfile->idcard.enabled = 0;
	    webAuthGlobalProfile->idcard.timeTotal = 48;
	    webAuthGlobalProfile->idcard.maxSessions = 1;
#endif
#if (WEBAUTH_AUTO == FYES)
	    strcpy(webAuthGlobalProfile->logServer, "116.236.120.162");
	    webAuthGlobalProfile->sms.enabled = 0;
	    webAuthGlobalProfile->sms.timeTotal = 2;
	    webAuthGlobalProfile->sms.maxSessions = 1;
	    webAuthGlobalProfile->sms.totalTimes = 0;
	    strcpy(webAuthGlobalProfile->sms.smsServer, "sms.greenwifi.com.cn");
	    strcpy(webAuthGlobalProfile->sms.contents, "ÑéÖ¤ÂëÇ##£¬ÉÏÍøÓÐÐ§ÆÚÎª**¡£ÄúÒ²¿ÉÒÔµã»÷Á´½Ó£º$$ £¬Ö±½ÓÈÏÖ¤ÉÏÍø¡£¡¾°¬Ì©¿Æ¼¼¡¿");
	    webAuthGlobalProfile->wechat.enabled = 0;
	    webAuthGlobalProfile->wechat.timeTotal = 2;
	    webAuthGlobalProfile->wechat.maxSessions = 1;
	    webAuthGlobalProfile->wechat.totalTimes = 0;
	    strcpy(webAuthGlobalProfile->wechat.wechatKeyContents, "ÉÏÍø/wifi");
	    strcpy(webAuthGlobalProfile->wechat.contents, "ÑéÖ¤ÂëÊÇ##£¬ÉÏÍøÓÐÐ§ÆÚÎª**¡£ÄúÒ²¿ÉÒÔµã»÷Á´½Ó£º$$ £¬Ö±½ÓÈÏÖ¤ÉÏÍø¡£");
	    memset(webAuthGlobalProfile->wechat.wechatName, 0, sizeof(webAuthGlobalProfile->wechat.wechatName));
	    memset(webAuthGlobalProfile->wechat.wechatToken, 0, sizeof(webAuthGlobalProfile->wechat.wechatToken));
#endif
#if (WEBAUTH_ADVANCE == FYES)
	    webAuthGlobalProfile->enDefPage = 1;
	    memset(webAuthGlobalProfile->pageName, 0, sizeof(webAuthGlobalProfile->pageName));
	    memset(webAuthGlobalProfile->picName, 0, sizeof(webAuthGlobalProfile->picName));
#endif
#if (WEBAUTH_SELF_HELP == FYES)
	    webAuthGlobalProfile->self_help = FUN_DISABLE;
#endif
#if(WEB_AUTH_STALE == FYES)
	    webAuthGlobalProfile->staleT = 10U;
#endif
#if (UTT_KNOWIFI == FYES)
	    webAuthGlobalProfile->knowifi_enable = 0;
	    strcpy(webAuthGlobalProfile->knowifi_routeId, "unknown");
#endif
	    webAuthGlobalProfile->active_pic = 0;
	    memset(webAuthGlobalProfile->exceptIpGroup, 0, sizeof(webAuthGlobalProfile->exceptIpGroup));
	    memset(webAuthGlobalProfile->tipstitle, 0, sizeof(webAuthGlobalProfile->tipstitle));
	    memset(webAuthGlobalProfile->tipsinfo, 0, sizeof(webAuthGlobalProfile->tipsinfo));
	    memset(webAuthGlobalProfile->contact, 0, sizeof(webAuthGlobalProfile->contact));
	    memset(webAuthGlobalProfile->pictureUrl, 0, sizeof(webAuthGlobalProfile->pictureUrl));
	    rst = TRUE;
	    break;
	case MIB_PROF_WEB_AUTH:
	    webAuthProfile = (WebAuthProfile*)defprof;
	    memset(webAuthProfile->head.name, 0, UTT_INST_NAME_LEN);
	    webAuthProfile->head.active = FALSE;
#if ((WEBAUTH_AUTO == FYES) || (WEBAUTH_IDCODE == FYES))
	case MIB_PROF_WEB_AUTH_CPT:
	    webAuthProfile = (WebAuthProfile*)defprof;
	    memset(webAuthProfile->head.name, 0, UTT_INST_NAME_LEN);
	    webAuthProfile->head.active = FALSE;
#endif
#if (WEBAUTH_SELF_HELP == FYES)
	    webAuthProfile->optCnt = 0U;
#endif 
	    rst = TRUE;
	    break;
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == FYES)
	case MIB_PROF_WHITE_LIST:
		whiteListProfile = (WhiteListProfile *)defprof;
		whiteListProfile->head.active = 0;
		memset(whiteListProfile->domain, 0, sizeof(whiteListProfile->domain));
		rst = TRUE;
	break;
#endif
#endif
#if (EXCEPT_QQ == FYES)
	case MIB_PROF_EXCEPT_QQ:
		exceptQQProfile = (ExceptQQProfile *)defprof;
		exceptQQProfile->head.active = 0;
		exceptQQProfile->qqNumber = 0U;
		exceptQQProfile->phoneHead = 0U;
		memset(exceptQQProfile->remark, 0, sizeof(exceptQQProfile->remark));
		rst = TRUE;
	break;
#endif
#if (EXCEPT_MSN == FYES)
	case MIB_PROF_EXCEPT_MSN:
		exceptMSNProfile = (ExceptMSNProfile *)defprof;
		exceptMSNProfile->head.active = 0;
		memset(exceptMSNProfile->msnNumber, 0, sizeof(exceptMSNProfile->msnNumber));
		memset(exceptMSNProfile->remark, 0, sizeof(exceptMSNProfile->remark));
		rst = TRUE;
	break;
#endif
#if (EXCEPT_ALI == FYES)
	case MIB_PROF_EXCEPT_ALI:
		exceptAliProfile = (ExceptAliProfile *)defprof;
		exceptAliProfile->head.active = 0;
		memset(exceptAliProfile->aliNumber, 0, sizeof(exceptAliProfile->aliNumber));
		memset(exceptAliProfile->remark, 0, sizeof(exceptAliProfile->remark));
		rst = TRUE;
	break;
#endif
#if (LOG_MANAGE == FYES)
	case MIB_PROF_LOG_MANAGE:
		logManageProfile = (LogManageProfile *)defprof;
		logManageProfile->head.active = FALSE;
		ProfSetNameByPoint(logManageProfile,"logmanage");
		rst = TRUE;
	break;
#endif
#if (AP_LOG_MANAGE == FYES)
	case MIB_PROF_AP_LOG_MANAGE:
		apLogManageProfile = (ApLogManageProfile *)defprof;
		apLogManageProfile->head.active = FALSE;
		ProfSetNameByPoint(apLogManageProfile,"aplogmanage");
		apLogManageProfile->apLogType[APLINKLOG] = LOG_ENABLE;
		apLogManageProfile->apLogType[APUSERLOG] = LOG_DISABLE;
		apLogManageProfile->apLogType[APUSERNUM] = LOG_DISABLE;
		rst = TRUE;
	break;
#endif
#if (IP_SEC == FYES)
	case MIB_PROF_IPSEC:
		ipsecProfile = (ipsecConfProfile *)defprof;
		ipsecProfile->head.active = FALSE;
		ipsecProfile->head.active=FUN_ENABLE;
		ipsecProfile->sa_conn=0U;
		ipsecProfile->flag=0;
#if (VPN_CONTROL == FYES)
		ipsecProfile->sa_state=0U;
#endif
		rst = TRUE;
	break;
	case MIB_PROF_TRANSFORMSET:
		ipsectransformsetProfile = (ipsecTransformSetProfile *)defprof;
		ipsectransformsetProfile->head.active = FALSE;
		rst = TRUE;
	break;
	case MIB_PROF_ISAKMP:
		isakmpProfile = (isakmpConfProfile *)defprof;
		isakmpProfile->head.active = FALSE;
		rst = TRUE;
	break;
	case MIB_PROF_ISAKMP_POLICY:
		isakmppolicyProfile = (isakmpPolicyProfile *)defprof;
		isakmppolicyProfile->head.active = FALSE;
		rst = TRUE;
	break;
#endif
#if (SYSLOG == FYES)
	case MIB_PROF_SYSLOG:
		syslogProfile = (SyslogProfile *)defprof;
		syslogProfile->head.active = FALSE;
		ProfSetNameByPoint(syslogProfile,"syslog");
		rst = TRUE;
	break;
#endif
#if (CWMP == FYES)
    case MIB_PROF_CWMP:
		cwmpProfile = (CwmpProfile *)defprof;
		cwmpProfile->head.active = TRUE;
		ProfSetNameByPoint(cwmpProfile,"cwmp");
#if(WECHAT_CWMP == FYES)
        strcpy(cwmpProfile->acsUrl,"http://acs.greenwifi.com.cn:80/wx/tr069");
#else
        strcpy(cwmpProfile->acsUrl,"http://acs.greenwifi.com.cn:80/ACS/tr069");
#endif
        cwmpProfile->periodicInformInterval = 300;
        cwmpProfile->stunPeriodicInterval   = 30;
#if 1
        strcpy(cwmpProfile->stunUrl,"stun.greenwifi.com.cn:3478");
#else
        strcpy(cwmpProfile->stunIp,"200.200.202.70");
        cwmpProfile->stunPort = 3478;
#endif
		rst = TRUE;
	break;
#endif
#if (FAT_FIT_SWITCH == FYES)
	case MIB_PROF_FAT_FIT_SWITCH:
		if (instindex == 0) {
		fatFitSwitchProfile = (FatFitSwitchProfile *)defprof;
		fatFitSwitchProfile->head.active = TRUE;
		fatFitSwitchProfile->fatEnable = PROF_DISABLE;

		ProfSetNameByPoint(fatFitSwitchProfile, "apSwitch");
		rst = TRUE;
		}
	break;
#endif
#if (AC_WORK_MODE == FYES)
	case MIB_PROF_AC_WORK_MODE:
		if (instindex == 0) {
		acWorkModeProfile = (AcWorkModeProfile*)defprof;
		acWorkModeProfile ->head.active = TRUE;
#if (UTT_NV_WX20S == FYES)
		acWorkModeProfile ->workMode= PROF_ENABLE;
#else
		acWorkModeProfile ->workMode= PROF_DISABLE;
#endif
		acWorkModeProfile->gateWay = htonl(0x00000000);
		acWorkModeProfile->pDns = htonl(0x00000000);

		ProfSetNameByPoint(acWorkModeProfile, "acMode");
		rst = TRUE;
		}
	break;
#endif

#if (AP_LOAD_BALANCE == FYES)
    case MIB_PROF_AP_LOAD_BALANCE:
		apLoadBalanceProfile = (ApLoadBalanceProfile*)defprof;
		apLoadBalanceProfile->head.active = FALSE;
		apLoadBalanceProfile->source_sum = 32u;
		rst = TRUE;
	break;

#endif
#if (AP_PERMISSION == FYES)
	case MIB_PROF_AP_PERMISSION:
		if (instindex == 0) {
			apPermissionProfile = (ApPermissionProfile *)defprof;
            ProfSetNameByPoint(apPermissionProfile, "P0");
			strcpy(apPermissionProfile->head.name, "P0");
			strcpy(apPermissionProfile->ap_name, "default");
			apPermissionProfile->head.active = TRUE;
			apPermissionProfile->action      = PROF_ENABLE;
			apPermissionProfile->num_start   = 0;
			apPermissionProfile->num_end     = 0;
			rst = TRUE;
		}
		break;
#endif

#if (AP_CONF == FYES)
	case MIB_PROF_AP_CONF:
		if (instindex == 0) {
			apConfProfile = (ApConfProfile *)defprof;
			ProfSetNameByPoint(apConfProfile, "default");
			apConfProfile->head.active = TRUE;
			memset(apConfProfile->types, '\0', sizeof(apConfProfile->types));
			memset(apConfProfile->softVersion, '\0', sizeof(apConfProfile->softVersion));
			memset(apConfProfile->firmVersion, '\0', sizeof(apConfProfile->firmVersion));
			rst = TRUE;
		}
		break;
#endif

#if (SERVICE_ZONE == FYES)
	case MIB_PROF_SERVICE_ZONE:
		if (instindex == 0) {
			serviceZoneProfile = (ServiceZoneProfile *)defprof;
            ProfSetNameByPoint(serviceZoneProfile, "default");
			serviceZoneProfile->head.active = TRUE;
			serviceZoneProfile->serviceEn = PROF_ENABLE;
			strcpy(serviceZoneProfile->ssid, "ssid_1");
#if(SSID_ENCODE_TYPE == FYES)
			serviceZoneProfile->ssid_encode_type = 0;
#endif
			serviceZoneProfile->ssidBcast = PROF_ENABLE;
			serviceZoneProfile->ssidIsolate = PROF_DISABLE;
			serviceZoneProfile->vid = 0;
			serviceZoneProfile->client = 32u;
			serviceZoneProfile->dhcpEn = PROF_ENABLE;
			serviceZoneProfile->gw = htonl(0xc0a80101);
			serviceZoneProfile->netmask = htonl(0xffffff00);
			serviceZoneProfile->virif = htonl(0xc0a80101);
			serviceZoneProfile->securMode = NONE;
			memset(serviceZoneProfile->strApVid, 0, sizeof(serviceZoneProfile->strApVid));
			rst = TRUE;
		}
#if (SZ_SHARE_LIMIT_AC == FYES)
		serviceZoneProfile = (ServiceZoneProfile *)defprof;
		serviceZoneProfile->limit_type = 0u;
		serviceZoneProfile->limit_down = 0u;
		serviceZoneProfile->limit_up = 0u;
		rst = TRUE;
#endif
		break;
#endif

#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	case MIB_PROF_AP_MAC_FILTER:
		    apMacFilterProfile = (ApMacFilterProfile *)defprof;
		    apMacFilterProfile->head.active = FALSE;
			memset(apMacFilterProfile->head.name, '\0', UTT_INST_NAME_LEN);
#if (FIT_AP_MAC_FILTER == FYES)
			if (instindex >= 4)
			{
			    sprintf(tmpStr, "BlackList%d", instindex+1);
			    ProfSetNameByPoint(apMacFilterProfile, tmpStr);
			}
#endif
		    apMacFilterProfile->mode = 0;
		    apMacFilterProfile->num = 0;
			memset(apMacFilterProfile->sz, '\0', sizeof(apMacFilterProfile->sz));
			memset(apMacFilterProfile->mac, '\0', sizeof(apMacFilterProfile->mac));
			rst = TRUE;
		break;
#endif

#if (FEATURE_AP_MAIL == FYES)
	case MIB_PROF_AP_MAIL:
		if (instindex == 0) {
				apMailProfile = (ApMailProfile*)defprof;
				apMailProfile->head.active = FALSE;
				ProfSetNameByPoint(apMailProfile, "apMail");
				apMailProfile->spaceTime = 60;
				memset(apMailProfile->senderEmail,'\0',sizeof(apMailProfile->senderEmail));
				memset(apMailProfile->senderEmailPasswd,'\0',sizeof(apMailProfile->senderEmailPasswd));
				memset(apMailProfile->receiverEmail,'\0',sizeof(apMailProfile->receiverEmail));
				memset(apMailProfile->smtpServer,'\0',sizeof(apMailProfile->smtpServer));
				memset(apMailProfile->emailTitle,'\0',sizeof(apMailProfile->emailTitle));
				rst = TRUE;
		}
		break;
#endif

#if (FEATURE_AP_CONF_TEMP == FYES)
	case MIB_PROF_AP_CONF_TEMP:
		    apConfTempProfile = (ApConfTempProfile *)defprof;
		    apConfTempProfile->head.active = FALSE;
			memset(apConfTempProfile->head.name, 0, UTT_INST_NAME_LEN);
			memset(apConfTempProfile->tempName, 0, sizeof(apConfTempProfile->tempName));
		break;
#endif
#if (FEATURE_AP_SCAN == FYES)
	case MIB_PROF_AP_SCAN:
		if (instindex == 0) {
		    apScanProfile = (ApScanProfile*)defprof;
		    apScanProfile->head.active = TRUE;
		    apScanProfile->maxAp = MAX_AP_MANAGED;
		    apScanProfile->autoScan = TRUE;
		    apScanProfile->scanAll = PROF_DISABLE;
		    ProfSetNameByPoint(apScanProfile, "apScanf");
		    rst = TRUE;
		}
		break;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
	case MIB_PROF_AP_SW_MNG:
		if (instindex == 0) {
		    apSwMngProfile = (ApSwMngProfile*)defprof;
		    apSwMngProfile->head.active = FALSE;
		    memset(apSwMngProfile->head.name, 0, sizeof(apSwMngProfile->head.name));
		    memset(apSwMngProfile->sw_name, 0, sizeof(apSwMngProfile->sw_name));
		    memset(apSwMngProfile->sw_version, 0, sizeof(apSwMngProfile->sw_version));
		    memset(apSwMngProfile->sw_model, 0, sizeof(apSwMngProfile->sw_model));
		    memset(apSwMngProfile->hw_version, 0, sizeof(apSwMngProfile->hw_version));
		    apSwMngProfile->defaulted = 0u;
		    rst = TRUE;
		}
		break;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
	case MIB_PROF_AP_BASIC_CONF:
		if (instindex == 0) {
		    apBasicConfProfile = (ApBasicConfProfile*)defprof;
		    apBasicConfProfile->head.active = TRUE;
		    strcpy(apBasicConfProfile->dev_name, "");
		    apBasicConfProfile->dhcp_en = 1;
		    apBasicConfProfile->ip_addr = 0;
		    apBasicConfProfile->net_mask = 0xFFFFFF00;
		    strcpy(apBasicConfProfile->password, "admin");
		    //apBasicConfProfile->max_clt = 32u;
		    apBasicConfProfile->channel = 0u;
                    apBasicConfProfile->dbm = 0u;
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
                    apBasicConfProfile->dbm_5g = 0u;
#endif
		    ProfSetNameByPoint(apBasicConfProfile, "apBasicConf");
		    rst = TRUE;
		}
		break;
#endif
#if (CAPWAP_CONFIG_AP == FYES)
	case MIB_PROF_AP_CAPWAP_CONFIG:
		if(0 == instindex)
		{
		    apCapwapConfigProfile = (ApCapwapConfigProfile *)defprof;
		    apCapwapConfigProfile->head.active = FALSE;
		    apCapwapConfigProfile->acHostInfo[0][0] = '\0';
		    apCapwapConfigProfile->acHostInfo[1][0] = '\0';
		    apCapwapConfigProfile->acHostInfo[2][0] = '\0';
		    ProfSetNameByPoint(apCapwapConfigProfile, "apCapwap");
		    rst = TRUE;
		}
		break;
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
	case MIB_PROF_AP_EFFECT_CONF:
		if (instindex == 0) {
		apEffectConfProfile = (ApEffectConfProfile *)defprof;
		apEffectConfProfile->head.active = TRUE;
		apEffectConfProfile->config = PROF_DISABLE;

		ProfSetNameByPoint(apEffectConfProfile, "apEffectConf");
		rst = TRUE;
		}
	break;
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)
	case MIB_PROF_FIT_AP_SZ:
		if (instindex == 0) {
		fitApSzProfile = (FitApSzProfile *)defprof;
		fitApSzProfile->head.active = FALSE;
		strcpy(fitApSzProfile->ssid, "UTT-HIPER_1");
		fitApSzProfile->ssidBcast = PROF_ENABLE;
		fitApSzProfile->ssidIsolate = PROF_DISABLE;
		fitApSzProfile->vid = 0u;
//		fitApSzProfile->client = 32u;
		fitApSzProfile->client = 0u;
//		fitApSzProfile->dbm = 0;
		fitApSzProfile->securMode = NONE;
		fitApSzProfile->wepSecurity.auMethod = AUTO_AUTH;
		fitApSzProfile->wepSecurity.kType = HEX;
		fitApSzProfile->wepSecurity.kNum1.key_state_action = PROF_ENABLE;
		memset(fitApSzProfile->wepSecurity.kNum1.passwd, 0, sizeof(fitApSzProfile->wepSecurity.kNum1.passwd));
		fitApSzProfile->wepSecurity.kNum1.kFmt = FORBID;
		fitApSzProfile->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
		memset(fitApSzProfile->wepSecurity.kNum2.passwd, 0, sizeof(fitApSzProfile->wepSecurity.kNum2.passwd));
		fitApSzProfile->wepSecurity.kNum2.kFmt = FORBID;
		fitApSzProfile->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
		memset(fitApSzProfile->wepSecurity.kNum3.passwd, 0, sizeof(fitApSzProfile->wepSecurity.kNum3.passwd));
		fitApSzProfile->wepSecurity.kNum3.kFmt = FORBID;
		fitApSzProfile->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
		memset(fitApSzProfile->wepSecurity.kNum4.passwd, 0, sizeof(fitApSzProfile->wepSecurity.kNum4.passwd));
		fitApSzProfile->wepSecurity.kNum4.kFmt = FORBID;
		fitApSzProfile->wpaSecurity.wVersion = AUTO_WPA;
		fitApSzProfile->wpaSecurity.sAlg = AUTO_SEC;
		fitApSzProfile->wpaSecurity.radiusIp = 0u;
		fitApSzProfile->wpaSecurity.radiusPort = 1812u;
		memset(fitApSzProfile->wpaSecurity.radiusPasswd, 0, sizeof(fitApSzProfile->wpaSecurity.radiusPasswd));
		fitApSzProfile->wpaSecurity.updataTime = 0u;
		fitApSzProfile->wpaPskSecurity.wVersion =AUTO_WPA ;
		fitApSzProfile->wpaPskSecurity.sAlg = AUTO_SEC;
		memset(fitApSzProfile->wpaPskSecurity.sharePasswd, 0, sizeof(fitApSzProfile->wpaPskSecurity.sharePasswd));
		fitApSzProfile->wpaPskSecurity.updataTime =0u;

		memset(fitApSzProfile->head.name, '\0', sizeof(fitApSzProfile->head.name));
		rst = TRUE;
		}
	break;
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)
#if (WIRELESS_5G == FYES)
	case MIB_PROF_FIT_AP_SZ_5G:
	if (instindex == 0)
	{
	    fitAp5gSzProfile = (FitAp5gSzProfile *)defprof;
	    fitAp5gSzProfile->head.active = FALSE;
	    strcpy(fitAp5gSzProfile->ssid, "UTT-HIPER_5G_1");
	    fitAp5gSzProfile->ssidBcast = PROF_ENABLE;
	    fitAp5gSzProfile->ssidIsolate = PROF_DISABLE;
	    fitAp5gSzProfile->vid = 0u;
//	    fitAp5gSzProfile->client = 32u;
	    fitAp5gSzProfile->client = 0u;
//	    fitAp5gSzProfile->dbm = 0;
	    fitAp5gSzProfile->securMode = NONE;
	    fitAp5gSzProfile->wepSecurity.auMethod = AUTO_AUTH;
	    fitAp5gSzProfile->wepSecurity.kType = HEX;
	    fitAp5gSzProfile->wepSecurity.kNum1.key_state_action = PROF_ENABLE;
	    memset(fitAp5gSzProfile->wepSecurity.kNum1.passwd, 0, sizeof(fitAp5gSzProfile->wepSecurity.kNum1.passwd));
	    fitAp5gSzProfile->wepSecurity.kNum1.kFmt = FORBID;
	    fitAp5gSzProfile->wepSecurity.kNum2.key_state_action = PROF_DISABLE;
	    memset(fitAp5gSzProfile->wepSecurity.kNum2.passwd, 0, sizeof(fitAp5gSzProfile->wepSecurity.kNum2.passwd));
	    fitAp5gSzProfile->wepSecurity.kNum2.kFmt = FORBID;
	    fitAp5gSzProfile->wepSecurity.kNum3.key_state_action = PROF_DISABLE;
	    memset(fitAp5gSzProfile->wepSecurity.kNum3.passwd, 0, sizeof(fitAp5gSzProfile->wepSecurity.kNum3.passwd));
	    fitAp5gSzProfile->wepSecurity.kNum3.kFmt = FORBID;
	    fitAp5gSzProfile->wepSecurity.kNum4.key_state_action = PROF_DISABLE;
	    memset(fitAp5gSzProfile->wepSecurity.kNum4.passwd, 0, sizeof(fitAp5gSzProfile->wepSecurity.kNum4.passwd));
	    fitAp5gSzProfile->wepSecurity.kNum4.kFmt = FORBID;
	    fitAp5gSzProfile->wpaSecurity.wVersion = AUTO_WPA;
	    fitAp5gSzProfile->wpaSecurity.sAlg = AUTO_SEC;
	    fitAp5gSzProfile->wpaSecurity.radiusIp = 0u;
	    fitAp5gSzProfile->wpaSecurity.radiusPort = 1812u;
	    memset(fitAp5gSzProfile->wpaSecurity.radiusPasswd, 0, sizeof(fitAp5gSzProfile->wpaSecurity.radiusPasswd));
	    fitAp5gSzProfile->wpaSecurity.updataTime = 0u;
	    fitAp5gSzProfile->wpaPskSecurity.wVersion =AUTO_WPA ;
	    fitAp5gSzProfile->wpaPskSecurity.sAlg = AUTO_SEC;
	    memset(fitAp5gSzProfile->wpaPskSecurity.sharePasswd, 0, sizeof(fitAp5gSzProfile->wpaPskSecurity.sharePasswd));
	    fitAp5gSzProfile->wpaPskSecurity.updataTime =0u;

	    memset(fitAp5gSzProfile->head.name, '\0', sizeof(fitAp5gSzProfile->head.name));
	    rst = TRUE;
	}
	break;
#endif
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
        case MIB_PROF_AP_BLACK_WHITE_LIST:
		apBlackWhiteListProfile = (ApBlackWhiteListProfile*)defprof;
		apBlackWhiteListProfile ->head.active = FALSE;
		sprintf(tmpStr, "BlackList%d", instindex+1);
		ProfSetNameByPoint(apBlackWhiteListProfile, tmpStr);
		rst = TRUE;
	break;
#endif
#if (UTT_LICENSE == FYES)
	case MIB_PROF_LICENSE:
	    licenseProfile = (LicenseProfile *)defprof;
	    licenseProfile->head.active = TRUE;
	    licenseProfile->authResult = 0xff;
	    if (instindex == 0) {
		ProfSetNameByPoint(licenseProfile, "gloLicense");
	    }
	    if (instindex == 1)
	    {
		ProfSetNameByPoint(licenseProfile, "tmpLicense");
	    }
	    rst = TRUE;
	    break;
#endif
#if (POLICY_ROUTE == FYES)
	case MIB_PROF_POLICY_ROUTE:
	    policyrouteProfile = (PolicyRouteProfile *)defprof;
	    policyrouteProfile->head.active = TRUE;
	    rst = TRUE;
	    break;
#endif
#if (P2P_LIMIT == FYES)
	case MIB_PROF_P2P_LIMIT:
	    p2pLimitProfile = (P2PLimitProfile *)defprof;
	    p2pLimitProfile->head.active = FALSE;
	    ProfSetNameByPoint(p2pLimitProfile,"p2pLimit");
	    strcpy(p2pLimitProfile->ipGrpName, "");
	    p2pLimitProfile->day = 127;
	    strcpy(p2pLimitProfile->timeStart, "All");
	    strcpy(p2pLimitProfile->timeStop, "0");
	    p2pLimitProfile->limitPolicy = EXCLUSIVE_RATE_P2P;
	    rst = TRUE;
	    break;
#endif
#if (SYS_LOG_INFO == FYES)
	case MIB_PROF_SYS_LOG_INFO:
	    sysLogInfoProfile = (SysLogInfoProfile *)defprof;
	    sysLogInfoProfile->head.active = FALSE;
	    ProfSetNameByPoint(sysLogInfoProfile, "SysLogInfo");
#ifdef CONFIG_UTT_DHCP_COLLISION
	    sysLogInfoProfile->DhcpLogEnable = LOG_ENABLE;
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
	    sysLogInfoProfile->ArpLogEnable = LOG_DISABLE;
#endif
	    sysLogInfoProfile->PppoeLogEnable = LOG_ENABLE;
	    rst = TRUE;
	    break;
#endif
#if (EASY_TASK == FYES)
	case MIB_PROF_EASY_TASK:
	    easyTaskProfile = (EasyTaskProfile *)defprof;
	    easyTaskProfile->head.active = TRUE;
	    ProfSetNameByPoint(easyTaskProfile,"easyTask");
	    easyTaskProfile->ConnStatus = DISCONNECT;
	   // easyTaskProfile->easyTaskNoticeEn = FUN_ENABLE;
	    easyTaskProfile->easyTaskNoticeEn = FUN_DISABLE;
#if (WLAN_WORK_TIME == FYES)
	    easyTaskProfile->safeToNet = FUN_DISABLE;
#endif
	    rst = TRUE;
	    break;
#endif
#if (HTTP_CACHE == FYES)
	case MIB_PROF_CACHE:
		cacheProfile = (CacheProfile *)defprof;
		cacheProfile->head.active = FALSE;
		ProfSetNameByPoint(cacheProfile,"cache");
		cacheProfile->size = 400u;
		cacheProfile->cacheEnable=FUN_ENABLE;
		cacheProfile->youkuVideoEnable=FUN_ENABLE;
		cacheProfile->tudouVideoEnable=FUN_ENABLE;
		cacheProfile->tcVideoEnable=FUN_ENABLE;
		cacheProfile->sinaVideoEnable=FUN_ENABLE;
		cacheProfile->ifengVideoEnable=FUN_ENABLE;
		cacheProfile->letvVideoEnable=FUN_ENABLE;
		cacheProfile->v56VideoEnable=FUN_ENABLE;
		cacheProfile->funshionVideoEnable=FUN_ENABLE;
		cacheProfile->baiduMp3Enable=FUN_ENABLE;
		cacheProfile->sinaPageEnable=FUN_ENABLE;
		rst = TRUE;
		break;
#endif
#if (NET_SNIPER == FYES)
	case MIB_PROF_NET_SNIPER:
		netSniperProfile = (NetSniperProfile *)defprof;
		netSniperProfile->head.active = FALSE;
		ProfSetNameByPoint(netSniperProfile,"netSniper");
		strcpy(netSniperProfile->ipGrpName,"");
		netSniperProfile->ctlTime = 300u;
		netSniperProfile->linearIntvl= 500u;
		netSniperProfile->chkIntvl= 10u;
		netSniperProfile->chkCnt= 10u;
		rst = TRUE;
		break;
#endif
#if (NET_MODE_SWITCH == FYES)
	case MIB_PROF_NET_MODE_SWITCH:
		netModeSwitchProfile = (NetModeSwitchProfile *)defprof;
		netModeSwitchProfile->head.active = FALSE;
		ProfSetNameByPoint(netModeSwitchProfile,"netMode");
		netModeSwitchProfile->bridgeEnable=FUN_ENABLE;
		rst = TRUE;
		break;
#endif
#if (BRIDGE == FYES)
	case MIB_PROF_BRIDGE:
		bridgeProfile = (BridgeProfile *)defprof;
		bridgeProfile->head.active = FALSE;
		ProfSetNameByPoint(bridgeProfile,"bridge");
		bridgeProfile->ip = htonl(0xc0a801fc);
		bridgeProfile->nm = htonl(0xffffff00);
		bridgeProfile->gw = htonl(0xc0a80101);
		bridgeProfile->dns1 = htonl(0xc0a80101);
		bridgeProfile->dns2 = htonl(0x00000000);
		tmpsn = uttSerioNo;
		bridgeProfile->interface[0].enable = TRUE;
		strcpy(bridgeProfile->interface[0].ifName,"eth0");
#ifdef CONFIG_CHINA_MAC_04714B
        bridgeProfile->interface[0].mac.addr[0] = 0x04;
		bridgeProfile->interface[0].mac.addr[1] = 0x71;
		bridgeProfile->interface[0].mac.addr[2] = 0x4B;
#else
#ifdef CONFIG_UTT_MAC_FC2FEF
		bridgeProfile->interface[0].mac.addr[0] = 0xFC;
		bridgeProfile->interface[0].mac.addr[1] = 0x2F;
		bridgeProfile->interface[0].mac.addr[2] = 0xEF;
#elif defined(CONFIG_UTT_MAC_C850E9)
		bridgeProfile->interface[0].mac.addr[0] = 0xC8;
		bridgeProfile->interface[0].mac.addr[1] = 0x50;
		bridgeProfile->interface[0].mac.addr[2] = 0xE9;
#else
        bridgeProfile->interface[0].mac.addr[0] = 0x00;
		bridgeProfile->interface[0].mac.addr[1] = 0x22;
		bridgeProfile->interface[0].mac.addr[2] = 0xaa;
#endif
#endif
		bridgeProfile->interface[0].mac.addr[3] = (0xFF & (tmpsn >> 16));
		bridgeProfile->interface[0].mac.addr[4] = (0xFF & (tmpsn >> 8));
		bridgeProfile->interface[0].mac.addr[5] = (0xFF & tmpsn);
		tmpsn = uttSerioNo + 0xa10 +1;
		bridgeProfile->interface[1].enable = TRUE;
		strcpy(bridgeProfile->interface[1].ifName,"eth1");
#ifdef CONFIG_CHINA_MAC_04714B
		bridgeProfile->interface[1].mac.addr[0] = 0x04;
		bridgeProfile->interface[1].mac.addr[1] = 0x71;
		bridgeProfile->interface[1].mac.addr[2] = 0x4B;
#else
#ifdef CONFIG_UTT_MAC_FC2FEF
		bridgeProfile->interface[1].mac.addr[0] = 0xFC;
		bridgeProfile->interface[1].mac.addr[1] = 0x2F;
		bridgeProfile->interface[1].mac.addr[2] = 0xEF;
#elif defined(CONFIG_UTT_MAC_C850E9)
		bridgeProfile->interface[1].mac.addr[0] = 0xC8;
		bridgeProfile->interface[1].mac.addr[1] = 0x50;
		bridgeProfile->interface[1].mac.addr[2] = 0xE9;
#else
        bridgeProfile->interface[1].mac.addr[0] = 0x00;
		bridgeProfile->interface[1].mac.addr[1] = 0x22;
		bridgeProfile->interface[1].mac.addr[2] = 0xaa;
#endif
#endif
		bridgeProfile->interface[1].mac.addr[3] = (0xFF & (tmpsn >> 16));
		bridgeProfile->interface[1].mac.addr[4] = (0xFF & (tmpsn >> 8));
		bridgeProfile->interface[1].mac.addr[5] = (0xFF & tmpsn);
		tmpsn = uttSerioNo + 0xa10 +2;
		bridgeProfile->interface[2].enable = FALSE;
		strcpy(bridgeProfile->interface[2].ifName,"eth2");
#ifdef CONFIG_CHINA_MAC_04714B
		bridgeProfile->interface[2].mac.addr[0] = 0x04;
		bridgeProfile->interface[2].mac.addr[1] = 0x71;
		bridgeProfile->interface[2].mac.addr[2] = 0x4B;
#else
#ifdef CONFIG_UTT_MAC_FC2FEF
		bridgeProfile->interface[2].mac.addr[0] = 0xFC;
		bridgeProfile->interface[2].mac.addr[1] = 0x2F;
		bridgeProfile->interface[2].mac.addr[2] = 0xEF;
#elif defined(CONFIG_UTT_MAC_C850E9)
		bridgeProfile->interface[2].mac.addr[0] = 0xC8;
		bridgeProfile->interface[2].mac.addr[1] = 0x50;
		bridgeProfile->interface[2].mac.addr[2] = 0xE9;
#else
        bridgeProfile->interface[2].mac.addr[0] = 0x00;
		bridgeProfile->interface[2].mac.addr[1] = 0x22;
		bridgeProfile->interface[2].mac.addr[2] = 0xaa;
#endif
#endif
		bridgeProfile->interface[2].mac.addr[3] = (0xFF & (tmpsn >> 16));
		bridgeProfile->interface[2].mac.addr[4] = (0xFF & (tmpsn >> 8));
		bridgeProfile->interface[2].mac.addr[5] = (0xFF & tmpsn);
		tmpsn = uttSerioNo + 0xa10 +3;
		bridgeProfile->interface[3].enable = FALSE;
		strcpy(bridgeProfile->interface[3].ifName,"eth3");
#ifdef CONFIG_CHINA_MAC_04714B
        bridgeProfile->interface[3].mac.addr[0] = 0x04;
		bridgeProfile->interface[3].mac.addr[1] = 0x71;
		bridgeProfile->interface[3].mac.addr[2] = 0x4B;
#elif defined(CONFIG_UTT_MAC_C850E9)
        bridgeProfile->interface[3].mac.addr[0] = 0xc8;
		bridgeProfile->interface[3].mac.addr[1] = 0x50;
		bridgeProfile->interface[3].mac.addr[2] = 0xe9;
#else
        bridgeProfile->interface[2].mac.addr[0] = 0x00;
		bridgeProfile->interface[2].mac.addr[1] = 0x22;
		bridgeProfile->interface[2].mac.addr[2] = 0xaa;
#endif
		bridgeProfile->interface[2].mac.addr[3] = (0xFF & (tmpsn >> 16));
		bridgeProfile->interface[2].mac.addr[4] = (0xFF & (tmpsn >> 8));
		bridgeProfile->interface[2].mac.addr[5] = (0xFF & tmpsn);
		tmpsn = uttSerioNo + 0xa10 +3;
		bridgeProfile->interface[3].enable = FALSE;
		strcpy(bridgeProfile->interface[3].ifName,"eth3");
#ifdef CONFIG_CHINA_MAC_04714B
        bridgeProfile->interface[3].mac.addr[0] = 0x04;
		bridgeProfile->interface[3].mac.addr[1] = 0x71;
		bridgeProfile->interface[3].mac.addr[2] = 0x4B;
#else
#ifdef CONFIG_UTT_MAC_FC2FEF
		bridgeProfile->interface[3].mac.addr[0] = 0xFC;
		bridgeProfile->interface[3].mac.addr[1] = 0x2F;
		bridgeProfile->interface[3].mac.addr[2] = 0xEF;
#elif defined(CONFIG_UTT_MAC_C850E9)
		bridgeProfile->interface[3].mac.addr[0] = 0xC8;
		bridgeProfile->interface[3].mac.addr[1] = 0x50;
		bridgeProfile->interface[3].mac.addr[2] = 0xE9;
#else
        bridgeProfile->interface[3].mac.addr[0] = 0x00;
		bridgeProfile->interface[3].mac.addr[1] = 0x22;
		bridgeProfile->interface[3].mac.addr[2] = 0xaa;
#endif
#endif
		bridgeProfile->interface[3].mac.addr[3] = (0xFF & (tmpsn >> 16));
		bridgeProfile->interface[3].mac.addr[4] = (0xFF & (tmpsn >> 8));
		bridgeProfile->interface[3].mac.addr[5] = (0xFF & tmpsn);
		rst = TRUE;
		break;
#endif
#if (CACHE_SERVER == FYES)
	case MIB_PROF_CACHE_SERVER:
		cacheServerProfile = (CacheServerProfile *)defprof;
		cacheServerProfile->head.active = FALSE;
		cacheServerProfile->cacheServerEnable=FALSE;
		strcpy(cacheServerProfile->cacheServerIp,"192.168.1.252");
		ProfSetNameByPoint(cacheServerProfile,"cacheServer");
		rst = TRUE;
	break;
#endif
#if (MAC_FILTER == FYES)
	case MIB_PROF_MAC_FILTER_GLOBAL:
	    macFilterGlobalProfile = (MacFilterGlobalProfile *)defprof;
	    macFilterGlobalProfile->head.active = FALSE;
	    ProfSetNameByPoint(macFilterGlobalProfile,"macFilter");
	    macFilterGlobalProfile->action = MAC_ALLOW;
	    rst = TRUE;
	    break;
	case MIB_PROF_MAC_FILTER:
	    macFilterProfile = (MacFilterProfile *)defprof;
	    macFilterProfile->head.active = FALSE;
	    rst = TRUE;
	    break;
#endif
#if (TAG_VLAN == FYES)
#if (WAN_TAG_VLAN == FYES)
        case MIB_PROF_WAN_TAG_VLAN:
	    if (instindex == 0) {
		wanTagVlanProf = (WanTagVlanProfile *)defprof;
		ProfSetNameByPoint(wanTagVlanProf, "wanTagVlan");
		wanTagVlanProf->head.active = FALSE;
		wanTagVlanProf->vid = DEF_WAN_VID;
		wanTagVlanProf->tag = 0;
		rst = TRUE;
	    }
	    break;
#endif
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
	case MIB_PROF_SHORTCUT_MENU:
	    shortCutMenuProfile = (ShortCutMenuProfile *)defprof;
	    shortCutMenuProfile->head.active = TRUE;
	    ProfSetNameByPoint(shortCutMenuProfile,"shortCutMenu");
	    
	    strncpy(shortCutMenuProfile->appName[0],"internetSet",50);
	    strncpy(shortCutMenuProfile->appName[1],"wlanSet",50);
#if (UTT_KNOWIFI == FYES)
	    strncpy(shortCutMenuProfile->appName[2],"learnConfig",50);
#elif (NET_SHARE_MANAGE == FYES)
	    strncpy(shortCutMenuProfile->appName[2],"netshare",50);
#else
	    strncpy(shortCutMenuProfile->appName[2],"wlanTiming",50);
#endif

#if (SIMPLE_MENU == FYES)
	    strncpy(shortCutMenuProfile->appName[3],"wlanTiming",50);
		strncpy(shortCutMenuProfile->appName[4],"link",50);
#else
#if (ADFILTER== FYES)
		strncpy(shortCutMenuProfile->appName[3],"ADfilter",50);
#if (UTT_KNOWIFI == FYES)
		strncpy(shortCutMenuProfile->appName[4],"link",50);
#elif (GAME_BOOST == FYES)
		strncpy(shortCutMenuProfile->appName[4],"SmartQos",50);
#endif
#elif (UTT_KNOWIFI == FYES)
		strncpy(shortCutMenuProfile->appName[3],"link",50);
#elif (GAME_BOOST == FYES)
		strncpy(shortCutMenuProfile->appName[3],"SmartQos",50);
#endif
#endif
#if (UTT_NV_A309Wv2 == FYES)
		strncpy(shortCutMenuProfile->appName[3],"link",50);
	    strncpy(shortCutMenuProfile->appName[4],"learnConfig",50);
#endif
#if (APP_ELINKC == FYES)
        strncpy(shortCutMenuProfile -> appName[3],"elink",50);
#endif
        rst = TRUE;
	    break;
#endif
#endif
#if (NET_SHARE_MANAGE == FYES)
    case MIB_PROF_NET_SHARE_MANAGE:
        netShareManageProfile = (NetShareManageProfile *)defprof;
		netShareManageProfile->head.active = TRUE;
		ProfSetNameByPoint(netShareManageProfile, "NetShareManage");
		netShareManageProfile->GlbEn = FUN_DISABLE ;
		netShareManageProfile->need_passwd = FUN_DISABLE;
        {
            int i;
            for(i=0;i<MAX_STORAGE_NUM;i++){
                netShareManageProfile->smbEnable[i] = 1;
            }
        }
#if (UTT_SMART_UI == FYES)
		netShareManageProfile->sambaEn = FUN_ENABLE;
		netShareManageProfile->ftpEn = FUN_ENABLE;
#endif
		rst = TRUE;
        break;
#endif

#if (FTP_SERVER == FYES)
    case MIB_PROF_FTP_SERVER:
        ftpServerProfile = (FtpServerProfile *)defprof;
		ProfSetNameByPoint(ftpServerProfile, "FtpServer");
		ftpServerProfile->head.active = TRUE;
		ftpServerProfile->ftpEnable = FUN_DISABLE ;
		ftpServerProfile->allow_wan_visit = FUN_DISABLE ;
		ftpServerProfile->ftp_port = 21u;
#if (UTT_SMART_UI == FYES)
		ftpServerProfile->need_passwd = FUN_DISABLE;
#endif
		rst = TRUE;
        break;

    case MIB_PROF_FTP_SHARE_DIR:
        ftpShareDirProfile = (FtpShareDirProfile *)defprof;
		rst = TRUE;
        break;
#endif

#if (NET_SHARE_USER == FYES)
    case MIB_PROF_NET_SHARE_USER:
        netShareUserProfile = (NetShareUserProfile *)defprof;
        if (instindex == 0) {
		    ProfSetNameByPoint(netShareUserProfile, "admin");
            netShareUserProfile->head.active = TRUE;
            memcpy(netShareUserProfile->password,"admin",5);
            netShareUserProfile->jurisdiction = SHARE_READ_WRITE;
            netShareUserProfile->allow_ftp = FUN_ENABLE;
        }
        if (instindex == 1) {
		    ProfSetNameByPoint(netShareUserProfile, "guest");
            netShareUserProfile->head.active = TRUE;
            memcpy(netShareUserProfile->password,"guest",5);
            netShareUserProfile->jurisdiction = SHARE_READ_ONLY;
            netShareUserProfile->allow_ftp = FUN_DISABLE;
        }
#if (UTT_SMART_UI == FYES)
        if (instindex == 2) {
		    ProfSetNameByPoint(netShareUserProfile, "anonymous");
            netShareUserProfile->head.active = TRUE;
            memcpy(netShareUserProfile->password,"anonymous",9);
            netShareUserProfile->jurisdiction = SHARE_READ_WRITE;
            netShareUserProfile->allow_ftp = FUN_ENABLE;
        }
#endif
		rst = TRUE;
        break;
#endif
#if (ADFILTER == FYES)
    case MIB_PROF_ADFILTER:
        adfilterProfile = (AdfilterProfile *)defprof;
        if (instindex == 0) {
	    ProfSetNameByPoint(adfilterProfile, "adfilter");
            adfilterProfile->head.active = FALSE;
#if (APP_POLICY_REMOTE_UPDATE == FYES)
	    adfilterProfile->policyVersion = 1;
	    adfilterProfile->checkCycle = 1;
	    adfilterProfile->oldVersion = 1;
	    adfilterProfile->newVersion = 1;
#endif
	    adfilter_rule_set(adfilterProfile, MAX_ADFILTER_ITEMS, adfilter_tuple);
        }
	rst = TRUE;
	break;
#endif
#if (MAKE_MONEY == FYES)
    case MIB_PROF_MAKE_MONEY:
        makeMoneyProfile = (MakeMoneyProfile *)defprof;
        if (instindex == 0) {
            ProfSetNameByPoint(makeMoneyProfile, "makemoney");
            makeMoneyProfile->head.active = FALSE;
            makeMoneyProfile->SaftyEn = FUN_ENABLE;
            makeMoneyProfile->wifitime_active = TRUE;
	    makeMoneyProfile->FreeTime=5;
            strcpy(makeMoneyProfile->charge,"0.1");
            strcpy(makeMoneyProfile->TimeStart,"00:00");
            strcpy(makeMoneyProfile->TimeStop,"23:59");
            sprintf(makeMoneyProfile->ssid,"0.1Ôª30·ÖÖÓ_%X",getSystemSn());
            sprintf(makeMoneyProfile->ssid2,"0.1Ôª30·ÖÖÓ_%X",getSystemSn());
            sprintf(makeMoneyProfile->ssid_5g,"0.1Ôª30·ÖÖÓ5G_%X",getSystemSn());
            sprintf(makeMoneyProfile->ssid2_5g,"0.1Ôª30·ÖÖÓ5G_%X",getSystemSn());
            strcpy(makeMoneyProfile->encodeType,"UTF-8");
            strcpy(makeMoneyProfile->encodeType2,"GBK");
            strcpy(makeMoneyProfile->encodeType_5g,"UTF-8");
            strcpy(makeMoneyProfile->encodeType2_5g,"GBK");

        }
        rst = TRUE;
        break;
#endif

#if (CLOUD_MANAGE == FYES)
    case MIB_PROF_CLOUD_MANAGE:
         cloudManageProfile = (CloudManageProfile*)defprof;
	if(instindex == 0)
	{
	    ProfSetNameByPoint(cloudManageProfile, "cloudmanage");
	    cloudManageProfile->head.active = FALSE;
	    cloudManageProfile->configNeedSyn = 1;
	    cloudManageProfile->configVer = 0;
	}	
        rst = TRUE;
        break;
#endif
#if (CLOUD_CONFIG == FYES)
    case MIB_PROF_CLOUD_CONFIG:
         cloudConfigProfile = (CloudConfigProfile*)defprof;
	if(instindex == 0)
	{
	    ProfSetNameByPoint(cloudConfigProfile, "cloudconfig");
	    cloudConfigProfile->head.active = FALSE;
	    cloudConfigProfile->noticeBanMask = 0;//NOTICE_BAND_ALL;
	    memset(cloudConfigProfile->friendMac,0,sizeof(cloudConfigProfile->friendMac));
	    memset(cloudConfigProfile->friendName,0,sizeof(cloudConfigProfile->friendName));
    
	}	
        rst = TRUE;
        break;
#endif
#if (UTT_KNOWIFI == FYES)
    case MIB_PROF_KNOWIFI_DOMAIN:
	knowifiDomainProfile = (KnowifiDomainProfile*)defprof;
	//ProfSetNameByPoint(knowifiDomainProfile, "knowifiDomain");
	knowifiDomainProfile->head.active = FALSE;
	strcpy(knowifiDomainProfile->deviceName, "unknown");
	strcpy(knowifiDomainProfile->username, "unknown");
	strcpy(knowifiDomainProfile->ip , "0.0.0.0");
	strcpy(knowifiDomainProfile->mac , "00:00:00:00:00:00");
	knowifiDomainProfile->status = 0;
	knowifiDomainProfile->conntype = 0;
	strcpy(knowifiDomainProfile->domain, "none");
	rst = TRUE;
	break;
#endif
	default:
		break;
    }
    *_prof = defprof;    
    return rst;
}
