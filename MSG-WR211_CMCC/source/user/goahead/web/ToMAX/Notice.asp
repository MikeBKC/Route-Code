<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("DnsNotice"); %>
<% langTran("Notice"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>

<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
<%getSysLang();%>
<%aspOutputNoticeData();%>
var NoticeType=new Array();NoticeType[0]="edithtml";
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
function DateOnly()
{
    if (!(((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || (window.event.keyCode == 45)))
    {
	window.event.keyCode = 0;
    }
}
function ZeroBegin_Del(str)
{
    for (i = 0; i < str.length; i++) 
    {
	str[i] = str[i].toString();
	if (str[i] == "0")
	{
	    return "0";
	}
	if (str[i] == "")
	{
	    return "";
	}

	while (str[i].length > 0 && str[i].indexOf('0') == 0) 
	{
	    str[i] = str[i].substring(1, str[i].length);
	}
    }

    return str;

}
function CheckDate(date,low,high)
{
    if(!((eval(date)>eval(low))&&(eval(date)<eval(high))))
    {
	return false;
    }
    return true;
}

//判断日期月份输入是否合法
function CheckMonthDay(year, month, day)
{
    if ((eval(month) == 4 || eval(month) == 6 || eval(month) == 9 || eval(month) == 11) && eval(day) == 31)    
    {
	return false;
    }
	if(eval(day) > 29 && eval(month) == 2)
	{
	return false;
    }
	if ((eval(year)%4 != 0) && eval(day) > 28 && eval(month) == 2)
    {
	return false;
    }
    return true;
}
/*检查函数*/
function checkData()
{
	var tbl = formNotice;
	if (tbl.noticeType.selectedIndex == 0)
	{
	    var htmlLen = 898;
	    if (!checkIP(tbl.FromAddr, "<% write(StartSegment); %>", 0)) 
		return false;
	    if(tbl.FromAddr.value == "0.0.0.0")
	    {
		alert("<% write(StartSegmentErr); %>");
		return false;
	    }
	    if (!checkIP(tbl.EndAddr, "<% write(EndingSegment); %>", 0)) 
		return false; 
	    if(tbl.EndAddr.value == "0.0.0.0")
	    {
		alert("<% write(EndingSegmentErr); %>");
		return false;
	    }
	    if (compareIP(tbl.FromAddr.value, tbl.EndAddr.value) == 1)
	    {
		alert("<% write(SegmentDataErr); %>");
		return false;
	    }
	    if (!checkName(tbl.noticeTitle, 20, "<% write(AnnounceTitle); %>", 1))
		return false;
	    if (tbl.hidBdy.value.lstrLen() > htmlLen)
	    {
		alert("<% write(AnnounceLenErr); %>");
		return false;
	    } 
	}
	else if (tbl.noticeType.selectedIndex == 1)
	{
		if (!checkName(tbl.importhtml, 63, "<% write(TheImportPage); %>", 0))
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
    sysDate1=new Date();
    sysDate2=new Date();
    var d1;
    var d2;
    var timezone="UTC-0800";
    if( tbl.year1.value == "yyyy-mm-dd" )
    {
	alert("<% write(NullStartDataErr); %>");
	return false;
    }

//分割日期
    TimeArrBegin = tbl.year1.value.split("-");


    if(TimeArrBegin.length != 3)
    {
	alert("<% write(BadStartData); %>");
	return false; 
    }
    if(TimeArrBegin[0]==""||TimeArrBegin[1]==""||TimeArrBegin[2]=="")
    {
	alert("<% write(BadStartData); %>");
	return false; 
    }
    if( TimeArrBegin[0].length > 4 || TimeArrBegin[1].length > 2 ||TimeArrBegin[2].length > 2 )
    {
	alert("<% write(BadStartData); %>");
	return false;
    }
	
    TimeArrBegin = ZeroBegin_Del(TimeArrBegin); 	
	
    if( !CheckDate(TimeArrBegin[0],1988, 2051) )
    {
	alert("<% write(StartDataShould); %>");
	return false;
    }
    if((!CheckDate(TimeArrBegin[1],0,13))||(!CheckDate(TimeArrBegin[2],0,32)))
    {
	alert("<% write(BadStartData); %>");
	return false;
    }	
    if(!CheckMonthDay(TimeArrBegin[0], TimeArrBegin[1], TimeArrBegin[2]))
    {
	alert("<% write(BadStartData); %>");
	return false;
    }
    sysDate1.setFullYear(eval(TimeArrBegin[0]),eval(TimeArrBegin[1]-"1"),eval(TimeArrBegin[2]));
    sysDate1.setHours(eval("00"));
    sysDate1.setMinutes(eval("00"));
    sysDate1.setSeconds(eval("00"));
    d1=LocalToUTC(eval(Date.parse(sysDate1.toUTCString()))/1000,TimezoneToSecond(timezone));
    if(d1 == false)
    {
	return false;
    }
    tbl.BeginDate.value=d1;
	
	if( tbl.year2.value == "yyyy-mm-dd" )
    {
	alert("<% write(NullEndDataErr); %>");
	return false;
    }		
    TimeArrEnd = tbl.year2.value.split("-");
    if(TimeArrEnd.length != 3)
    {
	alert("<% write(BadEndData); %>");
	return false;
    }
	

    if(TimeArrEnd[0]==""||TimeArrEnd[1]==""||TimeArrEnd[2]=="")
    {
	alert("<% write(BadEndData); %>");
	return false; 
    }


    if( TimeArrEnd[0].length > 4 || TimeArrEnd[1].length > 2 ||TimeArrEnd[2].length > 2 )
    {
	alert("<% write(BadEndData); %>");
	return false;
    }
    
	
//去掉日期开头的0

    TimeArrEnd = ZeroBegin_Del(TimeArrEnd);
	
	
//保持和原来页面相同

    if( !CheckDate(TimeArrEnd[0],1988, 2051) )
    {
	alert("<% write(EndDataShould); %>");
	return false;
    }

    if((!CheckDate(TimeArrEnd[1],0,13))||(!CheckDate(TimeArrEnd[2],0,32)))
    {
	alert("<% write(BadEndData); %>");
	return false;
    }
    

    if(!CheckMonthDay(TimeArrEnd[0], TimeArrEnd[1], TimeArrEnd[2]))
    {
        alert("<% write(BadEndData); %>");
	return false;
    }

    sysDate2.setFullYear(eval(TimeArrEnd[0]),eval(TimeArrEnd[1]-"1"),eval(TimeArrEnd[2]));
    sysDate2.setHours(eval("23"));
    sysDate2.setMinutes(eval("59"));
    sysDate2.setSeconds(eval("59"));
    d2=LocalToUTC(eval(Date.parse(sysDate2.toUTCString()))/1000,TimezoneToSecond(timezone));
    if(d2 == false)
    {
	return false;
    }
    tbl.EndDate.value=d2;
    if(d1 > d2)
    {
	alert("<% write(StartEndErr); %>");
	return false;
    }
	return true;
}

/*保存函数*/
function save(){
    var tbl = formNotice;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml1(str);
    if(tbl.year1.value == "")
    {
	tbl.year1.value = "1990-01-01";
    }
    if(tbl.year2.value == "")
    {
	tbl.year2.value = "2020-01-01";
    }
    if (!checkData())
    {
	return;
    }
    getTimeValue(tbl);
    if(tbl.day.value=="0000000"&&!tbl.everyday.checked){
	alert("<% write(PleaseSelectData); %>");
	tbl.day.value="";
	return;
    }
    tbl.action = "/goform/formConfigNoticeConfig";
    tbl.submit();
}

/*初始化按钮*/
function init()
{
	var tbl = formNotice;
	tbl.enabled.checked = (lively.toLowerCase() == "1") ? true : false;
	

	if (NoticeType[0].toLowerCase() == "edithtml")
	{
		tbl.noticeType.selectedIndex = 0;
	}
	else if (NoticeType[0].toLowerCase() == "importhtml")
	{
		tbl.noticeType.selectedIndex = 1;
	}
	noticeTypeChg();
	tbl.noticeTitle.value = htmlTitle;
	tbl.skiptime.value = skiptimes;
	tbl.skipurl.value = skipurls;
	tbl.FromAddr.value = ipStart;
	tbl.EndAddr.value = ipEnd;
	group_days=(group_days=="0000000"?"1111111":group_days);
	group_timestarts=(group_timestarts==""?"All":group_timestarts); 
	setTimeValue(tbl,group_days,group_timestarts,group_timestops);/*common.js中的公用函数*/
	group_datestarts = group_datestarts.replace(/:/g ,"-");
	group_datestops = group_datestops.replace(/:/g ,"-");
	tbl.year1.value = group_datestarts;
	tbl.year2.value = group_datestops;
}


/*预览页面*/
function preview()
{
    var tbl = formNotice;
    var htmlLen = 898;

    tbl.hidBdy.value = ""; 
    var str = tbl.bdy.value;
    tbl.hidBdy.value = strToHtml(str);
    if (!checkName(tbl.noticeTitle, 20, "<% write(AnnounceTitle); %>", 1))
	return false;
    if (tbl.hidBdy.value.lstrLen() > htmlLen)
    {
	alert("<% write(AnnounceLenErr); %>");
	return false;
    } 
    openForm(formNotice,"Notify.asp",750,470);
}


/*提供页面方式更改方式*/
function noticeTypeChg() {
	var tbl = formNotice;
	if (tbl.noticeType.selectedIndex == 0) {
		showTable(noticeHtml, 1);
		showTable(noticeHtml2, 1);
		showTable(noticeUrl, 0);
		previewTd.innerHTML = '<input type="button" name="previewB" value=" <% write(PreviewPage); %> " onclick="preview()" />';
	} else {
		showTable(noticeHtml, 0);
		showTable(noticeHtml2, 0);
		showTable(noticeUrl, 1);
		previewTd.innerHTML = '&nbsp;';
	}
}

</script>
<style type=text/css>
/*日历控件所用到的class*/
#calendar a:link {color:#464646;text-decoration:none;}
#calendar a:visited {color:#464646;text-decoration:none;}
#calendar a:hover{color:#ed145b;text-decoration:underline;}
#calendar a:active{color:#ed145b;text-decoration:underline;}
#calendar td{font-size:12px}
/*/*/
/*DateCSS样式*/
.header {font: 12px Arial, Tahoma !important;font-weight: bold !important;font: 11px Arial, Tahoma;font-weight: bold;color: #154BA0;background:#C2DEED;height: 25px;padding-left: 10px;
}
.header td {padding-left: 10px;}
.header a {color: #154BA0;}
.header input {background:none;vertical-align: middle;height: 16px;}
.category {font: 12px Arial, Tahoma !important;font: 11px Arial, Tahoma;color: #92A05A;height:20px;background-color: #FFFFD9;}
.category td {border-bottom: 1px solid #DEDEB8;}
.expire, .expire a:link, .expire a:visited {color: #999999;}
.default, .default a:link, .default a:visited {color: #000000;}
.checked, .checked a:link, .checked a:visited {color: #FF0000;}
.today, .today a:link, .today a:visited {color: #00BB00;}
#calendar_year {display: none;line-height: 130%;background: #FFFFFF;position: absolute;z-index: 10;}
#calendar_year .col {float: left;background: #FFFFFF;margin-left: 1px;border: 1px solid #86B9D6;padding: 4px;}
#calendar_month {display: none;background: #FFFFFF;line-height: 130%;border: 1px solid #86B9D6;padding: 4px;position: absolute;z-index: 11;}
.tableborder {background: white;border: 1px solid #86B9D6;}
#year,#month{padding-right:10px;}
</style>
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
<td><script language="javascript">showSecondTitle("<% write(NoticeSecondTitle); %>");</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td><form id="formNotice" name="formNotice" action="/goform/" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
					"80%",
						2,
						new Array("<% write(NoticeDaily); %>",'Notice.asp',1,1),
						new Array("<% write(NoticeIDTimeout); %>",'PPPoENotice.asp',PPPoEGlobal,0)
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
	
	<tr style="display:none">
	<td align="right" height="30"><% write(ProPageWay); %></td>
	<td>&nbsp;</td>
	<td><select name="noticeType" id="noticeType" onChange="noticeTypeChg()" class="input" style="width:100 ">
	<option value="edithtml" ><% write(EditPage); %></option>
	<option value="importhtml" ><% write(ImportPage); %></option>
	</select></td>
	</tr>
	
	<tr>
	<td width="34%" align="right" height="30"><% write(AnnounceNotice); %></td>
	<td width="4%">&nbsp;</td>
	<td width="52%" height="30" align="left">
	<input type="text" name="FromAddr" id="FromAddr" value="" class="input" size="15" onKeyPress="ipOnly()"/>
	&nbsp;<% write(To); %>&nbsp; 
	<input type="text" name="EndAddr" id="EndAddr" value="" class="input" size="15" onKeyPress="ipOnly()"/>
	<span class="validate" id="endAddrTxt"></span>
	</td>
	</tr>

	<tr id = "noticeHtml">
	<td width="34%" height="30" align="right"><% write(AnnounceTitle); %></td>
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
	</table>
	
	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeUrl" style="display:none ">
	<tr>
	<td width="34%" height="30" align="right"><% write(ImportPage); %></td>
	<td width="4%">&nbsp;</td>
	<td width="52%"><strong>http://</strong>
	</tr>
	</table>



	<table border="0" cellpadding="0" cellspacing="0" width="600" id="noticeHtml2" style="display">
	
	<tr>
	<td height="30" colspan="2" align="center" valign="bottom"><% write(AnnData); %></td>
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
	<td width="14%" id="previewTd"><input type="button" id="preview" name="preview" value=" <% write(PreviewPage); %> " onClick="preview()" /></td>
	<td width="11%"><input type="button" id="baocun" name="baocun" value=" <% write(com_save); %> " onClick="save()" /></td>
	<td width="11%"><input type="button" id="help_notice" name="help" value=" <% write(com_help); %> " onClick="window.open('<% write(com_helppage); %>#NormalNotice')"/></td>
	<td width="10%" >&nbsp;</td>
	</tr>
	</table>
	

	<table border="0" cellpadding="0" cellspacing="0" width="600" style="display">

 <script language="javascript">
		/*added by jqSun20120208*/


/*
	[Discuz!] (C)2001-2007 Comsenz Inc.
	This is NOT a freeware, use is subject to license terms

	$RCSfile: Notice.asp,v $
*/

var controlid = null;
var currdate = null;
var startdate = null;
var enddate = null;
var yy = null;
var mm = null;
var hh = null;
var ii = null;
var currday = null;
var addtime = false;
var today = new Date();
var lastcheckedyear = false;
var lastcheckedmonth = false;

var ie = navigator.appName == "Microsoft Internet Explorer" ? true : false;
function $cla(objID) {
    return document.getElementById(objID);
}

function _cancelBubble(event) {
    e = event ? event : window.event;
    if (ie) {
        e.cancelBubble = true;
    } else {
        e.stopPropagation();
    }
}
function getposition(obj) {
    var r = new Array();
    r['x'] = obj.offsetLeft;
    r['y'] = obj.offsetTop;
    while (obj = obj.offsetParent) {
        r['x'] += obj.offsetLeft;
        r['y'] += obj.offsetTop;
    }
    return r;
}
function loadcalendar() {
    s = '';
    s += '<div id="calendar" style="display:none; position:absolute; z-index:9;" onclick="_cancelBubble(event)">';
    if (ie) {
        s += '<iframe width="200" height="185" src="about:blank" style="position: absolute;z-index:-1;"></iframe>';
}
    s += '<div style="width: 200px;"><table class="tableborder" cellspacing="0" cellpadding="0" width="100%" style="text-align: center">';

    s += '<tr align="center" class="header">';
    s += '<td colspan="1" class="header"><a href="#" onclick="refreshcalendar(yy-1, mm);return false" title="<% write(PreY); %>"><<</a>&nbsp;&nbsp;<a href="#" onclick="refreshcalendar(yy, mm-1);return false" title="<% write(PreM); %>"><</a></td>';
    s += '<td colspan="5" style="text-align: center;width:150px" class="header"><a href="#" onclick="showdiv(\'year\');_cancelBubble(event);return false" title="<% write(ClickToSelectY); %>" id="year"></a><a id="month" title="<% write(ClickToSelectM); %>" href="#" onclick="showdiv(\'month\');_cancelBubble(event);return false"></a></td>';
    s += '<td colspan="1" class="header"><A href="#" onclick="refreshcalendar(yy, mm+1);return false" title="<% write(NextM); %>">></A>&nbsp;&nbsp;<A href="#" onclick="refreshcalendar(yy+1, mm);return false" title="<% write(NextY); %>">>></A></td>';
    s += '</tr>';

    s += '<tr class="category"><td><% write(com_sunday); %></td><td><% write(com_monday); %></td><td><% write(com_tuesday); %></td><td><% write(com_wednesday); %></td><td><% write(com_thursday); %></td><td><% write(com_friday); %></td><td><% write(com_saturday); %></td></tr>';
    for (var i = 0; i < 6; i++) {
        s += '<tr class="altbg2">';
        for (var j = 1; j <= 7; j++)
            s += "<td id=d" + (i * 7 + j) + " height=\"19\">0</td>";
        s += "</tr>";
    }
    s += '<tr id="hourminute"><td colspan="7" align="center"><input type="text" size="1" value="" id="hour" onKeyUp=\'this.value=this.value > 23 ? 23 : zerofill(this.value);controlid.value=controlid.value.replace(/\\d+(\:\\d+)/ig, this.value+"$1")\'> <% write(H); %> <input type="text" size="1" value="" id="minute" onKeyUp=\'this.value=this.value > 59 ? 59 : zerofill(this.value);controlid.value=controlid.value.replace(/(\\d+\:)\\d+/ig, "$1"+this.value)\'> <% write(Minute); %></td></tr>';
 	s += '<tr class="header"><td colspan="4" align="left"><% write(TodayIs); %><a href="#" onclick="settoday();return false;">'+today.getFullYear() + '-' + (today.getMonth()+1) + '-' + today.getDate() +'</a></td><td align="right" colspan="3"><a href="#" onclick="closecalendar();return false"><% write(Close); %></a> </td></tr>';
    s += '</table></div></div>';
    s += '<div id="calendar_year" onclick="_cancelBubble(event)"><div class="col">';
    for (var k = 1930; k <= 2019; k++) {
        s += k != 1930 && k % 10 == 0 ? '</div><div class="col">' : '';
        s += '<a href="#" onclick="refreshcalendar(' + k + ', mm);$cla(\'calendar_year\').style.display=\'none\';return false"><span' + (today.getFullYear() == k ? ' class="today"' : '') + ' id="calendar_year_' + k + '">' + k + '</span></a><br />';
    }
    s += '</div></div>';
    s += '<div id="calendar_month" onclick="_cancelBubble(event)">';
    for (var k = 1; k <= 12; k++) {
        s += '<a href="#" onclick="refreshcalendar(yy, ' + (k - 1) + ');$cla(\'calendar_month\').style.display=\'none\';return false"><span' + (today.getMonth() + 1 == k ? ' class="today"' : '') + ' id="calendar_month_' + k + '">' + k + (k < 10 ? ' ' : '') + ' </span></a><br />';
    }
    s += '</div>';
    var nElement = document.createElement("div");
    nElement.innerHTML = s;
    document.getElementsByTagName("body")[0].appendChild(nElement);
    document.onclick = function (event) {
        $cla('calendar').style.display = 'none';
        $cla('calendar_year').style.display = 'none';
        $cla('calendar_month').style.display = 'none';
    }
    $cla('calendar').onclick = function (event) {
        _cancelBubble(event);
        $cla('calendar_year').style.display = 'none';
        $cla('calendar_month').style.display = 'none';
    }
}
function parsedate(s) {
    /(\d+)\-(\d+)\-(\d+)\s*(\d*):?(\d*)/.exec(s);
    var m1 = (RegExp.$1 && RegExp.$1 > 1899 && RegExp.$1 < 2101) ? parseFloat(RegExp.$1) : today.getFullYear();
    var m2 = (RegExp.$2 && (RegExp.$2 > 0 && RegExp.$2 < 13)) ? parseFloat(RegExp.$2) : today.getMonth() + 1;
    var m3 = (RegExp.$3 && (RegExp.$3 > 0 && RegExp.$3 < 32)) ? parseFloat(RegExp.$3) : today.getDate();
    var m4 = (RegExp.$4 && (RegExp.$4 > -1 && RegExp.$4 < 24)) ? parseFloat(RegExp.$4) : 0;
    var m5 = (RegExp.$5 && (RegExp.$5 > -1 && RegExp.$5 < 60)) ? parseFloat(RegExp.$5) : 0;
    /(\d+)\-(\d+)\-(\d+)\s*(\d*):?(\d*)/.exec("0000-00-00 00\:00");
    return new Date(m1, m2 - 1, m3, m4, m5);
}
function settime(d) {
    $cla('calendar').style.display = 'none';
    controlid.value = yy + "-" + zerofill(mm + 1) + "-" + zerofill(d) + (addtime ? ' ' + zerofill($cla('hour').value) + ':' + zerofill($cla('minute').value) : '');
}
function settoday() {
    today = new Date();
    yy = today.getFullYear();
    mm = today.getMonth();
    var d = today.getDate();
    $cla('calendar').style.display = 'none';
    controlid.value = yy + "-" + zerofill(mm + 1) + "-" + zerofill(d);
}
function closecalendar() {
    $cla('calendar').style.display = 'none';
}
function showcalendar(event, _controlid1, addtime1, startdate1, enddate1) {
    controlid = _controlid1;
    addtime = addtime1;
    startdate = startdate1 ? parsedate(startdate1) : false;
    enddate = enddate1 ? parsedate(enddate1) : false;
    currday = controlid.value ? parsedate(controlid.value) : today;
    hh = currday.getHours();
    ii = currday.getMinutes();
    var p = getposition(controlid);
    $cla('calendar').style.display = 'block';
    $cla('calendar').style.left = p['x'] + 'px';
    $cla('calendar').style.top = (p['y'] + 20) + 'px';
    _cancelBubble(event);
    refreshcalendar(currday.getFullYear(), currday.getMonth());
    if (lastcheckedyear != false) {
        $cla('calendar_year_' + lastcheckedyear).className = 'default';
        $cla('calendar_year_' + today.getFullYear()).className = 'today';
    }
    if (lastcheckedmonth != false) {
        $cla('calendar_month_' + lastcheckedmonth).className = 'default';
        $cla('calendar_month_' + (today.getMonth() + 1)).className = 'today';
    }
    $cla('calendar_year_' + currday.getFullYear()).className = 'checked';
    $cla('calendar_month_' + (currday.getMonth() + 1)).className = 'checked';
    $cla('hourminute').style.display = addtime ? '' : 'none';
    lastcheckedyear = currday.getFullYear();
    lastcheckedmonth = currday.getMonth() + 1;
}
function refreshcalendar(y, m) {
    var x = new Date(y, m, 1);
    var mv = x.getDay();
    var d = x.getDate();
    var dd = null;
    yy = x.getFullYear();
    mm = x.getMonth();
    $cla("year").innerHTML = yy;
    $cla("month").innerHTML = mm + 1 > 9 ? (mm + 1) : '0' + (mm + 1);
    for (var i = 1; i <= mv; i++) {
        dd = $cla("d" + i);
        dd.innerHTML = " ";
        dd.className = "";
    }
    while (x.getMonth() == mm) {
        dd = $cla("d" + (d + mv));
        dd.innerHTML = '<a href="###" onclick="settime(' + d + ');return false">' + d + '</a>';
        if (x.getTime() < today.getTime() || (enddate && x.getTime() > enddate.getTime()) || (startdate && x.getTime() < startdate.getTime())) {
            dd.className = 'expire';
        } else {
            dd.className = 'default';
        }
        if (x.getFullYear() == today.getFullYear() && x.getMonth() == today.getMonth() && x.getDate() == today.getDate()) {
            dd.className = 'today';
            dd.firstChild.title = '<% write(Today); %>';
        }
        if (x.getFullYear() == currday.getFullYear() && x.getMonth() == currday.getMonth() && x.getDate() == currday.getDate()) {
            dd.className = 'checked';
        }
        x.setDate(++d);
    }
    while (d + mv <= 42) {
        dd = $cla("d" + (d + mv));
        dd.innerHTML = " ";
        d++;
    }
    if (addtime) {
        $cla('hour').value = zerofill(hh);
        $cla('minute').value = zerofill(ii);
    }
}
function showdiv(id) {
    var p = getposition($cla(id));
    $cla('calendar_' + id).style.left = p['x'] + 'px';
    $cla('calendar_' + id).style.top = (p['y'] + 16) + 'px';
    $cla('calendar_' + id).style.display = 'block';
}
function zerofill(s) {
    var s = parseFloat(s.toString().replace(/(^[\s0]+)|(\s+$)/g, ''));
    s = isNaN(s) ? 0 : s;
    return (s < 10 ? '0' : '') + s.toString();
}



  

/*end by jqSun20120208*/
</script>
	<tr>
	<td width="23%" height="28" align="right"><b><% write(StartingTimeConf); %></b></td>
	<td width="2%">&nbsp;</td>
	<td width="75%" align="left">
	<input name="year1" id="year1" style="width:138px" type="text" id="year1" class="input" onKeyPress="DateOnly()" onClick = "showcalendar(event,year1);" value="yyyy-mm-dd" onFocus="if(value=='yyyy-mm-dd') {value=''}" onBlur="if (value=='') {value='yyyy-mm-dd'}">
	&nbsp;<% write(To); %>&nbsp; 
	<input name="year2" id="year2" style="width:138px" type="text" id="year2" class="input"  onKeyPress="DateOnly()" onClick = "showcalendar(event,year2);" value="yyyy-mm-dd" onFocus="if(value=='yyyy-mm-dd') {value=''}" onBlur="if (value=='') {value='yyyy-mm-dd'}">
	</td>
	</tr>

	<input type="hidden" name="BeginDate" value="" />
	<input type="hidden" name="EndDate" value="" />
	<tr>
	<td width="23%" height="28" align="right"><b><% write(StartRate); %></b></td>
	<td width="2%">&nbsp;</td>
	<td width="75%">&nbsp;</td>
	</tr>
	<script language="javascript">CreateEftTime();</script>

	<tr>
	<td colspan="3" height="20">&nbsp;</td>
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
<script language="javascript">loadcalendar();</script>
</html>

