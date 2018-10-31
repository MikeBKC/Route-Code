<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("pushSoft");%>
<%langTran("commonWords");%>
<title><%write(UnitOutSoft);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //交互begin
<% 
	var waitOver=Form("waitOver");
	var isGroupOpt=Form("isGroupOpt");
	var devType=Form("devType");
	aspOutRemoteAPSoft();//打印软件信息，做过滤，只显示出适合该AP的条目
	/*
	write('var softwareName = new Array("123.bin", "321.bin");\n');
	write('var softwareVersion = new Array("V15", "V16");\n');
	write('var typesForFile = new Array("510W", "WA1800N");\n');
	write('var firmwareVersion = new Array("V1.0", "V2.0");\n');
	write('var fileSize = Array("1.5MB", "3.0MB");\n');
	write('var totalrecs = 2;\n');
	write('var macStr = "0022aaaedd34";\n');
	write('var ipStr = "200.200.202.125";\n');
	write('var passStr = "admin";\n');
	write('var errorArr = new Array("");\n');
	write('var errorstr="";\n');
	*/
%>
var isGroupOpt="<%write(isGroupOpt);%>";
var waitOver="<%write(waitOver);%>";
var devType="<%write(devType);%>";
//end交互

var urcpError="";
var radarray=new Array();
for(var i=0;i<totalrecs;i++)
{
	radarray[i]='<input type="radio" name="radConfSrc" value="'+softwareName[i]+' '+typesForFile[i]+'">';
}

var titles1 = new Array();
titles1[0] = _("<%write(chooseSoft1);%>");
titles1[1] = _("<%write(softName);%>");
titles1[2] = _("<%write(softVersion);%>");
titles1[3] = _("<%write(suitType);%>");
titles1[4] = _("<%write(hardVersion);%>");
titles1[5] = _("<%write(softSize);%>");

currpage[0] = 1;	//当前页
recscount[0] = totalrecs;	//当前条目数
maxrecs[0] = 24;	//最大条目数
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(outSoft);%>");
sortable[0] = new Array(0, 1, 1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_STR, TYPE_NUM);
arrayCount[0] = 6;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 0;
var arrayData1 = new Array(radarray, softwareName, softwareVersion, typesForFile, firmwareVersion, fileSize);
var oldDspList=dspList;

/**
重写表格显示dspList函数
为其增加新的内容
**/
dspList=function()
{
	/*原有内容*/
	oldDspList.apply(this,arguments);//等价于oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
	//默认选中第一个
	if(totalrecs>0)
	{
		frame12.document.getElementById('tableList').rows[1].cells[0].childNodes[0].checked = true;
	}
}

function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 0, 0);
}
function init() {
	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
	frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	pushSoft.passStr.value = passStr;
	pushSoft.ipStr.value= ipStr;
	pushSoft.macStr.value= macStr;
	pushSoft.szName.value= szName;
	pushSoft.managedEn.value= managedEn;
	pushSoft.isGroupOpt.value= isGroupOpt;
	if(!isGroupOpt)//单机管理
	{
		window.document.title = _("<%write(UnitOutSoft);%>");
		window.document.title+=" / "+ipStr;
		if(waitOver != "" && urcpError=="") alert(_("<%write(lawfull);%>"));//用户无法直接从界面上看到结果，过需提示
	}
	else if(waitOver != "")
	{
		if(!showBatchResult(errorArr,ipStr,macStr,_("<%write(failTips);%>"),1))
		{
			alert(_("<%write(lawfull);%>"));
		}
	}
}
function init2() {
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	pushSoft.passStr.value = passStr;
	pushSoft.ipStr.value= ipStr;
	pushSoft.macStr.value= macStr;
	pushSoft.szName.value= szName;
	pushSoft.managedEn.value= managedEn;
	pushSoft.isGroupOpt.value= isGroupOpt;
}
function reSet()
{
	pushSoft.reset();
	init2();
}
function save()
{
	var obj,i;
	var tmp;
		pushSoft.remoteSrcSoft.value="";
		obj = frame12.document.getElementsByName('radConfSrc');
		for(i=0;i<obj.length;i++)
		{
			if(obj[i].checked)
			{
				tmp = obj[i].value.split(" ");
				pushSoft.remoteSrcSoft.value = tmp[0];//用此隐藏控件记录联动源配置文件名
				pushSoft.web_type.value = devType;
			}
		}
		if(!confirm(_("<%write(continueTips);%>")))
		{
			return;
		}
	if (!softwareName[0])
	{
		alert(_("<%write(chooseFile);%>"));
		return;
	}
//	alert(pushSoft.web_type.value);
//	return;
	pushSoft.action="ap_wait_remote_conf.asp";
	pushSoft.submit();
}
function go_back()
{
	if(isGroupOpt)
		pushSoft.action="ap_remote_manage_group.asp";
	else
		pushSoft.action="ap_remote_manage.asp";
	pushSoft.devType.value= devType;
	pushSoft.optType.value="";//清空，防止返回的页面误操作
	pushSoft.submit();
}
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">
	urcpErrorPost("ap_remote_manage.asp");
</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(outSoft);%>"));//显示页面名称</script>
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
	<table border="0" width="80%" align='center'>	<tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="pushSoft" method="post">
<input type="hidden" name="delstr" value="">
<input type="hidden" name="statustr">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="oldName">
<input type="hidden" value="" name="remoteSrcSoft">
<input type="hidden" value="sendAPSoft" name="optType">
<input type="hidden" value="" name="web_type">
<input type="hidden" value="" name="devType">
<TABLE  width="600"  border="0"  cellpadding="0" cellspacing="0"   align="center">
<tr><td colspan="2" height="10"></td></tr>
</TABLE>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="600" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td height="" colspan="2" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	
</td></tr>
<tr>
<td colspan="2" align="center" valign="top">
<iframe scrolling="no" name="frame12" id="framelist" valign="top" align="center" width="600" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
<tr><td>
</td>
</tr>
</table>
<table width="600" align="center"  cellpadding="0" cellspacing="0">
<tr>
  <td height="21" align="center" valign="top" nowrap id="">
    <input type="button" name="button" id="outbutton" value="<%write(sureout);%>" onClick="save()" class="input-button">
    <input name=""  type="button" value="<%write(refill);%>" class="input-button" onClick="reSet()" id="resetButton">
    <input type="button" class="input-button" value="<%write(help);%>" name="input" onClick="window.open('<% write(com_helppage); %>#pushSoft')" id="helpButton">
    <input type="button" class="input-button" name="goBack" value="<%write(back);%>" onClick="go_back();" id="backButton">
  </td>
  </tr>
</table>
</tr>
</table>
</form>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
 </td><td class="gapTd"></td></tr> </table> </td>
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
