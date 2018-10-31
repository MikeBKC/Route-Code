<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("common"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
Butterlate.setTextDomain("internet");

var strSecondMenuFocus="L2TP服务器";

function initTranslation()
{
}
var errorstr = "";

<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name != "") OutputL2tpSrvAccountData(edit_Name);//需要修改
%>
var edit_Names = "<% write(edit_Name);%>";//需要修改的账号名

function checkData(){
	var f=l2tpSrvAccountConfig;
	if (!checkName(f.instName, 12,"隧道名称",0)) return false;
	if (!checkName(f.userName,32,"用户名",0)) return false;
	if(!checkPassword1(f.passwd, 11, 0, 0)) return false;	//检查密码：不能超过11个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
	if (f.userType.selectedIndex == 0) {
		if (!checkIP(f.remoteLanIp,"远端内网IP地址",0)) return false;
		if (!checkMask(f.remoteLanNetmask,"远端内网子网掩码",0)) return false;
	}
	if(edit_Names=="" || edit_Names == null){ 
			/*if(eval(max_totalrecs-totalrecs)<1)	{
			alert(_("PPTP账号已经达到最大个数,不能添加新的PPTP账号"));
			GoBack_Page("PPTPClientList.asp");
			return;
		}*/
	}
	return true;
}
function save(){
	if(checkData()){
		var f=l2tpSrvAccountConfig;
		f.instName.disabled = false;
		l2tpSrvAccountConfig.action="/goform/l2tpSrvAccountConfig";
		l2tpSrvAccountConfig.submit();
	}	
}


function init(){
	var f=l2tpSrvAccountConfig;
	if((edit_Names!="") && (edit_Names!=null)){ 
		f.instName.value = instNames;
		f.userName.value = userNames;
		f.passwd.value = passwds;
		f.remoteLanIp.value = remoteLanIps;
		f.remoteLanNetmask.value = remoteNetmasks;		
		f.Action.value="modify";
		f.userType.selectedIndex = getSelIndexByValue(f.userType, peerTypes);
		f.instName.disabled = true;
	}else{
		f.instName.value = "";
		f.userName.value = "";
		f.passwd.value = "";
		f.remoteLanIp.value = "";
		f.remoteLanNetmask.value = "";		
		f.Action.value="add";
		f.userType.selectedIndex = 0;
	}
	utypeChanged(f.userType)
}
function Reset(){
	l2tpSrvAccountConfig.reset();
	for(i=1;i<=5;i++)
		document.all["yutd"+i].innerHTML="";
	init();	
}


function utypeChanged(arg) {

	if (getListValue(arg) == "lantolan") {
		showTable(remoteLanIpTr, 1);
		showTable(remoteLanNetmaskTr, 1);
	} else {
		showTable(remoteLanIpTr, 0);
		showTable(remoteLanNetmaskTr, 0);	
	}
}
//-->
</script>
</head>
<body onLoad="initTranslation()" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td> 
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("L2TP服务器");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"80%",
						2,

						new Array("全局配置",'L2TPSrvGlobal.asp',1,0),
						new Array("账号配置",'L2TPSrvAccount.asp',1,1)
		);
		showThreeMenu();
	</script>
  <table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
     <td align="center" class="three_td" valign="top"><table width="800" border="0" cellspacing="0" cellpadding="0">

<form  name="l2tpSrvAccountConfig" method="post">
<p>&nbsp;</p>

<tr>
    <td align="center"  >

	
  <table border="0" width="800" align='center'>
	   <tr>
	   <td align="right"  height="25" width="40%" id="instNameTd"><input type="hidden" name="Action" value="add">隧道名称</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="instName" size="18"  maxlength="31"   value="" onChange="showfiledtip(re_checkName(this,'隧道名称'),yutd3)"></td><td width="34%"  id="yutd3"></td>
	   </tr><input type="hidden" name="oldSetName" >
  <tr>
	   <td align="right"  height="25" width="40%" id="userTypeTd">用户类型</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><select name="userType" onChange="utypeChanged(this)">
	   <option value="lantolan">LAN到LAN</option>
	   <option value="mobile">移动用户</option>
	   </select></td>
	   </tr>
  <tr>
	   <td align="right"  height="25" width="40%" id="userNameTd">用户名</td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="userName" size="18"  maxlength="32" onChange="showfiledtip(re_checkName(this,'用户名'),yutd1)"  value=""></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="passwdTd">密码</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input  type="password" name="passwd" size="18"   maxlength="11" onChange="showfiledtip(re_checkPassword1(this, 11, 0),yutd2)" value=""></td><td  id="yutd2"></td>
	   </tr>
	    
	    <tr id="remoteLanIpTr" style="display:">
	   <td align="right"  height="25" id="remoteLanIpTd">远端内网网络地址</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="remoteLanIp" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'远端内网IP地址'),yutd4)"  value=""></td><td  id="yutd4"></td>
	   </tr>
	    <tr id="remoteLanNetmaskTr" style="display:">
	   <td align="right"  height="25" id="remoteLanNetmaskTd">远端内网子网掩码</td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" name="remoteLanNetmask" size="18" onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'远端内网子网掩码'),yutd5)"  value=""></td><td  id="yutd5"></td>
	   </tr>
	   <tr><td colspan="4" height="20">&nbsp;</td></tr>
	  <tr><td  colspan="4">
  <table width="29%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="right" valign="top"><input type="button" class="input-button" value="保存" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="重填" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="返回" onClick="GoBack_Page('L2TPList.asp')" id="returnback"></td>
</tr>
</table>
  </td>
	
  </tr>
  <tr><td colspan="4" align="center" height="30">&nbsp;</td></tr>

  </table></td>
  </tr>

  <script language="javascript">init();</script>
</form>

</table>
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table></td></tr></table>
</body>
</html>
