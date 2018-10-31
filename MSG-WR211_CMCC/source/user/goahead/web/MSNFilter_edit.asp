<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("MSNFilter"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

var errorstr="";
/////////////////////////////交互变量/////////////////////
<% getSysLang(); %>

<%
    aspExceptMSNData();
    getWanIfCount();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutPutOneExceptMSNInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";


<!--

var msnnumber="";
var remark="";

function init(){
	var f=MSNFilter_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.msnNumber.disabled = false;
		f.msnNumber.value=curmsnNumber;
		f.remark.value=curremark;
		f.Action.value="modify";
		f.indexIDNew.value=curindexID;
		f.indexIDOld.value=curindexID;
	}
	for(i=1;i<=2;i++)
		document.all["yutd"+i].innerHTML="";
}

function save(){
	var f=MSNFilter_Config;
	if(checkData()){
		f.msnNumber.disabled = false;
		//f.RouteEnable.value=(f.RouteEnablew.checked==true?"1":"0");
		MSNFilter_Config.action="/goform/ConfigExceptMSN";
		MSNFilter_Config.submit();
		f.msnNumber.disabled = true;
	}
}
function checkMSNexist()
{
    var frm=MSNFilter_Config;
    for(var i=0;i<totalrecs;i++)
    {
	if((frm.Action.value == "modify")&&(curindex == i)){
	    continue;
	}
	if(msnNumber[i] == frm.msnNumber.value)
	{
	    return true;
	}
    }
	return false;
}
function checkData(){
	var f = MSNFilter_Config;
	if (MSNFilter_Config.msnNumber.value == "")
	{
	    alert("<% write(PleaseInputMSNID); %>");
	    return false;
	}
	/*
	if (!isNum(MSNFilter_Config.msnNumber.value))
	{
	    alert("<% write(PleaseInputRQQNu); %>");
	    return;
	}
	*/
	if (MSNFilter_Config.msnNumber.value.lstrLen() > 30)
	{
	    alert("<% write(MoreThan30MSNID); %>");
	    return false;
	}
	if (!checkName(MSNFilter_Config.msnNumber, 30, "<% write(MSNID); %>", 0))
	{
	    return false;
	}
	f.msnNumber.value = f.msnNumber.value.toLowerCase();
	if(checkMSNexist())
	{
	    //f.msnNumber.value="";
	    alert("<% write(EXISTMSNID); %>");
	    return false;
	}
	if (!checkRemark(MSNFilter_Config.remark,maxlen_remark,"<% write(Describe); %>",0))
	{
	    return false;
	}
	if (MSNFilter_Config.remark.value.lstrLen() > 30)
	{
	    alert("<% write(DescribeLengthMax); %>");
	    return false;
	}
	return true;
}
var maxlen_remark=30;
function Reset(){
	MSNFilter_Config.reset();
	init();
}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<% write(MSNWhiteConf); %>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="MSNFilter_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
	<tr>
	   <td width="34%" height="25" align="right"><% write(MSNID); %></td>
	   <td width="2%"><!--<img src="<% write(com_imgqt); %>" width="7" height="9">-->&nbsp;&nbsp;</td>
	   <td><input name="msnNumber" type="text" size="20" maxlength="30"></td><td  id="yutd1"></td>
	</tr> <input type="hidden" name="indexIDNew" ><input type="hidden" name="indexIDOld">
	<tr>
	   <td align="right" height="25"><% write(com_descript); %></td>
	   <td ><!--<img src="<% write(com_imgqt); %>" width="7" height="9">--></td>
	   <td ><input name="remark" type="text" size="20" maxlength="30" class="input2" value=""></td><td  id="yutd2"></td>
	   </tr>
	  
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>" onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<% write(com_help); %>" class="input-button" onClick="window.open('<% write(com_helppage); %>#MSNFilter')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_back); %>" onClick="GoBack_Page('MSNFilter.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
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
