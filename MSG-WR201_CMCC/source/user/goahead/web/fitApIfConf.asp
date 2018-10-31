<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("commonWords");%>
<%langTran("WANConfig");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("internet");
var strSecondMenuFocus="<%write(com_LinkList);%>";
function initTranslation()
{
}

<%Output3GConfig();%> 
<%aspWanConfig(); %>
<%aspOutPutApEffectConf();%>

var SpeedDownsOld = new Array();
var SpeedUpsOld = new Array();

/*
var totalrecs=2;
var Dev3Gs="ZTE-MF637U";
var ISP3Gs="CMC";
var AuthType3Gs="SIM";
var PinCodes="";
var AuthNames="";
var AuthPasswds="";
var ConnNames="CMNET";
var DialNums="*99***1#";
var PPPNames="CMNET";
var PPPPasswds="CMNET";
*/
var maxtotalrecs = totalrecs;

<%aspIspRunningConfig();%>
/* 修改的变量   
IPs[1] = "";
Masks[1] = "";
GateWays[1] = "";
ConnStatuss[1]="0";
ConnTimes[1]="0";
SpeedDowns[1]="0";
SpeedUps[1]="0";
*/
var config=1;
var ConnStatussw=new Array();
var PortNamesw=new Array();
var ConnTypesw=new Array();
var tmpIndexs=new Array();

for(i=0;i<totalrecs;i++){
	tmpIndexs[i]=i;
		 
	if(is3GPort(i + 1,totalrecs)){
		PortNamesw[i] = "3G";
	} else if (isAPCliPort(i + 1,totalrecs)) {
		PortNamesw[i] = "APClient";
	}
	else
	{
		PortNamesw[i]="WAN"+PortNames[i];
	}
	
	
	
	ConnTypesw[i]=(ConnTypes[i]=="STATIC"?"<%write(com_staticAccess);%>":(ConnTypes[i]=="DHCP"?"<%write(com_DynamicAccess);%>":(ConnTypes[i]=="PPPOE"?"<%write(com_PPPoE);%>":"<%write(com_G3Access);%>")));
	ConnStatussw[i]=(ConnStatuss[i]=="1"?"<%write(com_Connected);%>":"<%write(com_interrupt);%>");
	var runhour=parseInt(eval(ConnTimes[i])/3600);
	var runmiu=parseInt((eval(ConnTimes[i])%3600)/60);
	var runsec=parseInt((eval(ConnTimes[i])%3600)%60);
	if(ConnStatuss[i]=="1"&&ConnTypes[i]!="STATIC")
		ConnStatussw[i]+=" "+runhour+"<%write(com_hour);%>"+runmiu+"<%write(com_minute);%>"+runsec+"<%write(com_second);%>";
	if(ConnTypes[i]==""){
		ConnTypesw[i]="<font color=red><%write(com_unConfigured);%></font>";
		ConnStatussw[i]=IPs[i]=Masks[i]=GateWays[i]=SpeedDowns[i]=SpeedUps[i]=SpeedDownsOld[i]=SpeedUpsOld[i]="";
	}else{
		SpeedDownsOld[i] =  parseInt(eval(SpeedDowns[i])/8000);
		SpeedUpsOld[i] =  parseInt(eval(SpeedUps[i])/8000);
	}
}
 

recsPerPage[0] = maxtotalrecs;
currpage[0]=1;
recscount[0]=totalrecs;
recsPerPage[0] = 4;
maxrecs[0]=maxtotalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]="<%write(com_LinkList);%>";
var titles1=new Array(7);
var ii=0;
titles1[ii++]="<%write(com_Interface);%>";
titles1[ii++]="<%write(com_ConnectionType);%>";
titles1[ii++]="<%write(com_ConnectionStatus);%>";
titles1[ii++]="<%write(com_IP);%>";
titles1[ii++]="<%write(com_NetMask);%>";
titles1[ii++]="<%write(com_GateWayAddr);%>";
titles1[ii++]="<%write(com_downRates);%>(KB/s)";
titles1[ii++]="<%write(com_upRates);%>(KB/s)";
 
arrayData1=new Array(PortNamesw,ConnTypesw,ConnStatussw,IPs,Masks,GateWays,SpeedDownsOld,SpeedUpsOld,PortNames,ConnStatuss,ConnTypes,tmpIndexs);
sortable[0]=new Array(0,0,0,0,0,0,0,0,0,0,0,0);
sortType[0]=new Array(TYPE_CHINESE,TYPE_CHINESE,TYPE_CHINESE,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_NUM,TYPE_NUM,TYPE_STR,TYPE_STR,TYPE_STR,TYPE_NUM);
arrayCount[0]=12;
 
function dsplist(index,list)
{
	if(index==0) dspList(index,list,760,titles1,8,arrayData1,0,1);
}

function init(){
	var f=NetConfig;
	EditConfig(1,0);
 	if (totalrecs > 1 && platform != "nv1800N") {
		showTable(ispTr, 0);
	}
}
var SeniorShowEn=0;
function clickSenior(frm, adv) {
    if(SeniorShowEn == 0) {
        SeniorShowEn = 1;
    } else {
        SeniorShowEn = 0;
    }
    showTable(adv, SeniorShowEn);
}
 
function EditConfig(val,i){
 
	var f=NetConfig;
	var index=tmpIndexs[i];
	NetConfig.policyNames.value=NetConfig.ISPType.value; 
	f.staticIp.value=sta_IPs[index];
	f.staticNetmask.value=sta_Masks[index];
	f.staticGateway.value=sta_GateWays[index];
	f.staticPriDns.value=sta_MainDnss[index];
	f.staticSecDns.value=sta_SecDnss[index]; 
	f.pppoeUser.value="";
	f.pppoePass.value="";
	if (ac_ip_flag == 1)
	{
		f.Ac_ip0.value=ac_ip0;
		f.Ac_ip1.value=ac_ip1;
		f.Ac_ip2.value=ac_ip2;
		if (man_dns == "")
		{
			f.ManDns.value="0.0.0.0";
		}
		else
		{
			f.ManDns.value=man_dns;
		}

		if (sec_dns == "")
		{
			f.SecDns.value="0.0.0.0";
		}
		else
		{
			f.SecDns.value=sec_dns;
		}
	}
	
	selVal(f.pppoeAuthType,"EITHER");
	selVal(f.pppoeOPMode,"auto");
	selVal(f.pppoeDailMode,"Normal");
	f.pppoeIdleTime.value= "0";
	f.MTU.value= "1480";
	f.MacAddr.value= mac[index];
 
	if(val==1){
	/*
		if((platform != "nv510v2") && (platform != "nv540")){
			f.PortName.selectedIndex = parseInt(PortNamesw[i].substring(3))-1;
		}else
		{
		*/
			f.PortName.selectedIndex = i;
		//}
		selVal(f.connectionType,ConnTypes[i]);
		selVal(f.ISPType,ISPTypes[index]);
		changeISPType(f.ISPType.value);
		var selIndex=f.connectionType.selectedIndex;
		if(selIndex==1){//固定连接
			f.staticIp.value=sta_IPs[index];
			f.staticNetmask.value=sta_Masks[index];
			f.staticGateway.value=sta_GateWays[index];
			f.staticPriDns.value=sta_MainDnss[index];
			f.staticSecDns.value=sta_SecDnss[index];
		}
		else if(selIndex==0){//动态连接
 
		}
		else{//pppoe连接
			f.pppoeUser.value=ppp_UserNames[index];
			f.pppoePass.value=ppp_Passwds[index];
			ppp_AuthTypess[i]=(ppp_AuthTypess[index]==""?"EITHER":ppp_AuthTypess[index]);
			selVal(f.pppoeAuthType,ppp_AuthTypess[index]);
			selVal(f.pppoeOPMode,ppp_pppoeOPModes[index]);
			selVal(f.pppoeDailMode,ppp_pppoeDailModes[index]);
			f.pppoeIdleTime.value=(ppp_pppoeIdleTimes[index]==""?"0":ppp_pppoeIdleTimes[index]);
			f.MTU.value=(ppp_MTUs[index]==""?"1492":ppp_MTUs[index]);
		}
	}
//	changeAdvanced(f.Advanced.value);
	changeConnType(ConnTypes[i]);
 
	//if(platform == "nv510W") {
		  
	if(NetConfig.PortName.value == slot3G){
			selVal(f.Dev3G,Dev3Gs);
			selVal(f.ISP3G,ISP3Gs);
			selVal(f.AuthType3G,AuthType3Gs);
			f.PinCode.value=PinCodes;
			f.AuthName.value=AuthNames;
			f.AuthPasswd.value=AuthPasswds;
			   
			g3tbl.style.display=dis;
	 
			var x2 = document.getElementById("Access");
			x2.style.display = "none";
			onProviderChange();
	}
	//} 
	changeAuth3g(f.AuthType3G.value);
	changeAdvance(f.PortName.value);
	NetConfig_List.Isp_Name.value=f.PortName.value;
	showOpButton(i);
	for(i=1;i<=9;i++){
		document.all["yutd"+i].innerHTML="";
	}
 
}

function changeAdvance(val){
	if(is3GPort(val,totalrecs))
	{
		advanceTable.style.display = "none";
	}else
	{	
		advanceTable.style.display = dis;
	}
	
}

function delISP(index){
    if(index != 0){
	if(confirm("<%write(wan_delISPTip);%>")){
		NetConfig_List.delstr.value=PortNames[index];
		NetConfig_List.action="/goform/formRoadDel";
	    NetConfig_List.submit();
	}
    } else {
        alert("<%write(wan_delISPAlert);%>");
    }
}
function changeConnType(val){
	var f=NetConfig;
	if(val=="STATIC"||val=="bridged"){
		britbl.style.display=dis;
		pppoetbl.style.display="none";
		g3tbl.style.display="none";
 
		  var x2 = document.getElementById("Access");
		  x2.style.display = dis;
	}
	else if(val=="PPPOE"){
		britbl.style.display="none";
		pppoetbl.style.display=dis;
		g3tbl.style.display="none";
 
		var x2 = document.getElementById("Access");
		x2.style.display = dis;
	}
	else{
		britbl.style.display="none";
		pppoetbl.style.display="none";
		g3tbl.style.display="none";
 
		var x2 = document.getElementById("Access");
		x2.style.display = dis;
	}
 
}
 
 
 
function onProviderChange()
{
   var f=NetConfig;
   for(i=14;i<=17;i++){
		document.all["yutd"+i].innerHTML="";
	}
   var providerId=f.ISP3G.selectedIndex;
   var providerVal=getListValue(f.ISP3G);
	 if(providerVal !=ISP3Gs){
	   f.ConnName.value=g3_para_arr[providerId][4];
	   f.DialNum.value=g3_para_arr[providerId][1];
	   f.PPPName.value=g3_para_arr[providerId][2];
	   f.PPPPasswd.value=g3_para_arr[providerId][3];
   }else{
   	 f.ConnName.value=ConnNames;
	 f.DialNum.value=DialNums;
	 f.PPPName.value=PPPNames;
	 f.PPPPasswd.value=PPPPasswds;
   }
}
 
function changeAuth3g(val){
	var f=NetConfig;

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
function changeISPType(val){
	if(val == "Either"){
			 var x1 = document.getElementById("href1");
			 x1.style.display = "none";
			 var x2 = document.getElementById("href2");
			 x2.style.display = "none";
			 var x3 = document.getElementById("href3");
			 x3.style.display = "none";
			 
	}
	else if(val == "10000"){
			var x1 = document.getElementById("href1");
			x1.style.display = "inline";
			var x2 = document.getElementById("href2");
			x2.style.display = "none";
			var x3 = document.getElementById("href3");
			x3.style.display = "none";
			NetConfig.policyNames.value ="10000";
	}
	else if(val == "10010"){
			var x1 = document.getElementById("href2");
			x1.style.display = "inline";
			var x2 = document.getElementById("href1");
			x2.style.display =  "none";
			var x3 = document.getElementById("href3");
			x3.style.display = "none";
		 	NetConfig.policyNames.value ="10010";
	}
	else if(val == "10086"){
			var x1 = document.getElementById("href3");
			x1.style.display = "inline";
			var x2 = document.getElementById("href1");
			x2.style.display =  "none";
			var x3 = document.getElementById("href2");
			x3.style.display = "none";
			NetConfig.policyNames.value ="10086";
	}
	
 
	
}
 
function hrefupCMC(){
	var f = NetConfig;
	var r=confirm("<%write(wan_upCMCTip);%>");
	if (r==true){
		NetConfig.policyNames.value = "10000";
		NetConfig.action="/goform/formUpISPRoute";
	    NetConfig.submit();
	  }
}
 
function hrefupCUC(){
	var f = NetConfig;
	var r=confirm("<%write(wan_upCUCTip);%>");
	if (r==true){
		NetConfig.policyNames.value = "10010";
		NetConfig.action="/goform/formUpISPRoute";
	    NetConfig.submit();
	  }
 
}
 
function hrefupMobile(){
	var f = NetConfig;
	var r=confirm("<%write(wan_upMobileTip);%>");
	if (r==true){
		NetConfig.policyNames.value = "10086";
		NetConfig.action="/goform/formUpISPRoute";
	    NetConfig.submit();
	  }
}

function checkURL(URL){
/*
    var strRegex = "^((https|http|ftp|rtsp|mms)?:\/\/)"  
		+ "?(([0-9a-z_!~*'().&=+$%-]+: )?[0-9a-z_!~*'().&=+$%-]+@)?" //ftp的user@  
		+ "(([0-9]{1,3}\.){3}[0-9]{1,3}" // IP形式的URL- 199.194.52.184  
        + "|" // 允许IP和DOMAIN（域名） 
        + "([0-9a-z_!~*'()-]+\.)*" // 域名- www.  
        + "([0-9a-z][0-9a-z-]{0,61})?[0-9a-z]\." // 二级域名  
        + "[a-z]{2,6})" // first level domain- .com or .museum  
        + "(:[0-9]{1,4})?" // 端口- :80  
        + "((/?)|" // a slash isn't required if there is no file name  
        + "(/[0-9a-z_!~*'().;?:@&=+$,%#-]+)+/?)$"; 
*/
//		var strRegex = /(http(s)?:\/\/)?([\w-]+\.)+[\w-]+(\/[\w- .\/?%&=]*)?/;
		var strRegex = /^([\w-]+\.)+((com)|(net)|(org)|(gov\.cn)|(info)|(cc)|(com\.cn)|(net\.cn)|(org\.cn)|(name)|(biz)|(tv)|(cn)|(mobi)|(name)|(sh)|(ac)|   (io)|(tw)|(com\.tw)|(hk)|(com\.hk)|(ws)|(travel)|(us)|(tm)|(la)|(me\.uk)|(org\.uk)|(ltd\.uk)|(plc\.uk)|(in)|(eu)|(it)|(jp))$/;
        var re=new RegExp(strRegex);
        if (re.test(URL)){
            return (true);
        }else{
            return (false);
        }
} 

function checkData(){
	var f=NetConfig;
	var check_flag = 0;
	var indextext=getListValue(f.connectionType);
	
	if ((indextext=="DHCP" )&& (ConnTypes[0] != "DHCP"))
	{
		f.web_flag.value = "1";
	}
	
	if (ac_ip_flag == 1)
	{
		if (f.Ac_ip0.value != "")
		{
			check_flag = 1;
			if (!checkURL(f.Ac_ip0.value))
			{
				check_flag = 0;
				if (!checkIP(f.Ac_ip0, "IP地址或域名", 0))
				{
					return false;
				}
				else
				{
					if (f.Ac_ip0.value == ipAddr)
					{
						alert("与本机IP冲突");
						Reset();
						return false;
					}
				}
			}
		}
		if (f.Ac_ip1.value != "")
		{
			check_flag = 1;
			if (!checkURL(f.Ac_ip1.value))
			{
				check_flag = 0;
				if (!checkIP(f.Ac_ip1, "IP地址或域名", 0))
				{
					return false;
				}
				else
				{
					if (f.Ac_ip1.value == ipAddr)
					{
						alert("与本机IP冲突");
						Reset();
						return false;
					}
				}
			}
		}
		if (f.Ac_ip2.value != "")
		{
			check_flag = 1;
			if (!checkURL(f.Ac_ip2.value))
			{
				check_flag = 0;
				if (!checkIP(f.Ac_ip2, "IP地址或域名", 0))
				{
					return false;
				}
				else
				{
					if (f.Ac_ip2.value == ipAddr)
					{
						alert("与本机IP冲突");
						Reset();
						return false;
					}
				}
			}
		}
		
		if ((check_flag == 1) || (f.ManDns.value != "0.0.0.0") || (f.SecDns.value != "0.0.0.0") )
		{
			if (!checkMainDns(f.ManDns,"<%write(com_MDNS);%>",0)) return false;
			if (!checkIP(f.SecDns,"<%write(com_SDNS);%>",1)) return false;
		}
		
		if (indextext != "STATIC")
		{
			return true;
		}
	}
	
	if(indextext=="STATIC"){
		if (f.staticIp.value=="0.0.0.0"){ alert("<%write(com_checkIPAlert);%>");return false};
		if (!checkIP(f.staticIp,"<%write(com_IP);%>",0)) return false;
		for(var i = 0; i< sta_IPs.length;i++)
		{
			if(i != f.PortName.selectedIndex && ConnTypes[i]=="STATIC")
			{
				if(f.staticIp.value == sta_IPs[i])
				{
					alert("WAN"+(f.PortName.selectedIndex + 1)+"<%write(wan_checkIPTip1);%>"+(i+1)+" <%write(wan_checkIPTip2);%>");
					return false;
				}	
			}
		}
		
		if (!checkMask(f.staticNetmask,"<%write(com_NetMask);%>",0)) return false;
		if (!checkIP(f.staticGateway,"<%write(com_GateWayAddr);%>",0)) return false;
		if (!checkSameSubnet(f.staticIp.value,f.staticGateway.value,f.staticNetmask.value))
		{
			alert("<%write(com_subnetAlert);%>");
			return false;
		}
		if(platform != "nv1800N")
		{
			if (!checkMainDns(f.staticPriDns,"<%write(com_MDNS);%>",0)) return false;
			if (!checkIP(f.staticSecDns,"<%write(com_SDNS);%>",1)) return false;
		}
	}
	else if(indextext=="PPPOE"){
		if(!checkName(f.pppoeUser,31,"<%write(com_user);%>",0)) return false;
		if(!checkPassword1(f.pppoePass, 31, 0, 0)) return false;	//检查密码：不能超过11个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
		if(! checkNumber(f.pppoeIdleTime,0,65535,"<%write(wan_freetime);%>",0)) return false;
		if(! checkNumber(f.MTU,1,1492,"MTU",0)) return false;
	}
	 
	 if(f.PortName.value == "5"){
		if (!checkName(f.PinCode,31,"<%write(wan_PIN);%>",1)) return false;
		if (!checkName(f.AuthName,31,"<%write(com_user);%>",1)) return false;
		if (!checkName(f.AuthPasswd,31,"<%write(com_passwd);%>",1)) return false;
		if (!checkName(f.ConnName,31,"<%write(wan_connName);%>",1)) return false;
		if (!checkName(f.DialNum,31,"<%write(wan_dial);%>",1)) return false;
		if (!checkName(f.PPPName,31,"<%write(com_user);%>",1)) return false;
		if (!checkName(f.PPPPasswd,31,"<%write(com_passwd);%>",1)) return false;
	}else
	{
	 	if(!checkMac(f.MacAddr,"<%write(com_MAC);%>",0)) return false;
	}
	 
		
       
	return true;
}
function save(){
	if(checkData()){
		var f=NetConfig;
		NetConfig.action="/goform/formWanIfConfig";
		NetConfig.submit();
	}
}
 
function Reset(){
	NetConfig.reset();
	init();
	for(i=1;i<=9;i++){
		document.all["yutd"+i].innerHTML="";
	}
}
function showOpButton(i){
	var str="";
	showbutton.innerHTML="";
 
	    str='<input type="button" value="<%write(com_delete);%>" class="input-button" onClick="delISP('+i+')">';
	
	if(ConnTypes[i]=="DHCP"){
		showbutton.innerHTML=str+'&nbsp;&nbsp;<input type="button" value="<%write(wan_update);%>" class="input-button" onClick="doUpdate('+i+')">&nbsp;&nbsp;<input type="button" value="<%write(wan_release);%>" class="input-button" onClick="doDisconnect('+i+')">';
	}
	else if((ConnTypes[i]=="PPPOE") ||(ConnTypes[i]=="3G")){
		showbutton.innerHTML=str+'&nbsp;&nbsp;<input type="button" value="<%write(wan_dial);%>" class="input-button" onClick="doDial('+i+')">&nbsp;&nbsp;<input type="button" value="<%write(com_hangup);%>" class="input-button" onClick="doDisconnect('+i+')">';
	}
	
	else
		showbutton.innerHTML=str;
}
 
function doDisconnect(i){
	if((NetConfig_List.Isp_Name.value!="") && (NetConfig_List.Isp_Name.value != NetConfig.PortName.value)) {
	    alert("<%write(wan_dialAlert);%>");
		return;
	}
	if(ConnStatuss[i]!="1"){
		alert("<%write(wan_dialTip);%>");
		return ;
	}
	else{
		NetConfig_List.action="/goform/formReleaseConnect";
		NetConfig_List.submit();
	}
 
}
function doUpdate(i){
	if((NetConfig_List.Isp_Name.value!="") && (NetConfig_List.Isp_Name.value != NetConfig.PortName.value)) {
		alert("<%write(wan_dialAlert);%>");
		return;
	}
	else{
		if(ConnTypes[i]=="DHCP")
			NetConfig_List.action="/goform/formReConnect";
		NetConfig_List.submit();
	}
}
function doDial(){
	if((NetConfig_List.Isp_Name.value!="") && (NetConfig_List.Isp_Name.value != NetConfig.PortName.value)) {
		alert("<%write(wan_dialAlert);%>");
		return;
	}
	NetConfig_List.action="/goform/formReConnect";
	NetConfig_List.submit();
}
function initDisplay()
{
	if(platform != "nv1800N")
	{
		moduleDisplay('grouplist',0);
		moduleDisplay('dotTr',0);
		moduleDisplay('ifTr',0);
		moduleDisplay('zhuDNS',0);
		moduleDisplay('beiDNS',0);
		frame11.location="_listhead1.htm";
		frame12.location="_list1.htm" ;
	}
	else
	{
		NetConfig.connectionType.options[2] = null;/*去掉pppoe接入方式*/
	}
	if (config == 0)	//下发
	{
		NetConfig.connectionType.disabled=true;
		NetConfig.staticIp.disabled=true;
		NetConfig.staticIp.value="";
		NetConfig.staticNetmask.disabled=true;
		NetConfig.staticNetmask.value="";
		NetConfig.staticGateway.disabled=true;
		NetConfig.staticGateway.value="";
		NetConfig.MacAddr.disabled=true;
		NetConfig.MacAddr.value="";
		if (ac_ip_flag == 0)
		{
			document.getElementById("uttsave").disabled=true;
			document.getElementById("uttreset").disabled=true;
			document.getElementById("ac_flag").style.display="none";
		}
		else
		{
			document.getElementById("uttsave").disabled=false;
			document.getElementById("uttreset").disabled=false;
			document.getElementById("ac_flag").style.display="inline";
		}
	}
}
function initPage()
{
	initDisplay();
	initTranslation();
}

</script>
</head>
<body onLoad="initPage()" onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript"> 
  if( platform=="nv1800N") showSecondTitle("<%write(com_LanConfig);%>");else showSecondTitle("<%write(com_WanConfig);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showright(errorstr,80);</script>
  <tr valign="top">
    <td><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
     <td align="center" class="style7" valign="top"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align="center">
	
	 <form name="NetConfig_List" method="post" action="/goform/">
   <table id=grouplist style="display:none"	border="0" align="center" cellpadding="0" cellspacing="0">
   <tr><td colspan="4" height="22">&nbsp;</td></tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe  name="frame11" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr bgcolor="c0ddff">
		      <td colspan="4">
			<iframe name="frame12" align="MIDDLE" width="600" height="126" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
		    <tr>
		      <td  colspan="4" valign="bottom"><table id=dellist  width="100%"	height="19" border="0" cellspacing="0" cellpadding="1">
			  <tr>
			  <td width="5"></td>
			  <td  class="style6"><div align="left">
				</div></td>
			  <td  class="style6" ><input type="hidden" name="delstr" size=10 ><input type="hidden" name="id"></td>
			  <td id=showbutton width="150" valign="top" align="right"></td><script language="javascript">//showOpButton();</script>
			  <td width="40" valign="top" align="right"><input type="button" value="<%write(com_fresh);%>" class="input-button" onClick="location.reload()" id="update"></td>
			    <input name="Isp_Name" type="hidden"  >
				<input name="Isp_Type" type="hidden"  >
			  </tr>
		      </table></td>
		    </tr>
		  </table>
  </form>
	</table>
	<table border="0" width="80%" align='center' cellspacing="0" cellpadding="0">
      <form  name="NetConfig" method="post">
        <tr >
          <td colspan="4" align="center" height="25" valign="top"><table width=600 border="0" cellspacing="0" cellpadding="0">
              <tr id="dotTr" style="display:none">
                <td background="<%write(com_imgdot);%>"><img src="<%write(com_imgspace);%>" width="8" height="1"></td>
              </tr>
          </table>              </td>
        </tr>       
        <tr><td colspan="4"><table width=600 border="0" cellspacing="0" cellpadding="0">
        <tr id="ifTr" style="display:none" height="22">
          <td align="right"  width="41%"  id="method"><%write(com_Interface);%></td>
          <td width="2%">&nbsp;</td>
          <td><select name="PortName" class="list"  onChange="EditConfig(1,this.selectedIndex)">
              <script language="javascript"> 
		
/*			 	if((platform == "nv510v2") || (platform == "nv540")){
              		 document.writeln(" <option value=\"1\" >WAN1<\/option>")
				}else
				{	 
					for(i=1;i<=totalrecs;i++) 
					document.write('<option value="'+i+'" >WAN'+i+'</option>');	
				}
		  
*/
			for (i = 1; i <= totalrecs; i++) {
				if (is3GPort(i,totalrecs)) {
					document.write('<option value="'+i+'" >3G</option>');
				} else if (isAPCliPort(i,totalrecs)) {
					document.write('<option value="'+i+'" >APClient</option>');
				} else {
					document.write('<option value="'+i+'" >WAN'+i+'</option>');
				}
			}		 
		      </script>
          </select></td>
          <td   width="27%">&nbsp;</td>
        </tr>
        <tr id="Access">
          <td align="right" width="41%" height="22"><%write(com_accessType);%></td>
          <td width="2%">&nbsp;</td>
          <td><select name="connectionType" class="list"  onChange="changeConnType(this.value)">
              <option value="DHCP" id="dhcp"><%write(com_DynamicIP);%></option>
              <option value="STATIC" id="static"><%write(com_StaticIP);%></option>
              <option value="PPPOE" id="pppoe"><%write(com_PPPoE);%></option>
          </select></td>
          <td width="27%">&nbsp;</td>
        </tr>
        <tr id="ispTr" style="display:none" height="22">
          <td align="right" ><%write(wan_operate);%></td>
          <td>&nbsp;</td>
          <td colspan="2"><select name="ISPType" class="list" onChange="changeISPType(this.value)">
              <option value="Either" selected id="either" ><%write(wan_either);%></option>
	  <option value="10000" id="cmc"><%write(wan_cmc);%></option>
              <option value="10010" id="cuc"><%write(wan_cuc);%></option>
              <option value="10086" id="mobile"><%write(wan_mobile);%></option>
            </select>
              <span id = "href1"><a href="javascript:hrefupCMC()"><%write(wan_upCMC);%></a></span><span id = "href2"><a href="javascript:hrefupCUC()"><%write(wan_upCUC);%></a></span><span id = "href3"><a href="javascript:hrefupMobile()"><%write(wan_upMobile);%></a></span>
            <input type="hidden" name="policyNames" size=10 >
            <input type="hidden" name="Progress">          </td>
        </tr>
        </table></td></tr>
        <tr style="display:" id="britbl">
          <td colspan="4"><table width="600" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td align="right"  height="22" width="41%" id="ipaddr" ><%write(com_IP);%></td>
                <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td><input type="text" name="staticIp" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),yutd1)"> </td>
                <td  id="yutd1" width="27%"></td>
              </tr>
              <tr>
                <td align="right" height="22" id="subnet"><%write(com_NetMask);%></td>
                <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                    <td><input type="text" name="staticNetmask" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>'),yutd2)"></td>
                <td  id="yutd2"></td>
              </tr>
              <tr>
                <td align="right"  height="22" id="gateway"><%write(com_GateWayAddr);%></td>
                <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td><input type="text" name="staticGateway" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_GateWayAddr);%>'),yutd3)" ></td>
                <td  id="yutd3"></td>
              </tr>
              <tr id="zhuDNS" style="display:none">
                <td align="right"  height="22" ><%write(com_MDNS);%></td>
                <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td><input type="text" name="staticPriDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMainDns(this,'<%write(com_MDNS);%>'),yutd4)" ></td>
                <td  id="yutd4"></td>
              </tr>
              <tr id="beiDNS" style="display:none">
                <td align="right"  height="22"><%write(com_SDNS);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="staticSecDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<%write(com_SDNS);%>'),yutd9)"  ></td>
                <td  id="yutd9"></td>
              </tr>
          </table></td>
        </tr>
		<tr style="display:" id="ac_flag">
          <td colspan="4"><table width="600" border="0" cellspacing="0" cellpadding="0">
				<tr>
                <td align="right"  height="22" width="41%" id="ip_ac"><%write(ac_addr);%>1</td>
                <td width="2%"></td>
                <td><input type="text" name="Ac_ip0" size="18"  onKeyPress="" maxlength="64" onChange="" >&nbsp;(IP或域名)</td>
                <td  id="yutd18"></td>
              </tr>
			  		<tr>
                <td align="right"  height="22" id="ip_ac"><%write(ac_addr);%>2</td>
                <td ></td>
                <td><input type="text" name="Ac_ip1" size="18"  onKeyPress="" maxlength="64" onChange="" ></td>
                <td  id="yutd19"></td>
              </tr>
			  		<tr>
                <td align="right"  height="22" id="ip_ac"><%write(ac_addr);%>3</td>
                <td ></td>
                <td><input type="text" name="Ac_ip2" size="18"  onKeyPress="" maxlength="64" onChange="" ></td>
                <td  id="yutd20"></td>
              </tr>
			  		  		<tr>
                <td align="right"  height="22" id="ip_ac"><%write(com_MDNS);%></td>
                <td ></td>
                <td><input type="text" name="ManDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMainDns(this,'<%write(com_MDNS);%>'),yutd21)" ></td>
                <td  id="yutd21"></td>
              </tr>
			  		  		<tr>
                <td align="right"  height="22" id="ip_ac"><%write(com_SDNS);%></td>
                <td ></td>
                <td><input type="text" name="SecDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<%write(com_SDNS);%>'),yutd22)" ></td>
                <td  id="yutd22"></td>
              </tr>
          </table></td>
        </tr>
        <tr style="display:none" id="pppoetbl">
          <td colspan="4" align="center"><table width="600" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td align="right"  height="22" width="41%" id="name"><%write(com_user);%></td>
                <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td ><input type="text" name="pppoeUser" size="18" maxlength="31" onChange="showfiledtip(re_checkName(this,'<%write(com_user);%>'),yutd5)"  ></td>
                <td  id="yutd5" width="27%"></td>
              </tr>
              <tr>
                <td align="right" height="22" id="password"><%write(com_passwd);%></td>
                <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td><input type="password" name="pppoePass" size="18"  maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd6)" ></td>
                <td  id="yutd6"></td>
              </tr>
              <tr>
                <td align="right" height="22" id="pdmethod"><%write(com_checkPasswd);%></td>
                <td >&nbsp;</td>
                <td><select name="pppoeAuthType">
                    <option value="NONE" >NONE</option>
                    <option value="PAP">PAP</option>
                    <option value="CHAP">CHAP</option>
                    <option value="EITHER" selected="selected">EITHER</option>
                </select></td>
              </tr>
              <tr>
                <td align="right" height="22" id="phonemode"><%write(wan_dialType);%></td>
                <td >&nbsp;</td>
                <td><select name="pppoeOPMode">
                    <option value="KEEPALIVE" id="auto"><%write(wan_auto);%></option>
                    <option value="DEMAND" id="demand"><%write(wan_demand);%></option>
                    <option value="MANUAL" id="menual"><%write(wan_menual);%></option>
                </select></td>
              </tr>
              <tr>
                <td align="right" height="22" id="phonemode"><%write(wan_dialMode);%></td>
                <td >&nbsp;</td>
                <td><select name="pppoeDailMode">
                    <option value="NORMAL" id=""><%write(wan_normal);%></option>
                    <option value="SP1" id=""><%write(wan_sp1);%></option>
                    <option value="SP2" id=""><%write(wan_sp2);%></option>
                    <option value="SP3" id=""><%write(wan_sp3);%></option>
                </select></td>
              </tr>
              <tr>
                <td align="right"  height="22" id="freetime"><%write(wan_freetime);%></td>
                <td><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td><input type="text" name="pppoeIdleTime" size="18" maxlength="5"  onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,'0','65535','<%write(wan_freetime);%>'),yutd7)" value="0">
                  &nbsp;<%write(wan_minute);%></td>
                <td  id="yutd7"></td>
              </tr>
              <tr>
                <td align="right"  height="22" >MTU</td>
                <input type="hidden" name="page" value="WANConfig">
                <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
                <td><input type="text" name="MTU" size="18" maxlength="4"	 onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,'1','1492','MTU'),yutd8)"	value="1480">
                  &nbsp;<%write(wan_byte);%></td>
                <td  id="yutd8"></td>
              </tr>
              <tr>
                <td align="right"  height="22" >&nbsp;</td>
                <td ></td>
                <td align="left"><%write(wan_MTUTip);%></td>
              </tr>
          </table></td>
        </tr>
        <!--//////////////////////////3G///////////////////////////////////////////-->
        <tr style="display:none" id="g3tbl" >
          <td colspan="4" align="center"  valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td align="right"  height="22" width="41%"><%write(wan_G3Type);%></td>
                <td width="2%">&nbsp;</td>
                <td ><select name="Dev3G">
                    <option value="HUAWEI-E169">HUAWEI E169</option>
                    <option value="HUAWEI-E1750">HUAWEI E1750</option>
                    <option value="HUAWEI-EC1260">HUAWEI EC1260</option>
                    <option value="HUAWEI-ET128" >HUAWEI ET128</option>
                    <option value="ZTE-MF637U" >ZTE MF637U</option>
                </select></td>
              </tr>
              <tr>
                <td align="right" height="22"><%write(wan_service);%></td>
                <td >&nbsp;</td>
                <td><select name="ISP3G" onChange="onProviderChange()">
                    <option value="CMC"><%write(wan_yidong);%></option>
                    <option value="CUC"><%write(wan_liantong);%></option>
                    <option value="CDC"><%write(wan_dianxin);%></option>
                </select></td>
              </tr>
              <tr>
                <td align="right" height="22"><%write(wan_authType);%></td>
                <td >&nbsp;</td>
                <td><select name="AuthType3G" onChange="changeAuth3g(this.value)">
                    <option value="SIM"><%write(wan_SIM);%></option>
                    <option value="PassAuth"><%write(wan_passAuth);%></option>
                </select></td>
              </tr>
              <tr id="simtd" style="display:none">
                <td align="right"  height="22"><%write(wan_PIN);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="PinCode" size="18" maxlength="31"  onChange="showfiledtip(re_checkName(this,'<%write(wan_PIN);%>'),yutd11)"  ></td>
                <td   width="27%" id="yutd11"></td>
              </tr>
              <tr id="pawdtd1" style="display:none">
                <td align="right"  height="22"><%write(com_user);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="AuthName" size="18" maxlength="31"  onChange="showfiledtip(re_checkName(this,'<%write(com_user);%>'),yutd12)"  ></td>
                <td  id="yutd12"></td>
              </tr>
              <tr id="pawdtd2" style="display:none">
                <td align="right"  height="22"><%write(com_passwd);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="AuthPasswd" size="18" maxlength="31"  onChange="showfiledtip(re_checkName(this,'<%write(com_passwd);%>'),yutd13)" ></td>
                <td  id="yutd13"></td>
              </tr>
              <tr>
                <td align="right" height="22"><%write(wan_connName);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="ConnName" size="18" maxlength="31"  onChange="showfiledtip(re_checkName(this,'<%write(wan_connName);%>'),yutd14)" value="CMNET"></td>
                <td  id="yutd14"></td>
              </tr>
              <tr>
                <td align="right"  height="22"><%write(wan_dial);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="DialNum" size="18" maxlength="31" onChange="showfiledtip(re_checkName(this,'<%write(wan_dial);%>'),yutd15)" value="*99***1#"></td>
                <td  id="yutd15"></td>
              </tr>
              <tr>
                <td align="right"  height="22" ><%write(wan_PPPConfig);%></td>
                <td ></td>
                <td></td>
              </tr>
              <tr>
                <td align="right"  height="22"><%write(com_user);%></td>
                <td >&nbsp;</td>
                <td><input type="text" name="PPPName" size="18" maxlength="31" onChange="showfiledtip(re_checkName(this,'<%write(com_user);%>'),yutd16)" value="CMNET" ></td>
                <td  id="yutd16"></td>
              </tr>
              <tr>
                <td align="right"  height="22"><%write(com_passwd);%></td>
                <td >&nbsp;</td>
                <td><input type="password" name="PPPPasswd" maxlength="31" size="18"  onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd17)" ></td>
                <td  id="yutd17"></td>
              </tr>
       <tr><td colspan="4"><table width="100%">
	       <tr  height="50">
<td class="helpfont"  align="left" id="zhuyi"><strong><%write(com_zhuyi);%></strong><%write(wan_G3Tip);%></td></tr>
		       </table></td></tr>
          </table></td>
        </tr >
        <tr id="advanceTable" style="display:none"><td colspan="4"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr>
        <td width="41%" height="30" align="right" ><div style="position:relative; z-index:2;"><a href="javascript:clickSenior(NetConfig_List, adv1)" style="color:#000000;text-decoration:underline;"><strong><%write(wan_advance);%></strong></a></div></td>
        <td width="2%" >&nbsp;</td>
        <td width="34%"><%write(wan_advanceTip);%></td>
        <td width="23%"><input type="hidden" name="senior1" id="senior1" value="true" /><input type="hidden" name="senior2" id="senior2" value="true" /><input type="hidden" name="senior3" id="senior3" value="true" /></td></tr>
<tr id="adv1" style="display:none">
        <td colspan="4" >
        <table width="100%" border="0" cellpadding="0" cellspacing="0" >
        <tr>
        <td width="41%" height="25" align="right"><%write(com_MAC);%></td>
        <td width="2%" height="25">&nbsp;</td>
        <td width="57%" height="25"><div align="left"> <input name="MacAddr" type="text" class="input" id="MacAddr" onKeyPress="macOnly()" value="" size="25"> </div></td> 
        </tr>
        </table>        </td>
     </tr>		
		</table></td></tr>
         
     <tr>
          <td colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
              <tr>
                <td	 height="40" align="center"  valign="middle">&nbsp;&nbsp; &nbsp;
                    <input name="button" type="button" class="input-button" id="uttsave" onClick="save()" value="<%write(com_save);%>"><input type="hidden" id="web_flag" name="web_flag" value="0">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" id="uttreset" onClick="Reset()" value="<%write(com_reset);%>">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_aphelppage); %>#fitApIfConf')">				  </td>
              </tr>
              <tr>
                <td colspan="3" height="22">&nbsp;</td>
              </tr>
          </table></td>
        </tr>
      </form>
	  </table></td>
  </tr>
</table>
  </td></tr><script language="javascript">init();</script>
</table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
