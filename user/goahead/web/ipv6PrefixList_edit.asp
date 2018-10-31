<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("radvd");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
var errorstr="";
/////////////////////////////交互变量/////////////////////
<% 
    getWanIfCount();
    getAdInterval();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutputIpv6PrefixListOneInfo(edit_Name);//需要修改
%> 
var edit_Names = "<% write(Form("id"));%>";
<!--

function init(){
	var f=Ipv6Prefix_Config;
	if(edit_Names!=""&&edit_Names!=null){//alert();
		f.DesPrefix.value=DesPrefix;
		f.DesPrefixold.value=DesPrefix;
		f.DesPrefixLength.value=DesPrefixLength;
		f.PrefixstaleTime.value=PrefixstaleTime;
                f.AdvAutoEnabled.checked = (AdvAutoEnabled == "1")?true:false;
		f.Action.value="modify";
		//f.DesPrefix.disabled = true;
	}
	for(i=1;i<=3;i++)
		document.all["yutd"+i].innerHTML="";
}

function save(){
	var f=Ipv6Prefix_Config;
	if(checkData()){
		f.DesPrefix.disabled = false;
		Ipv6Prefix_Config.action="/goform/ConfigIpv6PrefixList";
		Ipv6Prefix_Config.submit();
		f.DesPrefix.disabled = true;
	}
}
function checkData(){
	var f = Ipv6Prefix_Config;
        f.DesPrefixLength.value = f.DesPrefixLength.value.trim();
        f.PrefixstaleTime.value = f.PrefixstaleTime.value.trim();
        f.DesPrefix.value = f.DesPrefix.value.trim();
        if (!checkNumber(f.DesPrefixLength,0,127,"<%write(ipv6_PreLen);%>",0)) return false;	
        if (!checkNumber(f.PrefixstaleTime,0,71582788,"<%write(pre_life);%>",0)) return false;	
        if (!checkIpv6(f.DesPrefix,"<%write(ipv6_prefix);%>",0)) return false;
	if (!checkIpv6Sp(f.DesPrefix,"<%write(ipv6_prefix);%>")) return false;
	if (f.PrefixstaleTime.value < adInterval){
	    alert("<%write(alert_two);%>");
	    return false;
	}
 	return true;
}
function Reset(){
	Ipv6Prefix_Config.reset();
	init();
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
  <td><script language="javascript">showSecondTitle('<%write(ipv6_PreList);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="Ipv6Prefix_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
    <tr>
       <td align="right" width="44%" height="25"><%write(Adv_auto);%></td>
       <td  width="2%">&nbsp;</td>
       <td width="12%" ><input type="checkbox" name="AdvAutoEnabled"  id=AdvAutoEnabled" checked = "true"></td>
    </tr>

	<tr>	
	   <td width="34%" height="25" align="right"><%write(ipv6_prefix);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="DesPrefix" type="text" maxlength="40" size="39"onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(ipv6_prefix);%>',0),yutd1)"></td><td  id="yutd1"></td>
	   </tr> <input type="hidden" name="DesPrefixold" >
	<tr>
	   <td align="right" height="25"><%write(ipv6_PreLen);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="DesPrefixLength" type="text" size="16" maxlength="3" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,0,127,'<%write(ipv6_PreLen);%>'),yutd2)" value="64"></td><td  id="yutd2"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><%write(pre_life);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="PrefixstaleTime" type="text" size="16" maxlength="8" onKeyPress="figureOnly()"  onChange="showfiledtip(re_checkNumber(this,0,71582788,'<%write(pre_life);%>'),yutd3)" value="2592000"></td><td  id="yutd3"></td>
	   </tr>
	    <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Route')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('ipv6PrefixList.asp')">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
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
