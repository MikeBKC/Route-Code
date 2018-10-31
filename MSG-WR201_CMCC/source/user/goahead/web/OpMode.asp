 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<title>艾泰科技</title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script language="javascript">
var errorstr="";
var OpModes="<% getCfgZero(1, "OperationMode"); %>";
var ConnTypes="<% getCfgGeneral(1, "wanConnectionMode"); %>";
var natEnbls="<% getCfgZero(1, "natEnabled"); %>";
//var ConnTypes="<% getCfgGeneral(1, "wanConnectionMode"); %>";
<!--

function save(){
	//if(checkData()){
		var f=OpModeConfig;
		f.natEnbl.value=(f.natEnblw.checked==true?"1":"0");
		OpModeConfig.action="/goform/setOpMode";
		OpModeConfig.submit();
	//}	
}
function init(){
	var f=OpModeConfig;
	if (OpModes == "1"){
		if(ConnTypes=="3G") f.opMode[1].checked = true;
		else f.opMode[0].checked = true;	
	}
	else if(OpModes == "3")
		f.opMode[2].checked = true;
	f.natEnblw.checked=(natEnbls=="1"?true:false);
}
function changeMode(){

}
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('接入方式');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
<tr>
    <td align="center"  class="style7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>
<form  name="OpModeConfig" method="post">
<tr><td colspan="3" height="20">&nbsp;</td></tr>
 <tr>
	   <td align="right" width="30%" height="20">设备接入方式选择：</td>
	  
	   <td>&nbsp;</td>
	   </tr>
   <tr>
   <td width="30%">&nbsp;</td>
	   <td align="right" height="20" width="5%"><input type="radio" name="opMode" id="opMode" value="1" onClick="changeMode()">&nbsp;&nbsp;</td>
	 
	   <td>有线网关模式</td>
	   </tr>
	   <tr><td>&nbsp;</td>
	   <td align="right" height="20"><input type="radio" name="opMode" id="opMode" value="4" onClick="changeMode()">&nbsp;&nbsp;</td>
	  
	   <td>3G客户端模式</td>
	   </tr>
	   <tr><td>&nbsp;</td>
	   <td align="right" height="20"><input type="radio" name="opMode" id="opMode" value="3" onClick="changeMode()">&nbsp;&nbsp;</td>
	  
	   <td>无线客户端模式</td>
	   </tr>
	   <tr><td>&nbsp;</td>
  <td  align=""id="oNat"  height="25"><input type="checkbox" name="natEnblw">&nbsp;&nbsp;</td><td align="left">启用NAT
    <input type="hidden" name="natEnbl">
  </td>
</tr>
	   
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
	    <tr><td  colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="保存" onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="重填" onClick="init()"></td>
  </tr>
</table>
  </td></tr>
  </form>
  </table>
 </td>
  </tr>
</table> 
  </td></tr><script language="javascript">init();</script>
</table></td></tr></table></td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
