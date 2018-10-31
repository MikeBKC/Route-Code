 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=GBK"/>
<%langTran("common");%>
<%langTran("FastConfig");%>
<%langTran("edit_SZ");%>
<%langTran("commonWords");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

        var vlanId_tmp = 0;
<%
//	getOneServiceZone("default");
	aspAddNewServiceZone();
	aspOutPutServiceZone();
%>

function init() 
{
	var f=edit_SZ;
    //checkWepKeyType();
	//alert("edit_Names:"+edit_Names);
	if(f.modifyFlag5.value == "change")
	{
		//	alert("apVlan:"+apVlan);
		f.szEn_new.checked = ("<% write(Form("szEn"));%>" == "1")? true : false ;
		f.zoneName_new.value = "<% write(Form("FzoneName"));%>";
		f.ssid_new.value = "<% write(Form("ssid"));%>";
		f.encodeType.value = "<% write(Form("ssidEncodeType"));%>";
		f.broadcastEn_new.checked = ("<% write(Form("broadcastEn"));%>" == "1")? true : false ;
		f.isolateEn_new.checked = ("<%write(Form("isolateEn"));%>" == "1")? true : false ;
		f.vlanId_new.value = "<% write(Form("FvlanId"));%>";
//		f.max_user.value = "<% write(Form("max_user"));%>";
		f.encryType_new.value = "<% write(Form("encryType"));%>";
		f.wepAuthType_new.value = "<% write(Form("wepAuthType"));%>";
		f.wepFormat_new.value = "<% write(Form("wepFormat"));%>";
		f.wepRadio_new.value = "<% write(Form("wepRadio"));%>";
		var tmp_wepKeyRadio = "<%write(Form("wepRadio"));%>";
		if(tmp_wepKeyRadio == "")
		{
			tmp_wepKeyRadio = 0;
		}else{
			tmp_wepKeyRadio = parseInt(tmp_wepKeyRadio);
		}
		f.wepKeyRadio_new[tmp_wepKeyRadio].checked = true;
		f.wepKey1Text_new.value = "<% write(Form("wepKey1Text"));%>";
		f.wepKey2Text_new.value = "<% write(Form("wepKey2Text"));%>";
		f.wepKey3Text_new.value = "<% write(Form("wepKey3Text"));%>";
		f.wepKey4Text_new.value = "<% write(Form("wepKey4Text"));%>";
        //checkWepKeyType();
		f.wepKey1Type_new.value = "<% write(Form("wepKey1Type"));%>";
		f.wepKey2Type_new.value = "<% write(Form("wepKey2Type"));%>";
		f.wepKey3Type_new.value = "<% write(Form("wepKey3Type"));%>";
		f.wepKey4Type_new.value = "<% write(Form("wepKey4Type"));%>";
		f.wpaVersion_new.value = "<% write(Form("wpaVersion"));%>";
		f.wpaEncrp_new.value =  "<% write(Form("wpaEncrp"));%>";
		f.radiusIP_new.value = "<% write(Form("radiusIP"));%>";
		f.radiusPort_new.value = "<% write(Form("radiusPort"));%>";
		f.radiusPsswd_new.value = "<% write(Form("radiusPsswd"));%>";
		f.wpaTime_new.value = "<% write(Form("wpaTime"));%>";
		f.pskVersion_new.value = "<% write(Form("pskVersion"));%>";
		f.pskEncry_new.value = "<% write(Form("pskEncry"));%>";
		f.pskPsswd_new.value = "<% write(Form("pskPsswd"));%>";
		f.pskTime_new.value = "<% write(Form("pskTime"));%>";
		f.ipAddr_new.value = "<% write(Form("ipAddr"));%>";
//		f.ifAddr_new.value = "<% write(Form("ifAddr"));%>";
//		f.maskAddr_new.value = "<% write(Form("maskAddr"));%>";
		f.dhcpServer_new.checked = ("<% write(Form("dhcpServer"));%>" == "1")? true : false;
		f.vlanType_new.checked = ("<% write(Form("vlanType"));%>" == "1")? true : false;
		f.apvlan_new.value = "<% write(Form("apvlan"));%>";                           
		
		poolInit();
		f.pool_new.value = "<% write(Form("pool"));%>";

	}
	else
	{
		f.ipAddr_new.value = gateWays[0];
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
	getdisable(f.wepKey1Type_new,f.wepKey2Type_new,f.wepKey3Type_new,f.wepKey4Type_new);
	auto_disable(f.wepKey1Text_new,f.wepKey2Text_new,f.wepKey3Text_new,f.wepKey4Text_new,f.wepKeyRadio_new,f.wepKey1Type_new,f.wepKey2Type_new,f.wepKey3Type_new,f.wepKey4Type_new);
	auto_length(f.wepKey1Text_new,f.wepKey2Text_new,f.wepKey3Text_new,f.wepKey4Text_new,f.wepKey1Type_new,f.wepKey2Type_new,f.wepKey3Type_new,f.wepKey4Type_new,f.wepFormat_new.selectedIndex);
	/*
	getdisable();
	auto_disable();
	auto_length();*/
}

function checkWepKeyType() 
{
        var f=edit_SZ;
        if(f.wepKey1Type_new.value == "0")
        {
            f.wepKey1Text_new.readOnly = true;
        }
        else
        {
            f.wepKey1Text_new.readOnly = false;
        }
        if(f.wepKey2Type_new.value == "0")
        {
            f.wepKey2Text_new.readOnly = true;
        }
        else
        {
            f.wepKey2Text_new.readOnly = false;
        }
        if(f.wepKey3Type_new.value == "0")
        {
            f.wepKey3Text_new.readOnly = true;
        }
        else
        {
            f.wepKey3Text_new.readOnly = false;
        }
        if(f.wepKey4Type_new.value == "0")
        {
            f.wepKey4Text_new.readOnly = true;
        }
        else
        {
            f.wepKey4Text_new.readOnly = false;
        }
}

function changeDhcpPoolSel()
{
//	document.getElementById('dhcpPool').value=document.getElementById('vlanId').value
    var vid = document.getElementById('vlanId_new').value;
    if(vid == "0"){
	    document.getElementById('dhcpPool_new').value="<%write(noMatchPool);%>";
        return false;
    }
    if(vid == "")
        vid = 0;
    for(var i=0;i<poolVid.length;i++)
    {
        if(vid == poolVid[i])
        {
//	        document.getElementById('dhcpPool').value=poolNames[i];
	        document.getElementById('dhcpPool_new').value=poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
            break;
        }
    }
    if(i>=poolVid.length)
    {
	    document.getElementById('dhcpPool_new').value="<%write(noMatchPool);%>";
    }
	var dhcpPoolVid = "<%write(Form("dhcpVid"));%>";
	var dhcpPoolName = "<%write(Form("poolName"));%>";
	var dhcpPoolStart = "<%write(Form("dhcpStart"));%>";
	var dhcpPoolEnd = "<%write(Form("dhcpEnd"));%>";
	var poolVid_tmp = new Array();
	if (dhcpPoolVid != "")
	{
		poolVid_tmp = dhcpPoolVid.split("VIF"); 
		dhcpPoolVid = poolVid_tmp[1];
		if (vid == dhcpPoolVid)
		{
		    document.getElementById('dhcpPool_new').value= dhcpPoolName+ "(" + dhcpPoolStart + "～" + dhcpPoolEnd + ")";
		}

	}
}

function changeDHCPfun()
{
	if(edit_Names == "default")	//默认服务区
	{
		if (confirm(_("<%write(warning8);%>")))
		{
			if(document.getElementById('dhcpServer_new').checked)	//勾选→未勾选
			{
				document.getElementById('dhcpServer_new').checked = true;
			}
			else	//	未勾选→勾选
			{
				document.getElementById('dhcpServer_new').checked = false;
			}
		}
		else
		{
			if(document.getElementById('dhcpServer_new').checked)	//勾选→未勾选
			{
				document.getElementById('dhcpServer_new').checked = false;
			}
			else	//	未勾选→勾选
			{
				document.getElementById('dhcpServer_new').checked = true;
			}
		}
	}
	else
	{
		if(document.getElementById('dhcpServer_new').checked)	//勾选→未勾选
		{
			poolChange();
		}
	}
	if (document.getElementById('dhcpServer_new').checked == false)
	{
		document.getElementById('ipAddr_new').onkeydown= "";
	//	document.getElementById('maskAddr_new').onkeydown= "";
		document.getElementById('ipAddr_new').readOnly = false;
	//	document.getElementById('maskAddr_new').readOnly = false;
	}
	else
	{
		document.getElementById('ipAddr_new').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
	//	document.getElementById('maskAddr_new').onkeydown= function (e) { if(e ? e.which : event.keyCode==8) return false; }
		document.getElementById('ipAddr_new').readOnly = true;
	//	document.getElementById('maskAddr_new').readOnly = true;
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
    if (!checkName(edit_SZ.FzoneName, 11, _("<%write(svcZoneName);%>"), 0)) {
        return false;
    }
    if (!checkName(edit_SZ.ssid, 14, _("<%write(sz_ssid);%>"), 0)) {
        return false;
    }

//	if (!checkNumber(edit_SZ.max_user, 1, 32, _("<%write(mazxCLT);%>"), 0, 0)) {
//		return false;
//	}
	var i;
	for (var j=0; j<4; j++)
	{
		if (edit_SZ.wepKeyRadio_new[j].checked)
		{
			i=j+1;
		}
	}
	if (edit_SZ.encryType.value == 1)
	{
		var format = edit_SZ.wepFormat.value;
		if (format == 1) /* 16进制 */
		{
			var type=document.getElementById('wepKey' + i + 'Type_new').value;
			var wep_pass = document.getElementById('wepKey' + i + 'Text_new').value;
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
			var type=document.getElementById('wepKey' + i + 'Type_new').value;
			var wep_pass = document.getElementById('wepKey' + i + 'Text_new').value;
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
/*		
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
*/		
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
//	var vlanid = document.getElementById("vlanId_new").value;
	var vlanid = edit_SZ.FvlanId.value;

//var vlanType = document.getElementById("vlanType_new").checked;
	var vlanType = edit_SZ.vlanType.value;
		
	if(vlanid !="" && vlanType == 1){
			alert("无线VLAN ID 不能和自动下发同时使用");
			return false;
	}

	//var reg =/^([1-9][0-9]{0,3}-[1-9][0-9][0,3])$/;
//	  var reg =/^[0-9]{1}$|^([1-9][0-9]{1,3})$|^([1-9][0-9]{0,3},){1,}[1-9][0-9]{0,3}$|^([1-9][0-9]{0,3}-[1-9][0-9]{0,3},){1,}[1-9][0-9]{0,3}-[1-9][0-9]{0,3}$|^([1-9][0-9]{0,3}-[1-9][0-9]{0,3})$|^((([1-9][0-9]{0,3}|[1-9][0-9]{0,3}-[1-9][0-9]{0,3}),)+([1-9][0-9]{0,3}|[1-9][0-9]{0,3}-[1-9][0-9]{0,3}))$/;
	var reg =/^(0|[1-9][0-9]{0,3})$|^((0|[1-9][0-9]{0,3}),){1,}(0|[1-9][0-9]{0,3})$|^((0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3}),){1,}(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})$|^(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})$|^((((0|[1-9][0-9]{0,3})|(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})),)+((0|[1-9][0-9]{0,3})|(0|[1-9][0-9]{0,3})-(0|[1-9][0-9]{0,3})))$/; 

	//var apvlan=document.getElementById("apvlan").value;
	var apvlan = edit_SZ.apvlan.value;
	//alert("apvlan:"+apvlan);
	if(vlanType == 1){
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
	//if (document.getElementById('dhcpServer').checked == true)
	if (edit_SZ.dhcpServer.value == true)
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
		}
		if (trimstr(edit_SZ.maskAddr.value) == "0.0.0.0") {
			inputErro(edit_SZ.maskAddr, _("<%write(warning10);%>"));
			return false;
		}
		*/
	}
	/*检查服务区名字/无线VLAN ID 是否冲突*/
	var auto = document.getElementById("vlanType_new").checked;
	if(!auto)
	{
		for(var i=0;i<zoneName.length;i++)
		{
			if(edit_SZ.FzoneName.value == zoneName[i])
			{
				alert("服务区名与已有的配置冲突！");	
				return false;
			}
			if((edit_SZ.FvlanId.value == vlanId[i])&&(edit_SZ.FvlanId.value != ""))
			{
				alert("无线VLAN ID与已有的配置冲突！");
				return false;
			}
		}
	}
	return true;
}
function changeEn(){
	var auto = document.getElementById("vlanType_new").checked;
	if(auto){
			document.getElementById("vlanId_new").disabled = true;
//			document.getElementById("ifAddr_new").disabled = true;
//			document.getElementById("maskAddr_new").disabled = true;
			document.getElementById("apvlan_new").disabled = false;
	        document.getElementById('dhcpPool_new').value="<%write(noMatchPool);%>";
	}else{
			document.getElementById("vlanId_new").disabled = false;
//			document.getElementById("ifAddr_new").disabled = false;
//			document.getElementById("maskAddr_new").disabled = false;
			document.getElementById("apvlan_new").disabled = true;	
            changeDhcpPoolSel();
	}	
}


function poolInit()
{
	var option, i;
	var f=edit_SZ;
	for (i = f.pool_new.length; i > -1; i--)
	{
		f.pool_new.remove(i);
	}
	for (i = 0; i < poolNames.length; i++)
	{
		option = document.createElement("OPTION");
		option.value = poolNames[i];
		option.text = poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
		f.pool_new.options.add(option);
	}

	var dhcpPoolVid = "<%write(Form("dhcpVid"));%>";
	var dhcpPoolName = "<%write(Form("poolName"));%>";
	var dhcpPoolStart = "<%write(Form("dhcpStart"));%>";
	var dhcpPoolEnd = "<%write(Form("dhcpEnd"));%>";
	if(f.modifyFlag4.value == "change")
	{
		option = document.createElement("OPTION");
		option.value = dhcpPoolName;
		option.text = dhcpPoolName + "(" + dhcpPoolStart + "～" + dhcpPoolEnd + ")";
		f.pool_new.options.add(option);
	}
    var vid = vlanId_tmp;
    if(vid == "")
    {
        vid = 0;
    }
    for(var i=0;i<poolVid.length;i++)
    {
        if(vid == poolVid[i])
        {
//	        document.getElementById('dhcpPool').value=poolNames[i];
	        document.getElementById('dhcpPool_new').value=poolNames[i] + "(" + beginIp[i] + "～" + endIp[i] + ")";
            break;
        }
    }

	if(i>=poolVid.length)
    {
	    document.getElementById('dhcpPool_new').value="<%write(noMatchPool);%>";
    }

	var poolVid_tmp = new Array();
	if (dhcpPoolVid != "")
	{
		poolVid_tmp = dhcpPoolVid.split("VIF"); 
		dhcpPoolVid = poolVid_tmp[1];
		if (vid == dhcpPoolVid)
		{
		    document.getElementById('dhcpPool_new').value= dhcpPoolName+ "(" + dhcpPoolStart + "～" + dhcpPoolEnd + ")";
		}

	}
}

function poolChange()
{
	var m;
	var i = document.getElementById('pool_new').value;
	for (m = 0; m < poolNames.length; m++)
	{
		if (i == poolNames[m])
		{
			break;
		}
	}
//	edit_SZ.ifAddr.value = gateWays[m];
	edit_SZ.ipAddr_new.value = gateWays[m];
//	edit_SZ.maskAddr_new.value = netMasks[m];
//	inputRight(edit_SZ.ifAddr);
	inputRight(edit_SZ.ipAddr_new);
//	inputRight(edit_SZ.maskAddr_new);
}

function radioChange(value)
{
	edit_SZ.wepRadio_new.value = value;
}
function Reset()
{
	edit_SZ.reset();
	init();
	allInputRight(edit_SZ);
	inputRight(edit_SZ.wepKey1Text_new);
	inputRight(edit_SZ.wepKey2Text_new);
	inputRight(edit_SZ.wepKey3Text_new);
	inputRight(edit_SZ.wepKey4Text_new);
	inputRight(edit_SZ.radiusPsswd_new);
	inputRight(edit_SZ.pskPsswd_new);
}

function changSecMode(){
    var frm=edit_SZ;
	var index=getListValue(frm.encryType_new);
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

function wepPasswordLengthChange()
{
    checkWepKeyType();
	var i, index, format, type, pw, flag;
	i=document.getElementsByName("wepKeyRadio_new");
	for (index=0; index<i.length; index++)
	{
		if(i[index].checked==true)
		{
			  break;
		}
	}
	index++;
	//alert(index);
	format=edit_SZ.wepFormat_new.value;
	//alert(format);
	type=document.getElementById('wepKey' + index + 'Type_new').value;
	pw=document.getElementById('wepKey' + index + 'Text_new').value;
	//alert(type);
	if (format == 1) /* 16进制 */
	{
		if (type == 1) /* 64位,密码长度必须为10*/
		{
			document.getElementById('wepKey' + index + 'Text_new').maxLength=10;
			flag=10;
		}
		else if (type == 2) /* 128位,密码长度必须为26 */
		{
			document.getElementById('wepKey' + index + 'Text_new').maxLength=26;
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
			document.getElementById('wepKey' + index + 'Text_new').maxLength=5;
			flag=5;
		}
		else if (type == 2) /* 128位,密码长度必须为13 */
		{
			document.getElementById('wepKey' + index + 'Text_new').maxLength=13;
			flag=13;
		}
		else
		{
		}
	}
	else
	{
	}
	document.getElementById('wepKey' + index + 'Text_new').value=pw.substr(0, flag);
}


 function jumpthere1()
{
     edit_SZ.action="FastConfig.asp";
     edit_SZ.submit();
}

 function jumpthere2()
{
     edit_SZ.action="FastConfig_acWorkMode.asp";
     edit_SZ.submit();
}

 function jumpthere3()
{
     edit_SZ.action="FastConfig_Net_WAN1.asp";
     edit_SZ.submit();
}

 function jumpthere4()
{
     edit_SZ.action="FastConfig_edit_Vlan.asp";
     edit_SZ.submit();
}
 function jumpthere5()
{
     edit_SZ.action="FastConfig_edit_DhcpPool.asp";
     edit_SZ.submit();
}
 function jumpthere6()
{
     edit_SZ.action="FastConfig_edit_SZ.asp";
     edit_SZ.submit();
}
 function jumpthere7()
{
     edit_SZ.action="FastConfig_MacFilterGloable_Config.asp";
     edit_SZ.submit();
}
// function jumpthere8()
//{
  //   edit_SZ.action="apManage.asp";
  //   edit_SZ.submit();
//}


function dopre(){
	var f=edit_SZ;

	f.szEn.value = (f.szEn_new.checked == true ? 1:0);
	f.FzoneName.value = f.zoneName_new.value;
	f.ssid.value = f.ssid_new.value;
	f.ssidEncodeType.value = f.encodeType.value;
	f.broadcastEn.value = (f.broadcastEn_new.checked == true ? 1 : 0);
	f.isolateEn.value = (f.isolateEn_new.checked == true ? 1:0);
	f.FvlanId.value = f.vlanId_new.value;
//	f.max_user.value = client;
	f.encryType.value = f.encryType_new.value;
	f.wepAuthType.value = f.wepAuthType_new.value;
	f.wepFormat.value = f.wepFormat_new.value;
	f.wepRadio.value = f.wepRadio_new.value;
	f.wepKeyRadio.value = f.wepRadio_new.value;	
	f.wepKey1Text.value = f.wepKey1Text_new.value;
	f.wepKey2Text.value = f.wepKey2Text_new.value;
	f.wepKey3Text.value = f.wepKey3Text_new.value;
	f.wepKey4Text.value = f.wepKey4Text_new.value;

	f.wepKey1Type.value = f.wepKey1Type_new.value;
	f.wepKey2Type.value = f.wepKey2Type_new.value;
	f.wepKey3Type.value = f.wepKey3Type_new.value;
	f.wepKey4Type.value = f.wepKey4Type_new.value;
	f.wpaVersion.value = f.wpaVersion_new.value;
	f.wpaEncrp.value = f.wpaEncrp_new.value; 
	f.radiusIP.value = f.radiusIP_new.value;
	f.radiusPort.value = f.radiusPort_new.value;
	f.radiusPsswd.value = f.radiusPsswd_new.value;
	f.wpaTime.value = f.wpaTime_new.value;
	f.pskVersion.value = f.pskVersion_new.value;
	f.pskEncry.value = f.pskEncry_new.value;
	f.pskPsswd.value = f.pskPsswd_new.value;
	f.pskTime.value = f.pskTime_new.value;
	f.ipAddr.value = f.ipAddr_new.value;
//	f.ifAddr.value = f.ifAddr_new.value;
//	f.maskAddr.value = f.maskAddr_new.value;
	f.dhcpServer.value = (f.dhcpServer_new.checked == true ? 1 : 0);
	f.vlanType.value = (f.vlanType_new.checked == true ? 1 : 0);
	f.apvlan.value = f.apvlan_new.value;
	f.pool.value = f.pool_new.value;

	f.modifyFlag5.value = "change";
    f.action = "FastConfig_edit_DhcpPool.asp";
    f.submit();
}

function done(){
	var f=edit_SZ;

	f.szEn.value = (f.szEn_new.checked == true ? 1:0);
	f.FzoneName.value = f.zoneName_new.value;
	f.ssid.value = f.ssid_new.value;
	f.ssidEncodeType.value = f.encodeType.value;
	f.broadcastEn.value = (f.broadcastEn_new.checked == true ? 1 : 0);
	f.isolateEn.value = (f.isolateEn_new.checked == true ? 1:0);
	f.FvlanId.value = f.vlanId_new.value;
//	f.max_user.value = client;
	f.encryType.value = f.encryType_new.value;
	f.wepAuthType.value = f.wepAuthType_new.value;
	f.wepFormat.value = f.wepFormat_new.value;
	f.wepRadio.value = f.wepRadio_new.value;
	f.wepKeyRadio.value = f.wepRadio_new.value;	
	f.wepKey1Text.value = f.wepKey1Text_new.value;
	f.wepKey2Text.value = f.wepKey2Text_new.value;
	f.wepKey3Text.value = f.wepKey3Text_new.value;
	f.wepKey4Text.value = f.wepKey4Text_new.value;

	f.wepKey1Type.value = f.wepKey1Type_new.value;
	f.wepKey2Type.value = f.wepKey2Type_new.value;
	f.wepKey3Type.value = f.wepKey3Type_new.value;
	f.wepKey4Type.value = f.wepKey4Type_new.value;
	f.wpaVersion.value = f.wpaVersion_new.value;
	f.wpaEncrp.value = f.wpaEncrp_new.value; 
	f.radiusIP.value = f.radiusIP_new.value;
	f.radiusPort.value = f.radiusPort_new.value;
	f.radiusPsswd.value = f.radiusPsswd_new.value;
	f.wpaTime.value = f.wpaTime_new.value;
	f.pskVersion.value = f.pskVersion_new.value;
	f.pskEncry.value = f.pskEncry_new.value;
	f.pskPsswd.value = f.pskPsswd_new.value;
	f.pskTime.value = f.pskTime_new.value;
	f.ipAddr.value = f.ipAddr_new.value;
//	f.ifAddr.value = f.ifAddr_new.value;
//	f.maskAddr.value = f.maskAddr_new.value;
	f.dhcpServer.value = (f.dhcpServer_new.checked == true ? 1 : 0);
	f.vlanType.value = (f.vlanType_new.checked == true ? 1 : 0);
	f.apvlan.value = f.apvlan_new.value;
	f.pool.value = f.pool_new.value;
	f.modifyFlag5.value = "change";

	if(checkData()){
		f.pageAction.value = "6"; 
		f.action="/goform/formConfigFastDirectionAC";
		f.submit();
	}	
}

function goaway(){
	parent.main.location="firstPage.asp";
}
</script>



<style type="text/css">
<!--
.STYLE3 {
	font-family: "新宋体";
	color: #02727F;
}
.STYLE4 {color: #FFFFFF; font-family: "新宋体";}
.STYLE5 {
	font-family: "新宋体";
	color:#868686;
}
.STYLE6 {
	font-family: "新宋体";
	color:#84A6AC;
}

-->
</style>
</head>
<body onbeforeunload="logOut()">
<table width="100%" border="0"  cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  
  
  
  <tr >
    <td height="80">
	<table width="100%" height="80" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#C5C5C5"><!--上半部分-->
		<tr>
	<!--		<td width="40"></td>
			<td valign="top" width="54">
		      <table width="54" height="90" border="0" background="images/1.png">
			  	<tr>
					<td>
						<table width="40" align="center" border="0">
							<tr>
								<td height="65" valign="bottom" align="center" width="40"><span class="STYLE4">配置向导</span>								</td>
							</tr>
						</table>
					</td>
                </tr>
              </table>
			</td>-->
			
			
			
			<td valign="top">&nbsp;</td>
		  	<td valign="top" align="right" width="600">
		  		<table width="100%" border="0" cellpadding="0" cellspacing="0"><!--三行开始-->
				
            		<tr>
              			<td height="20" colspan="2">&nbsp;</td>
            		</tr>
					
					
					
					
            		<tr>
              			<td height="20" colspan="2">
							<table  border="0" cellpadding="0" cellspacing="0"><!--文字步骤-->
                          		<tr>
					<td width="60"></td>
                            		<td width="34" valign="bottom" align="center"><span class="STYLE6">开始</span></td>
									<td width="35"></td>
                            		<td width="54" valign="bottom" align="center"><span class="STYLE6">接入模式</span></td>
									<td width="33"></td>
                            		<td width="40" align="center" valign="bottom" class="STYLE6">WAN口</td>
									<td width="30"></td>
                            		<td width="55" align="center" valign="bottom" class="STYLE6">VLAN接口</td>
									<td width="18"></td>
                            		<td width="80" align="center" valign="bottom" class="STYLE6">DHCP服务器</td>
									<td width="15"></td>
                            		<td width="42" align="center" valign="bottom" class="STYLE3">服务区</td>
					<!--				<td width="8"></td>
                            		<td width="110" align="center" valign="bottom" class="STYLE5">无线MAC地址过滤</td>-->
								<!--	<td width="7"></td>
                            		<td  align="left" valign="bottom" class="STYLE5">完成</td>-->
                       		  </tr>
                       		 </table><!--文字步骤-->
						</td>
           			</tr>
					
					
					
            		<tr>
			        <td width="60"></td>
              			<td height="30" valign="top">
							<table width="200" border="0" cellpadding="0" cellspacing="0"><!--图片步骤-->
                          		<tr>
                            		<td>
										<div onclick="jumpthere1()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片1-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/4.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#84A6AC" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                           			<td>
										<div onclick="jumpthere2()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片2-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/4.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#84A6AC" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                           			<td>
										<div onclick="jumpthere3()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片3-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/4.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#84A6AC" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                            		<td>
										<div onclick="jumpthere4()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片4-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/4.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#84A6AC" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                            		<td>
										<div onclick="jumpthere5()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片5-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/4.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#84A6AC" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                           			<td>
										<div onclick="jumpthere6()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片6-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/2.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
									<!--
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0">
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table>
									</td>
                            		<td>
											<div onclick="jumpthere7()">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0">
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/3.png" cellpadding="0" cellspacing="0">
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table>
											 </td>
                                     		 </tr>
										</table></div>
									</td>-->
                            	<!--	<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0">
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table>
									</td>
									
									
									
									
									
                            		<td>
										<div onclick="jumpthere8()">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0">
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/3.png" cellpadding="0" cellspacing="0">
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table>
											 </td>
                                     		 </tr>
										</table></div>
									</td>-->
                            		
									
									
									
									
                         		 </tr>
                        	</table><!--图片步骤-->
						</td>
            		</tr>
					
					
          		</table><!--三行结束-->
			</td>
		
		
		
		
		
		</tr>
		
	</table><!--上半部分-->	
	</td>
  </tr>
  
  
  
  
  <!--****中间内容****-->
  <!--下半部分-->
  <tr>
  <td>
  <table width="100%" border="0"  cellspacing="0" cellpadding="0" align="center" bgcolor="#F3F3F3">
   <script language="javascript"></script>
   <tr><td width="100%" height="80"></td></tr>
  <tr valign="top">
  <td>
<form name="edit_SZ" method="post" action="">
 <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center"><tr><td align="center" class="style7">
<input type="hidden" name="wepRadio_new">
<table border="0"  width="600" align='center'>
  <tr><td colspan="4" height="10">&nbsp;</td></tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="zoneStatus_td"><%write(startSZ);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input type="checkbox" name="szEn_new" id="szEn_new" class="input-checkbox"></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="zoneName_td"><%write(SZName);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="zoneName_new" id="zoneName_new" size="16" maxlength="11" value="" onChange='checkName(this,11,_("<%write(svcZoneName);%>"),0)'></td>
	   </tr>
       	   <tr>
	   <td align="right"  width="27%" height="25" id="ssid_td">SSID</td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input type="text" class="input" name="ssid_new" id="ssid_new" size="16" maxlength="14" value="" onChange='checkName(this,14,_("<%write(sz_ssid);%>"),0)'>&nbsp;&nbsp;编码&nbsp;&nbsp;
		<select name="encodeType" id="encodeType" width=8px>
                    <option value="0" selected>电脑优先</option><!--gb2312-->
                    <option value="1">手机优先</option><!--utf8-->
		</select>
		</td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="broadcast_td"><%write(startSSID);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="broadcastEn_new" type="checkbox" class="input-checkbox" id="broadcastEn_new" checked></td>
	   </tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="isolate_td"><%write(startIsolate);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="isolateEn_new" type="checkbox" class="input-checkbox" id="isolateEn_new"></td>
	   </tr>
       <tr id="vlan_td">
	   <td align="right"  width="27%" height="25" id="vlanTag_td">无线VLAN ID</td>
	   <td  width="2%"><!--<img src="images/qt.gif" width="7" height="9">--></td>
	   <td><input type="text" class="input" name="vlanId_new" id="vlanId_new" onchange="changeDhcpPoolSel();" size="5" maxlength="4" value="">&nbsp;<%write(sz_confirm);%></td>
	   </tr>
       
	   <tr>
	   <td align="right"  width="27%" height="25" id="dhcpPool_td">DHCP<%write(addrPool);%></td>
	   <td  width="2%"><!--<img src="images/qt.gif" width="7" height="9">--></td>
	   <td><input type="text" class="input" name="dhcpPool_new" id="dhcpPool_new" size="50" maxlength="11" value="" disabled></td>
	   </tr>
<!--
       <tr id="virif_td">
	   <td align="right"  width="27%" height="25" id="ifIpAddr_td"><%write(ifIpAddr);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input class="input" type="text" name="ifAddr_new" id="ifAddr_new" height="22" size="16"  maxlength="15" value="" onChange='checkIP(this,_("<%write(ifIpAddr);%>"),0)' ></td>
	   </tr>
	   
	   <tr id="mask_td" style="display:">
	   <td align="right"  width="27%" height="25" id="netMask_td"><%write(ifNetMask);%></td>
	   <td  width="2%"></td>
	   <td><input type="text" class="input" name="maskAddr_new" id="maskAddr_new" size="16" maxlength="15" value="" onChange='checkMask(this, _("<%write(ifNetMask);%>"),0)'></td>
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
	   <td><select name="encryType_new" id="encryType_new" class="input" onChange="changSecMode()">
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
	   <td><select name="wepAuthType_new" id="wepAuthType_new" class="input" style="width:75px">
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
		<td ><select name="wepFormat_new" id="wepFormat_new" class="input" style="width:75px" onChange="setKey();">
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
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio_new" type="radio" id="wepKey1Radio_new" value="0" checked onClick="radioChange(this.value)" onfocus="if(key1dis){alert('input key1dis:'+key1dis); this.blur();}else this.form.wepKeyRadio[0].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey1Text_new" id="wepKey1Text_new" class="input"></td>
        			<TD ><select name="wepKey1Type_new" id="wepKey1Type_new" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

				<TR><TD  width="27%" height="25" align="right" id="gateway_td21"><%write(key);%>2：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio_new" type="radio" id="wepKey2Radio_new" value="1"  onClick="radioChange(this.value)" onfocus="if(key2dis) this.blur();else this.form.wepKeyRadio[1].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey2Text_new" id="wepKey2Text_new" class="input" onChange=''></td>
        			<TD ><select name="wepKey2Type_new" id="wepKey2Type_new" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

				<TR><TD  width="27%" height="25" align="right" id="gateway_td22"><%write(key);%>3：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio_new" type="radio" id="wepKey3Radio_new" value="2"  onClick="radioChange(this.value)" onfocus="if(key3dis) this.blur();else this.form.wepKeyRadio[2].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey3Text_new" id="wepKey3Text_new" class="input" onChange=''></td>
        			<TD ><select name="wepKey3Type_new" id="wepKey3Type_new" class="input" style="width:75px" onChange="setKey();">
						    <option value="0" id="forbid"><%write(disable);%></option>
						    <option value="1" id="64bit"><%write(bit64);%></option>
						    <option value="2" id="128bit"><%write(bit128);%></option>
        				</select></td><td></td></tr></table></td></tr>

				<TR><TD  width="27%" height="25" align="right" id="gateway_td22"><%write(key);%>4：</td><td width="2%">&nbsp;</td>
        			<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input name="wepKeyRadio_new" type="radio" id="wepKey4Radio_new" value="3"  onClick="radioChange(this.value)" onfocus="if(key4dis) this.blur();else this.form.wepKeyRadio[3].checked=true;">&nbsp;&nbsp;&nbsp;&nbsp;<input type="text" maxLength=32 size="30" name="wepKey4Text_new" id="wepKey4Text_new" class="input" onChange=''></td>
        			<TD ><select name="wepKey4Type_new" id="wepKey4Type_new" class="input" style="width:75px" onChange="setKey();">
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
	   <td><select name="wpaVersion_new" id="wpaVersion_new" class="input" style="width:75px">
		    <option value="0" selected id="auot"><%write(auto);%></option>
		    <option value="1">WPA</option>
		    <option value="2">WPA2</option>
		  </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td13"><%write(encrypAlgo);%></td>
		<td >&nbsp;</td>
		<td ><select name="wpaEncrp_new" id="wpaEncrp_new" class="input" style="width:75px">
		    <option value="0" selected id="auto"><%write(auto);%></option>
		    <option value="1">TKIP</option>
		    <option value="2">AES</option>
		  </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td10"><%write(radiusSZIp);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusIP_new" id="radiusIP_new" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange='checkIP(this,_("<%write(RadiusSVCIP);%>"),0)' class="input"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td9"><%write(radiusSZPort);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPort_new" id="radiusPort_new" type="text" onKeyPress="figureOnly()" onChange='checkNumber(this,1,65535,_("<%write(RadiusPort);%>"),0,0)' size="10" maxlength="5" class="input">&nbsp;&nbsp;
		    <%write(tips_sec);%></td><td id="radiusporttd"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td8"><%write(radiusPasswd);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPsswd_new" id="radiusPsswd_new" type="password" size="32" maxlength="30" onChange='checkPassword2(this,30,_("<%write(RadiusKey);%>"),0)' class="input">&nbsp;&nbsp;<%write(charSize);%></td><td  id="radiuspatd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="gateway_td3"><%write(keyUpdateCyc);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="wpaTime_new" type="text" size="10" maxlength="5" onKeyPress="figureOnly()" id="wpaTime_new" class="input" value="0" onChange=''>&nbsp;&nbsp;<%write(miaoValueSizeTips);%></td><td  id="keyudcyctd"></td>
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
	   <td><select name="pskVersion_new" id="pskVersion_new" class="input" style="width:75px">
		    <option value="0" selected id="auto"><%write(auto);%></option>
		    <option value="1">WPA</option>
		    <option value="2">WPA2</option>
		    </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td13"><%write(encrypAlgo);%></td>
		<td >&nbsp;</td>
		<td ><select name="pskEncry_new" id="pskEncry_new" class="input" style="width:75px">
		    <option value="0" selected id="auto"><%write(auto);%></option>
		    <option value="1">TKIP</option>
		    <option value="2">AES</option>
		  </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="gateway_td4"><%write(PSK);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskPsswd_new" id="pskPsswd_new" type="password" size="32" maxlength="30" onChange='checkPassword2(this,30,_("<%write(PSK);%>"),0)' class="input">&nbsp;<%write(charSize);%></td><td id="psktd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="gateway_td3"><%write(keyUpdateCyc);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskTime_new" type="text" size="10" maxlength="5"  onKeyPress="figureOnly()" id="pskTime_new" class="input" value = "0" onChange=''>&nbsp;&nbsp;<%write(miaoValueSizeTips);%>;</script></td><td id="pskkudcyctd"></td>
	   </tr>
</table>
	   </td></tr>
        <tr>
	   <td align="right"  width="27%" height="25" id="autoIssued_td">自动下发</td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="vlanType_new" type="checkbox" class="input-checkbox" id="vlanType_new" onClick="changeEn();"></td>
	   </tr>
        <tr>
	   	<td height="25" align="right" id="vlanrange_td">AP管理VLAN范围</td>
		<td ><!--<img src="images/qt.gif" width="7" height="9">--></td>
		<td ><input name="apvlan_new" id="apvlan_new" type="text" size="32" maxlength="32"  class="input" />&nbsp;</td><td id="psktd"></td>
	   </tr>
        <tr valign="top">
	   	<td height="25" align="right"></td>
		<td ><!--<img src="images/qt.gif" width="7" height="9">--></td>
		<td >(取值范围：1-32个字符。连续vlan可用“-”连接，不连续vlan可用“,”分开。 例10,15-20)</td><td id="psktd"></td>
	   </tr>
       
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="dhcpServer_td"><%write(startDhcp);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><input name="dhcpServer_new" type="checkbox" class="input-checkbox" id="dhcpServer_new" onClick="changeDHCPfun();"></td>
	   </tr>
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="addrPool"><%write(addrPool);%></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><select name="pool_new" id="pool_new" class="input" onClick="poolChange();" onChange="poolChange();"></select></td>
	   </tr>
	   <tr style="display:none;">
	   <td align="right"  width="27%" height="25" id="gateway_td"><%write(gatewayIpAddr);%></td>
	   <td  width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input class="input" type="text" name="ipAddr_new" id="ipAddr_new" height="22" size="16" maxlength="15" value="" readonly onChange='checkIP(this,_("<%write(gateway);%>"),0)'></td>
	   </tr>

	    <!--///wpapsktbl end/// -->
<tr> <td colspan="4" width="100%"  height="30" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="上一步" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="重填" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="离开" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="done()" value="完成" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="window.open('rhelpinfo.htm#WANConfig')" value="帮助" id="help"></td>

				   	<input type="hidden" name="pageAction" value="" >
				   	<input type="hidden" name="modifyFlag1" value="<% write(Form("modifyFlag1"));%>" >
				   	<input type="hidden" name="acGateWay" value="<% write(Form("acGateWay"));%>" >
					<input type="hidden" name="acDns" value="<% write(Form("acDns"));%>" >
					<input type="hidden" name="acMode" value="<% write(Form("acMode"));%>" >
								
				   	<input type="hidden" name="modifyFlag2" value="<% write(Form("modifyFlag2"));%>" >
					<input type="hidden" name="connectionType" value="<% write(Form("connectionType")); %>">
					<input type="hidden" name="staticIp" value="<% write(Form("staticIp")); %>">
					<input type="hidden" name="staticNetmask" value="<% write(Form("staticNetmask")); %>">
					<input type="hidden" name="staticGateway" value="<% write(Form("staticGateway")); %>">
					<input type="hidden" name="staticPriDns" value="<% write(Form("staticPriDns")); %>">
					<input type="hidden" name="staticSecDns" value="<% write(Form("staticSecDns")); %>">
					<input type="hidden" name="pppoeUser" value="<% write(Form("pppoeUser")); %>">
					<input type="hidden" name="pppoePass" value="<% write(Form("pppoePass")); %>">
		
				   	<input type="hidden" name="modifyFlag3" value="<% write(Form("modifyFlag3"));%>" >
    				<input type="hidden" name="VlanEnable" value="<% write(Form("VlanEnable"));%>" >
				    <input type="hidden" name="VlanName" value="<% write(Form("VlanName"));%>" >
					<input type="hidden" name="VlanVid" value="<% write(Form("VlanVid"));%>" >
					<input type="hidden" name="VlanStart" value="<% write(Form("VlanStart"));%>" >
					<input type="hidden" name="VlanMask" value="<% write(Form("VlanMask"));%>" >

				   	<input type="hidden" name="modifyFlag4" value="<% write(Form("modifyFlag4"));%>" >
					<input type="hidden" name="DhcpEnable" value="<% write(Form("DhcpEnable"));%>" >
					<input type="hidden" name="poolName" value="<% write(Form("poolName"));%>" >
					<input type="hidden" name="dhcpVid" value="<% write(Form("dhcpVid"));%>" >
				    <input type="hidden" name="dhcpStart" value="<% write(Form("dhcpStart"));%>" >
					<input type="hidden" name="dhcpEnd" value="<% write(Form("dhcpEnd"));%>" >
					<input type="hidden" name="dhcpMask" value="<% write(Form("dhcpMask"));%>" >
					<input type="hidden" name="dhcpGateway" value="<% write(Form("dhcpGateway"));%>" >
				    <input type="hidden" name="dhcpPriDns" value="<% write(Form("dhcpPriDns"));%>" >
					<input type="hidden" name="dhcpSecDns" value="<% write(Form("dhcpSecDns"));%>" >
					<input type="hidden" name="dhcpLease" value="<% write(Form("dhcpLease"));%>" >

				   	<input type="hidden" name="modifyFlag5" value="<% write(Form("modifyFlag5"));%>" >
					<input type="hidden" name="szEn" value="<% write(Form("szEn"));%>" >
                    <input type="hidden" name="FzoneName" value="<% write(Form("FzoneName"));%>" >
                   	<input type="hidden" name="ssid" value="<% write(Form("ssid"));%>" >
                   	<input type="hidden" name="ssidEncodeType" value="<% write(Form("ssidEncodeType"));%>" >
                    <input type="hidden" name="broadcastEn" value="<% write(Form("broadcastEn"));%>" >
                    <input type="hidden" name="isolateEn" value="<% write(Form("isolateEn"));%>" >
	                <input type="hidden" name="FvlanId" value="<% write(Form("FvlanId"));%>" >
                    <input type="hidden" name="max_user" value="<% write(Form("max_user"));%>" >
                    <input type="hidden" name="encryType" value="<% write(Form("encryType"));%>" >
                    <input type="hidden" name="wepAuthType" value="<% write(Form("wepAuthType"));%>" >
				   	<input type="hidden" name="wepFormat" value="<% write(Form("wepFormat"));%>" >
					<input type="hidden" name="wepRadio" value="<% write(Form("wepRadio"));%>" >
					<input type="hidden" name="wepKeyRadio" value="<% write(Form("wepKeyRadio"));%>" >
					<input type="hidden" name="wepKey1Text" value="<% write(Form("wepKey1Text"));%>" >
					<input type="hidden" name="wepKey2Text" value="<% write(Form("wepKey2Text"));%>" >
				   	<input type="hidden" name="wepKey3Text" value="<% write(Form("wepKey3Text"));%>" >
				   	<input type="hidden" name="wepKey4Text" value="<% write(Form("wepKey4Text"));%>" >
					<input type="hidden" name="wepKey1Type" value="<% write(Form("wepKey1Type"));%>" >
					<input type="hidden" name="wepKey2Type" value="<% write(Form("wepKey2Type"));%>" >
					<input type="hidden" name="wepKey3Type" value="<% write(Form("wepKey3Type"));%>" >
					<input type="hidden" name="wepKey4Type" value="<% write(Form("wepKey4Type"));%>" >
					<input type="hidden" name="wpaVersion" value="<% write(Form("wpaVersion"));%>" >
					<input type="hidden" name="wpaEncrp" value="<% write(Form("wpaEncrp"));%>" >
					<input type="hidden" name="radiusIP" value="<% write(Form("radiusIP"));%>" >
				   	<input type="hidden" name="radiusPort" value="<% write(Form("radiusPort"));%>" >
					<input type="hidden" name="radiusPsswd" value="<% write(Form("radiusPsswd"));%>" >
					<input type="hidden" name="wpaTime" value="<% write(Form("wpaTime"));%>" >
					<input type="hidden" name="pskVersion" value="<% write(Form("pskVersion"));%>" >
					<input type="hidden" name="pskEncry" value="<% write(Form("pskEncry"));%>" >
					<input type="hidden" name="pskPsswd" value="<% write(Form("pskPsswd"));%>" >
					<input type="hidden" name="pskTime" value="<% write(Form("pskTime"));%>" >
					<input type="hidden" name="ipAddr" value="<% write(Form("ipAddr"));%>" >
<!--				<input type="hidden" name="ifAddr" value="<% write(Form("ifAddr"));%>" >
					<input type="hidden" name="maskAddr" value="<% write(Form("maskAddr"));%>" >
-->					
					<input type="hidden" name="dhcpServer" value="<% write(Form("dhcpServer"));%>" >
					<input type="hidden" name="vlanType" value="<% write(Form("vlanType"));%>" >
					<input type="hidden" name="apvlan" value="<% write(Form("apvlan"));%>" >
					<input type="hidden" name="pool" value="<% write(Form("pool"));%>" >                           

				   	<input type="hidden" name="modifyFlag6" value="<% write(Form("modifyFlag6"));%>" >
					<input type="hidden" name="confName" value="<% write(Form("confName"));%>" >
					<input type="hidden" name="MacFilterEnable" value="<% write(Form("MacFilterEnable"));%>" >
				   	<input type="hidden" name="servicezone" value="<% write(Form("servicezone"));%>" >
				   	<input type="hidden" name="filterRule" value="<% write(Form("filterRule"));%>" >
				   	<input type="hidden" name="textarea" value="<% write(Form("textarea"));%>" >


                    <!--  	<input type="hidden" name="" value="<% write(Form(""));%>" >-->



</tr>

</table>
</td></tr></table>
</form></td>
  </tr>
           <script language="javascript">init();</script>

</table>
  </td>
  </tr>
</table>
</body>
</html>


