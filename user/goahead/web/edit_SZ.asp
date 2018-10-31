<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<%langTran("edit_SZ");%>
<%langTran("commonWords");%>
<%langTran("common");%>
<%langTran("SmartQos");%>
<%langTran("left");%>
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

        var vlanId = 0;
<%
	aspAddNewServiceZone();
%>
RateArr = new Array("64K", "128K", "256K", "512K", "1M", "1.5M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "12M", "13M", "14M", "15M", "16M", "17M", "18M", "19M", "20M", "25M", "30M", "35M", "40M", "45M", "50M", "90M", "100M");

<% 
	var edit_Name="";
	edit_Name=Form("oldName");
	if (edit_Name != "")
	{
		//getOneSZEntry(edit_Name);
		getOneServiceZone(edit_Name);
	//	write('var enSZ = true;\n');
	//	write('var zoneName = "SZ_1";\n');
	//	write('var ssid = "SSID_1";\n');
	//	write('var enBroadcast = true;\n');
	//	write('var vlanId = "1";\n');
	//	write('var encryType = "3";\n');
	//	write('var wepAuthType = "0";\n');
	//	write('var wepFormat = "2";\n');
	//	write('var wepKeyRadio = "1";\n');	//0为KEY1，1为KEY2，2为KEY3
	//	write('var wepKey1Text = "weppassword1";\n');
	//	write('var wepKey2Text = "weppassword2";\n');
	//	write('var wepKey3Text = "weppassword3";\n');
	//	write('var wepKey1Type = "0";\n');
	//	write('var wepKey2Type = "1";\n');
	//	write('var wepKey3Type = "0";\n');
	//	write('var wpaVersion = "2";\n');
	//	write('var wpaEncrp = "0";\n');
	//	write('var radiusIP = "192.168.1.1";\n');
	//	write('var radiusPort = "1812";\n');
	//	write('var radiusPsswd = "radiusPsswd";\n');
	//	write('var wpaTime = "86400";\n');
	//	write('var pskVersion = "0";\n');
	//	write('var pskEncry = "2";\n');
	//	write('var pskPsswd = "pskPsswd";\n');
	//	write('var pskTime = "60";\n');
	//	write('var ipAddr = "192.168.1.1";\n');
	//	write('var maskAddr = "255.255.255.0";\n');
	//	write('var dhcpServer = true;\n');
	//	write('var pool = "0";\n');
	}
%>
// 交互end
var ratelimit=1000000;
if (RateArr.length==33)
	ratelimit=100000;
var edit_Names = "<% write(Form("oldName"));%>";

function init() 
{
	var f=edit_SZ;
    //checkWepKeyType();
	//alert("edit_Names:"+edit_Names);
	if(edit_Names != "" && edit_Names != null)
	{
	//	alert("apVlan:"+apVlan);
		f.szEn.checked = enSZ;
		f.zoneName.value = edit_Names;
		f.oldName.value = edit_Names;
		f.ssid.value = ssid;
		f.encodeType.value = encodeType;
		f.broadcastEn.checked = enBroadcast;
		f.isolateEn.checked = enIsolate;
		f.vlanId.value = vlanId;
//		f.max_user.value = client;
		f.encryType.value = encryType;
		f.wepAuthType.value = wepAuthType;
		f.wepFormat.value = wepFormat;
		f.wepRadio.value = wepKeyRadio;
		f.wepKeyRadio[wepKeyRadio].checked = true;	
		f.wepKey1Text.value = wepKey1Text;
		f.wepKey2Text.value = wepKey2Text;
		f.wepKey3Text.value = wepKey3Text;
		f.wepKey4Text.value = wepKey4Text;
        //checkWepKeyType();
		f.wepKey1Type.value = wepKey1Type;
		f.wepKey2Type.value = wepKey2Type;
		f.wepKey3Type.value = wepKey3Type;
		f.wepKey4Type.value = wepKey4Type;
		f.wpaVersion.value = wpaVersion;
		f.wpaEncrp.value = wpaEncrp;
		f.radiusIP.value = radiusIP;
		f.radiusPort.value = radiusPort;
		f.radiusPsswd.value = radiusPsswd;
		f.wpaTime.value = wpaTime;
		f.pskVersion.value = pskVersion;
		f.pskEncry.value = pskEncry;
		f.pskPsswd.value = pskPsswd;
		f.pskTime.value = pskTime;
		f.ipAddr.value = ipAddr;
//		f.ifAddr.value = ifAddr;
//		f.maskAddr.value = maskAddr;
		f.dhcpServer.checked = dhcpServer;
		f.vlanType.checked = (vlanType=="1"? true:false);
		f.apvlan.value = apvlan;
		
		poolInit();
		f.pool.value = pool;
		if (edit_Names == "default")
		{
			document.getElementById('vlanId').value = "";
			document.getElementById('vlanId').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
//			document.getElementById('ifAddr').value = "";
//			document.getElementById('ifAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
			document.getElementById('zoneName').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
			document.getElementById('vlanId').readOnly = true;
//			document.getElementById('ifAddr').readOnly = true;
			document.getElementById('zoneName').readOnly = true;
			poolInit();
			f.pool.value = "default";
			document.getElementById('pool').readOnly = true;
			document.getElementById('vlan_td').style.display = "none";
//			document.getElementById('virif_td').style.display = "none";
//			document.getElementById('mask_td').style.display = "none";
//            f.szEn.disabled = true;
            if(f.szEn.checked){
                f.dhcpServer.disabled = false;
            }else{
                f.dhcpServer.disabled = true;
            }

		}
		if (document.getElementById('dhcpServer').checked == false)
		{
			document.getElementById('ipAddr').onkeydown= "";
//			document.getElementById('maskAddr').onkeydown= "";
			document.getElementById('ipAddr').readOnly = false;
//			document.getElementById('maskAddr').readOnly = false;
		}
		else
		{
			document.getElementById('ipAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
//			document.getElementById('maskAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
			document.getElementById('ipAddr').readOnly = true;
//			document.getElementById('maskAddr').readOnly = true;
		}
		
		if (limit_type == "1")
		{
			f.share_select.value = "1";
		}
		else
		{
			f.share_select.value = "2";
		}
		f.rxBand.value = limit_down;
		changeSelect(f.rxBand, f.MaxDownRatew);
		f.txBand.value = limit_up;
		changeSelect(f.txBand, f.uMaxDownRatew);

	}
	else
	{
		f.share_select.value = "2";
		f.rxBand.value = "0";
		f.txBand.value = "0";
//		f.ifAddr.value = gateWays[0];
		f.ipAddr.value = gateWays[0];

//		f.maskAddr.value = netMasks[0];
		f.encryType.value = 0;
		poolInit();
	}
   // checkWepKeyType();
	changSecMode();
	setKey();
	changeEn();
}

var key1dis = 0;
var key2dis = 0;
var key3dis = 0;
var key4dis = 0;
var keylength1 = 10;
var keylength2 = 10;
var keylength3 = 10;
var keylength4 = 10;

function setKey()
{
	var f = edit_SZ;
	getdisable(f.wepKey1Type,f.wepKey2Type,f.wepKey3Type,f.wepKey4Type);
	auto_disable(f.wepKey1Text,f.wepKey2Text,f.wepKey3Text,f.wepKey4Text,f.wepKeyRadio,f.wepKey1Type,f.wepKey2Type,f.wepKey3Type,f.wepKey4Type);
	auto_length(f.wepKey1Text,f.wepKey2Text,f.wepKey3Text,f.wepKey4Text,f.wepKey1Type,f.wepKey2Type,f.wepKey3Type,f.wepKey4Type,f.wepFormat.selectedIndex);
	/*
	getdisable();
	auto_disable();
	auto_length();*/
}

function checkWepKeyType() 
{
        var f=edit_SZ;
        if(f.wepKey1Type.value == "0")
        {
            f.wepKey1Text.readOnly = true;
        }
        else
        {
            f.wepKey1Text.readOnly = false;
        }
        if(f.wepKey2Type.value == "0")
        {
            f.wepKey2Text.readOnly = true;
        }
        else
        {
            f.wepKey2Text.readOnly = false;
        }
        if(f.wepKey3Type.value == "0")
        {
            f.wepKey3Text.readOnly = true;
        }
        else
        {
            f.wepKey3Text.readOnly = false;
        }
        if(f.wepKey4Type.value == "0")
        {
            f.wepKey4Text.readOnly = true;
        }
        else
        {
            f.wepKey4Text.readOnly = false;
        }
}

function changeDhcpPoolSel()
{
//	document.getElementById('dhcpPool').value=document.getElementById('vlanId').value;
    var vid = document.getElementById('vlanId').value;
    if(vid == "0"){
	    document.getElementById('dhcpPool').value="<%write(noMatchPool);%>";
        return false;
    }
    if(vid == "")
        vid = 0;
    for(var i=0;i<poolVid.length;i++)
    {
        if(vid == poolVid[i])
        {
//	        document.getElementById('dhcpPool').value=poolNames[i];
	        document.getElementById('dhcpPool').value=poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
            break;
        }
    }
    if(i>=poolVid.length)
    {
	    document.getElementById('dhcpPool').value="<%write(noMatchPool);%>";
    }
}

function changeDHCPfun()
{
	if(edit_Names == "default")	//默认服务区
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
	{
		if(document.getElementById('dhcpServer').checked)	//勾选→未勾选
		{
			poolChange();
		}
	}
	if (document.getElementById('dhcpServer').checked == false)
	{
		document.getElementById('ipAddr').onkeydown= "";
//		document.getElementById('maskAddr').onkeydown= "";
		document.getElementById('ipAddr').readOnly = false;
//		document.getElementById('maskAddr').readOnly = false;
	}
	else
	{
		document.getElementById('ipAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
//		document.getElementById('maskAddr').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('ipAddr').readOnly = true;
//		document.getElementById('maskAddr').readOnly = true;
	}
}

function hexCheck(value)
{
    var num = trimstr(value);
	var length = num.length;
	for (var i = 0; i < length; i++) 
	{
		onenum = num.substring(i, i + 1);
		if ((onenum < "0" || onenum > "9") && (onenum < "a" || onenum > "f") && (onenum < "A" || onenum > "F")) 
		{
			alert("<%write(notHex);%>");
			return false;
		}
	}
    return true;
}

function checkData()
{
	if(!checkNumber(eval('edit_SZ.rxBand'),0, 100000,"<%write(com_band);%>",0) || !checkNumber(eval('edit_SZ.txBand'),0,100000,"<%write(com_band);%>",0))
	{
		return false;
	}
	var tips="";
    if (!checkName(edit_SZ.zoneName, 11, _("<%write(svcZoneName);%>"), 0)) {
        return false;
    }

	tips=re_checkSpecialName(edit_SZ.zoneName, '<%write(svcZoneName);%>');
	if(tips != "")
	{
	    alert(tips);
		return false;
	}

    /*if (!checkName(edit_SZ.ssid, 14, _("<%write(sz_ssid);%>"), 0)) {
        return false;
    }*/
	if(!checkNamess()){
		return false;
		}

//	if (!checkNumber(edit_SZ.max_user, 1, 32, _("<%write(mazxCLT);%>"), 0, 0)) {
//		return false;
//	}
	var i;
	for (var j=0; j<4; j++)
	{
		if (edit_SZ.wepKeyRadio[j].checked)
		{
			i=j+1;
		}
	}
	if (edit_SZ.encryType.value == 1)
	{
		var format=edit_SZ.wepFormat.value;
		if (format == 1) /* 16进制 */
		{
			var type=document.getElementById('wepKey' + i + 'Type').value;
			var wep_pass = document.getElementById('wepKey' + i + 'Text').value;
			if (!hexCheck(wep_pass))
			{
				return false;
			}
			if (type == 1) /* 64位,密码长度必须为10*/
			{
				if (wep_pass.length == 10)
				{
					if (wep_pass.indexOf(",") != -1 || wep_pass.indexOf(";") != -1 || wep_pass.indexOf("；") != -1 || wep_pass.indexOf("%") != -1 || wep_pass.indexOf("\"") != -1 || wep_pass.indexOf("\'") != -1 || wep_pass.indexOf("\\") != -1 || 　wep_pass.indexOf(" ") != -1) 
					{
						alert("<%write(key);%>" + i + "  <%write(wep_pw_tips);%>");
						return false;
					}
				}
				else
				{
					alert("<%write(key);%>" + i + "  <%write(wep_16_64);%>");
					return false;
				}
			}
			else if (type == 2) /* 128位,密码长度必须为26 */
			{
				if (wep_pass.length == 26)
				{
					if (wep_pass.indexOf(",") != -1 || wep_pass.indexOf(";") != -1 || wep_pass.indexOf("；") != -1 || wep_pass.indexOf("%") != -1 || wep_pass.indexOf("\"") != -1 || wep_pass.indexOf("\'") != -1 || wep_pass.indexOf("\\") != -1 || 　wep_pass.indexOf(" ") != -1) 
					{
						alert("<%write(key);%>" + i + "  <%write(wep_pw_tips);%>");
						return false;
					}
				}
				else
				{
					alert("<%write(key);%>" + i + "  <%write(wep_16_128);%>");
					return false;
				}
			}
			else
			{
				}
		}
		else if (format == 2) /* ASCII码 */
		{
			var type=document.getElementById('wepKey' + i + 'Type').value;
			var wep_pass = document.getElementById('wepKey' + i + 'Text').value;
			if (type == 1) /* 64位,密码长度必须为5*/
			{
				if (wep_pass.length == 5)
				{
					if (wep_pass.indexOf(",") != -1 || wep_pass.indexOf(";") != -1 || wep_pass.indexOf("；") != -1 || wep_pass.indexOf("%") != -1 || wep_pass.indexOf("\"") != -1 || wep_pass.indexOf("\'") != -1 || wep_pass.indexOf("\\") != -1 || 　wep_pass.indexOf(" ") != -1) 
					{
						alert("<%write(key);%>" + i + "  <%write(wep_pw_tips);%>");
						return false;
					}
				}
				else
				{
					alert("<%write(key);%>" + i + "  <%write(wep_ascii_64);%>");
					return false;
				}
			}
			else if (type == 2) /* 128位,密码长度必须为13 */
			{
				if (wep_pass.length == 13)
				{
					if (wep_pass.indexOf(",") != -1 || wep_pass.indexOf(";") != -1 || wep_pass.indexOf("；") != -1 || wep_pass.indexOf("%") != -1 || wep_pass.indexOf("\"") != -1 || wep_pass.indexOf("\'") != -1 || wep_pass.indexOf("\\") != -1 || 　wep_pass.indexOf(" ") != -1) 
					{
						alert("<%write(key);%>" + i + "  <%write(wep_pw_tips);%>");
						return false;
					}
				}
				else
				{
					alert("<%write(key);%>" + i + "  <%write(wep_ascii_128);%>");
					return false;
				}
			}
			else
			{
				}
		}
		else
		{
		}
		
		if (document.getElementById('wepKey1Radio').checked == true)
		{
			if (!checkPassword2(edit_SZ.wepKey1Text, 30, _("<%write(key);%>1"), 0)) {
				return false;
			}
		}
		if (document.getElementById('wepKey2Radio').checked == true)
		{
			if (!checkPassword2(edit_SZ.wepKey2Text, 30, _("<%write(key);%>2"), 0)) {
				return false;
			}
		}
		if (document.getElementById('wepKey3Radio').checked == true)
		{
			if (!checkPassword2(edit_SZ.wepKey3Text, 30, _("<%write(key);%>3"), 0)) {
				return false;
			}
		}
		if (document.getElementById('wepKey4Radio').checked == true)
		{
			if (!checkPassword2(edit_SZ.wepKey4Text, 30, _("<%write(key);%>4"), 0)) {
				return false;
			}
		}
		
	}
	if (edit_SZ.encryType.value == 2)
	{
		if (!checkNumber(edit_SZ.radiusPort, 1, 65535, _("<%write(RadiusPort);%>"),0 , 0)) {
			return false;
		}
		if (!checkIP(edit_SZ.radiusIP, _("<%write(RadiusSVCIP);%>"), 0)) {
			return false;
		}
		if (trimstr(edit_SZ.radiusIP.value) == "0.0.0.0") {
			inputErro(edit_SZ.radiusIP, _("<%write(warning9);%>"));
			return false;
		}
		if (!checkPassword2(edit_SZ.radiusPsswd, 30, _("<%write(RadiusKey);%>"), 0)) {
			return false;
		}
		if (edit_SZ.wpaTime.value != 0 || edit_SZ.wpaTime.value == "")
		{
			if (!checkNumber(edit_SZ.wpaTime, 60, 86400, _("<%write(keyUpdateCyc);%>"), 0, 0)) {
				return false;
			}
		}
	}
	if (edit_SZ.encryType.value == 3)
	{
		if (!checkPassword2(edit_SZ.pskPsswd, 30, _("<%write(PSK);%>"), 0)) {
			return false;
		}
		var len=edit_SZ.pskPsswd.value.length;
		if(len<8){alert("<%write(PSKSize);%>");return false;}

		if (edit_SZ.pskTime.value != 0 || edit_SZ.pskTime.value == "")
		{
			if (!checkNumber(edit_SZ.pskTime, 60, 86400, _("<%write(keyUpdateCyc);%>"), 0, 0)) {
				return false;
			}
		}
	}
	var vlanid = document.getElementById("vlanId").value;

	var vlanType = document.getElementById("vlanType").checked;
		
	if(vlanid!="" && vlanType == true){
			alert("无线VLAN ID 不能和自动下发同时使用");
			return false;
	}
	

	if (edit_Names != "default")
	{
		if(!vlanType){
			if (vlanid!="" && !checkNumber(edit_SZ.vlanId, 1, 4094, _("<%write(vlanTag);%>"), 0, 0)) {
				return false;
			}
			
/*
			if (trimstr(edit_SZ.ifAddr.value) == "0.0.0.0") {
				inputErro(edit_SZ.ifAddr, _("<%write(warning9);%>"));
				return false;
			}
			if(!checkIP(edit_SZ.ifAddr,("<%write(ifIpAddr);%>"),0)){
				return false;	
			}
			if (trimstr(edit_SZ.maskAddr.value) == "0.0.0.0") {
				inputErro(edit_SZ.maskAddr, _("<%write(warning10);%>"));
				return false;
			}
			if(!checkMask(edit_SZ.maskAddr,("<%write(ifNetMask);%>"),0)){
				return false;	
			}
*/
		}
	}
	//var reg =/^([1-9][0-9]{0,3}-[1-9][0-9][0,3])$/;
//	  var reg =/^[0-9]{1}$|^([1-9][0-9]{1,3})$|^([1-9][0-9]{0,3},){1,}[1-9][0-9]{0,3}$|^([1-9][0-9]{0,3}-[1-9][0-9]{0,3},){1,}[1-9][0-9]{0,3}-[1-9][0-9]{0,3}$|^([1-9][0-9]{0,3}-[1-9][0-9]{0,3})$|^((([1-9][0-9]{0,3}|[1-9][0-9]{0,3}-[1-9][0-9]{0,3}),)+([1-9][0-9]{0,3}|[1-9][0-9]{0,3}-[1-9][0-9]{0,3}))$/;
	var reg =/^(0|[1-9][0-9]{0,3})$|^((0|[1-9][0-9]{0,3}),){1,}(0|[1-9][0-9]{0,3})$|^((0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3}),){1,}(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})$|^(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})$|^((((0|[1-9][0-9]{0,3})|(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})),)+((0|[1-9][0-9]{0,3})|(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})))$/; 

	var apvlan=document.getElementById("apvlan").value;
	//alert("apvlan:"+apvlan);
	if(vlanType){
		if(apvlan==""){
			alert("AP管理VLAN范围不能为空");
			return false;	
		}
		var result = reg.test(apvlan);
		if(!result){
			alert("输入不正确，只能输入0-4094数字，中间可以使用'-'或者','进行分割");	
			return false;
		}else{
			if(apvlan.indexOf("-")==-1 || apvlan.indexOf(",")==-1){
				if(apvlan<0 || apvlan >4094){
					alert("输入错误，只能输入0-4094的数");	
					return false;
				}	
			}
			//包含，
			if(apvlan.indexOf(",")!=-1){
				var num = apvlan.split(",");
				
				for(var i =0;i<num.length;i++){
						if(num[i].indexOf("-")==-1){
							if(num[i]<0 || num[i] >4094){
								alert("输入错误，只能输入0-4094的数");
								return false;	
							}	
						}else{
							var startnum = 	num[i].split("-")[0];
							var endnum = 	num[i].split("-")[1];
							if(startnum - endnum >=0 ){
								alert("前面的数不能大于后面的数");	
								return false;	
							}else{
								if(endnum >4094 || startnum <0){
									alert("输入错误，只能输入0-4094的数");
									return false;
								}		
							}
						}
				}	
			}
			
			if(apvlan.indexOf(",")==-1 && apvlan.indexOf("-")!=-1){
				
				var startnum1 = apvlan.split("-")[0];
				var endnum1 = apvlan.split("-")[1];
			//	alert("endnum1:"+endnum);
				if(startnum1 - endnum1 >=0){
					alert("前面的数不能大于后面的数");	
					return false;
				}else{
					//alert("endnum1:"+endnum1);
					if(endnum1 >4094 || startnum1 <0){
						alert("输入错误，只能输入0-4094的数");
						return false;
					}	
				}
			}
				
		}
	}
	if (document.getElementById('dhcpServer').checked == true)
	{
		if (!checkIP(edit_SZ.ipAddr, _("<%write(gateway);%>"),0)) {
			return false;
		}
		if (trimstr(edit_SZ.ipAddr.value) == "0.0.0.0") {
			inputErro(edit_SZ.ipAddr, _("<%write(warning9);%>"));
			return false;
		}
		/*
		if (!checkMask(edit_SZ.maskAddr, _("<%write(netMask);%>"),0)) {
			return false;
		if (trimstr(edit_SZ.maskAddr.value) == "0.0.0.0") {
			inputErro(edit_SZ.maskAddr, _("<%write(warning10);%>"));
			return false;
		}
		}*/
	}
	return true;
}
function changeEn(){
	var auto = document.getElementById("vlanType").checked;
	if(auto){
			document.getElementById("vlanId").disabled = true;
//			document.getElementById("ifAddr").disabled = true;
//			document.getElementById("maskAddr").disabled = true;
			document.getElementById("apvlan").disabled = false;
	        document.getElementById('dhcpPool').value="<%write(noMatchPool);%>";
	}else{
			document.getElementById("vlanId").disabled = false;
//			document.getElementById("ifAddr").disabled = false;
//			document.getElementById("maskAddr").disabled = false;
			document.getElementById("apvlan").disabled = true;	
            changeDhcpPoolSel();
	}	
}
function szEnChange()
{
/*
	if (edit_Names == "default")
    {
        edit_SZ.szEn.checked = true;
    }
*/
}

function checkNames(){
	var f = edit_SZ;
	var names = f.ssid.value;
	var letters = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz 1234567890_,.^@$#!~?`*()-+/%'\"&;；<>";
	for (var i = 0 ;i<names.length; i++){
		e = names.charAt(i);
		if(letters.indexOf(e)< 0){//含中文
			if(f.encodeType.value == 0){//电脑
				if (!checkName(edit_SZ.ssid, 32, _("<%write(sz_ssid);%>"), 0)) {
       			 return false;
   			 	}
			}else if(f.encodeType.value == 1){//手机
				if (!checkName(edit_SZ.ssid, 32, _("<%write(sz_ssid);%>"), 0)) {
       			 return false;
				}
			}
		}//含中文
	}//for
	if (!checkName(edit_SZ.ssid, 32, _("<%write(sz_ssid);%>"), 0)) {
       			 return false;
   			 	}
		
}

function checkNamess(){
	var f = edit_SZ;
	var names = f.ssid.value;
	var letters = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz 1234567890_,.^@$#!~?`*()-+/%'\"&;；<>";
	for (var i = 0 ;i<names.length; i++){
		e = names.charAt(i);
		if(letters.indexOf(e)< 0){//含中文
			if(f.encodeType.value == 0){//电脑
				if (!checkName(edit_SZ.ssid, 32,"SSID", 0)) {
       			 return false;
   			 	}
			}else if(f.encodeType.value == 1){//手机
				if (!checkName(edit_SZ.ssid, 32,"SSID", 0)) {
       			 return false;
				}
			}
		}//含中文
	}//for
	if (!checkName(edit_SZ.ssid, 32, _("<%write(sz_ssid);%>"), 0)) {
       			 return false;
   			 	}
	return true;
}

function save()
{
	var f = edit_SZ;
	if (checkData())
	{
		f.szEn.value = (f.szEn.checked == true ? 1 : 0);
		f.broadcastEn.value = (f.broadcastEn.checked == true ? 1 : 0);
		f.isolateEn.value = (f.isolateEn.checked == true ? 1 : 0);
		f.dhcpServer.value = (f.dhcpServer.checked == true ? 1 : 0);
		f.vlanType.value = (f.vlanType.checked == true ? 1 : 0);
		f.action="/goform/formServiceZoneConfig";
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
	var f=edit_SZ;
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

    var vid = vlanId;
    if(vid == "")
    {
        vid = 0;
    }
    for(var i=0;i<poolVid.length;i++)
    {
        if(vid == poolVid[i])
        {
//	        document.getElementById('dhcpPool').value=poolNames[i];
	        document.getElementById('dhcpPool').value=poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
            break;
        }
    }
    if(i>=poolVid.length)
    {
	    document.getElementById('dhcpPool').value="<%write(noMatchPool);%>";
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
//	edit_SZ.ifAddr.value = gateWays[m];
	edit_SZ.ipAddr.value = gateWays[m];
//	edit_SZ.maskAddr.value = netMasks[m];
//	inputRight(edit_SZ.ifAddr);
	inputRight(edit_SZ.ipAddr);
//	inputRight(edit_SZ.maskAddr);
}

function radioChange(value)
{
	edit_SZ.wepRadio.value = value;
}
function Reset()
{
	edit_SZ.reset();
	init();
	allInputRight(edit_SZ);
	inputRight(edit_SZ.wepKey1Text);
	inputRight(edit_SZ.wepKey2Text);
	inputRight(edit_SZ.wepKey3Text);
	inputRight(edit_SZ.wepKey4Text);
	inputRight(edit_SZ.radiusPsswd);
	inputRight(edit_SZ.pskPsswd);
}

function changSecMode(){
    var frm=edit_SZ;
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

function changeSelect(ctrl1, ctrl2)
{
	if (ctrl1.value == "")
	{
		var selectIndex = 0;
	} else {
		var selectIndex = getSelIndexByValue(ctrl2, ctrl1.value);
	}
	if (selectIndex != -1)
	{
		ctrl2.selectedIndex = selectIndex;
	} else {
		ctrl2.selectedIndex = 1;
	}
}

function changeRate(ctl,ctl2){	
	var selValue = getListValue(ctl);
	selValue=(selValue=="zidingyi"?"":selValue);
	ctl2.value = selValue;	
}
/*
打印上行带宽行
*/
function writeUpLimit()
{
	  document.write('<tr>'
			  +'<td align="right"  width="27%" height="25"><%write(sma_upBand);%></td>'
			  +'<td width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>'
			  +'<td>'
			  );
			  document.write('<input type="text" name="txBand" onKeyPress="figureOnly()" onBlur="changeSelect(this,edit_SZ.uMaxDownRatew)" size="8" maxlength="7">');
			  document.write('&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;');
			  document.write('<select name="uMaxDownRatew" class="list" onChange="changeRate(this,edit_SZ.txBand)">');
			  document.write('<option value="0"><%write(com_noLimit);%></option>');
			  document.write(' <option value="zidingyi"><%write(com_defSelf);%></option>');
			  for(var i=0;i<RateArr.length;i++)
			   document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
			   document.write('</select>&nbsp;<%write(com_limitTip);%>');
			   document.write('</td></tr>');
}
/**
打印下行带宽行
**/
function writeDownLimit()
{
		  document.write('<tr>'
			  +'<td align="right"  width="27%" height="25"><%write(sma_downBand);%></td>'
			  +'<td width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>'
			  +'<td>'
			  );
			  document.write('<input type="text" name="rxBand" onKeyPress="figureOnly()" onBlur="changeSelect(this,edit_SZ.MaxDownRatew)" size="8" maxlength="7">');
			  document.write('&nbsp;kbit/s&nbsp;&nbsp;&nbsp;&nbsp;<b style="font-size:12px">&lt;==</b>&nbsp;');
			  document.write('<select name="MaxDownRatew" class="list" onChange="changeRate(this,edit_SZ.rxBand)">');
			   document.write('<option value="0"><%write(com_noLimit);%></option>');
			  document.write(' <option value="zidingyi"><%write(com_defSelf);%></option>');
			  for(var i=0;i<RateArr.length;i++)
			   document.write('<option value='+RateArrValue[i]+'>'+RateArr[i]+'</option>');
			   document.write('</select>&nbsp;<%write(com_limitTip);%>');
			   document.write('</td></tr>');
}

function wepPasswordLengthChange()
{
    checkWepKeyType();
	var i, index, format, type, pw, flag;
	i=document.getElementsByName("wepKeyRadio");
	for (index=0; index<i.length; index++)
	{
		if(i[index].checked==true)
		{
			  break;
		}
	}
	index++;
	//alert(index);
	format=edit_SZ.wepFormat.value;
	//alert(format);
	type=document.getElementById('wepKey' + index + 'Type').value;
	pw=document.getElementById('wepKey' + index + 'Text').value;
	//alert(type);
	if (format == 1) /* 16进制 */
	{
		if (type == 1) /* 64位,密码长度必须为10*/
		{
			document.getElementById('wepKey' + index + 'Text').maxLength=10;
			flag=10;
		}
		else if (type == 2) /* 128位,密码长度必须为26 */
		{
			document.getElementById('wepKey' + index + 'Text').maxLength=26;
			flag=26;
		}
		else
		{
		}
	}
	else if (format == 2) /* ASCII码 */
	{
		if (type == 1) /* 64位,密码长度必须为5*/
		{
			document.getElementById('wepKey' + index + 'Text').maxLength=5;
			flag=5;
		}
		else if (type == 2) /* 128位,密码长度必须为13 */
		{
			document.getElementById('wepKey' + index + 'Text').maxLength=13;
			flag=13;
		}
		else
		{
		}
	}
	else
	{
	}
	document.getElementById('wepKey' + index + 'Text').value=pw.substr(0, flag);
}

</script>
</head>
<body onLoad="init();"onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
   <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
 
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
  <!--////内容-->
  <td class="table_bg"><script language="javascript">showSecondTitle(_("<%write(sz);%>"));//显示页面名称</script>
   <table width="80%" border="0" height="450" cellspacing="0" cellpadding="0" align="center">
   <script language="javascript">showerror(errorstr,100);//显示保存返回的错误信息</script>
  <tr valign="top">
    <td>
	<!--//三级菜单-->
	<p>&nbsp;</p>
	<script language="javascript">
		var arrayThreeMenu=new Array(
						"100%",
						3,
						new Array(_("<%write(svcZoneList);%>"),'serviceZone.asp',1,0),
						new Array(_("<%write(svcZoneConf);%>"),'edit_SZ.asp',1,1),
						new Array(_("<%write(svcZoneAutoUpdata);%>"),'SzoneAutoUpdata.asp',1,0)
		);
		showThreeMenu();
	</script>
	 <!--//end-->
<table width="100%" border="0" cellspacing="0" cellpadding="0" align="center"><tr>
    <td align="center"  class="three_td"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr><td><table border="0" width="80%" align='center'><tr><td class="gapTd"></td><td>
	<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
<br>
<form name="edit_SZ" method="post" action="">
<input type="hidden" name="wepRadio">
<input type="hidden" name="oldName">
<table border="0" width="600" align='center'>
  <tr><td colspan="3" height="10">&nbsp;</td></tr>
	   <tr style="display:none">
	   <td align="right"  width="27%" height="25" id="zoneStatus_td"><%write(startSZ);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input type="checkbox" name="szEn" id="szEn" class="input-checkbox" checked="true" onClick="szEnChange();"></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="zoneName_td"><%write(SZName);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="zoneName" id="zoneName" size="16" maxlength="11" value="" onChange='checkName(this,11,_("<%write(svcZoneName);%>"),0)'></td>
	   </tr>
       	   <tr>
	   <td align="right"  width="27%" height="25" id="ssid_td">SSID</td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="ssid" id="ssid" size="16"  value="" onChange='checkNames()'>&nbsp;&nbsp;编码&nbsp;&nbsp;
		<select name="encodeType" id="encodeType" width=8px>
                    <option value="0" selected>电脑优先</option><!--gb2312-->
                    <option value="1">手机优先</option><!--utf8-->
		</select>
	   </td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="broadcast_td"><%write(startSSID);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="broadcastEn" type="checkbox" class="input-checkbox" id="broadcastEn" checked></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="isolate_td"><%write(startIsolate);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="isolateEn" type="checkbox" class="input-checkbox" id="isolateEn"></td>
	   </tr>
       <tr id="vlan_td">
	   <td align="right"  width="27%" height="25" id="vlanTag_td">无线VLAN ID</td>
	   <td  width="2%"><!--<img src="images/qt.gif" width="7" height="9">--></td>
	   <td><input type="text" class="input" name="vlanId" id="vlanId" onChange="changeDhcpPoolSel();" size="5" maxlength="4" value="">&nbsp;<%write(sz_confirm);%></td>
	   </tr>

	   <tr>
	   <td align="right"  width="27%" height="25" id="share_stype">限速策略</td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><select name="share_select" id="share_select" class="input" onChange="">
                <option value="2">独享(此范围每一个IP地址使用此带宽)</option>
                <option value="1">共享(此SSID使用此带宽)</option>
           </select></td>
	   </tr>
		<script language="javascript">
			writeUpLimit();
			writeDownLimit();
		</script>

	   <tr>
	   <td align="right"  width="27%" height="25" id="dhcpPool_td">DHCP<%write(addrPool);%></td>
	   <td  width="2%"><!--<img src="images/qt.gif" width="7" height="9">--></td>
	   <td><input type="text" class="input" name="dhcpPool" id="dhcpPool" size="50" maxlength="11" value="" disabled></td>
	   </tr>
    
       
       <!--
       <tr id="virif_td">
	   <td align="right"  width="27%" height="25" id="ifIpAddr_td"><%write(ifIpAddr);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input class="input" type="text" name="ifAddr" id="ifAddr" height="22" size="16"  maxlength="15" value="" onChange='checkIP(this,_("<%write(ifIpAddr);%>"),0)' ></td>
	   </tr>
	   
	   <tr id="mask_td" style="display:">
	   <td align="right"  width="27%" height="25" id="netMask_td"><%write(ifNetMask);%></td>
	   <td  width="2%"></td>
	   <td><input type="text" class="input" name="maskAddr" id="maskAddr" size="16" maxlength="15" value="" onChange='checkMask(this, _("<%write(ifNetMask);%>"),0)'></td>
	   </tr>
       -->
<!--
	   <tr>
	   <td align="right"  width="27%" height="25" id="mazxCLT_td"><%write(mazxCLT);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="max_user" id="max_user" size="3" maxlength="2" value="32" onChange='checkNumber(this, 1, 32, _("<%write(mazxCLT);%>"), 0);'>（<%write(range);%>: 1-32）</td>
	   </tr>
-->
	   <tr>
	   <td align="right"  width="27%" height="25" id="gateway_td2"><%write(encryption);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><select name="encryType" id="encryType" class="input" onChange="changSecMode()">
                <option value="0"><%write(no);%></option>
                <option value="1">WEP</option>
                <option value="2">WPA / WPA2</option>
                <option value="3" selected>WPA-PSK / WPA2-PSK</option>
           </select></td>
	   </tr>
	   <tr id="wutbl" style=" display:none"><td colspan="4"></td></tr>
	   <!--///weptbl///-->
	   <tr id="weptbl1" style="display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right"  width="27%" height="25"  id="gateway_td20"><%write(authType);%></td>
	   <td  width="3%">&nbsp;</td>
	   <td><select name="wepAuthType" id="wepAuthType" class="input" style="width:75px">
            <option value="0" id="auto"><%write(auto);%></option>
            <option value="1" id="openSys"><%write(openSystem);%></option>
            <option value="2" id="shareKey"><%write(shareKey);%></option>
	   </select></td>
	   </tr>
	     <tr><td colspan="4"><table width="100%"><tr>
	  <td width="27%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription5"><%write(show);%></td>	
	  </tr></table></td></tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td18"><%write(keyFormat);%></td>
		<td >&nbsp;</td>
		<td ><select name="wepFormat" id="wepFormat" class="input" style="width:75px" onChange="setKey();">
		    <option value="1" id="band"><%write(hex16);%></option>
		    <option value="2" id="ASCII"><%write(ascii);%></option>
	   </select></td>
	   </tr>
	   <tr>
	   <td width="27%" height="25" align="right" id="gateway_td17"><%write(keySelect);%></td>
	   <td width="2%">&nbsp;</td>
	   <td><table width="100%" border="0" cellspacing="0" cellpading="0"><tr>
	   <td width="55%" id="webKey_td" align="center"><%write(wepKey);%></td>
	   <td id="keyType_td"><%write(keyModel);%></td>
	   </tr></table></td>
	   </tr>
				<TR><TD  width="27%" height="25" align="right" id="gateway_td16"><%write(key);%>1：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio" type="radio" id="wepKey1Radio" value="0" checked onClick="radioChange(this.value)" onFocus="if(key1dis){alert('input key1dis:'+key1dis); this.blur();}else this.form.wepKeyRadio[0].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey1Text" id="wepKey1Text" class="input"></td>
        			<TD ><select name="wepKey1Type" id="wepKey1Type" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

				<TR><TD  width="27%" height="25" align="right" id="gateway_td21"><%write(key);%>2：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio" type="radio" id="wepKey2Radio" value="1"  onClick="radioChange(this.value)" onFocus="if(key2dis) this.blur();else this.form.wepKeyRadio[1].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey2Text" id="wepKey2Text" class="input" onChange=''></td>
        			<TD ><select name="wepKey2Type" id="wepKey2Type" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

				<TR><TD  width="27%" height="25" align="right" id="gateway_td22"><%write(key);%>3：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio" type="radio" id="wepKey3Radio" value="2"  onClick="radioChange(this.value)" onFocus="if(key3dis) this.blur();else this.form.wepKeyRadio[2].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey3Text" id="wepKey3Text" class="input" onChange=''></td>
        			<TD ><select name="wepKey3Type" id="wepKey3Type" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

				<TR><TD  width="27%" height="25" align="right" id="gateway_td22"><%write(key);%>4：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio" type="radio" id="wepKey4Radio" value="3"  onClick="radioChange(this.value)" onFocus="if(key4dis) this.blur();else this.form.wepKeyRadio[3].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey4Text" id="wepKey4Text" class="input" onChange=''></td>
        			<TD ><select name="wepKey4Type" id="wepKey4Type" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

</table>
	   </td></tr>
	    <!--///weptbl end///-->
		 <!--///wpatbl///-->
	   <tr id="wpatbl1" style="display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="27%" height="25" id="gateway_td6"><%write(wpaVer);%></td>
	   <td width="3%" >&nbsp;</td>
	   <td><select name="wpaVersion" id="wpaVersion" class="input" style="width:75px">
		    <option value="0" selected id="auot"><%write(auto);%></option>
		    <option value="1">WPA</option>
		    <option value="2">WPA2</option>
		  </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td13"><%write(encrypAlgo);%></td>
		<td >&nbsp;</td>
		<td ><select name="wpaEncrp" id="wpaEncrp" class="input" style="width:75px">
		    <option value="0" selected id="auto"><%write(auto);%></option>
		    <option value="1">TKIP</option>
		    <option value="2">AES</option>
		  </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td10"><%write(radiusSZIp);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusIP" id="radiusIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange='checkIP(this,_("<%write(RadiusSVCIP);%>"),0)' class="input"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td9"><%write(radiusSZPort);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPort" id="radiusPort"  value = "1812" type="text" onKeyPress="figureOnly()" onChange='checkNumber(this,1,65535,_("<%write(RadiusPort);%>"),0,0)' size="10" maxlength="5" class="input">&nbsp;&nbsp;
		    <%write(tips_sec);%></td><td id="radiusporttd"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td8"><%write(radiusPasswd);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPsswd" id="radiusPsswd" type="password" size="32" maxlength="30" onChange='checkPassword2(this,30,_("<%write(RadiusKey);%>"),0)' class="input">&nbsp;&nbsp;<%write(charSize);%></td><td  id="radiuspatd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="gateway_td3"><%write(keyUpdateCyc);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="wpaTime" type="text" size="10" maxlength="5" onKeyPress="figureOnly()" id="wpaTime" class="input" value="0" onChange=''>&nbsp;&nbsp;<%write(miaoValueSizeTips);%></td><td  id="keyudcyctd"></td>
	   </tr>
</table>
	   </td></tr>
	    <!--///wpatbl end///-->
       <!--///wpapsktbl///-->
	   <tr id="wpapsktbl1" style="display:none"><td colspan="3">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="27%" height="25" id="gateway_td6"><%write(wpaVer);%></td>
	   <td width="3%">&nbsp;</td>
	   <td><select name="pskVersion" id="pskVersion" class="input" style="width:75px">
		    <option value="0" selected id="auto"><%write(auto);%></option>
		    <option value="1">WPA</option>
		    <option value="2">WPA2</option>
		    </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td13"><%write(encrypAlgo);%></td>
		<td >&nbsp;</td>
		<td ><select name="pskEncry" id="pskEncry" class="input" style="width:75px">
		    <option value="0" selected id="auto"><%write(auto);%></option>
		    <option value="1">TKIP</option>
		    <option value="2">AES</option>
		  </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td4"><%write(PSK);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskPsswd" id="pskPsswd" type="password" size="32" maxlength="30" onChange='checkPassword2(this,30,_("<%write(PSK);%>"),0)' class="input">&nbsp;<%write(charSize);%></td><td id="psktd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="gateway_td3"><%write(keyUpdateCyc);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskTime" type="text" size="10" maxlength="5"  onKeyPress="figureOnly()" id="pskTime" class="input" value = "0" onChange=''>&nbsp;&nbsp;<%write(miaoValueSizeTips);%>;</script></td><td id="pskkudcyctd"></td>
	   </tr>
</table>
	   </td></tr>
        <tr>
	   <td align="right"  width="27%" height="25" id="autoIssued_td">自动下发</td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="vlanType" type="checkbox" class="input-checkbox" id="vlanType" onClick="changeEn();"></td>
	   </tr>
        <tr>
	   	<td height="25" align="right" id="vlanrange_td">AP管理VLAN范围</td>
		<td ><!--<img src="images/qt.gif" width="7" height="9">--></td>
		<td ><input name="apvlan" id="apvlan" type="text" size="32" maxlength="32"  class="input" />&nbsp;</td><td id="psktd"></td>
	   </tr>
        <tr valign="top">
	   	<td height="25" align="right"></td>
		<td ><!--<img src="images/qt.gif" width="7" height="9">--></td>
		<td >(取值范围：1-32个字符。连续vlan可用“-”连接，不连续vlan可用“,”分开。 例10,15-20)</td><td id="psktd"></td>
	   </tr>
       
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="dhcpServer_td"><%write(startDhcp);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="dhcpServer" type="checkbox" class="input-checkbox" id="dhcpServer" onClick="changeDHCPfun();"></td>
	   </tr>
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="addrPool"><%write(addrPool);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><select name="pool" id="pool" class="input" onClick="poolChange();" onChange="poolChange();"></select></td>
	   </tr>
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="gateway_td"><%write(gatewayIpAddr);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input class="input" type="text" name="ipAddr" id="ipAddr" height="22" size="16" maxlength="15" value="" readonly onChange='checkIP(this,_("<%write(gateway);%>"),0)'></td>
	   </tr>
<!--
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="netMask_td"><%write(gatewayIpSub);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="maskAddr" id="maskAddr" size="16" maxlength="15" value="" readonly onChange='checkMask(edit_SZ.maskAddr, _("<%write(netMask);%>"),0)'></td>
	   </tr>
-->
	    <!--///wpapsktbl end/// -->
  </table> </td>
<table align="center"><tr><td height="5"></td></tr><tr><td>
<input type="button" onClick="save()"class="input-button" value="<%write(save);%>" name="" id="uttsave">&nbsp;
<!--<input type="button" class="input-button" value="刷新" name="" onClick="location.reload()">-->
<input type="button" class="input-button" value="<%write(refill);%>" name="" id="uttreset" onClick="Reset();">
<input type="button" class="input-button" value="<%write(help);%>" name="" id="utthelp" onClick="window.open('<% write(com_helppage); %>#serviceZone')">
</td></tr></table>
</form>
<!--//直接将页面的form填在此处，例如：<form  name="OpModeConfig" method="post"><tr><td></tr></td>-->
</td><td class="gapTd"></td></tr>
  </table>
 </td>
  </tr>
</table> 
  </td></tr>
</table></td></tr></table></td>
<!--////end-->
  <td width="15">&nbsp;</td>
  </tr>
</table>
</body>
</html>
