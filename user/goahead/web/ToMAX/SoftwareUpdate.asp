<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("SoftwareUpdate"); %>
<% langTran("common"); %>
<% langTran("Link"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">


<% outputSystemVersion();%>
var errorstr="";

<% outputTheErrorMsg(); %>
<% outputProductID();%>
<% outputLinkID();%>
ProductLinkID="http://www.tomaxcom.com/shengjiruanjian";
<!--
function save()
{
  SoftWare_Config.action = "/goform/BackupFirmware";
  SoftWare_Config.submit();
}
function update()
{
  if(checkData())
  {
	SoftWare_Config.reBoot.disabled=false;
	SoftWare_Config.action = "/goform/UpdateFirmware";
	SoftWare_Config.submit();
  }
}

function printlinklogo()
{
    document.getElementById("downloadversion").href=ProductLinkID;          
    document.getElementById("downloadnewversion").href=ProductLinkID;
}

function checkData()
{
	if (SoftWare_Config.updatesoftware.value==""){
		 alert("<% write(SpecifyUpgradeSW); %>");return false;
	}
	 
	if (SoftWare_Config.reBoot.checked && !confirm("<% write(UpgradeMessage); %>")) return false;
	return true;
}

function init(){
	var f=SoftWare_Config;
	f.currSoftware.value=revisions;

	/* 硬件版本初始化 */
	if (hardwareID != "") {
		hwmodeltr.style.display = dis;
		document.getElementById("now software").innerHTML = "<strong><% write(SWVer); %></strong>";
		document.getElementById("now hardware").innerHTML = "<strong><% write(HDVer); %></strong>";
	} else {
		hwmodeltr.style.display = "none";
		document.getElementById("now software").innerHTML = "<% write(SWVer); %>";
	}
}
-->
</script>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(SoftwareUpdate); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form action="" name="SoftWare_Config" method="post" enctype="multipart/form-data">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
  		<tr id="hwmodeltr" style="display:none">
		<td width="36%" align="right" height="25" id="now hardware"><strong><% write(HDVer); %></strong></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><strong><script language="javascript">document.write(hardwareID);</script></strong></td>
	    </tr>	
        <tr>
		<td width="36%" align="right" height="25" id="now software"></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="currSoftware" type="text" size="40" class="input2"  disabled="disabled"></td>
	      </tr>
		  <!--<tr><td colspan="3" height="5">&nbsp;</td></tr>
		  <tr>
		<td  align="right" id="save software"><% write(SaveToHost); %></td>
		<td >&nbsp;</td>
		<td height="20"><input  type="button"  class="input-button" value="<% write(com_save); %>" onClick="save()" id="save" disabled></td>
	      </tr>
	      -->
		  <tr><td colspan="3" height="5">&nbsp;</td></tr>
 		  <tr><td colspan="3" height="25" align="center"><a href="#" onclick="printlinklogo()" target="_blank" id="downloadversion"><% write(DowloadNew); %></a></td></tr>
		  <tr>
		<td align="right" id="choose version"><% write(ChoseUpgradeSW); %></td>
		<td >&nbsp;</td>
		<td height="20"><input name="updatesoftware" type="file" ></td>
	      </tr>
		  <tr><td>&nbsp;</td><td  height="25" align="center">&nbsp;</td><td id="update then reset"><% write(RebootAfterUP); %>&nbsp;<input type="checkbox" name="reBoot"  disabled="disabled" checked="checked"/></td></tr>
		  <tr><td>&nbsp;</td><td>&nbsp;</td><td   height="25" ><input type="button"  class="input-button"  value="<% write(Upgrade); %>" onClick="update()" id="uttupdate">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#SoftwareUpdate')"></td></tr>  <tr><td colspan="3" height="20">&nbsp;</td></tr>
		   <tr><td colspan="4"><table width="80%" align="center">
	   <tr  height="30"><td width="2%"></td>
	   <td class="helpfont"><% write(Click); %><a href="#"  onclick="printlinklogo()" id="downloadnewversion" target="_blank">“<% write(DowloadNew); %>”</a><% write(DowloadMessageTomax); %></td></tr>
	   <tr  height="30"><td width="2%"></td>
	   <td class="helpfont" id="attention4-2"><% write(UpgradeMessage1); %></td></tr>
	   <tr  height="30"><td width="2%"></td>
	   <td class="helpfont" id="attention4-3"><% write(UpgradeMessage2); %></td></tr>
	   </table></td></tr>
		  <script language="javascript">init();</script>
  </table></td></tr>
</table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
	      
