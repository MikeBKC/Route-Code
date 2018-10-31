<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("ap_remote_manage_group");%>
<%langTran("commonWords");%>
<title><%write(APCentrMng);%>-<%write(patchOperate);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互
<%
var parentPage=Form("parentPage") ;//来自哪个页面
var ipStr=Form("ipStr");//远程ip字串
var macStr=Form("macStr");//远程mac字串
var passStr=Form("passStr");//远程密码字串
var optType=Form("optType");//操作类型
var serialNo=Form("serialNo");
var devType=Form("devType");
var szName=Form("szName");
var managedEn=Form("managedEn");
if (optType=="reboot") 
{
	aspRebootRemote();//重启（c函数）
} 
else if (optType=="Factory")
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
else if (optType=="Chn")
{
    aspChangeChannel(); //批量修改信道
}
else if (parentPage=="wait") aspOutErrorArr();//打印errorArr（c函数,每次打印过之后清空便可）
//以上每个操作后，都需打印相应的errorArr信息
%>

var passerr_flag = 0;
try
{
	if(errorArr.length>0){
		for (var i = 0; i < errorArr.length; i++)
		{
			if (errorArr[i] == "密码错误")
			{
				passerr_flag = 1;
			}
		}
	};//此语句是什么无关系，关键是判断errorArr是否存在。若不存在则产生异常
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
var serialNo="<%write(serialNo);%>";
var devType="<%write(devType);%>";
var szName="<%write(szName);%>";
var managedEn="<%write(managedEn);%>";

if(macStr=="")//用户直接打开此页面
{
	closeWin("<%write(lawless);%>");
}
function init()
{
	controlWin(750,500,1);
	REMOTECONF.passStr.value = passStr;
	REMOTECONF.ipStr.value= ipStr;
	REMOTECONF.macStr.value= macStr;
	REMOTECONF.serialNo.value= serialNo;
	REMOTECONF.devType.value= devType;
	REMOTECONF.szName.value= szName;
	REMOTECONF.managedEn.value= managedEn;
	
	if(parentPage=="wait")
	{
		if ((passerr_flag == 1) && (passStr.indexOf("Padmin") >= 0 ))
		{
			document.getElementById("DivS").style.display="";
			document.getElementById("tableInfo").style.display="none";
			for (var i = 0; i < errorArr.length; i++)
			{
				/*原先显示密码输入框和错误提示框，现更改为隐藏提示的窗口，只显示密码输入框*/
				errorArr[i] = "";
			}
		}
		showBatchResult(errorArr,ipStr,macStr,"<%write(pwAuthFailMG);%>",1);
	}
	else if(optType!="") 
	{
        if(optType == "setPass"){
	        closeWin("<%write(afterSetPass);%>");
        }
		else if(!showBatchResult(errorArr,ipStr,macStr,"<%write(failTips);%>",1))
		{
			alert("<%write(lawfull);%>");
		}
		document.getElementById("DivS").style.display="none";
		document.getElementById("tableInfo").style.display="";
	}
}
function save(opt)
{
	if(opt == "SZ")
	{
//		alert("批量下发服务区");
		REMOTECONF.action="serviceZone.asp";
	}
	else if(opt == "Conf")
	{
//		alert("批量下发配置");
        if( devType == "" ){
            alert("<%write(warning24);%>");
            return;
        }
		REMOTECONF.action="pushConf.asp";
	}
	else if(opt == "SW")
	{
//		alert("批量升级软件");
        if( devType == "" ){
            alert("<%write(warning24);%>");
            return;
        }
		REMOTECONF.action="pushSoft.asp";
	}
	else if(opt == "TMP")
	{
	    /*
		alert("批量下发模板");
	    if( devType == "" )
	    {
		alert("<%write(warning24);%>");
		return;
	    }
	    */
	    REMOTECONF.action="pushTmp.asp";
	}
	else if(opt == "reboot")
	{
//		alert("批量重启");
		if(!confirm("<%write(rebootDevTips);%>")) return;
	}
	else if(opt == "Factory")
	{
//		alert("批量恢复出厂设置");
		if(!confirm("<%write(reConfigTips);%>")) return;
	}
	else if(opt == "Chn")
	{
//		alert("批量修改信道");
	}
	else if(opt == "setPass")
	{
//		alert("批量修改密码");
		if(!checkPassword(REMOTECONF.pw1, REMOTECONF.pw2, 11, 0))
		{
			return;
		}
		else
		{
			if(!confirm("<%write(sureDevPW);%>")) return;
		}

	}else if(opt == "apMacFilter")
	{
		REMOTECONF.action="pushApMacFilter.asp";
	}
	
	REMOTECONF.optType.value=opt;//后台根据该值确定是什么操作
//	alert(REMOTECONF.serialNo.value);
//	return;
	REMOTECONF.submit();
}

function manageAPOk(){
	var page="ap_wait_remote_auth.asp";
	var str = '';
	
	REMOTECONF.optType.value="";//后台根据该值确定是什么操作
	for (var i = 0; i < errorArr.length; i++)
	{
		str=str+'P'+ document.getElementById("getPassword").value +' ';//P为设备获取密码标志位。因strtok无法区分连续的分隔符
	}
	REMOTECONF.passStr.value = str;
	REMOTECONF.ipStr.value=ipStr;
	REMOTECONF.macStr.value=macStr;
	REMOTECONF.devType.value=devType;
	REMOTECONF.szName.value= szName;
	REMOTECONF.managedEn.value= managedEn;
	REMOTECONF.action=page;

	REMOTECONF.submit();	
}

/**取消按钮**/
function cancel(){
	//	document.getElementById("DivS").style.display="none";
    if (navigator.userAgent.indexOf("MSIE") > 0) {
        if (navigator.userAgent.indexOf("MSIE 6.0") > 0) {
            window.opener = null;
            window.close();
        } else {
            window.open('', '_top');
            window.top.close();
        }
    }
    else if (navigator.userAgent.indexOf("Firefox") > 0) {
        window.location.href = 'about:blank ';
    } else {
        window.opener = null;
        window.open('', '_self', '');
        window.close();
    }
}

</script>
</head>
<body onLoad="init();" onbeforeunload="logOut()"  style="background:url(images/body_main.png);  background-repeat:repeat-x">

<div id="DivS" style="display:none; position: absolute; z-index: 9999; width: 250px; height: 100px; top: 80px; left: 250px; background-color: rgb(255, 255, 255); border:1px solid #f2f2f2;font-size:12px; ">
 <div id="tbPwd">
	<div  style="background-color:#008893;padding-top:5px;padding-bottom:5px;">
		<div style="text-align:left; color:#fff;" >&nbsp;输入密码</div>
		<div style="width:50px; text-align:right;color:#fff;position:absolute;right:5px;top:5px;border:1px soild red;z-index:1000;">
			<a style=" color:#fff; text-decoration:none;" href="javascript:cancel();">x</a>
		</div>
	</div>
  
    <div style="font-size:12px;padding-top:10px;padding-bottom:10px;"><label height="31" colspan="2"  align="center">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;请输入:
      <input type="password" size="13" value=""  class="input" name="getPassword" id="getPassword"></span>&nbsp;&nbsp;<label id="errorS" style="color:red;"></label>
      <input type="hidden" id="img_id" name="img_id" value="" />
      </div>
    <div  style="font-size:12px; text-align:center;"><span colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="manageAPOk()" value="确定" />
      <input name="" type="button" class="input-button"  value="取消" onClick="cancel()" /></span></div>
</div>
</div>

<table id ="tableInfo" width="100%" style="display:" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(patchOperate);%>");</script>
  <table width="100%" border="0" height="270" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
   <td>
<form name="REMOTECONF" method="post" target="_self">
<input type="hidden" value="Apply" name="_submit">
    <input type="hidden" value="" name="ipStr">
    <input type="hidden" value="" name="macStr">
    <input type="hidden" value="" name="passStr">
    <input type="hidden" value="" name="optType">
    <input type="hidden" value="" name="serialNo">
    <input type="hidden" value="" name="devType">
    <input type="hidden" value="" name="szName">
    <input type="hidden" value="" name="managedEn">
    <input type="hidden" name="isGroupOpt" value="1">

<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
    <br><br>
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
<table width="500" align="center" border="0" cellpadding="4" cellspacing="1"  class="style22">
<tr class="style18">
	<td><% write(batchMNG);%></td>
</tr>
</table>
<table width="500" align="center" border="0" cellpadding="4" cellspacing="1"  class="style22">
	<tr>
		<td width="125"><% write(channel);%></td>
		<td width="250">
			<select name="channelSelect" id="channelSelect" class="input" style="width:60px">
  			<option value="0" id="auto"><% write(auto);%></option>
  			<option value="1">1</option>
			<option value="2">2</option>
  			<option value="3">3</option>
  			<option value="4">4</option>
  			<option value="5">5</option>
  			<option value="6">6</option>
  			<option value="7">7</option>
  			<option value="8">8</option>
  			<option value="9">9</option>
  			<option value="10">10</option>
  			<option value="11">11</option>
			</select>
		</td>
		<td width="125">
			<input type="button"  onClick="save('Chn')"class="input-button" value="<% write(modifychannel);%>" name="" id="saveChnButton">&nbsp;
		</td>
	</tr>
	<tr class="style18">
		<td><% write(newpwd);%></td>
		<td>
			<input type="password" class="input" name="pw1" size="16" maxlength="11" value="" id="pw1">
		</td>
		<td></td>
	</tr>
	<tr>
		<td><% write(surePasswd);%></td>
		<td>			
			<input type="password" class="input" name="pw2" size="16" maxlength="11" value="" id="pw2">
		</td>
		<td>
			<input type="button"  onClick="save('setPass')"class="input-button" value="<% write(modifyPw);%>" name="" id="savesetPassButton">&nbsp;
		</td>
	</tr>
</table>
<table width="500" align="center" border="0" cellpadding="4" cellspacing="1"  class="style22">
	<tr class="style18">
		<td width="123" ><a href="javascript:save('SZ')"><% write(APjoinSZ);%></a></td>
<!--		<td width="120" ><a href="javascript:save('Conf')"><% write(apsendconf);%></a></td>-->
		<td width="123" ><a href="javascript:save('SW')"><% write(updatesoft);%></a></td>
		<td width="123" ><a href="javascript:save('Factory')"><% write(reConfig);%></a></td>
		<td width="128"><a href="javascript:save('apMacFilter')"><%write(apMacFilter);%></a></td>
	</tr>
	<tr>
		<td><a href="javascript:save('reboot')"><% write(apreboot);%></a></td>
		<td><a href="javascript:save('TMP')"><% write(apConfTmp);%></a></td>
<!--        <td>&nbsp;</td>-->
        <td>&nbsp;</td>
        <td>&nbsp;</td>
	</tr>
</table>
<br>
<b>注意：<%write(apMacFilter);%>只有在软件版本是1.9.2以及以上的AP设备生效！</b>
<br>

     </tr>
    </table>
    </td>
  </tr>
</table>
</form>
</td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
