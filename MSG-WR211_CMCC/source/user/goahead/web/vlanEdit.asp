
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("vlanConfig"); %>
<% langTran("commonWords"); %>
<% langTran("common"); %>
<% langTran("PdbUpdata"); %>
<% langTran("JS"); %>
<title><% write(utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" ContentType="text/xml">

//交互begin
<%
	var edit_Name = Form("oldName");
	if (edit_Name != "") {
		getOneVlanConfig(edit_Name);
	}
%>
//交互end
var edit_Names="<% write (Form("oldName")); %>";
var errorstr="";

/*
var poolNames = edit_Names;
var DhcpEnables="1";
var errorstr="";
var dhcpVids="VIF-5";
var dhcpStarts="192.168.1.100";
var dhcpMasks="255.255.255.0";
var dhcpEnds="192.168.1.200";
var dhcpGateways="192.168.1.1";
var dhcpLeases="86400";
var dhcpPriDnss="200.200.200.251";
var dhcpSecDnss="8.8.8.8";
*/

function init()
{
     var f = edit_DhcpPool;
     if(edit_Names != "")
     {
	 f.DhcpEnable.checked = poolSts;
	 f.poolName.value = poolNames;
	 f.oldName.value = edit_Names;
	 f.dhcpVid.value = poolVids;
	 f.dhcpStart.value = beginIp;
	 f.dhcpMask.value = netMask;
     }
     else f.poolName.value = "VIF";

     f.poolNameshow.value = f.poolName.value;

}

function checkData()
{
	var f=edit_DhcpPool;
	if (!checkName(f.poolName,11,_("<%write(dhcpPoolName);%>"),0)) return false;
	if (!checkNumber(f.dhcpVid,1,4094,_("<%write(poolVid);%>"),0)) return false;
	if (!checkIP(f.dhcpStart,_("<%write(startip);%>"),0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
		alert(_("<%write(warning9);%>"));
		f.dhcpStart.focus();
		return false;
	}
	if (!checkMask(f.dhcpMask,_("<%write(subnet);%>"),0)) return false;
	if (f.dhcpMask.value=="0.0.0.0")
	{
		alert(_("<%write(warning10);%>"));
		f.dhcpMask.focus();
		return false;
	}
	return true;
}
function save()
{
	var f = edit_DhcpPool;
	if (checkData())
	{
		f.action = "/goform/formVlanConfig";
		f.submit();	
	}
}

function filterVID()                                                            
{                                                                               
        var f=edit_DhcpPool;                                                    
        var num = trimstr(f.dhcpVid.value);                                     
        if (num == '') return false;                                            
        if((platform == "qv4240g" || platform == "qv3640" || platform == "u3000" || platform == "u4000" ||  platform == "qvN800" || platform == "qv4220g") && ( eval(num) == 5 || eval(num) == 1)){     
                return "VLAN 1和5 被系统使用，不能配置";                                            
         }                                                                      
        return "";                                                              
} 


function sameNetCheck(ip, mask)
{
	var ipA,maskA,ipB,maskB;
	var ip1=new Array();
	var mask1=new Array();
	var ip2=new Array();
	var mask2=new Array();
	ip1=ip.split(".");
	mask1=mask.split(".");
	ipA=((ip1[0]<<24)|(ip1[1]<<16)|(ip1[2]<<8)|(ip1[3]));
	maskA=((mask1[0]<<24))|((mask1[1]<<16))|((mask1[2]<<8))|((mask1[3]));
	for (var i=0; i<totalrecs; i++)
	{
		if ((modify == 1) && (modifyIndex == i))
		{
			continue;	
		}
		ip2=beginIp[i];
		mask2=netMask[i];
		ip2=ip2.split(".");
		mask2=mask2.split(".");
		ipB=((ip2[0]<<24))|((ip2[1]<<16))|((ip2[2]<<8))|((ip2[3]));
		maskB=((mask2[0]<<24))|((mask2[1]<<16))|((mask2[2]<<8))|((mask2[3]));
		if ((ipA & maskA) == (ipB & maskB))
		{
			alert(_("<%write(warning23);%>"));
			return false;
		}
	}
	return true;
}

function Reset()
{
	 var f = edit_DhcpPool; 
	edit_DhcpPool.reset();
        f.poolName.value = "VIF";
	f.poolNameshow.value = f.poolName.value;
	init();
	//EditConfig(1,w);
	allInputRight(edit_DhcpPool);
	/*	
	yutd1.innerHTML="";
	yutd2.innerHTML="";
	yutd3.innerHTML="";
	yutd4.innerHTML="";
	yutd5.innerHTML="";
	yutd6.innerHTML="";
	yutd7.innerHTML="";
	yutd9.innerHTML="";
	*/
}


var userAgent = navigator.userAgent.toLowerCase();
var is_opera = userAgent.indexOf('opera') != -1 && opera.version();
var is_moz = (navigator.product == 'Gecko') && userAgent.substr(userAgent.indexOf('firefox') + 8, 3);
var is_ie = (userAgent.indexOf('msie') != -1 && !is_opera) && userAgent.substr(userAgent.indexOf('msie') + 5, 3);

function showfiledtip1(tips,tdname){
        if(globalCheckTypeFlag) allInputRight();
	globalCheckTypeFlag = 0;/*调用该方法。则不采用交换机的错误提示风格。以防重叠*/
	tdname.innerHTML="";//alert(tips);
	if(tips!="") tdname.innerHTML+="<font style='font-size:9px;color=#333333'><img src='<% write(com_imgerr); %>'>"+tips+"</font>";
	else  tdname.innerHTML="";
        var f = edit_DhcpPool;
        f.poolName.value = "VIF" + f.dhcpVid.value ;
	f.poolNameshow.value = f.poolName.value;


}


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
  <td><script language="javascript">showSecondTitle("<%write(vlanEdit);%>");</script>
  <table width="100%" border="0" height="550" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td>
    <p>&nbsp;</p>    
          <script language="javascript">
                 var arrayThreeMenu=new Array(
	                                                     "80%",
	                                                     2,
	                                                     new Array("<%write(vlanList);%>",'vlanConfig.asp',1,0),
	                                                     new Array("<%write(vlanEdit);%>",'vlanEdit.asp',1,1)
	                     );
                     showThreeMenu();
             </script>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<!--<p>&nbsp;</p>-->
<tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td>
	<table border="0" width="80%" align='center'>

<form name="edit_DhcpPool" method="post" action="">
<input type="hidden" name="oldName" value="">
<div id="edit_pool" style="display:none">
<table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr>
		  <tr>
		    <td align="right"><%write(dhcpEnable);%></td>
		    <td> &nbsp;&nbsp;</td>
		    <td height="20"><input name="DhcpEnable" id="DhcpEnable" type="checkbox" size="18"></td>
		    <td ></td>
		    </tr>
		  <tr>
		    <td align="right" id="startip2"><%write(dhcpPoolName);%></td>
		    <td ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="poolNameshow" id="poolNameshow" type="text" size="18"  maxlength="11" disabled></td>
		    <td height="20" style="display:none"><input name="poolName" id="poolName" type="text" readonly  size="18"  maxlength="11"></td>
		    <td id="yutd8"></td>
		    </tr>
		  <tr>
		    <td width="33%" align="right" ><%write(poolVid);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="dhcpVid" id="dhcpVid" type="text" size="18" onKeyPress="figureOnly()" onChange="showfiledtip1(re_checkNumber(this,1,4094,'<%write(poolVid);%>'),yutd9)" onblur="showfiledtip1(filterVID(),VIDtips_1_5)"  maxlength="4"><span id="VIDtips_1_5"></td><td id="yutd9"></td>
		    </tr>
		  <tr>
		    <td width="33%" align="right" id="startip"><%write(startip);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="dhcpStart" id="dhcpStart" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(startip);%>'),yutd1)"  maxlength="15"></td><td id="yutd1"></td>
		    </tr>
		  <tr>
		<td align="right" id="subnet"><%write(subnet);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpMask" id="dhcpMask" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMask(this,'<%write(subnet);%>'),yutd7)" maxlength="15"></td><td id="yutd7"></td>
	      </tr>
		   
		<tr><td></td><td></td><tr><td colspan="4" height="20">&nbsp;</td></tr>
	   </table>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#vlanPoolForAc')" id="utthelp">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(back);%>" onClick="GoBack_Page('vlanConfig.asp')" id="goback"></td>
  </tr>
</table>
</div>
</form>
  


</table>
  </td>
  </tr>
  
  </table> 
</td></tr><script language="javascript">init();</script>
</table>
</td></tr></table>
</td>
<!--////end-->
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
