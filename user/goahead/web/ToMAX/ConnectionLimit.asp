<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("langChange"); %>
<% langTran("common"); %>
<% langTran("ConnectLimit"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>
var strSecondMenuFocus="<% write(ConnectLimit); %>";

var errorstr = "";
<% aspConnLimitData(); %>
function enableCLK() {
	var f = ConnLimitConfig;
	if (f.enable.checked) {
		f.totalCnt.disabled = false;
		f.tcp.disabled = false;
		f.udp.disabled = false;
		f.icmp.disabled = false;
	} else {
		f.totalCnt.disabled = true;
		f.tcp.disabled = true;
		f.udp.disabled = true;
		f.icmp.disabled = true;		
	}
}

function checkData(){
	var f=ConnLimitConfig;
	if (!f.enable.checked) {
		return true;
	}
	var MaxSession = eval(f.totalCnt.value);
	var TCPSession = eval(f.tcp.value);
	var UDPSession = eval(f.udp.value);
	var ICMPSession = eval(f.icmp.value);

	if ((MaxSession != 0) && (TCPSession > MaxSession || UDPSession > MaxSession
		|| ICMPSession > MaxSession)) {
		alert("<% write(ConnectConfErr); %>");
		return;
	}
	if(!checkNumber(f.totalCnt,0, maxSess,"<% write(TotleConCount); %>", 0)) return false;
	if(!checkNumber(f.tcp,0, maxSess,"<% write(TCPConCount); %>", 0)) return false;
	if(!checkNumber(f.udp,0, maxSess,"<% write(UDPConCount); %>", 0)) return false;
	if(!checkNumber(f.icmp,0, maxSess,"<% write(ICMPConCount); %>", 0)) return false;

	return true;
}
function save(){
	if(checkData()){
		var f=ConnLimitConfig;
		f.action="/goform/formConnLimit";
		f.submit();
	}	
}
function init(){
	var f = ConnLimitConfig;
	f.enable.checked = (connLimitSw == "on") ? true : false;
	f.totalCnt.value = totalLimit;
	f.tcp.value = tcpLimit;
	f.udp.value = udpLimit;
	f.icmp.value = icmpLimit;
	enableCLK();
}
function Reset(){
	ConnLimitConfig.reset();
	init();	
}

//-->
</script>
</head>
<body onbeforeunload="logOut()" >
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td> 
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(ConnectLimit); %>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	 
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="style7" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="ConnLimitConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >
  <table border="0" width="600" align='center'>
     
	   <tr>
	   <td width="37%" height="25" align="right"><% write(SUseConLimit); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="55%"><input type="checkbox" name="enable" id="enable" onClick="enableCLK()"><input type="hidden" name="tmp" value="1"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><% write(TotleConCount); %></td>
	   <td >&nbsp;</td>
	   <td><input name="totalCnt" id="totalCnt" type="text" size="18" maxlength="15" onKeyPress="figureOnly()"></td>
	   </tr><tr>
	   <td align="right" height="25"><% write(TCPConCount); %></td>
	   <td >&nbsp;</td>
	   <td><input name="tcp" id="tcp" type="text" size="18" maxlength="15" onKeyPress="figureOnly()"></td>
	   </tr><tr>
	   <td align="right" height="25"><% write(UDPConCount); %></td>
	   <td >&nbsp;</td>
	   <td><input name="udp" id="udp" type="text" size="18" maxlength="15" onKeyPress="figureOnly()"></td>
	   </tr><tr>
	   <td align="right" height="25"><% write(ICMPConCount); %></td>
	   <td >&nbsp;</td>
	   <td><input name="icmp" id="icmp" type="text" size="18" maxlength="15" onKeyPress="figureOnly()"></td>
	   </tr>
	   <tr><td colspan="4"><table width="100%">
	   <tr  height="30"> 
	   <td class="helpfont"  align="center" id="zhuyi"><strong><% write(HelpMessage1); %></strong><% write(HelpMessage2); %></td></tr> 
	   </table></td></tr>
	   
	  <tr><td  colspan="4">
  <table width="29%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  width="62%" height="30" align="center" valign="middle"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#ConnectionLimit')"></td>
  </tr>
</table>
  </td>
	
  </tr>
  <tr><td colspan="4" align="center" height="30">&nbsp;</td></tr>

  </table></td>
  </tr>

  <script language="javascript">init();</script>
</form>

</table>
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table></td></tr></table>
</body>
</html> 
