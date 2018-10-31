<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("DhcpFix_edit");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">



var errorstr="";
/////////////////////////////交互变量/////////////////////
<%
    aspNewDhcpCli();
%>


<%
	var edit_Name="";
	edit_Name=Form("id");
	if ( edit_Name!="") aspOutDhcpOneClientInfo(edit_Name);//需要修改
%>

var edit_Names = "<% write(Form("id"));%>";
<!--


function init(){
	var f=FixDhcp_Config;
	if(edit_Names!=""&&edit_Names!=null){
		f.UserName.value=FixUserNames;
		f.UserNameold.value=FixUserNames;
		f.IP.value=FixIPs;
		f.Mac.value=FixMacs;
		f.Action.value="modify";
		poolInit();
		f.pools.value = pool;
	}
	fixusernametd.innerHTML="";
	fixiptd.innerHTML="";
	fixmactd.innerHTML="";
	if (edit_Names == "")
	{
	    poolInit();
	}
}

function save(){
	var f=FixDhcp_Config;
	if(checkData()){
                if(f.Action.value=="add") {
                     f.UserNameold.value=f.UserName.value;
					 }
		for (var i = 0; i < poolNames.length; i++){
			if(f.pools.value == poolNames[i])
			{
				break;
			}
		}
		f.dhcpVid.value=poolVids[i];
		FixDhcp_Config.action="/goform/formDhcpListStatic";
		FixDhcp_Config.submit();
	}
}
function checkData(){
	var f = FixDhcp_Config;
	/*
	var name=document.getElementById('UserName').value;
	if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\     '") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1)
	{
		alert("用户名应为除:< > , % \' \\ \" & ; 和空格之外的字符。");
		return false;
	}
	*/
	if(!checkName(f.UserName,11,_("<%write(name);%>"),0)) return false;
//	if (!checkNumber(f.dhcpVid,0,4094,_("<%write(poolVid);%>"),0)) return false;
	if(!checkIP(f.IP,_("<%write(ipaddr);%>"),0)) return false;
	if(!checkMac(f.Mac,_("<%write(macaddr);%>"),0)) return false;
	return true;
}
function Reset(){
	FixDhcp_Config.reset();
	init();
}

function poolInit()
{
	var option, i;
	var f=FixDhcp_Config;
	for (i = f.pools.length; i > -1; i--)
	{
		f.pools.remove(i);
	}
	for (i = 0; i < poolNames.length; i++)
	{
		option = document.createElement("OPTION");
		option.value = poolNames[i];
		option.text = poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
		f.pools.options.add(option);
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
  <td><script language="javascript">showSecondTitle(_("<%write(staticDhcp);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="FixDhcp_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
     <td height="25" align="right" id="name2"><%write(dhcppool);%></td>
     <td><img src="images/qt.gif" width="7" height="9"></td>
     <td><select name="pools" id="pools" class="input">
     </select></td>
     <td  id="fixpoolnametd"></td>
   </tr>
   <tr>
	   <td width="35%" height="25" align="right" id="name"><%write(name);%></td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="UserName" id="UserName" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'<%write(name);%>'),fixusernametd)"></td><td  id="fixusernametd"></td>
	   </tr>

       <tr>
	   <td align="right" height="25" id="ipaddr"><%write(ipaddr);%></td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td ><input name="IP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(ipaddr);%>'),fixiptd)"></td><td  id="fixiptd"></td>
	   </tr>
	   <input type="hidden" name="UserNameold" >
	   <input type="hidden" name="dhcpVid" >
	    <tr>
	   <td align="right" height="25" id="MACaddr"><%write(macaddr);%></td>
	   <td ><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="Mac" type="text" size="18" maxlength="17" onKeyPress="macOnly()"  onChange="showfiledtip(re_checkMac(this,'<%write(macaddr);%>'),fixmactd)"></td><td  id="fixmactd"></td>
	   </tr>
 
	   <tr><td colspan="4">
	    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="40" align="left" valign="bottom" class="helpfont" id="staticDHCP_td"><%write(staticDHCPTips);%><br><%write(staticDhcpAtt);%> </td>
  </tr>
</table>
  </td></tr>
	  

	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="middle"><input type="button" class="input-button" value="<%write(save);%>"  onClick="save()" id="uttsave">&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#StaticDHCPForAc')" id="utthelp">&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(back);%>" onClick="GoBack_Page('DhcpFixForAc.asp')" id="returnback"></td>
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

