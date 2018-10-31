<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("isatap");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//---------交互开始-------------
/*
var ISATAPEnabled = 1;
var ISATAP_IPv6Pre = "";
var ISATAP_IPv4Gat = 0;
var errorstr="";
*/
<%aspOutIsatapInfo();%>
//----------交互结束---------------------

function init(){
	var f = ISATAPConfig;

	f.ISATAPEnabled.checked = (ISATAPEnabled == 1)?true:false;
	f.ISATAP_IPv6Pre.value = ISATAP_IPv6Pre;
	f.ISATAP_IPv4Gat.value = ISATAP_IPv4Gat;
	
	
	}
	


function checkDatas(){
	var f = ISATAPConfig;
        f.ISATAP_IPv6Pre.value = f.ISATAP_IPv6Pre.value.trim();	
	if(!checkIpv6(f.ISATAP_IPv6Pre,"<%write(is_ipv6);%>",0)) return false;
	if(!checkIpv6Sp(f.ISATAP_IPv6Pre,"<%write(is_ipv6);%>")) return false;
	if(!checkIP(f.ISATAP_IPv4Gat,"<%write(is_ipv4);%>",0)) return false;
	return true;
	
}

function save(){
	if(checkDatas()){
		ISATAPConfig.action = "/goform/formIsatap";
		ISATAPConfig.submit();
		}
}

function Reset(){
	ISATAPConfig.reset();
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
  <td><script language="javascript">showSecondTitle('<%write(is_SerConfig);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="ISATAPConfig" method="post">
<table width="80%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" bordercolor="green" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	<tr>
	   <td align="right" width="38%" height="25"><%write(is_enabled);%></td>
	   <td  width="2%">&nbsp;</td>
	   <td width="40%" ><input type="checkbox" name="ISATAPEnabled"  id="ISATAPEnabled"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(is_ipv6);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ISATAP_IPv6Pre" id="ISATAP_IPv6Pre" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(is_ipv6);%>',0),yutd1)"></td><td id="yutd1"></td>

	</tr>
   
    <tr>
	   <td align="right" height="25"><%write(is_ipv4);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ISATAP_IPv4Gat" id="ISATAP_IPv4Gat" type="text" size="16" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(is_ipv4);%>'),yutd2)"></td><td id="yutd2"></td>
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
