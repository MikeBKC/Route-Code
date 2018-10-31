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

	var tmpSrcPort = 0;
    for (i = 1; i <= maxLanPort; i++) {
		if (document.getElementById('port'+i).checked) {
			tmpSrcPort |= (1 << (i - 1));
		}
	}
	f.hidSourPort.value = tmpSrcPort;
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
	f.monitorPort.value=monitorPort;
	f.PortMirrorEn.checked=(portMirror.toLowerCase()=="on"?true:false);
	moniPortSel(monitorPort - 1);
	sourPortChk(sourcePort);
}

/* 根据value每一位的值，初始化chkbox 是否勾选 */
function sourPortChk(value) {
	for (i = 1; i <= maxLanPort; i++) {
		if ((value & (1 << (i - 1))) != 0) {
			document.getElementById('port'+i).checked = true;
		}
	}
}

function sourPortClear() {
	for (i = 1; i <= maxLanPort; i++) {
		document.getElementById('port'+i).checked = false;
		document.getElementById('port'+i).disabled = false;
	}
}

function moniPortSel(index) {
	var port = index + 1;
	sourPortClear();
	//document.getElementById('port'+port).disabled = true;
}
</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.gif">
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
		<td width="40%" align="right" height="25"><%write(por_start);%></td>
		<td width="1%">&nbsp;</td>
		<td width="59%" height="20"><input name="PortMirrorEn" type="checkbox" ></td>
	      </tr>
   <tr>
     <td  height="25" align="right" id="jiance3"><%write(monitor);%></td>
     <td height="20">&nbsp;</td>
     <td>&nbsp;<select name="monitorPort" onChange="moniPortSel(this.selectedIndex)">
     </select></td>
</tr>
   <tr style="display:none">
    <td align="right" height="25" id="jiance2"><%write(monitored);%></td>
    <td height="20">&nbsp;</td>
    <td height="20">
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
<script language="javascript">
var lanPort = maxLanPort;
/* 每行5个 */
for (i = 1; i <= lanPort; i++) {
	/* 行开始 */
	if (i % 5 == 1) {
		document.write('<tr>');
	}
	document.write('<td width="70"><input name="port'+i+'" id="port'+i+'" type="checkbox"><%write(por_port);%>'+i+'</td>');
	/* 行结尾 */
	if (i % 5 == 0) {
		document.write('</tr>');
	}
}
/* 做补齐 */
var remainder = lanPort % 5;
if (remainder != 0) {
	for (i = 0; i < remainder; i++) {
		document.write('<td>&nbsp;</td>');
	}
	document.write('</tr>');
}
</script>
</table>	
	</td>
</tr>
	   <tr><td colspan="4">
  <table width="21%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="bottom"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave"> &nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PortMirror8367M')"></td>
  </tr>
</table>

  </td></tr>
  <tr><td colspan="3" height="10">&nbsp;<input name="hidSourPort" type="hidden" value=""></td></tr>
</table></td></tr></table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td><script>init();</script>
  </tr>
</table>
</body>
</html>
