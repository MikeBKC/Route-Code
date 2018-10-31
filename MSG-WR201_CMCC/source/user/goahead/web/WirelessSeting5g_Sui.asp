<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK">
<%langTran("WirelessSui");%>
<%langTran("common");%>
<%langTran("EasyTask");%>
<%langTran("WlanSecurity");%>
<%langTran("WlanBase");%>
<%langTran("JS");%>
<title><%write(com_notitle);%></title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<link type="text/css" rel="stylesheet" href="css/jquery-ui-1.8.12.custom.css"/>
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script>  
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script> 
<script type="text/javascript" src="script/frame.js"></script>  

</head>
<style type="text/css">
#wireless_set .tab_info{ height:35px;}

</style>
<script type="text/javascript">
<% aspoutDynConfig(); %>
var wlanCurMac="<% getWlanCurrentMac_5g(); %>"
<%
    outputWlanBaseList_5g();
%>


<% aspOutWlanSecurityConfig_5g(); %>

var countryopt=new Array('America','Angola','Britain','Canada','Croatia','France','Germany','India','Italy','Malaysia','Mexico','Peru','Russia','Singapore','Spain','Taiwan','United Arab Emirates','other');
var channelopt=new Array(36,40,44,48,149, 153, 157, 161, 165);//�ŵ�ѡ��

/* ��ֹ�ظ����� */
var saveflag = 0;

/*AP�İ�ȫģʽ*/
var  secModes="";
var  secModes_2="";

//else secmodeopt[2]=new Array("WPAPSK","WPA-PSK/WPA2-PSK");
//get the value belong to the auth mode
var wepAuthModes="OPEN";
var wpaAuthModes="WPA1WPA2";
var wpapskAuthModes="WPAPSKWPA2PSK";
if(AuthModes=="OPEN"||AuthModes=="SHARED"||AuthModes=="WEPAUTO"){
	wepAuthModes=AuthModes;
	if(wpapskCiphers=="NONE") secModes="NONE";
	else secModes="WEP";
}
else if(AuthModes=="WEP")
{
	if(wpapskCiphers=="NONE") secModes="NONE";
	else secModes="WEP";
}
else if(AuthModes=="WPA"||AuthModes=="WPA2"||AuthModes=="WPA1WPA2"){
	secModes="WPA";
	wpaAuthModes=AuthModes;
}
 
else if(AuthModes=="WPAPSK"||AuthModes=="WPA2PSK"||AuthModes=="WPAPSKWPA2PSK"){
    secModes="WPAPSK";
    wpapskAuthModes=AuthModes;
}
else if(AuthModes=="WAPI")
{
     secModes="WAPI";
}
/*ssid2 ����*/
var wepAuthModes_2="OPEN";
var wpaAuthModes_2="WPA1WPA2";
var wpapskAuthModes_2="WPAPSKWPA2PSK";
if (typeof(AuthModes_2) != 'undefined') {
    if(AuthModes_2=="OPEN"||AuthModes_2=="SHARED"||AuthModes_2=="WEPAUTO"){
	    wepAuthModes_2=AuthModes_2;
	    if(wpapskCiphers_2=="NONE") secModes_2="NONE";
	    else secModes_2="WEP";
    }
    else if(AuthModes_2=="WEP")
    {
	    if(wpapskCiphers_2=="NONE") secModes_2="NONE";
	    else secModes_2="WEP";
    }
    else if(AuthModes_2=="WPA"||AuthModes_2=="WPA2"||AuthModes_2=="WPA1WPA2"){
	    secModes_2="WPA";
	    wpaAuthModes_2=AuthModes_2;
    }
    else if(AuthModes_2=="WPAPSK"||AuthModes_2=="WPA2PSK"||AuthModes_2=="WPAPSKWPA2PSK"){
	    secModes_2="WPAPSK";
	    wpapskAuthModes_2=AuthModes_2;
    }
}

/*WDS�İ�ȫģʽ*/
var wdsSecondeMode = "";

var wdsWepAuthModes="OPEN";
var wdsWpapskAuthModes="WPAPSKWPA2PSK";
if(WdsSecModes=="OPEN"||WdsSecModes=="SHARED"||WdsSecModes=="WEPAUTO"){
	wdsWepAuthModes=WdsSecModes;
	if(wdsEncType=="NONE") wdsSecondeMode="NONE";
	else wdsSecondeMode="WEP";
}
else if(WdsSecModes=="WEP")
{
	if(wdsEncType=="NONE") wdsSecondeMode="NONE";
	else wdsSecondeMode="WEP";
}
 
else if(WdsSecModes=="WPAPSK"||WdsSecModes=="WPA2PSK"||WdsSecModes=="WPAPSKWPA2PSK"){
	wdsSecondeMode="WPAPSK";
	wdsWpapskAuthModes=WdsSecModes;
}

function init(){

	
	
	/*��ʼ��AP����Ϣ*/
	var f=WlessNetSecConfig;
	f.secMode.value=secModes;
	if(f.secMode.value=="") f.secMode.value="NONE";
	f.wepAuthMode.value=(wepAuthModes==""?"WEPAUTO":wepAuthModes);
	f.keyFormat.value=(keyFormats==""?"1":keyFormats);
	f.wepkey1.value=wepkey1s;
	f.keyLength1.value=(keyLength1s==""?"0":keyLength1s);
	f.wepkey2.value=wepkey2s;
	f.keyLength2.value=(keyLength2s==""?"0":keyLength2s);
	f.wepkey3.value=wepkey3s;
	f.keyLength3.value=(keyLength3s==""?"0":keyLength3s);
	f.wepkey4.value=wepkey4s;
	f.keyLength4.value=(keyLength4s==""?"0":keyLength4s);
	if(keynums == "0")keynums = 0;
	if(radiusIPs == "0.0.0.0")radiusIPs = "";
	keynums=(keynums==""?1:keynums);
	f.keynum[keynums-1].checked=true;
    var utt_wapi = 1;
    if(utt_wapi == 0)
    {
        f.secMode.options.remove(2);
  } 
	f.wpaAuthMode.value=(wpaAuthModes==""?"auto":wpaAuthModes);
	selTxt(f.wpaCipher,wpaCiphers);
	f.radiusIP.value=(radiusIPs==""?"":radiusIPs);
	if(radiusPorts == ""||radiusPorts == "0" )radiusPorts = "1812";
	f.radiusPort.value= radiusPorts ;
	f.radiusPsswd.value=radiusPsswds;
	f.keyUpdateCyc.value=(keyUpdateCycs==""?"0":keyUpdateCycs);
	f.wpapskAuthMode.value=(wpapskAuthModes==""?"auto":wpapskAuthModes);
	selTxt(f.wpapskCipher,wpapskCiphers);
	f.pskPsswd.value=pskPsswds;
	f.pskkeyUpdateCyc.value=(pskkeyUpdateCycs==""?"0":pskkeyUpdateCycs);
	changSecMode(1);
	setKey();

	/*ssid2*/
	if (typeof(AuthModes_2) != 'undefined') {
	    $("#ssid2_safemode").show();
	    f.secMode_2.value=secModes_2;
	    if(f.secMode_2.value=="") f.secMode_2.value="NONE";
	    f.wepAuthMode_2.value=(wepAuthModes_2==""?"WEPAUTO":wepAuthModes_2);
	    f.keyFormat_2.value=(keyFormats_2==""?"1":keyFormats_2);
	    f.wepkey1_2.value=wepkey1s_2;
	    f.keyLength1_2.value=(keyLength1s_2==""?"0":keyLength1s_2);
	    f.wepkey2_2.value=wepkey2s_2;
	    f.keyLength2_2.value=(keyLength2s_2==""?"0":keyLength2s_2);
	    f.wepkey3_2.value=wepkey3s_2;
	    f.keyLength3_2.value=(keyLength3s_2==""?"0":keyLength3s_2);
	    f.wepkey4_2.value=wepkey4s_2;
	    f.keyLength4_2.value=(keyLength4s_2==""?"0":keyLength4s_2);
	    if(keynums_2 == "0")keynums_2 = 0;
	    if(radiusIPs_2 == "0.0.0.0")radiusIPs_2 = "";
	    keynums_2=(keynums_2==""?1:keynums_2);
	    f.keynum_2[keynums_2-1].checked=true;
     
	    f.wpaAuthMode_2.value=(wpaAuthModes_2==""?"auto":wpaAuthModes_2);
	    selTxt(f.wpaCipher_2,wpaCiphers_2);
	    f.radiusIP_2.value=(radiusIPs_2==""?"":radiusIPs_2);
	    if(radiusPorts_2 == ""||radiusPorts_2 == "0" )radiusPorts_2 = "1812";
	    f.radiusPort_2.value= radiusPorts_2 ;
	    f.radiusPsswd_2.value=radiusPsswds_2;
	    f.keyUpdateCyc_2.value=(keyUpdateCycs_2==""?"0":keyUpdateCycs_2);
	    f.wpapskAuthMode_2.value=(wpapskAuthModes_2==""?"auto":wpapskAuthModes_2);
	    selTxt(f.wpapskCipher_2,wpapskCiphers_2);
	    f.pskPsswd_2.value=pskPsswds_2;
	    f.pskkeyUpdateCyc_2.value=(pskkeyUpdateCycs_2==""?"0":pskkeyUpdateCycs_2);
	    changSecMode(2);
	    setKey_2();
	    $("#ssid1_sec_mode_lable").html("SSID 1 <%write(SafeMode);%>");
	} else {
	    $("#ssid2_safemode").hide();
	    $("#certificationmode_2").hide();
	    $("#keyformat_2").hide();
	    $("#keychoose_2").hide();
	    //WPA
	    $("#WPAversion_2").hide();
	    $("#encryption_2").hide();
	    $("#RadiusServiceIP_2").hide();
	    $("#Radiusport_2").hide();
	    $("#radiuspassword_2").hide();
	    $("#keyupdatetime_2").hide();
	    //WPA-PSK
	    $("#WPAversion2_2").hide();
	    $("#encryption2_2").hide();
	    $("#pre-sharedkey_2").hide();
	    $("#keyupdatetime2_2").hide();
	    $("#ssid1_sec_mode_lable").html("<%write(SafeMode);%>");
	}
	/*end of ssid2*/

	var switch_b = $(".on-off").children("button");
	initEnabledButton(switch_b,WrlessEnables);
	$("#ssid").val(ssids);
	$("#ssid2").val(ssid2s);
	if(lang_en==1){
	    $("#ssidSeparate_tr").show();
	}else{
	    $("#ssidSeparate_tr").hide();
	}
	if(ssidSeparateCtr){
	    $("#ssidSeparate").val(ssidSeparate);
	}
	$("#encodeType2").val(encodeType2);
	$("#encodeType").val(encodeType);
	var sercritymode="WEP";//��ȫģʽ
	$("#securityMode").val(sercritymode);
	if (lang_en == 1){
	    $("#country").val(countrys);
	    $("#tr_country").show();
	}else{
	    $("#country").val("11");
	    $("#tr_country").hide();
	}
	changeChannel();
	$("#channel").val(channels);//�����ŵ�
	$("#wrlessMode").val(wrlessModes);//����ģʽ
	if ($("#wrlessMode").val() == "2") {
	    $("#chw").hide();
	    $("#VHTchw").hide();
	} else if ($("#wrlessMode").val() == "8"){
	    $("#chw").show();
	    $("#VHTchw").hide();
	} else {
	    $("#chw").show();
	    $("#VHTchw").show();
	}

	$("#vhtChanWidth").val(vhtChanWidths);
	$("#chanWidth").val(chanWidths);
	$("#txPower").val(txPowers);
	//if($("#chanWidth").val() =="0" || (($("#channel").val() !="149") && ($("#channel").val() !="36") && ($("#channel").val() !="52") && ($("#channel").val() !="100"))){
	if($("#chanWidth").val() =="0"){
	    $("#vhtChanWidth").val("0");
	    $("#vhtChanWidth").prop("disabled",true);
	}else{
	    $("#vhtChanWidth").prop("disabled",false);
	}
	$("#mac").html(wlanCurMac);
	if (broadcastEnables == 0)
	{
	    $("input[name='hidenet']").attr("checked", true);
    }
    if(Wmmenable == 1)
    {
        if (wmmenables == 1)
        {
            $("input[name='wmmEnabled']").attr("checked", true);
        }
    }
	/*��ʼ��WDS����Ϣ*/
	f.wdsEnabled.checked=(WDSEnableds=="1"?true:false);
	changeWDS(f.wdsEnabled.checked);
	$("#apc_status").html("");
	if (WDSEnableds == "1") {
	    $("#bridgessid").val(bridgeSSIDs);
	    $("#bridgebssid").val(briggeBSSIDs);
	    if (WrlessEnables == "on" ) {
		$('body').everyTime('2s','Timerc',function(){
		    $.ajax({
			url:"/goform/formApcliConnStatus?timestamp="+(new Date()).getTime(),
			type:"post",
			cache:false,
			data:{
			    profIndex:profIndex
			},
			success:function(resp){
			    eval(resp);
			    if (apclii0 == 1) {
				$("#apc_status").html("<%write(com_Connected);%>");
			    } else if (apclii0 == 0){
				$("#apc_status").html("<%write(eas_connecting);%>");
			    } else {
				$("#apc_status").html("<%write(eas_connecting);%>");
			    }
			}
			    
		    });
		},0,true);
	    }
	    var profIndex = $("#profIndex").val();
	    /*��ȫģʽ����*/
	    $("#apclisecmode").val(wdsSecondeMode);
	    if($("#apclisecmode").val()=="") $("#apclisecmode").val("NONE");
	    /*WEP��ȫ����*/
	    f.wdswepAuthMode.value=(wdsWepAuthModes==""?"WEPAUTO":wdsWepAuthModes);
	    f.wdsKeyFormat.value=(wdskeyFormats==""?"1":wdskeyFormats);
	    f.wdsWepKey1.value=wdswepkey1s;
	    f.wdsWepKey1Len.value=(wdskeyLength1s==""?"0":wdskeyLength1s);
	    f.wdsWepKey2.value=wdswepkey2s;
	    f.wdsWepKey2Len.value=(wdskeyLength2s==""?"0":wdskeyLength2s);
	    f.wdsWepKey3.value=wdswepkey3s;
	    f.wdsWepKey3Len.value=(wdskeyLength3s==""?"0":wdskeyLength3s);
	    f.wdsWepKey4.value=wdswepkey4s;
	    f.wdsWepKey4Len.value=(wdskeyLength4s==""?"0":wdskeyLength4s);
	    var wdsKeySerNos = parseInt(wdskeynums);
	    wdsKeySerNos = wdsKeySerNos == 0 ? 1 : wdsKeySerNos; 
	    f.wdsKeySerNo[wdsKeySerNos-1].checked=true;

	    /*WPA��ȫģʽ����*/
	    f.apcliwpapskAuthMode.value=(wdsWpapskAuthModes==""?"WPA2PSK":wdsWpapskAuthModes);/*WPA�汾*/
	    selTxt(f.apcliwpapskCipher,wdsEncType);//�����㷨����,TKIP or AES
	    f.wdspskPsswd.value=wdspskPsswds;//Ԥ������Կ
	}
	changeApcliseSecMode();
	setApclientKey();
	/*������Ϣ����*/
	if(errorstr!=""){
		alert(errorstr);
	}
	showSuccessTips(rightstr);
	
		/*  */
	$('#wrlessMode').change(function(){
		makewrlessModeChange();
	})
	makewrlessModeChange();
	function makewrlessModeChange(){
		var tvals = $('#wrlessMode').val();
		if(tvals == '2' || tvals == '8'){
			if($('#chanWidth').children('[value="3"]').length>0){
				$('#chanWidth').children('[value="3"]').remove();
			}
		}else{
			if($('#chanWidth').children('[value="3"]').length==0){
			//	$('#chanWidth').append('<option value="3">80M</option>');
			}
			
		}
	}
}
var key1dis = 0;
var key2dis = 0;
var key3dis = 0;
var key4dis = 0;
var keylength1 = 10;
var keylength2 = 10;
var keylength3 = 10;
var keylength4 = 10;
var wep1dis = 0;
var wep2dis = 0;
var wep3dis = 0;
var wep4dis = 0;
var weplength1 = 10;
var weplength2 = 10;
var weplength3 = 10;
var weplength4 = 10;

/*ssid2*/
var key1dis_2 = 0;
var key2dis_2 = 0;
var key3dis_2 = 0;
var key4dis_2 = 0;
var keylength1_2 = 10;
var keylength2_2 = 10;
var keylength3_2 = 10;
var keylength4_2 = 10;
var wep1dis_2 = 0;
var wep2dis_2 = 0;
var wep3dis_2 = 0;
var wep4dis_2 = 0;
var weplength1_2 = 10;
var weplength2_2 = 10;
var weplength3_2 = 10;
var weplength4_2 = 10;

function setKey()
{
	var f = WlessNetSecConfig;
	getdisable_wep(f.keyLength1,f.keyLength2,f.keyLength3,f.keyLength4);
	auto_disable_wep(f.wepkey1,f.wepkey2,f.wepkey3,f.wepkey4,f.keynum,f.keyLength1,f.keyLength2,f.keyLength3,f.keyLength4);
	auto_length_wep(f.wepkey1,f.wepkey2,f.wepkey3,f.wepkey4,f.keyLength1,f.keyLength2,f.keyLength3,f.keyLength4,f.keyFormat.selectedIndex);
}
function setKey_2()
{
	var f = WlessNetSecConfig;
	if (typeof(AuthModes_2) != 'undefined') {
	    getdisable_wep_2(f.keyLength1_2,f.keyLength2_2,f.keyLength3_2,f.keyLength4_2);
	    auto_disable_wep_2(f.wepkey1_2,f.wepkey2_2,f.wepkey3_2,f.wepkey4_2,f.keynum_2,f.keyLength1_2,f.keyLength2_2,f.keyLength3_2,f.keyLength4_2);
	    auto_length_wep_2(f.wepkey1_2,f.wepkey2_2,f.wepkey3_2,f.wepkey4_2,f.keyLength1_2,f.keyLength2_2,f.keyLength3_2,f.keyLength4_2,f.keyFormat_2.selectedIndex);
	}
}
//var is_Hex = true;
function setApclientKey()
{
	var f = WlessNetSecConfig;
	getdisable(f.wdsWepKey1Len,f.wdsWepKey2Len,f.wdsWepKey3Len,f.wdsWepKey4Len);
	auto_disable(f.wdsWepKey1,f.wdsWepKey2,f.wdsWepKey3,f.wdsWepKey4,f.wdsKeySerNo,f.wdsWepKey1Len,f.wdsWepKey2Len,f.wdsWepKey3Len,f.wdsWepKey4Len);
	auto_length(f.wdsWepKey1,f.wdsWepKey2,f.wdsWepKey3,f.wdsWepKey4,f.wdsWepKey1Len,f.wdsWepKey2Len,f.wdsWepKey3Len,f.wdsWepKey4Len,f.wdsKeyFormat.selectedIndex);
}

function checkData(){
	var f=WlessNetSecConfig;

	if(!checkNameAllowSpace_P(f.ssid,32,"<%write(wire_name1);%>",0)) return false;
	if (f.ssid.value.length > 22) {
	   if (confirm("<%write(wire_warn1);%>") == false) {
	       return false;
	   }
	}
	if(!checkNameAllowSpace_P(f.ssid2,32,"<%write(wire_name2);%>",1)) return false;
    if (f.ssid2.value.length > 22) {
	   if (confirm("<%write(wire_warn2);%>") == false) {
	       return false;
	   }
	}
	var selIndex=getListValue(f.secMode);

	if(selIndex=="WEP"){
		if(!checkWEP_PP(f.wepkey1,f.wepkey2,f.wepkey3,f.wepkey4,f.keyFormat.selectedIndex)) return false;
	}
	else if(selIndex=="WPA"){
		if(!checkWPA_P()) return false;
	}
	else if(selIndex=="WPAPSK" || selIndex=="WAPI"){
		if(!checkWPAPSK_P()) return false;
	}
	if(!checkPassword1(f.pskPsswd,"63",1)) return false;
	/*ssid2������֤*/
	if (typeof(AuthModes_2) != 'undefined') {
	    var selIndex_2=getListValue(f.secMode_2);
	    if(selIndex_2=="WEP"){
		    if(!checkWEP_PP_2(f.wepkey1_2,f.wepkey2_2,f.wepkey3_2,f.wepkey4_2,f.keyFormat_2.selectedIndex)) return false;
	    }
	    else if(selIndex_2=="WPA"){
		    if(!checkWPA_P_2()) return false;
	    }
	    else if(selIndex_2=="WPAPSK"){
		    if(!checkWPAPSK_P_2()) return false;
	    }
	    if(!checkPassword1(f.pskPsswd_2,"63",1)) return false;
	}
	$("#error_msg").html("");
	var switch_b = $(".on-off").children("button");
	f.ebabledwifi.value = switch_b.attr("class");
	
	/*wds�����֤*/
	if(f.wdsEnabled.checked){
		if(!checkNameAllowSpace_P(f.bridgessid,32,"<%write(eas_wireName);%>",0)) return false;	
		if(!checkMac_P(f.bridgebssid,"<%write(wire_mac);%>",0)) return false;
		var selIndex=getListValue(f.apclisecmode);

		if(selIndex=="NONE"){
		    f.apcli_mode.value="OPEN";
		    f.apcli_enc.value="NONE";
		    f.wdsWepKey1.value="";
		    f.wdsWepKey2.value="";
		    f.wdsWepKey3.value="";
		    f.wdsWepKey4.value="";
		}
		else if(selIndex=="WEP"){
		    var wdsWepAuthMode=getListValue(f.wdswepAuthMode);
		    if(!checkWEP_P(f.wdsWepKey1,f.wdsWepKey2,f.wdsWepKey3,f.wdsWepKey4,f.wdsKeyFormat.selectedIndex)) return false;
		    f.apcli_mode.value=wdsWepAuthMode;
		    f.apcli_enc.value="WEP";
		}
		else if(selIndex=="WPAPSK" || selIndex=='WAPI'){
		    console.log(selIndex);
            var wdsWpapskAuthMode=getListValue(f.apcliwpapskAuthMode);
		    var wdsWpapskCipher=getListValue(f.apcliwpapskCipher);
		    if(!checkWDSWPAPSK_P()) return false;
		    f.apcli_mode.value=wdsWpapskAuthMode;
		    f.apcli_enc.value=wdsWpapskCipher;
		}
	}
	
	return true;	
}
function save(){
    if (saveflag) return false;
    var flag = 0;
    if(checkData()){
	var f=WlessNetSecConfig;
	var secModeVal=getListValue(f.secMode);
	if(secModeVal=="NONE"){
	    f.security_mode.value="Disable";
	}
	else if(secModeVal=="WEP"){
	    var wepAuthMode=getListValue(f.wepAuthMode);
	    f.security_mode.value=wepAuthMode;
	}
	else if(secModeVal=="WPA"){
	    flag = 1;
	    var wpaAuthMode=getListValue(f.wpaAuthMode);
	    if(wpaAuthMode=="WPA1WPA2") f.security_mode.value="WPA1WPA2";
	    else if(wpaAuthMode=="WPA")f.security_mode.value="WPA";
	    else f.security_mode.value="WPA2";
	    f.cipher.value=f.wpaCipher.value;
	    f.keyRenewalInterval.value=f.keyUpdateCyc.value;
	}
	else if(secModeVal=="WPAPSK"){
        flag = 1;
	    var wpapskAuthMode=getListValue(f.wpapskAuthMode);
	    f.security_mode.value=wpapskAuthMode;
	    f.cipher.value=f.wpapskCipher.value;
	    f.keyRenewalInterval.value=f.pskkeyUpdateCyc.value;
	}
    else if(secModeVal=="WAPI"){
	    flag = 1;
        f.security_mode.value="WAPI";
    }

	/*ssid2*/
	if (typeof(AuthModes_2) != 'undefined') {
	    var secModeVal_2=getListValue(f.secMode_2);
	    if(secModeVal_2=="NONE"){
		f.security_mode_2.value="Disable";
	    }
	    else if(secModeVal_2=="WEP"){
		var wepAuthMode_2=getListValue(f.wepAuthMode_2);
		f.security_mode_2.value=wepAuthMode_2;
	    }
	    else if(secModeVal_2=="WPA"){
		var wpaAuthMode_2=getListValue(f.wpaAuthMode_2);
		if(wpaAuthMode_2=="WPA1WPA2") f.security_mode_2.value="WPA1WPA2";
		else if(wpaAuthMode_2=="WPA")f.security_mode_2.value="WPA";
		else f.security_mode_2.value="WPA2";
		f.cipher_2.value=f.wpaCipher_2.value;
		if (flag == 0) {
		    f.keyRenewalInterval.value=f.keyUpdateCyc_2.value;
		}
	    }
	    else if(secModeVal_2=="WPAPSK"){
		var wpapskAuthMode_2=getListValue(f.wpapskAuthMode_2);
		f.security_mode_2.value=wpapskAuthMode_2;
		f.cipher_2.value=f.wpapskCipher_2.value;
		if (flag == 0) {
		    f.keyRenewalInterval.value=f.pskkeyUpdateCyc_2.value;
		}
	    }
	}

	saveflag = 1;
	WlessNetSecConfig.action="/goform/ConfigWirelessBase_Sui";
	WlessNetSecConfig.submit();
    }	
}
function checkWPA_P(){
	var f = WlessNetSecConfig;
	if(!checkIP_P(f.radiusIP,"<%write(RadiusSerIP);%>",0)) return false;
	if(!checkNumber_P(f.radiusPort,1,65535,"<%write(RadiusSerPort);%>",0)) return false;
	if(!checkPassword1_P(f.radiusPsswd,"31",0)) return false;
	if(f.keyUpdateCyc.value!="0"){
		if(!checkNumber_P(f.keyUpdateCyc,60,86400,"<%write(PWUpdateC);%>",0)) return false;
	}
	return true;
}
function checkWPA_P_2(){
    if (typeof(AuthModes_2) != 'undefined') {
	var f = WlessNetSecConfig;
	if(!checkIP_P(f.radiusIP_2,"<%write(RadiusSerIP);%>",0)) return false;
	if(!checkNumber_P(f.radiusPort_2,1,65535,"<%write(RadiusSerPort);%>",0)) return false;
	if(!checkPassword1_P(f.radiusPsswd_2,"31",0)) return false;
	if(f.keyUpdateCyc_2.value!="0"){
		if(!checkNumber_P(f.keyUpdateCyc_2,60,86400,"<%write(PWUpdateC);%>",0)) return false;
	}
    }
    return true;
}
function checkWDSWPAPSK_P(){
	var f = WlessNetSecConfig;
	var len=f.wdspskPsswd.value.length;
	if(len<8){$("#error_msg").html("<%write(PSKPWLen);%>");return false;}
	if(!checkPassword1_P(f.wdspskPsswd,"63",0)) return false;
	$("#error_msg").html("");
	return true;
}
function checkWPAPSK_P(){
	var f = WlessNetSecConfig;
	var len=f.pskPsswd.value.length;
	if(len<8){$("#error_msg").html("<%write(PSKPWLen);%>");return false;}
	if(!checkPassword1_P(f.pskPsswd,"63",0)) return false;
	if(f.pskkeyUpdateCyc.value!="0"){
		if(!checkNumber_P(f.pskkeyUpdateCyc,60,86400,"<%write(PWUpdateC);%>",0)) return false;
	}
	$("#error_msg").html("");
	return true;
}
function checkWPAPSK_P_2(){
    if (typeof(AuthModes_2) != 'undefined') {
	var f = WlessNetSecConfig;
	var len=f.pskPsswd_2.value.length;
	if(len<8){$("#error_msg").html("<%write(PSKPWLen);%>");return false;}
	if(!checkPassword1_P(f.pskPsswd_2,"63",0)) return false;
	if(f.pskkeyUpdateCyc_2.value!="0"){
		if(!checkNumber_P(f.pskkeyUpdateCyc_2,60,86400,"<%write(PWUpdateC);%>",0)) return false;
	}
	$("#error_msg").html("");
    }
    return true;
}
$(function(){

	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		swicth_act(status_now);
	});
		 $("li").corner("10px top");
	 changSecMode(1);	
	if (typeof(AuthModes_2) != 'undefined') {
	    changSecMode(2);	
	}
	 init();
	   
})

function changSecMode(sec){
    var frm=WlessNetSecConfig;
    var index;
    if (sec == 1) {
	index=getListValue(frm.secMode);
    } else {
	index=getListValue(frm.secMode_2);
    }

	switch(index){
		case "NONE":	
		    if (sec == 1) {
					//WEP
					$("#certificationmode").hide();
					$("#keyformat").hide();
					$("#keychoose").hide();
					$("#keychoose").hide();
					//WPA
					$("#WPAversion").hide();
					$("#encryption").hide();
					$("#RadiusServiceIP").hide();
					$("#Radiusport").hide();
					$("#radiuspassword").hide();
					$("#keyupdatetime").hide();
					//WPA-PSK
					$("#WPAversion2").hide();
					$("#encryption2").hide();
					$("#pre-sharedkey").hide();
					$("#keyupdatetime2").hide();
					 $("#bt_display").hide(); 
                     $('.noWAPI').show();
		    } else {
					//WEP
					$("#certificationmode_2").hide();
					$("#keyformat_2").hide();
					$("#keychoose_2").hide();
					//WPA
					$("#WPAversion_2").hide();
					$("#encryption_2").hide();
					$("#RadiusServiceIP_2").hide();
					$("#Radiusport_2").hide();
					$("#radiuspassword_2").hide();
					$("#keyupdatetime_2").hide();
					//WPA-PSK
					$("#WPAversion2_2").hide();
					$("#encryption2_2").hide();
					$("#pre-sharedkey_2").hide();
					$("#keyupdatetime2_2").hide();
					$("#bt_display_2").hide(); 
                    $('.noWAPI').show();
		    }
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		case "WEP":		//WEP
		    if (sec == 1) {
					$("#certificationmode").show();
					$("#keyformat").show();
					$("#keychoose").show();
					$("#keychoose").show();
					//WPA
					$("#WPAversion").hide();
					$("#encryption").hide();
					$("#RadiusServiceIP").hide();
					$("#Radiusport").hide();
					$("#radiuspassword").hide();
					$("#keyupdatetime").hide();
					//WPA-PSK
					$("#WPAversion2").hide();
					$("#encryption2").hide();
					$("#pre-sharedkey").hide();
					$("#keyupdatetime2").hide();
					$("#bt_display").hide(); 
                    $('.noWAPI').show();
		    } else {
					$("#certificationmode_2").show();
					$("#keyformat_2").show();
					$("#keychoose_2").show();
					//WPA
					$("#WPAversion_2").hide();
					$("#encryption_2").hide();
					$("#RadiusServiceIP_2").hide();
					$("#Radiusport_2").hide();
					$("#radiuspassword_2").hide();
					$("#keyupdatetime_2").hide();
					//WPA-PSK
					$("#WPAversion2_2").hide();
					$("#encryption2_2").hide();
					$("#pre-sharedkey_2").hide();
					$("#keyupdatetime2_2").hide();
					$("#bt_display_2").hide(); 
                    $('.noWAPI').show();
		    }
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		case "WPA":			//WEP
		    if (sec == 1) {
					$("#certificationmode").hide();
					$("#keyformat").hide();
					$("#keychoose").hide();
					$("#keychoose").hide();
					//WPA
					$("#WPAversion").show();
					$("#encryption").show();
					$("#RadiusServiceIP").show();
					$("#Radiusport").show();
					$("#radiuspassword").show();
					$("#keyupdatetime").show();
					//WPA-PSK
					$("#WPAversion2").hide();
					$("#encryption2").hide();
					$("#pre-sharedkey").hide();
					$("#keyupdatetime2").hide();
					$("#bt_display").hide(); 
                    $('.noWAPI').show();
		    } else {
					$("#certificationmode_2").hide();
					$("#keyformat_2").hide();
					$("#keychoose_2").hide();
					$("#keychoose_2").hide();
					//WPA
					$("#WPAversion_2").show();
					$("#encryption_2").show();
					$("#RadiusServiceIP_2").show();
					$("#Radiusport_2").show();
					$("#radiuspassword_2").show();
					$("#keyupdatetime_2").show();
					//WPA-PSK
					$("#WPAversion2_2").hide();
					$("#encryption2_2").hide();
					$("#pre-sharedkey_2").hide();
					$("#keyupdatetime2_2").hide();
					$("#bt_display_2").hide(); 
                    $('.noWAPI').show();
		    }
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		case "WPAPSK":		//WEP
		    if (sec == 1) {
					$("#certificationmode").hide();
					$("#keyformat").hide();
					$("#keychoose").hide();
					$("#keychoose").hide();
					//WPA
					$("#WPAversion").hide();
					$("#encryption").hide();
					$("#RadiusServiceIP").hide();
					$("#Radiusport").hide();
					$("#radiuspassword").hide();
					$("#keyupdatetime").hide();
					//WPA-PSK
					 $("#pre-sharedkey").show();
					$("#bt_display").show(); 
                    $('.noWAPI').show();
		    } else {
					$("#certificationmode_2").hide();
					$("#keyformat_2").hide();
					$("#keychoose_2").hide();
					$("#keychoose_2").hide();
					//WPA
					$("#WPAversion_2").hide();
					$("#encryption_2").hide();
					$("#RadiusServiceIP_2").hide();
					$("#Radiusport_2").hide();
					$("#radiuspassword_2").hide();
					$("#keyupdatetime_2").hide();
					//WPA-PSK
					/*
					$("#WPAversion2_2").show();
					$("#encryption2_2").show();
					$("#keyupdatetime2_2").show();
					*/
					$("#pre-sharedkey_2").show();
					$("#bt_display_2").show(); 
                    $('.noWAPI').show();
                    $("#bt_display").show();
		    }
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
                    break;
		case "WAPI":		//WAPI
		    if (sec == 1) {
					$("#certificationmode").hide();
					$("#keyformat").hide();
					$("#keychoose").hide();
					$("#keychoose").hide();
					//WPA
				    $("#keyupdatetime2").hide();
                    $("#WPAversion2").hide();
                    $("#encryption2").hide();
                    $("#WPAversion").hide();
					$("#encryption").hide();
					$("#RadiusServiceIP").hide();
					$("#Radiusport").hide();
					$("#radiuspassword").hide();
					$("#keyupdatetime").hide();
					//WPA-PSK
					 $("#pre-sharedkey").show();
					$("#bt_display").hide(); 
                    $('.noWAPI').hide();
		    } else {
					$("#certificationmode_2").hide();
					$("#keyformat_2").hide();
					$("#keychoose_2").hide();
					$("#keychoose_2").hide();
					//WPA
					$("#WPAversion_2").hide();
					$("#encryption_2").hide();
					$("#WPAversion2").hide();
                    $("#encryption2").hide();
                    $("#keyupdatetime2").hide();
                    $("#RadiusServiceIP_2").hide();
					$("#Radiusport_2").hide();
					$("#radiuspassword_2").hide();
					$("#keyupdatetime_2").hide();
					//WPA-PSK
					/*
					$("#WPAversion2_2").show();
					$("#encryption2_2").show();
					$("#keyupdatetime2_2").show();
					*/
					$("#pre-sharedkey_2").show();
					$("#bt_display_2").show();
                    $("#bt_display").hide();
                    $('.noWAPI').hide();
		    }
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
                    break;
        default:	break;
	}
 
}

var wpapsk=0;
function  showWPAPSK(){
     if(wpapsk==0){
					wpapsk=1;
					$("#WPAversion2").show();
					$("#encryption2").show();
					$("#keyupdatetime2").show();

		}else
		{
					wpapsk=0;
					$("#WPAversion2").hide();
					$("#encryption2").hide();
					$("#keyupdatetime2").hide();
		}
	    parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
}
var wpapsk_2=0;
function  showWPAPSK_2(){
				if(wpapsk_2==0){
					wpapsk_2=1;
				    $("#WPAversion2_2").show();                                    
				    $("#encryption2_2").show();                            
				    $("#keyupdatetime2_2").show();
				}else{
				       wpapsk_2=0;
				     $("#WPAversion2_2").hide();
				     $("#encryption2_2").hide(); 
				     $("#keyupdatetime2_2").hide();
				}
				     parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+130);
 }
function selwrlessMode(val){
	if(val=="2"){
	    $("#chw").hide();
	    $("#VHTchw").hide();
	    $("#chanWidth").val("0");
	    $("#vhtChanWidth").val("0");
	    parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
	} else if (val == "8") {
	    $("#chw").show();
	    $("#VHTchw").hide();
	    $("#chanWidth").val("1");
	    $("#vhtChanWidth").val("0");
	    parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
	} else {
	    $("#chw").show();
	    $("#VHTchw").show();
	    $("#chanWidth").val("1");
	    $("#chanWidth").prop("disabled",false);
	    $("#vhtChanWidth").val("0");
	    /*if(($("#channel").val() !="149") && ($("#channel").val() !="36") && ($("#channel").val() !="52") && ($("#channel").val() !="100")){
		$("#vhtChanWidth").prop("disabled",true);
	    } else {
		$("#vhtChanWidth").prop("disabled",false);
	    }*/
		$("#vhtChanWidth").prop("disabled",false);
	    parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
	}	
}
function changeWDS(val){
	
	var f=WlessNetSecConfig;
		f.apclisecmode.value="NONE";
		changeApcliseSecMode();
		if(val){
			
			$("#bridgessid_td").show();
			$("#bridgebssid_td").show();
			$("#scan_td").show();
			$("#securitymode_td").show();
		
			parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
			f.wdsEnable.value="1";
		}else{
			$("#bridgessid_td").hide();
			$("#bridgebssid_td").hide();
			$("#scan_td").hide();
			$("#securitymode_td").hide();
			parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
			f.wdsEnable.value="0";
		}

}
function changeApcliseSecMode(){
	var frm=WlessNetSecConfig;
	var index=getListValue(frm.apclisecmode);

	switch(index){
		case "NONE":
					$("#certificationmode_td").hide();
					$("#keyformat_td").hide();
					$("#keychoose_td").hide();
					$("#WPAversion1_td").hide();
					$("#encryption1_td").hide();
					$("#presharedkey1_td").hide();
					
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		case "WEP":	
					$("#certificationmode_td").show();
					$("#keyformat_td").show();
					$("#keychoose_td").show();
					$("#WPAversion1_td").hide();
					$("#encryption1_td").hide();
					$("#presharedkey1_td").hide();
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		case "AES":
		case "TKIP":
					$("#keyformat_td").hide();
					$("#keychoose_td").hide();
					$("#WPAversion1_td").hide();
					$("#encryption1_td").hide();
					$("#presharedkey1_td").show();
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		case "WPAPSK":
					$("#certificationmode_td").hide();
					$("#keyformat_td").hide();
					$("#keychoose_td").hide();
					$("#WPAversion1_td").show();
					$("#encryption1_td").show();
					$("#presharedkey1_td").show();
					parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
					break;
		default:	break;
	}
 
}

    var timename;
	var remandTime;
    function timeLeft() {
	 // var dd = document.getElementById("reloadId").value;
	 	
		remandTime--;
	  	if(remandTime > 0 ) 
	  	{
			document.getElementById('reloadId').innerHTML=remandTime;
	  	 }else{
			 clearInterval(timename);
			document.getElementById('reloadId').innerHTML=0;
			$("#wirelessSeting_box").hide();
			$("#wait_box").hide();
			$("#list_box").show();
			showlist();	 
		}

	}
function start(){
	    parent.art.dialog.list["func_box"].size(700,$("#wait_box").height()+300);
	   remandTime =8;
	   document.getElementById('reloadId').innerHTML=remandTime;
       timename=setInterval('timeLeft();',1000);
}
function scanlocation(){
	
	$("#wirelessSeting_box").hide();
	$("#list_box").hide();
	//parent.art.dialog.list["func_box"].size(700,400);
	$("#wait_box").show();
	var profIndex = $("#profIndex").val();
	$.ajax({
		url:"/goform/formApScan?timestamp="+(new Date()).getTime(),
		type:"post",
		cache:false,
		data:{
		    profIndex:profIndex
		},
		success:function(data){
		}
		
	});
	start();
}

function selVhtChannel(){
   // if (($("#chanWidth").val() == "0") || (($("#channel").val() != "149") && ($("#channel").val() != "36") && ($("#channel").val() != "52") && ($("#channel").val() != "100"))) {
	if ($("#chanWidth").val() == "0"){
	$("#vhtChanWidth").val("0");
	$("#vhtChanWidth").prop("disabled",true);
    } else {
	//$("#vhtChanWidth").val("0");
	$("#vhtChanWidth").prop("disabled",false);
    }
}
function changeChannel(){
    var cou = $("#country").val();
	if(utt_5g_channel_149_to_165 == 1){
		$("#countryRegionABand").val("4");
		channelopt= new Array(149, 153, 157, 161, 165);
	}
	else{
		$("#countryRegionABand").val("0");
		channelopt=new Array(36,40,44,48,52,56,60,64,149,153,157,161,165);
	}
	if(lang_en == 1){
    if (cou == "0"){
	channelopt=new Array(36, 40, 44, 48, 149, 153, 157, 161, 165);
	$("#countryRegionABand").val("10");
    }else if ((cou == "1") || (cou == "16")){
	channelopt=new Array(36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140);
	$("#countryRegionABand").val("1");
    } else if ((cou == "2") || (cou == "4") || (cou == "5") || (cou == "6") || (cou == "8") || (cou == "14")){
	channelopt=new Array(36, 40, 44, 48);
	$("#countryRegionABand").val("6");
    } else if (cou == "3"){
	channelopt=new Array(36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165);
	$("#countryRegionABand").val("9");
    } else if ((cou == "7") || (cou == "10") || (cou == "13")){
	channelopt= new Array(36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165);
	$("#countryRegionABand").val("0");
    } else if (cou == "9"){
	channelopt= new Array(52, 56, 60, 64, 149, 153, 157, 161, 165);
	$("#countryRegionABand").val("0");//0��Ӧ�Ľϴ󼯺�
    } else if (cou == "11"){
	channelopt= new Array(149, 153, 157, 161, 165);
	$("#countryRegionABand").val("4");
    } else if (cou == "12"){
	channelopt= new Array(149, 153, 157, 161);
	$("#countryRegionABand").val("5");
    } else if (cou == "15"){
	channelopt= new Array(56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165);
	$("#countryRegionABand").val("7");
    } else if (cou == "17"){
	channelopt= new Array(36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165);
	$("#countryRegionABand").val("7");
    }
	}
    var str='<select name="channel" id="channel" onChange = "selVhtChannel()" class="select" style="width:60px;"><option value="0"><%write(wwsui_auto);%></option>';
    for(i=0;i<channelopt.length;i++){
	str +='<option value="'+channelopt[i]+'">'+channelopt[i]+'</option>';
    }
    str += '</select>';
    $("#channelstr").html(str);
    selVhtChannel();
}
var bridgebssids = new Array();
var bridgessids = new Array();
var signalStrength = new Array();
var channelsArr = new Array();
var EncryptTypes = new Array();
var Encrypts = new Array();
var choice = new Array();
function showlist(){
    var listheadname="<%write(wire_apList);%>";
    var titles = new Array();
    var profIndex = $("#profIndex").val();
    titles[0]="ID";
    titles[1]="<%write(wire_mac);%>";
    titles[2]="<%write(eas_wireName);%>";
    titles[3]="<%write(wire_sigStrong);%>";
    titles[4]="<%write(Channel);%>";
    titles[5]="<%write(wire_isAddPW);%>";
    //titles[5]="���ܷ�ʽ";
    $.ajax({
	    url:"/goform/formApScanResult",
	    async:false,
	    type:"post",
	    data:{
		profIndex:profIndex	
	    },
	    success:function(data){
		//eval((data));
		var opstr ="", value = "";
		var  last = 0, end = 0;
		while (true) {
		    end = data.indexOf(";", last);
		    if(end == -1) {
			break; 
		    }
		    opstr = data.substring(last, end+1);
		    try {
			eval(opstr);
		    } catch (e) {
			var tmp = opstr.substring(0,opstr.indexOf("=", 0));
			tmp += "='';";
			eval(tmp);
		    }
		    last = end + 1;
		}
		for (var i = 0; i < totalrecs; i++) {
		    if (EncryptTypes[i].indexOf("NONE") != -1) {
			Encrypts[i] = "<%write(eas_no);%>";
		    } else {
			Encrypts[i] = "<%write(eas_yes);%>";
		    }
		    /*
		    if (EncryptTypes[i].indexOf("WEP") != -1) {
			Encrypts[i] = "WEP";
		    } else if (EncryptTypes[i].indexOf("WPA") != -1) {
			Encrypts[i] = "WPA-PSK/WPA2-PSK";
		    } else {
			Encrypts[i] = "�ް�ȫ����";
		    }
		    */
		}
		var arrayData=new Array(bridgebssids,bridgessids,signalStrength,channelsArr,Encrypts);
		var htm_str = dspListTab(listheadname,titles,titles.length,arrayData,0,600,totalrecs);
		$("#listinfo_box").html(htm_str);
		parent.art.dialog.list["func_box"].size(700,$("#list_box").height()+150);
	    }
    });
}
function EditConfig(index){
	var f=WlessNetSecConfig;
	$("#bridgessid").val(bridgessids[index]);
	$("#bridgebssid").val(bridgebssids[index]);
	$("#channel").val(channelsArr[index]);
	if (EncryptTypes[index].indexOf("WEP") != -1) {
	    $("#apclisecmode").val("WEP");
	} else if (EncryptTypes[index].indexOf("WPA") != -1) {
	    $("#apclisecmode").val("WPAPSK");
	    /*WPA��ȫģʽ����*/
	    if ((EncryptTypes[index].indexOf("WPA1PSK") != -1) || (EncryptTypes[index].indexOf("WPAPSK") != -1)) {
		f.apcliwpapskAuthMode.value = "WPAPSK";
	    } else {
		f.apcliwpapskAuthMode.value = "WPA2PSK";
	    }
	    if (EncryptTypes[index].indexOf("AES") != -1) {
		selTxt(f.apcliwpapskCipher,"AES");//�����㷨����,TKIP or AES
	    } else {
		selTxt(f.apcliwpapskCipher,"TKIP");//�����㷨����,TKIP or AES
	    }
	} else {
	    $("#apclisecmode").val("NONE");
	}
	changeApcliseSecMode();
	$("#wirelessSeting_box").show();
	$("#list_box").hide();
	$("#wait_box").hide();
	parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
}
function goback(){
	
	$("#wirelessSeting_box").show();
	$("#list_box").hide();
	$("#wait_box").hide();
	parent.art.dialog.list["func_box"].size(700,$("#wirelessSeting_box").height()+100);
}

</script>
<style type="text/css">
ul li {  float:left; margin-left:2px; margin-top:10px; height:29px; padding-top:3px;  padding-left:10px; padding-right:10px; color:#fff; width:120px;cursor:pointer;}

#firstli{background:#0099D2;color:#fff;}
#secondli{background:#fff;color:#0099D2;}
#wirelessSeting_box,#list_box{text-align:center; margin:0 auto; width:600px;}
</style>
<body id="wireless_set" scroll="no">
<br>
<div id="tabs" style="width:650px;">
	<div style=" height:42px;background-color:#0099D2;" >
		<ul >
				<li style="margin-left:15px;" id="firstli" onclick="window.parent.open_windows(null,'wifi',null)" >2.4G<%write(wire_band);%></li>
				<li id="secondli" onclick="window.parent.open_windows(null,'wifi_5g',null)" >5G<%write(wire_band);%></li>
			</ul>
	</div>
<div class="warp" >


    <br>
	<form action="" method="post" name="WlessNetSecConfig">
	<input type="hidden" id="profIndex" name="profIndex" value="1"/>
 	<div id="wirelessSeting_box" style="display:;">
	<table border="0" cellpadding="0" cellspacing="1" width="600px" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
    	<tr class="tab_info">
        	<td width="120px"><%write(wwsui_wlanEn);%></td>
            <td colspan="2"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="ebabledwifi" id="ebabledwifi" value="" /></span> &nbsp;&nbsp;<a href="#" onclick="iswireRestart();return false;" target="_blank" id="winaso"><%write(com_wireWarn1);%></a></td>
	</tr>
	<script language="javascript">
	function  iswireRestart()
	{
	    if(confirm("<%write(com_wireWarn2);%>")){
	     $.ajax({
		  url:"/goform/restartWireless",
		  });
	    }
	}
	</script>
        </tr>
        <tr class="tab_info">
        	<td><%write(wire_name1);%></td>
            <td colspan="2">
		<input type="text"   class="txt-input" name="ssid" id="ssid" style="width:150px;" value="" />&nbsp;&nbsp;<%write(wire_code);%>&nbsp;&nbsp;
		<select name="encodeType" id="encodeType" width=8px>
                    <option value="GBK" selected="selected"><%write(wire_priComp);%>(<%write(wire_simple);%>)</option>
                    <option value="BIG5"><%write(wire_priComp);%>(<%write(wire_complex);%>)</option>
                    <option value="UTF-8"><%write(wire_priPhone);%></option>
		</select>
	    </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(wire_name2);%></td>
            <td colspan="2">
	    <input type="text"   class="txt-input" name="ssid2" id="ssid2" style="width:150px;" value="" />&nbsp;&nbsp;<%write(wire_code);%>&nbsp;&nbsp;
		<select name="encodeType2" id="encodeType2" width=8px>
                    <option value="GBK" selected="selected"><%write(wire_priComp);%>(<%write(wire_simple);%>)</option>
                    <option value="BIG5"><%write(wire_priComp);%>(<%write(wire_complex);%>)</option>
                    <option value="UTF-8"><%write(wire_priPhone);%></option>
		</select>
	    </td>
           
        </tr>
        <tr class="tab_info">
        	<td><%write(wwsui_hidenet);%></td>
            <td colspan="2">
            	<input name="hidenet" id="hidenet" type="checkbox"/>
            </td>
        </tr>
        <tr class="tab_info" id = "ssidSeparate_tr">
        	<td><%write(wwsui_ssidSeparate);%></td>
            <td colspan="2">
            	<select name="ssidSeparate" id="ssidSeparate" class="select">
		    <option value="0" ><%write(ssid_no_isolation);%></option>
                    <option value="1"><%write(ssid1_isolation);%></option>
                    <option value="2"><%write(ssid2_isolation);%></option>
                </select>
            </td>
        </tr>
        <tr class="tab_info">
        	<td id="ssid1_sec_mode_lable"><%write(SafeMode);%></td>
            <td colspan="2">
            	<select name="secMode" id="secMode" class="select" onChange="changSecMode(1);">
                	<option value="NONE" ><%write(NoSafeMode);%></option>
                    <option value="WEP">WEP</option>
				<!--	<option value="WAPI">WAPI</option> -->
                    <option value="WPAPSK">WPA-PSK/WPA2-PSK</option>
                </select>
	    &nbsp;&nbsp;<span style="cursor:pointer;" onclick=" showWPAPSK();" id="bt_display"><strong><%write(com_advance);%> </strong></span>
            </td>
        </tr><input type="hidden" name="security_mode">
        <div id="weptbl1" style="display:none;">
         <tr class="tab_info" id="certificationmode">
            <td><%write(ApproveType);%></td>
            <td colspan="2">
            	<select name="wepAuthMode" class="select" style="width:120px;">
               <option value="WEPAUTO"><%write(Auto);%></option>
               <option value="OPEN"><%write(OpenSys);%></option>
               <option value="SHARED"><%write(ShareKey);%></option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="keyformat">
            <td><%write(PWForm);%></td>
            <td colspan="2">
            	<select name="keyFormat" onChange="setKey()" style="width:120px;" class="select" >
                   <option value="0" selected><%write(Hex16);%></option>
                   <option value="1"><%write(ASCIICode);%></option>
                </select>
            </td>
        </tr>
        <tr class="tab_info" id="keychoose">
            <td><%write(PWSelect);%></td>
            <td colspan="2">
            	<table border="0" cellpadding="0" cellspacing="0"  style="background:#5e5e5e;"  >
                	<tr>
                    	<td align="left" style="padding-left:0px;"></td>
                        <td></td>
                        <td><%write(PWWEP);%>
                        </td>
                        <td>
                        	 <%write(PWType);%>  
                         </td>
                    </tr>
                	<tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert7);%></td>
                        <td><input type="radio" name="keynum" value='1' /></td>
                        <td><input type="text" class="txt-input" name="wepkey1" maxLength="32" size="30" onfocus="if(wep1dis) this.blur(); else this.form.keynum[0].checked=true;" />
                        </td>
                        <td>
                        	<select name="keyLength1" class="select" style="width:80px;" id="keyLength1" onchange="setKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>

        			       </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert9);%></td>
                        <td><input type="radio" name="keynum" value='2' /></td>
                        <td><input type="text" class="txt-input" name="wepkey2" maxLength="32" size="30" onfocus="if(wep2dis) this.blur();else this.form.keynum[1].checked=true;" />
                        </td>
                        <td>
                        	<select name="keyLength2" class="select" style="width:80px;" id="keyLength2" onchange="setKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>
        			

        			       </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert11);%></td>
                        <td><input type="radio" name="keynum" value='3' /></td>
                        <td><input type="text" class="txt-input" name="wepkey3" maxLength="32" size="30" onfocus="if(wep3dis) this.blur();else this.form.keynum[2].checked=true;" />
                        </td>
                        <td>
                        	<select name="keyLength3" class="select" style="width:80px;" id="keyLength3" onchange="setKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>

        			       </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert13);%></td>
                        <td><input type="radio" name="keynum" value='4' /></td>
                        <td><input type="text" class="txt-input" name="wepkey4" maxLength="32" size="30" onfocus="if(wep4dis) this.blur();else this.form.keynum[3].checked=true;" />
                        </td>
                        <td>
                        	<select name="keyLength4" class="select" style="width:80px;" id="keyLength4" onchange="setKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>
						    </select>   
                         </td>
                    </tr>
                    <tr><td colspan="4">&nbsp;</td></tr>
                </table>
                
            </td>
        </tr>
      
      	
        </div>
        <div id="wpatbl1" style="display:none;">
        <tr class="tab_info" id="WPAversion">
            <td><%write(WPAVer);%></td>
            <td colspan="2">
            	<select name="wpaAuthMode" class="select" style="width:70px;">
               <option value="WPA1WPA2"><%write(Auto);%></option>
               <option value="WPA">WPA</option>
               <option value="WPA2">WPA2</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="encryption">
            <td><%write(PWALG);%></td>
            <td colspan="2">
            	<select name="wpaCipher" class="select" style="width:70px;">
                   <option value="2" selected><%write(Auto);%></option>
                   <option value="AES">AES</option>
                   <option value="TKIP">TKIP</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="RadiusServiceIP">
            <td><%write(RadiusSerIP);%></td>
            <td colspan="2">
            	<input name="radiusIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  class="txt-input" />
            </td>
        </tr>
        <tr class="tab_info" id="Radiusport">
            <td><%write(RadiusPort);%></td>
            <td colspan="2">
            	<input name="radiusPort" type="text" class="txt-input" style="width:70px;"  onKeyPress="figureOnly()"   size="10" maxlength="5" />&nbsp;&nbsp;<%write(range1);%>
            </td>
        </tr>
        <tr class="tab_info" id="radiuspassword">
            <td><%write(RadiusPW);%></td>
            <td colspan="2">
            	<input name="radiusPsswd" type="password" size="18" class="txt-input" maxlength="31" />&nbsp;&nbsp;<%write(range2);%>
            </td>
        </tr>
        <tr class="tab_info" id="keyupdatetime">
            <td><%write(PWUpdateC);%></td>
            <td colspan="2">
            	<input name="keyUpdateCyc" type="text" size="10" maxlength="5" style="width:70px;" class="txt-input" onKeyPress="figureOnly()" />&nbsp;&nbsp;<%write(range3);%>
            </td>
        </tr>
        </div>
        <div id="wpapsktbl1" style="display:none;">
        <tr class="tab_info" id="WPAversion2">
            <td><%write(WPAVer);%></td>
            <td colspan="2">
            	<select name="wpapskAuthMode" class="select" style="width:100px;">
                   <option value="WPAPSKWPA2PSK"><%write(Auto);%></option>
                   <option value="WPAPSK">WPA-PSK</option>
                   <option value="WPA2PSK">WPA2-PSK</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="encryption2">
            <td><%write(PWALG);%></td>
            <td colspan="2">
            	<select name="wpapskCipher" class="select" style="width:100px;">
                   <option value="TKIPAES" selected><%write(Auto);%></option>
                   <option value="AES">AES</option>
                   <option value="TKIP">TKIP</option>
                 </select>
            </td>
        </tr>
        <tr class="tab_info" id="pre-sharedkey">
            <td><%write(WSharePW);%></td>
            <td colspan="2">
		<span id="pass">
            	<input name="pskPsswd" id="pw"  type="password" size="18" class="txt-input" />
		</span>
		 &nbsp;<%write(com_dispass);%>&nbsp;<input type="checkbox" id="showpass"
		    onclick="
		    var temp=document.getElementById('pw').value;
                   if(this.checked){	
		   document.getElementById('pass').innerHTML='<input type=text id=pw name=pskPsswd size=18  class=txt-input>';
		     }
		    else
		    {
		    document.getElementById('pass').innerHTML='<input type=password id=pw name=pskPsswd size=18  class=txt-input>';
		    } document.getElementById('pw').value=temp;
		"/>&nbsp;&nbsp;<%write(range4);%>
	    </td>
        </tr>
        
        <tr class="tab_info" id="keyupdatetime2">
            <td><%write(PWUpdateC);%></td>
            <td colspan="2">
            	<input name="pskkeyUpdateCyc" type="text" style="width:70px;" size="10" maxlength="5" class="txt-input"  onKeyPress="figureOnly()" />&nbsp;&nbsp;<%write(range3);%>
            </td>
        </tr>
        </div>
        <tr class="tab_info" id = "ssid2_safemode">
        	<td>SSID 2 <%write(SafeMode);%></td>
            <td colspan="2">
            	<select name="secMode_2" id="secMode_2" class="select" onChange="changSecMode(2);">
		    <option value="NONE" ><%write(NoSafeMode);%></option>
                    <option value="WEP">WEP</option>
		    <option value="WPAPSK">WPA-PSK/WPA2-PSK</option>
                </select>
          &nbsp;&nbsp;<span style="cursor:pointer;" onclick=" showWPAPSK_2();" id="bt_display_2"><strong><%write(com_advance);%> </strong></span>
             
            </td>
        </tr><input type="hidden" name="security_mode_2">
        <div id="weptbl_2" style="display:none;">
         <tr class="tab_info" id="certificationmode_2">
            <td><%write(ApproveType);%></td>
            <td colspan="2">
            	<select name="wepAuthMode_2" class="select" style="width:120px;">
		   <option value="WEPAUTO"><%write(Auto);%></option>
		   <option value="OPEN"><%write(OpenSys);%></option>
		   <option value="SHARED"><%write(ShareKey);%></option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="keyformat_2">
            <td><%write(PWForm);%></td>
            <td colspan="2">
            	<select name="keyFormat_2" onChange="setKey_2()" style="width:90px;" class="select" >
                   <option value="0" selected><%write(Hex16);%></option>
                   <option value="1"><%write(ASCIICode);%></option>
                </select>
            </td>
        </tr>
        <tr class="tab_info" id="keychoose_2">
            <td><%write(PWSelect);%></td>
            <td colspan="2">
            	<table border="0" cellpadding="0" cellspacing="0"  style="background:#5e5e5e;"  >
                	<tr>
                    	<td align="left" style="padding-left:0px;"></td>
                        <td></td>
                        <td><%write(PWWEP);%>
                        </td>
                        <td>
                        	<%write(PWType);%>  
                         </td>
                    </tr>
                	<tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert7);%></td>
                        <td><input type="radio" name="keynum_2" value='1' /></td>
                        <td><input type="text" class="txt-input" name="wepkey1_2" maxLength="32" size="30" onfocus="if(wep1dis_2) this.blur(); else this.form.keynum_2[0].checked=true;" />
                        </td>
                        <td>
                        	<select name="keyLength1_2" class="select" style="width:80px;" id="keyLength1_2" onchange="setKey_2();">
				    <option value="0" ><%write(com_forbidden);%></option>
				    <option value="5"><%write(Bit64);%></option>
				    <option value="13"><%write(Bit128);%></option>
			       </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert9);%></td>
                        <td><input type="radio" name="keynum_2" value='2' /></td>
                        <td><input type="text" class="txt-input" name="wepkey2_2" maxLength="32" size="30" onfocus="if(wep2dis_2) this.blur();else this.form.keynum_2[1].checked=true;" />
                        </td>
                        <td>
			    <select name="keyLength2_2" class="select" style="width:80px;" id="keyLength2_2" onchange="setKey_2();">
                         	<option value="0" ><%write(com_forbidden);%></option>
				<option value="5"><%write(Bit64);%></option>
				<option value="13"><%write(Bit128);%></option>
			   </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert11);%></td>
                        <td><input type="radio" name="keynum_2" value='3' /></td>
                        <td><input type="text" class="txt-input" name="wepkey3_2" maxLength="32" size="30" onfocus="if(wep3dis_2) this.blur();else this.form.keynum_2[2].checked=true;" />
                        </td>
                        <td>
			    <select name="keyLength3_2" class="select" style="width:80px;" id="keyLength3_2" onchange="setKey_2();">
                         	<option value="0" ><%write(com_forbidden);%></option>
				<option value="5"><%write(Bit64);%></option>
				<option value="13"><%write(Bit128);%></option>
			   </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert13);%></td>
                        <td><input type="radio" name="keynum_2" value='4' /></td>
                        <td><input type="text" class="txt-input" name="wepkey4_2" maxLength="32" size="30" onfocus="if(wep4dis_2) this.blur();else this.form.keynum_2[3].checked=true;" />
                        </td>
                        <td>
			    <select name="keyLength4_2" class="select" style="width:80px;" id="keyLength4_2" onchange="setKey_2();">
                         	<option value="0" ><%write(com_forbidden);%></option>
				<option value="5"><%write(Bit64);%></option>
				<option value="13"><%write(Bit128);%></option>
			   </select>   
                         </td>
                    </tr>
                    <tr><td colspan="4">&nbsp;</td></tr>
                </table>
                
            </td>
        </tr>
        </div>
        <div id="wpatbl_2" style="display:none;">
        <tr class="tab_info" id="WPAversion_2">
            <td><%write(WPAVer);%></td>
            <td colspan="2">
            	<select name="wpaAuthMode_2" class="select" style="width:70px;">
		   <option value="WPA1WPA2"><%write(Auto);%></option>
		   <option value="WPA">WPA</option>
		   <option value="WPA2">WPA2</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="encryption_2">
            <td><%write(PWALG);%></td>
            <td colspan="2">
            	<select name="wpaCipher_2" class="select" style="width:70px;">
                   <option value="2" selected><%write(Auto);%></option>
                   <option value="AES">AES</option>
                   <option value="TKIP">TKIP</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="RadiusServiceIP_2">
            <td><%write(RadiusSerIP);%></td>
            <td colspan="2">
            	<input name="radiusIP_2" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  class="txt-input" />
            </td>
        </tr>
        <tr class="tab_info" id="Radiusport_2">
            <td><%write(RadiusPort);%></td>
            <td colspan="2">
            	<input name="radiusPort_2" type="text" class="txt-input" style="width:70px;"  onKeyPress="figureOnly()"   size="10" maxlength="5" />&nbsp;&nbsp;<%write(range1);%>
            </td>
        </tr>
        <tr class="tab_info" id="radiuspassword_2">
            <td><%write(RadiusPW);%></td>
            <td colspan="2">
            	<input name="radiusPsswd_2" type="password" size="18" class="txt-input" maxlength="31" />&nbsp;&nbsp;<%write(range2);%>
            </td>
        </tr>
        <tr class="tab_info" id="keyupdatetime_2">
            <td><%write(PWUpdateC);%></td>
            <td colspan="2">
            	<input name="keyUpdateCyc_2" type="text" size="10" maxlength="5" style="width:70px;" class="txt-input" onKeyPress="figureOnly()" />&nbsp;&nbsp;<%write(range3);%>
            </td>
        </tr>
        </div>
        <div id="wpapsktbl_2" style="display:none;">
        <tr class="tab_info" id="WPAversion2_2">
            <td><%write(WPAVer);%></td>
            <td colspan="2">
            	<select name="wpapskAuthMode_2" class="select" style="width:100px;">
                   <option value="WPAPSKWPA2PSK"><%write(Auto);%></option>
                   <option value="WPAPSK">WPA-PSK</option>
                   <option value="WPA2PSK">WPA2-PSK</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="encryption2_2">
            <td><%write(PWALG);%></td>
            <td colspan="2">
            	<select name="wpapskCipher_2" class="select" style="width:100px;">
                   <option value="TKIPAES" selected><%write(Auto);%></option>
                   <option value="AES">AES</option>
                   <option value="TKIP">TKIP</option>
                 </select>
            </td>
        </tr>
        <tr class="tab_info" id="pre-sharedkey_2">
            <td><%write(WSharePW);%></td>
            <td colspan="2">
		<span id="pass_2">
            	<input name="pskPsswd_2" id="pw_2" type="password" size="18" class="txt-input" />
		</span>
		&nbsp;<%write(com_dispass);%>&nbsp;<input type="checkbox" id="showpass_2"
		onclick="
		var temp=document.getElementById('pw_2').value;
		if(this.checked){
		document.getElementById('pass_2').innerHTML='<input type=text id=pw_2 name=pskPsswd_2 size=18  class=txt-input>';
		}   
		else
		{
		    document.getElementById('pass_2').innerHTML='<input type=password id=pw_2 name=pskPsswd_2 size=18 class=txt-input>';
		}
		document.getElementById('pw_2').value=temp;
		"/>
	        &nbsp;<%write(range4);%>
            </td>
        </tr>
        
        <tr class="tab_info" id="keyupdatetime2_2">
            <td><%write(PWUpdateC);%></td>
            <td colspan="2">
            	<input name="pskkeyUpdateCyc_2" type="text" style="width:70px;" size="10" maxlength="5" class="txt-input"  onKeyPress="figureOnly()" />&nbsp;&nbsp;<%write(range3);%>
            </td>
        </tr>
        </div>
        <tr class="tab_info" id="tr_country" name="tr_country">
        	<td><%write(wwsui_country);%></td>
            <td colspan="2"><select name="country" id="country" onChange = "changeChannel()" class="select" style="width:130px;">
	   <script language="javascript">
	   		for(i=0;i<countryopt.length;i++){
			 	document.write('<option value="'+i+'"'+(i==0?"selected":"")+'>'+countryopt[i]+'</option>');
			}
	   </script>
	   </select></td>
	   <input type="hidden" name="countryRegionABand" id = "countryRegionABand" />
        </tr>
        <tr class="tab_info noWAPI">
        	<td><%write(wire_channel);%></td>
            <td colspan="2">
	   <span id="channelstr" name="channelstr">
	   </span>
	   </td>
        </tr>
        <tr class="tab_info noWAPI">
        	<td><%write(WirelessMode);%></td>
            <td colspan="2"><select name="wrlessMode" id="wrlessMode" class="select" style="width:130px;" onChange="selwrlessMode(this.value)" >
	   <option value="2"><%write(chose4);%></option>
	   <option value="8"><%write(chose5);%></option>
	   <option value="14">11vht AC/AN/A</option>
	   <option value="15">11vht AC/AN</option>
</select>&nbsp;<%write(com_wireWarn3);%>&nbsp;</td>
        </tr>
        <tr class="tab_info noWAPI" id="chw">
        	<td><%write(ChannelBand);%></td>
            <td colspan="2"><select name="chanWidth" id="chanWidth" onChange="selVhtChannel()" class="select" style="width:90px;">
	      <option value="1"><%write(Auto);%></option><option value="0">20M</option><option value="2">40M</option>
       </select>&nbsp;&nbsp;20M<%write(com_wireWarn4);%></td>
        </tr>
        <tr class="tab_info" id="VHTchw">
        	<td><%write(VHTChannelBand);%></td>
            <td colspan="2"><select name="vhtChanWidth" id="vhtChanWidth" class="select" style="width:90px;">
	      <option value="0">20/40M</option><option value="1">80M</option>
	   </select></td>
        </tr>
        <tr class="tab_info noWAPI">
        	<td><%write(TxPower);%></td>
            <td colspan="2"><select name="txPower" id="txPower" class="select" style="width:90px;">
	      <option value=100><%write(TxHigh);%></option><option value=50><%write(TxMiddle);%></option><option value=20><%write(TxLow);%></option>
	   </select></td>
        </tr>
       <tr class="tab_info noWAPI">
        	<td><%write(wire_mac);%></td>
            <td colspan="2"><span name="mac" id="mac"></span></td>
        </tr>
        <tr class="tab_info noWAPI">
        	<td><%write(wire_openWDS);%></td>
            <td colspan="2">
            	<input name="wdsEnabled" type="checkbox" onClick="changeWDS(this.checked);" />
				<input type="hidden" name="wdsEnable" value="" />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:red;" id="apc_status"></span>
            </td>
        </tr>
        <script>
        if(Wmmenable == 1)
        {
            document.write(' <tr class="tab_info">');
            document.write(' <td><%write(wire_openWMM);%></td>');
            document.write(' <td colspan="2">');
            document.write(' <input name="wmmEnabled" id="wmmEnabled" type="checkbox"/>');
            document.write(' </td>');
            document.write(' </tr>');
        }
        </script>

        <tr class="tab_info" id="bridgessid_td">
        	<td><%write(eas_wireName);%></td>
            <td colspan="2">
            	<input name="bridgessid" id="bridgessid" type="text" size="18" maxlength="32" class="txt-input"  />

            </td>
        </tr>
        <tr class="tab_info" id="bridgebssid_td">
        	<td><%write(wire_macAddr);%></td>
            <td colspan="2">
            	<input name="bridgebssid" id="bridgebssid" type="text" size="18"  maxlength="32" class="txt-input"  />
            </td>
        </tr>
        <tr class="tab_info" id="scan_td">
        	<td></td>
            <td colspan="2">
            	<input type="button" value="<%write(wire_scan);%>"  onClick="scanlocation();"id="scan" class="input-button" />
            </td>
        </tr>
        <tr class="tab_info" id="securitymode_td">
        	<td><%write(SafeMode);%></td>
            <td colspan="2">
            	<select name="apclisecmode" id="apclisecmode" size="1" onChange="changeApcliseSecMode()" class="select">
                <option value="NONE"><%write(NoSafeMode);%></option>
                <option value="WEP">WEP</option>                
                <option value="WPAPSK">WPA-PSK/WPA2-PSK</option>
                  </select>
            </td>
        </tr>
        <tr class="tab_info" id="certificationmode_td" style="display:none;">
        	<td><%write(ApproveType);%></td>
            <td colspan="2">
            	<select name="wdswepAuthMode"  class="select">
               <option value="OPEN"><%write(OpenSys);%></option>
               <option value="SHARED"><%write(ShareKey);%></option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="keyformat_td">
        	<td><%write(PWForm);%></td>
            <td colspan="2">
            	<select name="wdsKeyFormat" onChange="setKey()"  class="select" style="width:90px;" >
                   <option value="0" selected><%write(Hex16);%></option>
                   <option value="1"><%write(ASCIICode);%></option>
                </select>
            </td>
        </tr>
         <tr class="tab_info" id="keychoose_td">
        	<td><%write(PWSelect);%></td>
            <td colspan="2">
            	<table border="0" cellpadding="0" cellspacing="0"  style="background:#5e5e5e;"  >
                	<tr>
                    	<td align="left" style="padding-left:0px;"></td>
                        <td></td>
                        <td><%write(PWWEP);%>
                        </td>
                        <td>
                        	<%write(PWType);%>  
                         </td>
                    </tr>
                	<tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert7);%></td>
                        <td><input type="radio" name="wdsKeySerNo" value="1" /></td>
                        <td><input type="text" class="txt-input" name="wdsWepKey1" class="txt-input" maxLength="32" size="30" onfocus="if(key1dis) this.blur();else this.form.wdsKeySerNo[0].checked=true;"/>
                        </td>
                        <td>
                        	<select name="wdsWepKey1Len" class="select" style="width:80px;" id='wdsWepKey1Len' onchange="setApclientKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>
        			      
        			       </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert9);%></td>
                        <td><input type="radio" name="wdsKeySerNo" value='2' /></td>
                        <td><input type="text" class="txt-input" name="wdsWepKey2" maxLength="32" size="30" onfocus="if(key2dis) this.blur();else this.form.wdsKeySerNo[1].checked=true;" />
                        </td>
                        <td>
                        	<select name="wdsWepKey2Len" class="select" style="width:80px;" id='wdsWepKey2Len' onchange="setApclientKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>
        			        
        			       </select>   
                         </td>
                    </tr>
                    <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert11);%></td>
                        <td><input type="radio" name="wdsKeySerNo" value='3' /></td>
                        <td><input type="text" class="txt-input" name="wdsWepKey3" maxLength="32" size="30" onfocus="if(key3dis) this.blur();else this.form.wdsKeySerNo[2].checked=true;" />
                        </td>
                        <td>
                        	<select name="wdsWepKey3Len" class="select" style="width:80px;" id='wdsWepKey3Len' onchange="setApclientKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>
        			        
        			       </select>   
                         </td>
                    </tr>
                     <tr>
                    	<td align="left" style="padding-left:0px;"><%write(JSAlert13);%></td>
                        <td><input type="radio" name="wdsKeySerNo" value='4' /></td>
                        <td><input type="text" class="txt-input" name="wdsWepKey4" maxLength="32" size="30" onfocus="if(key4dis) this.blur();else this.form.wdsKeySerNo[3].checked=true;" />
                        </td>
                        <td>
                        	<select name="wdsWepKey4Len" class="select" style="width:80px;" id='wdsWepKey4Len' onchange="setApclientKey();">
                         	<option value="0" ><%write(com_forbidden);%></option>
        					<option value="5"><%write(Bit64);%></option>
        				    <option value="13"><%write(Bit128);%></option>
        			        
        			       </select>   
                         </td>
                    </tr>
                    <tr><td colspan="4"></td></tr>
               </table>
            </td>
        </tr>
        <tr class="tab_info" id="WPAversion1_td">
        	<td><%write(WPAVer);%></td>
            <td colspan="2">
            	<select name="apcliwpapskAuthMode" class="select">
                   <option value="WPAPSK">WPA-PSK</option>
                   <option value="WPA2PSK">WPA2-PSK</option>
                   </select>
            </td>
        </tr>
        <tr class="tab_info" id="encryption1_td">
        	<td><%write(PWALG);%></td>
            <td colspan="2">
            <select name="apcliwpapskCipher" class="select">
               <option value="AES">AES</option>
               <option value="TKIP">TKIP</option>
               </select>
            </td>
        </tr>
        <tr class="tab_info" id="presharedkey1_td" style="display:">
        	<td><%write(SharePW);%></td>
            <td colspan="2">
           <input name="wdspskPsswd" type="password" size="18" class="txt-input" maxlength="63" />&nbsp;&nbsp;<%write(range4);%><input type="hidden" name="apcli_mode"><input type="hidden" name="apcli_enc"></td>

            </td>
        </tr>
        <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="right">
            <table cellpadding="0" cellspacing="0" width="600" border="0">
             <tr valign="top" style="background:#FFF;">
        	<td colspan="3" align="left"><span id="tips"><%write(wire_warn3);%><br>
<%write(wire_warn4);%></span></td>
			</tr>
             <tr><td align="right">
        <span id="error_msg"  style="color:red; padding-right:100px;"></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" name="back" id="savebtn" onClick="save();"value="<%write(com_save);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;
                 <input  type="hidden" name="cipher"><input type="hidden" name="keyRenewalInterval">
                 <input  type="hidden" name="cipher_2">
           
			</td></tr>
            <tr><td>&nbsp;</td></tr>
            </table>
            </td>
        </tr>
    </table>
    </div>
    <div id="wait_box" style="display:none;">
    
    <div style="text-align:center"><label><%write(wire_lessscan);%></label></div>
    <div style="text-align:center"><span id="reloadId">8</span>&nbsp;&nbsp;<%write(com_second);%></div>
    <br><br><br>
    </div>
  
   	<div id="list_box" style="display:none;">
    	 <div style="text-align:left;"><%write(wire_list);%></div>
         <br>
    	<div id="listinfo_box"></div>
        <div>
        	<table border="0" cellpadding="0" cellspacing="0" style="background:#5e5e5e; border-bottom:1px solid #5e5e5e; border-left:1px solid #5e5e5e; border-right:1px solid #5e5e5e;" class="group" >
                <tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onclick="scanlocation();" value="<%write(com_fresh);%>"/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="btn" onclick="goback();" value="<%write(com_back);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
           </table>

        </div>
    </div>
    </form>
<br>
<br>
</div>
</div>
<br>
</body>
</html>
