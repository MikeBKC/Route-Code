<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("edit_SZ");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互begin

var SzoneAutoUpdata;

<%aspOutSzoneAutoUpGlobalConfig();%> 

// 交互end

function init() 
{
    var f=SzoneAutoUpGloable_Config;
	f.SzoneAutoUpdata.checked=(SzoneAutoUpdata == "on" ? true : false);
}

function save()
{
	SzoneAutoUpGloable_Config.submit();
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(sz);%>"));//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						3,
						new Array(_("<%write(svcZoneList);%>"),'serviceZone.asp',1,0),
						new Array(_("<%write(svcZoneConf);%>"),'edit_SZ.asp',1,0),
						new Array(_("<%write(svcZoneAutoUpdata);%>"),'SzoneAutoUpdata.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>

<form name="SzoneAutoUpGloable_Config" method="post" action="/goform/formSzoneAutoUpGlobalConfig">
<table border="0" width="600" align='center'>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="zoneAutoUp_td"><%write(enableZoneAutoUpdata);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input type="checkbox" name="SzoneAutoUpdata" id="SzoneAutoUpdata" class="input-checkbox"></td>
	   </tr>
	    <!--///wpapsktbl end/// -->
</table> </td>
<table align="center"><tr><td height="5"></td></tr><tr><td>
<input type="button" onClick="save()"class="input-button" value="<%write(save);%>" name="" id="uttsave">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(help);%>" name="" id="utthelp" onClick="window.open('<% write(com_helppage); %>#serviceZone')">
</td></tr></table>
</form>

<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
