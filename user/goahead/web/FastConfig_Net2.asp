 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("FastConfig_Net2");%>
<%langTran("common");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("admin");
var errorstr="" ;
<% aspFastConfigNet(2); %> 

if(ConnTypes == "") 
{
	ConnTypes = "DHCP"
}


function initTranslation()
{
	var e;
/*	e = document.getElementById("jieshao");
	e.innerHTML= _("Net jieshao");*/
	e = document.getElementById("last");
	e.value= _("last");
/*	e = document.getElementById("uttreset");
	e.value= _("reset");*/
	e = document.getElementById("leave");
	e.value= _("leave");
	e = document.getElementById("next");
	e.value= _("next");

	e = document.getElementById("jieru");
	e.innerHTML= _("Net jieru");
	e = document.getElementById("ipaddr");
	e.innerHTML= _("Net ipaddr");
	e = document.getElementById("ziwang");
	e.innerHTML= _("Net ziwang");
	e= document.getElementById("wangguan");
	e.innerHTML= _("Net wangguan");
	e = document.getElementById("zhuDNS");
	e.innerHTML= _("Net zhuDNS");
	e = document.getElementById("beiDNS");
	e.innerHTML= _("Net beiDNS");

	e= document.getElementById("name");
	e.innerHTML= _("Net name");
	e= document.getElementById("password");
	e.innerHTML= _("Net password");

	e= document.getElementById("static");
	e.innerHTML= _("Net static");
	e = document.getElementById("dhcp");
	e.innerHTML= _("Net dhcp");
	e = document.getElementById("pppoe");
	e.innerHTML= _("Net pppoe");
}


function changeConnType(val){
	var f=FastConfig_Net_WAN2;
	 
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
	var f=FastConfig_Net_WAN2;
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
	var f=FastConfig_Net_WAN2;
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
   var f=FastConfig_Net_WAN2;
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
	var f=FastConfig_Net_WAN2;
	
		var indextext=getListValue(f.connectionTypew2);
		if(indextext=="STATIC"){
			if (f.staticIp2.value=="0.0.0.0"){ alert(_("warning1"));return false};
			if (!checkIP(f.staticIp2,"IP地址",0)) return false;
			if (!checkMask(f.staticNetmask2,"子网掩码",0)) return false;
			if (!checkIP(f.staticGateway2,"网关地址",0)) return false;
			if (!checkSameSubnet(f.staticIp2.value,f.staticGateway2.value,f.staticNetmask2.value))
			{
				alert(_("warning2"));
				return false;
			}
		
			if (!checkIP(f.staticPriDns2,"主DNS服务器",0)) return false;
			if (!checkIP(f.staticSecDns2,"备DNS服务器",1)) return false;
		}
		else if(indextext=="PPPOE"){
			if (!checkName(f.pppoeUser2,31,"用户名",0)) return false;
			if (!checkPassword1(f.pppoePass2,"31",0)) return false;
		}
	
	
	
	return true;
}
function init(){
	var f=FastConfig_Net_WAN2;
	if(f.connectionType2.value != "")
		{
			 f.connectionType2.value = "<% write(Form("connectionType2")); %>";
			 f.staticIp2.value ="<% write(Form("staticIp2")); %>";
			 f.staticNetmask2.value ="<% write(Form("staticNetmask2")); %>";
			 f.staticGateway2.value ="<% write(Form("staticGateway2")); %>";
			 f.staticPriDns2.value ="<% write(Form("staticPriDns2")); %>";
			 f.staticSecDns2.value ="<% write(Form("staticSecDns2")); %>";
			 f.pppoeUser2.value = "<% write(Form("pppoeUser2")); %>";
			 f.pppoePass2.value ="<% write(Form("pppoePass2")); %>";
					
			changeConnType(f.connectionType2.value);
			selVal(f.connectionTypew2,f.connectionType2.value);
			
			
		}
	else
		{
			changeConnType(ConnTypes);
		 	selVal(f.connectionTypew2,ConnTypes);
			var selIndex=f.connectionTypew2.selectedIndex;
 	 
			if(selIndex==1){//固定连接
				f.staticIp2.value=IPs;
				f.staticNetmask2.value=Masks;
				f.staticGateway2.value=GateWays;
				f.staticPriDns2.value=MainDnss;
				f.staticSecDns2.value=SecDnss;
			}
			else if(selIndex==2){//pppoe连接
				f.pppoeUser2.value=UserNames;
				f.pppoePass2.value=PassWds;
			}
			else{//动态连接
			
			}
 
		 }
	 
}
function Reset(){
	FastConfig_Net_WAN2.reset();
	FastConfig_Net_WAN2.connectionType2.value = "";
	init();
	for(i=1;i<=7;i++){
		document.all["yutd"+i].innerHTML="";
	}
}

function dopre(){
	if(checkData()){
		 FastConfig_Net_WAN2.connectionType2.value = FastConfig_Net_WAN2.connectionTypew2.value;
		 if(FastConfig_Net_WAN2.opMode1.value != "")
		 	FastConfig_Net_WAN2.action =  "FastConfig_Net.asp";
		 else {
             if ((platform == "nv520W")||(platform == "U1000N") ||(platform == "nv842W") ||(platform == "nv518W") ||(platform == "nv518W-VPN")||(platform == "nv512W")||(platform == "nv518W-Plus")||(platform == "nv518W-Lite")  || (platform == "nv520Wv3")|| (platform == "nv842Wv3")||(platform == "nv515W") || (platform == "nvD908W")) {
			 FastConfig_Net_WAN1.action = "FastConfig_OpMode_Ap.asp";
		     } else {
			 FastConfig_Net_WAN1.action = "FastConfig_OpMode.asp";
		     }
		     FastConfig_Net_WAN2.submit();
		 }
	}
}



function donext(){
	if(checkData()){
		var f=FastConfig_Net_WAN2;
					   
		f.action="SysInfo.asp"
		f.connectionType2.value = f.connectionTypew2.value;
		FastConfig_Net_WAN2.submit();
	}	
}

function dojump(){
			var f=FastConfig_Net_WAN2;
		f.action="SysInfo.asp"				 
		f.connectionType2.value = "";
		FastConfig_Net_WAN2.submit();

}


function goaway(){
	parent.main.location="SysInfo.asp";
}

</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("配置向导-WAN2口配置"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form  name="FastConfig_Net_WAN2" method="post">

<p>&nbsp;</p>
    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr>
        <td align="center" class="style7"><table border="0" width="600" align='center'>
          <tr>
            <td colspan="3" height="20">&nbsp;</td>
          </tr>
            <tr>
              <td colspan="3" align="center" height="30" valign="top" id="jieshao"><% write(fast_wan2); %><br></td>
            </tr>
		 
			
<tr   id="gatetable"><td colspan="3"><table width="100%" border="0" cellspacing="0" cellpadding="0">
 <!--//gateway table -->
            <tr>
	   <td align="right" width="38%" height="22" id="jieru">接入方式</td>
	   <td width="3%">&nbsp;</td>
	   <td width="59%"><select name="connectionTypew2" class="list"  onChange="changeConnType(this.value)">
	   	   <option value="DHCP" id="dhcp">动态IP接入</option>
	   <option value="STATIC" id="static">固定IP接入</option>
	    <option value="PPPOE" id="pppoe">PPPoE接入</option>
	   </select></td><input type="hidden" name="connectionType2" value="<% write(Form("connectionType2")); %>">
	   </tr>
	   <tr style="display:" id="britbl"><td colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
	   <td align="right"  height="25" width="38%" id="ipaddr">IP地址</td>
	   <td width="3%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td width="24%"><input type="text" name="staticIp2" size="18" style="ime-mode:Disabled"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'IP地址'),yutd1)"   ></td><td width="38%"  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="ziwang">子网掩码</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="staticNetmask2" size="18"  style="ime-mode:Disabled" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'子网掩码'),yutd2)" ></td>
	   <td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="wangguan">网关地址</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="staticGateway2" size="18" style="ime-mode:Disabled" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'网关地址'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="zhuDNS">主DNS服务器</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="staticPriDns2" size="18" style="ime-mode:Disabled"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'主DNS服务器'),yutd4)"  ></td><td  id="yutd4"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" id="beiDNS">备DNS服务器</td>
	   <td >&nbsp;</td>
	   <td><input type="text" name="staticSecDns2" size="18"  style="ime-mode:Disabled" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'备DNS服务器'),yutd5)"    ></td><td  id="yutd5"></td>
	   </tr>
</table>
</td></tr>
<tr style="display:none" id="pppoetbl"><td colspan="4" align="center"><table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
	   <td align="right"  height="25" width="38%" id="name">用户名</td>
	   <td width="3%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td  ><input type="text" name="pppoeUser2"  style="width:138px;ime-mode:Disabled" maxlength="32"   onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'用户名'),yutd6)"  ></td>
	   <td    id="yutd6"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="password">密码</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="password" name="pppoePass2" style="width:138px;ime-mode:Disabled"  maxlength="32"  onKeyPress="passwordonly()" onChange="showfiledtip(re_checkPassword1(this,'密码'),yutd7)" ></td><td  id="yutd7"></td>
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
                    <td  height="30" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="上一步" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="重填" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="离开" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="跳过" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="下一步" id="next"></td>
  				 
				 	<input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" >
					<input type="hidden" name="opMode" value="<% write(Form("opMode"));%>"  >
					<input type="hidden" name="oldopMode" value="<% write(Form("oldopMode"));%>" >
					<input type="hidden" name="opMode1"  value="<% write(Form("opMode1"));%>" >
					<input type="hidden" name="opMode2"  value="<% write(Form("opMode2"));%>">
					<input type="hidden" name="opMode3"  value="<% write(Form("opMode3"));%>">
					<input type="hidden" name="opMode4"  value="<% write(Form("opMode4"));%>">
					<input type="hidden" name="notPopUp" value="<% write(Form("notPopUp")); %>">
					 
					<input type="hidden" name="connectionType" value="<% write(Form("connectionType")); %>">
					<input type="hidden" name="staticIp" value="<% write(Form("staticIp")); %>">
					<input type="hidden" name="staticNetmask" value="<% write(Form("staticNetmask")); %>">
					<input type="hidden" name="staticGateway" value="<% write(Form("staticGateway")); %>">
					<input type="hidden" name="staticPriDns" value="<% write(Form("staticPriDns")); %>">
					<input type="hidden" name="staticSecDns" value="<% write(Form("staticSecDns")); %>">
					<input type="hidden" name="pppoeUser" value="<% write(Form("pppoeUser")); %>">
					<input type="hidden" name="pppoePass" value="<% write(Form("pppoePass")); %>">
					
					 
					<!-- <input type="hidden" name="connectionType3" value="<% write(Form("connectionType3"));%>" >
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
					<input type="hidden" name="wrlesMode_chanWid" value="<% write(Form("wrlesMode_chanWid"));%>" >-->

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



