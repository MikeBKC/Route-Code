<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FitApMacFilter"); %>
<%langTran("commonWords");%>
<title><%write(WirelessMacFilter);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script src="switch/js/common_switch.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
 //交互begin
<% 
    aspOutPutApMacFilter();//交互进行配置信息过滤，只显示适用于该ap所在服务区的配置
    aspOutPutApMacFilterConfUrcp();//交互进行配置信息过滤，只显示适用于该ap坐在服务区的配置

	var waitOver=Form("waitOver");
	var isGroupOpt=Form("isGroupOpt");
	var devType=Form("devType");
	var szName=Form("szName");
	var managedEn=Form("managedEn");

%>
var isGroupOpt="<%write(isGroupOpt);%>";
var waitOver="<%write(waitOver);%>";
var devType="<%write(devType);%>";
var managedEn="<%write(managedEn);%>";

//end交互

var urcpError="";
if(!isGroupOpt && waitOver != "")
{
	urcpError = errorArr[0];//单击管理
}
if(macStr == "")
{
	closeWin(_("<%write(mf_lawless);%>"));
}

var titles1 = new Array();
titles1[0] = _("<%write(mf_confName);%>");
titles1[1] = _("<%write(mf_SZ);%>");
titles1[2] = _("<%write(mf_rule);%>");

recsPerPage[0] = 10; //每页多少条数据
lineButton[0] = 0;
currpage[0] = 1;	//当前页
recscount[0] = totalrecs;	//当前条目数
maxrecs[0] = max_totalrecs;	//最大条目数
listname[0] = "frame12";
listheadname[0] = "frame11";
listheadcaption[0] = _("<%write(WirelessMacFilter);%>");
sortable[0] = new Array(1, 1, 1, 1);
sortType[0] = new Array(TYPE_STR, TYPE_STR, TYPE_STR, TYPE_NUM);
arrayCount[0] = 4;//总共多少列(这里指的是跟着排序变化的列)
var arrayData1 = new Array(macConfName, SZname, Rules, macNum);

function dsplist(index, list) {
    if (index == 0) dspList(index, list, 600, titles1, titles1.length, arrayData1, 1, 0);
}
function init() {
	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
	frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	pushApMacFilter.passStr.value = passStr;
	pushApMacFilter.ipStr.value= ipStr;
	pushApMacFilter.macStr.value= macStr;
	pushApMacFilter.szName.value= szName;
	pushApMacFilter.managedEn.value= managedEn;
	pushApMacFilter.isGroupOpt.value= isGroupOpt;
	if(!isGroupOpt)//单机管理
	{
		window.document.title = _("<%write(WirelessMacFilter);%>");
		window.document.title+=" / "+ipStr;
		if(waitOver != "" && urcpError=="") alert(_("<%write(mf_lawfull);%>"));//用户无法直接从界面上看到结果，过需提示
	}
	else if(waitOver != "")
	{
		if(!showBatchResult(errorArr,ipStr,macStr,_("<%write(mf_failTips);%>"),1))
		{
			alert(_("<%write(mf_lawfull);%>"));
		}
	}
}

function init2() {
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	pushApMacFilter.passStr.value = passStr;
	pushApMacFilter.ipStr.value= ipStr;
	pushApMacFilter.macStr.value= macStr;
	pushApMacFilter.isGroupOpt.value= isGroupOpt;
	pushApMacFilter.szName.value= szName;
	pushApMacFilter.managedEn.value= managedEn;
}

function reSet()
{
	pushApMacFilter.reset();
	init2();
}
function reGenData(formName, index, list, array, tips, flagstr) //删除列表中的元素
{
    var recbegin;
    var recend;
    var t = 0;
    var chks = list.document.getElementsByName("chklist");
    if (typeof(flagstr) == "undefined" || trimstr(flagstr) == '') flagstr = ',';
    formName.delstr.value = "";
    recbegin = (currpage[index] - 1) * recsPerPage[index]; //显示页面的开始记录数 以0为开始下标
    recend = recbegin + recsPerPage[index]; //显示页面的结束记录数

    if (recend >= recscount[index]) recend = recscount[index]; //如果是最后页　把总计量赋值给recend

    for (var i = recbegin; i < recend; i++) { //因为recbegin比显示的开始记录数小一，所以i< 并非 <= recend

        if (chks.length) { //循环遍历checkbox
            if (chks[t].checked == 1) if (chks[t].value != "on") {
                formName.delstr.value = formName.delstr.value + array[eval(chks[t].value)] + flagstr;
                //生成隐藏参数
            } else {
                formName.delstr.value = formName.delstr.value + array[i] + flagstr;
            }
        } else {
            if (chks.checked == 1) if (chks.value != "on") {
                formName.delstr.value = formName.delstr.value + array[eval(chks.value)] + flagstr;
            } else {
                formName.delstr.value = formName.delstr.value + array[i] + flagstr;
            }
        }

        t = t + 1;
    }
/*
    if (formName.delstr.value != "") return true; //判断是否选择checkbox
    else {
        alert("请先选择" + tips + "记录。");
        return false;
    }
*/
    return true;
}

function save()
{

	var MacNum=0;
	if (!reGenData(pushApMacFilter, 0, frame12, macConfName, _("<%write(MACAddrFilterList);%>"), ",")) {
        return;
    }
	pushApMacFilter.mfNameArray.value=pushApMacFilter.delstr.value.slice(0,-1);

	array = pushApMacFilter.mfNameArray.value.split(",");
	if (array.length > 4)
	{
		//alert("AP最多只能加入4个MAC地址配置！");	
		alert(_("<%write(mf_warning1);%>"));	
		return;
	}
	if (!reGenData(pushApMacFilter, 0, frame12, macNum, _("<%write(MACAddrFilterList);%>"), ",")) {
        return;
    }
	pushApMacFilter.mfMacNum.value=pushApMacFilter.delstr.value.slice(0,-1);
	array = pushApMacFilter.mfMacNum.value.split(",");
	for(var i=0;i<array.length;i++)
	{
	    MacNum += parseInt(array[i]);	
	}
	if(MacNum > 64)
	{
		alert(_("<%write(mf_warning3);%>"));	
		return;
	}

    if (!isGroupOpt){
//        if (array.length > maxSzNum)
//        {
//            //alert("所选的服务区数目已经超出该AP所支持的最大服务区数目\n该AP最多可加入" + maxSzNum + "个服务区。");
//            alert(_("<%write(mf_warning2);%>"));	
//            return;
//        }
    }else{
        document.getElementById('optType').value= "PapMacFilter";
    }

	pushApMacFilter.devType.value= devType;
	pushApMacFilter.action="ap_wait_remote_conf.asp";
	pushApMacFilter.submit();
}
function go_back()
{
	if(isGroupOpt)
		pushApMacFilter.action="ap_remote_manage_group.asp";
	else
		pushApMacFilter.action="ap_remote_manage.asp";
	pushApMacFilter.devType.value= devType;
	pushApMacFilter.szName.value= szName;
	pushApMacFilter.managedEn.value= managedEn;
	pushApMacFilter.optType.value="";//清空，防止返回的页面误操作
	pushApMacFilter.submit();
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
  <td><script language="javascript">showSecondTitle(_("<%write(WirelessMacFilter);%>"));//显示页面名称</script>
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
<form name="pushApMacFilter" method="post">
<input type="hidden" name="delstr" value="">
<input type="hidden" name="mfNameArray" value="">
<input type="hidden" name="mfMacNum" value="">
<input type="hidden" name="statustr">
<input type="hidden" value="Apply" name="_submit">
<input type="hidden" value="" name="ipStr">
<input type="hidden" value="" name="macStr">
<input type="hidden" value="" name="szName">
<input type="hidden" value="" name="managedEn">
<input type="hidden" value="" name="passStr">
<input type="hidden" value="" name="isGroupOpt">
<input type="hidden" value="" name="oldName">
<input type="hidden" value="apMacFilter" name="optType" id="optType">
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
	<td width="25" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist')) id="delChk"></td>
	<td align="left" width="46%" id="all"><%write(allSelect);%>/<%write(allUnselect);%></td>
  <td height="21" align="center" valign="top" nowrap id="">
    <input type="button" name="button" id="outbutton" value="<%write(mf_sureout);%>" onClick="save()" class="input-button">
    <input name=""  type="button" value="<%write(refill);%>" class="input-button" onClick="reSet()" id="resetButton">
    <input type="button" class="input-button" value="<%write(help);%>" name="input" onClick="window.open('rhelpinfo.htm#pushApMacFilter')" id="helpButton">
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
