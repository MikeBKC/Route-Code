<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("AlitalkFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript"> 
 
var errorstr="";
/////////////////////////////交互变量/////////////////////

 
var exceptAliEnable="";var indexID=new Array();var AliNumber=new Array();var remark=new Array();
exceptAliEnable="off";
var totalrecs=0;var max_totalrecs=2048;
var errorstr="";var wanIfCount=3;var maxwanIfCount=3; 
var edit_Names = "";
 
<% getSysLang(); %>

<%
    aspExceptAliData();
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutPutOneExceptAliInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";
 

 
var alinumber="";
var remark="";
 
function init(){
	var f=AlitalkFilter_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.aliNumber.disabled = false;
		f.aliNumber.value=curaliNumber;
		f.remark.value=curremark;
		f.Action.value="modify";
		f.indexIDNew.value=curindexID;
		f.indexIDOld.value=curindexID;
	}
	for(i=1;i<=2;i++)
		document.all["yutd"+i].innerHTML="";
}
 
function save(){
	var f=AlitalkFilter_Config;
	if(checkData()){
		f.aliNumber.disabled = false;
		AlitalkFilter_Config.action="/goform/ConfigExceptAli";
		AlitalkFilter_Config.submit();
		f.aliNumber.disabled = true;
	}
}
function checkAliexist()
{
    var frm=AlitalkFilter_Config;
    for(var i=0;i<totalrecs;i++)
    {
	if((frm.Action.value == "modify")&&(frm.aliNumber.value ==  curaliNumber)){
	    continue;
	}
	if(AliNumber[i] == frm.aliNumber.value)
	{
	    return true;
	}
    }
	return false;
}
function checkData(){
    var f = AlitalkFilter_Config;
	if (AlitalkFilter_Config.aliNumber.value == "")
	{
	    alert("<% write(PleaseInputAliID); %>");
	    return false;
	}
	if((AlitalkFilter_Config.aliNumber.value.lstrLen() > 25) || (AlitalkFilter_Config.aliNumber.value.lstrLen() < 5))
	{
	    alert("<% write(DescribeLength); %>");
	    return false;
	}
	if (!checkName(AlitalkFilter_Config.aliNumber, 30, "<% write(AliID) ;%>", 0))
	{
	    return false;
	}
	f.aliNumber.value = f.aliNumber.value.toLowerCase();
	if(checkAliexist())
	{
	    alert("<% write(EXISTAliID); %>");
	    return false;
	}
	if (!checkRemark(AlitalkFilter_Config.remark,maxlen_remark,"描述",0))
	{
	    return false;
	}
	if (AlitalkFilter_Config.remark.value.lstrLen() > 30)
	{
	    alert("<% write(MoreThan30AliID); %>");
	    return false;
	}
	return true;
}
var maxlen_remark=30;
function Reset(){
	AlitalkFilter_Config.reset();
	init();
}

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
  <td><script language="javascript">showSecondTitle('<% write(AliWhiteConf); %>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="AlitalkFilter_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
	<tr>
	   <td width="34%" height="25" align="right"><% write(AliID); %></td>
	   <td width="2%"><!--<img src="<% write(com_imgqt); %>" width="7" height="9">-->&nbsp;&nbsp;</td>
	   <td align="left"><input name="aliNumber" type="text" size="20" maxlength="25"></td><td  id="yutd1"></td>
	</tr> <input type="hidden" name="indexIDNew" ><input type="hidden" name="indexIDOld">
	<tr>
	   <td align="right" height="25"><% write(com_descript); %></td>
	   <td ><!--<img src="<% write(com_imgqt); %>" width="7" height="9">--></td>
	   <td align="left" ><input name="remark" type="text" size="20" maxlength="30" class="input2" value=""></td><td  id="yutd2"></td>
	   </tr>
	  
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#AliFilter')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('AlitalkFilter.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
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
