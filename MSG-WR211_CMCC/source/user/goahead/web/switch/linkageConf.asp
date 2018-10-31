<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>联动管理-联动配置</title>
<link rel=stylesheet type="text/css" href="../css/style.css" title="cool" id="lnk">
<script src="../js/common.js" type="text/javascript"></script>
<script src="js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%
aspOutRemoteConf();//打印配置文件信息
var waitOver=Form("waitOver");
var isGroupOpt=Form("isGroupOpt");
%>
//var errorArr = new Array("","","","");
var isGroupOpt="<%write(isGroupOpt);%>";
var waitOver="<%write(waitOver);%>";
var versionarray = new Array('SG3124R', 'SG3124R', 'SG3124R', 'SG3124R', 'SG3124R','SG3124R', 'SG3124R', 'SG3124R', 'SG3124R', 'SG3124R','SG3124F');
//end交互
var urcpError="";
if(!isGroupOpt && waitOver != "")
{
	urcpError = errorArr[0];//单击管理
}
var savearray = new Array();
var loadarray=new Array();
outerFlashTotalSpace=convertToByte(outerFlashTotalSpace);
for(var i=0;i<namearray.length;i++)
{
	loadarray[i]='<a href="javascript:parent.saveToPc(\''+ namearray[i] +'\')">下载</a>';////可排序。故用文件名
	if(outerFlashTotalSpace)
	{
		if(statuarray[i]) savearray[i]='已保存';
		else savearray[i]='<a href="javascript:parent.saveToDevice(\''+ namearray[i] +'\')">保存</a>';//可排序。故用文件名
	}
}
var i=0;
currpage[0] = 1;
recscount[0] = namearray.length;
maxrecs[0] = 24;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = "配置文件列表";
var titles1 = new Array(); //注意此处命名需为titles1(为了与列表中的变量对应)
var arrayData1 = new Array();
sortable[0] = new Array();
sortType[0] = new Array();
i = 0;

titles1[i] = "配置文件名";
arrayData1[i]=namearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_CHINESE;

titles1[i] = "创建时间";
arrayData1[i]=timearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

titles1[i] = "大小";
arrayData1[i]=filesizearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_DATASIZE;

if(outerFlashTotalSpace)
{
	arrayData1[i]=savearray;
	titles1[i] = "保存到U盘";
	sortable[0][i] = 1;
	sortType[0][i++] = TYPE_STR;
}

titles1[i] = "下载到PC";
arrayData1[i]=loadarray;
sortable[0][i] = 0;
sortType[0][i++] = TYPE_STR;

arrayData1[i]=statuarray;
sortable[0][i] = 0;
sortType[0][i++] = TYPE_NUM;

arrayCount[0] = i;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 0;
function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 1, 1);
}

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

function saveToPc(fileName)
{
	CONF.delstr.value = fileName;
	CONF.statustr.value = statuarray[namearray.searchData(fileName)];
	CONF.action="/cgi-bin/exportLinkageConf.cgi";
	CONF.submit();	
}
function saveToDevice(fileName)
{
	if(!confirm("您确定要保存所选配置文件到本设备？"))return;;
	CONF.delstr.value = fileName;
	CONF.statustr.value = statuarray[namearray.searchData(fileName)];//是否已经保存到设备
	CONF.action="/goform/formLinkageConfSave";
	CONF.submit();		
}
var moveDivObj = null;
function initHelp()
{
	if(macStr!='')
	{
		var obj = document.getElementsByName('helpButton');
		for(var i =0 ;i < obj.length; i++)
		{
	        obj[i].onclick = function() {window.open('../rhelpinfo.htm?#linkageGetConfUrcp')};
		}
	}
}
function init() {
	var obj = document.getElementById('reNameTb');
  	frame11.location.href = "_listhead2.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	moveDivObj = new moveDiv(obj,document.getElementById('moveDivPar'));
	try
	{
		obj.style.display="table";
	}
	catch(e)
	{
		obj.style.display="block";
	}
	moveDivObj.init("文件重命名");
	if(macStr!='')
	{
		controlWin(850,530,1);
		obj=document.getElementsByName("goBack");
		for(var i=0;i<obj.length;i++)
		{
			obj[i].style.display="inline";
		}
		initHelp();
		if(!isGroupOpt)//单机管理
		{
			window.document.title+=" / "+ipStr;
			if(waitOver != "" && urcpError=="") alert("操作成功！");
			
		}
		else if(waitOver != "")
		{
			if(!showBatchResult(errorArr,ipStr,macStr,"操作失败信息",1))
			{
				alert("操作成功!");
			}
		}
	}
	CONF.passStr.value = passStr;
	CONF.ipStr.value= ipStr;
	CONF.macStr.value= macStr;
	CONF.isGroupOpt.value= isGroupOpt;
	
}
function changeMode(obj) {
    if (obj.value == 0 || obj.value == 2) { //单个操作（删除，保存）
        disableAllCheck(0, frame12, false);
        CONF.delchk.disabled = false;
    } else {
        disableAllCheck(0, frame12, true);
        CONF.delchk.disabled = true;
    }
}
function save(opt)
{
	var index,i,j;
	var actStr='';
	var altStr='';
	if(opt == "reName")
	{//后台通过newName获取新名称，oldName为老名称
		if (!checkFileName(CONF.newName, 50, "配置文件名", 1)) {
            return;
        }
		i = namearray.searchDataCaseInsensitive(CONF.oldName.value);//window系统大小写不敏感，应兼容
		j = namearray.searchDataCaseInsensitive(CONF.newName.value);
		if( j>=0 && j!=i)
		{
			inputErro(CONF.newName,"该名称已经被其他配置文件使用，请重新输入！");
			return;
		}
		CONF.statustr.value = statuarray[i];
		actStr = "/goform/formLinkageConfReName";
	}
	else if(opt == "list")
	{
		index = CONF.sel_list.selectedIndex;
		if(index == 1)//删除全部
		{
			if(!confirm("您确定要删除全部的配置文件？\n注意：删除后不可恢复，但不包括本机当前使用的配置文件。"))return;
			actStr="/goform/formLinkageConfDelAll";
		}
		else if(index == 3)//全部保存到设备
		{
			if(!confirm("您确定要保存全部的配置文件到本设备？"))return;
			actStr="/goform/formLinkageConfSaveAll";
		}
		else
		{
			if (!genData(CONF, 0, frame12, statuarray, "\"设备配置文件\"", ",")) {
				return;
			}
			CONF.statustr.value = CONF.delstr.value.slice(0,-1);//是否已经保存到设备
			CONF.statustr.value = CONF.statustr.value.replace(/,/g,"");
			if (!genData(CONF, 0, frame12, namearray, "\"设备配置文件\"", ",")) {
				return;
			}
			CONF.delstr.value = CONF.delstr.value.slice(0,-1);
			if(index == 0)//删除
			{
				altStr="您确定要删除所选配置文件？\n注意：删除后不可恢复。";
				actStr="/goform/formLinkageConfDel";
			}
			else if(index == 2) //保存到设备
			{
				altStr="您确定要保存所选配置文件到本设备？";
				actStr="/goform/formLinkageConfSave";
			}
			if(!confirm(altStr))return;
		}
	}
	CONF.action=actStr;
	CONF.submit();
	return;
}
function go_back()
{
	if(isGroupOpt)
		CONF.action="remote_manage_group.asp";
	else
		CONF.action="remote_manage.asp";
	CONF.submit();
}
function EditConfig(val, i) {
	
    if (val == 0) 
	{
		//添加
	}
    else 
	{
		inputRight(CONF.newName);
		CONF.oldName.value = namearray[i];
		CONF.newName.value = namearray[i];
		moveDivObj.show();
		CONF.newName.focus();
	}
}
function DeleteConfig(index) {
    if (confirm("您确定要删除所选配置文件？\n注意：删除后不可恢复。")) {
      	CONF.delstr.value = namearray[index];
		CONF.statustr.value = statuarray[index];
        CONF.action = "/goform/formLinkageConfDel";
        CONF.submit();

    }
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
  <td><script language="javascript">showSecondTitle('联动配置');//显示页面名称</script>
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
    <form name="CONF" method="POST">
 
<input type="hidden" name="delstr" value="">
<input type="hidden" name="statustr">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="oldName">
<br>
<div id="moveDivPar"></div>
<table id="reNameTb" width="250" height="60" style="display:none">
<tr><td>请输入名称:<input type="text" size="30" maxlength="50" value=""  onChange="checkFileName(this,50,'配置文件名',1);"  class="input" name="newName" id="newName"></td></tr><tr><td colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="save('reName')" value="确定"><input name="" type="button" value="取消" class="input-button" onClick="moveDivObj.hide();"></td></tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="750" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td colspan="5" height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="750"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	

</td></tr>

<tr>
<td colspan="5" align="center" valign="top">
<iframe scrolling="no" name="frame12" id="framelist" valign="top" align="center" width="750" height="248" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
<tr><td><table width="750" align="center"  cellpadding="0" cellspacing="0">
<tr><td width="204" height="21" valign="top"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))>  &nbsp;全选 / 全不选&nbsp;</td>
<td width="544" align="right" valign="top">
  <select size="1" class="input" name="sel_list" id="sel_list" onChange="changeMode(this)">
    <option value="0" selected="selected">删除</option>
    <option value="1">删除全部</option>
    <script language="javascript">
	if(outerFlashTotalSpace)
	{
		document.write( '<option value="2">保存到U盘</option>');
		document.write( '<option value="3">全部保存到U盘</option>');
	}
	</script>
  </select>
  <input type="button" class="input-button" value="确定" name="btn_scan2" onClick="save('list')">

  <input type="button" id="helpButton" name="helpButton" value="帮助" class="input-button" onClick="window.open('../rhelpinfo.htm#linkageGetConf')">
  <input type="button" class="input-button" style="display:none" name="goBack" value="返回" onClick="go_back();"></td>
</tr></table></td></tr>
</table>
</td>
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
