 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>交换功能-端口镜像</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
<%
	var macStr = Form("macStr");//远程设备mac地址
	var urcpSet = Form("urcpSet");//是否联动设置镜像后跳转到此页面的
	if (macStr!="" || urcpSet!="")
	{
		aspOutPutMirrorUrcp();//联动信息
	}
	else
	{
		aspOutPutMirror();//本地信息
		write('var ipStr="";');
		write('var macStr="";');
		write('var passStr="";');
		write('var urcpError="";');
	}
%>
var aggArr=new Array();
var aggPortColor = "red";
for(var i=0;i<aggLists.length;i++)
{
	aggArr[i] = aggLists[i].split(" ");
}
function go_back()
{
	MIRROR.action="remote_manage.asp";
	MIRROR.submit();
}
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#mirrorUrcp')};
		}
	}
}
function init(){
	var obj=null;
	MIRROR.passStr.value = passStr;
	MIRROR.ipStr.value= ipStr;
	MIRROR.macStr.value= macStr;
	MIRROR.numPorts.value= numPorts;
	if(macStr!='')
	{
		window.document.title = "交换功能-端口镜像";
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		window.document.title+=" / "+ipStr;
		initHelp();
	}
	addMirrorPortOpt();
	eval("MIRROR.Port").value=mirrorPort;
	select_one(mirrorPort);
}
function reSet()
{
	document.forms[0].reset();
	eval("MIRROR.Port").value=mirrorPort;
	select_one(mirrorPort);
	init();
}
function addMirrorPortOpt()
{
	var obj=eval("MIRROR.Port");
	var flag=1,flag2=1;
	var opt = null;
	for(var pno=1;pno<=numPorts;pno++)
	{
		flag=1;
		for(var i=0;i<aggArr.length;i++)
		{
			if(aggArr[i].length>1 && aggArr[i].searchData(pno)>=0)
			{
				flag=0;//该端口属于某个汇聚组
				flag2=0;
				break;
			}
			
		}
		opt = newOpt(pno,pno);
		if(!flag) opt.style.color=aggPortColor;
		obj.options.add(opt);
	}
	if(!flag2)
	{
		obj.title="红色底字为已设置汇聚的端口";
	}
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="../images/body_main.png">
<script language="javascript">urcpErrorPost("remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('端口镜像');//显示页面名称</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br>
	<form method="POST" name="MIRROR">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="numPorts">
<script type="text/javascript">
var pno;
function select_one(ctl) {
    var val =  typeof(ctl)=="object" ? eval(ctl.value) : eval(ctl);
    for (var i = 0; i < numPorts; i++) {
          eval("MIRROR.R" + (0x10 + i).toString(16)).disabled = false; 
    }
	eval("MIRROR.R" + (0x10 + (val-1)).toString(16)).disabled = true; 
	eval("MIRROR.R" + (0x10 + (val-1)).toString(16)).checked = false; 
}
function sel_allChkbox(obj) {
    var val = eval(MIRROR.Port.value);
    for (var i = 0; i < numPorts; i++) {
		if(i!=(val-1))
          eval("MIRROR.R" + (0x10 + i).toString(16)).checked = obj.checked; 
    }
}
function save() {
	var flag = 1;
	for (var i = 0; i < numPorts; i++) {
		if(eval("MIRROR.R" + (0x10 + i).toString(16)).checked )
		{
		   flag = 0 ;
		   break;
		}
    }
	if(!flag)
	{
		if(MIRROR.Port.options[MIRROR.Port.selectedIndex].style.color == aggPortColor)
		{
			alert("监控端口不能在汇聚组内");
			return;
		}
	}
	if(macStr=="")
	{
 		MIRROR.action="/goform/formMirror";
	}
	else
	{
		MIRROR.action="/goform/formMirrorUrcp";
	}
    MIRROR.submit();
}
document.write('<TABLE cellSpacing="0" width="600" cellPadding=4 align="center" border="0">');
//document.write('<tr><td colspan="5" height="21" background="/../images/bg_title.jpg"></td></tr>');
document.write('<tr><td  align="left" class=vncell height="22">监控端口&nbsp;');
document.write('<select name="Port" id="Port" onChange="select_one(this)">');
document.write('<\/select><\/td>');
document.write('<td width="" align="right">' + '<input type="button" value="保存" class="input-button" name="" onClick="save()"> ' + '<input type="button" class="input-button" value="重填" name="" onClick="reSet()"> ' + '<input type="button" class="input-button" value="帮助" id="helpButton" name="helpButton" onClick=window.open("../rhelpinfo.htm#mirror")> ' +'<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();">'+ '</td><\/tr>');
document.write('<tr valign="bottom" ><td  align="left" >全选/全不选<input type="checkbox" name="chk_all" onClick="sel_allChkbox(this)"><\/td><td align="right" class="helpfont"><strong>注意：</strong>建议被监控端口只选取所需端口（如连接路由器的端口）</td></tr>');
document.write('</table>');
document.write('<table align="center" width="600" border="0"><tr><td align="left">');
document.write('');
document.write('</td></tr></table>') ;
document.write('<TABLE  border="0"  cellpadding="4" cellspacing="1" class="style22"  width="600"  align="center">');
document.write('<tr class="style18"><td  align="center" class="style18" height="22"><b>端口<\/b><\/font><\/td>');
document.write('<td  align="center" class=vtable><b>被监控端口<\/b><\/td>');
document.write('<td  align="center" class="style18" height="22"><b>端口<\/b><\/font><\/td>');
document.write('<td  align="center" class=vtable><b>被监控端口<\/b><\/td><\/tr>');
for (pno = 1; pno <= numPorts; pno += 2) {
    if (1 == ((pno - 1) / 2) % 2) {
        document.write('<tr class="style18">');
    } else if (0 == ((pno - 1) / 2) % 2) {
        document.write('<tr>')
    }

    document.write('<td  align="center"  height="22">' + pno + '<\/td>' + '<td  align="center" class=vtable><input type="checkbox" name="R' + (0x10 + pno - 1).toString(16) + '" value="1" ' + mirrorSource[pno - 1] + '><\/td>');
    document.write('<td  align="center"  height="22">' + (pno + 1) + '<\/td>' + '<td  align="center" class=vtable><input type="checkbox" name="R' + (0x10 + pno).toString(16) + '" value="1" ' + mirrorSource[pno] + '><\/td>' + '<\/tr>');
}

document.write('<\/table>');
</script>
<table align="center" width="600">
<tr><td height="10"> </td></tr>
<tr><td width="" align="center">
<input type="button" value="保存" class="input-button" name="" onClick="save()">
<input type="button" class="input-button" value="重填" name="" onClick="reSet()">
<input type="button" id="helpButton" name="helpButton" class="input-button" value="帮助" name="" onClick="window.open('../rhelpinfo.htm#mirror')">
<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();">
</td></tr></table>
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
