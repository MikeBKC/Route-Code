<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("License"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
<!--
var need;var isForver;var isOutDate;var remaintime=720;var impowertime=720;
var ip="";var includeip;var mac="";var includemac;var includeuser;var userinfo="";
var errorstr=""; var rightstr="";

<%  
 outputLicenseData();
 outputSystemSn();
 outputProductID();
%>

var remain=Math.ceil(remaintime/24);
-->
</script>
<style type="text/css">
<!--
.STYLE1 {color: #FF3333}
.STYLE2 {color: #FF0000}
.STYLE6 {color: #008893}
-->
</style>
</head>
<body onbeforeunload="logOut()" background="<% write(com_bodyMain); %>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<%write(lic_license);%>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
   <script language="javascript">showright(rightstr,80);</script>
  <tr valign="top">
    <td><form action="" method="post" name="License_Config" enctype="multipart/form-data" id="License_Config">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
  
  
 <table id="first" style="display:" width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr>
 <td width="15%" height="29">&nbsp;</td>
 <td width="84%"><strong><span class="STYLE1"><%write(lic_valid);%><span class="STYLE2">
   <script language="javascript">document.write(impowertime);</script></span><%write(lic_outdate);%></span></strong></td>
 <td width="1%"></td>
 </tr>
 </table>
 <table id="second" style="display:none" width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr><td width="15%" height="20">&nbsp;</td>
 <td width="84%"><strong><%write(lic_valid);%><span class="STYLE2"><script language="javascript">document.write(impowertime);</script></span><%write(lic_notoutdate);%><span class="STYLE2">
   <script language="javascript">document.write(remain);</script>
 </span><%write(com_day);%></strong></td>
 <td width="1%">&nbsp;</td>
 </tr>
 </table>
 <table id="third"  style="display:none" width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr>
 <td width="15%" height="20">&nbsp;</td>
 <td width="84%"><strong><%write(lic_valid);%><span class="STYLE6"><%write(lic_forever);%></span></strong></td>
 <td></td></tr>
 </table>
 
 <br/>
 <table width="100%" border="0" cellspacing="0" cellpadding="0" >
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right"><strong><%write(lic_licInfo);%>：</strong></td>
 </tr>
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right"><%write(lic_product);%>：</td>
 <td width="60%" align="left"><script language="javascript">document.write(ProductID);</script></td></tr>
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right"><%write(lic_serialnum);%>：</td>
 <td width="60%" align="left"><script language="javascript">document.write(productIDs);</script></td></tr>
 </table>
 <table id="user" style="display:" width="100%" border="0" cellspacing="0" cellpadding="0" >
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right"><%write(lic_userinfo);%>：</td>
 <td width="60%" align="left"><script language="javascript">document.write(userinfo);</script></td>
 </tr>
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right">License序号：</td>
 <td width="60%" align="left"><script language="javascript">document.write(curLicense);</script></td>
 </tr>
 </table>
 <table id="ipaddr" name="ipaddr" style="display:" width="100%" border="0" cellspacing="0" cellpadding="0" > 
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right"><%write(lic_ipaddr);%>：</td>
 <td width="60%" align="left"><script language="javascript">document.write(ip);</script></td>
 </tr>
 </table>
 <table id="macaddr" name="macaddr" style="display:" width="100%" border="0" cellspacing="0" cellpadding="0" > 
 <tr>
 <td width="5%" height="26">&nbsp;</td>
 <td width="35%"align="right"><%write(lic_macaddr);%>：</td>
 <td width="60%" align="left"><script language="javascript">document.write(mac);</script></td>
 </tr>
 </table>
 
 <br />
 <!--<fieldset class="styleDiv"><legend><font style="font-weight:bolder; color:#218587; font-size:15px">导入License</font></legend>-->
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr>
 <td height="26" width="33%"align="right"><strong><%write(lic_loadlicense);%></strong></td>
 <td height="26">&nbsp;</td>
 <td height="26"><td>
 </tr>
 <tr>
 <td height="26" width="33%"align="right"><%write(lic_select);%></td>
 <td height="26">&nbsp;</td>
 <td height="26">&nbsp;<input type="file" name="filename" value="" id="filename"/> &nbsp;&nbsp;&nbsp;<input type="button" name="jiazai" id="jiazai" class="input-button"  value="<%write(lic_load);%>" onClick="upload()"></td>
 </tr>
 </table>
  <!--</fieldset>-->
 
 <input type="hidden" name="temp" value="">
 <br /><br />
 <table width="100%" border="0" cellspacing="0" cellpadding="0">
 <tr><td width="10%"></td>
 <!--
 <td width="90%" align="left"><input type="text" name="time"><input type="button" name="runtime" value="确定" onClick="savetime()"></td>
 -->
 </table>
 <table class="htable" width="100%">
 <tr class="header">
 <td width="5%"> </td>
 <td width="83%" class="helpfont" valign="bottom" id="attention3"><strong><%write(lic_tips);%>:</strong></td>
 <td width="12%"></td>
 </tr>					
 <tr>
 <td width="5%"> </td>
   <td width="83%" class="helpfont" valign="bottom" id="attention3">&emsp;&emsp;1.<%write(lic_tipcontent);%> </td>
 <td width="12%"></td>
 </tr>
 </table>    
 </form>
        </div>
    </div>
</div>
<script language="javascript">
</script> 
<script language="javascript">
var frm = License_Config;

function savetime()
{
     frm.action="goform/formsavetime";
     frm.submit();
}
function checkSerialNum()
{
	if(midId%2 == 0)
		return true;
	else
		return false;	
}
function upload()
{
	if (frm.filename.value=="")
	{
	  alert("<%write(lic_assignfile);%>");
	  return;
	}
	frm.action = "goform/UpdateLicenseFile";
	frm.submit();
}

function showTable(tableName,showFlag){
var obj = typeof(tableName)=='object'?tableName:document.getElementById("tableName");
if(eval(showFlag))
obj.style.display="";
else
obj.style.display="none";
}
function init()
{
   if(isForver == 1){
   	showTable(first, 0);
   	showTable(second, 0);
   	showTable(third, 1);
   }else {
		if(isOutDate == 1){
			showTable(first, 1);
   			showTable(second, 0);
   			showTable(third, 0);
  		}else{
			showTable(first, 0);
   			showTable(second, 1);
   			showTable(third, 0);
  		}
   }
   
 	if(includeuser == 1)
	{
		showTable(user,1);
	}
	if(includeuser == -1)
	{
		showTable(user,0);
    }
    if(includeip == 1)
    {
		showTable(ipaddr,1);
    }
	if(includeip == -1)
    {
		showTable(ipaddr,0);
    }
    if(includemac == 1)
    {
		showTable(macaddr,1);
    }
    if(includemac == -1)
    {
		showTable(macaddr,0);
    }
}
</script>
<script language="javascript">
init();
</script>
</body>
</html>
