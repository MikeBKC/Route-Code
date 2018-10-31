<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DeviceConfig"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<% aspoutErrorMsg(); %>

<!--
function saveConfig(){
	Device_Config.action="/goform/formExportSettings";
	Device_Config.submit();
}
function saveImport(){
	if (Device_Config.importConfig.value=="")
	  	alert("<% write(warning1); %>");
	else if(confirm("<% write(warning2); %>"+"\n"+"<% write(warning4); %>")) {
		Device_Config.action = "/goform/formUploadSettings";
		Device_Config.submit();
	}
}
function saveReset(){
	if(confirm("<% write(warning3); %>")) {
		Device_Config.action="/goform/formResetSettings";
		Device_Config.submit();
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
  <td><script language="javascript">showSecondTitle("<% write(DeviceConf); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/" method="post" name="Device_Config" enctype="multipart/form-data">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
        <tr>
		<td width="36%" align="right" height="25" id="save to local"><% write(SaveConfToHost); %></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="saveconfig" type="button" size="18" value="<% write(com_save);%>" class="input-button" onClick="saveConfig()" id="save"></td>
	      </tr>
		  <tr><td colspan="3" height="15">&nbsp;</td></tr>
		  <tr>
		<td width="36%" align="right" id="import config"><strong><% write(ImportConf); %></strong></td>
		<td width="3%">&nbsp;</td>
		<td height="20">&nbsp;</td>
	      </tr>
		<tr>
		<td width="36%" align="right" id="before import reset"><% write(LMDBeforImpConf); %></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="chkset" type="checkbox" checked="checked"></td>
	      </tr>
		   <tr>
		<td width="36%" align="right" id="choose file"><% write(SelConfFile); %></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="importConfig" type="file">&nbsp;&nbsp;&nbsp;&nbsp;<input name="saveimport" type="button" class="input-button"  value="<% write(Import); %>" onClick="saveImport()" id="import"></td>
	      </tr>
		  <tr><td colspan="3" height="15">&nbsp;</td></tr>
		  <tr>
		<td width="36%" align="right" id="recover config"><% write(LoadManufactureDef); %></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="savereset" type="button" class="input-button"  value="<% write(Recover); %>" onClick="saveReset()" id="recover">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DeviceConfig')"></td>
	      </tr>
		  <tr><td colspan="4"><table width="100%">
	   <tr  height="40">
	   <td class="helpfont" valign="bottom" id="attention3"><strong><% write(Attention); %>：</strong>
	   <% write(LMDMessage); %></td></tr>
	   </table></td></tr>
		  <tr><td colspan="3" height="20">&nbsp;</td></tr>
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
	      
