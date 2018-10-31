<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("QQFilter");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

/////////////////////////////交互变量/////////////////////

<%
    aspExceptQQData();
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutPutOneExceptQQInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";


<!--


function init(){
	var f=QQFilter_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.qqNumber.disabled = false;
		f.Action.value="modify";
		f.indexIDNew.value=curindexID;
		f.indexIDOld.value=curindexID;
		f.qqNumber.value=curqqNumber;
		f.remark.value=curremark;
	}
	for(i=1;i<=2;i++)
		document.all["yutd"+i].innerHTML="";
}

function save(){
	var f=QQFilter_Config;
	if(checkData()){
		f.qqNumber.disabled = false;
		//f.RouteEnable.value=(f.RouteEnablew.checked==true?"1":"0");
		QQFilter_Config.action="/goform/ConfigExceptQQ";
		QQFilter_Config.submit();
		f.qqNumber.disabled = true;
	}
}
function checkQQexist()
{
    var tb=QQFilter_Config;
    var i;
    for(i=0;i<totalrecs;i++)
    {
	if((tb.Action.value == "modify")&&(curindex == i)){
	    continue;
	}
	if(qqNumber[i]==tb.qqNumber.value)
	{
	    //alert("find same");
	    return true;
	
	}
    }
    return false;
}
function checkData(){
	var f = QQFilter_Config;
	if (QQFilter_Config.qqNumber.value == "")
	{
	    alert("<%write(qqf_input);%>");
	    return false;
	}
	if (!isNum(QQFilter_Config.qqNumber.value))
	{
	    alert("<%write(qqf_numType);%>");
	    return false;
	}
	if (QQFilter_Config.qqNumber.value ==0)
	{
	    alert("<%write(qqf_qqZero);%>");
	    return false;
	}
	/*if ((QQFilter_Config.qqNumber.value >= 2700000000  && QQFilter_Config.qqNumber.value <= 2799999999))
	{
	    alert("<%write(qqf_qq27error);%>");
	    return false;
	}*/
	if ((QQFilter_Config.qqNumber.value > 4294967295 && QQFilter_Config.qqNumber.value < 13300000000)||(QQFilter_Config.qqNumber.value > 18999999999))
	{
	    //确认qq号码小于2^31-1
	    alert("<%write(qqf_qqLen);%>");
	    return false;
	}
	if(checkQQexist())
	{
	    alert("<%write(qqf_qqExist);%>");
	    return false;
	}
	if (!checkRemark(QQFilter_Config.remark,maxlen_remark,"<%write(com_descript);%>",0))
	{
	    return false;
	}
	if (QQFilter_Config.remark.value.lstrLen() > 30)
	{
	    alert("<%write(qqf_remarkLen);%>");
	    return false;
	}
	/*
	if(!checkName(f.RouteName,11,"路由名",0)) return false;
	if(!checkIP(f.DesIP,"目的网络",0)) return false;
	if (!checkMask(f.DesMask,"子网掩码",0)) return false;
	if(!checkIP(f.GateWay,"网关地址",0)) return false;
	if(! checkNumber(f.Metric,0,7,"优先级",0)) return false;
	*/
	return true;
}
var maxlen_remark=30;
function Reset(){
	QQFilter_Config.reset();
	init();
}
//-->
</script>
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
  <td><script language="javascript">showSecondTitle('<%write(qqf_qqConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="QQFilter_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
	<tr>
	   <td width="34%" height="25" align="right"><%write(qqf_qqNum);%></td>
	   <td width="2%"><!--<img src="images/qt.gif" width="7" height="9">-->&nbsp;&nbsp;</td>
	   <td><input name="qqNumber" type="text" size="20" onKeyPress="figureOnly()"></td><td  id="yutd1"></td>
	</tr> <input type="hidden" name="indexIDNew" ><input type="hidden" name="indexIDOld">
	<tr>
	   <td align="right" height="25"><%write(com_descript);%></td>
	   <td ><!--<img src="images/qt.gif" width="7" height="9">--></td>
	   <td ><input name="remark" type="text" size="20" maxlength="30" class="input2" value=""></td><td  id="yutd2"></td>
	   </tr>
	  
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#QQFilter')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('QQFilter.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
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
