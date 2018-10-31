<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("TimeRange");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

var errorstr="";
/////////////////////////////交互变量/////////////////////



<% 
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutputTimeGroupOneInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";


<!--


function setTimeValue2(f, days, timestarts, timestops) { //页面初始化时对生效时间段赋值函数
    if (days == effectDay) {
        f.everyday2.checked = true;
    } else {
        f.everyday2.checked = false;
        for (i = 0; i < days.length; i++) {
            if (days.charAt(i) == "1") f.date2[i].checked = true;
        }
    }
    if (timestarts == effectTime) {
        f.effecttime2[0].checked = true;
    } else {
        f.effecttime2[1].checked = true;
        var index1 = timestarts.indexOf(":");
        var index2 = timestops.indexOf(":");
        f.fromhour2.value = timestarts.substring(0, index1);
        f.fromminute2.value = timestarts.substring(index1 + 1);
        f.endhour2.value = timestops.substring(0, index2);
        f.endminute2.value = timestops.substring(index2 + 1);
    }
    seldate1(2, f, f.everyday2.checked);
    seltime1(2, f, timestarts);
}

function setTimeValue3(f, days, timestarts, timestops) { //页面初始化时对生效时间段赋值函数
    if (days == effectDay) {
        f.everyday3.checked = true;
    } else {
        f.everyday3.checked = false;
        for (i = 0; i < days.length; i++) {
            if (days.charAt(i) == "1") f.date3[i].checked = true;
        }
    }
    if (timestarts == effectTime) {
        f.effecttime3[0].checked = true;
    } else {
        f.effecttime3[1].checked = true;
        var index1 = timestarts.indexOf(":");
        var index2 = timestops.indexOf(":");
        f.fromhour3.value = timestarts.substring(0, index1);
        f.fromminute3.value = timestarts.substring(index1 + 1);
        f.endhour3.value = timestops.substring(0, index2);
        f.endminute3.value = timestops.substring(index2 + 1);
    }
    seldate1(3, f, f.everyday3.checked);
    seltime1(3, f, timestarts);
}
function init(){
	var f=TimeRange_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		
		f.TimeRangeName.value=timeRangeName;
		f.TimeRangeNameOld.value=timeRangeName;
		dateStart = dateStart.replace(/:/g ,"-");
		dateStop = dateStop.replace(/:/g ,"-");
		//f.year1.value=dateStart;
		//f.year2.value=dateStop;

                if(dateStart == "Forever")                                      
                {                                                               
                        f.effectyear[0].checked = true;                         
			seldate0(0,f,"Forever");
                }                                                               
                else                                                            
                {                                                               
                        f.effectyear[1].checked = true;                         
                        f.year1.value=dateStart;                             
                        f.year2.value=dateStop;                              
                }  

		f.Action.value="modify";
		    if(enable[0]=="on")
		    {
			f.timeRange1.checked=true;
			day[0]=(day[0]=="0000000"?"1111111":day[0]);
			timeStart[0]=(timeStart[0]==""?"All":timeStart[0]); 
			//alert("day[0]: "+day[0]+" timeStart[0]: "+timeStart[0]+" timeStop[0]: "+timeStop[0]);
			setTimeValue(f,day[0],timeStart[0],timeStop[0]);
		    }
		    else
		    {
			seldate(f, f.everyday.checked);
			seltime(f, "All");
		    }
		    if(enable[1]=="on")
		    {
			f.timeRange2.checked=true;
			day[1]=(day[1]=="0000000"?"1111111":day[1]);
			timeStart[1]=(timeStart[1]==""?"All":timeStart[1]); 
			//alert("day[1]: "+day[1]+" timeStart[1]: "+timeStart[1]+"timeStop[1]: "+timeStop[1]);
			setTimeValue2(f,day[1],timeStart[1],timeStop[1]);
		    }
		    else
		    {
			seldate1(2, f, f.everyday2.checked);
			seltime1(2, f, "All");
		    }
		    if(enable[2]=="on")
		    {
			f.timeRange3.checked=true;
			day[2]=(day[2]=="0000000"?"1111111":day[2]);
			timeStart[2]=(timeStart[2]==""?"All":timeStart[2]); 
			//alert("day[2]: "+day[2]+" timeStart[2]: "+timeStart[2]+"timeStop[2]: "+timeStop[2]);
			setTimeValue3(f,day[2],timeStart[2],timeStop[2]);
		    }
		    else
		    {
			seldate1(3, f, f.everyday3.checked);
			seltime1(3, f, "All");
		    }
	/*
		group_days=(group_days=="0000000"?"1111111":group_days);
	group_timestarts=(group_timestarts==""?"All":group_timestarts); 
	setTimeValue(tbl,group_days,group_timestarts,group_timestops);
	group_datestarts = group_datestarts.replace(/:/g ,"-");
	group_datestops = group_datestops.replace(/:/g ,"-");
	tbl.year1.value = group_datestarts;
	tbl.year2.value = group_datestops;
	*/	
	}
	else
	{
	seldate(f, f.everyday.checked);
	seltime(f, "All");
	seldate1(2, f, f.everyday2.checked);
	seltime1(2, f, "All");
	seldate1(3, f, f.everyday3.checked);
	seltime1(3, f, "All");
	seldate0(0,f,"Forever");
	}
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

function checkZero(val,str)
{
    if(val==0)
    {
	if((str=="0")||(str=="00")) { alert("<%write(com_dateFErr);%>"); return false;}
    }
    else
    {
	if((str=="0")||(str=="00")) { alert("<%write(com_dateTErr);%>"); return false;}
    }
    return true;
}

/*检查函数*/
function checkData()
{
    var tbl = TimeRange_Config;
    if (!checkName(tbl.TimeRangeName, 20, "<%write(tim_name);%>", 0))
	return false;
    sysDate1=new Date();
    sysDate2=new Date();
    var d1;
    var d2;
    var timezone="";
    if(tbl.year1.value == "yyyy-mm-dd")
    {
	tbl.year1.value = "1990-01-01";
    }
    if(tbl.year2.value == "yyyy-mm-dd")
    {
	tbl.year2.value = "2020-01-01";
    }
    /*
    if( tbl.year1.value == "yyyy-mm-dd" )
    {
	alert("开始日期不能为空！");
	return false;
    }
    */

//分割日期
    TimeArrBegin = tbl.year1.value.split("-");


    if(TimeArrBegin.length != 3)
    {
	alert("<%write(com_dateFErr);%>");
	return false; 
    }
    if(TimeArrBegin[0]==""||TimeArrBegin[1]==""||TimeArrBegin[2]=="")
    {
	alert("<%write(com_dateFErr);%>");
	return false; 
    }
    if( TimeArrBegin[0].length > 4 || TimeArrBegin[1].length > 2 ||TimeArrBegin[2].length > 2 )
    {
	alert("<%write(com_dateFErr);%>");
	return false;
    }
    if(!checkZero(0,TimeArrBegin[1])||!checkZero(0,TimeArrBegin[2]))	
    {
	return false;
    }

    TimeArrBegin = ZeroBegin_Del(TimeArrBegin); 	
	
    if( !CheckDate(TimeArrBegin[0],1988, 2051) )
    {
	alert("<%write(tim_startTip);%>");
	return false;
    }
    if((!CheckDate(TimeArrBegin[1],0,13))||(!CheckDate(TimeArrBegin[2],0,32)))
    {
	alert("<%write(com_dateFErr);%>");
	return false;
    }	
    if(!CheckMonthDay(TimeArrBegin[0], TimeArrBegin[1], TimeArrBegin[2]))
    {
	alert("<%write(com_dateFErr);%>");
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
	alert("<%write(com_dateTNull);%>");
	return false;
    }		
    TimeArrEnd = tbl.year2.value.split("-");
    if(TimeArrEnd.length != 3)
    {
	alert("<%write(com_dateTErr);%>");
	return false;
    }
	

    if(TimeArrEnd[0]==""||TimeArrEnd[1]==""||TimeArrEnd[2]=="")
    {
	alert("<%write(com_dateTErr);%>");
	return false; 
    }


    if( TimeArrEnd[0].length > 4 || TimeArrEnd[1].length > 2 ||TimeArrEnd[2].length > 2 )
    {
	alert("<%write(com_dateTErr);%>");
	return false;
    }
    if(!checkZero(1,TimeArrEnd[1])||!checkZero(1,TimeArrEnd[2]))
    {
	return false;
    }
	
//去掉日期开头的0

    TimeArrEnd = ZeroBegin_Del(TimeArrEnd);
	
	
//保持和原来页面相同

    if( !CheckDate(TimeArrEnd[0],1988, 2051) )
    {
	alert("<%write(tim_endTip);%>");
	return false;
    }

    if((!CheckDate(TimeArrEnd[1],0,13))||(!CheckDate(TimeArrEnd[2],0,32)))
    {
	alert("<%write(com_dateTErr);%>");
	return false;
    }
    

    if(!CheckMonthDay(TimeArrEnd[0], TimeArrEnd[1], TimeArrEnd[2]))
    {
        alert("<%write(com_dateTErr);%>");
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
	alert("<%write(com_dateCmp);%>");
	return false;
    }
	return true;
}

function seldate1(val,f,flag)
{
    if(val==2)
    {
	var len = f.date2.length;
	for (i = 0; i < len; i++) {
	    f.date2[i].disabled = (flag == true ? true: false);
	}
    }
    if(val==3)
    {
	var len = f.date3.length;
	for (i = 0; i < len; i++) {
	    f.date3[i].disabled = (flag == true ? true: false);
	}
    }
}

function seldate0(val,f,flag)
{
    if(val==0)
    {
	if(edit_Names!=""&&edit_Names!=null){//alert();
		    if(enable[0]=="on")
		    {
			f.timeRange1.checked=true;
			day[0]=(day[0]=="0000000"?"1111111":day[0]);
			timeStart[0]=(timeStart[0]==""?"All":timeStart[0]); 
			//alert("day[0]: "+day[0]+" timeStart[0]: "+timeStart[0]+" timeStop[0]: "+timeStop[0]);
			setTimeValue(f,day[0],timeStart[0],timeStop[0]);
		    }
		    else
		    {
			seldate(f, f.everyday.checked);
			seltime(f, "All");
		    }
		    if(enable[1]=="on")
		    {
			f.timeRange2.checked=true;
			day[1]=(day[1]=="0000000"?"1111111":day[1]);
			timeStart[1]=(timeStart[1]==""?"All":timeStart[1]); 
			//alert("day[1]: "+day[1]+" timeStart[1]: "+timeStart[1]+"timeStop[1]: "+timeStop[1]);
			setTimeValue2(f,day[1],timeStart[1],timeStop[1]);
		    }
		    else
		    {
			seldate1(2, f, f.everyday2.checked);
			seltime1(2, f, "All");
		    }
		    if(enable[2]=="on")
		    {
			f.timeRange3.checked=true;
			day[2]=(day[2]=="0000000"?"1111111":day[2]);
			timeStart[2]=(timeStart[2]==""?"All":timeStart[2]); 
			//alert("day[2]: "+day[2]+" timeStart[2]: "+timeStart[2]+"timeStop[2]: "+timeStop[2]);
			setTimeValue3(f,day[2],timeStart[2],timeStop[2]);
		    }
		    else
		    {
			seldate1(3, f, f.everyday3.checked);
			seltime1(3, f, "All");
		    }
	}
	f.year1.disabled = f.year2.disabled = (flag == "Forever" ? true: false);
	    f.timeRange1.disabled = false;
	    f.everyday.disabled = false;
	    f.effecttime[0].disabled = false;
	    f.effecttime[1].disabled = false;
	    f.timeRange2.disabled = false;
	    f.everyday2.disabled = false;
	    f.effecttime2[0].disabled = false;
	    f.effecttime2[1].disabled = false;
	    f.timeRange3.disabled = false;
	    f.everyday3.disabled = false;
	    f.effecttime3[0].disabled = false;
	    f.effecttime3[1].disabled = false;
	if(flag == "Forever") 
	{
	    f.effectyear[0].checked = true;
	    f.timeRange1.disabled = true;
	    seldate(f, true);
	    seltime(f, "All");
	    f.everyday.disabled = true;
	    f.effecttime[0].disabled = true;
	    f.effecttime[1].disabled = true;
	    f.timeRange2.disabled = true;
	    seldate1(2, f, true);
	    seltime1(2, f, "All");
	    f.everyday2.disabled = true;
	    f.effecttime2[0].disabled = true;
	    f.effecttime2[1].disabled = true;
	    f.timeRange3.disabled = true;
	    seldate1(3, f, true);
	    seltime1(3, f, "All");
	    f.everyday3.disabled = true;
	    f.effecttime3[0].disabled = true;
	    f.effecttime3[1].disabled = true;
	}
    }
}

function seltime1(val,f,flag)
{
    if(val==2)
    {
	f.fromhour2.disabled = f.fromminute2.disabled = f.endhour2.disabled = f.endminute2.disabled = (flag == effectTime ? true: false);
    }
    if(val==3)
    {
	f.fromhour3.disabled = f.fromminute3.disabled = f.endhour3.disabled = f.endminute3.disabled = (flag == effectTime ? true: false);
    }
}

function getTimeRangeValue(f)
{
    //get时间段二的值
    var len1 = f.date2.length;
    f.day2.value = "";
    if (f.everyday2.checked == false) {
        for (i = 0; i < len1; i++) {
            f.day2.value += (f.date2[i].checked == true ? "1": "0");
        }
    } else {
        f.day2.value = effectDay;
    }
    if (f.effecttime2[0].checked == false) {
        if (f.fromhour2.value == "00" && f.fromminute2.value == "00" && f.endhour2.value == "00" && f.endminute2.value == "00") {
            f.timestart2.value = effectTime;
            f.timestop2.value = "0";
        } else {
            f.timestart2.value = f.fromhour2.value + ":" + f.fromminute2.value;
            f.timestop2.value = f.endhour2.value + ":" + f.endminute2.value;
        }

    } else {
        f.timestart2.value = effectTime;
        f.timestop2.value = "0";
    }

    //get 时间段三的值
    var len3 = f.date3.length;
    f.day3.value = "";
    if (f.everyday3.checked == false) {
        for (i = 0; i < len3; i++) {
            f.day3.value += (f.date3[i].checked == true ? "1": "0");
        }
    } else {
        f.day3.value = effectDay;
    }
    if (f.effecttime3[0].checked == false) {
        if (f.fromhour3.value == "00" && f.fromminute3.value == "00" && f.endhour3.value == "00" && f.endminute3.value == "00") {
            f.timestart3.value = effectTime;
            f.timestop3.value = "0";
        } else {
            f.timestart3.value = f.fromhour3.value + ":" + f.fromminute3.value;
            f.timestop3.value = f.endhour3.value + ":" + f.endminute3.value;
        }

    } else {
        f.timestart3.value = effectTime;
        f.timestop3.value = "0";
    }
}

function save()
{
    var f=TimeRange_Config;
    if(!checkData())
    {
	return false;
    }
    f.day.value="";
    getTimeValue(f);
    getTimeRangeValue(f);
    if(f.timeRange1.checked==true)
    {
    if(f.day.value=="0000000"&&!f.everyday.checked){
	alert("<%write(tim_selOne);%>");
	f.day.value="";
	return;
    }
    }
    if(f.timeRange2.checked==true)
    {
    if(f.day2.value=="0000000"&&!f.everyday2.checked){
	alert("<%write(tim_selTwo);%>");
	f.day2.value="";
	return;
    }
    }
    if(f.timeRange3.checked==true)
    {
    if(f.day3.value=="0000000"&&!f.everyday3.checked){
	alert("<%write(tim_selThree);%>");
	f.day3.value="";
	return;
    }
    }
    
    /*
    alert("name: "+f.TimeRangeName.value);
    alert("datestart: "+f.year1.value);
    alert("datestop: "+f.year2.value);
    alert("checked:"+f.timeRange1.value);
    alert("checked:"+f.timeRange2.value);
    alert("checked:"+f.timeRange3.value);
    if(f.timeRange1.checked==true)
    {
    alert("day: "+f.day.value);
    alert("timestart1: "+f.timestart.value);
    alert("timestop1: "+f.timestop.value);
    }
    */
    f.action="/goform/formTimeGroupConfig";
    f.submit();
}

function Reset(){
	TimeRange_Config.reset();
	init();
}
//-->
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
<body onLoad=""onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(com_timeConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="TimeRange_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
	   <td width="24%" height="25" align="right"><%write(tim_name);%></td>
	   <td width="4%"><!--<img src="images/qt.gif" width="7" height="9">--></td>
	   <td width="62%"><input name="TimeRangeName" type="text" maxlength="20" size="16"></td>
	   </tr> <input type="hidden" name="TimeRangeNameOld" >
<script language="javascript">
		/*added by jqSun20120208*/


/*
	[Discuz!] (C)2001-2007 Comsenz Inc.
	This is NOT a freeware, use is subject to license terms

	$RCSfile: TimeRange_edit.asp,v $
	$Revision: 1.1 $
	$Date: 2015/02/12 15:59:30 $
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
    s += '<td colspan="1" class="header"><a href="#" onclick="refreshcalendar(yy-1, mm);return false" title="<%write(com_preYear);%>"><<</a>&nbsp;&nbsp;<a href="#" onclick="refreshcalendar(yy, mm-1);return false" title="<%write(com_preMonth);%>"><</a></td>';
    s += '<td colspan="5" style="text-align: center;width:150px" class="header"><a href="#" onclick="showdiv(\'year\');_cancelBubble(event);return false" title="<%write(com_selYear);%>" id="year"></a><a id="month" title="<%write(com_selMonth);%>" href="#" onclick="showdiv(\'month\');_cancelBubble(event);return false"></a></td>';
    s += '<td colspan="1" class="header"><A href="#" onclick="refreshcalendar(yy, mm+1);return false" title="<%write(com_nextMonth);%>">></A>&nbsp;&nbsp;<A href="#" onclick="refreshcalendar(yy+1, mm);return false" title="<%write(com_nextYear);%>">>></A></td>';
    s += '</tr>';

    s += '<tr class="category"><td><%write(com_sun);%></td><td><%write(com_mon);%></td><td><%write(com_tue);%></td><td><%write(com_wed);%></td><td><%write(com_thu);%></td><td><%write(com_fri);%></td><td><%write(com_sat);%></td></tr>';
    for (var i = 0; i < 6; i++) {
        s += '<tr class="altbg2">';
        for (var j = 1; j <= 7; j++)
            s += "<td id=d" + (i * 7 + j) + " height=\"19\">0</td>";
        s += "</tr>";
    }
    s += '<tr id="hourminute"><td colspan="7" align="center"><input type="text" size="1" value="" id="hour" onKeyUp=\'this.value=this.value > 23 ? 23 : zerofill(this.value);controlid.value=controlid.value.replace(/\\d+(\:\\d+)/ig, this.value+"$1")\'> 点 <input type="text" size="1" value="" id="minute" onKeyUp=\'this.value=this.value > 59 ? 59 : zerofill(this.value);controlid.value=controlid.value.replace(/(\\d+\:)\\d+/ig, "$1"+this.value)\'> 分</td></tr>';
 	s += '<tr class="header"><td colspan="4" align="left"><%write(com_today);%><a href="#" onclick="settoday();return false;">'+today.getFullYear() + '-' + (today.getMonth()+1) + '-' + today.getDate() +'</a></td><td align="right" colspan="3"><a href="#" onclick="closecalendar();return false"><%write(com_close);%></a> </td></tr>';
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
            dd.firstChild.title = '<%write(tim_today);%>';
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
	<td width="24%" height="28" align="right"><b><%write(tim_date);%></b></td>
	<td width="4%">&nbsp;</td>
	<td width="62%" align="left">
	<input type="radio" name="effectyear" value="Forever" onClick="seldate0(0,this.form,this.value)" checked>&nbsp;<%write(tim_forever);%>
	</td>
	</tr>


       <tr>
	<td width="24%" height="28" align="right"></td>
	<td width="4%">&nbsp;</td>
	<td width="62%" align="left">
	<input type="radio" name="effectyear" value="Daterange" onClick="seldate0(0,this.form,this.value)" >&nbsp;
	<input name="year1" id="year1" style="width:138px" type="text" id="year1" class="input" onKeyPress="DateOnly()" onClick = "showcalendar(event,year1);" value="yyyy-mm-dd" onFocus="if(value=='yyyy-mm-dd') {value=''}" onBlur="if (value=='') {value='yyyy-mm-dd'}">
	&nbsp;<%write(com_toIP);%>&nbsp; 
	<input name="year2" id="year2" style="width:138px" type="text" id="year2" class="input"  onKeyPress="DateOnly()" onClick = "showcalendar(event,year2);" value="yyyy-mm-dd" onFocus="if(value=='yyyy-mm-dd') {value=''}" onBlur="if (value=='') {value='yyyy-mm-dd'}">
	</td>
	</tr>
	<input type="hidden" name="BeginDate" value="" />
	<input type="hidden" name="EndDate" value="" />
	
	<tr>
	<td width="24%" height="28" align="right"><b><%write(tim_rangeOne);%></b></td>
	<td width="4%">&nbsp;</td>
	<td width="62%"><input type="checkbox" name="timeRange1" id="timeRange1"></td>
	</tr>
	<script language="javascript">CreateEftTime();</script>
	<tr>
	<td colspan="3" height="20">&nbsp;</td>
	</tr>

	<tr>
	<td width="24%" height="28" align="right"><b><%write(tim_rangeTwo);%></b></td>
	<td width="4%">&nbsp;</td>
	<td width="62%"><input type="checkbox" name="timeRange2" id="timeRange2"></td>
	</tr>
	
	<tr><td align="right" height="25" ><%write(com_date);%></td><td >&nbsp;</td><td ><input type="checkbox" name="everyday2" onClick="seldate1(2,this.form,this.checked)" checked="checked" value="everyday2"><%write(com_every);%></td></tr>
	<tr><td align="right" height="25" >&nbsp;</td><td width="2%">&nbsp;</td><td><input type="checkbox" name="date2" value="1"><%write(com_monday);%>&nbsp;<input type="checkbox" name="date2" value="2"><%write(com_tuesday);%>&nbsp;<input type="checkbox" name="date2" value="3"><%write(com_wednesday);%>&nbsp;<input type="checkbox" name="date2" value="4"><%write(com_thursday);%>&nbsp;<input type="checkbox" name="date2" value="5"><%write(com_friday);%>&nbsp;<input type="checkbox" name="date2" value="6"><%write(com_saturday);%>&nbsp;<input type="checkbox" name="date2" value="7"><%write(com_sunday);%></td></tr>
	<tr><td align="right" height="25"><%write(com_time);%></td><td width="2%">&nbsp;</td><td ><input type="radio" name="effecttime2" value="All" onClick="seltime1(2,this.form,this.value)" checked>&nbsp;<%write(com_allDay);%></td></tr>
	<tr><td align="right" height="25"  >&nbsp;</td><td width="2%">&nbsp;</td><td><input type="radio" name="effecttime2" value="timerange" onClick="seltime1(2,this.form,this.value)">&nbsp;<%write(com_from);%>&nbsp;<select name="fromhour2" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="fromminute2" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<%write(com_toIP);%>&nbsp;<select name="endhour2" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="endminute2" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="day2"><input type="hidden" name="timestart2"><input type="hidden" name="timestop2">
	
	<tr>
	<td colspan="3" height="20">&nbsp;</td>
	</tr>
	<tr>
	<td width="24%" height="28" align="right"><b><%write(tim_rangeThree);%></b></td>
	<td width="4%">&nbsp;</td>
	<td width="62%"><input type="checkbox" name="timeRange3" id="timeRange3"></td>
	</tr>
	
	<tr><td align="right" height="25" ><%write(com_date);%></td><td >&nbsp;</td><td ><input type="checkbox" name="everyday3" onClick="seldate1(3,this.form,this.checked)" checked="checked" value="everyday3"><%write(com_every);%></td></tr>
	<tr><td align="right" height="25" >&nbsp;</td><td width="2%">&nbsp;</td><td><input type="checkbox" name="date3" value="1"><%write(com_monday);%>&nbsp;<input type="checkbox" name="date3" value="2"><%write(com_tuesday);%>&nbsp;<input type="checkbox" name="date3" value="3"><%write(com_wednesday);%>&nbsp;<input type="checkbox" name="date3"    value="4"><%write(com_thursday);%>&nbsp;<input type="checkbox" name="date3" value="5"><%write(com_friday);%>&nbsp;<input type="checkbox" name="date3" value="6"><%write(com_saturday);%>&nbsp;<input   type="checkbox" name="date3" value="7"><%write(com_sunday);%></td></tr>
	<tr><td align="right" height="25"><%write(com_time);%></td><td width="2%">&nbsp;</td><td ><input type="radio" name="effecttime3" value="All" onClick="seltime1(3,this.form,this.value)" checked>&nbsp;<%write(com_allDay);%></td></tr>
	<tr><td align="right" height="25"  >&nbsp;</td><td width="2%">&nbsp;</td><td><input type="radio" name="effecttime3" value="timerange" onClick="seltime1(3,this.form,this.value)">&nbsp;<%write(com_from);%>&nbsp;<select name="fromhour3" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="fromminute3" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<%write(com_toIP);%>&nbsp;<select name="endhour3" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="endminute3" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="day3"><input type="hidden" name="timestart3"><input type="hidden" name="timestop3">
	
	<tr>
	<td colspan="3" height="20">&nbsp;</td>
	</tr>

	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#TimeRange')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('TimeRange.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
  </tr>
</table>
  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr><script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
<script language="javascript">loadcalendar();</script>
</html>
