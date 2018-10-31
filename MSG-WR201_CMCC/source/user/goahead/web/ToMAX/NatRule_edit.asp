<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("NatRule");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">

var errorstr="";
/////////////////////////////交互变量//////  
/*
var RuleIDs="";
var NatTypes="";  
var OutIPs="";
var InFromIPs=""; 
var InEndIPs="";
*/

<%
    getWanIfCount();
    aspOutNatRulePassthroughDisPlay();
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutNatRuleOneInfo(edit_Name);//需要修改
%>
var edit_Names = "<% write(Form("id"));%>";
<!--

//var RuleIDs="1";var NatTypes="1";var OutIPs="192.168.0.63";var InFromIPs="192.168.1.23";var InEndIPs="192.168.1.23";var Binds="3";var errorstr="";
//var edit_Names = "1";


function init(){
	var f=NatRule_Config;
	if (passthroughNotDisplay == 1) {
	    f.NatType.options.remove(2);
	}
	if(edit_Names!=""&&edit_Names!=null){
		f.RuleID.value=RuleIDs;
		f.RuleIDold.value=RuleIDs;
		selVal(f.NatType,NatTypes);
		if(NatTypes=="3"){
		showTable(outIPtr,0);
		}else{
		showTable(outIPtr,1);
		}
		f.OutIP.value=OutIPs;
		f.InFromIP.value=InFromIPs;
		f.InEndIP.value=InEndIPs;
		selVal(f.Bind,Binds);
		f.Action.value="modify";
		outiptd.innerHTML=(NatTypes=="1"?"<%write(nar_exterIP);%>":"<%write(nar_exterFIP);%>");
		helptd.innerHTML=(NatTypes=="1"?"<%write(nar_easyIPTip);%>":(NatTypes=="2"?"<%write(nar_one2oneTip);%>":""));
		//selNatType();
		//alert(outiptd.innerHTML);
	}
	else{
	    outiptd.innerHTML="<%write(nar_exterIP);%>";
		helptd.innerHTML="<%write(nar_easyIPTip);%>";
	}
	 for(i=1;i<=4;i++){
	 	document.all["yutd"+i].innerHTML="";
	 }
}
function Reset(){
	NatRule_Config.reset();
	init();
}

function save(){
	var f=NatRule_Config;
	if(checkData()){
		
		if(f.Action.value == "add")
		f.RuleIDold.value = f.RuleID.value;
		
		NatRule_Config.action="/goform/formNatRule";
		NatRule_Config.submit();
	}
}
function checkData(){
	var f = NatRule_Config;
	var outstr=(f.NatType.selectedIndex==0?"<%write(nar_exterIP);%>":"<%write(nar_exterFIP);%>");
	var isnull=(f.NatType.selectedIndex==0?1:0);
	if(!checkName(f.RuleID,11,"<%write(nar_ruleName);%>",0)) return false;
	if(f.RuleID.value.toLowerCase()=="default"){
		alert("<%write(nar_ruleIDTip);%>");
		return false;
	}
	if(f.NatType.selectedIndex!=2) {
	if(!checkIP(f.OutIP,outstr,0)) return false;
	}
	/*
	if(f.OutIP.value=="0.0.0.0"){
		alert(outstr+"不能为0.0.0.0");
		return false;
	}*/
	if(!checkIP(f.InFromIP,"<%write(com_innerStartIP);%>",0)) return false;
	if(!checkIP(f.InEndIP,"<%write(com_innerEndIP);%>",0)) return false;
	if (compareIP(f.InFromIP.value,f.InEndIP.value)==1)
	{
		alert("<%write(nar_cmpIPTip);%>");
		return false;
	}
	return true;
}
function selNatType(){
	var f = NatRule_Config;
	var typetext=getListText(f.NatType);
	outiptd.innerHTML=(typetext=="EasyIP"?"<%write(nar_exterIP);%>":"<%write(nar_exterFIP);%>");
	helptd.innerHTML=(typetext=="EasyIP"?"<%write(nar_easyIPTip);%>":(typetext=="One2One"?"<%write(nar_one2oneTip);%>":""));
	if(typetext=="Passthrough"){
	    showTable(outIPtr,0);
	}else{
	    showTable(outIPtr,1);
	}
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
  <td><script language="javascript">showSecondTitle('<%write(nar_natRuleConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="NatRule_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
	   <td width="35%" height="25" align="right"><%write(nar_ruleName);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="RuleID" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(nar_ruleName);%>'),yutd1)"></td><td  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25"><%write(nar_natType);%></td>
	   <td >&nbsp;</td>
	   <td ><select name="NatType" onChange="selNatType(this.value)">
	   <option value="1">EasyIP</option>
	   <option value="2">One2One</option>
	   <option value="3">Passthrough</option>
	   </select>
	   </td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" id="helptd" class="helpfont"><%write(nar_easyIPTip);%></td></tr>
	   <input type="hidden" name="RuleIDold" >
	    <tr name="outIPtr" id="outIPtr" style="display:">
	   <td align="right" height="25" id="outiptd"><%write(nar_exterIP);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="OutIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(nar_exterIP);%>'),yutd2)" ></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><%write(com_innerStartIP);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="InFromIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_innerStartIP);%>'),yutd3)"></td><td  id="yutd3"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><%write(com_innerEndIP);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input name="InEndIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()"  onChange="showfiledtip(re_checkIP(this,'<%write(com_innerEndIP);%>'),yutd4)"></td><td  id="yutd4"></td>
	   </tr>
	   <tr>
	   <td align="right" height="25"><%write(nar_bind);%></td>
	   <td >&nbsp;</td>
	   <td><select name="Bind" >
		<script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
				if (is3GPort(i,wanIfCount)) {
					document.write('<option value="WAN'+i+'" >3G</option>');
				} else if (isAPCliPort(i, wanIfCount)) {
					document.write('<option value="WAN'+i+'" >APClient</option>');
				} else {
					document.write('<option value="WAN'+i+'" >WAN'+i+'</option>');
				}
		 }
		</script> 
			</select></td> 
	   </tr>
	  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#NatRuleEdit')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('NatRule.asp')"></td>
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
