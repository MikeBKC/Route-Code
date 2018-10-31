<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("commonWords");%>
<%langTran("DhcpServer");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

/*
<%
	//aspDhcpServerConfig();
	write('var lanDhcpTypes = "Disable";\n');	//值为Enable和Disable
	write('var poolNames = new Array("default", "pool_2");\n');
	write('var beginIp = new Array("192.168.20.100", "172.16.16.100");\n');
	write('var endIp = new Array("192.168.20.200", "172.16.16.200");\n');
	write('var setPoolName = "default";\n');
	write('var errorstr = "";\n');
%> 
*/

<%
	aspDhcpServerConfig();
%>


function save(){
	var f=DhcpServer_Config;
	//if(checkData()){
		f.lanDhcpType.value=(f.lanDhcpTypew.checked==true?"SERVER":"DISABLE");
		//DhcpServer_Config.dnspEnbl.value=(DhcpServer_Config.dnspEnblw.checked==true?"on":"off");
//		alert(f.lanDhcpType.value);
//		alert(f.pools.value);
//		return;
		f.action="/goform/formDhcpServer";
		f.submit();
	//}
}
	/*
function checkData(){
	var f=DhcpServer_Config;
	if (!checkIP(f.dhcpStart,"起始IP地址",0)) return false;
	if (!checkIP(f.dhcpEnd,"结束IP地址",0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
		alert(_("warning5"));
		f.dhcpStart.focus();
		return false;
	}
	if (f.dhcpEnd.value=="0.0.0.0")
	{
		alert(_("warning6"));
		f.dhcpEnd.focus();
		return false;
	}
	if (compareIP(f.dhcpStart.value,f.dhcpEnd.value)==1)
	   {
			alert("起始地址不能大于结束地址。");
			return false;
	   }   
	if (!checkMask(f.dhcpMask,"子网掩码",0)) return false;
	if (!checkIP(f.dhcpGateway,"网关地址",0)) return false;
	if (!checkNumber(f.dhcpLease,1,864000,"租用时间",0)) return false;
	if (!checkMainDns(f.dhcpPriDns,"主DNS服务器",0)) return false;
	if (!checkIP(f.dhcpSecDns,"备DNS服务器",1)) return false;
	if (!checkIP(f.ISPDns,"运营商DNS服务器1",1)) return false;
	if (!checkIP(f.ISPDns2,"运营商DNS服务器2",1)) return false;
	return true;
}
	*/
function init(){
	var f=DhcpServer_Config;
	f.lanDhcpTypew.checked=(lanDhcpTypes=="Enable"?true:false);
	/*
	f.dhcpStart.value=dhcpStarts;
	f.dhcpEnd.value=dhcpEnds;
	f.dhcpMask.value=dhcpMasks;
	f.dhcpGateway.value=dhcpGateways;
	f.dhcpLease.value=dhcpLeases;
	f.dnspEnblw.checked=(dnspEnbls=="Enable"?true:false);
	f.dhcpPriDns.value=dhcpPriDnss;
	f.dhcpSecDns.value=dhcpSecDnss;
	f.ISPDns.value=ISPDnss;
	f.ISPDns2.value=ISPDnss2;
	 for(i=1;i<=9;i++){
	 	document.all["yutd"+i].innerHTML="";
	 }
	*/
	poolInit();
	f.pools.value = setPoolName;
}
/*
function initDisplay()
{
	var obj=null;
	if(platform != "nv1800N")
	{
		obj=document.getElementsByName('dnsAgTr');
		for(var i =0;i<obj.length;i++)
		{
			obj[i].style.display=dis;
		}
	}
}
function initPage()
{
	initDisplay();
}
*/

function poolInit()
{
	var option, i;
	var f=DhcpServer_Config;
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

function changeDHCPfun()
{
	if (confirm(_("<%write(_warning9);%>")))
	{
		if(document.getElementById('lanDhcpTypew').checked)	//勾选→未勾选
		{
			document.getElementById('lanDhcpTypew').checked = true;
		}
		else	//	未勾选→勾选
		{
			document.getElementById('lanDhcpTypew').checked = false;
		}
	}
	else
	{
		if(document.getElementById('lanDhcpTypew').checked)	//勾选→未勾选
		{
			document.getElementById('lanDhcpTypew').checked = false;
		}
		else	//	未勾选→勾选
		{
			document.getElementById('lanDhcpTypew').checked = true;
		}
	}
}

</script>
</head>
<body onLoad="init();" onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("<%write(DHCP);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" background="images/table_main.png">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form action=""  name="DhcpServer_Config" method="post">
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"98%",
						5,
						new Array(_("<%write(dhcppool);%>"),'DhcpPoolForAc.asp',1,0),
						new Array(_("<%write(dhcpSvcSet);%>"),'DhcpServerForAc.asp',1,1),
						new Array(_("<%write(staticDhcp);%>"),'DhcpFixForAc.asp',1,0),
						new Array(_("<%write(dhcpClient);%>"),'DhcpClientForAc.asp',1,0),
						new Array(_("<%write(dnsSet);%>"),'DnsProxyForAc.asp',1,0)
		);
		showThreeMenu();
	</script>
<table width="98%" border="0" cellspacing="0" cellpadding="0" align="center" height="200">
  <tr>
    <td align="center" class="three_td" valign="top">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="30">&nbsp;</td></tr> 
    <tr>
		<td width="33%" align="right" height="25" id="dhcpset"><%write(startDhcp);%></td>
		<td width="3%">&nbsp;</td>
		<td height="20"><input name="lanDhcpTypew" id="lanDhcpTypew" type="checkbox" onClick="changeDHCPfun();"><input type="hidden" name="lanDhcpType"></td>
	      </tr>
		  <tr><td></td><td></td><td colspan="2" class="helpfont" id="dhcp shuoming"><%write(show);%></td></tr>
		  <tr>
		<td  align="right" id="beiDNS"><%write(addrPool);%></td>
		<td>&nbsp;</td>
		<td height="20"><span class="vtable">
		  <select name="pools" id="pools" class="input" disabled>
		    </select>
		</span></td><td id="yutd6"></td>
	      </tr>
		<tr><td></td><td></td><tr><td colspan="4" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DhcpServerForAc')" id="utthelp"></td>
	
  </tr>
</table>

  </td></tr>
  </table></td>
  </tr>
</table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

