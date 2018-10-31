<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
<%aspOutputFlowNoticeData();%>
var NoticeType = new Array(); NoticeType[2] = "edithtml";
var errorstr="";

/*字符串转换成html代码*/
function strToHtml(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    str = str.replace(/%/g ,"%%");
    return str;
}

/*检查函数*/
function checkData()
{
	var tbl = PPPoEFlowNotice;
	if (tbl.noticeType.selectedIndex == 0)
	{
	    var htmlLen = 1200;
	    //if (!checkIP(tbl.FromAddr, "起始网段", 0)) 
	    //	return false;
	    //if (!checkIP(tbl.EndAddr, "结束网段", 0)) 
	    //	return false; 
	    //if (compareIP(tbl.FromAddr.value, tbl.EndAddr.value) == 1)
	    //{
	       	//alert("网段范围填写错误！");
		//return false;
	    //}
	    if(!checkNumber(tbl.txtrxflowidle, 0, 4294967295, "通告上行剩余流量", 1))
	    {
		return false;
	    }
	    if(!checkNumber(tbl.txttxflowidle, 0, 4294967295, "通告下行剩余流量", 1))
	    {
		return false;
	    }
	    if (!checkName(tbl.noticeTitle, 20, "通告标题", 1))
		return false;
	    if (tbl.bdy.value.lstrLen() > htmlLen)
	    {
		alert("通告内容长度不能超过"+htmlLen+"位！");
		return false;
	    } 
	}
	else if (tbl.noticeType.selectedIndex == 1)
	{
		if (!checkName(tbl.importhtml, 63, "导入的页面", 0))
			return false;
	}
	return true;
}

/*保存函数*/
function save(){
    var tbl = PPPoEFlowNotice;

    if (!checkData())
    {
	return;
    }
    //getTimeValue(tbl);
    //if(tbl.day.value=="0000000"&&!tbl.everyday.checked){
	//alert("请选择日期！");
	//tbl.day.value="";
	//return;
    //}
    tbl.action = "/goform/formFlowNoticeConfig";
    tbl.submit();
}


/*根据通知方式赋给标题提供页面方式*/
function showInstInformation(instance)
{
    var tbl = PPPoEFlowNotice;
    tbl.enabled.checked = (lively[instance].toLowerCase() == "1") ? true : false;
    tbl.noticeTitle.value = htmlTitle[instIndex];
    if (NoticeType[2].toLowerCase() == "edithtml")
    {
	tbl.noticeType.selectedIndex = 0;
    }
    else if (NoticeType[2].toLowerCase() == "importhtml")
    {
	tbl.noticeType.selectedIndex = 1;
    }
    noticeTypeChg();

}

/*更改通知方式按钮*/
function timeTypeChanged()
{
	var tbl = PPPoEFlowNotice;
	
	if (tbl.timeType.selectedIndex == 0) 
	{
		enableNoticeTd.innerHTML = "启用日期到期推送";
		
		showTable(timeTR, 1);
		showTable(flowrxTR, 0);
		showTable(flowtxTR, 0);
		showInstInformation(1);
	} 
	else if (tbl.timeType.selectedIndex == 1)
	{
		enableNoticeTd.innerHTML = "启用流量到期推送";
		
		showTable(timeTR, 0);
		showTable(flowrxTR, 1);
		showTable(flowtxTR, 1);
		showInstInformation(2);
	}
}

/*初始化按钮*/
function init()
{
	var tbl = PPPoEFlowEotice;
	tb1.enabled.checked=(lively.toLowerCase() == "1")?true : false;

	if(NoticeType[2].toLowerCase() == "edithtml")
	{
	    tb1.noticeType.selectedIndex = 0;
	}
	else if(NoticeType[2].toLowerCase() == "importhtml")
	{
	    tb1.noticeType.selectedIndex = 1;
	}
	noticeTypeChg();
	tbl.noticeTitle.value = htmlTitle;
	tbl.txtrxflowidle.value = rxflowidle;
	tbl.txttxflowidle.value = txflowidle;
}


/*预览页面*/
function preview()
{
    var tbl = PPPoEFlowNotice;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml(str);
    openForm(PPPoEFlowNotice,"Notify.asp",750,470);
}


/*提供页面方式更改方式*/
function noticeTypeChg() {
	var tbl = PPPoEFlowNotice;
	if (tbl.noticeType.selectedIndex == 0) {
		showTable(noticeHtml, 1);
		showTable(noticeHtml2, 1);
		previewTd.innerHTML = '<input type="button" name="previewB" value=" 预览页面 " onclick="preview()" />';
	} else {
		showTable(noticeHtml, 0);
		showTable(noticeHtml2, 0);
		showTable(noticeUrl, 1);
		previewTd.innerHTML = '&nbsp;';
	}
}

</script>
</head>

<body onLoad="" onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("通告");</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form id="PPPoEFlowNotice" name="PPPoEFlowNotice" action="/goform/" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"80%",
						3,
						new Array("通告",'Notice.asp',1,0),
						new Array("账号到期通告",'PPPoENotice.asp',1,0),
						new Array("账号流量到期通告",'PPPoEFlowNotice.asp',1,1)
		);
		showThreeMenu();
	</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" id="all">
<tr>
<td align="center" class="three_td">
	<table border="0" cellpadding="0" cellspacing="0" width="600">
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	
	<tr style="display:none">
	<td width="34%" height="30" align="right">通告方式</td>
	<td width="4%">&nbsp;</td>
	<td width="52%"><select id="timeType" name="timeType" class="input" style="width:100 " onChange="timeTypeChanged()">
	<option value="time">日期到期通告</option>	
	<option value="flow">流量到期通告</option>
	</select>
	</td>
	</tr>	
	
	<tr>
	<td width="34%" height="30" align="right" id="enableNoticeTd">启用</td>
	<td width="4%">&nbsp;</td>
	<td width="52%"><input type="checkbox" name="enabled" id="enabled"></td>
	</tr>
	
	<tr id = "flowrxTR" style="display: ">
	<td width="290" height="30" align="right">通告上行剩余流量</td>
	<td width="23">&nbsp;</td>
	<td width="405"><input type="text" name="txtrxflowidle" id="txtflowrxidle" class="input">&nbsp;MBytes</td>
	</tr>
	
	<tr id = "flowtxTR" style="display: ">
	<td width="290" height="30" align="right">通告下行剩余流量</td>
	<td width="23">&nbsp;</td>
	<td width="405"><input type="text" name="txttxflowidle" id="txtflowtxidle" class="input">&nbsp;MBytes</td>
	</tr>
	
	<tr style="display:none">
	<td align="right" height="30">提供页面方式</td>
	<td>&nbsp;</td>
	<td><select name="noticeType" id="noticeType" onChange="noticeTypeChg()" class="input" style="width:100 ">
	<option value="edithtml" >编辑页面</option>
	<option value="importhtml" >导入页面</option>
	</select></td>
	</tr>
	
	</table>
	
	

	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeHtml" style="display">

	<tr>
	<td width="34%" height="30" align="right">通告标题</td>
	<td width="4%" >&nbsp;</td>
	<td width="52%" ><input type="text" name="noticeTitle" id="noticeTitle" class="input2" maxlength="20"><input type="hidden" name="hidBdy" id="hidBdy" value=""></td>
	</tr>


	</table>


	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeHtml2" style="display">
	
	<tr>
	<td height="30" colspan="2" align="center" valign="bottom">通告内容</td>
	</tr>

	<tr>
	<td height="10">&nbsp;</td>
	</tr>

	<tr>
	<td>&nbsp;</td>
	<td align="center"><textarea name="bdy" id="bdy" style="font-size:13px; font-family:'新宋体'; width:492px; height:250px"><% write(aspOutputNoticeContentData()); %></textarea></td>
	<td>&nbsp;</td>
	</tr>

	</table>

	<table width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
	<td height="40" width="15%" id="saveTd">&nbsp;</td>
	<td width="58%" id="saveTd">&nbsp;</td>
	<td width="6%" id="previewTd"><input type="button" id="preview" name="preview" value=" 预览页面 " onClick="preview()" /></td>
	<td width="4%"><input type="button" id="baocun" name="baocun" value=" 保 存 " onClick="save()" /></td>
	<td width="17%" >&nbsp;</td>
	</tr>
	</table>
	

	

</td></tr></table>
</form></td>
</tr><script language="javascript">init();</script>
</table>
</td>
<td width="15"></td>
</tr>
</table>
</body>
</html>

