<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("P2PLimit");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
<%getSysLang();%>
var errorstr="";
<%aspOutIpGrpList();%>
<%aspOutP2PLimitConf();%>


function init(){
	var f=P2PLimitConfig;
	f.P2PLimitEnable.checked = (P2PLimitEn == "on") ? true : false;
	seldate(f,true);
	seltime(f,"All");
	f.MaxDownRatetxt.value=dRate.replace(/k/g,"");
	f.uMaxDownRatetxt.value=uRate.replace(/k/g,"");
	changeSelect(f.MaxDownRatetxt, f.MaxDownRatew);
	changeSelect(f.uMaxDownRatetxt, f.uMaxDownRatew);
	f.policy.value=policy;
	f.except.value = ipGrpNames;
	setTimeValue(f,p2p_days,p2p_timestart,p2p_timestop);//common.js中的公用函数
}
function Reset(){
	P2PLimitConfig.reset();
	init();
}


function save(){
	var f=P2PLimitConfig;
	if(checkData()){
		getTimeValue(f);
		if(f.day.value=="0000000"&&!f.everyday.checked){
			alert("<%write(com_selectDate);%>");
			f.day.value="";
			return;
		}
			 f.dRate.value=f.MaxDownRatetxt.value;
			
			 f.uRate.value=f.uMaxDownRatetxt.value;
 
		P2PLimitConfig.action="/goform/formP2PLimitConfig";
		P2PLimitConfig.submit();
	}
}
function checkData(){
	var f = P2PLimitConfig;
	
	var down_rate = parseInt(f.MaxDownRatetxt.value);
	var up_rate = parseInt(f.uMaxDownRatetxt.value);
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
  <td><script language="javascript">showSecondTitle('<%write(P2P_LimitConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="P2PLimitConfig" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="700" align='center'>
  <tr><td colspan="4" height="20"><input type="hidden" name="Action" value="add"></td></tr>
  
   <tr>
	   <td width="28%" height="25" align="right"><%write(P2P_Limit);%></td>
	   <td width="2%"/>
	   <td><input name="P2PLimitEnable" id="P2PLimitEnable" type="checkbox"></td>
	   </tr>
	
	   <tr>
	   <td align="right" height="25"><%write(P2P_LimitPolicy);%></td>
	   <td >&nbsp;</td>
	   <td  colspan="2">
	   		<select name="policy">
	   			<option value="1"><%write(P2P_inContent);%></option>
	   			<option value="2"><%write(P2P_shContent);%></option>
	   		</select>
	   </td>
	   </tr>
	   <tr>
	   <td align="right" height="25" width="27%"><%write(com_uploadLimit);%></td>
	   <td width="2%">&nbsp;</td>
	   <td><input type="text" name="uMaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,P2PLimitConfig.uMaxDownRatew)" size="8" maxlength="6">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="uMaxDownRatew" class="list" onChange="changeRate(this,P2PLimitConfig.uMaxDownRatetxt)">
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
	   <td><input type="text" name="MaxDownRatetxt" onKeyPress="figureOnly()" onBlur="changeSelect(this,P2PLimitConfig.MaxDownRatew)" size="8" maxlength="6">&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;<select name="MaxDownRatew" class="list" onChange="changeRate(this,P2PLimitConfig.MaxDownRatetxt)">
	  <option value="0"><%write(com_noLimit);%></option>
	   <option value="zidingyi"><%write(com_defSelf);%></option>
		<script language="javascript">
		for(i=0;i<RateArr.length;i++)
			document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
		</script>
</select>&nbsp;<%write(com_limitTip);%></td>
	   </tr>
	    <tr>
	    <td width="28%" height="25" align="right"><%write(P2P_except);%></td>
	    <td width="2%"/>
	    <td  colspan="1">
		<select name="except">
		    <option value=""><%write(com_noselect);%></option>
		    <script language="javascript">
			for(var i=0;i<addGroupNames.length;i++)
			{
			    document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
			}
		    </script>
		</select>
	    </td>
	    </tr>
	   <tr><td colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	   <tr><td width="28%" height="28" align="right"><b><%write(com_setTime);%></b></td><td width="2%">&nbsp;</td><td>&nbsp;</td></tr>
	      <script language="javascript">CreateEftTime();</script>
</table>
</td></tr>
	  <tr><td colspan="4" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#P2PLimit')"></td>
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
