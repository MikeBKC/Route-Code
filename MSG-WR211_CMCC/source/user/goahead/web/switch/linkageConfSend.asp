<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>联动管理-下发配置</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">

 //交互
<%
var isGroupOpt=Form("isGroupOpt");
var waitOver=Form("waitOver");
aspOutRemoteConf();//打印配置文件信息
%>
var isGroupOpt="<%write(isGroupOpt);%>";
var waitOver="<%write(waitOver);%>";
var versionarray = new Array('SG3124R', 'SG3124R', 'SG3124R', 'SG3124R', 'SG3124R','SG3124R', 'SG3124R', 'SG3124R', 'SG3124R', 'SG3124R','SG3124F');
//end交互

var urcpError="";
if(!isGroupOpt && waitOver != "")
{
	urcpError = errorArr[0];//单击管理
}
if(macStr == "")
{
	closeWin("非法操作！");
}
var radarray=new Array();
for(var i=0;i<namearray.length;i++)
{
	radarray[i]='<input type="radio" name="radConfSrc" value="'+namearray[i]+' '+statuarray[i]+'">';
}
var titles1 = new Array(); //注意此处命名需为titles1(为了与列表中的变量对应)
titles1[0] = "选择此配置";
titles1[1] = "配置文件名";
titles1[2] = "创建时间";
//titles1[3] = "版本";

currpage[0] = 1;
recscount[0] = namearray.length;
maxrecs[0] = 24;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "配置文件列表";
sortable[0] = new Array(0,1, 1);
sortType[0] = new Array(TYPE_STR , TYPE_CHINESE, TYPE_STR);
arrayCount[0] = 3;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 0;
var arrayData1 = new Array(radarray,namearray,timearray);
var oldDspList=dspList;
var recbegin=0,recend=0;

function dsplisthead(index, listhead, arrayName) {
    var arrayIndex = 1;
    if (arrayName == null) {
        arrayName = "arrayData1";
    }
    if (index == 0) {
        arrayName = "arrayData1";
    }
    if (index == 1) {
        arrayIndex = 2;
        arrayName = "arrayData2";
    }

    var pages;
    var strPages;
    var strRecs;
    if (recscount[index] == 0) strPages = "0/0";
    else {
        pages = Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1; //为了避免recscount[index]的值是recsPerPage[index]的整数倍，所以recscount[index]-1，
        strPages = currpage[index].toString() + "/" + pages.toString();
    }
    strRecs = recscount[index].toString();
    out_head(listhead);
    listhead.document.write('<table width="100%" height="20"	 border="0" cellpadding="0" cellspacing="0">');

    listhead.document.write('  <tr>');
    listhead.document.write('    <td><table width="100%" height="20" border="0" cellspacing="0" cellpadding="0" bgcolor="">');
    listhead.document.write('      <tr>');
    //listhead.document.write('	 <td width="11" ></td>');
    listhead.document.write('	 <td width="226" background="../images/tablehead.gif" style="background-repeat:no-repeat"><img src="../images/space.gif" width="10" height="8"><span class="style4">' + listheadcaption[index] + '</span></td>');
    //listhead.document.write('	 <td width="26"></td>');
    listhead.document.write('	 <td   width="324"align="right"  class="style5" ><img src="../images/space.gif" width="10" height="8"></td><td width="50" align="right"  >' + strRecs + '</td>');
    listhead.document.write('      </tr>');
    listhead.document.write('    </table></td>');
    listhead.document.write('  </tr>');

    listhead.document.write('  <tr class="style18">');
    listhead.document.write('    <td height="24" colspan="4"  ><table width="600"  border="0" cellspacing="0" cellpadding="0">');
    listhead.document.write('      <tr>');
    listhead.document.write('	 <td width="10"><img src="../images/space.gif" width="10" height="8"></td>');
    listhead.document.write('	 <td width="25" valign="bottom" class="style6">' + strPages + '</td>');

    if (currpage[index] == 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks">第一页</td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.firstpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks">第一页</a></td>');
    if (currpage[index] <= 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks">上一页</td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.prepage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks">上一页</a></td>');

    if (currpage[index] * recsPerPage[index] < recscount[index]) listhead.document.write('	    <td width="40" valign="bottom"><a href="javascript:parent.nextpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks">下一页</a></td>');
    else listhead.document.write('	 <td width="40" valign="bottom" class="ablacks">下一页</td>');
    if ((recscount[index] < 1) || (currpage[index] == Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1)) //?
    listhead.document.write('	 <td width="40" valign="bottom" class="ablacks">最后页</td>');
    else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.lastpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  class="ablacks">最后页</a></td>');
    listhead.document.write('	 <td width="30" valign="bottom"><a href="javascript:parent.goanotherpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks">前往</a></td>');
    listhead.document.write('	 <td width="15" valign="bottom"><span class="style6">第</span></td>');
    listhead.document.write('	 <td width="10" valign="bottom" class="style6"><input name="pagenumber" type="text" class="input" onKeyPress="javascript:parent.gotopage(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  size="1" maxlength="3"></td>');
    listhead.document.write('	 <td width="10" valign="bottom" class="style6">&nbsp;&nbsp;页</td>');
    listhead.document.write('	 <td width="25" valign="bottom" class="style6">&nbsp;</td>');

    listhead.document.write('	 <td width="35" valign="bottom"><span class="style6">搜索</span></td>');
    listhead.document.write('	 <td width="55 " valign="bottom" class="style6"><input name="txtSearch" type="text" class="input" onKeyPress="javascript:parent.searchDataEx(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ',parent.' + arrayName + ',this.value,' + arrayIndex + ')"	size="1" maxlength="60"  style="width:100px"></td>');
    listhead.document.write('      </tr>');
    listhead.document.write('    </table></td>');
    listhead.document.write('  </tr>');
    listhead.document.write('</table>');

    out_tail(listhead);
    if (allcheckobj[index] != null) //把所有的checkbox取消选中
    allcheckobj[index].checked = false;
    //chgLsHeadColor(listhead);
}

/**
重写表格显示dspList函数
为其增加新的内容
**/
dspList=function()
{
	/*原有内容*/
	oldDspList.apply(this,arguments);//等价于oldDspList(arugumens[0],arguments[1]...arguments[argumens.length-1])
	//默认选中第一个
	if(namearray.length>0)
	{
		frame12.document.getElementById('tableList').rows[1].cells[0].childNodes[0].checked = true;
	}
}
function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1,0, 0);
}
function init() {
  	frame11.location.href = "_listhead2.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	CONF.passStr.value = passStr;
	CONF.ipStr.value= ipStr;
	CONF.macStr.value= macStr;
	CONF.isGroupOpt.value= isGroupOpt;
	if(!isGroupOpt)//单机管理
	{
		window.document.title = "联动管理-下发配置";
		window.document.title+=" / "+ipStr;
		if(waitOver != "" && urcpError=="") alert("操作成功！");//用户无法直接从界面上看到结果，过需提示
	}
	else if(waitOver != "")
	{
		if(!showBatchResult(errorArr,ipStr,macStr,"操作失败信息",1))
		{
			alert("操作成功!");
		}
	}
}

function init2() {
  	frame11.location.href = "_listhead2.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	CONF.passStr.value = passStr;
	CONF.ipStr.value= ipStr;
	CONF.macStr.value= macStr;
	CONF.isGroupOpt.value= isGroupOpt;
}
/*源配置文件模式选择（本机或联动配置文件）
function radMode()
{
	var obj=document.getElementById("confListTb");
	if(CONF.srcMode[0].checked) obj.style.display="none";
	else
	{
		try
		{
			obj.style.display="table";
		}
		catch(e)
		{
			obj.style.display="block";
		}
	}
}
*/
/*下发方式选择（手动或覆盖）*/
function selMode()
{
	var selObj=CONF.sendMode;
	if(selObj.value != 2)
	{
		document.getElementById('tbDiyConf').style.display="none";
	}
	else
	{
		try
		{
			document.getElementById('tbDiyConf').style.display="table";//ie8以上以及ff支持
		}
		catch(e)
		{
			document.getElementById('tbDiyConf').style.display="block";//ie7以及以下版本支持
		}
	}
}
function reSet()
{
	CONF.reset();
	//radMode();
	selMode();
	init2();
}
function save(opt)
{
	var obj,i;
	var tmp;
	/*
	if(CONF.srcMode[1].checked)//联动配置文件
	{
	*/
		CONF.remoteSrcConf.value="";
		obj = frame12.document.getElementsByName('radConfSrc');
		for(i=0;i<obj.length;i++)
		{
			if(obj[i].checked)
			{
				tmp = obj[i].value.split(" ");
				CONF.remoteSrcConf.value = tmp[0];//用此隐藏控件记录联动源配置文件名
				CONF.statustr.value = tmp[1];//用此隐藏控件记录配置文件是否保存到flash
			}
		}
	/*
	}
	*/
	if(CONF.sendMode[1].selected)
	{
		i = CONF.sysConf.checked || CONF.vlanConf.checked || CONF.aggrConf.checked ||
			CONF.portConf.checked || CONF.rateLimitConf.checked || CONF.arpConf.checked;
		if(!i)
		{
			alert("请至少选择一个配置项！");
			return;
		}
	}
	else
	{
		if(!confirm("以完全覆盖方式下发，需重启远程设备，才能使配置生效。确定继续？"))
		{
			return;
		}
	}
	if (!namearray[0])
	{
		alert("请在配置文件列表中选择一个文件！");
		return;
	}
	CONF.action="wait_remote_conf.asp";
	CONF.submit();
}

function go_back()
{
	if(isGroupOpt)
		CONF.action="remote_manage_group.asp";
	else
		CONF.action="remote_manage.asp";
	CONF.optType.value="";//清空，防止返回的页面误操作
	CONF.submit();
}
</script>
</head>
<body onLoad="init();" background="../images/body_main.png" style="background-repeat:repeat-x">
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
  <td><script language="javascript">showSecondTitle('配置下发');//显示页面名称</script>
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
	<table border="0" width="80%" align='center'>
	  <tr>
	    <td align="center" height="5"></td>
	    </tr>
	  <tr><td class="gapTd"></td><td align="center">
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
    <br>
    <form name="CONF" method="POST">
    <input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="remoteSrcConf">
<input type="hidden" value="sendConf" name="optType">
<input type="hidden" name="statustr">
<!--
<table width="600">
<tr><td width="93" title="当前设备正在使用的配置文件">
  <input type="radio" name="srcMode" value="1"  onClick="radMode()" checked>本机配置文件
</td>
  <td width="115" title="通过联动操作，获取并保存在本设备的，其他设备的配置文件"><input type="radio" name="srcMode"  onClick="radMode()" value="2">
联动配置文件</td>
  <td width="376">&nbsp;</td>
</tr>
</table>
-->
<table width="600" border="0" align="center" cellpadding="0" cellspacing="0" id="confListTb" style="display:block">

<tr><td colspan="5" height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="600"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	
</td></tr>
<tr>
<td colspan="5" align="center" valign="top">
<iframe scrolling="no" name="frame12" id="framelist" valign="top" align="center" width="600" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
</table>
<table width="600">
<tr><td>
下发方式<select name="sendMode" onChange="selMode()">
<!--<option value="0">智能覆盖</option>-->
<option value="1">完全覆盖</option>
<option value="2" selected>手动选择配置项</option>
</select>
</td>
</tr>
<tr height="5"><td></td></tr>
</table>
<TABLE  width="600" border="0" class="style22" cellpadding="4" cellspacing="1" align="center" id="tbDiyConf">
<tr><td><input type="checkbox" name="sysConf" value="1" >系统配置</td>
<td><input type="checkbox"  name="vlanConf" value="1" >vlan配置</td>
<td><input type="checkbox"  name="aggrConf" value="1" >汇聚配置</td></tr>
<tr><td><input type="checkbox"  name="portConf" value="1" >端口配置</td>
<td><input type="checkbox"  name="rateLimitConf" value="1">速率限制</td>
<td><input type="checkbox"  name="arpConf" value="1">IP/MAC绑定</td></tr>
</TABLE>
<table width="600" align="center" border="0">
<tr height="10"><td align="center"></td></tr>
<tr height="22">
<td width="" align="center">
<input name="" type="button" value="确定下发" class="input-button" onClick="save()">
<input name=""  type="button" value="重填" class="input-button" onClick="reSet()">
<input type="button" class="input-button" value="帮助" name="input" onClick="window.open('../rhelpinfo.htm#linkageSendConf')">
<input type="button" class="input-button" name="goBack" value="返回" onClick="go_back();">
</td></tr>
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
