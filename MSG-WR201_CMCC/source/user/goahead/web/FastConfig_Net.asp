 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FastConfig_Net");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("admin");
var errorstr="" ;
<% aspFastConfigNet(1); %>
<% OutputConnNum();%> 
 
if(ConnTypes == "")
{
    ConnTypes = "DHCP" 
}
function initIntroduce()
{
	if(platform == "nv1800N")
	{
		document.getElementById('jieshao').innerHTML="<%write (configGuideTip1);%>";
	}
}
/*功能：检查是否合法用户名并返回提示信息,用户名可为全0*/
function re_checkName_allowZero(cntrl, tips) {
    var retips = "";
    var name = cntrl.value;
    if (name == "") {
            retips = tips + "<%write(com_null);%>";
            return retips;
    }
    if (name == null) {
            retips = tips + "<%write(com_null);%>";
            return retips;
    }

    if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
        retips = tips + "<%write(com_chkName);%>";
    }
    return retips;
}
function save()
{
	if(!checkData()) return;
    var f=FastConfig_Net_WAN1;
    f.connectionType.value = f.connectionTypew.value;
    f.action="/goform/formConfigFastDirection";
    f.submit();
}
function changeConnType(val){
    var f=FastConfig_Net_WAN1;
    	f.connectionTypew.value=val;
    if(val=="DHCP" || val=="3G"){
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
    var f=FastConfig_Net_WAN1;
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
    var f=FastConfig_Net_WAN1;
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
   var f=FastConfig_Net_WAN1;
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
    var f=FastConfig_Net_WAN1;
    var indextext=getListValue(f.connectionTypew);
    	if(indextext=="STATIC"){
    	    if (f.staticIp.value=="0.0.0.0"){ alert(_("<%write(com_checkIPAlert);%>"));return false};
    	    if (!checkIP(f.staticIp,"<%write(com_IP);%>",0)) return false;
    	    if (!checkMask(f.staticNetmask,"<%write(com_NetMask);%>",0)) return false;
    	    if (!checkIP(f.staticGateway,"<%write(com_GateWayAddr);%>",0)) return false;
    	    if (!checkSameSubnet(f.staticIp.value,f.staticGateway.value,f.staticNetmask.value))
    	    {
    	    	alert(_("<%write(com_subnetAlert);%>"));
    	    	return false;
    	    }
			if(platform != "nv1800N")
			{
    	    	if (!checkMainDns(f.staticPriDns,"<%write(com_MDNS);%>",0)) return false;
    	    	if (!checkIP(f.staticSecDns,"<%write(com_SDNS);%>",1)) return false;
			}
    	}
    	else if(indextext=="PPPOE"){
    	    if (!checkEnglishName_allowZero(f.pppoeUser,31,"<%write(com_user);%>",0)) return false;
    	    if (!checkPassword1(f.pppoePass,"31",0)) return false;
    	}
    	return true;
}

function init(){
    var f=FastConfig_Net_WAN1;
    changeConnType(ConnTypes);
    selVal(f.connectionTypew,ConnTypes);
    var selIndex=f.connectionTypew.selectedIndex;
    if(selIndex==1){//固定连接
    	f.staticIp.value=IPs;
    	f.staticNetmask.value=Masks;
    	f.staticGateway.value=GateWays;
    	f.staticPriDns.value=MainDnss;
    	f.staticSecDns.value=SecDnss;
    } else if(selIndex==2){//pppoe连接
    	f.pppoeUser.value=UserNames;
    	f.pppoePass.value=PassWds;
    }
}

function Reset(){
    FastConfig_Net_WAN1.reset();
    FastConfig_Net_WAN1.connectionTypew.value = "";
    init();
    for(i=1;i<=7;i++){
        document.all["yutd"+i].innerHTML="";
    }
}
 
function dopre(){
    //if(checkData()){ 返回上一步不应该check  否则无法返回
    if ((platform == "nv520W")|| (platform == "U1000N") || (platform == "nv842W")|| (platform == "nv518W")|| (platform == "nv518W-VPN")|| (platform == "nv512W")|| (platform == "nv518W-Plus")|| (platform == "nv518W-Lite") || (platform == "nv520Wv3") || (platform == "nv842Wv3")|| (platform == "nv515W") || (platform == "nvD908W")) {
	    FastConfig_Net_WAN1.action = "FastConfig_OpMode_Ap.asp";
	} else if( (platform == "nv750W")||(platform == "nvA755W")|| (platform == "nv1220GW") || (platform == "nvA750W") || (platform == "nvA650W") ||(platform == "nvA755W")|| (platform == "nv521GW") || (platform == "nv750GW") || (platform == "nvA650W_INT") || (platform == "u1000Nv2") || (platform == "nvA655W")||(platform == "nv658W)) {
		FastConfig_Net_WAN1.connectionType_OpMode.value = "on";
	    FastConfig_Net_WAN1.action = "FastConfig.asp";
	} else {
	    FastConfig_Net_WAN1.action = "FastConfig_OpMode.asp";
	}

	if(connNum==1)
	    FastConfig_Net_WAN1.action = "FastConfig.asp";
    	FastConfig_Net_WAN1.connectionType.value = FastConfig_Net_WAN1.connectionTypew.value;
    	FastConfig_Net_WAN1.submit();
   // }
}

function donext(){
    /*if(checkData()){
    	var f=FastConfig_Net_WAN1;
    	FastConfig_Net_WAN1.opMode.value = FastConfig_Net_WAN1.opMode2.value;
     	f.action="SysInfo.asp"
    	f.connectionType.value = f.connectionTypew.value;
    	FastConfig_Net_WAN1.submit();
    }	*/
	if(checkData()){
		var f=FastConfig_Net_WAN1;
	
		if(!FastConfig_Net_WAN1.opMode2.value == "")
		{
				  		FastConfig_Net_WAN1.opMode.value = FastConfig_Net_WAN1.opMode2.value;
	 					FastConfig_Net_WAN1.action="FastConfig_Net2.asp";
		 }
		 else
		{
				if(!FastConfig_Net_WAN1.opMode4.value == "")
				 {
						 FastConfig_Net_WAN1.opMode.value = FastConfig_Net_WAN1.opMode4.value;
						 FastConfig_Net_WAN1.action="FastConfig_Net_3G.asp";
				 }else{
						 if(!FastConfig_Net_WAN1.opMode3.value == ""){
								 FastConfig_Net_WAN1.opMode.value = FastConfig_Net_WAN1.opMode3.value;
								 FastConfig_Net_WAN1.action="FastConfig_Net_WLAN.asp";
						 }else{
									  FastConfig_Net_WAN1.opMode.value = FastConfig_Net_WAN1.opMode1.value;
									  if ( (platform == "nv750W")|| (platform == "nv1220GW") || (platform == "nvA750W") ||(platform == "nvA755W")|| (platform == "nvA650W")||(platform == "nvA755W") || (platform == "nv521GW") || (platform == "nv750GW") || (platform == "nvA650W_INT") ||(platform == "u1000Nv2") || (platform == "nvA655W")||(platform == "nv658W")) {
										f.opMode1.value="1";
										f.connectionType_OpMode.value = "on";
									  }
									  FastConfig_Net_WAN1.action="FastConfig_Wlan.asp";
						  }
				 }
		}
		
 		f.connectionType.value = f.connectionTypew.value;
		FastConfig_Net_WAN1.submit();
	}	
}

function dojump(){
    var f=FastConfig_Net_WAN1;
    
	if(f.opMode2.value != "")
	{
		f.opMode.value = f.opMode2.value;
	 	f.action="FastConfig_Net2.asp";
	} else if(f.opMode4.value != ""){
		f.opMode.value = f.opMode4.value;
		f.action="FastConfig_Net_3G.asp";
	}else if(f.opMode3.value != ""){
		f.opMode.value = f.opMode3.value;
		f.action="FastConfig_Net_WLAN.asp";
	}else{
		f.opMode.value = f.opMode1.value;
		f.action="FastConfig_Wlan.asp";
	}
	
    f.connectionType.value = "";
    FastConfig_Net_WAN1.submit();
}
function goaway(){
	parent.main.location="SysInfo.asp";
}
function initDisplay()
{
	if(platform != "nv1800N")
	{
		moduleDisplay('zhuDNS',1);
		moduleDisplay('beiDNS',1);
	}
	else
	{
		FastConfig_Net_WAN1.connectionTypew.options[2] = null;/*去掉pppoe接入方式*/
	}
}
function initPage()
{
	initDisplay();
	initIntroduce();
}
</script>
</head>
<body onLoad="initPage();" onbeforeunload="logOut()" background="<%write(com_bodyMain);%>"> 
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript"> if( platform=="nv1800N") showSecondTitle("<%write(configGuide1);%>");else showSecondTitle("<%write(configGuide2);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form  name="FastConfig_Net_WAN1" method="post">

<p>&nbsp;</p>
    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr>
        <td align="center" class="style7"><table border="0" width="600" align='center'>
          <tr>
            <td colspan="3" height="20">&nbsp;</td>
          </tr>
            <tr>
              <td colspan="3" align="center" height="30" valign="top" id="jieshao"><%write(configGuideTip2);%><br></td>
            </tr>
		<tr   id="gatetable"><td colspan="3"><table width="600" border="0" cellspacing="0" cellpadding="0">
            <tr><td colspan="4"><table width="600" border="0" cellspacing="0" cellpadding="0">
            <tr>
	   <td align="right" width="41%" height="22" id="jieru"><%write(com_accessType);%></td>
	   <td width="2%">&nbsp;</td>
	   <td align="left"><select name="connectionTypew" class="list" onChange="changeConnType(this.value)">
	    <option value="DHCP" id="dhcp" ><%write(com_DynamicIP);%></option>
	   <option value="STATIC" id="static"><%write(com_StaticIP);%></option>
	    <option value="PPPOE" id="pppoe"><%write(com_PPPoE);%></option>
	   </select></td><td width="27%">&nbsp;</td></tr> </table></td></tr>
	   <tr style="display:" id="britbl"><td colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
	   <td align="right"  height="25" width="41%" id="ipaddr"><%write(com_IP);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" style="ime-mode:Disabled" name="staticIp" size="18"  onKeyPress="ipOnly()"   maxlength="15"   onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),yutd1)"></td><td width="27%"  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="ziwang"><%write(com_NetMask);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="staticNetmask" style="ime-mode:Disabled" size="18"  onKeyPress="ipOnly()" maxlength="15"  onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>'),yutd2)"></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="wangguan"><%write(com_GateWayAddr);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="staticGateway" style="ime-mode:Disabled" size="18"  onKeyPress="ipOnly()" maxlength="15"  onChange="showfiledtip(re_checkIP(this,'<%write(com_GateWayAddr);%>'),yutd3)" ></td><td  id="yutd3"></td>
	   </tr>
	   <tr id="zhuDNS" style="display:none">
	   <td align="right"  height="25"><%write(com_MDNS);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="staticPriDns" size="18" style="ime-mode:Disabled"   onKeyPress="ipOnly()" maxlength="15"   onChange="showfiledtip(re_checkMainDns(this,'<%write(com_MDNS);%>'),yutd4)"></td><td  id="yutd4"></td>
	   </tr>
	    <tr id="beiDNS" style="display:none">
	   <td align="right"  height="25"><%write(com_SDNS);%></td>
	   <td >&nbsp;</td>
	   <td><input type="text" name="staticSecDns" size="18" style="ime-mode:Disabled"  onKeyPress="ipOnly()" maxlength="15"    onChange="showfiledtip(re_checkIP_empty(this,'<%write(com_SDNS);%>'),yutd5)"></td><td  id="yutd5"></td>
	   </tr>
</table>
</td></tr>
<tr style="display:none" id="pppoetbl"><td colspan="4" align="center"><table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
	   <td align="right"  height="25" width="41%" id="name"><%write(com_user);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input type="text" name="pppoeUser" size="18" maxlength="31"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName_allowZero(this,'<%write(com_user);%>'),yutd6)" ></td>
	   <td width="27%" id="yutd6"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="password"><%write(com_passwd);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="password" name="pppoePass" size="18" style="ime-mode:Disabled" maxlength="31"  onKeyPress="passwordonly()" onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd7)" ></td><td  id="yutd7"></td>
	   </tr></table></td></tr>
	   <!--//gateway end -->
</table>
</td></tr>
            <tr>
              <td colspan="3" height="20">&nbsp;</td>
            </tr>
            <tr>
              <td colspan="3"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                  <tr>
                    <td  height="30" align="center">
<script language="javascript">
if ((isWirelessSupport()) || (platform == "nv750W")|| (platform == "nv1220GW") || (platform == "nvA750W") || (platform == "nvA650W") ||(platform == "nvA755W")|| (platform == "nv521GW") || (platform == "nv750GW") || (platform == "nvA650W_INT") || (platform == "u1000Nv2") || (platform == "nvA655W")||(platform == "nvA658W")) {
	document.write('<input name="button" type="button" class="input-button" onClick="dopre()" value="<%write(com_previous);%>" id="last">&nbsp;&nbsp;&nbsp;');
}
</script>
					<input name="button" type="button" class="input-button" onClick="Reset()" value="<%write(com_reset);%>" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="<%write(leave);%>" id="leave">&nbsp;&nbsp;&nbsp;
<script language="javascript">
if ((isWirelessSupport()) || (platform == "nv512Wv3") || (platform == "nv750W")||(platform == "nvA755W")|| (platform == "nv1220GW") || (platform == "nvA750W") || (platform == "nvA650W") || (platform == "nv521GW") || (platform == "nv750GW") || (platform == "nvA650W_INT") ||(platform == "u1000Nv2") || (platform == "nvA655W")||(platform == "nvA658W")) {
	document.write('<input name="button" type="button" class="input-button" onClick="dojump()" value="<%write(skip);%>" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="<%write(com_next);%>" id="next">&nbsp;&nbsp;&nbsp;');
} else {
	document.write('<input name="button" type="button" class="input-button" onClick="save()" value="<%write(com_complete);%>" id="next">&nbsp;&nbsp;&nbsp;');
}
</script>
					<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#FastConfig')"></td>
  				    <input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" >
					<input type="hidden" name="opMode" value=1 >
					<input type="hidden" name="oldopMode" value="<% write(Form("oldopMode"));%>" >
					<input type="hidden" name="opMode1"  value="<% write(Form("opMode1"));%>" >
					<input type="hidden" name="opMode2"  value="<% write(Form("opMode2"));%>">
					<input type="hidden" name="opMode3"  value="<% write(Form("opMode3"));%>">
					<input type="hidden" name="opMode4"  value="<% write(Form("opMode4"));%>">
					<input type="hidden" name="notPopUp" value="<% write(Form("notPopUp")); %>">
					<input type="hidden" name="connectionType" value="<% write(Form("connectionType")); %>">
					
					<input type="hidden" name="connectionType2" value="<% write(Form("connectionType2")); %>">
					<input type="hidden" name="staticIp2" value="<% write(Form("staticIp2")); %>">
					<input type="hidden" name="staticNetmask2" value="<% write(Form("staticNetmask2")); %>">
					<input type="hidden" name="staticGateway2" value="<% write(Form("staticGateway2")); %>">
					<input type="hidden" name="staticPriDns2" value="<% write(Form("staticPriDns2")); %>">
					<input type="hidden" name="staticSecDns2" value="<% write(Form("staticSecDns2")); %>">
					<input type="hidden" name="pppoeUser2" value="<% write(Form("pppoeUser2")); %>">
					<input type="hidden" name="pppoePass2" value="<% write(Form("pppoePass2")); %>">
					
			 
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
				
					<input type="hidden" name="APssid" value="<% write(Form("APssid"));%>" >
					<input type="hidden" name="wds_list" value="<% write(Form("wds_list"));%>" >
				
					<input type="hidden" name="connectionType4" value="<% write(Form("connectionType4"));%>" >
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
					<input type="hidden" name="connectionType3" value="<% write(Form("connectionType3"));%>" >
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
