<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("WlanMultiSsid_edit");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

Butterlate.setTextDomain("wireless");

function initTranslation()
{
	var e;
	e = document.getElementById("safe mode");
	e.innerHTML= _("safe mode");
	e = document.getElementById("certification mode");
	e.innerHTML= _("certification mode");
	e = document.getElementById("discription5");
	e.innerHTML= _("discription5");
	e = document.getElementById("key format");
	e.innerHTML= _("key format");
	e = document.getElementById("key choose");
	e.innerHTML= _("key choose");
	e = document.getElementById("wep key");
	e.innerHTML= _("wep key");
	e = document.getElementById("key mode");
	e.innerHTML= _("key mode");
	e = document.getElementById("WPA version");
	e.innerHTML= _("WPA version");
	e = document.getElementById("WPA version2");
	e.innerHTML= _("WPA version");
	e = document.getElementById("encryption");
	e.innerHTML= _("encryption");
	e = document.getElementById("encryption2");
	e.innerHTML= _("encryption");
	e = document.getElementById("Radius Service IP");
	e.innerHTML= _("Radius Service IP");
	e = document.getElementById("Radius port");
	e.innerHTML= _("Radius port");
	e = document.getElementById("radius password");
	e.innerHTML= _("radius password");
	e = document.getElementById("key update time");
	e.innerHTML= _("key update time");
	e = document.getElementById("key update time2");
	e.innerHTML= _("key update time");
	e = document.getElementById("pre-shared key");
	e.innerHTML= _("pre-shared key");
	e = document.getElementById("start wireless");
	e.innerHTML= _("start wireless");
	e = document.getElementById("start SSID");
	e.innerHTML= _("start SSID");
	
	e = document.getElementById("save");
	e.value= _("save");
	e = document.getElementById("goback");
	e.value= _("goback");	
	
}

var errorstr="";
/////////////////////////////交互变量/////////////////////
/*
var WrlessEnables="on";//启用无线功能
var wrlesModes="11GHT20";//模式和带宽
var broadcastEnables="on";//启用SSID广播
var vlanids="456";
*/
<%
	var edit_ssid="";
	edit_ssid=Form("id");
	if ( edit_ssid!="") aspOutputOneWlanMultiSsid(edit_ssid);

%>
var edit_ssid_js = "<% write(Form("id"));%>";//alert(edit_ssid_js);

//if(secModes=="") secModes="NONE";
var secmodeopt=new Array();
secmodeopt[0]=new Array("NONE","无安全机制");
secmodeopt[1]=new Array("WEP","WEP");
secmodeopt[2]=new Array("WPA","WPA/WPA2");;
secmodeopt[3]=new Array("WPAPSK","WPA-PSK/WPA2-PSK");
var key1dis = 0;
var key2dis = 0;
var key3dis = 0;
var key4dis = 0;
var keylength1 = 10;
var keylength2 = 10;
var keylength3 = 10;
var keylength4 = 10;
var is_Hex = true;
<!--


function init(){
	var f=WlanMultiSsid_Config;
	if(edit_ssid_js!=""&&edit_ssid_js!=null){
		f.WrlessEnable.checked=(WrlessEnables=="on"?true:false);
		f.ssid.value=ssids;
		f.ssidold.value=ssids;
		f.broadcastEnable.checked=(broadcastEnables=="on"?true:false);
		//f.vlanid.value=vlanids;
		selVal(f.secMode,secModes);
		f.wepAuthMode.value=(wepAuthModes==""?"auto":wepAuthModes);
		f.keyFormat.value=(keyFormats==""?"ASCII":keyFormats);
		f.wepkey1.value=wepkey1s;
		f.keyLength1.value=(keyLength1s==""?"0":keyLength1s);
		f.wepkey2.value=wepkey2s;
		f.keyLength2.value=(keyLength2s==""?"0":keyLength2s);
		f.wepkey3.value=wepkey3s;
		f.keyLength3.value=(keyLength3s==""?"0":keyLength3s);
		f.wepkey4.value=wepkey4s;
		f.keyLength4.value=(keyLength4s==""?"0":keyLength4s);
		keynums=(keynums==""?1:keynums);
		f.keynum[keynums-1].checked=true;
	
		f.wpaAuthMode.value=(wpaAuthModes==""?"auto":wpaAuthModes);
		f.wpaCipher.value=(wpaCiphers==""?"auto":wpaCiphers);
		f.radiusIP.value=(radiusIPs==""?"":radiusIPs);
		f.radiusPort.value=(radiusPorts==""?"1812":radiusPorts);
		f.radiusPsswd.value=radiusPsswds;
		f.keyUpdateCyc.value=(keyUpdateCycs==""?"0":keyUpdateCycs);
	
		f.wpapskAuthMode.value=(wpapskAuthModes==""?"auto":wpapskAuthModes);
		f.wpapskCipher.value=(wpapskCiphers==""?"auto":wpapskCiphers);
		f.pskPsswd.value=pskPsswds;
		f.pskkeyUpdateCyc.value=(pskkeyUpdateCycs==""?"0":pskkeyUpdateCycs);
		changSecMode();
		
		f.Action.value="modify";
	}
	setKey();
	ssidtd.innerHTML="";//vlanidtd.innerHTML="";
	pskkudcyctd.innerHTML="";keyudcyctd.innerHTML="";
	radiuspatd.innerHTML="";radiuspatd.innerHTML="";
	radiusporttd.innerHTML="";radiusiptd.innerHTML="";
	 for (var i=1; i<5; i++){
	 	document.all["wepkeytd"+i].innerHTML="";
	 }
}
function changSecMode(){
    var frm=WlanMultiSsid_Config;
	var index=frm.secMode.selectedIndex;
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
		case 0:		weptbl1.style.display="none";
					wpatbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display=dis;
					break;
		case 1:		weptbl1.style.display=dis;
					wpatbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case 2:		weptbl1.style.display="none";
					wpatbl1.style.display=dis;
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case 3:		weptbl1.style.display="none";
					wpatbl1.style.display="none";
					wpapsktbl1.style.display=dis;
					wutbl.style.display="none";
		default:	break;
	}

}

function setKey()
{
	var f = WlanMultiSsid_Config
	getdisable();
	auto_disable();
	auto_length();
}
function getdisable()
{
	var f =WlanMultiSsid_Config;
	key1dis = (f.keyLength1.selectedIndex == 0);
	key2dis = (f.keyLength2.selectedIndex == 0);
	key3dis = (f.keyLength3.selectedIndex == 0);
	key4dis = (f.keyLength4.selectedIndex == 0);
}
function auto_disable()
{
	var f =WlanMultiSsid_Config;
	var keychoosed;
	f.wepkey1.disabled = f.keynum[0].disabled = key1dis;
	f.wepkey2.disabled = f.keynum[1].disabled = key2dis;
	f.wepkey3.disabled = f.keynum[2].disabled = key3dis;
	f.wepkey4.disabled = f.keynum[3].disabled = key4dis;
	for (i = 0; i < f.keynum.length; i ++)
	{
		if (f.keynum[i].checked)
		{
			keychoosed = f.keynum[i].value;
			break;
		}
	}
	if ((0 == key1dis) || (0 == key2dis) || (0 == key3dis) || (0 == key4dis))
	{
		if ((keychoosed == 1 && f.keyLength1.selectedIndex == 0) ||
			(keychoosed == 2 && f.keyLength2.selectedIndex == 0) ||
			(keychoosed == 3 && f.keyLength3.selectedIndex == 0) ||
			(keychoosed == 4 && f.keyLength4.selectedIndex == 0))
		{
			if (0 == key1dis)
				f.keynum[0].checked = 1;
			else if (0 == key2dis)
				f.keynum[1].checked = 1;
			else if (0 == key3dis)
				f.keynum[2].checked = 1;
			else
				f.keynum[3].checked = 1;
		}
	}
}

function auto_length()
{
	var f = WlanMultiSsid_Config;

	keylength1 =(1 == key1dis) ? keylength1 :(((3 == f.keyLength1.selectedIndex) ? 16 : ((2 == f.keyLength1.selectedIndex) ? 13 : 5)) *((0 == f.keyFormat.selectedIndex) ? 2 : 1));
	keylength2 =(1 == key2dis) ? keylength2 :(((3 == f.keyLength2.selectedIndex) ? 16 : ((2 == f.keyLength2.selectedIndex) ? 13 : 5)) *((0 == f.keyFormat.selectedIndex) ? 2 : 1));
	keylength3 =(1 == key3dis) ? keylength3 :(((3 == f.keyLength3.selectedIndex) ? 16 : ((2 == f.keyLength3.selectedIndex) ? 13 : 5)) *((0 == f.keyFormat.selectedIndex) ? 2 : 1));
	keylength4 =(1 == key4dis) ? keylength4 :(((3 == f.keyLength4.selectedIndex) ? 16 : ((2 == f.keyLength4.selectedIndex) ? 13 : 5)) *((0 == f.keyFormat.selectedIndex) ? 2 : 1));

	if ((f.wepkey1.value.length > keylength1) && (key1dis != 1))
		f.wepkey1.value = f.wepkey1.value.substring(0,keylength1);
	if ((f.wepkey2.value.length > keylength2) && (key2dis != 1))
		f.wepkey2.value = f.wepkey2.value.substring(0,keylength2);
	if ((f.wepkey3.value.length > keylength3) && (key3dis != 1))
		f.wepkey3.value = f.wepkey3.value.substring(0,keylength3);
	if ((f.wepkey4.value.length > keylength4) && (key4dis != 1))	
		f.wepkey4.value = f.wepkey4.value.substring(0,keylength4);
	f.wepkey1.maxLength =  keylength1;
	f.wepkey2.maxLength =  keylength2;
	f.wepkey3.maxLength =  keylength3;
	f.wepkey4.maxLength =  keylength4;

	if (f.keyFormat.selectedIndex == 0)
		is_Hex = true;
	else
		is_Hex = false;
}
function save(){
	var f=WlanMultiSsid_Config;
	if(checkData()){
		WlanMultiSsid_Config.action="/goform/ConfigWlanMultiSsid";
		WlanMultiSsid_Config.submit();
	}
}
function checkData(){
	var f=WlanMultiSsid_Config;
	if(!checkEnglishName(f.ssid,32,"SSID",0)) return false;
	//if(!checkNumber(f.vlanid,1,4092,"VLAN ID",0)) return false;
	var selIndex=f.secMode.selectedIndex;
	if(selIndex==1){
		if(!checkWEP()) return false;
	}
	else if(selIndex==2){
		if(!checkWPA()) return false;
	}
	else if(selIndex==3){
		if(!checkWPAPSK()) return false;
	}
	return true;
}
function checkWEP()
{
	var f = WlanMultiSsid_Config;var str="该密钥类型下的";
	if(key1dis&&key2dis&&key3dis&&key4dis){
		alert("请至少配置一个密钥！");return false;
	}
	if(!key1dis){
		if(f.wepkey1.value.length!=keylength1){alert(str+"密钥1必须为"+keylength1+"位"); return  false;}
		else{
			if(f.keyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.wepkey1,keylength1,"密钥1",0))	return false;
			}
			else{
				if (!checkHex(f.wepkey1,"密钥1",0))	return false;
			}
		}
	}
	if(!key2dis){
		if(f.wepkey2.value.length!=keylength2){alert(str+"密钥2必须为"+keylength2+"位"); return  false;}
		else{
			if(f.keyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.wepkey2,keylength2,"密钥2",0))	return false;
			}
			else{
				if (!checkHex(f.wepkey2,"密钥2",0))	return false;
			}
		}	
	}
	if(!key3dis){
		if(f.wepkey3.value.length!=keylength3){alert(str+"密钥3必须为"+keylength3+"位"); return  false;}
		else{
			if(f.keyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.wepkey3,keylength3,"密钥3",0))	return false;
			}
			else{
				if (!checkHex(f.wepkey3,"密钥3",0))	return false;
			}
		}	
	}
	if(!key4dis){
		if(f.wepkey4.value.length!=keylength4){alert(str+"密钥4必须为"+keylength4+"位"); return  false;}
		else{
			if(f.keyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.wepkey4,keylength4,"密钥4",0))	return false;
			}
			else{
				if (!checkHex(f.wepkey4,"密钥4",0))	return false;
			}
		}
	}
	return true;
}
function checkWPA(){
	var f = WlanMultiSsid_Config;
	if(!checkIP(f.radiusIP,"Radius服务器IP",0)) return false;
	if(!checkNumber(f.radiusPort,1,65535,"Radius服务器端口",0)) return false;
	if(!checkPassword1(f.radiusPsswd,"11",0)) return false;
	if(f.keyUpdateCyc.value!="0"){
		if(!checkNumber(f.keyUpdateCyc,60,86400,"密钥更新周期",0)) return false;
	}
	return true;
}
function checkWPAPSK(){
	var f = WlanMultiSsid_Config;
	var len=f.pskPsswd.value.length;
	if(len<8){alert("PSK密码长度不能小于8位！");return false;}
	if(!checkPassword1(f.pskPsswd,"63",0)) return false;
	if(f.pskkeyUpdateCyc.value!="0"){
		if(!checkNumber(f.pskkeyUpdateCyc,60,86400,"密钥更新周期",0)) return false;
	}
	return true;
}
//-->
</script>
</head>
<body onLoad="initTranslation()"onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle(_("more SSID"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="WlanMultiSsid_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="90%" align='center'>
  <tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
  
   <tr>
	   <td width="27%" height="25" align="right" id="start wireless">启用无线功能</td>
	   <td width="2%">&nbsp;</td>
	   <td><input name="WrlessEnable" type="checkbox"></td>
	   </tr>
       <tr>
	   <td align="right" height="25">SSID</td>
	   <td >&nbsp;</td>
	   <td ><input name="ssid" type="text" size="15" maxlength="32"  onChange="showfiledtip(re_checkEnglishName(this,'SSID'),ssidtd)"></td><td  id="ssidtd"></td>
	   </tr>
	   <input type="hidden" name="ssidold" >
	    <!--<tr>
	   <td align="right" height="25">VLAN ID</td>
	   <td >&nbsp;</td>
	   <td><input name="vlanid" type="text" onKeyPress="figureOnly()" size="15" maxlength="4" onChange="showfiledtip(re_checkNumber(this,'1','4092','VLAN ID'),vlanidtd)"></td><td  id="vlanidtd"></td>
	   </tr>-->
	    <tr>
	   <td align="right" height="25" id="start SSID">启用SSID广播</td>
	   <td >&nbsp;</td>
	   <td><input name="broadcastEnable" type="checkbox"></td>
	   </tr>
	   <!--/////////安全机制//////////////-->
	   <tr>
	   <td align="right"width="27%" height="25" id="safe mode">安全机制</td>
	   <td  width="2%">&nbsp;</td>
	   <td><select name="secMode" class="list" onChange="changSecMode()">
	   <script language="javascript">
	   		for(i=0;i<secmodeopt.length;i++){
			 	document.write('<option value="'+secmodeopt[i][0]+'">'+secmodeopt[i][1]+'</option>');
			}
	   </script>
	   </select></td>
	   </tr>
	     <tr id="wutbl" style=" display:none"><td colspan="4"></td></tr>
	   <!--///weptbl///-->
	   <tr id="weptbl1" style=" display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right"  width="27%" height="25" id="certification mode">认证类型</td>
	   <td  width="3%">&nbsp;</td>
	   <td><select name="wepAuthMode" class="list">
	   <option value="auto" id="auto"><script language="javascript">document.write(_("auto"));</script></option>
	   <option value="opensys" id="opensys">开放系统</option>
	   <option value="sharekey" id="sharekey">共享密钥</option>
	   </select></td>
	   </tr>
	     <tr><td colspan="4"><table width="100%"><tr>
	  <td width="27%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription5">自动表示设备会根据无线客户端的请求自动选择开放系统或共享密钥方式。</td>	
	  </tr></table></td></tr>
	   <tr>
	   	<td height="25" align="right" id="key format">密钥格式</td>
		<td >&nbsp;</td>
		<td ><select name="keyFormat" class="list" onChange="setKey()">
	   <option value="HEX" id="16 hex">16进制</option>
	   <option value="ASCII" selected id="ASCII code">ASCII码</option>
	   </select></td>
	   </tr>
	   <tr>
	   <td width="27%" height="25" align="right" id="key choose">密钥选择</td>
	   <td width="2%">&nbsp;</td>
	   <td><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr>
	   <td width="55%" id="wep key">WEP密钥</td>
	   <td id="key mode">密钥类型</td>
	   </tr></table></td>
	   </tr>
	   <script language="javascript">
	   for (var i=1; i<5; i++)
        		{   
        			j = i-1;
        			document.write('<TR><TD  width="27%" height="25" align="right">密钥 '+i+'：<input type=radio  disabled name=keynum value='+i+'></td><td width="2%">&nbsp;</td>');
        			document.write('<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input  disabled type=text class=text name=wepkey'+i+' maxLength=32 size="30" onfocus="if(key'+i+'dis) this.blur();else this.form.keynum['+j+'].checked=true;" onChange="showfiledtip(re_checkEnglishName(this),wepkeytd'+i+')"></td>' );
        			document.write('<TD ><select name=keyLength'+i+' class=list id=keyLength'+i+' onchange="setKey();" >');
        			document.write('<option value=0 >'+_("unable")+'</option>');
        			document.write('<option value=5 >'+_("64 bit")+'</option>');
        			document.write('<option value=13>'+_("128 bit")+'</option>');
        			document.write('<option value=16>'+_("152 bit")+'</option>');
        			document.write('</select></td><td id=wepkeytd'+i+'></td></tr></table></td></tr>');
        		}
	   </script>
</table>
	   </td></tr>
	    <!--///weptbl end///-->
		 <!--///wpatbl///-->
	   <tr id="wpatbl1" style="display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="27%" height="25" id="WPA version">WPA版本</td>
	   <td width="3%" >&nbsp;</td>
	   <td><select name="wpaAuthMode" class="list">
	   <option value="auto" id="auto2"><script language="javascript">document.write(_("auto"));</script></option>
	   <option value="wpa">WPA</option>
	   <option value="wpa2">WPA2</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="encryption">加密算法</td>
		<td >&nbsp;</td>
		<td ><select name="wpaCipher" class="list">
	   <option value="auto" selected id="auto3"><script language="javascript">document.write(_("auto"));</script></option>
	   <option value="tkip">TKIP</option>
	   <option value="aes">AES</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="Radius Service IP">Radius服务器IP</td>
		<td >&nbsp;</td>
		<td ><input name="radiusIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'IP地址'),radiusiptd)" ></td><td  id="radiusiptd"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="Radius port">Radius端口</td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPort" type="text" size="10" maxlength="5" onKeyPress=figureOnly() onChange="showfiledtip(re_checkNumber(this,1,65535,'端口'),radiusporttd)" >&nbsp;&nbsp;<script language="javascript">document.write(_("range1"));</script></td><td id="radiusporttd"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="radius password">Radius密码</td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPsswd" type="password" size="18" maxlength="11" onChange="showfiledtip(re_checkPassword1(this,11),radiuspatd)">&nbsp;&nbsp;<script language="javascript">document.write(_("range2"));</script></td><td  id="radiuspatd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="key update time">密钥更新周期</td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="keyUpdateCyc" type="text" size="10" maxlength="5" onKeyPress="figureOnly()" >&nbsp;&nbsp;<script language="javascript">document.write(_("range3"));</script></td><td  id="keyudcyctd"></td>
	   </tr>
</table>
	   </td></tr>
	    <!--///wpatbl end///-->
       <!--///wpapsktbl///-->
	   <tr id="wpapsktbl1" style="display:none"><td colspan="3">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="27%" height="25" id="WPA version2">WPA版本</td>
	   <td width="3%">&nbsp;</td>
	   <td><select name="wpapskAuthMode" class="list">
	   <option value="auto"><script language="javascript">document.write(_("auto"));</script></option>
	   <option value="wpapsk">WPA-PSK</option>
	   <option value="wpa2psk">WPA2-PSK</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="encryption2">加密算法</td>
		<td >&nbsp;</td>
		<td ><select name="wpapskCipher" class="list">
	   <option value="auto" selected><script language="javascript">document.write(_("auto"));</script></option>
	   <option value="tkip">TKIP</option>
	   <option value="aes">AES</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="pre-shared key">预共享密钥</td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskPsswd" type="password" size="18" maxlength="63" onChange="showfiledtip(re_checkPassword1(this,63),psktd)">&nbsp;&nbsp;<script language="javascript">document.write(_("range4"));</script></td><td id="psktd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="key update time2">密钥更新周期</td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskkeyUpdateCyc" type="text" size="10" maxlength="5"  onKeyPress="figureOnly()" >&nbsp;&nbsp;<script language="javascript">document.write(_("range3"));</script></td><td id="pskkudcyctd"></td>
	   </tr>
</table>
	   </td></tr><tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td   height="30" align="center" valign="top"><input type="button" class="input-button" value="保存"  onClick="save()" id="save">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="返回" onClick="history.go(-1)" id="goback"></td>
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
