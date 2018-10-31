
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("TaskScheduler"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%
var edit_Name="";
edit_Name=Form("id");
if ( edit_Name != "") aspGetTaskOne(edit_Name);//需要修改
%>

var strSecondMenuFocus="<% write(SchTaskConf); %>"; 
var errorstr = "";
var edit_Names = "<% write(Form("id"));%>";//需要修改的账号名

/*
    每星期   str = "01";
    每天 	 str = "02";
    每小时   str = "03";
    每分钟   str = "04";
    
*/

 
	 
	
function Reset(){
    formTaskEdit.reset();
    for(i=1;i<=1;i++)
	document.all["yutd"+i].innerHTML="";
    init();	
}


function selDateType_onChange(ctl) {
	//formTaskEdit.selDay.value = "01";
	if(!formTaskEdit.selDay.value)formTaskEdit.selDay.value = "01";
    var str = "##";
    var hour2 = formTaskEdit.txtHour2;
    var min2 = formTaskEdit.txtMin2;
    var sec2 = formTaskEdit.txtSec2;
	var sec1 = formTaskEdit.txtSec1;
    if (ctl.value == "01") {
	tableruntime.style.display = dis;
	table1.style.display = "none";
	hour2.disabled = true;
	sec2.disabled = true;
	sec1.disabled = true;
	sec1.value = "00";
    } else if (ctl.value == "02") {
	tableruntime.style.display = "none";
	table1.style.display = dis;
	hour2.disabled = false;
	min2.disabled = false;
	sec2.disabled = true;
	hour2.value = "00";
	min2.value = "00";
	sec2.value = str;
	if(sec2.value == str) sec2.value = "00" ;
    } else if (ctl.value == "03") {
	tableruntime.style.display = "none";
	table1.style.display = dis;
	hour2.disabled = true;
	min2.disabled = false;
	sec2.disabled = true;
	hour2.value = str;
	min2.value = "00";
	sec2.value = str;
	if(sec2.value == str) sec2.value = "00" ;
    } else if (ctl.value == "04") {
	tableruntime.style.display = "none";
	table1.style.display = "none";
	hour2.disabled = true;
	min2.disabled = true;
	sec2.disabled = true;
	hour2.value = str;
	min2.value = str;
	sec2.value = str;
    }
}


function changeDateType(string){
    var str = string;
    if(str == "01" ) str = "01";
    else if(str == "02")	 str = "02";
    else if(str == "03")  str = "03";
    else if(str == "04")  str = "04";
    else if(str== "05")  str = "05";
    return str;
}
function changeCmdTypes(string){
    if(string == "<% write(custom); %>" ) return "userdefined";
    else if(string == "<% write(predef); %>")  return "predefined";
}

function changeDay(string){
    var str=string.split(" ");
    if(str[0] == "01" ) return "01";
    else if(str[0] == "02")  return "02";
    else if(str[0] == "03")  return "03";
    else if(str[0] == "04")  return "04";
    else if(str[0] == "05")  return "05";
    else if(str[0] == "06")  return "06";
    else if(str[0] == "00")  return "00";
}
function changeTime(string){
    var str=string.split(" ");
    var time = str[1].split(":");
    return time;
}
<!--编辑填充数据-->
function fillData() {

    formTaskEdit.ID.value = names;
	//formTaskEdit.selDateType.value = changeDateType(types);
	formTaskEdit.selDateType.value = types;
    selDateType_onChange(formTaskEdit.selDateType);
    formTaskEdit.selDay.value = changeDay(times);  
	 
    //填充时间
    var time = changeTime(times);
    if (getListText(formTaskEdit.selDateType) == "<% write(EveryWeak); %>") {
	formTaskEdit.txtHour1.value = time[0];
	formTaskEdit.txtMin1.value = time[1];
	formTaskEdit.txtSec1.value = time[2];
    }else{
	if (time[0] != "##"){
	    formTaskEdit.txtHour2.value = time[0];
	}
	if (time[1] != "##"){
	    formTaskEdit.txtMin2.value = time[1];
	}
	formTaskEdit.txtSec2.value = time[2];
    }
}

function checkData(){
    var f=formTaskEdit;
    if(!checkName(f.ID,31,"<% write(TaskName); %>",0)) return false;
	if (f.txtHour1.value.length==1)
	    f.txtHour1.value="0"+f.txtHour1.value;
	if (f.txtMin1.value.length==1)
	    f.txtMin1.value="0"+f.txtMin1.value;
	if (f.txtHour2.value.length==1)
	    f.txtHour2.value="0"+f.txtHour2.value;
	if (f.txtMin2.value.length==1)
	    f.txtMin2.value="0"+f.txtMin2.value;
 
		
		
	if (f.selDateType.value == "01") {
		if(!checkHour(f.txtHour1))		return false;
		if(!checkMinute(f.txtMin1))		return false;
	}
	if (f.selDateType.value == "02") {
 		if(!checkHour(f.txtHour2))		return false;
		if(!checkMinute(f.txtMin2))		return false;
	}
		if (f.selDateType.value == "03") {
		if(!checkMinute(f.txtMin2))		return false;
	}
		
		
formTaskEdit.txtHour2.disabled = false;
formTaskEdit.txtMin2.disabled = false;
formTaskEdit.txtSec2.disabled = false;
formTaskEdit.txtHour1.disabled = false;
formTaskEdit.txtMin1.disabled = false;
formTaskEdit.txtSec1.disabled = false;
	
	
    return true;
}

function save(){
    if(checkData()){
	formTaskEdit.action="/goform/formTaskEdit";
	formTaskEdit.submit();
    }
}



function init() {
    if(edit_Names!=""&&edit_Names!=null){ 
	formTaskEdit.Action.value="modify";
	formTaskEdit.IDold.value = names;
	fillData();
    }else
    {
	var ctl1 = formTaskEdit.selDateType;
	selDateType_onChange(formTaskEdit.selDateType);
	
    }
}	

//-->
</script>
</head>
<body onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td> 
<!--////内容-->
<td><script language="javascript">showSecondTitle("<% write(SchTask); %>");</script>
<table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr,80);</script>
<tr valign="top">
<td>
<p>&nbsp;</p>
<script language="javascript">
var arrayThreeMenu=new Array(
	"80%",
	2,

	new Array("<% write(SchTask); %>",'TaskScheduler.asp',1,0),
	new Array("<% write(SchTaskConf); %>",'TaskScheduler_edit.asp',1,1)
	);
showThreeMenu();
</script>
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">

<tr>
<td align="center" class="three_td" valign="top"><table width="600" border="0" cellspacing="0" cellpadding="0">

<form  name="formTaskEdit" method="post">
<p>&nbsp;</p>

<tr>
<td align="center"  >
<table border="0" width="600" align='center'>

<tr>
<td align="right"  height="25" width="40%" id="setnametd"><% write(TaskName); %></td>
<td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
<td width="37%"><input type="text" name="ID" size="18"  maxlength="11"   value="" onChange="showfiledtip(re_checkName(this,'<% write(TaskName); %>'),yutd1)"  ></td><td width="21%"  id="yutd1"></td>
</tr> 
<tr>
<td align="right"  height="25"  ><% write(StartType); %></td>
<td >&nbsp;</td>
<td ><select name="selDateType" onChange="selDateType_onChange(this)">
<option value="01" selected><% write(EveryWeak); %></option>
<option value="02"><% write(EveryDay); %></option>
<option value="03"><% write(EveryH); %></option>
<option value="04"><% write(EveryMin); %></option>

</select></td>
<td></td>
</tr>
<tr  id="tableruntime">
<td height="20" align="right" colspan="4" valign="middle"> <table width="100%" cellpadding="0">
<tr>
<td align="right"  height="20" width="40%" ><% write(RunTime); %></td>
<td width="2%" >&nbsp;</td>
<td align="left"><table cellpadding="0" cellspacing="0" border="0"><tr><td align="left"><select name="selDay" size="1"   id="selDay" >
<option value="01" selected="selected"><% write(Monday); %></option>
<option value="02"><% write(Tuesday); %></option>
<option value="03"><% write(Wednesday); %></option>
<option value="04"><% write(Thursday); %></option>
<option value="05"><% write(Friday); %></option>
<option value="06"><% write(Saturday); %></option>
<option value="00"><% write(Sunday); %></option>
</select></td>
<td> &nbsp;
<input name="txtHour1"  type="text"  onKeyPress="HourOnly(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time">
:
<input name="txtMin1" type="text"  onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time">
:
<input name="txtSec1" type="text"  onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time"> </td></tr></table></td>
</tr>
</table>
</td>
</tr>

<tr width="100%" style="display:none" id="table1">
<td height="20" align="right" colspan="4" valign="middle"> 
<table width="100%" >
<tr>
<td width="40%"  align="right"><% write(RunTime); %></td>
<td width="2%" >&nbsp;</td>
<td colspan="4" align="left"><input name="txtHour2" type="text"  onKeyPress="HourOnly(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time">
:
<input name="txtMin2" type="text"  onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time">
:
<input name="txtSec2" type="text"  onKeyPress="Minute_Second_Only(this)" onClick="clearDefaultText(this)" value="00" maxlength="2" class="time"> </td><td width="21%"></td>
</tr>
</table>
</td>
</tr>

<tr id="trPredefined">
<td align="right"  height="20"  ><% write(TaskCon); %></td>
<td >&nbsp;</td>
<td ><select name="selContent" >

<option value="rebootS"><% write(Reboot); %></option>
</select></td>
<td></td>
</tr>

<tr><td colspan="3" height="20">&nbsp;</td></tr>
<tr><td  colspan="4">
<table width="46%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
<td  height="30" align="center" valign="top"><input type="hidden" name="IDold" ><input type="hidden" name="Action" value="add"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#TaskScheduler')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('TaskScheduler.asp')" id="returnback"></td>
</tr>
</table>
</td>
</tr>
<tr><td colspan="4" align="center" height="30">&nbsp;</td></tr>
</table></td>
</tr>


</form>

</table><script language="javascript">init();</script>
</td></tr>
</table></td></tr></table></td>
<!--////end-->
<td width="15"></td>
</tr>
</table></td></tr></table>
</body>
</html>
