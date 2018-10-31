<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("matter");
function initTranslation() 
{
    var e;
    e = document.getElementById("leixing");
    e.innerHTML= _("leixing");
    e = document.getElementById("zhuangtai");
    e.innerHTML= _("zhuangtai");
    e = document.getElementById("ipaddr");
    e.innerHTML= _("ip addr");
    e = document.getElementById("subnet");
    e.innerHTML= _("subnet");
    e = document.getElementById("gateway");
    e.innerHTML= _("gateway");
    e = document.getElementById("MACaddr");
    e.innerHTML= _("MAC addr");
    e = document.getElementById("zhuDNS");
    e.innerHTML= _("zhuDNS");
    e = document.getElementById("beiDNS");
    e.innerHTML= _("beiDNS");
    e = document.getElementById("update");
    e.value= _("update");
    e = document.getElementById("zhuangtai2");
    e.innerHTML= _("zhuangtai");
    e = document.getElementById("pinduan");
    e.innerHTML= _("pinduan");
    e = document.getElementById("mode");
    e.innerHTML= _("mode");
    e = document.getElementById("MACaddr3");
    e.innerHTML= _("MAC addr");
}

var errorstr="";
var ConnOTimes = new Array();
var ConnTypesw = new Array();
var ConnTypes = new Array();
var ConnStatuss= new Array();

<%outputNetConfig();%>

var wan_IPs = new Array();
var wan_Masks = new Array();
var wan_Macs= new Array();
var wan_GateWays = new Array();
var wan_MainDnss = new Array();
var wan_SecDnss = new Array();

ConnStatuss[0]=ConnStatuss[2];
ConnTimes[0]=ConnTimes[2];
SpeedDowns[0]=SpeedDowns[2];
SpeedUps[0]=SpeedUps[2];
ConnStatuss[1]=ConnStatuss[3];
ConnTimes[1]=ConnTimes[3];
SpeedDowns[1]=SpeedDowns[3];
SpeedUps[1]=SpeedUps[3];

 
ConnTypes[0] =  "<%getCfgGeneral(1, "Wan3ConnMode");%>";
wan_IPs[0]="<%getWanIp(3);%>";
wan_Masks[0]="<%getWanNetmask(3);%>";
wan_GateWays[0]="<%getWanGateway(3);%>";
wan_Macs[0]="<%getWanMac("3");%>"
wan_MainDnss[0]="<%getWanDns(3, 1);%>";
wan_SecDnss[0]="<%getWanDns(3, 2);%>";

ConnTypes[1] =  "<%getCfgGeneral(1, "Wan4ConnMode");%>";
wan_IPs[1]="<%getWanIp(4);%>";
wan_Masks[1]="<%getWanNetmask(4);%>";
wan_GateWays[1]="<%getWanGateway(4);%>";
wan_Macs[1]="<%getWanMac(4);%>"
wan_MainDnss[1]="<%getWanDns(4, 1);%>";
wan_SecDnss[1]="<%getWanDns(4, 2);%>";

for(i=0;i<2;i++){
	var runday=parseInt((eval(ConnTimes[i])/3600)/24);
	var runhour=parseInt((eval(ConnTimes[i])/3600)%24);
	var runmiu=parseInt((eval(ConnTimes[i])%3600)/60);
	var runsec=parseInt((eval(ConnTimes[i])%3600)%60);
	ConnOTimes[i] = runday+"天"+runhour+"小时"+runmiu+"分"+runsec+"秒";
}
 
 

<% outputWlanRunStation();%>

var APModeStasw="";
switch(APWorkModes){
	case "1": APModeStasw="AP Mode";
					break;
	case "5": APModeStasw="APClient Mode";
			  		break;
    case "3": APModeStasw="Repeater Mode";
			  		break;
	case "2": APModeStasw="Bridge Mode";
			  		break;
	case "4": APModeStasw="Lazy Mode";
			  		break;
	default: break;
}
wrlessMacs="<% getWlanMac(); %>";
 
 
////////////////////////////end////////////////////////////////
function init(){
	var f=WireStatus_Config;
	

		 
 
		if(ConnTypes[0] != "")
		{

			ConnTypesw[0]="3G接入";
			wlan2table.style.display = dis;
			f.ConnType.value=ConnTypesw[0];
			f.ConnStatus.value = ( ConnStatuss[0] =="1"?"已连接":"未连接");
			if(ConnStatuss[0]=="0" && wan_IPs[0]=="10.64.64.64")
				wan_IPs[0] = "";
			f.wan_IP.value=wan_IPs[0];
			f.wan_Mask.value=wan_Masks[0];
			f.wan_Mac.value=wan_Macs[0];
			f.wan_GateWay.value=wan_GateWays[0];
			f.MainDns.value=wan_MainDnss[0];
			f.SecDns.value=wan_SecDnss[0];
			f.ConnTime.value = ConnOTimes[0];
		}
		else
		{
			//3gtable.style.display = "none";
		}
		
		
		if(ConnTypes[1] == "")
			wlantable.style.display="none";
		else{
					if(ConnTypes[1]=="STATIC"){
						 ConnTypesw[1]="固定IP接入";
					}
					else if(ConnTypes[1]=="DHCP") {
						ConnTypesw[1]="动态IP接入";
					}
					else if(ConnTypes[1]=="PPPOE") {
						ConnTypesw[1]="PPPoE接入";
					}
			f.ConnType2.value=ConnTypesw[1];
			f.ConnStatus2.value = ( ConnStatuss[1] =="1"?"已连接":"未连接");
			if(ConnStatuss[1]=="0" && wan_IPs[1]=="10.64.64.64")
				wan_IPs[1] = "";
			f.wan_IP2.value=wan_IPs[1];
			f.wan_Mask2.value=wan_Masks[1];
			f.wan_Mac2.value=wan_Macs[1];
			f.wan_GateWay2.value=wan_GateWays[1];
			f.MainDns2.value=wan_MainDnss[1];
			f.SecDns2.value=wan_SecDnss[1];
			f.ConnTime2.value = ConnOTimes[1];
		}
	
	
	f.wlessconnStat.value=(wlessconnStats=="on"?"启用":"禁用");
	f.APModeSta.value=APModeStasw;
	f.ssid.value=ssids;
        if(channels == "0")
	    f.channle.value=_("auto");
        else
	    f.channle.value=channels;
	var strtext="";
	switch(wrlessModes){
		case "4": strtext="仅11g";break;
		case "6":strtext="仅11n";break;
		case "9":strtext="11b/g/n混合";break;
		default :break;
	}
	f.wrlessMode.value=strtext;
//	f.wrlessIP.value=wrlessIPs;
//	f.wrlessMask.value=wrlessMasks;
//	if(wrlessMacs!="") wrlessMacs=wrlessMacs.replace(/:/g,"");
	f.wrlessMac.value=wrlessMacs;
	
}
/////////////////////////////////////end///////////////////////
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
  <td><script language="javascript">showSecondTitle(_("RunStatues"));</script>
  <table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,
						//new Array(_("GlobalConfig	"),'DoubleRoad.asp',1,0),
						//new Array(_("DoubleRoadInfo"),'DoubleRoadInfo.asp',1,1)
						new Array("有线状态",'RunStatus.asp',1,0),
						new Array("无线状态",'RunStatus_Wire.asp',1,1)
		);
		showThreeMenu();
	</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <form  name="WireStatus_Config" method="post"><td align="center"  class="three_td">
		<table border="0" width="600" align='center'><tr>
        <td  colspan="5" height="26">&nbsp;</td>
      </tr></table>
	  
	<table border="0" width="600" align='center' id="wlan2table"  style="display:none">
	   <tr>
        <td width="30%" align="right" height="25" valign="bottom" > 
            <strong>3G状态：</strong></td>
        <td  colspan="5">&nbsp;</td>
      </tr>
      <tr>
        <td width="30%" align="right" height="25" id="leixing">连接类型</td>
        <td width="3%">&nbsp;</td>
        <td  width="12%"><input  name="ConnType" class="input3" size="23" disabled></td>
        <td width="20%" align="right" id="zhuangtai">连接状态</td>
        <td width="3%"></td>
        <td ><input  name="ConnStatus" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="ipaddr">IP地址</td>
        <td >&nbsp;</td>
        <td  ><input  name="wan_IP" class="input3" size="23" disabled></td>
        <td  align="right" id="subnet">子网掩码</td>
        <td ></td>
        <td><input  name="wan_Mask" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="gateway">网关地址</td>
        <td >&nbsp;</td>
        <td  ><input  name="wan_GateWay" class="input3" size="23" disabled></td>
        <td  align="right" id="MACaddr">MAC地址</td>
        <td ></td>
        <td><input  name="wan_Mac" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="zhuDNS">主DNS服务器</td>
        <td >&nbsp;</td>
        <td  ><input  name="MainDns" class="input3" size="23" disabled></td>
        <td  align="right" id="beiDNS">备DNS服务器</td>
        <td ></td>
        <td><input  name="SecDns" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="shijian">连接时间</td>
        <td >&nbsp;</td>
        <td  ><input  name="ConnTime" class="input3" size="33" disabled></td>
      </tr>
      <tr>
        <td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
            </tr>
        </table></td>
      </tr>
	  </table>
	  <table border="0" width="600" align='center' style="display:" id="wlantable">
      <tr>
        <td width="30%" align="right" height="25" valign="bottom" id="APsign"> 
            <strong>APClient状态：</strong></td>
        <td  colspan="5">&nbsp;</td>
      </tr>
      <tr>
        <td width="30%" align="right" height="25" id="leixing2">连接类型</td>
        <td width="3%">&nbsp;</td>
        <td  width="12%"><input  name="ConnType2" class="input3" size="23" disabled></td>
        <td width="20%" align="right" id="zhuangtai">连接状态</td>
        <td width="3%"></td>
        <td ><input  name="ConnStatus2" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="ipaddr2">IP地址</td>
        <td >&nbsp;</td>
        <td  ><input  name="wan_IP2" class="input3" size="23" disabled></td>
        <td  align="right" id="subnet">子网掩码</td>
        <td ></td>
        <td><input  name="wan_Mask2" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="gateway2">网关地址</td>
        <td >&nbsp;</td>
        <td  ><input  name="wan_GateWay2" class="input3" size="23" disabled></td>
        <td  align="right" id="MACaddr">MAC地址</td>
        <td ></td>
        <td><input  name="wan_Mac2" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="zhuDNS2">主DNS服务器</td>
        <td >&nbsp;</td>
        <td  ><input  name="MainDns2" class="input3" size="23" disabled></td>
        <td  align="right" id="beiDNS2">备DNS服务器</td>
        <td ></td>
        <td><input  name="SecDns2" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="shijian2">连接时间</td>
        <td >&nbsp;</td>
        <td  ><input  name="ConnTime2" class="input3" size="33" disabled></td>
      </tr>
      <tr>
        <td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
            </tr>
        </table></td>
      </tr>
	  </table>
	  
	   <table border="0" width="600" align='center'>
      <tr>
        <td width="27%" align="right" height="20" valign="bottom"  id="wireless"><strong>无线状态：</strong></td>
      </tr>
      <tr>
        <td width="27%" align="right" height="25" id="zhuangtai2">连接状态</td>
        <td width="2%">&nbsp;</td>
        <td  width="28%"><input  name="wlessconnStat" class="input3" size="23" disabled></td>
        <td width="15%" align="right" id="apworkmode">AP工作模式</td>
        <td width="2%"></td>
        <td><input  name="APModeSta" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td width="27%" align="right" height="25" id="SSID">SSID</td>
        <td width="2%">&nbsp;</td>
        <td  width="28%"><input  name="ssid" class="input3" size="23" disabled></td>
        <td width="15%" align="right"  id="mode">无线模式</td>
        <td width="2%"></td>
        <td><input  name="wrlessMode" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td  align="right" height="25" id="pinduan">信道</td>
        <td >&nbsp;</td>
        <td  ><input  name="channle" class="input3" size="23" disabled></td>
        <td  align="right" id="MACaddr3">MAC地址</td>
        <td ></td>
        <td  ><input  name="wrlessMac" class="input3" size="23" disabled></td>
      </tr>
      <tr>
        <td colspan="6" height="10">&nbsp;</td>
      </tr>
      <tr>
        <td  colspan="6"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
            <tr>
              <td  height="20" align="center" valign="top"><input name="button" type="button" class="input-button" onClick="location.reload()" value="刷新" id="update"></td>
            </tr>
			   <tr><td colspan="3" height="20">&nbsp;</td></tr>
        </table></td>
      </tr>
    </table></td>
</form></tr>
</table>

</td>
<script language="javascript">init();</script>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
	      
