<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DnsFilter"); %>
<% langTran("DnsNotice"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

<%getSysLang();%>
<%aspOutputDnsFilterNoticeData();%>
var NoticeType=new Array();NoticeType[2] ="edithtml";
var errorstr="";

/*字符串转换成html代码*/
function strToHtml(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    str = str.replace(/%/g ,"%%");/*预览时%会消失，用2个%即可*/
    return str;
}

function strToHtml1(str)
{
    str = str.replace(/ /g ,"&nbsp;");
    str = str.replace(/\r/g,'');
    str = str.replace(/\n/g ,"<br>");
    return str;
}

/*检查函数*/
function checkData()
{
    var tbl = DnsFilterNotice;
    if (tbl.noticeType.selectedIndex == 0)
    {
	var htmlLen = 898;
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
    if (!checkNumber(tbl.skiptime, 0, 999, "<% write(AutoSkipTime); %>", 1))
		return false;
    if (!checkName(tbl.skipurl, 63, "<% write(AutoSkipUrl); %>", 1))
		return false;
    if (searchStr("http://", tbl.skipurl.value, true) == 0)
    {
	tbl.skipurl.value = tbl.skipurl.value.substring(7);
    }
    return true;
}

/*保存函数*/
function save(){
    var tbl = DnsFilterNotice;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml1(str);
    if (!checkData())
    {
	return;
    }
    tbl.action = "/goform/formDnsNoticeConfig";
    tbl.submit();
}

/*初始化按钮*/
function init()
{
    var tbl = DnsFilterNotice;
    tbl.enabled.checked = (lively.toLowerCase() == "1") ? true : false;

    if (NoticeType[2].toLowerCase() == "edithtml")
    {
	tbl.noticeType.selectedIndex = 0;
    }
    else if (NoticeType[2].toLowerCase() == "importhtml")
    {
	tbl.noticeType.selectedIndex = 1;
    }
    noticeTypeChg();
    tbl.noticeTitle.value = htmlTitle;
    tbl.skiptime.value = skiptimes;
    tbl.skipurl.value = skipurls;
}


/*预览页面*/
function preview()
{
    var tbl = DnsFilterNotice;
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
    openForm(DnsFilterNotice,"Notify.asp",750,470);
}


/*提供页面方式更改方式*/
function noticeTypeChg() {
    var tbl = DnsFilterNotice;
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
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle("<% write(dns_dnsFilterNotice); %>");</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form id="DnsFilterNotice" name="DnsFilterNotice" action="/goform/" method="post">
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"80%",
	2,
	new Array("<%write(dns_dnsFilterConfig);%>",'DnsFilter.asp',1,0),
	new Array("<%write(dns_dnsFilterNotice);%>",'DnsFilter_Notice.asp',1,1)
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

<tr id = "noticeHtml">
<td width="34%" height="30" align="right"><% write(NoticeTitle); %></td>
<td width="4%" >&nbsp;</td>
<td width="52%" ><input type="text" name="noticeTitle" id="noticeTitle" class="input2" maxlength="20"><input type="hidden" name="hidBdy" id="hidBdy" value=""></td>
</tr>


<tr>
<td width="34%" height="30" align="right" id="autoskiptime"><% write(AutoSkipTime); %></td>
<td width="4%">&nbsp;</td>
<td width="52%" ><input type="text" name="skiptime" id="skiptime" class="input2" maxlength="3"><% write(com_noticetips); %></td>
</tr>

<tr>
<td width="34%" height="30" align="right" id="autoskipurl"><% write(AutoSkipUrl); %></td>
<td width="4%">&nbsp;</td>
<td width="52%"><strong>http://</strong> <input type="text" name="skipurl" id="skipurl" class="input2" maxlength="63"></td>
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
<td align="center"><textarea name="bdy" id="bdy" style="font-size:13px; font-family:'新宋体'; width:492px; height:250px"><% write(aspOutputNoticeContentData()); %></textarea></td>
<td>&nbsp;</td>
</tr>

</table>

<table width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
<td height="40" width="9%" id="saveTd">&nbsp;</td>
<td width="45%" id="saveTd">&nbsp;</td>
<td width="14%" id="previewTd"><input type="button" id="preview" name="preview" value=" <% write(PreviewPage); %>" onClick="preview()" /></td>
<td width="11%"><input type="button" id="baocun" name="baocun" value="<% write(com_save); %>" onClick="save()" /></td>
<td width="11%"><input type="button" id="help" name="help" value=" <% write(com_help); %> " onClick="window.open('<% write(com_helppage); %>#DnsFilterNotice')"/>&nbsp;</td>
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
