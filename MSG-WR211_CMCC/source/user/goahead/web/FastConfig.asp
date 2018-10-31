 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FastConfig");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

//Butterlate.setTextDomain("admin");
<%outputTheErrorMsg();%> 
<%aspFastConfigPopWelcome();%> 
<% OutputConnNum();%> 
function donext(){
    if (isWirelessSupport()) {
      if ((platform == "nv520W")|| (platform == "nv842W")|| (platform == "nv518W")|| (platform == "nv518W-VPN")|| (platform == "nv512W") || (platform == "U1000N") || (platform == "nv520Wv3") || (platform == "nv842Wv3")|| (platform == "nv515W") || (platform == "nvD908W")) {
	    FastConfig1.action="FastConfig_OpMode_Ap.asp";
	} else {
	    FastConfig1.action="FastConfig_OpMode.asp";
	}
    } else {
	FastConfig1.action="FastConfig_Net.asp";
    }
	if(connNum==1)
	FastConfig1.action="FastConfig_Net.asp";

	FastConfig1.submit();
}
function exitGuide(){
	parent.main.location="SysInfo.asp";
}

//function initTranslation()
//{
//	var e;
//	e = document.getElementById("shuoming");
//	e.innerHTML= _("shuoming");
//	e = document.getElementById("exit");
//	e.value= _("exit");
//	e = document.getElementById("next");
//	e.value= _("next");
//}


function SaveUserAllow()
{
	var str;
 
	FastConfig1.action ="/goform/formFastConfPop";    //需要添加form的地方，当被选择的时候提交，没有被选择的时候按照以前的在点击下一步后处理。 
	FastConfig1.submit();
			


	
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(fas_guideConfig);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form  name="FastConfig1" method="post">

<p>&nbsp;</p>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
   <tr><td colspan="3" height="20">&nbsp;</td></tr>
  <tr><td colspan="3" id="shuoming"><%write(guideContent1);%>
  <br>
<%write(guideContent2);%>
<br>
<%write(guideContent3);%></td>
</tr>
<tr><td height="50"><input name="notPopUp" type="checkbox"   onClick= "SaveUserAllow()">
<script language="javascript">document.write(_("<%write(fas_nomore);%>"));</script></td></tr>


 <tr><td><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td height="30"  align="center"><input id="exit" type="button" value="<%write(quitGuide);%>" class="input-button" onClick="exitGuide()">&nbsp;&nbsp;&nbsp;<input id="next" type="button" value="<%write(com_next);%>" class="input-button" onClick="donext()"></td><input type="hidden"  name="aa" value="1">
					<input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" >
					<input type="hidden" name="oldopMode">
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
				
				    <input type="hidden" name="connectionType4" value="<% write(Form("connectionType4"));%>" >
					<input type="hidden" name="APssid" value="<% write(Form("APssid"));%>" >
					<input type="hidden" name="wds_list" value="<% write(Form("wds_list"));%>" >
					<input type="hidden" name="apcli_mode" value="<% write(Form("apcli_mode"));%>" >
					<input type="hidden" name="apcli_enc" value="<% write(Form("apcli_enc"));%>" >
					<input type="hidden" name="apclikeyFormat" value="<% write(Form("apclikeyFormat"));%>" >
					<input type="hidden" name="apclikeynum" value="<% write(Form("apclikeynum"));%>" >
					<input type="hidden" name="apcliwepkey1" value="<% write(Form("apcliwepkey1"));%>" >
					<input type="hidden" name="apcliwepkey2" value="<% write(Form("apcliwepkey2"));%>" >
					<input type="hidden" name="apcliwepkey3" value="<% write(Form("apcliwepkey3"));%>" >
					<input type="hidden" name="apcliwepkey4" value="<% write(Form("apcliwepkey4"));%>" >
					<input type="hidden" name="apclikeyLength1" value="<% write(Form("apclikeyLength1"));%>" >
					<input type="hidden" name="apclikeyLength2" value="<% write(Form("apclikeyLength2"));%>" >
					<input type="hidden" name="apclikeyLength3" value="<% write(Form("apclikeyLength3"));%>" >
					<input type="hidden" name="apclikeyLength4" value="<% write(Form("apclikeyLength4"));%>" >
					<input type="hidden" name="apclipskPsswd" value="<% write(Form("apclipskPsswd"));%>" >
					
					
					 <input type="hidden" name="connectionType5" value="<% write(Form("connectionType5"));%>" >
					<input type="hidden" name="ssid" value="<% write(Form("ssid"));%>" >
					<input type="hidden" name="wrlessMode" value="<% write(Form("wrlessMode"));%>" >
					<input type="hidden" name="channel" value="<% write(Form("channel"));%>" >
					<input type="hidden" name="chanWidth" value="<% write(Form("chanWidth"));%>" >
					<input type="hidden" name="wrlesMode_chanWid" value="<% write(Form("wrlesMode_chanWid"));%>" >
  </tr>
  <script language="javascript">
  	document.forms[0].notPopUp.checked = (notPopUps=="1"?true:false);
  </script>
</table>
</td></tr>
<tr><td colspan="3" height="20">&nbsp;</td></tr>
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


