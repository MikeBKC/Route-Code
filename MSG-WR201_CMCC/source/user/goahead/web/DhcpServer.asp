<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DHCP"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
<%getSysLang();%>
<%aspDhcpServerConfig();%> 

function save(){
	var f=DhcpServer_Config;
	if(checkData()){
		DhcpServer_Config.lanDhcpType.value=(DhcpServer_Config.lanDhcpTypew.checked==true?"SERVER":"DISABLE");
		DhcpServer_Config.dnspEnbl.value=(DhcpServer_Config.dnspEnblw.checked==true?"on":"off");
		DhcpServer_Config.action="/goform/formDhcpServerConfig";
		DhcpServer_Config.submit();
	}
}

function checkData(){
	var f=DhcpServer_Config;
	if (!checkIP(f.dhcpStart,"<% write(com_startIP); %>",0)) return false;
	if (!checkIP(f.dhcpEnd,"<% write(com_endIP); %>",0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
		alert("<% write(com_warning5); %>");
		f.dhcpStart.focus();
		return false;
	}
	if (f.dhcpEnd.value=="0.0.0.0")
	{
		alert("<% write(com_warning6); %>");
		f.dhcpEnd.focus();
		return false;
	}
	if (compareIP(f.dhcpStart.value,f.dhcpEnd.value)==1)
	   {
			alert("<% write(StartIPGTEndErr); %>");
			return false;
	   }
        if ((compareIP(f.dhcpStart.value,f.dhcpGateway.value) != 1) 
	    && (compareIP(f.dhcpGateway.value,f.dhcpEnd.value) != 1)){
	    alert("网关地址不可包含在地址池中");
	    return false;
	}  
	for(var i=0;i<number;i++)  
	{ 
	    if ((compareIP(f.dhcpStart.value,FixIPs[i]) == 1) 
		    || (compareIP(FixIPs[i],f.dhcpEnd.value) == 1)){
		alert("该操作将导致静态绑定的IP不包含在地址池中，请修改静态绑定后重新保存");
		return false;
	    }  
	}
	if (!checkMask(f.dhcpMask,"<% write(com_NetMask); %>",0)) return false;
	if (!checkIP(f.dhcpGateway,"<% write(com_GateWayAddr); %>",0)) return false;
	if (!checkNumber(f.dhcpLease,600,86400,"<% write(ReimbursedTime); %>",0)) return false;
	if (!checkMainDns(f.dhcpPriDns,"<% write(com_MDNS); %>",0)) return false;
	if (!checkIP(f.dhcpSecDns,"<% write(com_SDNS); %>",1)) return false;
	if (!checkIP(f.ISPDns,"<% write(OperDNSServer1); %>",1)) return false;
	if (!checkIP(f.ISPDns2,"<% write(OperDNSServer2); %>",1)) return false;
	
	if (dhcpOption43Types != -1)
	{
	    if (f.option43Type.value == 1) {
		if (!checkIP(f.option43Addr, "<% write(com_optin43Addr); %>", 1)) return false;
	    } else if (f.option43Type.value == 2) {
		if (!checkEnglishName(f.option43Addr, 63, "<% write(com_optin43Addr); %>", 1)) return false;
	    } else if (f.option43Type.value == 3) {
		if (!checkEnglishName(f.option43Addr, 127, "<% write(com_optin43Addr); %>", 1)) return false;
	    }
	}
	return true;
}
function init(){
	var f=DhcpServer_Config;
	f.lanDhcpTypew.checked=(lanDhcpTypes=="Enable"?true:false);
	f.dhcpStart.value=dhcpStarts;
	f.dhcpEnd.value=dhcpEnds;
	f.dhcpMask.value=dhcpMasks;
	f.dhcpGateway.value=dhcpGateways;
	f.dhcpLease.value=dhcpLeases;
	f.dnspEnblw.checked=(dnspEnbls=="Enable"?true:false);
	f.dhcpPriDns.value=dhcpPriDnss;
	f.dhcpSecDns.value=dhcpSecDnss;
	f.ISPDns.value=ISPDnss;
	f.ISPDns2.value=ISPDnss2;
	 for(i=1;i<=9;i++){
	 	document.all["yutd"+i].innerHTML="";
	 }
	if (dhcpOption43Types == -1)
	{
	    option43Type_tr.style.display="none";
	    option43Addr_tr.style.display="none";
	    option43Show_tr.style.display="none";
	}
	else
	{
	    f.option43Type.value = dhcpOption43Types;
	    f.option43Addr.value = dhcpOption43Addrs;
	    if (dhcpOption43Types == 0)
		f.option43Addr.disabled = true;
	}
}
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
}
function changOption43Type()
{
	var f=DhcpServer_Config;
	if (f.option43Type.value == 0) 
	    f.option43Addr.disabled = true;
	else
	    f.option43Addr.disabled = false;
	if (f.option43Type.value == 3)
	    alert("<% write(com_option43Warning); %>");
}
</script>
</head>
<body onLoad="initPage();" onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(DHCP); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" background="<% write(com_imgtablemain); %>">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form action=""  name="DhcpServer_Config" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						4,
						new Array("<% write(DHCPset); %>",'DhcpServer.asp',1,1),
						new Array("<% write(StaticDHCP); %>",'DhcpFix.asp',1,0),
						new Array("<% write(DHCPAutoBind); %>",'DhcpAutoBind.asp',1,0),
						new Array("<% write(DHCPClient); %>",'DhcpClient.asp',1,0)
		);
		showThreeMenu();
	</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" height="250">
  <tr>
    <td align="center" class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr> 
    <tr>
		<td width="33%" align="right" height="25" id="dhcpset"><% write(SUseDHCPServer); %></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="lanDhcpTypew" type="checkbox" ><input type="hidden" name="lanDhcpType"></td>
	      </tr>
		  <tr><td></td><td></td><td colspan="2" class="helpfont" id="dhcp shuoming"><% write(ExplainHelp1); %></td></tr>
		  <tr>
		<td align="right" id="startip"><% write(com_startIP); %></td>
		<td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="dhcpStart" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<% write(com_startIP); %>'),yutd1)" maxlength="15"></td><td id="yutd1"></td>
	      </tr>
		   
		  <tr>
		<td  align="right" id="endip"><% write(com_endIP); %></td>
		<td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="dhcpEnd" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<% write(com_endIP); %>'),yutd2)" maxlength="15"></td><td id="yutd2"></td>
	      </tr>
		  <tr>
		<td align="right" id="subnet"><% write(com_NetMask); %></td>
		<td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="dhcpMask" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMask(this,'<% write(com_NetMask); %>'),yutd7)" maxlength="15"></td><td id="yutd7"></td>
	      </tr>
         <tr>
		<td  align="right" id="gateway"><% write(com_GateWayAddr); %></td>
		<td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="dhcpGateway" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<% write(com_GateWayAddr); %>'),yutd3)" maxlength="15"></td><td id="yutd3"></td>
	      </tr>
          <tr>
		<td  align="right" id="usetime"><% write(ReimbursedTime); %></td>
		<td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="dhcpLease" type="text" size="18" onKeyPress="figureOnly()" maxlength="6" onChange="showfiledtip(re_checkNumber(this,600,86400,'<% write(ReimbursedTime); %>'),yutd4)">&nbsp;&nbsp;
		<script language="javascript">document.write("<% write(DhcpSecond); %>");</script>
		</td><td id="yutd4"></td>
	      </tr>
		  <tr>
		<td align="right" id="zhuDNS"><% write(com_MDNS); %></td>
		<td ><img src="<% write(com_imgqt); %>" width="7" height="9"></td>
		<td height="20"><input name="dhcpPriDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMainDns(this,'<% write(com_MDNS); %>'),yutd5)" maxlength="15"></td><td id="yutd5"></td>
	      </tr>
		  <tr>
		<td  align="right" id="beiDNS"><% write(com_SDNS); %></td>
		<td>&nbsp;</td>
		<td height="20"><input name="dhcpSecDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<% write(com_SDNS); %>'),yutd6)" maxlength="15"></td><td id="yutd6"></td>
		

	      <tr id="option43Type_tr" style="display">
		<td align="right" height="25"><% write(com_option43); %></td><td >&nbsp;</td>
		<td><select name="option43Type" id="option43Type" style="width:100px" onChange="changOption43Type()">
		    <option value="0"><% write(com_option43disable); %></option>
		    <option value="1"><% write(com_option43hex); %></option>
		    <option value="2"><% write(com_option43ascii); %></option>
		    <option value="3"><% write(com_option43custom); %></option></select></td>
	      </tr>
	      <tr id="option43Addr_tr" style="display">
		<td align="right" height="25"><% write(com_optin43Addr); %></td><td >&nbsp;</td>
		<td><input name="option43Addr" id="option43Addr" type="text" size="18"></td>
	      </tr>
	      <tr id="option43Show_tr" style="display">
		<td align="right" height="25"><% write(com_option43TLV); %></td><td >&nbsp;</td>
		<td><%aspOutputDhcpOption43Show();%></td>
	      </tr>


	      </tr>	  
		  <tr name="dnsAgTr" id="dnsAgTr" style="display:none"><td colspan="4" align="center" height="40"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
   <td background="<% write(com_imgdot); %>"><img src="<% write(com_imgspace); %>" width="8" height="1"></td>
  </tr>
</table>
</td></tr>
		   <tr  name="dnsAgTr" id="dnsAgTr" style="display:none">
		  <td  align="right" height="25" id="DNSset"><% write(SUseDNSAgency); %></td>
		<td >&nbsp;</td>
		<td height="20"><input name="dnspEnblw" type="checkbox"><input type="hidden" name="dnspEnbl"></td>
	      </tr>
		  <tr name="dnsAgTr" id="dnsAgTr" style="display:none"><td></td><td></td><td colspan="2" class="helpfont" id="dns shuoming"><% write(ExplainHelp2); %></td>
		  </tr>
		  <tr name="dnsAgTr" id="dnsAgTr" style="display:none">
		<td align="right"><% write(OperDNSServer1); %></td>
		<td ></td>
		<td height="20"><input name="ISPDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<% write(OperDNSServer1); %>'),yutd8)" maxlength="15"></td><td id="yutd8"></td></tr>
			  <tr name="dnsAgTr" id="dnsAgTr" style="display:none">
		<td align="right"><% write(OperDNSServer2); %></td>
		<td ></td>
		<td height="20"><input name="ISPDns2" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<% write(OperDNSServer2); %>'),yutd9)" maxlength="15"></td><td id="yutd9"></td></tr>
		<tr><td></td><td></td><tr><td colspan="4" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DhcpServers')"></td>
	
  </tr>
</table>

  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

