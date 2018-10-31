<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("netMode");%>
<%langTran("commonWords");%>
<%langTran("common");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%
	aspOutPutNetModeSwitch();
%>

function init()
{
	if (netMode == 1)
	{
		document.getElementById('netMode').value = 1;

	}
	else
	{
		document.getElementById('netMode').value = 0;
	}
}
function save()
{
	netMode_switch.submit();
}
</script>
</head>
<body onbeforeunload="logOut()"  style="background:url(images/body_main.png);  background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(netMode);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/formNetModeSwitch" name="netMode_switch" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td width="102" height="20">&nbsp;</td></tr>
	   <tr>
	     <td colspan="2"><table width="100%"><tr>
	       <td width="48%" align="right" height="25" id="netMode_td"><%write(netMode);%></td>
	       <td width="3%">&nbsp;</td>
	       <td width="49%"><select name="netMode" id="netMode" class="list">
	         <option value="0" id="gateway"><%write(gateway);%></option>
	         <option value="1" id="bridge"><%write(bridge);%></option>
	         </select></td>
	       </tr>
	         <tr>
	           <td height="25" colspan="3" align="center" class="helpfont"><%write(rebootTips);%></td>
	           </tr>
	     </table></td>
	     </tr>
	   <tr><td height="20">&nbsp;</td></tr>
<tr><td colspan="2"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<%write(refill);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#netMode')"></td>
  </tr>
</table></td></tr>
<input type="hidden"  name="wrlesMode_chanWid"><input type="hidden" name="wds_list">
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
