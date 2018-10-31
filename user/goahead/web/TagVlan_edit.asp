<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("TagVlan");%>
<%langTran("commonWords");%>
<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
//交互begin
	//默认的服务区只能选择默认的地址池。下面这句也是针对默认服务区，不针对其他服务区。
	//在“DHCP服务器设置”页面“启用DHCP服务器”改变时，提示“改操作将同时对默认服务区用户生效”。默认服务区下的"启用DHCP服务器"  改变时给提示，"该操作将同时对有线用户生效"
/*
var poolNames = new Array("default", "pool_2");
var beginIp = new Array("192.168.20.100", "172.16.16.100");
var endIp = new Array("192.168.20.200", "172.16.16.200");
var gateWays = new Array("192.168.20.1", "172.16.16.1");
var netMasks = new Array("255.255.255.0", "255.255.0.0");
var errorstr = "";
*/

<%
	aspAddNewDhcpPool();
%>

<% 
	var edit_Name="";
	edit_Name=Form("oldName");
	if (edit_Name != "")
	{
		getOneTagVlan(edit_Name);
	}
	getWanIfCount();
%>
// 交互end

var edit_Names = "<% write(Form("oldName"));%>";


function init() 
{
	var f=edit_TV;
	
	switch(wanIfCount) {
		case 1:
			break;
		case 2:
			showTable(port4Tr, 0);
			break;
		case 3:
			showTable(port4Tr, 0);
			showTable(port3Tr, 0);
			break;
		case 4:
			showTable(port4Tr, 0);
			showTable(port3Tr, 0);
			showTable(port2Tr, 0);
			break;
		default:
			break;		
	}	
	if(edit_Names != "" && edit_Names != null)
	{
		f.vlanName.value = edit_Names;
		f.oldName.value = edit_Names;
		f.vlanId.value = vlanId;
		f.ipAddr.value = ipAddr;
		f.maskAddr.value = maskAddr;
		f.dhcpServer.checked = dhcpServer;
		poolInit();
		f.pool.value = pool;

		if (document.getElementById('dhcpServer').checked == false)
		{
			document.getElementById('ipAddr').onkeydown= "";
			document.getElementById('maskAddr').onkeydown= "";
			document.getElementById('ipAddr').readOnly = false;
			document.getElementById('maskAddr').readOnly = false;
		}
		else
		{
			document.getElementById('ipAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
			document.getElementById('maskAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
			document.getElementById('ipAddr').readOnly = true;
			document.getElementById('maskAddr').readOnly = true;
		}
		//TAG_VLAN
		var i;
		var mbrCkb;
		var ruleDdb;
		for (i = 1; i <= 4; i++) {
			mbrCkb = document.getElementById('member' + i);
			ruleDdb = document.getElementById('rule' + i);
			if ((mbrmsk & (1 << (i - 1))) != 0) {
				mbrCkb.checked = true;
			
				if ((untagmsk & (1 << (i - 1))) != 0) {
					ruleDdb.value = "untag";
				} else {
					ruleDdb.value = "tag";
				}
			}
		}
	}
	else
	{
		f.ipAddr.value = gateWays[0];
		f.maskAddr.value = netMasks[0];
		poolInit();
	}
//	changSecMode();
	
	//TAG VLAN

	onClickMbr1();
	onClickMbr2();
	onClickMbr3();
	onClickMbr4();
}

function changeDHCPfun()
{
	/*if(edit_Names == "default")	//默认服务区
	{
		if (confirm(_("<%write(warning8);%>")))
		{
			if(document.getElementById('dhcpServer').checked)	//勾选→未勾选
			{
				document.getElementById('dhcpServer').checked = true;
			}
			else	//	未勾选→勾选
			{
				document.getElementById('dhcpServer').checked = false;
			}
		}
		else
		{
			if(document.getElementById('dhcpServer').checked)	//勾选→未勾选
			{
				document.getElementById('dhcpServer').checked = false;
			}
			else	//	未勾选→勾选
			{
				document.getElementById('dhcpServer').checked = true;
			}
		}
	}
	else
	{ */
		if(document.getElementById('dhcpServer').checked)	//勾选→未勾选
		{
			poolChange();
		}
//	}
	if (document.getElementById('dhcpServer').checked == false)
	{
		document.getElementById('ipAddr').onkeydown= "";
		document.getElementById('maskAddr').onkeydown= "";
		document.getElementById('ipAddr').readOnly = false;
		document.getElementById('maskAddr').readOnly = false;
	}
	else
	{
		document.getElementById('ipAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('maskAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('ipAddr').readOnly = true;
		document.getElementById('maskAddr').readOnly = true;
	}
}

/*function hexCheck(value)
{
    var num = trimstr(value);
	var length = num.length;
	for (var i = 0; i < length; i++) 
	{
		onenum = num.substring(i, i + 1);
		if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) 
		{
			alert("");
			return false;
		}
	}
    return true;
}
*/
function checkData()
{
    if (!checkName(edit_TV.vlanName, 11, _("<%write(vlanName);%>"), 0)) {
        return false;
    }

	if (!checkNumber(edit_TV.vlanId, 6, 4094, _("<%write(vlanId);%>"), 0, 0)) {
		return false;
	}
	
	if (document.getElementById('dhcpServer').checked == true)
	{
		if (!checkIP(edit_TV.ipAddr, _("<%write(gateway);%>"),0)) {
			return false;
		}
		if (trimstr(edit_TV.ipAddr.value) == "0.0.0.0") {
			inputErro(edit_TV.ipAddr, _("<%write(warning9);%>"));
			return false;
		}
		if (!checkMask(edit_TV.maskAddr, _("<%write(netMask);%>"),0)) {
			return false;
		}
		if (trimstr(edit_TV.maskAddr.value) == "0.0.0.0") {
			inputErro(edit_TV.maskAddr, _("<%write(warning10);%>"));
			return false;
		}
	}
	return true;
}

function save()
{
	var f = edit_TV;
	if (checkData())
	{
//		f.szEn.value = (f.szEn.checked == true ? 1 : 0);
	//	f.broadcastEn.value = (f.broadcastEn.checked == true ? 1 : 0);
		f.dhcpServer.value = (f.dhcpServer.checked == true ? 1 : 0);
		
		//TAG VLAN
		var i, mbrmsk = 0, rulemsk = 0;
		var mbrCkb;
		var ruleDdb;
		for (i = 1; i <= 4; i++) {
			mbrCkb = document.getElementById('member' + i);
			ruleDdb = document.getElementById('rule' + i);
			if (mbrCkb.checked == true) {
				mbrmsk |= 1 << (i - 1);
			
				if (ruleDdb.value == "untag") {
					rulemsk |= 1 << (i - 1);
				}
			}
		}
		f.hidmember.value = mbrmsk;
		f.hidrule.value = rulemsk;
		f.action="/goform/formTagVlanConfig";
		//alert(f.szEn.value);
		//alert(f.broadcastEn.value);
		//alert(f.dhcpServer.value);
		//return;
		f.submit();	
	}
}

function poolInit()
{
	var option, i;
	var f=edit_TV;
	for (i = f.pool.length; i > -1; i--)
	{
		f.pool.remove(i);
	}
	for (i = 0; i < poolNames.length; i++)
	{
		option = document.createElement("OPTION");
		option.value = poolNames[i];
		option.text = poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
		f.pool.options.add(option);
	}
}

function poolChange()
{
	var m;
	var i = document.getElementById('pool').value;
	for (m = 0; m < poolNames.length; m++)
	{
		if (i == poolNames[m])
		{
			break;
		}
	}
	edit_TV.ipAddr.value = gateWays[m];
	edit_TV.maskAddr.value = netMasks[m];
	inputRight(edit_TV.ipAddr);
	inputRight(edit_TV.maskAddr);
}
/*
function radioChange(value)
{
	edit_TV.wepRadio.value = value;
}
*/

function Reset()
{
	edit_TV.reset();
	init();
	allInputRight(edit_TV);
	/*
	inputRight(edit_TV.wepKey1Text);
	inputRight(edit_TV.wepKey2Text);
	inputRight(edit_TV.wepKey3Text);
	inputRight(edit_TV.wepKey4Text);
	inputRight(edit_TV.radiusPsswd);
	inputRight(edit_TV.pskPsswd);
	*/
}
function changSecMode(){
    var frm=edit_TV;
	var index=getListValue(frm.encryType);
	wutbl.style.display="none";
	weptbl1.style.display="none";
	wpatbl1.style.display="none";
	wpapsktbl1.style.display="none";
	var dis="";
	if(   document.all   )   
    	dis="block";   
 	else   
     	dis="table-row";
	switch(index){
		case "0":		weptbl1.style.display="none";
					wpatbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display=dis;
					break;
		case "1":		weptbl1.style.display=dis;
					wpatbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case "2":		weptbl1.style.display="none";
					wpatbl1.style.display=dis;
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case "3":		weptbl1.style.display="none";
					wpatbl1.style.display="none";
					wpapsktbl1.style.display=dis;
					wutbl.style.display="none";
		default:	break;
	}

}

//TAG VLAN
function onClickMbr1() {
	var frm=edit_TV;
	if (frm.member1.checked == true) {
		frm.rule1.disabled = false;
	} else {
		frm.rule1.disabled = true;
	}
}

function onClickMbr2() {
	var frm=edit_TV;
	if (frm.member2.checked == true) {
		frm.rule2.disabled = false;
	} else {
		frm.rule2.disabled = true;
	}
}

function onClickMbr3() {
	var frm=edit_TV;
	if (frm.member3.checked == true) {
		frm.rule3.disabled = false;
	} else {
		frm.rule3.disabled = true;
	}
}

function onClickMbr4() {
	var frm=edit_TV;
	if (frm.member4.checked == true) {
		frm.rule4.disabled = false;
	} else {
		frm.rule4.disabled = true;
	}
}
//end TAG VLAN
</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.gif" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle("<% write(tagVlanSetting);%>");//显示页面名称</script>
  
  
  
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						4,
						new Array("<% write(tagVlanList); %>",'TagVlanList.asp',1,0),
						new Array("<% write(tagVlanSetting); %>",'TagVlan_edit.asp',1,1),
						new Array("<% write(pvidSetting); %>",'vid.asp',1,0),
						new Array("<% write(vlanRouting); %>",'vlan_cc.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->






<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td">
	
	
	<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td>
  
  
  
  
  
  
  
  <table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>




<form name="edit_TV" method="post" action="">
<input type="hidden" name="oldName">
<table border="0" width="600" align='center'>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="vlanName_td"><%write(vlanName);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="vlanName" id="vlanName" size="16" maxlength="11" value="" onChange='checkName(this,11,_("<%write(vlanName);%>"),0)'></td>
	   </tr>
	   <tr id="vlan_td">
	   <td align="right"  width="27%" height="25" id="vlanTag_td"><% write(vlanId); %></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="vlanId" id="vlanId" size="5" maxlength="4" value="" onChange='checkNumber(this,6,4094,_("<%write(vlanId);%>"),0,0)'>（<%write(range);%>: 6-4094）</td>
	   </tr>
	   <tr id="member" style=" display:"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0" style="display:">
	   <tr  id="port1Tr" style="display:">
	   <td  align="right"  width="27%" height="25" ><% write(membership); %></td>
	   <td  width="2%">&nbsp;</td>
	   <td width="71%">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0" style="display:">
	   <tr>
	   <td width="6%"><input name="member1" type="checkbox" class="input-checkbox" id="member1" onClick="onClickMbr1()"></td>
	   <td width="17%">Port 1</td>
	   <td width="16%"><select name="rule1" id="rule1" class="input" ><option value="untag">untag</option><option value="tag">tag</option></select></td>
	  <td width="61%"><% write(vlanRule); %></td>
	   </tr>
	   </table>
	   </td>
	   </tr>
	   <tr id="port2Tr" style="display:">
	   <td  align="right"  width="27%" height="25" >&nbsp;</td>
	   <td  width="2%">&nbsp;</td>
	   <td width="71%">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0" style="display:">
	   <tr>
	   <td width="6%"><input name="member2" type="checkbox" class="input-checkbox" id="member2" onClick="onClickMbr2()"></td>
	   <td width="17%">Port 2</td>
	   <td width="16%"><select name="rule2" id="rule2" class="input" ><option value="untag">untag</option><option value="tag">tag</option></select></td>
	  <td width="61%"><% write(vlanRule); %></td>
	   </tr>
	   </table>
	   </td>
	   </tr>
	   <tr id="port3Tr" style="display:">
	   <td  align="right"  width="27%" height="25" >&nbsp;</td>
	   <td  width="2%">&nbsp;</td>
	   <td width="71%">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0" style="display:">
	   <tr>
	   <td width="6%"><input name="member3" type="checkbox" class="input-checkbox" id="member3" onClick="onClickMbr3()"></td>
	   <td width="17%">Port 3</td>
	   <td width="16%"><select name="rule3" id="rule3" class="input" ><option value="untag">untag</option><option value="tag">tag</option></select></td>
	  <td width="61%"><% write(vlanRule); %></td>
	   </tr>
	   </table>
	   </td>
	   </tr>
	   <tr id="port4Tr" style="display:">
	   <td  align="right"  width="27%" height="25" >&nbsp;</td>
	   <td  width="2%">&nbsp;</td>
	   <td width="71%">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0" style="display:">
	   <tr>
	   <td width="6%"><input name="member4" type="checkbox" class="input-checkbox" id="member4" onClick="onClickMbr4()"></td>
	   <td width="17%">Port 4</td>
	   <td width="16%"><select name="rule4" id="rule4" class="input" ><option value="untag">untag</option><option value="tag">tag</option></select></td>
	  <td width="61%"><% write(vlanRule); %></td>
	   </tr>
	   </table>
	   </td>
	   </tr>
	   </table>
	   </td></tr>
	   <tr><td colspan="3" height="20"><input type="hidden" name="hidmember"><input type="hidden" name="hidrule"></td></tr>
	   
	   <tr>
	   <td align="right"  width="27%" height="25" id="dhcpServer_td"><%write(startDhcp);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="dhcpServer" type="checkbox" class="input-checkbox" id="dhcpServer" checked onClick="changeDHCPfun();"></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="addrPool"><%write(addrPool);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><select name="pool" id="pool" class="input" onClick="poolChange();" onChange="poolChange();"></select></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="gateway_td"><%write(gatewayIpAddr);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input class="input" type="text" name="ipAddr" id="ipAddr" height="22" size="16" maxlength="15" value="" readonly onChange='checkIP(this,_("<%write(gateway);%>"),0)'></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="netMask_td"><%write(gatewayIpSub);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="maskAddr" id="maskAddr" size="16" maxlength="15" value="" readonly onChange='checkMask(edit_TV.maskAddr, _("<%write(netMask);%>"),0)'></td>
	   </tr>
	    <!--///wpapsktbl end/// -->
  </table>
<table align="center"><tr><td height="5"></td></tr><tr><td>
<input type="button" onClick="save()"class="input-button" value="<%write(save);%>" name="" id="uttsave">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(refill);%>" name="" id="uttreset" onClick="Reset();">
<input type="button" class="input-button" value="<%write(help);%>" name="" id="utthelp" onClick="window.open('enrhelpinfo.htm#serviceZone')">
</td></tr></table>
</form>






<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
  
  
  
  
  
  
  
 </td>
  </tr>
</table> 





  </td></tr>
</table>




</td></tr></table>



</td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
