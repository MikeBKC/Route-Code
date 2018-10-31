/*
 ***************************************************************************
 * 
*/

#include "smnt_airkiss.h"
#ifdef AIRKISS_CRYPT_UTT
#include "crypt_aes.h"
#endif
#define AIRKISS_LIB_DEBUG 0 /*需使用带log的airkiss库，如libairkiss_log.a*/

/* 超时自动退出airkiss，主要为防止误按键
 * 这里不采用定时器，而是用包触发计时
 * 是因为 1.时效要求没那么严 2.代码简单高效*/
#define UTT_AIRKISS_TIMEOUT_EN 1

#if UTT_AIRKISS_TIMEOUT_EN
#include <linux/jiffies.h>
#define UTT_AIRKISS_TIMEOUT 300*HZ /*超时时间5分钟*/
static unsigned long uttAkOutTime = 0u;
#endif

#ifdef CONFIG_UTT_AIRKISS_NETLINK
#include "linux/netfilter_ipv4/utt_filter_kernel.h" 
#endif
//MONITOR_CH_LIST g_ak_monitor_list[14] = {{6},{1},{11},{3},{9},{7},{8},{2},{4},{5},{10},{12},{13},{14}};
//INT g_ak_monitor_ch_index = 0;
INT g_ak_cur_ch = 1;
//BOOLEAN g_ak_ch_lock_done = FALSE;
airkiss_status_t g_ak_status = AIRKISS_STATUS_CONTINUE;
char airkiss_crypt_key[17] = "1234567890123456";
static airkiss_context_t g_ak_contex;
//NDIS_802_11_AUTHENTICATION_MODE g_ak_auth_mode;
//NDIS_802_11_WEP_STATUS  g_ak_wep_status;    
NDIS_SPIN_LOCK g_ak_sp_lock;
static const airkiss_config_t g_ak_conf =
{
    (airkiss_memset_fn)&memset,
    (airkiss_memcpy_fn)&memcpy,
    (airkiss_memcmp_fn)&memcmp,
#if AIRKISS_LIB_DEBUG
    (airkiss_printf_fn)&printk,
#else
    (airkiss_printf_fn)NULL,
#endif
};


INT ak_change_channel(void)
{
    return airkiss_change_channel(&g_ak_contex);/*清缓存,增加锁定信道成功的几率*/
}

/****************************************************************************/
INT ak_init(RTMP_ADAPTER *pAd, char *crypt_key)
{
    INT ret;
    //static BOOLEAN ak_ch_timerInit = FALSE;
    ret = airkiss_init(&g_ak_contex,&g_ak_conf);

    ak_debug(("\n-----Airkiss version: %s------\n", airkiss_version()));

#ifdef UTT_AIRKISS_TIMEOUT_EN
    uttAkOutTime = 0u;
#endif

    if (ret == 0) {
        ak_debug(("AirKiss init sucess !!!\n"));
        //g_ak_monitor_ch_index = 0;
        //g_ak_ch_lock_done = FALSE;
        g_ak_status = AIRKISS_STATUS_CONTINUE;
        //g_ak_auth_mode = Ndis802_11AuthModeOpen;
        //g_ak_wep_status = Ndis802_11WEPDisabled;
        //set switch channel timer
        //if (ak_ch_timerInit == false) {
        //    RTMPInitTimer(pAd, &pAd->tAK_MonitorTimer, GET_TIMER_FUNCTION(ak_chMonitorTimeout), pAd, false);
        //    ak_ch_timerInit = true;
        //}
        //switch to channel index 0 first
        //g_ak_cur_ch = g_ak_monitor_list[g_ak_monitor_ch_index].monitor_ch;
        //AsicSwitchChannel(pAd, g_ak_cur_ch, FALSE);
        //AsicSetChannel(pAd, g_ak_cur_ch, BW_20,  EXTCHA_NONE, TRUE);
        //RTMPSetTimer(&pAd->tAK_MonitorTimer, AK_CH_MONITER_TIMER);
#ifdef CONFIG_UTT_FLAG
#ifdef CONFIG_STA_SUPPORT
        pAd->Mlme.SyncMachine.CurrState = SCAN_LISTEN;
        pAd->MlmeAux.Channel = FirstChannel(pAd);
#endif
#ifdef CONFIG_AP_SUPPORT
	pAd->Mlme.ApSyncMachine.CurrState = AP_SCAN_LISTEN;
#endif
        pAd->ScanCtrl.Channel = 0; /*对于MT7628 wifi driver 这里设置的是当前信道。扫描时会自动加1.所以设置0 就是从1开始*/
#else
        pAd->ScanCtrl.Channel = FirstChannel(pAd);
#endif
        g_ak_cur_ch = pAd->ScanCtrl.Channel;
#ifdef CONFIG_UTT_FLAG
#ifdef CONFIG_AP_SUPPORT
        RTMPSetTimer(&pAd->ScanCtrl.APScanTimer, AK_CH_MONITER_TIMER);
#endif
#ifdef CONFIG_STA_SUPPORT
        RTMPSetTimer(&pAd->MlmeAux.ScanTimer, AK_CH_MONITER_TIMER);
#endif
#else
        RTMPSetTimer(&pAd->MlmeAux.ScanTimer, AK_CH_MONITER_TIMER);
#endif
#ifdef AIRKISS_ENABLE_CRYPT
        if ( crypt_key == NULL )
            airkiss_set_key(&g_ak_contex, airkiss_crypt_key, strlen(airkiss_crypt_key));
        else
            airkiss_set_key(&g_ak_contex, crypt_key, strlen(crypt_key));
#endif        
    }
    else {
        ak_debug(("AirKiss init fail !!!\n"));
    }

    return ret;
}

/*****************************************************************************/
/*VOID ak_chMonitorTimeout(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3)
{
    ak_debug(("AirKiss Test - %s !!! (locked:%d)\n", __FUNCTION__, g_ak_ch_lock_done));
    NdisAcquireSpinLock(&g_ak_sp_lock);
    if (g_ak_ch_lock_done == false) {
        PRTMP_ADAPTER pAd = (RTMP_ADAPTER *)FunctionContext;
        g_ak_monitor_ch_index++;
        if (g_ak_monitor_ch_index == 14)
            g_ak_monitor_ch_index = 0;
        
        if (g_ak_monitor_ch_index >= 0 && g_ak_monitor_ch_index < 14 ) {
            g_ak_cur_ch = g_ak_monitor_list[g_ak_monitor_ch_index].monitor_ch;
            AsicSwitchChannel(pAd, g_ak_cur_ch, FALSE);
            //AsicSetChannel(pAd, g_ak_cur_ch, BW_20,  EXTCHA_NONE, TRUE);
            RTMPSetTimer(&pAd->tAK_MonitorTimer, AK_CH_MONITER_TIMER);
        }else {
            ak_debug(("AirKiss Test - %s - channel index overflow !!! \n", __FUNCTION__));
        }
    }else {
    }
    NdisReleaseSpinLock(&g_ak_sp_lock);
}*/

/*******************************************************************************/
INT ak_get_Channel(VOID)
{
    if ( g_ak_status == AIRKISS_STATUS_CHANNEL_LOCKED || g_ak_status == AIRKISS_STATUS_COMPLETE ) {
        return g_ak_cur_ch;
    } else {
        return -1;
    }
}

/*******************************************************************************/
VOID ak_rcv_frame_handle(RTMP_ADAPTER *pAd, PHEADER_802_11 phdr, USHORT data_len)
{
    //BOOLEAN bAK_TimerCancelled = FALSE;
#ifdef AIRKISS_CRYPT_UTT
    UINT8 plainPass[UTT_MAX_WIFIPWD_LEN+1]; /*明文密码*/
    INT plainPassLen = UTT_MAX_WIFIPWD_LEN; /*分配的明文密码空间长度*/
    UINT8 *IV = airkiss_crypt_key;/*IV初始化向量，经验证微信IV跟key是一样*/
#endif

    INT ch_get = 1;
    airkiss_status_t ak_status = AIRKISS_STATUS_CONTINUE; 

#ifdef AIRKISS_CRYPT_UTT
    memset (plainPass, 0 ,sizeof(plainPass));
#endif
    //NdisAcquireSpinLock(&g_ak_sp_lock);
    //ak_debug(("AirKiss Test - receive pkt len(%d),  Sequence(%d) !!! \n", data_len, phdr->Sequence));
#if UTT_AIRKISS_TIMEOUT_EN
    if(uttAkOutTime == 0u)
    {/*只所以不在ak_init设置超时时间，是因为该收包函数可能快于ak_init执行*/
	uttAkOutTime = jiffies + UTT_AIRKISS_TIMEOUT;/*jiffies为当前时间*/
    }
    else if(time_after(uttAkOutTime,jiffies))/*未超时*/
    {
#endif
    ak_status = airkiss_recv(&g_ak_contex, phdr, data_len);
    if (ak_status == AIRKISS_STATUS_CHANNEL_LOCKED){
        g_ak_status = ak_status;
        ak_debug(("add1 : %2x:%2x:%2x:%2x:%2x:%2x\n", phdr->Addr1[0],phdr->Addr1[1],phdr->Addr1[2],phdr->Addr1[3],phdr->Addr1[4],phdr->Addr1[5]));
        ak_debug(("add2 : %2x:%2x:%2x:%2x:%2x:%2x\n", phdr->Addr2[0],phdr->Addr2[1],phdr->Addr2[2],phdr->Addr2[3],phdr->Addr2[4],phdr->Addr2[5]));
        ak_debug(("add3 : %2x:%2x:%2x:%2x:%2x:%2x\n", phdr->Addr3[0],phdr->Addr3[1],phdr->Addr3[2],phdr->Addr3[3],phdr->Addr3[4],phdr->Addr3[5]));
        g_ak_cur_ch = pAd->ScanCtrl.Channel;
        ak_debug(("AirKiss Test - %s, airkiss stop channel at (%d) !!! \n",__FUNCTION__, ak_get_Channel()));
        ch_get = ak_get_locked_ch(&pAd->ScanTab, phdr->Addr2);

        if ( g_ak_cur_ch != ch_get && ch_get > 0 ) {
            g_ak_cur_ch = ch_get;
        }
        ak_debug(("AirKiss Test - go to cur locked channel (%d) !!! \n", ak_get_Channel()));
        //g_ak_ch_lock_done = TRUE;
        //make sure to cancel timer
        //RTMPCancelTimer(&pAd->tAK_MonitorTimer, &bAK_TimerCancelled);
        //RTMPCancelTimer(&pAd->MlmeAux.ScanTimer, &bAK_TimerCancelled);
    }
    else if (ak_status == AIRKISS_STATUS_COMPLETE && g_ak_status == AIRKISS_STATUS_CHANNEL_LOCKED)
    {
        INT err;
        airkiss_result_t result;
        g_ak_status = ak_status;
        err = airkiss_get_result(&g_ak_contex, &result);
        if (err == 0) {
            ak_debug(("=====================================================================\n"));
            ak_debug(("===== AirKiss Test - %s, ssid(%s), password(%s), random(%d) !!! =====\n",__FUNCTION__, result.ssid, result.pwd, result.random));
            ak_debug(("=====================================================================\n"));

#ifdef AIRKISS_CRYPT_UTT
	    /*AES 128，CBC，PKCS7Padding*/
	    AES_CBC_Decrypt((UINT8 *)result.pwd, (UINT)result.pwd_length,airkiss_crypt_key,16,IV, 16, (UINT8 *) plainPass, (UINT *) &plainPassLen);                 
	    if((plainPassLen < UTT_MAX_WIFIPWD_LEN) && (plainPassLen >= 0))
	    {
		plainPass[plainPassLen] = '\0';/*AES_CBC_Decrypt会改变plainPass中的0结尾*/
		ak_debug(("===== AirKiss Test Decrypt ok - %s, ssid(%s), password(%s), len(%d) random(%d) !!! =====\n",__FUNCTION__, result.ssid, plainPass, plainPassLen, result.random));
		ak_debug(("=====================================================================\n"));
/*xrgadd need to send*/
#ifdef	CONFIG_UTT_AIRKISS_NETLINK
		uttNlAirkissToUser(result.ssid,plainPass,result.random);	
#endif
	    }
	    else
	    {/*解密失败*/
		ak_debug(("\n\n===== AirKiss Decrypt error  =====\n"));
#ifdef CONFIG_UTT_AP_AIRKISS_SUPPORT
		ak_debug(("\n\n======Restart Airkiss===============\n\n"));
		Set_AirkissEn_Proc(pAd, "1");/*restart airkiss*/
		return;
#endif
	    }
#else
	
/*xrgadd need to send*/
#ifdef	CONFIG_UTT_AIRKISS_NETLINK
		uttNlAirkissToUser(result.ssid,result.pwd,result.random);	
#endif
#endif

#ifdef CONFIG_UTT_AP_AIRKISS_SUPPORT
	    Set_AirkissEn_Proc(pAd, "0");/*close airkiss*/
#else
            /* change to infra mode and start connecting */
            Set_NetworkType_Proc(pAd, "Infra");
            if ( result.pwd_length > 0 ) {
                Set_AuthMode_Proc(pAd, "WPAPSKWPA2PSK");
                Set_EncrypType_Proc(pAd, "AES");
                Set_WPAPSK_Proc(pAd, result.pwd);
            }else
            {
                Set_AuthMode_Proc(pAd, "OPEN");
                Set_EncrypType_Proc(pAd, "NONE");
            }
            ak_debug(("Start connecting !!! AutoConnect(%d), auth mode(%d), encryption(%d)\n",pAd->StaCfg.bAutoReconnect, pAd->StaCfg.wdev.AuthMode, pAd->StaCfg.wdev.WepStatus));
            Set_SSID_Proc(pAd, result.ssid);
#endif
            //////////////////////////////////////
        } else {
	    ak_debug(("AirKiss Test - %s - airkiss get result fail !!! \n",__FUNCTION__));
#ifdef CONFIG_UTT_AP_AIRKISS_SUPPORT
	    ak_debug(("\n\n=========Restart Airkiss============\n\n"));
	    Set_AirkissEn_Proc(pAd, "1");/*reset airkiss*/
	    return;
#endif
        }
    }
#ifdef UTT_AIRKISS_TIMEOUT_EN
    }
    else/*airkiss 超时退出*/
    {
	ak_debug(("\n\n------------AirKiss Test TimeOut- %s - stop Airkiss!!!--------- \n",__FUNCTION__));
#ifdef CONFIG_UTT_AP_AIRKISS_SUPPORT
	Set_AirkissEn_Proc(pAd, "0");/*close airkiss*/
#endif
#ifdef CONFIG_UTT_AIRKISS_NETLINK
       uttNlAirkissToUser(NULL,NULL,0);//airkiss 超时后发送给用户层 
#endif
    }
#endif

    //NdisReleaseSpinLock(&g_ak_sp_lock);
}

INT ak_get_locked_ch(BSS_TABLE *Tab, UCHAR *pBssid)
{
    INT i;
    for (i = 0; i < Tab->BssNr; i++) {
        if (MAC_ADDR_EQUAL(Tab->BssEntry[i].Bssid, pBssid)) { 
            //g_ak_auth_mode = Tab->BssEntry[i].AuthMode;
            //g_ak_wep_status =Tab->BssEntry[i].WepStatus;
            return Tab->BssEntry[i].Channel;
        }
    }
    return -1;
}

airkiss_status_t ak_get_status()
{
    return g_ak_status;
}

/*******************************************************************************/
VOID ak_test(RTMP_ADAPTER *pAd)
{
}

