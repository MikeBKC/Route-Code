<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("IdentifyBind"); %>
<% langTran("left"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%aspOutIdentifyBind();%>
function save()
{
    var f=IdentifyBind_Config;
    f.IdentifyBindEnable.value=(f.IdentifyBindEnablew.checked==true?"on":"off"); 
    IdentifyBind_Config.action ="/goform/formIdentifyBind";
    IdentifyBind_Config.submit();
}
function init()
{
    var f=IdentifyBind_Config;
    f.IdentifyBindEnablew.checked=(IdentifyBindEnables.toLowerCase()=="1"?true:false);
}
</script>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("<% write(com_IdentifyBind); %>");</script>
<table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center" background="<% write(com_imgtablemain); %>">
<script language="javascript">showerror(errorstr, 80);</script>
<tr valign="top">
<td><form action=""  name="IdentifyBind_Config" method="post" >
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"80%",
	4,
	new Array("<%write(com_globalConfig);%>",'DoubleRoad.asp',1,0),
	new Array("<%write(com_routeCombinationStatus);%>",'DoubleRoadInfo.asp',1,0),
	new Array("<% write(com_lineCombinationCheck); %>",'DoubleRoadInfo_edit.asp',1,0),
	new Array("<% write(com_IdentifyBind); %>",'IdentifyBind.asp',1,1)
	);
showThreeMenu();
</script>

<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" height="150">
<tr>
<td align="center"  class="three_td" valign="top">
<table border="0" width="600" align='center'>
<tr><td colspan="3" height="30">&nbsp;</td></tr>
<tr>
<td width="45%" align="right" id="IdentifyBindEn"><% write(IdentifyBindEn); %></td>
<td width="2%"></td>
<td><input  type="checkbox" name="IdentifyBindEnablew"><input type="hidden" name="IdentifyBindEnable"></td>
</tr>
<tr><td colspan="3" height="5">&nbsp;</td></tr>
<tr><td colspan="4">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
<td height="30" align="center" valign="top">
<input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">
&nbsp;&nbsp;&nbsp;
<input type="button" class="input-button" value="<% write(com_reset); %>"  onClick="init()" id="uttreset">
&nbsp;&nbsp;&nbsp;
<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DoubleLine')">
</td>
</tr>
<script language="javascript">init();</script>
</table>
</td></tr>
</table></td></tr></table>
</form></td>
</tr>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
