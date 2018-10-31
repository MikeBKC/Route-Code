<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("getApConf");%>
<%langTran("commonWords");%>
<%langTran("list");%>

<title><%write(getConf);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script language="javascript">
//交互
<%
	var urcpSet=Form("urcpSet");
	var devType=Form("devType");
    aspOutPutRemoteApConf();//打印配置文件信息
//	write('var outerFlashTotalSpace = "0";\n');
//	write('var macStr = "123456789123";\n');
//	write('var ipStr = "200.200.202.125";\n');
//	write('var passStr = "admin";\n');
//	write('var namearray = new Array("configWA1800N_00-22-AA-AF-7A-02_20111108_214400.txt");\n');
//	write('var types = new Array("WA1800N");\n');
//	write('var softVersion = new Array("WA1800N v1.0.0-120507");\n');
//	write('var firmVersion = new Array("V1.0");\n');
//	write('var statuarray = new Array("0");\n');
//	write('var filesizearray = new Array("694B");\n');
//	write('var totalrecs = 1;\n');
//	write('var errorArr = new Array("");\n');
//	write('var errorstr="";\n');
    var waitOver=Form("waitOver");
    var isGroupOpt=Form("isGroupOpt");
%>
var isGroupOpt="<%write(isGroupOpt);%>";
var waitOver="<%write(waitOver);%>";
var devType="<%write(devType);%>";
//end交互
var urcpError="";
if(!isGroupOpt && waitOver != "")
{
    urcpError = errorArr[0];//单击管理
}

var savearray = new Array();
var loadarray=new Array();
//outerFlashTotalSpace=convertToByte(outerFlashTotalSpace);
for(var i=0;i<namearray.length;i++)
{
    loadarray[i]='<a href="javascript:parent.saveToPc(\''+ namearray[i] +'\')" id="download_a"><%write(download);%></a>';////可排序。故用文件名
/*	
    if(outerFlashTotalSpace)
    {
	    if(statuarray[i]) savearray[i]=_("saved");
	    else savearray[i]='<a href="javascript:parent.saveToDevice(\''+ namearray[i] +'\')">_("save")</a>';//可排序。故用文件名
    }
*/	
}



var i=0;
currpage[0] = 1;
recscount[0] = totalrecs;
lineButton[0]=0;
maxrecs[0] = 24;
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(configList);%>");
var titles1 = new Array(); //注意此处命名需为titles1(为了与列表中的变量对应)
var arrayData1 = new Array();
sortable[0] = new Array();
sortType[0] = new Array();
i = 0;

titles1[i] = _("<%write(confName);%>");
arrayData1[i]=namearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_CHINESE;

/*
titles1[i] = "创建时间";
arrayData1[i]=timearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;
*/

titles1[i] = _("<%write(APType);%>");
arrayData1[i]=types;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

titles1[i] = _("<%write(softVersion);%>");
arrayData1[i]=softVersion;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

titles1[i] = _("<%write(hardVersion);%>");
arrayData1[i]=firmVersion;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

titles1[i] = _("<%write(size);%>");
arrayData1[i]=filesizearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_DATASIZE;
/*
if(outerFlashTotalSpace)
{
    arrayData1[i]=savearray;
    titles1[i] = _("saveToU");
    sortable[0][i] = 1;
    sortType[0][i++] = TYPE_STR;
}
*/
titles1[i] = _("<%write(downToPc);%>");
arrayData1[i]=loadarray;
sortable[0][i] = 0;
sortType[0][i++] = TYPE_STR;
/*
arrayData1[i]=statuarray;
sortable[0][i] = 0;
sortType[0][i++] = TYPE_NUM;
*/
arrayCount[0] = i;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
function dsplist(index, list) {
if (index == 0) dspList(index, list, 750, titles1, titles1.length, arrayData1, 1, 1);
}
/*
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

if (currpage[index] == 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(firstPage);%></td>');
else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.firstpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(firstPage);%></td>');
if (currpage[index] <= 1) listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(prePage);%></td>');
else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.prepage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(prePage);%></a></td>');

if (currpage[index] * recsPerPage[index] < recscount[index]) listhead.document.write('	    <td width="40" valign="bottom"><a href="javascript:parent.nextpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(nextPage);%></a></td>');
else listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(nextPage);%></td>');
if ((recscount[index] < 1) || (currpage[index] == Math.floor((recscount[index] - 1) / recsPerPage[index]) + 1)) //?
listhead.document.write('	 <td width="40" valign="bottom" class="ablacks"><%write(lastPage);%></td>');
else listhead.document.write('	 <td width="40" valign="bottom"><a href="javascript:parent.lastpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  class="ablacks"><%write(lastPage);%></a></td>');
listhead.document.write('	 <td width="30" valign="bottom"><a href="javascript:parent.goanotherpage(' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')" class="ablacks"><%write(go);%></a></td>');
listhead.document.write('	 <td width="15" valign="bottom"><span class="style6"><%write(the);%> </span></td>');
listhead.document.write('	 <td width="10" valign="bottom" class="style6"><input name="pagenumber" type="text" class="input" onKeyPress="javascript:parent.gotopage(event,' + index.toString() + ',parent.' + listheadname[index] + ',parent.' + listname[index] + ')"  size="1" maxlength="3"></td>');
listhead.document.write('	 <td width="10" valign="bottom" class="style6">&nbsp;&nbsp;<%write(page);%></td>');
listhead.document.write('	 <td width="25" valign="bottom" class="style6">&nbsp;</td>');

listhead.document.write('	 <td width="35" valign="bottom"><span class="style6"><%write(search);%></span></td>');
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
*/
function saveToPc(fileName)
{
    CONF.delstr.value = fileName;
    CONF.statustr.value = statuarray[namearray.searchData(fileName)];
    CONF.action="/cgi-bin/exportLinkageConf.cgi";
    CONF.submit();	
}
function saveToDevice(fileName)
{
    if(!confirm(_("<%write(confirmSaveTips);%>")))return;;
    CONF.delstr.value = fileName;
    CONF.statustr.value = statuarray[namearray.searchData(fileName)];//是否已经保存到设备
    CONF.action="/goform/formApConfFileSave";
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
	    obj[i].onclick = function() {window.open('<% write(com_helppage); %>?#getApConf')};
	    }
    }
}
function init() {
    var obj = document.getElementById('reNameTb');
    frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
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
    moveDivObj.init(_("<%write(rename);%>"));
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
		    if(waitOver != "" && urcpError=="") alert(_("<%write(lawfull);%>"));
		    
	    }
	    else if(waitOver != "")
	    {
		    if(!showBatchResult(errorArr,ipStr,macStr,_("<%write(failTips);%>"),1))
		    {
			    alert(_("<%write(lawfull);%>"));
		    }
	    }
    }
    CONF.passStr.value = passStr;
    CONF.ipStr.value= ipStr;
    CONF.macStr.value= macStr;
    CONF.szName.value= szName;
    CONF.managedEn.value= managedEn;
    CONF.isGroupOpt.value= isGroupOpt;
	CONF.devType.value= devType;
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
	    if (!checkFileName(CONF.newName, 50, _("<%write(confName);%>"), 1)) {
	return;
    }
	    i = namearray.searchDataCaseInsensitive(CONF.oldName.value);//window系统大小写不敏感，应兼容
	    j = namearray.searchDataCaseInsensitive(CONF.newName.value);
	    if( j>=0 && j!=i)
	    {
		    inputErro(CONF.newName,_("<%write(usedTips);%>"));
		    return;
	    }
	    CONF.statustr.value = statuarray[i];
	    actStr = "/goform/formApConfFileReName";
    }
    else if(opt == "list")
    {
	    index = CONF.sel_list.selectedIndex;
	    if(index == 1)//删除全部
	    {
		    if(!confirm(_("<%write(confirmDelAllFileTips);%>")))return;
		    actStr="/goform/formApConfFileDelAll";
	    }
	    else if(index == 3)//全部保存到设备
	    {
		    if(!confirm(_("<%write(confirmSaveAllTips);%>")))return;
		    actStr="/goform/formApConfFileSaveAll";
	    }
	    else
	    {
		    if (!genData(CONF, 0, frame12, statuarray, "\"<%write(deviceProfiles);%> \"", ",")) {
			    return;
		    }
		    CONF.statustr.value = CONF.delstr.value.slice(0,-1);//是否已经保存到设备
		    CONF.statustr.value = CONF.statustr.value.replace(/,/g,"");
		    if (!genData(CONF, 0, frame12, namearray, "\"<%write(deviceProfiles);%> \"", ",")) {
			    return;
		    }
		    CONF.delstr.value = CONF.delstr.value.slice(0,-1);
		    if(index == 0)//删除
		    {
			    altStr=_("<%write(confirmDelTips);%>");
			    actStr="/goform/formApConfFileDel";
		    }
		    else if(index == 2) //保存到设备
		    {
			    altStr=_("<%write(confirmSaveTips);%>");
			    actStr="/goform/formApConfFileSave";
		    }
		    if(!confirm(altStr))return;
	    }
    }
    //alert(CONF.delstr.value);
    //return;
    CONF.action=actStr;
    CONF.submit();
    return;
}
function go_back()
{
    CONF.macStr.value = macStr;
    CONF.szName.value = szName;
    CONF.managedEn.value = managedEn;
    if(isGroupOpt)
	    CONF.action="ap_remote_manage_group.asp";
    else
	    CONF.action="ap_remote_manage.asp";
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
if (confirm(_("<%write(confirmDelTips);%>"))) {
    CONF.delstr.value = namearray[index];
	    CONF.statustr.value = statuarray[index];
	    //alert(CONF.delstr.value);
	    //return;
    CONF.action = "/goform/formApConfFileDel";
    CONF.submit();
}
}
</script>
</head>
<body onLoad="init();" background="images/body_main.png" style="background-repeat:repeat-x">
<script language="javascript">urcpErrorPost("ap_remote_manage.asp");</script>
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
  <td><script language="javascript">showSecondTitle(_("AP<%write(conf);%>"));//显示页面名称</script>
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
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="oldName">
<input type="hidden" value="" name="devType">
<br>
<div id="moveDivPar"></div>
<table id="reNameTb" width="300" height="60" style="display:none">
<tr><td id="inputName_td"><%write(enterName);%> </td><td><input type="text" size="30" maxlength="50" value=""  onChange="checkFileName(this,50,'<%write(confName);%>',1);"  class="input" name="newName" id="newName"></td></tr><tr><td colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="save('reName')" value="<%write(ok);%>" id="confirm"><input name="" type="button" value="<%write(cancel);%> " class="input-button" onClick="moveDivObj.hide();" id="cancel"></td></tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="750" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td colspan="5" height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="750"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	

</td></tr>

<tr>
<td colspan="5" align="center" valign="top">
<iframe scrolling="auto" name="frame12" valign="top" align="center" width="750" height="250" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
<tr><td><table width="750" align="center"  cellpadding="0" cellspacing="0">
<tr><td width="204" height="21" valign="top"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="all">  &nbsp;<%write(allSelect);%> / <%write(allUnselect);%> &nbsp;</td>
<td width="544" align="right" valign="top">
  <select size="1" class="input" name="sel_list" id="sel_list" onChange="changeMode(this)">
    <option value="0" selected="selected" id="del"><%write(del);%></option>
    <option value="1" id="delall"><%write(delAll);%> </option>
    <script language="javascript">
/*	
	if(outerFlashTotalSpace)
	{
		document.write( '<option value="2" id="saveToU">保存到U盘</option>');
		document.write( '<option value="3" id="saveAllToU">全部保存到U盘</option>');
	}
*/	
	</script>
  </select>
  <input type="button" class="input-button" value="<%write(ok);%> " name="btn_scan2" onClick="save('list')" id="confirm">
  <input type="button" id="helpButton" name="helpButton" value="<%write(help);%> " class="input-button" onClick="window.open('<% write(com_helppage); %>#getApConf')">
  <input type="button" class="input-button" style="display:none" name="goBack" value="<%write(back);%>" onClick="go_back();" id="backButton"></td>
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
