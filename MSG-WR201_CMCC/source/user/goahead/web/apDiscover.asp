<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("apDiscover");%>
<%langTran("commonWords");%>

<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互end
<% 
aspOutPutScanInfo();
%>


function init()
{
	if (autoScan == 1)
	{
		document.getElementById('manu').checked = false;
		document.getElementById('auto').checked = true;
	}
	else
	{
		document.getElementById('auto').checked = false;
		document.getElementById('manu').checked = true;
	}
//	document.getElementById('maxMng').value = maxAp;
	document.getElementById('allDevice').checked = (allDev == 1 ? true : false);
	//scanType_sw();
}
/*
function scanType_sw()
{
	var obj1 = document.getElementById("auto");
	var obj2 = document.getElementById("scanButton");
	if (obj1.checked == true)
	{
		obj2.style.display = "none";
	}
	else
	{
		obj2.style.display = "block";
	}
}
*/
function Reset()
{
	apScan.reset();
	init();
	allInputRight(apScan);
}

function save()
{
	if (document.getElementById('auto').checked == true)
	{
		apScan.mode.value = 1;
	}
	else
	{
		apScan.mode.value = 0;
	}
	if (document.getElementById('allDevice').checked == true)
	{
		apScan.allDevice.value = 1;
	}
	else
	{
		apScan.allDevice.value = 0;
	}
//	alert(apScan.mode.value);
//	alert(apScan.maxMng.value);
//	alert(apScan.allDevice.value);
//	return;
	var f = apScan;
	f.action = "/goform/formScanMode";
	f.submit();	
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(APAdmin);%>"));//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  >
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array(_("AP<%write(list);%>"),'apManage.asp',1,0),
//						new Array(_("AP<%write(power);%>"),'apPermission.asp',1,0),
						new Array(_("AP<%write(scan);%><%write(method);%>"),'apDiscover.asp',1,1)
//						new Array(_("APMngRcd"),'apMngRecord.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form method="post" target="" name="apScan">
<br>
<br>
<table width="300" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td width="25" align="left"><input name="mode" type="radio" value="auto" checked onClick="" id="auto"></td>
    <td width="66" align="left" id="autoScan"><%write(autoScan);%></td>
    <td align="right"><input type="radio" name="mode" value="manu" onClick="" id="manu"></td>
    <td align="left" id="manuScan"> <%write(manualScan);%></td>
  </tr>
  <tr>
    <td colspan="2" align="right">&nbsp;</td>
    <td width="33">&nbsp;</td>
    <td width="176">&nbsp;</td>
  </tr>
<!--  <tr>
    <td colspan="3" align="left" id="maxAP"><%write(mngNumLargAP);%></td>

    <td align="left"><input type="text" name="maxMng" id="maxMng" size="4" onKeyPress="figureOnly()" maxlength="3"></td>

    <td align="left">
    <select name="maxMng" id="maxMng" class="input">
      <option value="1">4</option>
      <option value="2">8</option>
      <option value="3">16</option>
      <option value="4">32</option>
      <option value="5" selected>64</option>
    </select>
    </td>

  </tr>
-->
  <tr>
    <td colspan="2">&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td colspan="3" align="left" id="scanDev"><%write(scnAllWireDev);%></td>
    <td align="left"><input type="checkbox" name="allDevice" id="allDevice"></td>
  </tr>
  <tr>
    <td colspan="2">&nbsp;</td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td colspan="2"><!--<input type="button" name="scanButton" id="scanButton" class="input-button" onClick="save();" value="扫描">--></td>
    <td>&nbsp;</td>
    <td>&nbsp;</td>
  </tr>
</table>
<table align="center"  ><tr><td height="5"></td></tr><tr><td>
<input type="button" id="saveButton" onClick="save()"class="input-button" value="<%write(save);%>" name="">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" id="resetButton" class="input-button" value="<%write(refill);%>" name="" onClick="Reset();">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="<%write(help);%>" onClick="window.open('<% write(com_helppage);%>#apDiscover')"></td></tr></table>

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
