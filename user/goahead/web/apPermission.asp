<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("apPermission");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互begin
//var name = new Array("default", "test");
//var noBegin = new Array("", "13735937");
//var noEnd = new Array("", "59753987");
//var actions = new Array("0", "0");
//var totalrecs = 2;
//var errorstr = "";

//交互end
<% 
	aspOutPutApPermission();
%>
var flag = 0;
recsPerPage[0] =5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=24;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("AP<%write(power);%>");
var titles1=new Array();
titleindex=0;
titles1[0]=_("<%write(name);%>");
titles1[1]=_("<%write(beginSN);%>");
titles1[2]=_("<%write(endSN);%>");
titles1[3]=_("<%write(action);%>");

arrayData1=new Array(names, noBegin, noEnd, actions);

sortable[0]=new Array(1,1,1,1);
sortType[0]=new Array(TYPE_STR, TYPE_NUM, TYPE_NUM, TYPE_CHINESE);
arrayCount[0]=4;
function dsplist(index,list)
{
	if(index==0) dspList(index,list,-1,titles1,titles1.length,arrayData1,1,1);
}

function init()
{
	var i;
	for (i = 0; i < totalrecs; i++)
	{
		if (actions[i] == "0")
		{
			actions[i] = _("<%write(forbid);%>");
		}
		else
		{
			actions[i] = _("<%write(allow);%>");
		}
	}
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
}
var w=0;
function EditConfig(val,i){
	if(val==0)	//添加
	{ 
		if(eval(maxrecs[0]-totalrecs)<1)
		{
			alert(_("<%write(warning7);%>"));
			return;
		}
		document.getElementById('add').value = 1;
		document.getElementById('beginNo').value = "";
	}
	else	//修改
	{
		if (i == 0)	//默认
		{
			if (actions[0] == _("<%write(allow);%>"))	//允许
			{
				document.getElementById('Defdeny').checked = false;
				document.getElementById('DefAllow').checked = true;
			}
			else	//禁止
			{
				document.getElementById('DefAllow').checked = false;
				document.getElementById('Defdeny').checked = true;
			}
		}
		else	//非默认
		{
			w=i;
			if (actions[i] == _("<%write(allow);%>"))	//允许
			{
				document.getElementById('nonDefdeny').checked = false;
				document.getElementById('nonDefAllow').checked = true;
			}
			else	//禁止
			{
				document.getElementById('nonDefAllow').checked = false;
				document.getElementById('nonDefdeny').checked = true;
			}
			document.getElementById('serialNoBegin').value = noBegin[i];
			document.getElementById('serialNoEnd').value = noEnd[i];
			document.getElementById('name').value = names[i];
			document.getElementById('add').value = 0;
			document.getElementById('beginNo').value = noBegin[i];
		}
	}
	if (i == 0)
	{
		flag = 1;
		document.getElementById("ap_permission").style.display = "none";
		document.getElementById("custom").style.display = "none";
		document.getElementById("default").style.display = "block";
	}
	else
	{
		flag = 2;
		document.getElementById("ap_permission").style.display = "none";
		document.getElementById("custom").style.display = "block";
		document.getElementById("default").style.display = "none";
	}
}

function checkData()
{
	var f=nonDefaults;
	if (parseInt(document.getElementById('serialNoBegin').value) - parseInt(document.getElementById('serialNoEnd').value) > 0)
	{
		alert(_("<%write(warning6);%>"));
		return false;
	}
	if (!checkNumber(f.serialNoBegin,1,99999999,_("<%write(serialNum);%>"),0)) return false;	
	if (!checkNumber(f.serialNoEnd,1,99999999,_("<%write(serialNum);%>"),0)) return false;	
	if (!checkName(f.name,11,_("<%write(designation);%>"),1)) return false;	//检查用户名：不能为空、不能全为0、不能超11个字符、应为除:< > , % \' \\ \" & ; 和空格之外的字符。
	return true;
}

function save()
{
	if (flag == 1)	//默认
	{
		var f = defaults;
		f.action = "/goform/formApPermissionDefault";
		f.submit();	
	}
	else	//非默认
	{
		var f = nonDefaults;
		if (checkData())
		{
			f.action = "/goform/formApPermissionConfig";
			f.submit();	
		}
	}
}

function DeleteConfig(index){
	if (index == 0)
	{
		alert(_("<%write(warning5);%>"));
		return;
	}
	if(confirm(_("<%write(warning3);%>"))){
		AP_Permission.delstr.value=noBegin[index];
		AP_Permission.action="/goform/formApListStaticDel";
	    AP_Permission.submit();
	}
}

function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		AP_Permission.action="/goform/formApListStaticDelAll";
		AP_Permission.submit();
	}
}

function customDel()
{
	genData(AP_Permission,0,frame12,noBegin,"<%write(APpermissions);%>");
	AP_Permission.action="/goform/formApListStaticDel";
	AP_Permission.submit();
}

function Reset()
{
	if (flag == 1)
	{
		defaults.reset();
		EditConfig(1,0);
	}
	else
	{
		nonDefaults.reset();
		EditConfig(1,w);
	}
}

</script>
</head>
<body onLoad="init();" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(APAdmin);%>"));//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg" >
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						3,
						new Array(_("AP<%write(list);%>"),'apManage.asp',1,0),
						new Array(_("AP<%write(power);%>"),'apPermission.asp',1,1),
						new Array(_("AP<%write(scan);%><%write(method);%>"),'apDiscover.asp',1,0)
//						new Array(_("APMngRcd"),'apMngRecord.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<form name="AP_Permission" method="POST" action="">
<div id="ap_permission" style="display:block">
<input type="hidden" name="delstr">
<br>
<table  border="0" width="600" align="center" cellpadding="0" cellspacing="0">
<tr>
<td colspan="6" height="" align="center" valign="bottom">
<iframe src="_listhead1.htm" scrolling="no" name="frame11" valign="bottom" align="center" width="600" height="45" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="6" align="center" valign="top">
<iframe src="_list1.htm" name="frame12" scrolling="no" id="framelist" valign="top" align="MIDDLE" width="600" height="150" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
  <td width="4%"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
<td width="47%" height="21" id="all"> <%write(allSelect);%> / <%write(allUnselect);%></td>
<td width="49%"  align="right">	   
<input type="button" class="input-button" id="addnewcase" onClick="EditConfig(0)" value="<%write(addNew);%>">
<input type="button" class="input-button" onClick="delAll()" id="delAllEntry" value="<%write(delAll);%>">
<input type="button" class="input-button" id="delButton" onClick='customDel();' value="<%write(del);%>">
<!--
<input type="submit" class="input-button" id="delButton" onClick='return(genData(AP_Permission,0,frame12,name,"AP权限"));' value="删除">
-->
<input type="button" class="input-button" id="backButton" style="display:none" name="goBack" value="<%write(back);%>" onClick="go_back();"></td>
</tr></table>
</div>
</form>
<form method="post" name="nonDefaults" action="">
<div id="custom" style="display:none">
<input type="hidden" name="add" id="add">
<input type="hidden" name="beginNo" id="beginNo">
<br>
<br>
<table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="231" align="right" id="power"><%write(permissions);%> &nbsp;&nbsp;</td>
    <td width="2%" align="center"><img src="images/qt.gif" width="7" height="9"></td>
    <td width="5%" align="right"><input name="nonDefPms" type="radio" id="nonDefAllow" value="allow" checked></td>
    <td width="10%" align="left" id="allow"><%write(allow);%></td>
    <td width="5%" align="right"><input type="radio" name="nonDefPms" id="nonDefdeny" value="deny"></td>
    <td width="289" align="left" id="ban"><%write(forbid);%></td>
    </tr>
  <tr>
    <td align="right">&nbsp;</td>
    <td align="right">&nbsp;</td>
    <td colspan="4" align="left">&nbsp;</td>
  </tr>
  <tr>
    <td align="right" id="serialNum"><%write(serialNum);%> &nbsp;&nbsp;</td>
    <td width="2%" align="center"><img src="images/qt.gif" width="7" height="9"></td>
    <td colspan="4" align="left"><input type="text" name="serialNoBegin" id="serialNoBegin" size="10">
      ～
      <input type="text" name="serialNoEnd" id="serialNoEnd" size="10"></td>
    </tr>
  <tr>
    <td align="right">&nbsp;</td>
    <td align="right">&nbsp;</td>
    <td colspan="4" align="left">&nbsp;</td>
  </tr>
  <tr>
    <td align="right" id="designation"><%write(name);%> &nbsp;&nbsp;</td>
    <td width="2%" align="center">&nbsp;</td>
    <td colspan="3" align="left"><input type="text" name="names" id="name" size="18" maxlength="11" onChange=''></td>
    <td id="fixNameTd"></td>
    </tr>
  </table>
<br>
<br>
<table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>"  onClick="save()" id="uttsave">&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="Reset()" id="uttreset">&nbsp;<input type="button" class="input-button" value="<%write(help);%>" onClick="window.open('<% write(com_helppage); %>#apPermission')" id="utthelp">&nbsp;<input type="button" class="input-button" value="<%write(back);%>" onClick="GoBack_Page('apPermission.asp')" id="goback"></td>
  </tr>
</table>
</div>
</form>
<form method="post" name="defaults" action="">
<div id="default" style="display:none">
<br>
<br>
<table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="265" align="right" id="defPower"><%write(dfaultPermissions);%></td>
    <td width="38" align="right"><input name="DefPms" type="radio" id="DefAllow" value="allow" checked></td>
    <td width="28" id="allow"><%write(allow);%></td>
    <td width="30" align="right"><input type="radio" name="DefPms" id="Defdeny" value="deny"></td>
    <td width="239" id="ban"><%write(forbid);%></td>
  </tr>
</table>
<br>
<table width="600" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>"  onClick="save()" id="uttsave">&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="Reset()" id="uttreset">&nbsp;<input type="button" class="input-button" value="<%write(help);%>" onClick="window.open('<% write(com_helppage); %>#apPermission')" id="utthelp">&nbsp;<input type="button" class="input-button" value="<%write(back);%>" onClick="GoBack_Page('apPermission.asp')" id="goback"></td>
  </tr>
</table>
</div>
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
