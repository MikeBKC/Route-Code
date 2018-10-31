<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("portSpeed");%>
<title><%write(port_speed);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
var numPorts = 3;
var curSpeed = new Array("Down","Down","Down");
var speedMode = new Array("3","3","3");
<%
	aspOutputPortConfigure();
	//本地需增加numPorts打印
	write('var ipStr="";');
	write('var macStr="";');
	write('var passStr="";');
	write('var urcpError="";');
	write('var errorstr="";');
	write('var aggLists=new Array();');
	write('var portInfo=new Array();');
%>
<%getWanIfCount();%>
portInfo[0]="LAN";
ConnStatuss[0]="";
for(i=1;i<numPorts;i++){
	 
    if(is3GPort(i + 1,wanIfCount)){
	portInfo[i] = "3G";
    } else if (isAPCliPort(i + 1,wanIfCount)) {
	portInfo[i] = "APClient";
    }
    else
    {
	portInfo[i]="WAN"+PortNames[i];
    }
    ConnStatuss[i]=ConnStatuss[i]==1?"<%write(com_Connected);%>":"<%write(com_interrupt);%>";
}


</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript"></script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(port_speed);%>");//显示页面名称</script>
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
	<form method="POST" name="PORTS">
	<input type="hidden" value="Apply" name="_submit">
	<input type="hidden" value="0" name="hiddenvalue">
	<input type="hidden" value="0" name="hiddenvalue2">
	<input type="hidden" value="0" name="hiddenvalue3">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="numPorts">
	<br>
	<table border="0" width="600" class="styleblue" cellpadding="0" cellspacing="0" align="center" style="display:none">
	<tr>
	<td height="25">全局设置	
	  <script language="javascript">
	     document.write('<select name="sel_global" class="input" onChange="sel_global_fuc(this)">');
	     document.write('<option value="0">设置模式</option>');
	     document.write('</select>&nbsp;');
	  </script>
      <script language="javascript">
		  document.write('<select class="input" size="1" name="sel1" onBlur="sel_globalport_func(this)" style="display">');
			document.write('<option value="3">自动协商</option>');
			document.write('<option value="4">10M(半双工)</option>');
			document.write('<option value="0">10M(全双工)</option>');
			document.write('<option value="5">100M(半双工)</option>');
			document.write('<option value="1">100M(全双工)</option>');
			document.write('<option value="2">1000M(全双工)</option>');
			document.write('<option value="6">禁止</option>');
			document.write('</select>');
	  </script>
      <input class="input"  type="text" name="txt1" onKeyUp="javascript:txt_keypress(event,this)" onKeyPress="figureOnly()" maxlength="4" style="display:none;width:80px; height:20px" onChange="checkNumber(this, 1518, 9600, '允许最大帧')">
      <input type="checkbox" name="chk1" style="display:none" onClick="chk1_click(this)"></td>
<td width="256" align="right">
<input type="button" value="保存" class="input-button" onClick="save()" name="">
<!--<input type="button" value="刷新" class="input-button" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="重填" name="" onClick="reSet();">
<input type="button" class="input-button" value="帮助" id="helpButton" name="helpButton" onClick="window.open('<% write(com_helppage); %>#ports')">
<input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();">
</td>
</tr>
<tr><td colspan="2" height="8"></td></tr>
</table>
<script type="text/javascript">
var pno;
var i;
//交互
var aggArr = new Array();
for (i = 0; i < aggLists.length; i++) {
    aggArr[i] = aggLists[i].split(" ");
}
var linkStyle = new Array(); //连接状态边框的样式
for (var i = 0; i < numPorts; i++) {
    if (trimstr(curSpeed[i]) == 'Down' || trimstr(curSpeed[i]) == 'Disabled') {
        linkStyle[i] = 'border:0px';
    } else if(curSpeed[i].indexOf('G')>=0 || curSpeed[i].indexOf('g')>=0) {
        linkStyle[i] = 'border:3px solid;border-color:#FF6633';//1000m端口
    }
	else//100m以及以下
	{
		linkStyle[i] = 'border:3px solid;border-color:#79BE4E';
	}
}

function go_back()
{
	PORTS.action="portSpeed.asp";
	PORTS.submit();
}
function init() {
	var obj=null;
	PORTS.passStr.value = passStr;
	PORTS.ipStr.value= ipStr;
	PORTS.macStr.value= macStr;
	PORTS.numPorts.value= numPorts;
	PORTS.sel1.style.display = 'inline';
	PORTS.txt1.style.display = 'none';
	PORTS.chk1.style.display = 'none';
}
function initHelp()
{
}
function save() {
    var objArray = document.getElementsByTagName("input");
    var n = 0;
    var tmp = null,
    tmp2 = 0;
    var protection = 0,
    bloodmask = 0;

    for (var i = 0; i < objArray.length; i++) {
        if (objArray[i].type == "checkbox" && objArray[i].name != "chk1") {
            if (objArray[i].checked) {
                objArray[i].value = "1";
            } else {
                objArray[i].value = "0";
            }
        }
        if (objArray[i].type != "text" || objArray[i].name == "txt1") {
            continue;
        }
        n = eval("0x" + objArray[i].name.substring(1)) - 0x10;
    }
    // objArray = document.getElementsByName("chk_arp");
    // for (var i = 0; i < objArray.length; i++) {
    //      if (objArray[i].checked) {
    //         tmp2 = setbit(tmp2, i)
    //    }
    // }
    PORTS.action="/goform/formPortReducedConfigure";
    PORTS.submit();
}
function modeSelected(pno, value) {
    if (speedMode[(pno - 1)] == value) {
        return "selected";
    } else {
        return "";
    }
}
function reSet()
{
	PORTS.reset();
	init();
	allInputRight(PORTS);
}
document.write('<TABLE  width="600"  border="0" align="center"  cellpadding="4" cellspacing="1" class="style22" >' + '<tr style="background-color:ffffff">' + '<td  class="bh" height="22" width="20" align="center"><b><%write(port_ID);%><\/b><\/td>' + '<td  class="bh" align="center" width="135"><b><%write(port_name);%><\/b><\/td>' + '<td class="bh" align="center" width="120"><b><%write(com_ConnectionStatus);%><\/b><\/td>' + '<td class="bh" align="center" width="137"><b><%write(port_setmode);%><\/b><\/td>');

/*if(pageType!=2) document.write('<td class="bh" align="center" width="96"><b>允许最大帧<\/b><\/td>');
document.write('<td class="bh" align="center" width=""><b>流控<\/b><\/td>');
if(pageType !=2 && pageType !=3) document.write('<td class="bh" align="center" width="43"><b>端口保护<\/b><\/td>' + '<td class="bh" align="center" width=""><b>泛洪<\/b><\/td>');
*/
document.write('<\/tr>');
for (pno = 1; pno <= numPorts; pno++) {
    i = pno - 1;
    if (pno % 2 == 0) {
        document.write('<tr style="background-color:ffffff">');
    } else {
        document.write('<tr class="style18">');
    }
    //document.write('<td  class=vncell height="22"  align="center">' + pno + '<\/td>' + '<td  align="center" class=vtable><input type="text" class="input" maxlength="11" size="15" name="portInfo' + pno + '"  value="' + portInfo[pno - 1] + '" onChange="checkName(this,11,\'端口名称\',1)"><div id="errI' + pno + '"></div><\/td>' + '<td   bgcolor=""align="center"><div style="' + linkStyle[pno - 1] + '">' + curSpeed[pno - 1] + '</div><\/td>');
    document.write('<td  class=vncell height="22"  align="center">' + pno + '<\/td>' + '<td  align="center" class=vtable>'+portInfo[pno - 1]+'<div id="errI' + pno + '"></div><\/td>' + '<td   bgcolor=""align="center"><div style="' + linkStyle[pno - 1] + '">' + ConnStatuss[pno - 1] + '</div><\/td>');
	/*if (pageType==2)
	{
		document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + (0x50 + pno -1).toString(16) + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>自动协商<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M(半双工)<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M(全双工)<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M(半双工)<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M(全双工)<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '>禁止<\/option>' + '<\/select><\/td>');
	}
	else if (pageType==3)
	{
		document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + (0x50 + pno -1).toString(16) + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>自动协商<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M(半双工)<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M(全双工)<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M(半双工)<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M(全双工)<\/option>' + '<option value="2" ' + modeSelected(pno, 2) + '>1000M(全双工)<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '>禁止<\/option>' + '<\/select><\/td><td  align="center" class=vtable><input class="input" type="text" name="R' + (0x10 + pno).toString(16) + '" id="R' + (0x10 + pno).toString(16) + '" style="width:50px" onKeyPress="javascript:figureOnly()" maxlength="4" onChange="checkNumber(this, 1518, 9600, \'允许最大帧\')"><div id="err' + pno + '"></div><\/td>');
	}
	else
	{*/
		//document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + (0x50 + pno -1).toString(16) + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>自动协商<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M(半双工)<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M(全双工)<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M(半双工)<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M(全双工)<\/option>' + '<option value="2" ' + modeSelected(pno, 2) + '>1000M(全双工)<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '>禁止<\/option>' + '<\/select><\/td>');
    if(maxSpeed[pno-1]==-1){
	document.write('<td  class=vtable align="center">&nbsp;&nbsp;&nbsp;&nbsp;<\/td>');
    }else if(maxSpeed[pno-1]==1000){
	document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + PortNames[pno-1] + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>Auto<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M-HD<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M-FD<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M-HD<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M-FD<\/option>' + '<option value="2" ' + modeSelected(pno, 2) + '>1000M-FD<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '><%write(com_ban);%><\/option>' + '<\/select><\/td>');
    } else if(maxSpeed[pno-1]==100){
	document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + PortNames[pno-1] + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>Auto<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M-HD<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M-FD<\/option>' + '<option value="5" ' + modeSelected(pno, 5) + '>100M-HD<\/option>' + '<option value="1" ' + modeSelected(pno, 1) + '>100M-FD<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '><%write(com_ban);%><\/option>' + '<\/select><\/td>');
    } else if(maxSpeed[pno-1]==10){
	document.write('<td  class=vtable align="center"><select class="input" size="1" name="R' + PortNames[pno-1] + '">' + '<option value="3" ' + modeSelected(pno, 3) + '>Auto<\/option>' + '<option value="4" ' + modeSelected(pno, 4) + '>10M-HD<\/option>' + '<option value="0" ' + modeSelected(pno, 0) + '>10M-FD<\/option>' + '<option value="6" ' + modeSelected(pno, 6) + '><%write(com_ban);%><\/option>' + '<\/select><\/td>');
    }


    document.write('<\/tr>');
}
document.write('<\/table>');
addLoadEvent(init);
addLoadEvent(initHelp);
</script>
<table><tr height="10"><td></td></tr><tr><td width="600" align="center">
<input type="button" value="<%write(com_save);%>" class="input-button" onClick="save()" name="">
<!--<input type="button" value="刷新" class="input-button" name="" onClick="location.reload()">-->
<input type="button" class="input-button" style="display:none" value="<%write(com_reset);%>" name="" onClick="reSet();">
<input type="button" class="input-button" value="<%write(com_help);%>" name="helpButton"  id="helpButton" onClick="window.open('rhelpinfo.htm')">
<input type="button" class="input-button" style="display:none" name="goBack" value="<%write(com_back);%>" onClick="go_back();">
</td></tr></table>
</form>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
  </td><td class="gapTd"></td></tr> </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
  </tr>
</table>
</body>
</html>
