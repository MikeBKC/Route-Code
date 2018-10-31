 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FastConfig_Wlan"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

<%getSysLang();%>

<% outputWlanBaseList_5g();%>

<!--
var channelopt=new Array(36,40,44,48,149, 153, 157, 161, 165);//信道选项

function checkData(){
	var f=FastConfig_Wlan_5g;
		if(!checkName(f.ssid,32,"SSID",0)) return false;
	return true;
		

}
function dopre(){
	if(checkData()){

		FastConfig_Wlan_5g.action = "FastConfig_Wlan.asp"
	 
		FastConfig_Wlan_5g.connectionType5.value = "";
		FastConfig_Wlan_5g.submit();
	}
}

function Reset(){
	FastConfig_Wlan_5g.reset();
	FastConfig_Wlan_5g.connectionType5.value = "";
	init();
}
function goaway(){
	parent.main.location="SysInfo.asp";
}
function done(){
	var f=FastConfig_Wlan_5g;
	if(!checkName(f.ssid,32,"SSID",0)) return;
	var chan_value=f.chanWidth.value;
	f.chanWidth.disabled=false;
	FastConfig_Wlan_5g.action="/goform/formConfigFastDirectionW_5g";
	FastConfig_Wlan_5g.submit();
}


function selwrlessMode(val){
	var f=FastConfig_Wlan_5g;
	if(val=="2"){
		document.getElementById("daikuan").style.display = "none";
		document.getElementById("VHTchw").style.display = "none";
		f.chanWidth.value="0";
		f.vhtChanWidth.value="0";
	} else if (val == "8") {
		document.getElementById("daikuan").style.display = "";
		document.getElementById("VHTchw").style.display = "none";
		f.chanWidth.value="1";
		f.vhtChanWidth.value="0";
//		f.chanWidth.disabled=false;
	} else {
		document.getElementById("daikuan").style.display = "";
		document.getElementById("VHTchw").style.display = "";
		f.chanWidth.value="1";
		f.chanWidth.disabled=false;
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=false;
	}	
}

function selVhtChannel(){
	var f=FastConfig_Wlan_5g;
	if(f.chanWidth.value =="0"){
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=true;
	} else {
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=false;
	}	
}

function init(){
	var f=FastConfig_Wlan_5g;
	f.ssid.value=ssids;
	f.wrlessMode.value=wrlessModes;
	f.channel.value=channels;

	f.chanWidth.value=chanWidths;
	f.vhtChanWidth.value=vhtChanWidths;

	if (f.wrlessMode.value == "2") {
		document.getElementById("daikuan").style.display = "none";	
		document.getElementById("VHTchw").style.display = "none";	
	} else if (f.wrlessMode.value == "8") {
		document.getElementById("daikuan").style.display = "";	
		document.getElementById("VHTchw").style.display = "none";	
	} else {
		document.getElementById("daikuan").style.display = "";
		document.getElementById("VHTchw").style.display = "";
	}
	if(f.chanWidth.value =="0"){
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=true;
	}
	
	ssidtd.innerHTML="";
	zhuyi.innerHTML="";
	helptd.style.display="none";
	if(f.opMode3.value!=""){
		helptd.style.display=dis;
		zhuyi.innerHTML="<strong><% write(Attention); %></strong><% write(WirelessAtt); %>";
	}
	else if(f.opMode4.value!=""){
		helptd.style.display=dis;
	    zhuyi.innerHTML="<strong><% write(Attention); %></strong><% write(G3Att); %>";
		}
	else
		helptd.style.display="none";
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
  <td><script language="javascript">showSecondTitle("<% write(Wlanxiangdao); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form  name="FastConfig_Wlan_5g" method="post">

<p>&nbsp;</p>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
  <tr><td colspan="3" align="center" height="30" valign="top" id="jieshao"><% write(ShowMsg5g); %><br></td>
</tr>
  <tr>
	 <td colspan="4"><table width="100%"><tr>
	   <td align="right" width="38%" height="20" id="ssid">SSID</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td  width="26%"><input name="ssid" type="text" size="18" maxlength="32"   onChange="showfiledtip(re_checkName(this,'SSID'),ssidtd)"></td><td width="34%"  id="ssidtd"></td>
	  </tr></table></td>
	   </tr>
	   <tr>
	   <td colspan="4"><table width="100%"><tr>
	   <td width="38%" align="right" height="20" id="moshi"><% write(WirelessMode); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width=""><select name="wrlessMode" class="list" onChange="selwrlessMode(this.value)">
	   <option value="2" id="wrlessmode1"><% write(chose4); %></option>
	   <!--<option value="11N">11n-only</option>-->
	   <option value="8" id="wrlessmode2"><% write(chose5); %></option>
	   <option value="14" id="wrlessmode3"><% write(chose6); %></option>
	   <option value="15" id="wrlessmode3"><% write(chose7); %></option>
	   </select></td>
	   </tr></table></td>
	   </tr>
	   <tr>
	   <td colspan="4"><table width="100%"><tr>
	   <td width="38%" align="right" height="20" id="xindao"><% write(Chain); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="channel" class="list"><option value="0" id="auto1"><% write(Automatic); %></option>
	   <script language="javascript">
	   		for(i=0;i<channelopt.length;i++){
			 	document.write('<option value="'+channelopt[i]+'"'+(i==5?"selected":"")+'>'+channelopt[i]+'</option>');
			}
	   </script>
	   </select></td>
	   </tr></table></td>
	   </tr>
	   <tr>
	   <td colspan="4"><table width="100%"  style = "display:" id="daikuan"><tr>
	   <td width="38%" align="right" height="20" ><% write(ChainnelBandW); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="chanWidth" class="list" onChange = "selVhtChannel()">
	      <option value="1"><% write(Automatic); %></option><option value="0">20M</option><option value="2">40M</option>
	   </select></td>
	   </tr></table></td>
	   </tr>

	   <tr>
	   <td colspan="4"><table width="100%" style = "display:" id = "VHTchw"><tr>
	   <td width="38%" align="right"  height="20" id="vht channel bandwidth"><% write(VHTChannelBand); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="vhtChanWidth" class="list">
	      <option value="0">20/40</option><option value="1">80M</option>
	   </select></td>
	   </tr></table></td>
	   </tr>

	   <tr>
	   <td colspan="4" id="helptd" style="display:none"><table width="100%"><tr height="30">
<td class="helpfont"  align="center" id="zhuyi"></td></tr>
	   </table></td>
	   </tr>
	  <!-- <tr>
	   <td align="right" width="35%" height="25"><% write(Speed); %></td>
	   <td width="4%">&nbsp;</td>
	   <td><select name="rate" class="list">
	      <option value="auto"><% write(Automatic); %></option>
	   </select></td>
	   </tr>-->
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
 <tr><td colspan="3"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td height="30"  align="center"><input type="hidden" name="connectionType5" value="<% write(Form("connectionType5"));%>" ><input id="last" type="button" value="<% write(com_previous); %>" class="input-button" onClick="dopre()"> &nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="<% write(ReWrite); %>" id="uttreset">&nbsp;&nbsp;&nbsp;<input id="leave" type="button" value="<% write(LeavePage); %>" class="input-button" onClick="goaway()">&nbsp;&nbsp;&nbsp;<input id="end" type="button" value="<% write(Finsh); %>" class="input-button" onClick="done()"></td>					<input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" >	
					<input type="hidden" name="wrlesMode_chanWid">
					<input type="hidden" name="notPopUp" value="<% write(Form("notPopUp")); %>">
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

					<input type="hidden" name="ssid_24" value="<% write(Form("ssid_24"));%>" >
					<input type="hidden" name="wrlessModes_24" value="<% write(Form("wrlessModes_24"));%>" >
					<input type="hidden" name="channel_24" value="<% write(Form("channel_24"));%>" >
					<input type="hidden" name="chanWidth_24" value="<% write(Form("chanWidth_24"));%>" >
	
  </tr>
</table>
</td></tr><script language="javascript">init();</script>
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


