 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>系统状态-端口详情</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
var errorstr="";
<%
	getWanIfCount();
	var port = Form("port");
	aspOutPortStatics(port);
%>
var cprefix = 'bw_r';
function init()
{
	document.getElementById('port').value=port;
	cookie.set(cprefix + 'tab', port, 14);//记录当前端口号，方便流量图使用
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0" >
<!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle('端口详情');//显示页面名称</script>
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
						new Array('端口流量','../InterfaceFlow.asp',1,0),
						new Array('端口详情','stat_detailed.asp?port=1',1,1)
		);
		showThreeMenu();
	</script>
		<!--//end-->
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
     <form name="STAT"  id="STAT" method="POST" action="/goform/formStatReset">
     <input type="hidden" name="page" value="det">
	<input type="hidden" name="tag" value="tail">
	<input type="hidden" name="port" id="port" value="">
	<table width="620" border="0" align="center">
	<tr><td align="" height="50"><b>端口<script language="javascript">
	document.write(port);
    </script>详细信息</b></td></tr>

	</table>
	<TABLE cellSpacing=0 class="" cellPadding=4 width="623" border="0" align="center">
  <tr>
<td valign="top" align="center">
	<input type="submit" class="input-button" name="submit1" value="清除">
    <input type="button" class="input-button" name="submit1"  value="刷新"  onClick="window.location.reload()">
</td>
<td valign="top" align="right">
<table  border="0"  cellpadding="5" cellspacing="1" class="style22" >
<script type="text/javascript">
  var pno;;
  var numPorts;
  var portsPerLine;
  var lines;

  var j;

/*function showPortInfo(port)
{
	STAT.port.value=port;
	STAT.submit();
	
}*/
function showCnt(txt, id)
{
    document.write(
      '<td width="25%" class=vtable><font size="1">' + txt + '<\/font><\/td>' +
      '<td width="25%" align="right" class=vtable><font size="1" face="Arial">' + counters[id] + '<\/font><\/td>');
}
var counts = 0;

  numPorts = 24;
  var sumPorts=numPorts+wanIfCount+1;/*总端口数=lan口数+wan口数+一个lan*/
  if (sumPorts == 12)
    portsPerLine = 6;
  else if (sumPorts < 8)
    portsPerLine = sumPorts;
  else
    portsPerLine = 10;
  lines = sumPorts / portsPerLine;

  pno = 1;
  while (lines-- > 0)
  {
		if (counts++ % 2 == 0)
		document.write('<tr style="background-color:ffffff">');
		else document.write('<tr class="style18">');
		j = 0;
		if(counts ==1)
		{
			for(j=0; j<wanIfCount; j++)
			{/*绘制wan口*/
				  document.write(
				  '<td width="33" valign="middle" align="center">' + 
				  '<a href="stat_detailed.asp?port=WAN' + (j+1) + '" class="alink"><font color="#666666">WAN ' + (j+1) + '<\/a>' + 
				  '<\/font> <\/td>');
			}
			/*绘制lan口*/
			document.write(
			'<td width="33" valign="middle" align="center">' + 
			'<a href="stat_detailed.asp?port=LAN" class="alink"><font color="#666666">LAN<\/a>' + 
			'<\/font> <\/td>');
			j=0;
			j+=(wanIfCount+1);
		}
		
/*		var iCountNo = 1;
		if (iCountNo == 1) 
		{
			j = wanIfCount + 1;
		}
		else
		{
			j = 0;
		}
*/		
		while (j++ < portsPerLine)
		{
			if(pno == 25)
			{
				document.write('<td>&nbsp;<\/td>');
			}
			else
			{
				  document.write(
					'<td width="33" valign="middle" align="center">' + 
					'<a href="stat_detailed.asp?port=' + pno + '"class="alink"><font color="#666666">端口 ' + pno + '<\/a>' + 
					'<\/font> <\/td>');
				  pno++;
			}
		}
		
/*		iCountNo++;
		alert(iCountNo);
*/		
    	document.write('<\/tr>');
  }
counts = 0;
  document.write(
    '<\/table>' +
    '<\/td>' +
    '<\/tr>' +
    '<\/table>');
document.write('<br>');
if (wanIfCount == 4 )
{
	if (port >= 1 && port <= 24)
	{
		document.write('<TABLE  border="0" cellpadding="4" cellspacing="1" class="style22"  width="615"  align="center">');
		document.write(
		'<tr style="background-color:ffffff">' +
		'<td width="50%" colspan="2" align="center" class=vncell>' + 
		'<b>接收包<\/b>' +
		'<\/td>' +
		'<td width="50%" colspan="2" align="center" class=vncell>' +
		'<b>发送包<\/b>' +
		'<\/td>' +
		'<\/tr>');
		document.write('<tr class="style18">');
		showCnt("接收帧", 0);
		showCnt("发送帧", 1);
		document.write('<\/tr>');
		document.write('</table>');
	}
	else
	{
		document.write('<TABLE  border="0" cellpadding="4" cellspacing="1" class="style22"  width="615"  align="center">');
		document.write(
		'<tr style="background-color:ffffff">' +
		'<td width="50%" colspan="2" align="center" class=vncell>' + 
		'<b>接收包<\/b>' +
		'<\/td>' +
		'<td width="50%" colspan="2" align="center" class=vncell>' +
		'<b>发送包<\/b>' +
		'<\/td>' +
		'<\/tr>');
		document.write('<tr class="style18">');
		showCnt("接收帧", 0);
		showCnt("发送帧", 1);
		document.write('<\/tr>');
		document.write('<tr style="background-color:ffffff">');
		showCnt("接收字节", 2);
		showCnt("发送字节", 3);
		document.write('<\/tr>');
		document.write('<tr class="style18">');
		showCnt("接收错误包", 6);
		showCnt("发送错误包", 7);
		document.write('<\/tr>');
		document.write('</table>');
	}
}
else
{
	document.write('<TABLE  border="0"  cellpadding="4" cellspacing="1" class="style22"  width="615"  align="center">');
	document.write(
	'<tr style="background-color:ffffff">' +
	'<td width="50%" colspan="2" align="center" class=vncell>' + 
	'<b>接收包<\/b>' +
	'<\/td>' +
	'<td width="50%" colspan="2" align="center" class=vncell>' +
	'<b>发送包<\/b>' +
	'<\/td>' +
	'<\/tr>');
	document.write('<tr class="style18">');
	showCnt("接收帧", 0);
	showCnt("发送帧", 1);
	document.write('<\/tr>');
	document.write('<tr style="background-color:ffffff">');
	showCnt("接收字节", 2);
	showCnt("发送字节", 3);
	document.write('<\/tr>');
	document.write('<tr class="style18">');
	showCnt("接收广播和多播", 4);
	showCnt("发送广播和多播", 5);
	document.write('<\/tr>');
	document.write('<tr style="background-color:ffffff">');
	showCnt("接收错误包", 6);
	showCnt("发送错误包", 7);
	document.write('<\/tr>');
	document.write(
	'<tr class="style18">' +
	'<td width="50%" colspan="2" align="center" class=vncell>' +
	'<b>接收包计数器</b>' +
	'<\/td>' +
	'<td width="50%" colspan="2" align="center" class=vncell>' +
	'<b>发送包计数器</b>' +
	'<\/td>' +
	'<\/tr>');
	document.write('<tr style="background-color:ffffff">');
	showCnt("接收小包(64Bytes)",8);
	showCnt("发送小包(64Bytes)", 9);
	document.write('<\/tr>');
	document.write('<tr class="style18">');
	showCnt("接收大包(1024-Bytes)", 10);
	showCnt("发送大包(1024-Bytes)", 11);
	document.write('<\/tr>');
	document.write('</table>');
}
</script>
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
