<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("PPPoENotice"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script language="javascript">

<%getSysLang();%>
<%aspOutputTimeNoticeData();%>
var NoticeType=new Array();NoticeType[1] ="edithtml";
var errorstr="";

/*�ַ���ת����html����*/
function strToHtml(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    str = str.replace(/%/g ,"%%");/*Ԥ��ʱ%����ʧ����2��%����*/
    return str;
}

function strToHtml1(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    return str;
}

/*��麯��*/
function checkData()
{
	var tbl = PPPoENotice;
	if (tbl.noticeType.selectedIndex == 0)
	{
	    var htmlLen = 898;
	    //if (!checkIP(tbl.FromAddr, "<% write(StartingSegment); %>", 0)) 
	    //	return false;
	    //if (!checkIP(tbl.EndAddr, "<% write(EndingSegment); %>", 0)) 
	    //	return false; 
	    //if (compareIP(tbl.FromAddr.value, tbl.EndAddr.value) == 1)
	    //{
		//alert("<% write(SegmentErr); %>");
		//return false;
	    //}
	    if(!checkNumber(tbl.txtimeidle, 0, 4294967295, "<% write(AccRemainDay); %>", 1))
	    {
		return false;
	    }
	    if (!checkName(tbl.noticeTitle, 20, "<% write(NoticeTitle); %>", 1))
		return false;
	    if (tbl.hidBdy.value.lstrLen() > htmlLen)
	    {
		alert("<% write(NoticeLenErr); %>");
		return false;
	    } 
	}
	else if (tbl.noticeType.selectedIndex == 1)
	{
		if (!checkName(tbl.importhtml, 63, "<% write(ImportPage); %>", 0))
			return false;
	}
	return true;
}

/*���溯��*/
function save(){
    var tbl = PPPoENotice;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml1(str);
    if (!checkData())
    {
	return;
    }
    //getTimeValue(tbl);
    //if(tbl.day.value=="0000000"&&!tbl.everyday.checked){
	//alert("<% write(PleaseSelecData); %>");
	//tbl.day.value="";
	//return;
    //}
    tbl.action = "/goform/formTimeNoticeConfig";
    tbl.submit();
}


/*����֪ͨ��ʽ���������ṩҳ�淽ʽ*/
function showInstInformation(instance)
{
    var tbl = PPPoENotice;
    tbl.enabled.checked = (lively[instance].toLowerCase() == "1") ? true : false;
    tbl.noticeTitle.value = htmlTitle[instIndex];
    if (NoticeType[1].toLowerCase() == "edithtml")
    {
	tbl.noticeType.selectedIndex = 0;
    }
    else if (NoticeType[1].toLowerCase() == "importhtml")
    {
	tbl.noticeType.selectedIndex = 1;
    }
    noticeTypeChg();

}

/*����֪ͨ��ʽ��ť*/
function timeTypeChanged()
{
	var tbl = PPPoENotice;
	
	if (tbl.timeType.selectedIndex == 0) 
	{
		enableNoticeTd.innerHTML = "<% write(DueDataDel); %>";
		
		showTable(timeTR, 1);
		showTable(flowrxTR, 0);
		showTable(flowtxTR, 0);
		showInstInformation(1);
	} 
	else if (tbl.timeType.selectedIndex == 1)
	{
		enableNoticeTd.innerHTML = "<% write(DueDataDel); %>";
		
		showTable(timeTR, 0);
		showTable(flowrxTR, 1);
		showTable(flowtxTR, 1);
		showInstInformation(2);
	}
}

/*��ʼ����ť*/
function init()
{
	var tbl = PPPoENotice;
	tbl.enabled.checked = (lively.toLowerCase() == "1") ? true : false;
	
	if (NoticeType[1].toLowerCase() == "edithtml")
	{
		tbl.noticeType.selectedIndex = 0;
	}
	else if (NoticeType[1].toLowerCase() == "importhtml")
	{
		tbl.noticeType.selectedIndex = 1;
	}
	noticeTypeChg();
	tbl.noticeTitle.value = htmlTitle;
	tbl.txtimeidle.value = timeidle;
	
}


/*Ԥ��ҳ��*/
function preview()
{
    var tbl = PPPoENotice;
    var htmlLen = 898;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml(str);
    if (!checkName(tbl.noticeTitle, 20, "<% write(NoticeTitle); %>", 1))
	return false;
    if (tbl.hidBdy.value.lstrLen() > htmlLen)
    {
	alert("<% write(NoticeLenErr); %>");
	return false;
    } 
    openForm(PPPoENotice,"Notify.asp",750,470);
}


/*�ṩҳ�淽ʽ���ķ�ʽ*/
function noticeTypeChg() {
	var tbl = PPPoENotice;
	if (tbl.noticeType.selectedIndex == 0) {
		showTable(noticeHtml, 1);
		showTable(noticeHtml2, 1);
		previewTd.innerHTML = '<input type="button" name="previewB" value=" <% write(PreviewPage); %> " onclick="preview()" />';
	} else {
		showTable(noticeHtml, 0);
		showTable(noticeHtml2, 0);
		showTable(noticeUrl, 1);
		previewTd.innerHTML = '&nbsp;';
	}
}

</script>
</head>

<body onLoad="" onbeforeunload="logOut()" background="<% write(com_bodyMain); %>" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****���ϼ��****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****�м�����****-->
<tr>
<td width="15"></td>
<!--////����-->
<td><script language="javascript">showSecondTitle("<% write(NoticeSecondTitle); %>");</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form id="PPPoENotice" name="PPPoENotice" action="/goform/" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"80%",
						2,
						new Array("<% write(NoticeDaily); %>",'Notice.asp',1,0),
						new Array("<% write(NoticeIDTimeout); %>",'PPPoENotice.asp',PPPoEGlobal,1)
						//new Array("<% write(NoticeIDFlowout); %>",'PPPoEFlowNotice.asp',1,0)
		);
		showThreeMenu();
	</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" id="all">
<tr>
<td align="center" class="three_td">
	<table border="0" cellpadding="0" cellspacing="0" width="600">
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	
	
	<tr>
	<td width="34%" height="30" align="right" id="enableNoticeTd"><% write(StartUsing); %></td>
	<td width="4%">&nbsp;</td>
	<td width="52%"><input type="checkbox" name="enabled" id="enabled"></td>
	</tr>
	
	<tr id = "timeTR" style="display: ">
	<td width="290" height="30" align="right"><% write(Pre); %></td>
	<td width="23">&nbsp;</td>
	<td width="405"><input type="text" name="txtimeidle" id="txtimeidle" class="input2" size="1" maxlength="3">&nbsp;<% write(NoticeDaySendTimeout); %></td>
	</tr>

	<tr id = "noticeHtml">
	<td width="34%" height="30" align="right"><% write(NoticeTitle); %></td>
	<td width="4%" >&nbsp;</td>
	<td width="52%" ><input type="text" name="noticeTitle" id="noticeTitle" class="input2" maxlength="20"><input type="hidden" name="hidBdy" id="hidBdy" value=""></td>
	</tr>


	
	<tr style="display:none">
	<td align="right" height="30"><% write(ProPageWay); %></td>
	<td>&nbsp;</td>
	<td><select name="noticeType" id="noticeType" onChange="noticeTypeChg()" class="input" style="width:100 ">
	<option value="edithtml" ><% write(EditPage); %></option>
	<option value="importhtml" ><% write(ImportPage); %></option>
	</select></td>
	</tr>
	
	</table>
	
	



	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeHtml2" style="display">
	
	<tr>
	<td height="30" colspan="2" align="center" valign="bottom"><% write(NoticeContent); %></td>
	</tr>

	<tr>
	<td height="10">&nbsp;</td>
	</tr>

	<tr>
	<td>&nbsp;</td>
	<td align="center"><textarea name="bdy" id="bdy" style="font-size:13px; font-family:'������'; width:492px; height:250px"><% write(aspOutputNoticeContentData()); %></textarea></td>
	<td>&nbsp;</td>
	</tr>

	</table>

	<table width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr>
	<td height="40" width="9%" id="saveTd">&nbsp;</td>
	<td width="45%" id="saveTd">&nbsp;</td>
	<td width="14%" id="previewTd"><input type="button" id="preview" name="preview" value=" <% write(PreviewPage); %>" onClick="preview()" /></td>
	<td width="11%"><input type="button" id="baocun" name="baocun" value="<% write(com_save); %>" onClick="save()" /></td>
	<td width="11%"><input type="button" id="help" name="help" value=" <% write(com_help); %> " onClick="window.open('<% write(com_helppage); %>#PPPoENotice')"/>&nbsp;</td>
	<td width="10%" >&nbsp;</td>
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

