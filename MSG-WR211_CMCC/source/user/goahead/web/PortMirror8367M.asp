<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PortMirror");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">
<%aspPortMirrorData();%>
function save(){
	var f=formportmirror;
	if(parseInt(f.monitorPort.value) == parseInt(f.sourcePort.value))
	{
		alert("<%write(mirrCheck);%>");
		return;
	}
    f.submit();
}  

function initTranslation()
{
    var e;
}
function mirrorSelBuild(obj)
{
	var opt;
	for(var i=1;i<=maxLanPort;i++)
	{    
	    opt = newOpt("LAN"+i,i);
        obj.options.add(opt);
	}
}

function init()
{
    var f=formportmirror;
	mirrorSelBuild(f.monitorPort);
	mirrorSelBuild(f.sourcePort);
	f.monitorPort.value=monitorPort;
	f.sourcePort.value=sourcePort;
	f.PortMirrorEn.checked=(portMirror.toLowerCase()=="on"?true:false);
}

</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <tr>
  <td width="15"></td>
  <td><script language="javascript">showSecondTitle("<%write(por_mirror);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">//showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/ConfigPortMirror"  name="formportmirror" method="post" enctype="multipart/form-data">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7">
	<table border="0" width="600" align='center'>
    
 <tr><td colspan="3" height="30">&nbsp;</td></tr>
 <tr>
		<td width="46%" align="right" height="25"><%write(por_start);%></td>
		<td width="1%">&nbsp;</td>
		<td width="53%" height="20"><input name="PortMirrorEn" type="checkbox" ></td>
	      </tr>
   <tr>
     <td  height="25" align="right" id="jiance3"><%write(monitor);%></td>
     <td height="20">&nbsp;</td>
     <td><select name="monitorPort">
     </select></td>
</tr>
   <tr>
    <td align="right" height="25" id="jiance2"><%write(monitored);%></td>
    <td height="20">&nbsp;</td>
    <td height="20"><select name="sourcePort">
      <option value="0"><%write(com_noselect);%></option>
    </select></td>
</tr>
	   <tr><td colspan="4">
  <table width="21%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="bottom"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave"> &nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PortMirror8367M')"></td>
  </tr>
</table>

  </td></tr>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td><script>init();</script>
  </tr>
</table>
</body>
</html>
