<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("tunnel");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//---------交互开始-------------
/*
var TunnelEnabled = 1;
var local_ip4_addr = 0;
var remote_ip4_addr = 0;

var remote_ip6_addr = "";

var errorstr="";
*/
<%aspOutTunnelInfo();%>
//----------交互结束---------------------

function init(){
	var f = TunnelConfig;

	f.tunnelEnabled.checked = (tunnelEnabled == 1)?true:false;
	f.local_ip4_addr.value = local_ip4_addr;
	f.remote_ip4_addr.value = remote_ip4_addr;
	f.remote_ip6_addr.value = remote_ip6_addr;
	
	}
	


function checkDatas(){
	var f = TunnelConfig;
        f.remote_ip6_addr.value = f.remote_ip6_addr.value.trim();	
	if(!checkIP(f.local_ip4_addr,"<%write(local_ipv4);%>",0)) return false;
	if(!checkIP(f.remote_ip4_addr,"<%write(remote_ipv4);%>",0)) return false;
        if((f.remote_ip6_addr.value !="") && (!checkIpv6(f.remote_ip6_addr,"<%write(remote_ipv6);%>",0))) return false;
        if(!checkIpv6Sp(f.remote_ip6_addr,"<%write(remote_ipv6);%>")) return false;
	return true;
	
}

function save(){
	if(checkDatas()){
		TunnelConfig.action = "/goform/formTunnel";
		TunnelConfig.submit();
		}
}

function Reset(){
	TunnelConfig.reset();
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
  <td><script language="javascript">showSecondTitle('<%write(tunnelConfig);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="TunnelConfig" method="post">
<table width="80%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" bordercolor="green" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	<tr>
	   <td align="right" width="38%" height="25"><%write(tunnel_start);%></td>
	   <td  width="2%">&nbsp;</td>
	   <td width="38%" ><input type="checkbox" name="tunnelEnabled"  id="tunnelEnabled"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(local_ipv4);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="local_ip4_addr" id="local_ip4_addr" type="text" size="16" maxlength="15" value="0.0.0.0" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(local_ipv4);%>'),yutd1)"></td><td id="yutd1"></td>
	</tr>
   
    <tr>
	   <td align="right" height="25"><%write(remote_ipv4);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="remote_ip4_addr" id="remote_ip4_addr" type="text" size="16" maxlength="15" value="0.0.0.0" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(remote_ipv4);%>'),yutd2)"></td><td id="yutd2"></td>
	</tr>
    
    <tr>
	   <td align="right" height="25"><%write(remote_ipv6);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="remote_ip6_addr" id="remote_ip6_addr" type="text"  size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(remote_ipv6);%>',0),yutd3)" ></td><td  id="yutd3"></td>
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
