<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("DnsProxy");%>

<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
/*
<%
	//aspDhcpServerConfig();
	write('var dnspEnbls = "Enable";\n');	//Enable为启用
	write('var ISPDnss = "8.8.8.8";\n');
	write('var ISPDnss2 = "9.9.9.9";\n');
	write('var errorstr = "";\n');
%> 
*/

<%
	aspOutPutDnsAgency();
%>


function save(){
	var f=DhcpServer_Config;
	if(checkData()){
		//DhcpServer_Config.lanDhcpType.innerHTML=(DhcpServer_Config.lanDhcpTypew.checked==true?"SERVER":"DISABLE");
		DhcpServer_Config.dnspEnbl.value=(DhcpServer_Config.dnspEnblw.checked==true?"on":"off");
//		alert(f.ISPDns.value);
//		alert(f.ISPDns2.value);
//		return;
		DhcpServer_Config.action="/goform/formDnsAgency";
		DhcpServer_Config.submit();
	}
}

function checkData(){
	var f=DhcpServer_Config;
	/*
	if (!checkIP(f.dhcpStart,"起始IP地址",0)) return false;
	if (!checkIP(f.dhcpEnd,"结束IP地址",0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
		alert(_("warning5"));
		f.dhcpStart.focus();
		return false;
	}
	if (f.dhcpEnd.value=="0.0.0.0")
	{
		alert(_("warning6"));
		f.dhcpEnd.focus();
		return false;
	}
	if (compareIP(f.dhcpStart.value,f.dhcpEnd.value)==1)
	   {
			alert("起始地址不能大于结束地址。");
			return false;
	   }   
	if (!checkMask(f.dhcpMask,"子网掩码",0)) return false;
	if (!checkIP(f.dhcpGateway,"网关地址",0)) return false;
	if (!checkNumber(f.dhcpLease,1,864000,"租用时间",0)) return false;
	if (!checkMainDns(f.dhcpPriDns,"主DNS服务器",0)) return false;
	if (!checkIP(f.dhcpSecDns,"备DNS服务器",1)) return false;
	*/
	if (!checkIP(f.ISPDns,_("<%write(ISPDNSSvc1);%>"),1)) return false;
	if (!checkIP(f.ISPDns2,_("<%write(ISPDNSSvc2);%>"),1)) return false;
	return true;
}
function init(){
	var f=DhcpServer_Config;
	/*
	f.lanDhcpTypew.checked=(lanDhcpTypes=="Enable"?true:false);
	f.dhcpStart.value=dhcpStarts;
	f.dhcpEnd.value=dhcpEnds;
	f.dhcpMask.value=dhcpMasks;
	f.dhcpGateway.value=dhcpGateways;
	f.dhcpLease.innerHTML=dhcpLeases;
	f.dhcpPriDns.value=dhcpPriDnss;
	f.dhcpSecDns.value=dhcpSecDnss;
	*/
	f.dnspEnblw.checked=(dnspEnbls=="Enable"?true:false);
	f.ISPDns.value=ISPDnss;
	f.ISPDns2.value=ISPDnss2;
/*	
	 for(i=1;i<=9;i++){
	 	document.all["yutd"+i].innerHTML="";
	 }
*/	 
}
/*
function initDisplay()
{
	var obj=null;
	if(platform != "nv1800N")
	{
		obj=document.getElementsByName('dnsAgTr');
		for(var i =0;i<obj.length;i++)
		{
			obj[i].style.display=dis;
		}
	}
}
function initPage()
{
	initDisplay();
	initTranslation();
}
*/
</script>
</head>
<body onLoad="init();" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(DHCP);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" background="images/table_main.png">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form action=""  name="DhcpServer_Config" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						4,
						new Array(_("<%write(dhcppool);%>"),'DhcpPoolForAc.asp',1,0),
//						new Array(_("<%write(dhcpSvcSet);%>"),'DhcpServerForAc.asp',0,0),
						new Array(_("<%write(staticDhcp);%>"),'DhcpFixForAc.asp',1,0),
						new Array(_("<%write(dhcpClient);%>"),'DhcpClientForAc.asp',1,0),
						new Array(_("<%write(dnsSet);%>"),'DnsProxyForAc.asp',1,1)
		);
		showThreeMenu();
	</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" height="200">
  <tr>
    <td align="center" class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
		   <tr  name="dnsAgTr" id="dnsAgTr" style="">
		     <td width="33%" height="25"  align="right" id="DNSset"><%write(startDns);%></td>
		     <td width="3%" >&nbsp;</td>
		     <td height="20"><input name="dnspEnblw" type="checkbox"><input type="hidden" name="dnspEnbl"></td>
		     </tr>
		  <tr name="dnsAgTr" id="dnsAgTr" style=""><td></td><td></td><td colspan="2" class="helpfont" id="dns shuoming"><%write(show);%></td>
		  </tr>
		  <tr name="dnsAgTr" id="dnsAgTr" style="">
		<td align="right" id="ISPDns_td1"><%write(ISPDNSSvc1);%></td>
		<td ></td>
		<td height="20"><input name="ISPDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(ISPDNSSvc1);%>'),yutd8)" maxlength="15"></td><td id="yutd8"></td></tr>
			  <tr name="dnsAgTr" id="dnsAgTr" style="">
		<td align="right" id="ISPDns_td2"><%write(ISPDNSSvc2);%></td>
		<td ></td>
		<td height="20"><input name="ISPDns2" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(ISPDNSSvc2);%>'),yutd9)" maxlength="15"></td><td id="yutd9"></td></tr>
		<tr><td></td><td></td><tr><td colspan="4" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DNSProxyForAc')" id="utthelp"></td>
	
  </tr>
</table>

  </td></tr>
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

