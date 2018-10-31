<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("DHCPv6");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//---------交互开始-------------
/*
var dhcpEnabled = 1;
var sta_ipv6_addr = "";
var end_ipv6_addr = "";
var dhcpPrefixLength = 64;
var ipv6dhcpGateway = "";
var staleTime = 3600;
var ipv6DnsZ = "";
var ipv6DnsB = "";


var errorstr="";
*/
<%aspOutDhcp6Info(); %>
//----------交互结束---------------------

function init(){
	var f = DHCPv6Config;

	f.dhcpEnabled.checked = (dhcpEnabled == 1)?true:false;
	f.sta_ipv6_addr.value = sta_ipv6_addr;
	f.end_ipv6_addr.value = end_ipv6_addr;
	f.dhcpPrefixLength.value = dhcpPrefixLength;
//	f.ipv6dhcpGateway.value = ipv6dhcpGateway;
	f.staleTime.value = staleTime;
	f.ipv6DnsZ.value = ipv6DnsZ;
	f.ipv6DnsB.value = ipv6DnsB;
	}
	


function checkDatas(){
	var f = DHCPv6Config;
        f.sta_ipv6_addr.value = f.sta_ipv6_addr.value.trim();
        f.end_ipv6_addr.value = f.end_ipv6_addr.value.trim();
//        f.ipv6dhcpGateway.value = f.ipv6dhcpGateway.value.trim();
        f.ipv6DnsZ.value = f.ipv6DnsZ.value.trim();
        f.ipv6DnsB.value = f.ipv6DnsB.value.trim();
	if (!checkNumber(f.dhcpPrefixLength,1,127,"<%write(ipv6_PreLen);%>",1)) return false;
	if (!checkNumber(f.staleTime,60,2147483647,"<%write(sta_time);%>",1)) return false;
        if (!checkIpv6(f.sta_ipv6_addr,"<%write(ipv6_addr);%>",0)) return false;	
        if (!checkIpv6(f.end_ipv6_addr,"<%write(ipv6_addr);%>",0)) return false;	
//        if (!checkIpv6(f.ipv6dhcpGateway,"IPv6网关")) return false;	
        if (!checkIpv6(f.ipv6DnsZ,"<%write(ipv6_dns);%>",0)) return false;	
        if (!checkIpv6(f.ipv6DnsB,"<%write(ipv6_dns);%>",0)) return false;	
         if (!checkIpv6Sp(f.sta_ipv6_addr,"<%write(ipv6_addr);%>")) return false;	
        if (!checkIpv6Sp(f.end_ipv6_addr,"<%write(ipv6_addr);%>")) return false;	
//        if (!checkIpv6Sp(f.ipv6dhcpGateway,"IPv6网关")) return false;	
        if (!checkIpv6Sp(f.ipv6DnsZ,"<%write(ipv6_dns);%>")) return false;	
        if (!checkIpv6Sp(f.ipv6DnsB,"<%write(ipv6_dns);%>")) return false;	
       if (!checkIpv6Subnet(f.sta_ipv6_addr,f.end_ipv6_addr,f.dhcpPrefixLength)){
	    alert("<%write(alert_one);%>");
	    return false;
	}
//	if (!checkIpv6Subnet(f.sta_ipv6_addr,f.ipv6dhcpGateway,f.dhcpPrefixLength)){
//	    alert("IP地址和网关不在同一网段");
//	    return false;
//	}
        if(compareIPv6(f.sta_ipv6_addr,f.end_ipv6_addr) == -1){
	    alert("<%write(alert_two);%>");
	    return false;
	}
//        if((compareIPv6(f.end_ipv6_addr,f.ipv6dhcpGateway) != 1) && (compareIPv6(f.ipv6dhcpGateway,f.sta_ipv6_addr) != 1)){
//            alert("网关地址不能包含在地址池中");
//            return false;
//	}
	return true;

	
}

function save(){
	if(checkDatas()){
		DHCPv6Config.action = "/goform/formDhcp6";
		DHCPv6Config.submit();
		}
}

function Reset(){
	DHCPv6Config.reset();
	init();
 
	}

</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(dhcpv6);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="DHCPv6Config" method="post">
<table width="80%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" bordercolor="green" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	<tr>
	   <td align="right" width="35%" height="25"><%write(en_dhcp);%></td>
	   <td  width="2%">&nbsp;</td>
	   <td width="48%" ><input type="checkbox" name="dhcpEnabled"  id="dhcpEnabled"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(sta_ipv6);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="sta_ipv6_addr" id="sta_ipv6_addr" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(sta_ipv6);%>',0),sttd)"></td><td  id="sttd"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(end_ipv6);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="end_ipv6_addr" id="end_ipv6_addr" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(end_ipv6);%>',0),entd)"></td><td  id="entd"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(ipv6_PreLen);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="dhcpPrefixLength" id="dhcpPrefixLength" type="text" size="10" maxlength="63" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,0,127,'<%write(ipv6_PreLen);%>'),prtd)" ></td><td  id="prtd"></td>
	</tr>
  <!--  <tr>
	   <td align="right" height="25">IPv6网关地址</td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6dhcpGateway" id="ipv6dhcpGateway" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'IPv6网关地址'),gwtd)"></td><td  id="gwtd"></td>
	</tr>-->
    <tr>
	   <td align="right" height="25"><%write(sta_time);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="staleTime" id="staleTime" type="text" size="34" maxlength="11" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,60,2147483647,'<%write(sta_time);%>'),titd)"></td><td  id="titd"></td> 
	</tr>
	<tr>
	   <td align="right" height="25"><%write(main_dns);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6DnsZ" id="ipv6DnsZ" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(main_dns);%>',0),d1td)"></td><td  id="d1td"></td>
	</tr>
    <tr>
	   <td align="right" height="25"><%write(bei_dns);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6DnsB" id="ipv6DnsB" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(bei_dns);%>',0),d2td)"></td><td  id="d2td"></td>
	</tr>
	
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	    <td height="30" align="center" valign="top">
	    <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Syslog')">
	    </td>
	   </tr>
	</table>
	</td></tr>
    </table>
    </td></tr>
</table>
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
