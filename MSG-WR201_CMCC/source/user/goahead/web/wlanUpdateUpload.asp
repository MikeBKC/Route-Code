<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("wlanUpdateUpload");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
<%langTran("apSoftMng");%>
//交互begin
<%
    aspOutPutUploadFirmware();
%>
// 交互end

function softUpload()
{
  if(checkData())
  {
	SoftUpload.action = "/goform/formUploadFirmware";
	SoftUpload.submit();
  }
}

function checkData()
{
	if (SoftUpload.uploadSoftware.value==""){
		 alert(_("<%write(chooseVer);%>"));
		 return false;
	}
	return true;
}

</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(AdminAPSoft);%>"));//显示页面名称</script>
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
						new Array(_("<%write(swList);%>"),'apSoftMng.asp',1,0),
						new Array(_("<%write(swUpload);%>"),'wlanUpdateUpload.asp',1,1)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form method="post" name="SoftUpload" enctype="multipart/form-data" action="">
<br>
<table border="0" width="600" align='center'>
  <tr>
    <td width="210" align="right" id="choosVersion_td"><%write(swSelect);%></td>
    <td width="5" >&nbsp;</td>
    <td width="372" height="20"><input name="uploadSoftware" type="file"></td>
  </tr>
  <tr>
    <td align="right">&nbsp;</td>
    <td  height="25" align="center">&nbsp;</td>
    <td id="update then reset4">&nbsp;</td>
  </tr>
  <tr><td>&nbsp;</td><td>&nbsp;</td><td   height="25" ><input type="button" name="upload" class="input-button"  value="<%write(swUpload);%>" onClick="softUpload();" id="upload">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#apSoftMng')" id="helpBotton"></td></tr>
</table>
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
