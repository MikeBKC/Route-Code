 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FastConfig_Net_WLAN"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">

<%getSysLang();%>
var errorstr="";
<%
   outputWlanBaseList();
%>

<!--
function changeConnType(val){
	var f=FastConfig_Net_WLAN;
	if(val=="DHCP"){
		britbl.style.display="none";
		pppoetbl.style.display="none";
	}
	else if(val=="PPPOE"){
		britbl.style.display="none";
		pppoetbl.style.display=dis;
	}
	else{
		britbl.style.display=dis;
		pppoetbl.style.display="none";
	}
}
function changeOpmode(val){
	var f=FastConfig_Net_WLAN;
	if(val=="3"){
		gatetable.style.display="none";
		g3table.style.display="none";
		apclitable.style.display=dis;
	}
	else if(val=="4"){
		gatetable.style.display="none";
		g3table.style.display=dis;
		apclitable.style.display="none";
	}
	else{
		gatetable.style.display=dis;
		g3table.style.display="none";
		apclitable.style.display="none";
	}
}
function changeAuth3g(val){
	var f=FastConfig_Net_WLAN;
	if(val=="SIM"){
		simtd.style.display=dis;
		pawdtd1.style.display="none";
		pawdtd2.style.display="none";
	}
	else if(val=="PassAuth"){
		simtd.style.display="none";
		pawdtd1.style.display=dis;
		pawdtd2.style.display=dis;
	}
}
function onProviderChange()
{
   var f=FastConfig_Net_WLAN;
   var providerId=f.ISP3G.selectedIndex;
   var providerVal=getListValue(f.ISP3G);
	 if(providerVal !=ISP3Gs){
	   f.ConnName.value=g3_para_arr[providerId][4];
	   f.DialNum.value=g3_para_arr[providerId][1];
	   f.PPPName.value=g3_para_arr[providerId][2];
	   f.PPPPasswd.value=g3_para_arr[providerId][3];
   }else{
   	//new Array("中国移动","*99***1#","","","CMNET"),/**/
   	 f.ConnName.value=ConnNames;
	 f.DialNum.value=DialNums;
	 f.PPPName.value=PPPNames;
	 f.PPPPasswd.value=PPPPasswds;
   }
}
function checkData(){
	var f=FastConfig_Net_WLAN;
	 
	 
		if(!checkName(f.APssid,32,"<% write(CheckAPSSIDName); %>",0)) return false;
		if(!checkMac(f.wds_list,"<% write(CheckAPMAC); %>",0)) return false;
		var selIndex=getListValue(f.apclisecmode);
			if(selIndex=="WEP"){
				if(!checkWEP(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeyFormat.selectedIndex)) return false;
			}
			else if(selIndex=="WPAPSK"){
				var len=f.apclipskPsswd.value.length;
				if(len<8){alert("<% write(PassWordLen); %>");return false;}
				if(!checkPassword1(f.apclipskPsswd,"63",0)) return false;	
			}
	 
	return true;
}
function init(){
	var f=FastConfig_Net_WLAN;
	/*
	if(apclikeynums == "" || apclikeynums == "0")apclikeynums = 1;
	if(APCliMac == "" || APCliMac == "000000000000")APCliMac = "0022aa112233";
	var APMacs = APCliMac;
	*/
	if(f.connectionType4.value != "")
	{
		var APssids = "<% write(Form("APssid"));%>"  ;
		var APMacs = "<%write(Form("wds_list"));%>"  ;
		var apcliSecModes = "<%write(Form("apcli_mode"));%>"  ;
		var apcliAuthModes = "<%write(Form("apcli_enc"));%>"  ;
		var apclikeyFormats  = "<%write(Form("apclikeyFormat"));%>"  ;
		var apclikeynums  = "<%write(Form("apclikeynum"));%>"  ;
		var apcliwepkey1s  = "<%write(Form("apcliwepkey1"));%>"  ;
		var apcliwepkey2s  = "<%write(Form("apcliwepkey2"));%>"  ;
		var apcliwepkey3s  = "<%write(Form("apcliwepkey3"));%>"  ;
		var apcliwepkey4s  = "<%write(Form("apcliwepkey4"));%>"  ;
		var apclikeyLength1s  =  "<%write(Form("apclikeyLength1"));%>"  ;
		var apclikeyLength2s  =  "<%write(Form("apclikeyLength2"));%>"  ;
		var apclikeyLength3s  =   "<%write(Form("apclikeyLength3"));%>"  ;
		var apclikeyLength4s =  "<%write(Form("apclikeyLength4"));%>"  ;
		var apclipskPsswds  = "<%write(Form("apclipskPsswd"));%>" ;
	}
	else
	{
		<% outputWlanBaseList();%>
		var APMacs = APCliMac;
	}
 

var web_secModes="NONE";
var web_AuthModes="OPEN";
var web_wpapskAuthModes="WPAPSK";


	if(apcliAuthModes=="NONE") web_secModes=apcliAuthModes;
	if((apcliSecModes=="OPEN"||apcliSecModes=="SHARED")&&apcliAuthModes!="NONE"){
		web_secModes="WEP";
		web_AuthModes=apcliSecModes;
	}
	else if(apcliSecModes=="WPAPSK"||apcliSecModes=="WPA2PSK"){
		web_secModes="WPAPSK";
		web_wpapskAuthModes=apcliSecModes;
	}
	
	
	
	
		f.APssid.value=APssids;
		f.wds_list.value=APMacs;
		f.apclisecmode.value=web_secModes;
		if(f.apclisecmode.value=="") f.apclisecmode.value="NONE";
		selVal(f.apcliwepAuthMode,web_AuthModes);
		f.apclikeyFormat.value=(apclikeyFormats==""?"1":apclikeyFormats);
		f.apcliwepkey1.value=apcliwepkey1s;
		f.apclikeyLength1.value=(apclikeyLength1s==""?"0":apclikeyLength1s);
		f.apcliwepkey2.value=apcliwepkey2s;
		f.apclikeyLength2.value=(apclikeyLength2s==""?"0":apclikeyLength2s);
		f.apcliwepkey3.value=apcliwepkey3s;
		f.apclikeyLength3.value=(apclikeyLength3s==""?"0":apclikeyLength3s);
		f.apcliwepkey4.value=apcliwepkey4s;
		f.apclikeyLength4.value=(apclikeyLength4s==""?"0":apclikeyLength4s);
		apclikeynums=(apclikeynums==0?1:apclikeynums);
		f.apclikeynum[apclikeynums-1].checked=true;
	
		f.apcliwpapskAuthMode.value=web_wpapskAuthModes;
		selVal(f.apcliwpapskCipher,apcliAuthModes);
		if(f.apcliwpapskCipher.value=="") f.apcliwpapskCipher.selectedIndex=0;
		f.apclipskPsswd.value=apclipskPsswds;
		changSecMode();
	        setKey();
 
}
function Reset(){
	FastConfig_Net_WLAN.reset();
	FastConfig_Net_WLAN.connectionType4.value = "";
	init();
	for(i=1;i<=3;i++){
		document.all["yutd"+i].innerHTML="";
	}
}

function dopre(){
	if(checkData()){
		if(FastConfig_Net_WLAN.opMode4.value != "")
			FastConfig_Net_WLAN.action ="FastConfig_Net_3G.asp";
		else if(FastConfig_Net_WLAN.opMode2.value != "")
		 	FastConfig_Net_WLAN.action =  "FastConfig_Net2.asp";
		else if(FastConfig_Net_WLAN.opMode1.value != "")
		 	FastConfig_Net_WLAN.action =  "FastConfig_Net.asp";
		 else {
             if ((platform == "nv520W")|| (platform == "U1000N")|| (platform == "nv842W")|| (platform == "nv518W")|| (platform == "nv518W-VPN")|| (platform == "nv512W")|| (platform == "nv518W-Plus")|| (platform == "nv518W-Lite") || (platform == "nv520Wv3") || (platform == "nv842Wv3") || (platform == "nv515W") || (platform == "nvD908W")) {
			 FastConfig_Net_WLAN.action = "FastConfig_OpMode_Ap.asp";
		     } else {
			 FastConfig_Net_WLAN.action = "FastConfig_OpMode.asp";
		     }
		 }
		 
		  FastConfig_Net_WLAN.connectionType4.value = "DHCP";
		 FastConfig_Net_WLAN.submit();
	 }
	
}
function donext(){
	if(checkData()){
		var f=FastConfig_Net_WLAN;
		var secModeVal=getListValue(f.apclisecmode);
			if(secModeVal=="NONE"){
				f.apcli_mode.value="OPEN";
				f.apcli_enc.value="NONE";
			}
			else if(secModeVal=="WEP"){
				var wepAuthMode=getListValue(f.apcliwepAuthMode);
				f.apcli_mode.value=wepAuthMode;
				f.apcli_enc.value="WEP";
			}
			else if(secModeVal=="WPAPSK"){
				var wpapskAuthMode=getListValue(f.apcliwpapskAuthMode);
				var wpapskCipher=getListValue(f.apcliwpapskCipher);
				f.apcli_mode.value=wpapskAuthMode;//alert(wpapskAuthMode);
				f.apcli_enc.value=wpapskCipher;
			}
			//f.wds_list.value=WebMacTOLinuxMac(f.APMac0.value);
			//f.wds_list.value=WebMacTOLinuxMac(f.wds_list.value);
		 

		
		//f.staticIp.disabled=f.staticNetmask.disabled=f.staticGateway.disabled=false;
	
		f.connectionType4.value = "DHCP";
		FastConfig_Net_WLAN.opMode.value = "3";
		FastConfig_Net_WLAN.action="FastConfig_Wlan.asp";
		FastConfig_Net_WLAN.submit();
	}	
}

function dojump(){
		var f=FastConfig_Net_WLAN;
		var secModeVal=getListValue(f.apclisecmode);
			if(secModeVal=="NONE"){
				f.apcli_mode.value="OPEN";
				f.apcli_enc.value="NONE";
			}
			else if(secModeVal=="WEP"){
				var wepAuthMode=getListValue(f.apcliwepAuthMode);
				f.apcli_mode.value=wepAuthMode;
				f.apcli_enc.value="WEP";
			}
			else if(secModeVal=="WPAPSK"){
				var wpapskAuthMode=getListValue(f.apcliwpapskAuthMode);
				var wpapskCipher=getListValue(f.apcliwpapskCipher);
				f.apcli_mode.value=wpapskAuthMode;//alert(wpapskAuthMode);
				f.apcli_enc.value=wpapskCipher;
			}
			//f.wds_list.value=WebMacTOLinuxMac(f.APMac0.value);
			//f.wds_list.value=WebMacTOLinuxMac(f.wds_list.value);
		 

		
		//f.staticIp.disabled=f.staticNetmask.disabled=f.staticGateway.disabled=false;
	
		f.connectionType4.value = "";
		FastConfig_Net_WLAN.opMode.value = "3";
		FastConfig_Net_WLAN.action="FastConfig_Wlan.asp";
		FastConfig_Net_WLAN.submit();
}

function goaway(){
	parent.main.location="SysInfo.asp";
}
var key1dis = 0;
var key2dis = 0;
var key3dis = 0;
var key4dis = 0;
var keylength1 = 10;
var keylength2 = 10;
var keylength3 = 10;
var keylength4 = 10;
function setKey()
{
	var f = FastConfig_Net_WLAN;
	getdisable(f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4);
	auto_disable(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeynum,f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4);
	auto_length(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4,f.apclikeyFormat.selectedIndex);
	/*
	getdisable();
	auto_disable();
	auto_length();*/
}
function changSecMode(){
    var frm=FastConfig_Net_WLAN;
	var index=getListValue(frm.apclisecmode);
	wutbl.style.display="none";
	weptbl1.style.display="none";
	wepauthmode.style.display="none";
	wpapsktbl1.style.display="none";
	var dis="";
	if(   document.all   )   
    	dis="block";   
 	else   
     	dis="table-row";
	switch(index){
		case "NONE":weptbl1.style.display="none";
					wepauthmode.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display=dis;
					break;
		case "WEP":	weptbl1.style.display=dis;
					wepauthmode.style.display=dis;
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case "WPAPSK":weptbl1.style.display="none";
					wepauthmode.style.display="none";
					wpapsktbl1.style.display=dis;
					wutbl.style.display="none";	
					break;
		default:	break;
	}

}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(SecondTital); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form  name="FastConfig_Net_WLAN" method="post">

<p>&nbsp;</p>
    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr>
        <td align="center" class="style7"><table border="0" width="600" align='center'>
          <tr>
            <td colspan="3" height="20">&nbsp;</td>
          </tr>
            <tr>
              <td colspan="3" align="center" height="30" valign="top" id="jieshao"><% write(ShowMsg); %><br></td>
            </tr>
			
			<tr id="apclitable"><td colspan="3"><table width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr >
	   <td align="right" width="38%" height="25"><input type="hidden" name="connectionType4" value="<% write(Form("connectionType4"));%>" ><% write(CheckAPSSIDName); %></td>
	   <td width="3%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td width="32%"><input name="APssid" type="text" size="18" maxlength="32"  onKeyPress="englishnamedonly()"  onChange="showfiledtip(re_checkName(this,'<% write(CheckAPSSIDName); %>'),yutd1)"></td><td id="yutd1" width="31%"></td>
	   </tr>
 <tr >
	   <td align="right"  height="25"><% write(CheckAPMAC); %></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="wds_list" type="text" size="18" style="ime-mode:Disabled"  maxlength="17"  onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(CheckAPMAC); %>'),yutd2)" ></td><td  id="yutd2"></td>
	   </tr>
	   <!--//apcli secmod-->
<tr> 
  <td align="right" width="38%" height="25"><% write(SafeMode); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="apclisecmode" id="apclisecmode" size="1" onChange="changSecMode()">
	   <option value="NONE"><!--<script language="javascript">document.write(_("no safe mode"));</script>--><% write(NoSafeMode); %></option>
	<option value="WEP">WEP</option>
	<option value="WPAPSK">WPA-PSK/WPA2-PSK</option>
      </select></td>
  </tr>
  <!--//wep-->
  <tr id="wutbl" style=" display:none"><td colspan="4"></td></tr>
	   <!--///weptbl///-->
	   <tr id="weptbl1" style=" display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr id="wepauthmode" style="display:none">
	   <td align="right"  width="38%" height="25"  id="certification mode"><% write(ApproveType); %></td>
	   <td  width="3%">&nbsp;</td>
	   <td width="62%"><select name="apcliwepAuthMode" class="list">
	   <option value="OPEN"><% write(OpenSys); %><!--<script language="javascript">document.write(_("opensys"));</script>--></option>
	   <option value="SHARED"><% write(SharePW); %><!--<script language="javascript">document.write(_("sharekey"));</script>--></option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="key format"><% write(PWForm); %></td>
		<td >&nbsp;</td>
		<td ><select name="apclikeyFormat" class="list" onChange="setKey()">
	   <option value="0" selected><!--<script language="javascript">document.write(_("16 hex"));</script>--><% write(Scale16); %></option>
	   <option value="1"><!--<script language="javascript">document.write(_("ASCII code"));</script>--><% write(ASCII); %></option>
	   </select></td>
	   </tr>
	   <tr>
	   <td width="35%" height="25" align="right" id="key choose"><% write(SelectPW); %></td>
	   <td width="3%">&nbsp;</td>
	   <td><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr>
	   <td width="55%" id="wep key"><% write(WEPPW); %></td>
	   <td id="key mode"><% write(PWType); %></td>
	   </tr></table></td>
	   </tr>
	   <script language="javascript">
	   for (var i=1; i<5; i++)
        		{   
        			j = i-1;
        			document.write('<TR><TD  width="26%" height="25" align="right"> '+"<% write(Key); %>"+i+'：<input type=radio name=apclikeynum value='+i+'></td><td width="2%">&nbsp;</td>');
        			document.write('<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input type=text class=text name=apcliwepkey'+i+' maxLength=32 size="30" onfocus="if(key'+i+'dis) this.blur();else this.form.apclikeynum['+j+'].checked=true;"  style="ime-mode:Disabled" onKeyPress="englishnamedonly()" onChange="showfiledtip(re_checkEnglishName(this),wepkeytd'+i+')"></td>' );
        			document.write('<TD ><select name=apclikeyLength'+i+' class=list id=apclikeyLength'+i+' onchange="setKey();">');
        			document.write('<option value=0 >'+"<% write(Unable); %>"+'</option>');
        			document.write('<option value=5 >'+"<% write(Bit64); %>"+'</option>');
        			document.write('<option value=13>'+"<% write(Bit128); %>"+'</option>');
        			document.write('</select></td><td id=wepkeytd'+i+'></td></tr></table></td></tr>');
        		}
	   </script>
</table>
	   </td></tr>
  <!--//wpapsk-->
  <tr id="wpapsktbl1" style="display:none"><td colspan="6"><table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr>
      <td align="right" width="38%" height="25" id="WPA version2"><% write(WPAVer); %></td>
      <td width="3%">&nbsp;</td>
      <td><select name="apcliwpapskAuthMode" class="list">
          <option value="WPAPSK">WPA-PSK</option>
          <option value="WPA2PSK">WPA2-PSK</option>
      </select></td>
    </tr>
    <tr>
      <td height="25" align="right" id="encryption2"><% write(EncryptionAlg); %></td>
      <td >&nbsp;</td>
      <td ><select name="apcliwpapskCipher" class="list">
          <option value="TKIP">TKIP</option>
          <option value="AES">AES</option>
      </select></td>
    </tr>
    <tr>
      <td height="25" align="right" id="pre-shared key"><% write(YSharePW); %></td>
      <td ><img src="images/qt.gif" width="7" height="9"></td>
      <td  width="32%"><input name="apclipskPsswd" type="text" size="18" maxlength="63"  onKeyPress="passwordonly()"  style="ime-mode:Disabled" onChange="showfiledtip(re_checkPassword1(this,
63),yutd3)">
        &nbsp;<!--<script language="javascript">document.write(_("range4"));</script>--></td>
      <td  width="31%"id="yutd3"></td>
      <input type="hidden" name="apcli_mode" value="<% write(Form("apcli_mode"));%>" >
      <input type="hidden" name="apcli_enc" value="<% write(Form("apcli_enc"));%>" >
    </tr>
    <tr>
      <td height="20" align="right"></td>
      <td ></td>
      <td  width="32%"><% write(ShowMsg1); %></td>
    </tr>
  </table></td>
  </tr>

	   <!--//apcli secmod en-->
</table></td></tr>
			
            <tr>
              <td colspan="3" height="20">&nbsp;</td>
            </tr>
            <tr>
              <td colspan="3"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                  <tr>
                     <td  height="30" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="<% write(com_previous); %>" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="<% write(ReWrite); %>" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="<% write(LeavePage); %>" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="<% write(SkipPage); %>" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="<% write(com_next); %>" id="next"></td>
					<input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" >
                    <input type="hidden" name="notPopUp" value="<% write(Form("notPopUp"));%>" >
					<input type="hidden" name="opMode" value="<% write(Form("opMode"));%>" >
					<input type="hidden" name="oldopMode" value="<% write(Form("oldopMode"));%>" >
			
					<input type="hidden" name="opMode1"  value="<% write(Form("opMode1"));%>" >
					<input type="hidden" name="opMode2"  value="<% write(Form("opMode2"));%>">
					<input type="hidden" name="opMode3"  value="<% write(Form("opMode3"));%>">
					<input type="hidden" name="opMode4"  value="<% write(Form("opMode4"));%>">
					<input type="hidden" name="connectionType" value="<% write(Form("connectionType")); %>">
					<input type="hidden" name="staticIp" value="<% write(Form("staticIp")); %>">
					<input type="hidden" name="staticNetmask" value="<% write(Form("staticNetmask")); %>">
					<input type="hidden" name="staticGateway" value="<% write(Form("staticGateway")); %>">
					<input type="hidden" name="staticPriDns" value="<% write(Form("staticPriDns")); %>">
					<input type="hidden" name="staticSecDns" value="<% write(Form("staticSecDns")); %>">
					<input type="hidden" name="pppoeUser" value="<% write(Form("pppoeUser")); %>">
					<input type="hidden" name="pppoePass" value="<% write(Form("pppoePass")); %>">
					  

					<input type="hidden" name="connectionType2" value="<% write(Form("connectionType2")); %>">
					<input type="hidden" name="staticIp2" value="<% write(Form("staticIp2")); %>">
					<input type="hidden" name="staticNetmask2" value="<% write(Form("staticNetmask2")); %>">
					<input type="hidden" name="staticGateway2" value="<% write(Form("staticGateway2")); %>">
					<input type="hidden" name="staticPriDns2" value="<% write(Form("staticPriDns2")); %>">
					<input type="hidden" name="staticSecDns2" value="<% write(Form("staticSecDns2")); %>">
					<input type="hidden" name="pppoeUser2" value="<% write(Form("pppoeUser2")); %>">
					<input type="hidden" name="pppoePass2" value="<% write(Form("pppoePass2")); %>">
					

					<input type="hidden" name="connectionType3" value="<% write(Form("connectionType3"));%>" >
					<input type="hidden" name="Dev3G" value="<% write(Form("Dev3G"));%>" >
					<input type="hidden" name="ISP3G" value="<% write(Form("ISP3G"));%>" >
					<input type="hidden" name="AuthType3G" value="<% write(Form("AuthType3G"));%>" >
					<input type="hidden" name="PinCode" value="<% write(Form("PinCode"));%>" >
					<input type="hidden" name="AuthName" value="<% write(Form("AuthName"));%>" >
					<input type="hidden" name="AuthPasswd" value="<% write(Form("AuthPasswd"));%>" >
					<input type="hidden" name="ConnName" value="<% write(Form("ConnName"));%>" >
					<input type="hidden" name="PPPPasswd" value="<% write(Form("PPPPasswd"));%>" >
					<input type="hidden" name="DialNum" value="<% write(Form("DialNum"));%>" >
					<input type="hidden" name="PPPName" value="<% write(Form("PPPName"));%>" >
					
					<input type="hidden" name="connectionType5" value="<% write(Form("connectionType5"));%>" >
					<input type="hidden" name="ssid" value="<% write(Form("ssid"));%>" >
					<input type="hidden" name="wrlessMode" value="<% write(Form("wrlessMode"));%>" >
					<input type="hidden" name="channel" value="<% write(Form("channel"));%>" >
					<input type="hidden" name="chanWidth" value="<% write(Form("chanWidth"));%>" >
					<input type="hidden" name="wrlesMode_chanWid" value="<% write(Form("wrlesMode_chanWid"));%>" >

            
					</tr>
              </table></td>
            </tr>
          <script language="javascript">init();</script>
            <tr>
              <td colspan="3" height="20">&nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table>
    </form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>



