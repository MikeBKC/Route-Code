<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("PPPoEServer");%>
<%langTran("JS");%>
<%langTran("PPPoE_edit");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
<%coefficient();%>
<%getUserGrpList();%>

<%aspPppoeMaxSess();%>
<%
var edit_Name="";
edit_Name=Form("id");
if ( edit_Name!="") aspPppoeOneUserInfo(edit_Name);//需要修改
%> 

var edit_Names = "<% write(Form("id"));%>";//需要修改的账号名
var errorstr = "";

function changeRate(ctl,ctl2){	
    var selValue = getListValue(ctl);
    selValue=(selValue=="zidingyi"?"":selValue);
    ctl2.value = selValue;	
}

function changeSelect(ctrl1, ctrl2)
{

    if (ctrl1.value == "")
    {
	var selectIndex = 0;
    } else {
	var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);
    }
    if (selectIndex != -1)
    {
	ctrl2.selectedIndex = selectIndex;
    } else {
	ctrl2.selectedIndex = 1;
    }
}
function fillacc(index)
{
    if(index)
    {
    var f=PPPoE_Config;
    sysDate=new Date(UTCtoLocal(eval(starttm),0) * 1000);
    f.year1.value = sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
    if (f.year1.value == "1970-1-1") 
    {
	f.year1.value ="";
    }
    sysDate=new Date(UTCtoLocal(eval(endtm),0) * 1000);
    f.year2.value = sysDate.getFullYear()+"-"+eval(sysDate.getMonth()+1)+"-"+sysDate.getDate();
    if (f.year2.value == "1970-1-0") 
    {
	f.year2.value ="";
    }
    }
}
function init(){
    var f=PPPoE_Config;
	
    if(edit_Names!=""&&edit_Names!=null){ 
	//if(bindmacs == "000000000000")
		//bindmacs = "";
	f.pppoename.value=usernames;
	f.pppoenameold.value=usernames;
	f.passwd1.value = passwd1s;
	if(AccMACBinds == "")
	    AccMACBinds = "nobind";
	f.AccMACBind.value = AccMACBinds;
	f.bindmac.value = bindmacs;
	f.bindmac2.value = bindmac2s;
	f.bindmac3.value = bindmac3s;
	f.bindmac4.value = bindmac4s;
	if(accmaxconvs == 0)
	    accmaxconvs = 1;
	f.accmaxconv.value = accmaxconvs;
	f.aspip.value=aspips;
	f.Action.value="modify";
	f.instIndex.value = instIndexs;
	if(accMode == "date")
	{
	    f.enabled.checked=true;
	    fillacc(1);
	}
	else
	{
	    f.enabled.checked=false;
	    fillacc(0);
	}
	selectmacbind(f.AccMACBind);
	f.remark.value = remarks;
	f.remarkold.value = remarks;
	f.MaxDownRatetxt.value=dRate.replace(/k/g,"");
	f.uMaxDownRatetxt.value=uRate.replace(/k/g,"");
    }
    else
    {
	f.MaxDownRatetxt.value=0;
	f.uMaxDownRatetxt.value=0;
    }
    changeSelect(f.MaxDownRatetxt, f.MaxDownRatew);
    changeSelect(f.uMaxDownRatetxt, f.uMaxDownRatew);
}

function save(){
    if(checkData()){
	//PPPoE_Config.dRate.value=PPPoE_Config.MaxDownRatetxt.value;
	//PPPoE_Config.uRate.value=PPPoE_Config.uMaxDownRatetxt.value;
	if((PPPoE_Config.AccMACBind.value == "nobind") || (PPPoE_Config.AccMACBind.value == "autobind"))
	{
	    PPPoE_Config.bindmac.value = "000000000000";
	    PPPoE_Config.bindmac2.value = "000000000000";
	    PPPoE_Config.bindmac3.value = "000000000000";
	    PPPoE_Config.bindmac4.value = "000000000000";
	}
	PPPoE_Config.dRate.value=PPPoE_Config.MaxDownRatetxt.value;
	PPPoE_Config.uRate.value=PPPoE_Config.uMaxDownRatetxt.value;
	PPPoE_Config.action="/goform/formPppoeUserEdit";
	PPPoE_Config.submit();
    }
}

function selectmacbind(ctl)
{
    showTable(bindtbl,0);
    switch(ctl.value){
	case "nobind" :  
	    showTable(bindtbl,0);
	showTable(moreMac,0);
	break;
	case "autobind":  
	    showTable(bindtbl,0);
	showTable(moreMac,0);
	break;
	case "handbind":   
	    showTable(bindtbl,1);
	if (PPPoE_Config.More.checked == 1)
	{
	    showTable(moreMac, 1);
	}
	else
	{
	    showTable(moreMac, 0);
	}
	break;
    }
}

function DateOnly()
{
    if (!(((window.event.keyCode >= 48) && (window.event.keyCode <= 57)) || (window.event.keyCode == 45)))
    {
	window.event.keyCode = 0;
    }
}


function checkMac_allowZero(cntrl, tips, allownull, noChange) {
	var retips = '';
    var num = cntrl.value.toUpperCase();
    num = trimstr(num);
    if (num == "") {
        if (!eval(allownull)) {
            retips = tips + "<% write(JSAlert33); %>";
        } 
    } else {
        var length = num.length;
        for (var i = 0; i < length; i++) {
            onenum = num.substring(i, i + 1);
            twonum = num.substring(i + 1, i + 2);
            if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) {
                if (onenum == "-" || onenum == ":") {
                    if (twonum == "-" || twonum == ":") {
                        length = 0;
                        break;
                    } else {
                        num = num.substring(0, i) + num.substring(i + 1, length);
                        length--;
                    }
                } else {
                    length = 0;
                    break;
                }
            }
        }
        if (length != 12) {
            retips = tips + "<% write(JSAlert35); %>";
        } 
    }
	if (retips != '') {
        inputErro(cntrl, retips);
        return false;
    } else {
        inputRight(cntrl);
        if (noChange != 1) cntrl.value = num;
        return true;
    }
}

function checkData(){
    var f=PPPoE_Config;
    sysDate1=new Date();
    sysDate2=new Date();
    var d1;
    var d2;
    var timezone="";
    if(!checkEnglishName_allowZero(f.pppoename,31,"<%write(com_user);%>",0)) return false;
    if(f.pppoename.value.indexOf("-")!=-1)
    {
	alert("<%write(com_except);%>");
	return false;
    }
	if(!checkPassword1(f.passwd1, 31, 0, 0)) return false;	//检查密码：不能超过31个字符、不能为空、应为除% \' \\ \"  , ；; 以及空格之外的字符。
    if (f.aspip.value=="0.0.0.0"){ alert("<%write(pps_ipNull);%>");return false};
    if (!checkIP(f.aspip,"<%write(pps_ip);%>",1)) return false;
    if((f.aspip.value != "") && (f.accmaxconv.value > 1))
    {
	alert("<%write(pps_ipaccmax);%>");
	return false
    }
    if(f.remark.value.len > 31)
    {
	alert("<%write(pps_remarkLen);%>");
	return false;
    }
    if (!checkRemark(f.remark,31,"<%write(pps_remark);%>",1))
    {
        return false;
    }
    if(f.enabled.checked == true)
    {
    if( f.year1.value == "yyyy-mm-dd" )
    {
	alert("<%write(com_dateFNull);%>");
	return false;
    }
    if(f.remark.value.len > 31)
    {
	alert("<%write(pps_remarkLen);%>");
	return false;
    }
    if (!checkRemark(f.remark,31,"<%write(pps_remark);%>",1))
    {
        return false;
    }

//分割日期
    TimeArrBegin = f.year1.value.split("-");


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
	
     if(!re_checkSpecialNumber(f.year1, "<%write(pps_dateStart);%>")) return false;
     if(!re_checkSpecialNumber(f.year2, "<%write(pps_dateEnd);%>")) return false;
    if( !CheckDate(TimeArrBegin[0],1988, 2038) )
    {
	alert("<%write(com_dateStart);%>");
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
    f.BeginDate.value=d1;
	
	if( f.year2.value == "yyyy-mm-dd" )
    {
	alert("<%write(com_dateTNull);%>");
	return false;
    }		
    TimeArrEnd = f.year2.value.split("-");
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

    if( !CheckDate(TimeArrEnd[0],1988, 2038) )
    {
	alert("<%write(com_dateEnd);%>");
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
    f.EndDate.value=d2;
    }
    if(d1 > d2)
    {
	alert("<%write(com_dateCmp);%>");
	return false;
    }
    
    if(! checkNumber(f.accmaxconv,1,smaxconvs,"<%write(pps_accmax);%>",0)) return false;
    if(f.MaxDownRatetxt.value!="0")
    if(! checkNumber(f.MaxDownRatetxt,coefficient,102400,"<%write(com_downloadLimit);%>",0)) return false;
    if(f.uMaxDownRatetxt.value!="0")
    if(! checkNumber(f.uMaxDownRatetxt,coefficient,102400,"<%write(com_uploadLimit);%>",0)) return false;
    var down_rate = parseInt(f.MaxDownRatetxt.value);
    var up_rate = parseInt(f.uMaxDownRatetxt.value);
	if(f.AccMACBind.value == "handbind")
	{
	    if(!checkMac_allowZero(f.bindmac,"<%write(pps_mac);%>",1)) return false;
	    if(!checkMac_allowZero(f.bindmac2,"<%write(pps_mac);%>2",1)) return false;
	    if(!checkMac_allowZero(f.bindmac3,"<%write(pps_mac);%>3",1)) return false;
	    if(!checkMac_allowZero(f.bindmac4,"<%write(pps_mac);%>4",1)) return false;
	    if((f.bindmac.value == "") && ((f.bindmac2.value != "") || (f.bindmac3.value != "") || (f.bindmac4.value != "")))
	    {
		alert("<%write(pps_bindmacfirst);%>");
		return false;
	    }
	}
	if((f.AccMACBind.value == "handbind") && (f.accmaxconv.value > 4))
	{
	    alert("<%write(pps_handbindaccmax);%>");
	    return false
	}
	if((f.AccMACBind.value == "autobind") && (f.accmaxconv.value > 1))
	{
	    alert("<%write(pps_autobindaccmax);%>");
	    return false
	}
		
    return true;
}

/*计费模式选择*/
function selectAccMode(ctl)
{
    switch(ctl.value)
    {
	case "accNone":
	    showTable(accTableTime,0);
	break;

	case "accDate":
	    showTable(accTableTime,1);
	break;

    }
}
function Reset(){
    PPPoE_Config.reset();
    init();
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
<body onbeforeunload="logOut()">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle(_("<%write(com_pppAcc);%>"));</script>
<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr);</script>
<tr valign="top">
<td><form action="/goform/" onSubmit="return " name="PPPoE_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
<td align="center" class="style7">
<table border="0" width="600" align='center'>
<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add"><input type="hidden" name="instIndex" >&nbsp;</td></tr>



<tr>
<td width="35%" align="right" height="25" id="name"><%write(com_user);%></td>
<td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
<td width="27%"  height="20"><input name="pppoename" type="text" style="width:138px" maxlength="31" onChange="showfiledtip(re_checkEnglishName(this,'<%write(com_user);%>'),yutd1)"></td><td width="36%" id="yutd1"></td>
</tr><input type="hidden" name="pppoenameold" >
<tr>
<td  align="right" id="password"><%write(com_passwd);%></td>
<td><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
<td height="20"><input name="passwd1"  type="password" style="width:138px" maxlength="31" onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd2)"></td><td  id="yutd2"></td>
</tr>
<!--<tr >
<td  align="right" id="password2" >确认密码</td>
<td><img src="images/qt.gif" width="7" height="9"></td>
<td height="20"><input name="passwd2"  type="password" style="width:138px" maxlength="11" onChange="showfiledtip(re_checkPassword1(this,11),passwd2tb)"></td><td  id="passwd2tb"></td>

</tr>-->

<!--<tr>
<td  align="right" id="pppoeremark">描述</td>
<td> </td>
<td height="20"><input  name="remark" style="width:138px"  type="text" maxlength="20" size="18" onChange="showfiledtip(re_checkName(this,'描述'),yutd3)"></td><td  id="yutd3"></td>
</tr>
-->
<!--
<td  align="right"  >计费模式</td>
<td> </td>
<td height="20"> <select name="accMode"  onChange="selectAccMode(this)">
<option  value="accNone">无</option>
<option  value="accDate">按日期</option>
<option  value="accHour">按小时</option>
<option  value="accTraffic">按流量</option>
</select></td> 
</tr>
<tr>
<td  align="right" id="aspip">账号开通日期</td>
<td> </td>
<td height="20"><input  name="aspip" style="width:138px"  type="text" onChange="showfiledtip(re_checkIP_empty(this,'固定IP地址'),aspiptd)" onKeyPress="ipOnly()" size="18" maxlength="15" ></td><td  id="aspiptd"></td>
</tr>
<tr>
<td  align="right" id="aspip">账号停用日期</td>
<td> </td>
<td height="20"><input  name="aspip" style="width:138px"  type="text" onChange="showfiledtip(re_checkIP_empty(this,'固定IP地址'),aspiptd)" onKeyPress="ipOnly()" size="18" maxlength="15" ></td><td  id="aspiptd"></td>
</tr>
<tr>
<td  align="right" id="aspip">小时数</td>
<td> </td>
<td height="20"><input  name="aspip" style="width:138px"  type="text" onChange="showfiledtip(re_checkIP_empty(this,'固定IP地址'),aspiptd)" onKeyPress="ipOnly()" size="18" maxlength="15" ></td><td  id="aspiptd"></td>
</tr>
 
-->
<tr style="display">
<td  align="right"  ><%write(pps_ipmacbind);%></td>
<td width="2%"></td>
<td height="20"><div>
<select name="AccMACBind" class="input" onChange="selectmacbind(this)">
<option  value="nobind"><%write(pps_nobind);%></option>
<option  value="autobind"><%write(pps_autobind);%></option>
<option  value="handbind"><%write(pps_handbind);%></option>
</select>
</div></td>
</tr>

<tr><td colspan="3">
<table width="100%"  border="0" cellspacing="0" cellpadding="0" id="bindtbl" style="display:none ">
<tr>
<td width="55%" align="right" height="25"><%write(pps_mac);%></td>
<td width="4%" height="9"></td>
<td height="20"><input name="bindmac" style="width:138px" type="text" id="bindmac" onKeyPress="macOnly()" /></td> 
</tr>
<tr>
<td height="25" align="right"></td>
<td height="9">&nbsp;</td>
<td height="20"><input name="More" type="checkbox" id="More" onClick="showTable(moreMac,this.checked)">
<%write(pps_more_mac);%></td>
</tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellpadding="0" id="moreMac" style="display:none">
<tr>
<td width="55%" align="right" height="25"><%write(pps_mac);%>2</td>
<td width="4%" height="9"></td>
<td height="20"><input name="bindmac2" style="width:138px" type="text" id="bindmac2" onKeyPress="macOnly()" /></td> 
</tr>
<tr>
<td width="55%" align="right" height="25"><%write(pps_mac);%>3</td>
<td width="4%" height="9"></td>
<td height="20"><input name="bindmac3" style="width:138px" type="text" id="bindmac3" onKeyPress="macOnly()" /></td> 
</tr>
<tr>
<td width="55%" align="right" height="25"><%write(pps_mac);%>4</td>
<td width="4%" height="9"></td>
<td height="20"><input name="bindmac4" style="width:138px" type="text" id="bindmac4" onKeyPress="macOnly()" /></td> 
</tr>
</table>
</td></tr>

<tr style="display">
<td  align="right"  ><%write(pps_accmax);%></td>
<td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
<td height="20"><input  name="accmaxconv" style="width:138px"  type="text" onChange="showfiledtip(re_checkNumber(this,1,smaxconvs,'<%write(pps_accmax);%>'),yutd3)"  size="18" maxlength="3" value="1" ></td><td  id="yutd3"></td>

</tr>
<tr>
<td  align="right" id="aspip"><%write(pps_ip);%></td>
<td> </td>
<td height="20"><input  name="aspip" style="width:138px"  type="text" onChange="showfiledtip(re_checkIP_empty(this,'<%write(pps_ip);%>'),yutd4)" onKeyPress="ipOnly()" size="18" maxlength="15" ></td><td  id="yutd4"></td>
</tr>

<tr>
<td align="right" ><% write(pps_addGrp); %></td>
<td></td>
<td height="20">
   <select name="UserGroupName" id="UserGroupName">
   <option value=""><% write(pps_wu); %></option>
   <script language="javascript">
   for(var i=0;i<UserGroupNames.length;i++)
   {
   		document.write('<option value='+UserGroupNames[i]+'>'+UserGroupNames[i]+'</option>');
   }
   </script>
   </select>
 </td>
 </tr>

 <tr name="mode" id="mode" style="display">
   <td width="35%" height="25" align="right"><%write(pps_mode);%></td>
   <td width="2%" >&nbsp;</td>
   <td  width="27%" height="30"><input type="checkbox" name="enabled" id="enabled" />
   <!--<div align="left">
   <select name="accMode" class="input" onChange="selectAccMode(this)">
   <option  value="accNone">无</option>
   <option  value="accDate">按日期</option>
   <option  value="accTraffic">按流量</option>
   </select></div>-->
   </td>
            </tr>
 <script language="javascript">
		/*added by jqSun20120208*/


/*
	[Discuz!] (C)2001-2007 Comsenz Inc.
	This is NOT a freeware, use is subject to license terms

	$RCSfile: PPPoE_edit.asp,v $
	$Revision: 1.1 $
	$Date: 2015/02/12 15:59:28 $
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
            dd.firstChild.title = '<%write(pps_today);%>';
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
                <td colspan="4"><table width="100%"  border="0" cellspacing="0" cellpadding="0" id = "accTableTime" style="display:">
                        <tr>
                            <td width="35%" height="25" align="right"><%write(pps_start);%></td>
                            <td width="2%">&nbsp;</td>
                            <td width="33%">&nbsp;<input name="year1" id="year1" style="width:138px" type="text" id="year1" class="input" onKeyPress="DateOnly()" onClick = "showcalendar(event,year1);" value="yyyy-mm-dd" onFocus="if(value=='yyyy-mm-dd') {value=''}" onBlur="if (value=='') {value='yyyy-mm-dd'}">
                                    </td>
							
                        </tr>
			 <tr>
                            <td width="35%" height="25" align="right"><%write(pps_end);%></td>
                            <td width="2%">&nbsp;</td>
                            <td width="33%">&nbsp;<input name="year2" id="year2" style="width:138px" type="text" id="year2" class="input"  onKeyPress="DateOnly()" onClick = "showcalendar(event,year2);" value="yyyy-mm-dd" onFocus="if(value=='yyyy-mm-dd') {value=''}" onBlur="if (value=='') {value='yyyy-mm-dd'}">
                                    </td>
									
                        </tr>
<tr>
<td colspan="4"><table width="100%"  border="0" cellspacing="0" cellpadding="0" id = "accTableTraffic" style="display:">
<tr style="display:">
<td align="right" height="25" width="35%"><%write(com_uploadLimit);%></td>
<td width="2%" >&nbsp;</td>
<td width="63%">&nbsp;<input type="text" name="uMaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,PPPoE_Config.uMaxDownRatew)" size="8" maxlength="6">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="uMaxDownRatew" class="list" onChange="changeRate(this,PPPoE_Config.uMaxDownRatetxt)">
<option value="0"><%write(com_noLimit);%></option>
<option value="zidingyi"><%write(com_defSelf);%></option>
<script language="javascript">
for(i=0;i<RateArr.length;i++)
document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
</script>
</select>
    <input type="hidden" name="page" value="limit"><input type="hidden" name="dRate"><input type="hidden" name="dCeil" value="0"><input type="hidden" name="uRate"><input type="hidden" name="uCeil" value="0"><input type="hidden" name="policy_name"></td>
</tr>
<tr style="display:">
<td align="right" height="25" width="35%"><%write(com_downloadLimit);%></td>
<td width="2%" >&nbsp;</td>
<td width="63%">&nbsp;<input type="text" name="MaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,PPPoE_Config.MaxDownRatew)" size="8" maxlength="6">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="MaxDownRatew" class="list" onChange="changeRate(this,PPPoE_Config.MaxDownRatetxt)">
<option value="0"><%write(com_noLimit);%></option>
<option value="zidingyi"><%write(com_defSelf);%></option>
<script language="javascript">
for(i=0;i<RateArr.length;i++)
document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
</script>
</select>&nbsp;</td>
</tr>
</table></td>
</tr>

			 <tr>
                            <td width="35%" height="25" align="right"><%write(pps_remark);%></td>
                            <td width="2%">&nbsp;</td>
                            <td width="33%">&nbsp;<input name="remark" id="remark" style="width:138px" type="text" class="input2" onChange="showfiledtip(re_checkRemark(this,'<%write(pps_remark);%>'),yutd5)"></td><td width="30%" id="yutd5"></td>
                        </tr><input type="hidden" name="remarkold" id="remarkold">
                </table></td>
            </tr>
			
			
			
			<!-- 流量计费 -->
			    <tr>
                <td colspan="4"><table width="100%"  border="0" cellspacing="0" cellpadding="0" id = "accTableTraffic" style="display:none">
                        <tr>
                            <td width="35%" height="25" align="right">上行流量额度</td>
                            <td width="2%">&nbsp;</td>
                            <td width="63%">&nbsp;<input name="trafficRx" id="trafficRx" style="width:138px" type="text" id="trafficRx" class="input"  onKeyPress="figureOnly()" />&nbsp;MBytes&nbsp;&nbsp;(<%write(no_limition);%>)
                                    </td>
                        </tr>
						<tr>
                            <td width="35%" height="25" align="right">下行流量额度</td>
                            <td width="2%">&nbsp;</td>
                            <td width="63%">&nbsp;<input name="trafficTx" id="trafficTx" style="width:138px" type="text" id="trafficTx" class="input"  onKeyPress="figureOnly()" size="18" maxlength="15" />&nbsp;MBytes&nbsp;&nbsp;(<%write(no_limition);%>)
                                    </td>
                        </tr>
                </table></td>
            </tr>
 <input type="hidden" name="BeginDate" value="" />
            <input type="hidden" name="EndDate" value="" />
<tr><td colspan="4"><table width="100%">

</table></td></tr>
<tr><td colspan="3" height="20">&nbsp;</td></tr>
<tr><td colspan="4">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
<td	 height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"	onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#PPPoEEdit')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('PPPoEServer.asp')" id="returnback"></td>

</tr>
</table>
</td></tr>
</table></td></tr>

</table><script language="javascript">init();</script>
</form></td>
</tr>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
<script language="javascript">loadcalendar();</script>
</html>
