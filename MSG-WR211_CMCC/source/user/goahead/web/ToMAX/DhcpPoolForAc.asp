<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("DhcpPool");%>
<%langTran("commonWords");%>
<% langTran("common"); %>

<title><%write(utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">
//交互begin
<%
	aspDhcpPoolConfig();
%>
//交互end



var flag = 0;
recsPerPage[0] =5;
lineButton[0]=0;
currpage[0]=1;
recscount[0]=totalrecs;
maxrecs[0]=max_totalrecs;
listname[0]="frame12";
listheadname[0]="frame11";
listheadcaption[0]=_("<%write(dhcppool);%>");
var titles1=new Array();
titles1[0]=_("<%write(dhcpPoolName);%>");
titles1[1]=_("<%write(poolSt);%>");
titles1[2]=_("<%write(poolVid);%>");
titles1[3]=_("<%write(startip);%>");
titles1[4]=_("<%write(endip);%>");
titles1[5]=_("<%write(subnet);%>");
titles1[6]=_("<%write(gateway);%>");
titles1[7]=_("<%write(usetime);%>(<%write(minute);%>)");
titles1[8]=_("<%write(zhuDNS);%>");
titles1[9]=_("<%write(beiDNS);%>");

arrayData1=new Array(poolNames, poolSts, poolVids, beginIp, endIp, netMask, gateway, lease, primaryDns, backupDns);
sortable[0]=new Array(1,1,1,1,1,1,1,1,1,1);
sortType[0]=new Array(TYPE_STR,TYPE_STR,TYPE_STR,TYPE_IP,TYPE_IP,TYPE_IP,TYPE_IP,TYPE_NUM,TYPE_IP,TYPE_IP);
arrayCount[0]=10;

function dsplist(index,list)
{
	if(index==0) dspList(index,list,1000,titles1,titles1.length,arrayData1,1,1);
}
var modify=0;
var modifyIndex=-1;
var w=0;

//IP转成整型
function _ip2int(ip) 
{
    var num = 0;
    ip = ip.split(".");
    num = Number(ip[0]) * 256 * 256 * 256 + Number(ip[1]) * 256 * 256 + Number(ip[2]) * 256 + Number(ip[3]);
    num = num >>> 0;
    return num;
}
//整型解析为IP地址
function _int2iP(num) 
{
    var str;
    var tt = new Array();
    tt[0] = (num >>> 24) >>> 0;
    tt[1] = ((num << 8) >>> 24) >>> 0;
    tt[2] = (num << 16) >>> 24;
    tt[3] = (num << 24) >>> 24;
    str = String(tt[0]) + "." + String(tt[1]) + "." + String(tt[2]) + "." + String(tt[3]);
    return str;
}
function getGateWay(ip, mask)
{
	var f = edit_DhcpPool;
	var ipA;
	var ip1=new Array();
	ip1=ip.split(".");
	ipA=((ip1[0]<<24)|(ip1[1]<<16)|(ip1[2]<<8)|0);

    f.dhcpStart.value = _int2iP(ipA + 100);
    f.dhcpEnd.value = _int2iP(ipA + 200);
    f.dhcpGateway.value = _int2iP(ipA + 1);
}
function changeSel()
{
	var f = edit_DhcpPool;

	var tmp = document.getElementById('poolVid').value;

	for (var i = 0; i < vlanConfig.length; i++) {
		if (tmp == vlanConfig[i]) {
			f.dhcpMask.value = vlanNetMask[i];
            getGateWay(vlanIp[i], vlanNetMask[i]);
		}
	}	

	if (ChangeF != 0) {
		if (tmp == poolVids[ChangeF]) {
			f.dhcpMask.value = netMask[ChangeF];
		}
		
	}

}

var ChangeF = 0;
function EditConfig(val,i){
	if(val==0)	//添加
	{ 
		if(eval(maxrecs[0]-totalrecs)<1)
		{
			alert(_("<%write(warning7);%>"));
			return;
		}
		document.getElementById('dhcpMask').readOnly = false;
		modify=0;
	}
	else	//修改
	{
		ChangeF = i;
		if (poolNames[i] == "default")
		{
			option = document.createElement("OPTION");
			option.value = poolVids[0];
			option.text = poolVids[0];
			edit_DhcpPool.poolVid.options.add(option);
			document.getElementById('poolVid').disabled = true;
			document.getElementById('dhcpMask').readOnly = false;
		} else {
			option = document.createElement("OPTION");
			option.value = poolVids[i];
			option.text = poolVids[i];
			edit_DhcpPool.poolVid.options.add(option);
			document.getElementById('dhcpMask').readOnly = false;
		}
		w=i;
		document.getElementById('poolName').value = poolNames[i];
		if (used[i] == "1")
		{
			document.getElementById('poolName').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
			document.getElementById('poolName').readOnly = true;
		}
		edit_DhcpPool.oldName.value = poolNames[i];
		document.getElementById('DhcpEnable').checked = (used[i] == "1")? true:false;
		document.getElementById('dhcpStart').value = beginIp[i];
		document.getElementById('dhcpEnd').value = endIp[i];
		document.getElementById('poolVid').value = poolVids[i];
		document.getElementById('dhcpMask').value = netMask[i];
		document.getElementById('dhcpGateway').value = gateway[i];
		document.getElementById('dhcpLease').value = lease[i];
		document.getElementById('dhcpPriDns').value = primaryDns[i];
		document.getElementById('dhcpSecDns').value = backupDns[i];
		modify=1;
		modifyIndex=i;
	}
	if (i == 0)
	{
		flag = 1;
		document.getElementById('poolName').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('poolName').readOnly = true;
	}
	else
	{
		flag = 2;
	}
	document.getElementById("pool_list").style.display = "none";
	document.getElementById("edit_pool").style.display = "block";
}

function checkData()
{
	var f=edit_DhcpPool;
	if (!checkName(f.poolName,11,_("<%write(dhcpPoolName);%>"),0)) return false;
//	if (!checkNumber(f.dhcpVid,0,4094,_("<%write(poolVid);%>"),0)) return false;
	if (!checkIP(f.dhcpStart,_("<%write(startip);%>"),0)) return false;
	if (!checkIP(f.dhcpEnd,_("<%write(endip);%>"),0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
		alert(_("<%write(com_warning5);%>"));
		f.dhcpStart.focus();
		return false;
	}
	if (f.dhcpEnd.value=="0.0.0.0")
	{
		alert(_("<%write(com_warning6);%>"));
		f.dhcpEnd.focus();
		return false;
	}
	if (compareIP(f.dhcpStart.value,f.dhcpEnd.value)==1)
	{
		alert(_("<%write(warning17);%>"));
		return false;
	}   
	if (!checkMask(f.dhcpMask,_("<%write(subnet);%>"),0)) return false;
	if (!checkIP(f.dhcpGateway,_("<%write(gateway);%>"),0)) return false;
	if (!checkNumber(f.dhcpLease,1,14400,_("<%write(usetime);%>"),0)) return false;
	if (!checkMainDns(f.dhcpPriDns,_("<%write(zhuDNS);%>"),0)) return false;
	if (!checkIP(f.dhcpSecDns,_("<%write(beiDNS);%>"),1)) return false;
	if (!sameNetCheck(f.dhcpStart.value, f.dhcpMask.value)) return false;
//	if (!checkIP(f.ISPDns,"运营商DNS服务器1",1)) return false;
//	if (!checkIP(f.ISPDns2,"运营商DNS服务器2",1)) return false;
	return true;
}
function save()
{
	var f = edit_DhcpPool;
	if (flag == 1)	//默认
	{
		if (checkData())
		{	
			
			
			f.action = "/goform/formDhcpPoolConfig";
			f.submit();	
		}
	}
	else	//非默认
	{
		if (checkData())
		{	
			
			
			f.action = "/goform/formDhcpPoolConfig";
			f.submit();	
		}
	}
}

function DeleteConfig(index){
	if (index == 0)
	{
		alert(_("<%write(warning8);%>"));
		return;
	}
	if(confirm(_("<%write(warning3);%>"))){
		DhcpPool_List.delstr.value=poolNames[index];
//		alert(DhcpPool_List.delstr.value);
//		return;
		DhcpPool_List.action="/goform/formDhcpPoolDel";
	    DhcpPool_List.submit();
	}
}

function delAll(){
	if(confirm(_("<%write(warning4);%>"))){
		DhcpPool_List.action="/goform/formDhcpPoolDelAll";
		DhcpPool_List.submit();
	}
}

function customDel()
{
	genData(DhcpPool_List,0,frame12,poolNames,"<%write(APpermissions);%>");
//	alert(DhcpPool_List.delstr.value);
//	return;
	DhcpPool_List.action="/goform/formDhcpPoolDel";
	DhcpPool_List.submit();
}
/*
function Reset()
{
	if (flag == 1)
	{
		edit_DhcpPool.reset();
	}
	else
	{
		edit_DhcpPool.reset();
	}
}
*/
function init()
{
  	frame11.location.href = "_listhead1.htm"; //注意必须在此处初始化，否则页面刷新的时候可能出现bug
    frame12.location.href = "_list1.htm"; //比如排序后刷新 页面会读缓存而非服务器
	vlanInit();

	for(var i=0; i<poolSts.length;i++)
	{
		if(poolSts[i] == "1")
		{
			poolSts[i] = "<%write(poolEnable);%>";
		}else{
			poolSts[i] = "<%write(poolDisable);%>";
		}
	}

	changeSel();

//	netmaskInit();
}
/*
function dec2bin(dec)
{
	bin = "";
	while (dec > 0)
	{
		if (dec%2 != 0)
		{
			bin = "1" + bin;
		}
		else
		{
			bin = "0" + bin;
		}
		dec = parseInt(dec/2);
	}
	return bin;
}
*/
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
	edit_DhcpPool.reset();
	EditConfig(1,w);
	allInputRight(edit_DhcpPool);
	yutd1.innerHTML="";
	yutd2.innerHTML="";
	yutd3.innerHTML="";
	yutd4.innerHTML="";
	yutd5.innerHTML="";
	yutd6.innerHTML="";
	yutd7.innerHTML="";
	//yutd9.innerHTML="";
}

function netmaskInit()
{
	var f = edit_DhcpPool;
	if (f.poolVid.value != poolVids[0]){
		document.getElementById('dhcpMask').disabled = true;
	} else {
		document.getElementById('dhcpMask').disabled = false;
	}
	
}

function vlanInit()
{
	var option, i;
	var f=edit_DhcpPool;
	for (i = f.poolVid.length; i > -1; i--)
	{
		f.poolVid.remove(i);
	}
	for (i = 0; i < vlanConfig.length; i++)
	{
		option = document.createElement("OPTION");
		option.value = vlanConfig[i];
		option.text = vlanConfig[i];
		f.poolVid.options.add(option);
	}
//		option = document.createElement("OPTION");
//		option.value = poolVids[0];
//		option.text = poolVids[0];
//		f.poolVid.options.add(option);
}

</script>
</head>
<body onLoad="init()" onbeforeunload="logOut()" background="images/body_main.png">
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
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form name="DhcpPool_List" method="post" action="/goform/formDhcpPoolDel">
	<p>&nbsp;</p>
	<script language="javascript">
        if((platform == "WX20S")){
            var tmp = 0;
        }else{
            var tmp = 1;
        }
		var arrayThreeMenu=new Array(
						"80%",
						4,
						new Array(_("<%write(dhcppool);%>"),'DhcpPoolForAc.asp',1,1),
//						new Array(_("<%write(dhcpSvcSet);%>"),'DhcpServerForAc.asp',0,0),
						new Array(_("<%write(staticDhcp);%>"),'DhcpFixForAc.asp',1,0),
						new Array(_("<%write(dhcpClient);%>"),'DhcpClientForAc.asp',1,0),
						new Array(_("<%write(dnsSet);%>"),'DnsProxyForAc.asp',tmp,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<input type="hidden" name="delstr">
   <table id=grouplist style="display" width="80%"  border="0" align="center" cellpadding="0" cellspacing="0" height="300">
<tr>
    <td align="center" class="three_td" valign="top">
<div id="pool_list" style="display:block">
    <table id=grouplist style="display" width="600"  border="0" align="center" cellpadding="0" cellspacing="0">
	<tr><td colspan="4" height="40">&nbsp;</td></tr>
		    <tr>
		      <td colspan="4"><iframe src="_listhead1.htm" name="frame11"  scrolling="no" align="MIDDLE" width="600" height="45" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden ></iframe></td>
		    </tr>
		    <tr>
		      <td colspan="4"><iframe src="_list1.htm" name="frame12" id="framelist" align="MIDDLE" width="600" height="150" marginwidth="1" marginheight="1" frameborder="0" overflow-y:hidden></iframe></td>
		    </tr>
<tr>
<td width="4%" height="21"><input type="checkbox" name="delchk" onClick=selectAllCheck(0,delchk,frame12.document.getElementsByName('chklist'))></td>
<td width="" id="all"><%write(allSelect);%>/<%write(allUnselect);%></td>
<td width=""  align="right">
<input type="button" class="input-button" id="addEntry" onClick="EditConfig(0)" value="<%write(addNew);%>">
<input type="submit" class="input-button" id="delButton" onClick="return(genData(DhcpPool_List,0,frame12,poolNames,'<%write(dhcpP);%>'))" value="<%write(del);%>">
<input type="button" class="input-button" onClick="delAll()" id="delAllEntry" value="<%write(delAll);%>">
</tr></table>
</div>
</form>
<br>
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
		    <td height="20"><input name="poolName" id="poolName" type="text" size="18" onChange="showfiledtip(re_checkName(this,'<%write(dhcpPoolName);%>'),yutd8)" maxlength="11"></td>
		    <td id="yutd8"></td>
		    </tr>
<!--
		  <tr>
		    <td width="33%" align="right" ><%write(poolVid);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="dhcpVid" id="dhcpVid" type="text" size="18" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,0,4094,'<%write(poolVid);%>'),yutd9)" maxlength="15"></td><td id="yutd9"></td>
		    </tr>
-->
	<tr>
	 <td align="right"  width="22%" height="25"><%write(poolVid);%></td>
	 <td  width="2%">&nbsp;</td>
	 <td align="left" height="25"><select name="poolVid" id="poolVid" class="input" onChange="changeSel()"></select></td>
	 </tr>
		  <tr>
		    <td width="33%" align="right" id="startip"><%write(startip);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="dhcpStart" id="dhcpStart" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(startip);%>'),yutd1)" maxlength="15"></td><td id="yutd1"></td>
		    </tr>
		   
		  <tr>
		<td  align="right" id="endip"><%write(endip);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpEnd" id="dhcpEnd" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(endip);%>'),yutd2)" maxlength="15"></td><td id="yutd2"></td>
	      </tr>
		  <tr>
		<td align="right" id="subnet"><%write(subnet);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpMask" id="dhcpMask" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMask(this,'<%write(subnet);%>'),yutd7)" maxlength="15"></td><td id="yutd7"></td>
	      </tr>
         <tr>
		<td  align="right" id="gateway"><%write(gateway);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpGateway" id="dhcpGateway" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(gateway);%>'),yutd3)" maxlength="15"></td><td id="yutd3"></td>
	      </tr>
          <tr>
		<td  align="right" id="usetime"><%write(usetime);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpLease" id="dhcpLease" type="text" value="120" size="18" onKeyPress="figureOnly()" maxlength="6" onChange="showfiledtip(re_checkNumber(this,1,14400,'<%write(usetime);%>'),yutd4)">&nbsp;&nbsp;
		分钟</script>
		</td><td id="yutd4"></td>
	      </tr>
		  <tr>
		<td align="right" id="zhuDNS"><%write(zhuDNS);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpPriDns" id="dhcpPriDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMainDns(this,'<%write(zhuDNS);%>'),yutd5)" maxlength="15"></td><td id="yutd5"></td>
	      </tr>
		  <tr>
		<td  align="right" id="beiDNS"><%write(beiDNS);%></td>
		<td>&nbsp;</td>
		<td height="20"><input name="dhcpSecDns" id="dhcpSecDns" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(beiDNS);%>'),yutd6)" maxlength="15"></td><td id="yutd6"></td>
	      </tr>
		<tr><td></td><td></td><tr><td colspan="4" height="20">&nbsp;</td></tr>
	   </table>
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td width="62%"  height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(save);%>" onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(refill);%>" onClick="Reset()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DHCPPoolForAc')" id="utthelp">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(back);%>" onClick="GoBack_Page('DhcpPoolForAc.asp')" id="goback"></td>
  </tr>
</table>
</div>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>

