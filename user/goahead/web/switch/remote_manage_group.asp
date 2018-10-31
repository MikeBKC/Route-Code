<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>网络管理-联动管理</title>
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
if (optType=="reboot") 
{
	aspRebootRemote();//重启（c函数）
} 
else if (optType=="reset")
{
	aspResetRemote();//恢复出厂配置（c函数）
}
else if (optType=="bind")
{
        aspBindRemote();//安全绑定（c函数）
} 
else if (optType=="setPass")
{
	aspSetPassRemote();//修改密码（c函数）
} 
else if (optType=="enablePunish")
{
	//aspEnablePunishRemote();//启动联动惩罚（c函数）
} 
else if (optType=="delPunish")
{
	//aspDelPunishRemote();//解除全部惩罚（c函数）
} 
else if (parentPage=="wait") aspOutErrorArr();//打印errorArr（c函数,每次打印过之后清空便可）
//以上每个操作后，都需打印相应的errorArr信息
%>
try
{
	if(errorArr.length>0){};//此语句是什么无关系，关键是判断errorArr是否存在。若不存在则产生异常
}
catch(e)
{
	var errorArr=new Array("");
}
var errorstr='';
//end交互
var ipStr="<%write(ipStr);%>";//远程ip字串
var macStr="<%write(macStr);%>";//远程mac字串
var passStr="<%write(passStr);%>";//远程密码字串
var parentPage="<%write(parentPage);%>";//来自的页面
var optType="<%write(optType);%>";

if(macStr=="")//用户直接打开此页面
{
	closeWin("非法操作！");
}

function init()
{
	controlWin(650,339,1);
	REMOTECONF.passStr.value = passStr;
	REMOTECONF.ipStr.value= ipStr;
	REMOTECONF.macStr.value= macStr;
	if(parentPage=="wait")showBatchResult(errorArr,ipStr,macStr,"密码验证失败信息",1);
	else if(optType!="") 
	{
		if(!showBatchResult(errorArr,ipStr,macStr,"操作失败信息",1))
		{
			alert("操作成功!");
		}
	}
}
function save(opt)
{
	if(opt == "bind")//安全绑定
	{
		if(!confirm("确定所选设备，全部进行安全绑定?")) return;
	}
	else if(opt == "reboot")//重启设备
	{
		if(!confirm("确定重启所选设备?（设备重启约需1分钟后，才可再次工作。）")) return;
	}
	else if(opt == "reset")//恢复出厂配置
	{
		if(!confirm("确定恢复所选设备出厂配置?（注：强烈建议恢复前，先进行配置保存。恢复后，请立即重启设备）")) return;
	}
/*	else if(opt == "getEventslog")//获取日志
	{
		controlWin(750,470,1);
		REMOTECONF.action="wait_remote_conf.asp";
	}
*/	
	else if(opt == "getConf")//获取配置
	{
		controlWin(750,505,1);
		REMOTECONF.action="wait_remote_conf.asp";
	}
	else if(opt == "sendConf")//下发配置
	{
		controlWin(750,505,1);
		REMOTECONF.action="linkageConfSend.asp";
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
	else if(opt == "setPass")
	{
		if(!checkPassword(REMOTECONF.pw1, REMOTECONF.pw2, 11, 1))
		{
			return;
		}
		else
		{
			if(!confirm("确定修改所选设备密码？")) return;
		}
	}
	REMOTECONF.optType.value=opt;//后台根据该值确定是什么操作
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
 <td><script language="javascript">showSecondTitle('网络管理-联动管理');//显示页面名称</script>
  <table width="100%" border="0" height="200" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'><tr><td  class="gapTd"></td><td align="center">
    <!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br><br>
    <form  method="post" name="REMOTECONF" target="_self">
    <input type="hidden" value="Apply" name="_submit">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="optType">
    <input type="hidden" name="isGroupOpt" value="1">
    <table width="500" border="0"  cellpadding="4" cellspacing="1" class="style22">
    <tr class="style18">
    <td colspan="5" align="left"><b>批量管理</b></td>
    </tr>
    <tr>
    <td align="center"><a href="javascript:save('bind')">安全绑定</a></td>
    <td align="center"><a href="javascript:save('reboot')">重启设备</a></td>
    <td align="center"><a href="javascript:save('reset')">恢复出厂配置</a></td>
    <td align="center"><a href="javascript:save('getConf')">获取配置</a></td>
    <td align="center"><a href="javascript:save('sendConf')">下发配置</a></td>
    <!--
    <td align="center"><a href="javascript:save('getEventslog')">获取日志</a></td>
    -->
    </tr>
        <tr>
      <td height="25" colspan="6">密码：
       <input type="password" class="input" name="pw1" size="16" maxlength="11" value="" onChange="checkPassword1(this,11,1);" id="pw1">
        密码确认：
        <input type="password" class="input" name="pw2" size="16" maxlength="11" value="" onChange="checkPassword(REMOTECONF.pw1,this,11,1)" id="pw2">
        <label>
          <input type="button" name="button" id="button"  class="input-button" value="修改密码" onClick="save('setPass')">
        </label></td>
</tr>
        <tr style="display:none">
          <td height="25" colspan="6">启用联动惩罚
            <input type="checkbox" value="1" name="punishEnable" class="input-checkbox">
            <input type="button" class="input-button" value="保存" onClick="save('enablePunish')" id="uttsave">
            <input type="submit" name="button2" id="button2"  class="input-button" onClick="save('delPunish')"  value="解除全部惩罚"></td>
</tr>
    </table>
    </form>
    <br><br>
   <!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
   </td><td class="gapTd"></td> </tr></table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<td width="15"></td>
  </tr>
</table>
</body>
</html>
