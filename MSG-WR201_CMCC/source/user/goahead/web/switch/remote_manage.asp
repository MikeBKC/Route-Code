<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>联动管理-联动管理</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%
var parentPage=Form("parentPage") ;//来自哪个页面
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
var passStr=Form("passStr");//远程密码字串
var optType=Form("optType");//操作类型
if (parentPage=="discover")
{
	aspSignalRemoteAuth();//远程密码验证。可与批量密码验证公用
}
else if (parentPage=="topoy")
{
        aspTopoInfo();//拓扑页面输入密码后，需发送密码验证及读取软件版本等信息
}
else if (optType=="reboot") 
{
	aspRebootRemote();//重启（c函数）
} 
else if (optType=="reset")
{
	aspResetRemote();//恢复出厂配置（c函数）
}
else if (optType=="enablePunish")
{
	//aspEnablePunishRemote();//启动联动惩罚（c函数）
} 
else if (optType=="delPunish")
{
	//aspDelPunishRemote();//解除全部惩罚（c函数）
} 
aspOutSysInfoRemote();//打印设备信息（c函数）
%>
//var errorArr = new Array("","","","");//放在每个操作函数里面打印
var ipStr="<%write(ipStr);%>";//远程ip字串
var macStr="<%write(macStr);%>";//远程mac字串
var passStr="<%write(passStr);%>";//远程密码字串
var parentPage="<%write(parentPage);%>";//来自的页面
var optType="<%write(optType);%>";
//end交互
var errorstr="";
try
{
	errorstr=errorArr[0];
}
catch(e)
{//页面未打印errorArr[0]信息
	errorstr="";
}
if(macStr=="")//用户直接打开此页面
{
	closeWin("非法操作！");
}
if(parentPage=="discover" || parentPage=="topoy")
{
	if(trimstr(errorstr)!="")
	{
		closeWin(errorstr);
	}
}
else if(optType!='')
{
	if(errorstr.indexOf("密码错误")>=0)
	{
		closeWin(errorstr);
	}
}
if(optType!='')//是本页面操作后跳过来的(重启 或 恢复出厂配置)
{
	if(errorstr=='')
	alert("操作成功！"); 
}
var secondTitle='联动管理';
var MAC='';
for(var i=0;i<macStr.length;i+=2) MAC=MAC+macStr.slice(i,i+2)+'-';
MAC=MAC.slice(0,-1);
function init()
{  
    controlWin(760,530,1);
	REMOTECONF.passStr.value = passStr;
	REMOTECONF.ipStr.value= ipStr;
	REMOTECONF.macStr.value= macStr;
	window.document.title+=" / "+ipStr;
}
function save(opt)
{
	var page="remote_manage.asp";
	if(opt == "reboot")//重启设备
	{
		if(!confirm("确定重启设备?（设备重启约需1分钟后，才可再次工作。）")) return;
	}
	else if(opt == "reset")//恢复出厂配置
	{
		if(!confirm("确定恢复出厂配置?（注：强烈建议恢复前，先进行配置保存。恢复后，请立即重启设备。）")) return;
	}
	else if(opt == "enablePunish")//启用联动惩罚
	{
		if(!REMOTECONF.punishEnable.checked)
		{
			if(!confirm("关闭联动惩罚，当前所有被禁止的主机将自动解禁。确定继续?")) return;
		}
		else 
		{
			if(!confirm("开启联动惩罚，将禁止非法主机上网。确定继续?")) return;
		}
	}
	else if(opt == "delPunish")//解除全部惩罚
	{
		if(!confirm("确定解除全部联动惩罚？\n注意：只解除本次惩罚，若被解禁的主机再次有非法行为，将依然被禁止。")) 	
		  return;
	}
    else if(opt =="getConf" || opt =="getEventslog")
	{
		page = "wait_remote_conf.asp" ;//直接打开页面	
	}
	else
	{
		page = opt ;//直接打开页面
		opt = "";//后无需知道是什么操作
	}
	REMOTECONF.optType.value=opt;//后台根据该值确定是什么操作
	REMOTECONF.action=page;
	REMOTECONF.submit();
}

</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="../images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
 <td><script language="javascript">showSecondTitle(secondTitle);//显示页面名称</script>
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
    <form name="REMOTECONF" id="REMOTECONF" method="post">
    <input type="hidden" value="Apply" name="_submit">
    <input type="hidden" value="" name="optType">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
     <table width="600" cellpadding="0" cellspacing="0">
    <tr><td width="273"><b>远程设备信息</b></td>
    </tr>
    <tr height="8"><td></td></tr>
    </table>
    <TABLE  cellSpacing=1  width="600" cellPadding=4  class="style22">
      <tr class="style18">
        <td width="68" height="27" class=vncell>设备名</td>
        <td width="171"><script language="javascript">document.write(deviceName);</script></td>
        <td width="68" class=vncell>型号</td>
        <td width="254"><script language="javascript">document.write(ProductID);</script></td>
      </tr>
      <tr>
        <td class=vncell height="27">序列号</td>
        <td><script language="javascript">document.write(sysSerial);</script></td>
        <td class=vncell>IP地址</td>
        <td><script language="javascript">document.write(ipStr);</script></td>
      </tr>
      <tr class="style18">
        <td class=vncell height="27">MAC地址</td>
        <td><script language="javascript">document.write(MAC);</script></td>
        <td class=vncell height="27">软件版本</td>
        <td><script language="javascript">document.write(sysVersion);</script></td>
      </tr>
    </TABLE>
    <table width=600 border="0" cellspacing="0" cellpadding="0" align="center">
<tr height="25"><td></td></tr>
<tr height="1"><td background="../images/dot.gif"><img src="../images/space.gif" width="8" height="1"></td></tr>
<tr height="18"><td></td></tr>
</table>
    <table width="600" cellpadding="0" cellspacing="0">
    <tr>
      <td width="273"><b>远程设备管理</b></td>
      <td width="325" align="right"></td>
    </tr>
    <tr height="8"><td colspan="2"></td></tr>
    </table>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr class="style18">
    <td colspan="5" align="left"><b>端口管理</b></td>
    </tr>
    <tr>
    <td width="190"><a href="javascript:save('ports.asp')">端口管理</a></td>
    <td width="191"><a href="javascript:save('port_vlan.asp')">端口VLAN</a></td>
    <td width="191"><a href="javascript:save('aggregation.asp')">端口汇聚</a></td>
    </tr>
    </table>
    <br>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr>
      <td colspan="6" align="left" class="style18"><b>网络安全管理</b></td>
    </tr>
    <tr>
      <td width="190"><a href="javascript:save('mirror.asp')">端口镜像</a></td>
    </tr>
    <tr style="display:none">
      <td colspan="3">启用联动惩罚
      <input type="checkbox" value="1" name="punishEnable" class="input-checkbox">
      <input type="button" class="input-button" value="保存" onClick="save('enablePunish')" id="uttsave">
      <input type="submit" name="button2" id="button2"  class="input-button" onClick="save('delPunish')"  value="解除全部惩罚">		     </td>
</tr>
    </table>
    <br>
    <table width="600" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr>
      <td colspan="6" align="left" class="style18"><b>系统管理</b></td>
      </tr>
      <tr>
       <td width="86"><a href="javascript:save('sysInfo.asp')">系统信息</a></td>
       <td width="82"><a href="javascript:save('sysConf.asp')">系统设置</a></td>
      <td width="69"><a href="javascript:save('reboot')">重启设备</a></td>
      <td width="92"><a href="javascript:save('reset')">恢复出厂配置</a></td>
      <td width="78"><a href="javascript:save('getConf')">获取配置</a></td>
      <td width="138"><a href="javascript:save('linkageConfSend.asp')">下发配置</a></td>
</tr>
</table>   
    </form>               
    <!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
	  </td><td class="gapTd"></td> </tr></table>
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
