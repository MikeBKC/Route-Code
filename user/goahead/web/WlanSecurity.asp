<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("WlanSecurity"); %>
<% langTran("common"); %>
<title><% write(com_utt); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

//start交互
/*AuthModes="OPEN";var keyFormats="0";var keyLength1s="";var keyLength2s="";var keyLength3s="";var keyLength4s="";var wepkey1s="";var wepkey2s="";var wepkey3s="";var wepkey4s="";var keynums="1";var wpaCiphers="NONE";var radiusIPs="";var radiusPorts="1812";var radiusPsswds="";var keyUpdateCycs="3600";var wpapskCiphers="NONE";var pskPsswds="";var pskkeyUpdateCycs="3600";var errorstr=""; */
//end交互

<%
  aspOutWlanSecurityConfig();
%>

var  secModes="";
var secmodeopt=new Array();
secmodeopt[0]=new Array("NONE","<% write(NoSafeMode); %>");
secmodeopt[1]=new Array("WEP","WEP");
//if(APWorkModes=="single_11n"){
	secmodeopt[2]=new Array("WPA","WPA/WPA2");
	secmodeopt[3]=new Array("WPAPSK","WPA-PSK/WPA2-PSK");
//}
//else secmodeopt[2]=new Array("WPAPSK","WPA-PSK/WPA2-PSK");
//get the value belong to the auth mode
var wepAuthModes="OPEN";
var wpaAuthModes="WPA1WPA2";
var wpapskAuthModes="WPAPSKWPA2PSK";
if(AuthModes=="OPEN"||AuthModes=="SHARED"||AuthModes=="WEPAUTO"){
	wepAuthModes=AuthModes;
	if(wpapskCiphers=="NONE") secModes="NONE";
	else secModes="WEP";
}
else if(AuthModes=="WEP")
{
	if(wpapskCiphers=="NONE") secModes="NONE";
	else secModes="WEP";
}
else if(AuthModes=="WPA"||AuthModes=="WPA2"||AuthModes=="WPA1WPA2"){
	secModes="WPA";
	wpaAuthModes=AuthModes;
}

else if(AuthModes=="WPAPSK"||AuthModes=="WPA2PSK"||AuthModes=="WPAPSKWPA2PSK"){
	secModes="WPAPSK";
	wpapskAuthModes=AuthModes;
}
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
function changSecMode(){
    var frm=WlessNetSecConfig;
	var index=getListValue(frm.secMode);
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
		case "NONE":		weptbl1.style.display="none";
					wpatbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display=dis;
					break;
		case "WEP":		weptbl1.style.display=dis;
					wpatbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case "WPA":		weptbl1.style.display="none";
					wpatbl1.style.display=dis;
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case "WPAPSK":		weptbl1.style.display="none";
					wpatbl1.style.display="none";
					wpapsktbl1.style.display=dis;
					wutbl.style.display="none";
		default:	break;
	}

}

function setKey()
{
	var f = WlessNetSecConfig
	//getdisable();
	//auto_disable();
	//auto_length();
	getdisable(f.keyLength1,f.keyLength2,f.keyLength3,f.keyLength4);
	auto_disable(f.wepkey1,f.wepkey2,f.wepkey3,f.wepkey4,f.keynum,f.keyLength1,f.keyLength2,f.keyLength3,f.keyLength4);
	auto_length(f.wepkey1,f.wepkey2,f.wepkey3,f.wepkey4,f.keyLength1,f.keyLength2,f.keyLength3,f.keyLength4,f.keyFormat.selectedIndex);
	/*getdisable("keyLength");
	auto_disable("wepkey","keynum","keyLength");
	auto_length("wepkey","keyLength",f.keyFormat.selectedIndex);*/
}
/*function getdisable()
{
	var f =WlessNetSecConfig;
	key1dis = (f.keyLength1.selectedIndex == 0);
	key2dis = (f.keyLength2.selectedIndex == 0);
	key3dis = (f.keyLength3.selectedIndex == 0);
	key4dis = (f.keyLength4.selectedIndex == 0);
}
function auto_disable()
{
	var f =WlessNetSecConfig;
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
	var f = WlessNetSecConfig;

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

	//if (f.keyFormat.selectedIndex == 0)
		//is_Hex = true;
	//else
		//is_Hex = false;
}*/
function init(){
	var f=WlessNetSecConfig;
	f.secMode.value=secModes;
	if(f.secMode.value=="") f.secMode.value="NONE";
	f.wepAuthMode.value=(wepAuthModes==""?"WEPAUTO":wepAuthModes);
	f.keyFormat.value=(keyFormats==""?"1":keyFormats);
	f.wepkey1.value=wepkey1s;
	f.keyLength1.value=(keyLength1s==""?"0":keyLength1s);
	f.wepkey2.value=wepkey2s;
	f.keyLength2.value=(keyLength2s==""?"0":keyLength2s);
	f.wepkey3.value=wepkey3s;
	f.keyLength3.value=(keyLength3s==""?"0":keyLength3s);
	f.wepkey4.value=wepkey4s;
	f.keyLength4.value=(keyLength4s==""?"0":keyLength4s);
	if(keynums == "0")keynums = 0;
	if(radiusIPs == "0.0.0.0")radiusIPs = "";
	keynums=(keynums==""?1:keynums);
	f.keynum[keynums-1].checked=true;

	f.wpaAuthMode.value=(wpaAuthModes==""?"auto":wpaAuthModes);
	//f.wpaCipher.value=(wpaCiphers=="NONE"?"2":wpaCiphers);
	selTxt(f.wpaCipher,wpaCiphers);
	f.radiusIP.value=(radiusIPs==""?"":radiusIPs);
	if(radiusPorts == ""||radiusPorts == "0" )radiusPorts = "1812";
	f.radiusPort.value= radiusPorts ;
	f.radiusPsswd.value=radiusPsswds;
	f.keyUpdateCyc.value=(keyUpdateCycs==""?"0":keyUpdateCycs);
	f.wpapskAuthMode.value=(wpapskAuthModes==""?"auto":wpapskAuthModes);
	//f.wpapskCipher.value=(wpapskCiphers=="NONE"?"2":wpapskCiphers);
	selTxt(f.wpapskCipher,wpapskCiphers);
	f.pskPsswd.value=pskPsswds;
	f.pskkeyUpdateCyc.value=(pskkeyUpdateCycs==""?"0":pskkeyUpdateCycs);
	changSecMode();
	setKey();
	pskkudcyctd.innerHTML="";keyudcyctd.innerHTML="";
	radiuspatd.innerHTML="";radiuspatd.innerHTML="";
	radiusporttd.innerHTML="";radiusiptd.innerHTML="";
	 for (var i=1; i<5; i++){
	 	document.all["wepkeytd"+i].innerHTML="";
	 }
}
function save(){
	var f=WlessNetSecConfig;
	if(checkData()){
		var secModeVal=getListValue(f.secMode);
		if(secModeVal=="NONE"){
			f.security_mode.value="Disable";
		}
		else if(secModeVal=="WEP"){
			var wepAuthMode=getListValue(f.wepAuthMode);
			 f.security_mode.value=wepAuthMode;
		}
		else if(secModeVal=="WPA"){
			var wpaAuthMode=getListValue(f.wpaAuthMode);
			if(wpaAuthMode=="WPA1WPA2") f.security_mode.value="WPA1WPA2";
			else if(wpaAuthMode=="WPA")f.security_mode.value="WPA";
			else f.security_mode.value="WPA2";
			f.cipher.value=f.wpaCipher.value;
			f.keyRenewalInterval.value=f.keyUpdateCyc.value;
		}
		else if(secModeVal=="WPAPSK"){
			var wpapskAuthMode=getListValue(f.wpapskAuthMode);
			 f.security_mode.value=wpapskAuthMode;
			f.cipher.value=f.wpapskCipher.value;
			f.keyRenewalInterval.value=f.pskkeyUpdateCyc.value;
		}
		//alert(f.security_mode.value);
		WlessNetSecConfig.action="/goform/APSecurity";
		WlessNetSecConfig.submit();
	}
}
function checkData(){
	var f=WlessNetSecConfig;
	var selIndex=getListValue(f.secMode);
	if(selIndex=="WEP"){
		if(!checkWEP(f.wepkey1,f.wepkey2,f.wepkey3,f.wepkey4,f.keyFormat.selectedIndex)) return false;
	}
	else if(selIndex=="WPA"){
		if(!checkWPA()) return false;
	}
	else if(selIndex=="WPAPSK"){
		if(!checkWPAPSK()) return false;
	}
	return true;
}/*
function checkWEP()
{
	var f = WlessNetSecConfig;var str="该密钥类型下的";
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
}*/
function checkWPA(){
	var f = WlessNetSecConfig;
	if(!checkIP(f.radiusIP,"<% write(RadiusSerIP); %>",0)) return false;
	if(!checkNumber(f.radiusPort,1,65535,"<% write(RadiusSerPort); %>",0)) return false;
	if(!checkPassword1(f.radiusPsswd,"31",0)) return false;
	if(f.keyUpdateCyc.value!="0"){
		if(!checkNumber(f.keyUpdateCyc,60,86400,"<% write(PWUpdateC); %>",0)) return false;
	}
	return true;
}
function checkWPAPSK(){
	var f = WlessNetSecConfig;
	var len=f.pskPsswd.value.length;
	if(len<8){alert("<% write(PSKPWLen); %>");return false;}
	if(!checkPassword1(f.pskPsswd,"63",0)) return false;
	if(f.pskkeyUpdateCyc.value!="0"){
		if(!checkNumber(f.pskkeyUpdateCyc,60,86400,"<% write(PWUpdateC); %>",0)) return false;
	}
	return true;
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
  <td><script language="javascript">showSecondTitle("<% write(WirelessSafe); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action=""  name="WlessNetSecConfig" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
	   <tr>
	   <td align="right"  width="27%" height="25" id="safe mode"><% write(SecurityRe); %></td>
	   <td  width="2%">&nbsp;&nbsp;&nbsp;&nbsp;</td>
	   <td><select name="secMode" class="list" onChange="changSecMode()">
	   <script language="javascript">
	   		for(i=0;i<secmodeopt.length;i++){
			 	document.write('<option value="'+secmodeopt[i][0]+'">'+secmodeopt[i][1]+'</option>');
			}
	   </script>
	   </select></td>
	   </tr><input type="hidden" name="security_mode">
	   <tr id="wutbl" style=" display:none"><td colspan="4"></td></tr>
	   <!--///weptbl///-->
	   <tr id="weptbl1" style=" display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right"  width="27%" height="25"  id="certification mode"><% write(ApproveType); %></td>
	   <td  width="3%">&nbsp;</td>
	   <td><select name="wepAuthMode" class="list">
	   <option value="WEPAUTO"><script language="javascript">document.write("<% write(Auto); %>");</script></option>
	   <option value="OPEN"><script language="javascript">document.write("<% write(Opensys); %>");</script></option>
	   <option value="SHARED"><script language="javascript">document.write("<% write(ShareKey); %>");</script></option>
	   </select></td>
	   </tr>
	     <tr><td colspan="4"><table width="100%"><tr>
	  <td width="27%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription5"><% write(ShowMsg); %></td>	
	  </tr></table></td></tr>
	   <tr>
	   	<td height="25" align="right" id="key format"><% write(PWLayout); %></td>
		<td >&nbsp;</td>
		<td ><select name="keyFormat" class="list" onChange="setKey()">
	   <option value="0" selected><script language="javascript">document.write("<% write(Hex16); %>");</script></option>
	   <option value="1"><script language="javascript">document.write("<% write(ASCIICode); %>");</script></option>
	   </select></td>
	   </tr>
	   <tr>
	   <td width="27%" height="25" align="right" id="key choose"><% write(PWSelect); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr>
	   <td width="55%" id="wep key"><% write(PWWEP); %></td>
	   <td id="key mode"><% write(PWType); %></td>
	   </tr></table></td>
	   </tr>
	   <script language="javascript">
	   for (var i=1; i<5; i++)
        		{   
        			j = i-1;
        			document.write('<TR><TD  width="27%" height="25" align="right"> '+"<% write(Key); %>"+i+'：<input type=radio name=keynum value='+i+'></td><td width="2%">&nbsp;</td>');
        			document.write('<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="55%"><input type=text class=text name=wepkey'+i+' maxLength=32 size="30" onfocus="if(key'+i+'dis) this.blur();else this.form.keynum['+j+'].checked=true;" onChange="showfiledtip(re_checkEnglishName(this, \'输入\'),wepkeytd'+i+')"></td>' );
        			document.write('<TD ><select name=keyLength'+i+' class=list id=keyLength'+i+' onchange="setKey();">');
        			document.write('<option value=0 >'+"<% write(Unable); %>"+'</option>');
        			document.write('<option value=5 >'+"<% write(Bit64); %>"+'</option>');
        			document.write('<option value=13>'+"<% write(Bit128); %>"+'</option>');
        			//document.write('<option value=16>'+_("152 bit")+'</option>');
        			document.write('</select></td><td id=wepkeytd'+i+'></td></tr></table></td></tr>');
					/*
					document.write('<TR><TD  width="27%" height="25" align="right">密钥 '+i+'：<input type=radio name=keynum value='+i+'></td><td width="2%">&nbsp;</td>');
        			document.write('<TD ><input type=text class=text name=wepkey'+i+' maxLength=32 size="30" onfocus="if(key'+i+'dis) this.blur();else this.form.keynum['+j+'].checked=true;" onChange="showfiledtip(re_checkEnglishName(this),wepkeytd'+i+')"></td>' );
        			document.write('<TD ><select name=keyLength'+i+' class=list id=keyLength'+i+' onchange="setKey();">');
        			document.write('<option value=0 >禁用</option>');
        			document.write('<option value=5 >64位</option>');
        			document.write('<option value=13>128位</option>');
        			document.write('<option value=16>152位</option>');
        			document.write('</select></td><td id=wepkeytd'+i+'></td></tr>');*/
        		}
	   </script>
</table>
	   </td></tr>
	    <!--///weptbl end///-->
		 <!--///wpatbl///-->
	   <tr id="wpatbl1" style="display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="27%" height="25" id="WPA version"><% write(WPAVer); %></td>
	   <td width="3%" >&nbsp;</td>
	   <td><select name="wpaAuthMode" class="list">
	   <option value="WPA1WPA2"><script language="javascript">document.write("<% write(Auto); %>");</script></option>
	   <option value="WPA">WPA</option>
	   <option value="WPA2">WPA2</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="encryption"><% write(EncryptALG); %></td>
		<td >&nbsp;</td>
		<td ><select name="wpaCipher" class="list">
	   <option value="TKIPAES" selected><script language="javascript">document.write("<% write(Auto); %>");</script></option>
	   <option value="TKIP">TKIP</option>
	   <option value="AES">AES</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="Radius Service IP"><% write(RadiusSerIP); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusIP" type="text" size="18" maxlength="15" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<% write(IPAddr); %>'),radiusiptd)" ></td><td  id="radiusiptd"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="Radius port"><% write(RadiusPort); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPort" type="text" onChange="showfiledtip(re_checkNumber(this,1,65535,'<% write(Port); %>'),radiusporttd)" onKeyPress=figureOnly()   size="10" maxlength="5" >&nbsp;&nbsp;
		    <script language="javascript">document.write("<% write(range1); %>");</script></td><td id="radiusporttd"></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="radius password"><% write(RadiusPW); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="radiusPsswd" type="password" size="18" maxlength="31" onChange="showfiledtip(re_checkPassword1(this,31),radiuspatd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range2); %>");</script></td><td  id="radiuspatd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="key update time"><% write(PWUpdateC); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="keyUpdateCyc" type="text" size="10" maxlength="5" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber_allowZero(this,60,86400,'<% write(PWUpdateC); %>'),keyudcyctd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range3); %>");</script></td><td  id="keyudcyctd"></td>
	   </tr>
</table>
	   </td></tr>
	    <!--///wpatbl end///-->
       <!--///wpapsktbl///-->
	   <tr id="wpapsktbl1" style="display:none"><td colspan="3">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="27%" height="25" id="WPA version2"><% write(WPAVer); %></td>
	   <td width="3%">&nbsp;</td>
	   <td><select name="wpapskAuthMode" class="list">
	   <option value="WPAPSKWPA2PSK"><script language="javascript">document.write("<% write(Auto); %>");</script></option>
	   <option value="WPAPSK">WPA-PSK</option>
	   <option value="WPA2PSK">WPA2-PSK</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="encryption2"><% write(EncryptALG); %></td>
		<td >&nbsp;</td>
		<td ><select name="wpapskCipher" class="list">
	   <option value="TKIPAES" selected><script language="javascript">document.write("<% write(Auto); %>");</script></option>
	   <option value="TKIP">TKIP</option>
	   <option value="AES">AES</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="pre-shared key"><% write(WSharePW); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskPsswd" type="password" size="18" maxlength="63" onChange="showfiledtip(re_checkPassword1(this,63),psktd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range4); %>");</script></td><td id="psktd"></td>
	   </tr>
	    <tr>
	   	<td height="25" align="right" id="key update time2"><% write(PWUpdateC); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="pskkeyUpdateCyc" type="text" size="10" maxlength="5"  onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber_allowZero(this,60,86400,'<% write(PWUpdateC); %>'),pskkudcyctd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range3); %>");</script></td><td id="pskkudcyctd"></td>
	   </tr>
</table>
	   </td></tr><tr><td colspan="3" height="20">&nbsp;</td></tr>
	    <tr><td colspan="4">
  <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td  height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<% write(com_reset); %>"  onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#WlanSecurity')"></td>
  </tr>
</table>
  </td></tr>
	    <!--///wpapsktbl end/// -->
  </table> </td>
  </tr>
 <input  type="hidden" name="cipher"><input type="hidden" name="keyRenewalInterval">
 <script language="javascript">init();</script>
</table>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
