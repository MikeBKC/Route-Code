<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("TagVlan");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

Butterlate.setTextDomain("admin");

<% 
	aspOutputPvid(); 
	getWanIfCount();
%>

function save(){
	var f = vid;
	f.action="/goform/formPvidConfig";
	f.submit();	
}

function init () {
	var f = vid;
	
	switch(wanIfCount) {
		case 1:
			break;
		case 2:
			showTable(port4Tr, 0);
			break;
		case 3:
			showTable(port4Tr, 0);
			showTable(port3Tr, 0);
			break;
		case 4:
			showTable(port4Tr, 0);
			showTable(port3Tr, 0);
			showTable(port2Tr, 0);
			break;
		default:
			break;		
	}	
	f.pvid1.value = pvid1;
	f.pvid2.value = pvid2;
	f.pvid3.value = pvid3;
	f.pvid4.value = pvid4;

}

function initTranslation()
{
}

</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.gif">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(pvidSetting);%>");</script>
  <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="vid" method="post" enctype="multipart/form-data">
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						4,
						new Array("<%write(tagVlanList);%>",'TagVlanList.asp',1,0),
						new Array("<%write(tagVlanSetting);%>",'TagVlan_edit.asp',1,0),
						new Array("<%write(pvidSetting);%>",'vid.asp',1,1),
						new Array("<%write(vlanRouting);%>",'vlan_cc.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->	

<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="style7">	
	<table border="0" width="600" align='center'>
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
	<!--<tr>
    <td width="44%" align="right">端口</td>
  	<td width="8%"></td>
	<td width="48%">PVID</td>
  </tr> -->
  <tr id="port1Tr" style="display:">
    <td width="41%" align="right"><%write(port1);%></td>
  	<td width="12%" align="right"><%write(pvid);%></td>
	<td width="47%"><input type="text" class="input" name="pvid1" id="pvid1" size="5" maxlength="4" value=""></td>
  </tr>
  <tr id="port2Tr" style="display:">
    <td width="41%" align="right"><%write(port2);%></td>
  	<td width="12%" align="right"><%write(pvid);%></td>
	<td width="47%"><input type="text" class="input" name="pvid2" id="pvid2" size="5" maxlength="4" value=""></td>
  </tr>
  <tr id="port3Tr" style="display:">
    <td width="41%" align="right"><%write(port3);%></td>
  	<td width="12%" align="right"><%write(pvid);%></td>
	<td width="47%"><input type="text" class="input" name="pvid3" id="pvid3" size="5" maxlength="4" value=""></td>
  </tr>    
  <tr id="port4Tr" style="display:">
    <td width="41%" align="right"><%write(port4);%></td>
  	<td width="12%" align="right"><%write(pvid);%></td>
	<td width="47%"><input type="text" class="input" name="pvid4" id="pvid4" size="5" maxlength="4" value=""></td>
  </tr>  
  <tr><td colspan="3" height="30">
  <table align="center"><tr><td height="5"></td></tr><tr><td>
<input type="button" onClick="save()"class="input-button" value="<%write(com_save);%>" name="" id="uttsave">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(com_reset);%>" name="" id="uttreset" onClick="Reset();">
<input type="button" class="input-button" value="<%write(com_help);%>" name="" id="utthelp" onClick="window.open('<%write(com_helppage);%>#serviceZone')">
</td></tr></table>
  </td></tr>
  <input type="hidden" value="1" name="re">
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
