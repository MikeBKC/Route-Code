<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("apTmp");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //交互begin
 /*
 var tmpname = new Array("temp_1", "temp_2");
 var types = new Array("510W", "WA1800N", "510w");
 var firmwareVersion = new Array("V1.0", "V2.0", "V1.0");
 var defaults = new Array("yes", "", "");
 
 var errorstr = "";
 var totalrecs = 3;
 */
// 交互end
<% 
	aspOutPutApConfTempList();
%>
/*
var tmpname = new Array("temp_1", "temp_2");
var wlFres = new Array("2.4G","5G");        
var wlModes = new Array("11g","11n");       
var wlRates = new Array("300M","750M");
var totalrecs = 2;var errorstr="";
*/
var titles1 = new Array();
titles1[0] = _("<%write(modeName);%>");
titles1[1] = _("<%write(wlFre);%>");
titles1[2] = _("<%write(wlMode);%>");
titles1[3] = _("<%write(wlRate);%>");
var strSecondMenuFocus=_("<%write(APSettingMode);%>"); 

currpage[0] = 1;	//当前页
recscount[0] = totalrecs;	//当前条目数
maxrecs[0] = 24;	//最大条目数
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(APSettingModeList);%>");
sortable[0] = new Array(1, 1, 1, 1, 0);

sortType[0] = new Array(TYPE_NUM, TYPE_CHINESE, TYPE_STR, TYPE_STR, TYPE_STR);
arrayCount[0] = 4;//总共多少列(这里指的是跟着排序变化的列)
recsPerPage[0] = 10; //每页多少条数据
lineButton[0]=0;
//var arrayData1 = new Array(tmpname, types, firmwareVersion, defaults);
var arrayData1 = new Array(tmpname,wlFres, wlModes, wlRates);

function dsplist(index, list) {
    if (index == 0) dspList(index, list, -1, titles1, titles1.length, arrayData1, 1, 1);
}

var typeArr =  new Array();
var firmwareArr = new Array();


function init() {
	//initDefault();
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
}

function EditConfig(val,i){
	if(val==0)	//添加
	{ 
		confTempList.oldTmpName.value="";
		if(eval(maxrecs[0]-totalrecs)<1)
		{
			alert(_("<%write(warning7);%>"));
			return;
		}
	}
	else	//修改
	{
		confTempList.oldTmpName.value=tmpname[i];
	}
	confTempList.action="editTemp.asp";
	confTempList.submit();
}

function DeleteConfig(index){
	if(confirm(_("<%write(warning3);%>"))){
		confTempList.delstr.value=tmpname[index];
		confTempList.action="/goform/formApConfTempDel";
	    confTempList.submit();
	}
}

function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		confTempList.action="/goform/formApConfTempDelAll";
		confTempList.submit();
	}
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(APSettingMode);%>"));//显示页面名称</script>
  <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						2,
						new Array(_("<%write(modeList);%>"),'apTmp.asp',1,1),
						new Array(_("<%write(editMode);%>"),'editTemp.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="confTempList" method="post" action="/goform/formApConfTempDel">
<input type="hidden" name="delstr">
<input type="hidden" name="oldTmpName">
<input type="hidden" name="defName">
<table  border="0" width="600" align="center" cellpadding="0" cellspacing="0">
<tr>
<td colspan="6" height="" align="center" valign="bottom">
<iframe  scrolling="no" name="frame11" valign="bottom" align="center" width="650" height="44" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe>	  </td>
</tr>
<tr>
<td colspan="6" align="center" valign="top">
<iframe name="frame12" id="framelist" scrolling="no" valign="top" align="MIDDLE" width="650" height="248" marginwidth="1" marginheight="0" frameborder="0" overflow-y:hidden></iframe></td>
</tr>
<tr>
<td width="26" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
<td width="274" id="all"> <%write(allSelect);%> / <%write(allUnselect);%> </td>
<td width="300"  align="right">	  
<input type="button" class="input-button" onClick="EditConfig(0)" value="<%write(addNew);%>"  id="addNewCase">
<input type="button" class="input-button" onClick="delAll()" value="<%write(delAll);%>" id="delAllEntry">
<input type="submit" class="input-button" onClick="return(genData(confTempList,0,frame12,tmpname,'<%write(APSettingMode);%>'))" value="<%write(del);%>"  id="delButton">
<input type="button" id="helpButton" name="helpButton" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#apTmpMng')">
<input type="button" class="input-button" style="display:none" name="goBack" value= "<%write(back);%>"  onClick="go_back();" id="backButton"></td>
</tr></table>
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
