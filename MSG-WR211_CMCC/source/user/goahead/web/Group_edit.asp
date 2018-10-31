<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("Group");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script src="js/config.js" type="text/javascript"></script>
<script language="javascript">
<%coefficient();%>
<%aspOutIpGrpList();%>
var errorstr="";
var dRate="0";
var uRate="0"; 
var policy="1";
<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") 
	{
		aspOutOneGroupBaseConfig(edit_Name);
	}
%>

var edit_Names = "<% write(Form("id"));%>";
/*
dRate=(dRate=="nolimit"?"0":dRate);
dCeil=(dCeil=="nolimit"?"0":dCeil);
uRate=(uRate=="nolimit"?"0":uRate);
uCeil=(uCeil=="nolimit"?"0":uCeil);
*/
/*

var dRate="nolimit";
var dCeil="nolimit";
var uRate="nolimit";
var uCeil="nolimit";
var edit_Names="";
var GroupNames="";
var FromIPs="";
var EndIPs="";
var DenyQQs="";
var DenyMSNs="";
var DenyBTs="";
var DenyThunders="";

var group_days="1111111";
var group_timestarts="ALL";
var group_timestops="";
*/
//<!--


function init(){
	var f=Group_Config;
	seldate(f,true);
	seltime(f,"All");
	f.MaxDownRatetxt.value=dRate.replace(/k/g,"");
//	f.MaxUpRatetxt.value=dCeil.replace(/k/g,"");
	f.uMaxDownRatetxt.value=uRate.replace(/k/g,"");
//	f.uMaxUpRatetxt.value=uCeil.replace(/k/g,"");
	changeSelect(f.MaxDownRatetxt, f.MaxDownRatew);
//	changeSelect(f.MaxUpRatetxt, f.MaxUpRatew);
	changeSelect(f.uMaxDownRatetxt, f.uMaxDownRatew);
//	changeSelect(f.uMaxUpRatetxt, f.uMaxUpRatew);
	if(edit_Names!="" && edit_Names!=null)
	{
		f.GroupName.value=GroupNames;
		f.GroupNameold.value=GroupNames;
		if(sourceIPs == 0){
			f.FromIP.value=FromIPs;
			f.EndIP.value=EndIPs;
			f.sourceIP[0].checked = true;
			selObject(f,"ipRange");
        }else{
			f.GroupNameType.value = ipGrpNames;
			f.sourceIP[1].checked = true;
			selObject(f,"groupSel");
    	}
		if(dst_sourceIPs == 0){
			f.dst_FromIP.value = dst_FromIPs;
			f.dst_EndIP.value = dst_EndIPs;
			f.dst_sourceIP[0].checked = true;
			selObject(f,"dst_ipRange");
		}else{
			f.dst_GroupNameType.value = dst_ipGrpNames;
			f.dst_sourceIP[1].checked = true;
			selObject(f,"dst_groupSel");
		}
		f.policy.value=policy;
		f.Action.value="modify";
		setTimeValue(f,group_days,group_timestarts,group_timestops);//common.js中的公用函数
	}else{
		f.Action.value="add";
		selObject(f,"ipRange");
		selObject(f,"dst_ipRange");
	}
}
function Reset(){
	Group_Config.reset();
	init();
	for(i=1;i<=3;i++){
	 	document.all["yutd"+i].innerHTML="";
	 }
}

function save(){
	var f=Group_Config;
	if(checkData()){
		getTimeValue(f);
		if(f.day.value=="0000000"&&!f.everyday.checked){
			alert("<%write(com_selectDate);%>");
			f.day.value="";
			return;
		}
		/*
		if(f.MaxDownRatetxt.value=="0")
	   		f.dRate.value="nolimit";
	   else
		    f.dRate.value=f.MaxDownRatetxt.value+"k";
			*/
			 f.dRate.value=f.MaxDownRatetxt.value;
			
//		if(f.MaxUpRatetxt.value=="0")
//			f.dCeil.value="nolimit";
//		else
//			f.dCeil.value=f.MaxUpRatetxt.value+"k";
		
		/*
		if(f.uMaxDownRatetxt.value=="0")
	   		f.uRate.value="nolimit";
	   else
		    f.uRate.value=f.uMaxDownRatetxt.value+"k";*/
			 f.uRate.value=f.uMaxDownRatetxt.value;
			
//		if(f.uMaxUpRatetxt.value=="0")
//			f.uCeil.value="nolimit";
//		else
//			f.uCeil.value=f.uMaxUpRatetxt.value+"k";
 
		//alert(f.DenyQQ.value+f.DenyMSN.value+f.DenyBT.value+f.DenyThunder.value);
		Group_Config.action="/goform/formGroupConfig";
		Group_Config.submit();
	}
}
function checkData(){
	var f = Group_Config;
	if(!checkName(f.GroupName,11,"<%write(com_group);%>",0)) return false;
        //if (f.FromIP.value=="0.0.0.0"){ alert("起始IP地址不能为0.0.0.0");return false};
	if(!checkIP(f.FromIP,"<%write(com_startIP);%>",0)) return false;
	if(!checkIP(f.EndIP,"<%write(com_endIP);%>",0)) return false;
	if(!checkIP(f.dst_FromIP,"<%write(com_startIP);%>",0)) return false;
	if(!checkIP(f.dst_EndIP,"<%write(com_endIP);%>",0)) return false;
    if (compareIP(f.FromIP.value,f.EndIP.value)==1)
	{
		alert("<%write(com_cmpIPTip);%>");
		return false;
	}
    if (compareIP(f.dst_FromIP.value,f.dst_EndIP.value)==1)
	{
		alert("<%write(com_cmpIPTip);%>");
		return false;
	}
    if ((platform == "nv1220GW")||(platform == "nv750GW")||(platform == "nv521GW")||(platform == "nv521G") ||(platform == "nv521Gv2")||(platform == "nv840G")||(platform == "nv840Gv2")||(platform == "nv1220G")||(platform == "nv3320G")||(platform == "qv2610g")||(platform == "qv4240g")||(platform == "qv4220g")||(platform == "lv4330G")||(platform == "lv6530G")||(platform == "lv6530GV4"))
    {	
	var Max = 1024000;
    }else{
	var Max = 102400;
    }
    if(f.MaxDownRatetxt.value!="0")
	    if(! checkNumber(f.MaxDownRatetxt,coefficient,Max,"<%write(com_downloadLimit);%>",0)) return false;
//     	if(f.MaxUpRatetxt.value!="0")
//	    if(! checkNumber(f.MaxUpRatetxt,coefficient,102400,"下载最大速率",0)) return false;
	if(f.uMaxDownRatetxt.value!="0")
	    if(! checkNumber(f.uMaxDownRatetxt,coefficient,Max,"<%write(com_uploadLimit);%>",0)) return false;
//	if(f.uMaxUpRatetxt.value!="0")
//	    if(! checkNumber(f.uMaxUpRatetxt,coefficient,102400,"上传最大速率",0)) return false;
	var down_rate = parseInt(f.MaxDownRatetxt.value);
//	var down_ceil = parseInt(f.MaxUpRatetxt.value);
	var up_rate = parseInt(f.uMaxDownRatetxt.value);
//	var up_ceil = parseInt(f.uMaxUpRatetxt.value);
/*	if (down_rate > down_ceil)
	{
		alert("下载保证速率不能大于下载最大速率。");
		f.MaxDownRatetxt.focus();
		return false;
	}
	if (up_rate > up_ceil)
	{
		alert("上传保证速率不能大于上传最大速率。");
		f.uMaxDownRatetxt.focus();
		return false;
	}
*/
	return true;
}

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
function selObject(f,val)
{
	if(val=="ipRange")
	{
		f.FromIP.disabled=false;
		f.EndIP.disabled=false;
		f.GroupNameType.disabled=true;
	}
	if(val=="groupSel")
	{
		f.FromIP.disabled=true;
		f.EndIP.disabled=true;
		f.GroupNameType.disabled=false;
	}
	if(val=="dst_ipRange")
	{
		f.dst_FromIP.disabled=false;
		f.dst_EndIP.disabled=false;
		f.dst_GroupNameType.disabled=true;
	}
	if(val=="dst_groupSel")
	{
		f.dst_FromIP.disabled=true;
		f.dst_EndIP.disabled=true;
		f.dst_GroupNameType.disabled=false;
	}
}                           
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(gro_limitConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="Group_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="4" height="20"><input type="hidden" name="Action" value="add"></td></tr>
  
   <tr>
	   <td width="28%" height="25" align="right"><%write(com_group);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="GroupName" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(com_group);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr>
	   <input type="hidden" name="GroupNameold" >
	   <tr>
	   <td colspan="4"><table width="100%">
<tr>
	<td width="28%" height="25" align="right"><% write(gro_selectLimit); %></td>
	<td width="2%" align="right">&nbsp;</td>
	<td width="60%"><input type="radio" name="sourceIP" value="ipRange" onClick="selObject(this.form,this.value)" checked="checked">源网段&nbsp;&nbsp;&nbsp;
	  <input name="FromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_startIP);%>'),yutd2)" value="0.0.0.0">	  &nbsp;<% write(Dao); %>&nbsp;	  <input name="EndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_endIP);%>'),yutd3)" value="0.0.0.0"></td>
<td id="yutd2">&nbsp;</td>
</tr>
<tr >
	<td></td>
	<td align="right">&nbsp;</td>
	<td height="25"><input type="radio" name="sourceIP" value="groupSel" onClick="selObject(this.form,this.value)">源地址组&nbsp;
	  <select name="GroupNameType" id="GroupNameType">
	    <option value=""><% write(com_userAll); %></option>
	    <script language="javascript">
	   for(var i=0;i<addGroupNames.length;i++)
	   {
	       document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
	   }
	   </script>
	      </select>	</td>
<td id="yutd3"></td>
</tr>
</table></td>
<!-- <td align="right" height="25"><%write(com_startIP);%></td> <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td> <td><input name="FromIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_startIP);%>'),yutd2)"></td><td  id="yutd2"></td> </tr> <tr> <td align="right" height="25"><%write(com_endIP);%></td> <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td> <td><input name="EndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_endIP);%>'),yutd3)"></td><td  id="yutd3"></td> -->
	   </tr>

<tr><td colspan="4"><table width="100%">
<tr><td width="28%" height="25" align="right"><% write(gro_selectLimit); %></td>
	<td width="2%" align="right">&nbsp;</td>
	<td width="60%"><input type="radio" name="dst_sourceIP" value="dst_ipRange" onClick="selObject(this.form,this.value)" checked="checked">目的网段&nbsp;
	  <input name="dst_FromIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'目的起始ip'),yutd2)" value="0.0.0.0">	  &nbsp;<%write(Dao);%>&nbsp;	  <input name="dst_EndIP" type="text" size="15" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'目的截至ip'),yutd3)" value="0.0.0.0"></td>
<td id="yutd2">&nbsp;</td>
</tr>
<tr>
	<td></td>
	<td align="right">&nbsp;</td>
	<td height="25"><input type="radio" name="dst_sourceIP" value="dst_groupSel" onClick="selObject(this.form,this.value)">目的地址组&nbsp;
		  <select name="dst_GroupNameType" id="dst_GroupNameType">
			   <option value=""><% write(com_userAll); %></option>
			      <script language="javascript">
					   for(var i=0;i<addGroupNames.length;i++){
						   document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
					   }
				   </script>
	      </select>	
     </td>
<td id="yutd3"></td>
</tr>
</table>
</td></tr>	
    
    
	   <tr>
	   <td align="right" height="25"><%write(gro_speed);%></td>
	   <td >&nbsp;</td>
	   <td  colspan="2">
	   		<select name="policy">
	   			<option value="1"><%write(gro_inContent);%></option>
	   			<option value="2"><%write(gro_shContent);%></option>
	   		</select>
	   </td>
	   </tr>
	   <tr>
	   <td align="right" height="25" width="27%"><%write(com_uploadLimit);%></td>
	   <td width="2%">&nbsp;</td>
	   <td><input type="text" name="uMaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,Group_Config.uMaxDownRatew)" size="8" maxlength="7">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="uMaxDownRatew" class="list" onChange="changeRate(this,Group_Config.uMaxDownRatetxt)">
	  <option value="0"><%write(com_noLimit);%></option>
	   <option value="zidingyi"><%write(com_defSelf);%></option>
		<script language="javascript">
		for(i=0;i<RateArr.length;i++)
			document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
		</script>
</select>&nbsp;<%write(com_limitTip);%><input type="hidden" name="page" value="limit"><input type="hidden" name="dRate"><input type="hidden" name="dCeil" value="0"><input type="hidden" name="uRate"><input type="hidden" name="uCeil" value="0"><input type="hidden" name="policy_name"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25" width="27%"><%write(com_downloadLimit);%></td>
	   <td width="2%">&nbsp;</td>
	   <td><input type="text" name="MaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,Group_Config.MaxDownRatew)" size="8" maxlength="7">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="MaxDownRatew" class="list" onChange="changeRate(this,Group_Config.MaxDownRatetxt)">
	  <option value="0"><%write(com_noLimit);%></option>
	   <option value="zidingyi"><%write(com_defSelf);%></option>
		<script language="javascript">
		for(i=0;i<RateArr.length;i++)
			document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
		</script>
</select>&nbsp;<%write(com_limitTip);%></td>
	   </tr>
<!--	    <tr>
	   <td align="right" height="25">上传最大速率</td>
	   <td >&nbsp;</td>
	   <td><input type="text" name="uMaxUpRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,Group_Config.uMaxUpRatew)" size="8" maxlength="6">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="uMaxUpRatew" class="list" onChange="changeRate(this,Group_Config.uMaxUpRatetxt)">
	   <option value="0">不限速</option>
	   <option value="zidingyi">自定义</option>
		<script language="javascript">
		for(i=0;i<RateArr.length;i++)
			document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
		</script>
</select>&nbsp;(0表示不限速)</td>
	   </tr>
-->

	   <tr><td colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	   <tr><td width="28%" height="28" align="right"><b><%write(com_setTime);%></b></td><td width="2%">&nbsp;</td><td>&nbsp;</td></tr>
	      <script language="javascript">CreateEftTime();</script>
</table>
</td></tr>
	  <tr><td colspan="4" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Group')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('Group.asp')"></td>
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
</html>
