<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
</head>
<script language="javascript">
<!--
function clickAP(){alert(iframe_scan.document.aplist_fm.apidx.value)
	if(iframe_scan.document.aplist_fm.apidx.value!=""){
		SelAP_Config.selssid.value=iframe_scan.document.aplist_fm.apidx.value;
		SelAP_Config.action="/goform/";
		SelAP_Config.submit();
	}
}



//-->
</script>
<style type="text/css">
.blank_table { margin:0px 0px; padding:0px 0px; align:left; border-collapse:collapse; border-width:0 0 0 0; border-style:solid solid solid solid; border-color: gray gray gray gray; }
.wizard_outline_table {
	margin-left:0; margin-right:0; margin-top:0px; margin-bottom:0; padding:0px 0px 0px 0px;align:left; border-collapse:collapse; border-width:1 1 1 1; border-style:solid solid solid solid; border-color: gray gray gray gray;
	}
</style>

<body onLoad=""onbeforeunload="logOut()" background="images/body_main.png"  bottommargin="0" rightmargin="0" leftmargin="5" topmargin="5" marginwidth="0" marginheight="0">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('管理员配置');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form method=post action="/goform/" name=SelAP_Config>
<input type="hidden" name="selssid" >
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
<table class=blank_table align="center">
<tr><td height="20">&nbsp;</td></tr>
<tr>
<td align=right height="30">
<input type=button name=search_bt value="重新扫描AP" onClick="iframe_scan.location.reload();" class="input-button" ></td>
</tr>
<tr>
<td  align="center">
<table class=wizard_outline_table><tr><td align="center">
<iframe  src="ScanAPfrmae.asp" width=500 height=250 frameborder=no scrolling=auto name=wizardscan  id="iframe_scan"></iframe>
</td></tr>
</table></td></tr>
<tr>
<td align=right height="40">双击一个AP行或者单击“选择AP”按钮&nbsp;&nbsp;&nbsp;<input type=button name=select_bt value="选择AP" onClick="clickAP();" class="input-button"></td></tr><tr><td height="20">&nbsp;</td></tr>
</table>
</td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
