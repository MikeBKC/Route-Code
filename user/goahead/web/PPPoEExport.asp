<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEServer");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

function ExportUser(){
	PPPoE_export.action="/goform/formPppoeExport";
	PPPoE_export.submit();
}

</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(com_pppSer);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"class="table_bg">
  <tr valign="top">
    <td><form name="PPPoE_export" method="post" action="">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						5,
						new Array(_("<%write(com_pppGlo);%>"),'PPPoEGlobal.asp',1,0),
						new Array(_("<%write(com_pppAcc);%>"),'PPPoEServer.asp',1,0),
						new Array(_("<%write(com_pppSta);%>"),'PPPoEConSta.asp',1,0),
						new Array(_("<%write(com_pppExp);%>"),'PPPoEExport.asp',1,1),
						new Array(_("<%write(com_pppImp);%>"),'PPPoEImport.asp',1,0)
		);
		showThreeMenu();
	</script>
   <table id=grouplist style="display" width="100%"  border="0" align="center" cellpadding="0" cellspacing="0" height="90">
  <tr>
    <td align="center" class="three_td" valign="top"><table id=grouplist style="display" width="700"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
			  <tr>
			  <td width="100%" valign="top" align="center"><input type="button" value="<%write(pps_export);%>" class="input-button" onClick="ExportUser();" id="export"></td>
			  </tr>
		  </table></td></tr></table>
  </form></td>
  </tr>
</table></td> 
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
