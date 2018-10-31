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
	aspOutPutApUpdateConf();
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

var i=0;
currpage[0] = 1;
recscount[0] = count;
lineButton[0]=0;
maxrecs[0] = totalrecs;
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

titles1[i] = "序列号";
arrayData1[i]=serialarray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_DATASIZE;

titles1[i] = _("<%write(APType);%>");
arrayData1[i]=types;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

titles1[i] = "更新时间";
arrayData1[i]=uptimearray;
sortable[0][i] = 0;
sortType[0][i++] = TYPE_STR;

titles1[i] = _("<%write(hardVersion);%>");
arrayData1[i]=firmVersion;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

titles1[i] = _("<%write(size);%>");
arrayData1[i]=filesizearray;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_DATASIZE;

titles1[i] = _("<%write(softVersion);%>");
arrayData1[i]=softVersion;
sortable[0][i] = 1;
sortType[0][i++] = TYPE_STR;

/*
arrayData1[i]=statuarray;
sortable[0][i] = 0;
sortType[0][i++] = TYPE_NUM;
*/
arrayCount[0] = i;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
function dsplist(index, list) {
if (index == 0) dspList(index, list, 750, titles1, titles1.length, arrayData1, 1, 0);
}
function saveAllToPc()
{
    CONF.delstr.value = "apUpdateConf.tar";
//    CONF.statustr.value = statuarray[namearray.searchData(fileName)];
    CONF.action="/cgi-bin/exportApUpdateConf.cgi";
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

function delApUpdate()
{
		    if (!genData(CONF, 0, frame12, namearray, "\"<%write(deviceProfiles);%> \"", ",")) {
			    return;
		    }

		    CONF.delstr.value = CONF.delstr.value.slice(0,-1);
		    var altStr=_("<%write(confirmDelTips);%>");
		    var actStr="/goform/formApUpdateConfFileDel";
		    CONF.action=actStr;
   		    CONF.submit();
   		    return;
}

function delAllApUpdate()
{
		    var actStr="/goform/formApUpdateConfFileDelAll";
		    CONF.action=actStr;
   		    CONF.submit();
   		    return;
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
	    actStr = "/goform/formApUpdateConfFileReName";
    }
    else if(opt == "list")
    {
	    index = CONF.sel_list.selectedIndex;
	    if(index == 1)//删除全部
	    {
		    if(!confirm(_("<%write(confirmDelAllFileTips);%>")))return;
		    actStr="/goform/formApUpdateConfFileDelAll";
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
			    actStr="/goform/formApUpdateConfFileDel";
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
    CONF.action = "/goform/formApUpdateConfFileDel";
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
<td class="table_bg"><script language="javascript">showSecondTitle(_("AP<%write(conf);%>"));//显示页面名称</script> 
 <table width="80%" border="0"  height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,80);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array(_("<%write(configList);%>"),'apUpdateConf.asp',1,1),
						new Array(_("<%write(updateConf);%>"),'apUploadConf.asp',1,0)
		);
		showThreeMenu();
	</script>
  <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align="center">
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
<input type="hidden" value="" name="devType">
<br>
<div id="moveDivPar"></div>
<table id="reNameTb" width="300" height="60" style="display:none">
<tr><td id="inputName_td"><%write(enterName);%> </td><td><input type="text" size="30" maxlength="50" value=""  onChange="checkFileName(this,50,'<%write(confName);%>',1);"  class="input" name="newName" id="newName"></td></tr><tr><td colspan="2" align="center"><input name="input" type="button" class="input-button" onClick="save('reName')" value="<%write(ok);%>" id="confirm"><input name="" type="button" value="<%write(cancel);%> " class="input-button" onClick="moveDivObj.hide();" id="cancel"></td></tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellping="0">
<tr>
<td ><table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">

<tr><td colspan="5" height="" align="center" valign="bottom" >
	<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="650"  height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden> </iframe>	

</td></tr>

<tr>
<td colspan="5" align="center" valign="top">
<iframe scrolling="auto" name="frame12" valign="top" align="center" width="650" height="260" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>
</td></tr>
<tr><td><table width="100%" align="center"  cellpadding="0" cellspacing="0">
<tr><td width="204" height="21" valign="top"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="all">  &nbsp;<%write(allSelect);%> / <%write(allUnselect);%> &nbsp;</td>
<td width="" align="right" valign="top">
<!--  <select size="1" class="input" name="sel_list" id="sel_list" onChange="changeMode(this)">
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
-->
  <input type="button" class="input-button" value="<%write(del);%> " name="btn_scan1" onClick="delApUpdate()" id="confirm">
  <input type="button" class="input-button" value="<%write(delAll);%> " name="btn_scan2" onClick="delAllApUpdate()" id="confirm">
  <input type="button" class="input-button" style="width:70px" value="<%write(apConfBackup);%> " name="apUpdateConf" onClick="saveAllToPc()" id="confirm">
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
