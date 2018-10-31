<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<% langTran("WlanBase"); %>
<% langTran("common"); %>
<% langTran("WirelessSui"); %>
<% langTran("_WirelessSui"); %>
<title><% write(WTitle); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">


 
var OpModes="<%
//getCfgZero(1, "OperationMode"); 
%>";
var wlanCurMac="<% getWlanCurrentMac_5g(); %>"

<%

		 var aplist = Form("AP_LIST");
		 var WrlessEnables=Form("WrlessEnable");
          var ssids=Form("ssid");
		var APWorkModes = Form("APWorkMode");
          var WDSEnableds=Form("wdsEnabled");
          var wrlessModes=Form("wrlessMode");
          var channels=Form("channel");
          var chanWidths=Form("chanWidth");
          var vhtChanWidths=Form("vhtChanWidth");
          var broadcastEnables=Form("broadcastEnable");
		  var APssids = Form("APssid");
		  var APCliMac = Form("APCliMacs");
		  var APMacs0 = Form("APMac0");
		  var APMacs1 = Form("APMac1");
		  var APMacs2 = Form("APMac2");
		  var APMacs3 = Form("APMac3");
		  var wdsSecModes = Form("apclisecmode");

		  var apcliSecModes = "NONE";

		  var apcliAuthModes = "";
		  var apclipskPsswds = "";
		  var apclikeyFormats = "0";
		  var apclikeynums = "0";
		  var apcliwepkey1s = "";
		  var apcliwepkey2s = "";
		  var apcliwepkey3s = "";
		  var apcliwepkey4s = "";
		  var apclikeyFormats = "0";
		  var apclikeyLength1s = "0";
		  var apclikeyLength2s = "0";
		  var apclikeyLength3s = "0";
		  var apclikeyLength4s = "0";
		  var wdspskPsswds = Form("wdspskPsswd");
		  var wdskeynums = Form("wepkeynum");
		  var wdswepkey1s = Form("apcliwepkey1");
		  var wdswepkey2s = Form("apcliwepkey2");
		  var wdswepkey3s = Form("apcliwepkey3");
		  var wdswepkey4s = Form("apcliwepkey4");
		  var wdskeyFormats = Form("apclikeyFormat");
		  var wdskeyLength1s = Form("apclikeyLength1");
		  var wdskeyLength2s = Form("apclikeyLength2");
		  var wdskeyLength3s = Form("apclikeyLength3");
		  var wdskeyLength4s = Form("apclikeyLength4");

          var bridgeSSIDs=Form("bridgessid");
          var briggeBSSIDs=Form("bridgebssid");
		  var keytypes=Form("keytype");
		  var wepkeynums=Form("wepkeynum");
		  var certificatetypes=Form("certificatetype");
		  var keys=Form("key");
     	if (aplist != ""){
			 write('\nvar str = "aplist";\n');
		}
		else 
		{
			 write('\nvar str = "";\n');
		}
%>

if(str==""){

<%
	 outputWlanBaseList_5g();
%>
//信道选项
//var WrlessEnables="on";
//var wrlessModes="8";
//var channels="64";
//var chanWidths="1";
//var vhtChanWidths="0";
//var broadcastEnables="1";
//var WDSEnableds="0";
//var briggeBSSIDs="A8-15-4D-FE-BC-94";
//var keytypes="0";
//var keynums="0";
//var certifications ="0";
//var keyinfos="";
//var errorstr="";
}else{
var APWorkModes = "<% write(APWorkModes);%>";
var WrlessEnables="<% write(WrlessEnables);%>";
var ssids="<% write(ssids);%>";
var wrlessModes="<% write(wrlessModes);%>";
var channels="<% write(channels);%>";
var chanWidths="<% write(chanWidths);%>";
var vhtChanWidths="<% write(vhtChanWidths);%>";
var broadcastEnables="<% write(broadcastEnables);%>";
var WDSEnableds="<% write(WDSEnableds);%>";
var wdsSecModes ="<% write(wdsSecModes);%>"; 
var bridgeSSIDs="<% write(bridgeSSIDs);%>";
var briggeBSSIDs="<% write(briggeBSSIDs);%>"; 
var APMacs0 = briggeBSSIDs;
var APMacs1 = "000000000000";
var APMacs2 = "000000000000";
var APMacs3 = "000000000000";
var wdspskPsswds = "<% write(wdspskPsswds);%>";
var wdswepkey1s = "<% write(wdswepkey1s);%>";
var wdswepkey2s = "<% write(wdswepkey2s);%>";
var wdswepkey3s = "<% write(wdswepkey3s);%>";
var wdswepkey4s = "<% write(wdswepkey4s);%>";
var wdskeyFormats = "<% write(wdskeyFormats);%>";
var wdskeyLength1s = "<% write(wdskeyLength1s);%>";
var wdskeyLength2s = "<% write(wdskeyLength2s);%>";
var wdskeyLength3s = "<% write(wdskeyLength3s);%>";
var wdskeyLength4s = "<% write(wdskeyLength4s);%>";
var wdskeynums = "<% write(wdskeynums);%>";
var apclikeynums = "1";
//var  keytypes=  "<% write(keytypes);%>";
//var  keynums=  "<% write(wepkeynums);%>";
//var certifications ="<% write(certificatetypes);%>";
//var keyinfos="<% write(keys);%>";
var errorstr="";
}
//start 交互
/*var WrlessEnables="on";var APWorkModes="1";var ssids="-HIPER_a89e08";var channels="6";var broadcastEnables="0";var wrlessModes="9";var APssids="HIPER";var APCliMac="00:22:aa:11:22:33";var APMacs="";var chanWidths="1";
var apcliSecModes="";var apcliAuthModes="";var apclipskPsswds="";var apclikeynums="";var apcliwepkey1s="";var apcliwepkey2s="";var apcliwepkey3s="";var apcliwepkey4s="";var apclikeyFormats="";
var apclikeyLength1s="";var apclikeyLength2s="";var apclikeyLength3s="";var apclikeyLength4s="";var wdsSecModes="NONE";var wdspskPsswds="";var wdskeynums="1";var wdswepkey1s="";var wdswepkey2s="";var wdswepkey3s="";var wdswepkey4s="";var wdskeyFormats="0";
var wdskeyLength1s="";var wdskeyLength2s="";var wdskeyLength3s="";var wdskeyLength4s="";var errorstr="";
var OpModes="1";
var wlanCurMac="00:22:aa:11:22:33";*/
//end交互
var apmodeopt=new Array();//AP模式选项
//var WDSEnableds="1";
var i=0;
//var APCliMac = "00:22:aa:00:22:aa";
apmodeopt[i++]=new Array("1","AP Mode");
/*if(OpModes=="3") */
if (typeof(remove_apclient)=='undefined')
	var remove_apclient = 0;
if (remove_apclient == 0)
	apmodeopt[i++]=new Array("5","APClient Mode");
apmodeopt[i++]=new Array("3","Repeater Mode");
apmodeopt[i++]=new Array("2","Bridge Mode");
apmodeopt[i++]=new Array("4","Lazy Mode");

//var channelopt=new Array(36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165);//信道选项
var channelopt=new Array(36,40,44,48,149, 153, 157, 161, 165);//信道选项
var web_secModes="NONE";
var web_AuthModes="OPEN";
var web_wpapskAuthModes="WPAPSK";
if(apcliAuthModes=="NONE") web_secModes=apcliAuthModes;
if((apcliSecModes=="OPEN"||apcliSecModes=="SHARED")&&apcliAuthModes!="NONE"){
	web_secModes="WEP";
	web_AuthModes=apcliSecModes;
}
else if(apcliSecModes=="WPAPSK"||apcliSecModes=="WPA2PSK"){
	web_secModes="WPAPSK";
	web_wpapskAuthModes=apcliSecModes;
}

function save(){
	if(checkData()){
		var f=WlanBase_Config;
		var chan_value=f.chanWidth.value;
		var vht_chan_value=f.vhtChanWidth.value;
		f.broadcastEnable.value=(f.broadcastEnablew.checked==true?"1":"0");
		var selval=getListValue(f.APWorkMode);
		var secModeVal=getListValue(f.apclisecmode);
		if(selval=="5"){
			if(secModeVal=="NONE"){
				f.apcli_mode.value="OPEN";
				f.apcli_enc.value="NONE";
			}
			else if(secModeVal=="WEP"){
				var wepAuthMode=getListValue(f.apcliwepAuthMode);
				f.apcli_mode.value=wepAuthMode;
				f.apcli_enc.value="WEP";
			}
			else if(secModeVal=="WPAPSK"){
				var wpapskAuthMode=getListValue(f.apcliwpapskAuthMode);
				var wpapskCipher=getListValue(f.apcliwpapskCipher);
				f.apcli_mode.value=wpapskAuthMode;//alert(wpapskAuthMode);
				f.apcli_enc.value=wpapskCipher;
			}
			f.wds_list.value=WebMacTOLinuxMac(f.APMac0.value);
		}
		else if(selval=="2"||selval=="3"||selval=="4"){
		    if(selval=="2"||selval=="3"){
				var wds_list_all="";
				for(j=0;j<=3;j++){
					if(eval("f.APMac"+j).value!=""){
						wds_list_all += WebMacTOLinuxMac(eval("f.APMac"+j).value);
						wds_list_all += ';';
					}
				}
				f.wds_list.value=wds_list_all;
			}
			f.apcli_mode.value=secModeVal;
			
		}

		if (wlanworktimeEn == 1)
		{
		    var len = f.date.length;
		    for (i = 0; i < len; i++) 
		    {
			f.day.value += (f.date[i].checked == true ? "1": "0");
		    }    
		    f.weekdaytimestart.value = f.weekdayfromhour.value + ":" +f.weekdayfromminute.value;
		    f.weekdaytimestop.value = f.weekdayendhour.value + ":" + f.weekdayendminute.value;
		    f.weekendtimestart.value = f.weekendfromhour.value + ":" + f.weekendfromminute.value;
		    f.weekendtimestop.value = f.weekendendhour.value + ":" + f.weekendendminute.value;
		}

		WlanBase_Config.actiong="/goform/ConfigWirelessBase_5g";
		WlanBase_Config.submit();
	}	
}

function init(){
	var f=WlanBase_Config;
	f.WrlessEnable.checked=(WrlessEnables.toLowerCase()=="on"?true:false);
//	f.APWorkMode.value=APWorkModes;
//	selAPWorkMode(f.APWorkMode.value,1);
	f.wdsEnabled.checked=(WDSEnableds.toLowerCase()=="1"?true:false);
	changeWDS(f.wdsEnabled.checked);
	f.bridgessid.value=bridgeSSIDs;
	f.ssid.value=ssids;
if(multi_ssid ==1){
    document.getElementById("encode1").style.display="";
    document.getElementById("encode2").style.display="";
    f.ssid2.value=ssid2s;
    f.encodeType.value=encodeType;
    f.encodeType2.value=encodeType2;
}else{
    document.getElementById("encode1").style.display="none";
    document.getElementById("encode2").style.display="none";
}
	selVal(f.wrlessMode,wrlessModes);
	f.chanWidth.value=chanWidths;
	f.vhtChanWidth.value=vhtChanWidths;
	f.channel.value=channels;
	f.broadcastEnablew.checked=(broadcastEnables.toLowerCase()=="1"?true:false);
	if (f.wrlessMode.value == "2") {
		document.getElementById("chw").style.display = "none";	
		document.getElementById("VHTchw").style.display = "none";	
	} else if (f.wrlessMode.value == "8") {
		document.getElementById("chw").style.display = "";	
		document.getElementById("VHTchw").style.display = "none";	
	} else {
		document.getElementById("chw").style.display = "";
		document.getElementById("VHTchw").style.display = "";
	}
	if(f.chanWidth.value =="0"){
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=true;
	}

	if(APssids == "")
		APssids = "HIPER_5G";
	f.APssid.value=APssids;
	/*
	if (APMacs != "") {
		APMacsArray = APMacs.split(";");
		for (i = 0; i <APMacsArray.length; i++){
			if(APMacsArray[i]!="")
				eval("f.APMac"+i).value = APMacsArray[i];
		}
	}*/
	if(briggeBSSIDs =="000000000000") 
	briggeBSSIDs = "";
	if(APMacs0 =="000000000000") 
	APMacs0 = "";
	if(APMacs1 =="000000000000") 
	APMacs1 = "";
	if(APMacs2 =="000000000000") 
	APMacs2 = "";
	if(APMacs3 =="000000000000") 
	APMacs3 = "";
	if(APCliMac =="000000000000") 
	APCliMac = "0022aa112233";
	
	f.bridgebssid.value=briggeBSSIDs;
	f.APCliMacs.value = APCliMac;
	f.APMac0.value = APMacs0;
	f.APMac1.value = APMacs1;
	f.APMac2.value = APMacs2;
	f.APMac3.value = APMacs3;
	
	//APCLISEC
	//alert(web_secModes);
	if(APWorkModes=="5"){
		f.apclisecmode.value=web_secModes;
		if(f.apclisecmode.value=="") f.apclisecmode.value="NONE";
		selVal(f.apcliwepAuthMode,web_AuthModes);
		f.apclikeyFormat.value=(apclikeyFormats==""?"1":apclikeyFormats);
		f.apcliwepkey1.value=apcliwepkey1s;
		f.apclikeyLength1.value=(apclikeyLength1s==""?"0":apclikeyLength1s);
		f.apcliwepkey2.value=apcliwepkey2s;
		f.apclikeyLength2.value=(apclikeyLength2s==""?"0":apclikeyLength2s);
		f.apcliwepkey3.value=apcliwepkey3s;
		f.apclikeyLength3.value=(apclikeyLength3s==""?"0":apclikeyLength3s);
		f.apcliwepkey4.value=apcliwepkey4s;
		f.apclikeyLength4.value=(apclikeyLength4s==""?"0":apclikeyLength4s);
		if(apclikeynums == "" || apclikeynums == "0")apclikeynums = 1;
		f.wepkeynum.value = apclikeynums;
		f.apclikeynum[apclikeynums-1].checked=true;
	
		f.apcliwpapskAuthMode.value=web_wpapskAuthModes;
		selVal(f.apcliwpapskCipher,apcliAuthModes);
		f.APCliMacs.value = APCliMac;
		if(f.apcliwpapskCipher.value=="") f.apcliwpapskCipher.selectedIndex=0;
		f.apclipskPsswd.value=apclipskPsswds;
	}
	else if(APWorkModes=="2"||APWorkModes=="3"||APWorkModes=="4"){
		f.apclisecmode.value=wdsSecModes;
		if(f.apclisecmode.value=="") f.apclisecmode.value="NONE";
		if(wdsSecModes=="WEP"){
			f.apclikeyFormat.value=(wdskeyFormats==""?"1":wdskeyFormats);
			f.apcliwepkey1.value=wdswepkey1s;
			f.apclikeyLength1.value=(wdskeyLength1s==""?"0":wdskeyLength1s);
			f.apcliwepkey2.value=wdswepkey2s;
			f.apclikeyLength2.value=(wdskeyLength2s==""?"0":wdskeyLength2s);
			f.apcliwepkey3.value=wdswepkey3s;
			f.apclikeyLength3.value=(wdskeyLength3s==""?"0":wdskeyLength3s);
			f.apcliwepkey4.value=wdswepkey4s;
			f.apclikeyLength4.value=(wdskeyLength4s==""?"0":wdskeyLength4s);
			apclikeynums=(wdskeynums==""?1:wdskeynums);
			f.wepkeynum.value = apclikeynums;
			f.apclikeynum[wdskeynums-1].checked=true;
		}
		else{
			f.wdspskPsswd.value=wdspskPsswds;
		}
	}
	changSecMode();
	setKey();
	//END APCLISEC
	ssidtd1.innerHTML="";
if(multi_ssid ==1){
	ssidtd2.innerHTML="";
}
//	apmactd.innerHTML="";
	APClientmac.innerHTML="";
	ap1mactd.innerHTML="";
	ap2mactd.innerHTML="";
	ap3mactd.innerHTML="";
	psktd.innerHTML="";
	wdspsktd.innerHTML="";
	apssidtd.innerHTML="";
	for (var i=1; i<5; i++){
	    document.all["wepkeytd"+i].innerHTML="";
	}
	if (wlanworktimeEn == 1)
	{
	    weekday.style.display="";
	    weekdaytime.style.display="";
	    weekend.style.display="";
	    weekendtime.style.display="";
	    for (i = 0; i < days.length; i++) {
		if (days.charAt(i) == "1") f.date[i].checked = true;
	    }    
	    var index1 = weekdaytimestarts.indexOf(":");
	    var index2 = weekdaytimestops.indexOf(":");
	    f.weekdayfromhour.value = weekdaytimestarts.substring(0, index1);
	    f.weekdayfromminute.value = weekdaytimestarts.substring(index1 + 1);
	    f.weekdayendhour.value = weekdaytimestops.substring(0, index2);
	    f.weekdayendminute.value = weekdaytimestops.substring(index2 + 1);

	    index1 = weekendtimestarts.indexOf(":");
	    index2 = weekendtimestops.indexOf(":");
	    f.weekendfromhour.value = weekendtimestarts.substring(0, index1);
	    f.weekendfromminute.value = weekendtimestarts.substring(index1 + 1);
	    f.weekendendhour.value = weekendtimestops.substring(0, index2);
	    f.weekendendminute.value = weekendtimestops.substring(index2 + 1);
	}
}

function checkData(){
	var f=WlanBase_Config;
if(multi_ssid == 1){
	if(!checkName(f.ssid,32,"<%write(wire_name1);%>",0)) return false;
    /*if (f.ssid.value.length > 22) {
        if (confirm("<%write(wire_warn1);%>") == false){
            return false;
        }
    }*/
    if(!checkName(f.ssid2,32,"<%write(wire_name2);%>",1)) return false;
    /*if (f.ssid2.value.length > 22) {
        if (confirm("<%write(wire_warn2);%>") == false){
            return false;
        }
    }*/
}else{
    if(!checkName(f.ssid,32,"ssid",0)) return false;
}
	f.APMac0.value = f.bridgebssid.value;
    var selval=getListValue(f.APWorkMode);
	if(selval=="5"||selval=="2"||selval=="3"){
		
		if(selval=="5"){
		    if(!checkName(f.APssid,32,"<% write(APsSSID); %>",0)) return false;
			if(!checkMac(f.APCliMacs,"<% write(APsMAC); %>",0)) return false;
		}
		if(selval=="2"||selval=="3"){	
			if(!checkMac(f.APMac1,"<% write(APsMAC); %>",1)) return false;
			if(!checkMac(f.APMac2,"<% write(APsMAC); %>",1)) return false;
			if(!checkMac(f.APMac3,"<% write(APsMAC); %>",1)) return false;
			if(!checkMac(f.APMac0,"<% write(APsMAC); %>",0)) return false;
		}
	}
	if(selval=="4"||selval=="5"||selval=="2"||selval=="3"){
		var selIndex=getListValue(f.apclisecmode);
		if(selIndex=="WEP"){
			if(!checkWEP(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeyFormat.selectedIndex)) return false;
		}
		else if(selIndex=="WPAPSK"&&selval=="5"){
			var len=f.apclipskPsswd.value.length;
			if(len<8){alert("<% write(AlertPWLen); %>");return false;}
			if(!checkPassword1(f.apclipskPsswd,"63",0)) return false;	
		}
		else if(selIndex!="WEP"&&selIndex!="NONE"&&selval!="5"){
			var len=f.wdspskPsswd.value.length;
			if(len<8){alert("<% write(AlertPWLen); %>");return false;}
			if(!checkPassword1(f.wdspskPsswd,"63",0)) return false;	
		}
	}
	
    return true;
}
/*
function checkWEP()
{
	var f = WlanBase_Config;
	var str="该密钥类型下的";
	if(key1dis&&key2dis&&key3dis&&key4dis){
		alert("请至少配置一个密钥！");return false;
	}
	if(!key1dis){
		if(f.apcliwepkey1.value.length!=keylength1){alert(str+"密钥1必须为"+keylength1+"位"); return  false;}
		else{
			if(f.apclikeyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.apcliwepkey1,keylength1,"密钥1",0))	return false;
			}
			else{
				if (!checkHex(f.apcliwepkey1,"密钥1",0))	return false;
			}
		}
	}
	if(!key2dis){
		if(f.apcliwepkey2.value.length!=keylength2){alert(str+"密钥2必须为"+keylength2+"位"); return  false;}
		else{
			if(f.apclikeyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.apcliwepkey2,keylength2,"密钥2",0))	return false;
			}
			else{
				if (!checkHex(f.apcliwepkey2,"密钥2",0))	return false;
			}
		}	
	}
	if(!key3dis){
		if(f.apcliwepkey3.value.length!=keylength3){alert(str+"密钥3必须为"+keylength3+"位"); return  false;}
		else{
			if(f.apclikeyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.apcliwepkey3,keylength3,"密钥3",0))	return false;
			}
			else{
				if (!checkHex(f.apcliwepkey3,"密钥3",0))	return false;
			}
		}	
	}
	if(!key4dis){
		if(f.apcliwepkey4.value.length!=keylength4){alert(str+"密钥4必须为"+keylength4+"位"); return  false;}
		else{
			if(f.apclikeyFormat.selectedIndex == 1){
				if (!checkEnglishName(f.apcliwepkey4,keylength4,"密钥4",0))	return false;
			}
			else{
				if (!checkHex(f.apcliwepkey4,"密钥4",0))	return false;
			}
		}
	}
	return true;
}
*/
function selAPWorkMode(val,flag){
	var f=WlanBase_Config;
	ssidentbl.style.display="none";
	//apenabletbl.style.display="none";
//	mactbl.style.display="none";
	APClientmactbl.style.display="none";
	opssidtbl.style.display="none";
	apclisec.style.display="none";
//	mulmactbl.style.display="none";
//	mulmactbl1.style.display="none";
//	mulmactbl2.style.display="none";
	wepauthmode.style.display="none";
	var mysel = f.apclisecmode;
	mysel.options.length = 0;
	var dis="";
	if(document.all)   
    	dis="block";   
 	else   
     	dis="table-row";
	switch(val){
		case "1":   ssidentbl.style.display=dis;
//	                    mactbl.style.display="none";
						APClientmactbl.style.display="none";
	                    opssidtbl.style.display="none";
	                    apclisec.style.display="none";
//	                    mulmactbl.style.display="none";
//	                    mulmactbl1.style.display="none";
//	                    mulmactbl2.style.display="none";
	                    wepauthmode.style.display="none";
	                    break;
		case "5":  ssidentbl.style.display=dis;
				   APClientmactbl.style.display=dis;
//		           mactbl.style.display="none";
		           opssidtbl.style.display=dis;
		           apclisec.style.display=dis;
//		           mulmactbl.style.display="none";
//		           mulmactbl1.style.display="none";
//		           mulmactbl2.style.display="none";
		           wepauthmode.style.display=dis;
		           mysel.options[0] = new Option("<% write(NoSafeMode); %>","NONE");
		           mysel.options[1] = new Option("WEP", "WEP");
		           mysel.options[2] = new Option("WPA-PSK/WPA2-PSK", "WPAPSK");
		           break;
		case "3": ssidentbl.style.display=dis;
						APClientmactbl.style.display="none";
//						 mactbl.style.display=dis;
						 opssidtbl.style.display="none";
						 apclisec.style.display=dis;
						 wepauthmode.style.display="none";
//						 mulmactbl.style.display=dis;
//						 mulmactbl1.style.display=dis;
//						 mulmactbl2.style.display=dis;
						//if(!flag)
							//alert("此模式需要对端的主AP工作在Lazy Mode下！");
						 break;
		case "2":  ssidentbl.style.display="none";
					   	 APClientmactbl.style.display="none";
//						 mactbl.style.display=dis;
						 opssidtbl.style.display="none";
						 apclisec.style.display=dis;
						 wepauthmode.style.display="none";	
//						 mulmactbl.style.display=dis;
//						 mulmactbl1.style.display=dis;
//						 mulmactbl2.style.display=dis;
						 break;
		case "4":   ssidentbl.style.display=dis;
				APClientmactbl.style.display="none";
//			    mactbl.style.display="none";
			    opssidtbl.style.display="none";
			    apclisec.style.display=dis;
			    wepauthmode.style.display="none";
//			    mulmactbl.style.display="none";
//			    mulmactbl1.style.display="none";
//			    mulmactbl2.style.display="none";
			    break;
		default: break;
	}
	if(val=="2"||val=="3"||val=="4"){
		mysel.options[0] = new Option("<% write(NoSafeMode); %>","NONE");
		mysel.options[1] = new Option("WEP", "WEP");
		mysel.options[2] = new Option("TKIP", "TKIP");
		mysel.options[3] = new Option("AES", "AES");
	}
	changSecMode();
}
function selwrlessMode(val){
	var f=WlanBase_Config;
	if(val=="2"){
		document.getElementById("chw").style.display = "none";
		document.getElementById("VHTchw").style.display = "none";
		f.chanWidth.value="0";
		f.vhtChanWidth.value="0";
	} else if (val == "8") {
		document.getElementById("chw").style.display = "";
		document.getElementById("VHTchw").style.display = "none";
		f.chanWidth.value="1";
		f.vhtChanWidth.value="0";
//		f.chanWidth.disabled=false;
	} else {
		document.getElementById("chw").style.display = "";
		document.getElementById("VHTchw").style.display = "";
		f.chanWidth.value="1";
		f.chanWidth.disabled=false;
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=false;
	}	
}

function scanlocation(){
      var f=WlanBase_Config;
	  f.broadcastEnable.value=(f.broadcastEnablew.checked==true?"1":"0");
      f.action="WlanBase_list_wait.asp";
      f.submit();
}

function selVhtChannel(){
	var f=WlanBase_Config;
	if(f.chanWidth.value =="0"){
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=true;
	} else {
		f.vhtChanWidth.value="0";
		f.vhtChanWidth.disabled=false;
	}	
}
function changurl(){
    parent.main.location="NetConfig.asp";
}
/*
function ScanAP(){
	parent.main.location="ScanAP.asp";
}
*/
function changSecMode(){
    var frm=WlanBase_Config;
	var index=getListValue(frm.apclisecmode);
	wutbl.style.display="none";
	weptbl1.style.display="none";
	wpapsktbl1.style.display="none";
	wdstbl1.style.display="none";
	var dis="";
	if(   document.all   )   
    	dis="block";   
 	else   
     	dis="table-row";
	switch(index){
		case "NONE":weptbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display=dis;
					wdstbl1.style.display="none";
					break;
		case "WEP":	weptbl1.style.display=dis;
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					wdstbl1.style.display="none";
					break;
		case "WPAPSK":weptbl1.style.display="none";
					wpapsktbl1.style.display=dis;
					wutbl.style.display="none";
					wdstbl1.style.display="none";
					break;
		case "AES":
		case "TKIP":weptbl1.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					wdstbl1.style.display=dis;
		default:	break;
	}

}
function changeWDS(val){
			var f=WlanBase_Config;
		if(val){
			f.APWorkMode.value="3";
			selAPWorkMode(f.APWorkMode.value,1);
			document.getElementById("startwds").style.display="";
			f.wdsEnable.value="1";
		}else{
			f.APWorkMode.value="1";
			selAPWorkMode(f.APWorkMode.value,1);
			document.getElementById("startwds").style.display="none";
			f.wdsEnable.value="0";
		}
	
	
}
var key1dis = 0;
var key2dis = 0;
var key3dis = 0;
var key4dis = 0;
var keylength1 = 10;
var keylength2 = 10;
var keylength3 = 10;
var keylength4 = 10;
//var is_Hex = true;
function setKey()
{
	var f = WlanBase_Config;
	getdisable(f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4);
	auto_disable(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeynum,f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4);
	for (i = 0; i < f.apclikeynum.length; i++) {
		if (f.apclikeynum[i].checked) {
			f.wepkeynum.value = i + 1;
			break;
		}
	}
	auto_length(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4,f.apclikeyFormat.selectedIndex);
	/*
	getdisable();
	auto_disable();
	auto_length();*/
}
/*
function getdisable()
{
	var f =WlanBase_Config;
	key1dis = (f.apclikeyLength1.selectedIndex == 0);
	key2dis = (f.apclikeyLength2.selectedIndex == 0);
	key3dis = (f.apclikeyLength3.selectedIndex == 0);
	key4dis = (f.apclikeyLength4.selectedIndex == 0);
}
function auto_disable()
{
	var f =WlanBase_Config;
	var keychoosed;
	f.apcliwepkey1.disabled = f.apclikeynum[0].disabled = key1dis;
	f.apcliwepkey2.disabled = f.apclikeynum[1].disabled = key2dis;
	f.apcliwepkey3.disabled = f.apclikeynum[2].disabled = key3dis;
	f.apcliwepkey4.disabled = f.apclikeynum[3].disabled = key4dis;
	for (i = 0; i < f.apclikeynum.length; i ++)
	{
		if (f.apclikeynum[i].checked)
		{
			keychoosed = f.apclikeynum[i].value;
			break;
		}
	}
	if ((0 == key1dis) || (0 == key2dis) || (0 == key3dis) || (0 == key4dis))
	{
		if ((keychoosed == 1 && f.apclikeyLength1.selectedIndex == 0) ||
			(keychoosed == 2 && f.apclikeyLength2.selectedIndex == 0) ||
			(keychoosed == 3 && f.apclikeyLength3.selectedIndex == 0) ||
			(keychoosed == 4 && f.apclikeyLength4.selectedIndex == 0))
		{
			if (0 == key1dis)
				f.apclikeynum[0].checked = 1;
			else if (0 == key2dis)
				f.apclikeynum[1].checked = 1;
			else if (0 == key3dis)
				f.apclikeynum[2].checked = 1;
			else
				f.apclikeynum[3].checked = 1;
		}
	}
}

function auto_length()
{
	var f = WlanBase_Config;

	keylength1 =(1 == key1dis) ? keylength1 :( ((2 == f.apclikeyLength1.selectedIndex) ? 13 : 5) *((0 == f.apclikeyFormat.selectedIndex) ? 2 : 1));
	keylength2 =(1 == key2dis) ? keylength2 :(((2 == f.apclikeyLength2.selectedIndex) ? 13 : 5) *((0 == f.apclikeyFormat.selectedIndex) ? 2 : 1));
	keylength3 =(1 == key3dis) ? keylength3 :(((2 == f.apclikeyLength3.selectedIndex) ? 13 : 5) *((0 == f.apclikeyFormat.selectedIndex) ? 2 : 1));
	keylength4 =(1 == key4dis) ? keylength4 :(((2 == f.apclikeyLength4.selectedIndex) ? 13 : 5) *((0 == f.apclikeyFormat.selectedIndex) ? 2 : 1));

	if ((f.apcliwepkey1.value.length > keylength1) && (key1dis != 1))
		f.apcliwepkey1.value = f.apcliwepkey1.value.substring(0,keylength1);
	if ((f.apcliwepkey2.value.length > keylength2) && (key2dis != 1))
		f.apcliwepkey2.value = f.apcliwepkey2.value.substring(0,keylength2);
	if ((f.apcliwepkey3.value.length > keylength3) && (key3dis != 1))
		f.apcliwepkey3.value = f.apcliwepkey3.value.substring(0,keylength3);
	if ((f.apcliwepkey4.value.length > keylength4) && (key4dis != 1))	
		f.apcliwepkey4.value = f.apcliwepkey4.value.substring(0,keylength4);
	f.apcliwepkey1.maxLength =  keylength1;
	f.apcliwepkey2.maxLength =  keylength2;
	f.apcliwepkey3.maxLength =  keylength3;
	f.apcliwepkey4.maxLength =  keylength4;

	//if (f.apclikeyFormat.selectedIndex == 0)
		//is_Hex = true;
	//else
		//is_Hex = false;
}
*/
</script>
</head>
<body onbeforeunload="logOut()" >
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(WirelessBase); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/ConfigWirelessBase_5g" onSubmit="return " name="WlanBase_Config" method="post">
<table width="95%" border="0" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td colspan="3" height="20">&nbsp;</td></tr>
   <tr>
   <td colspan="4"><table width="100%"><tr>
	   <td align="right" width="26%" height="25" id="start wireless"><% write(StartWireless); %></td>
	   <td width="2%">&nbsp;</td>
	   <td class="helpfont"><input name="WrlessEnable" type="checkbox"></td>
	   </tr></table></td>
	   </tr>
	  <tr><td colspan="4"><table width="100%"><tr>
	  <td width="26%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription1"><% write(ShowMsg); %></td>	  
	  </tr></table></td></tr>

	   <tr name="weekday" id="weekday" style="display:none">
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right"  height="25"><% write(wlantimetip); %></td>
	   <td width="2%">&nbsp;</td>
	   <td>
		    <input type="checkbox" name="date" value="1"><% write(com_monday); %>&nbsp;
		    <input type="checkbox" name="date" value="2"><% write(com_tuesday); %>&nbsp;
		    <input type="checkbox" name="date" value="3"><% write(com_wednesday); %>&nbsp;
		    <input type="checkbox" name="date" value="4"><% write(com_thursday); %>&nbsp;
		    <input type="checkbox" name="date" value="5"><% write(com_friday); %>
	    </td>
	   </tr></table></td>
	   </tr>

	   <tr name="weekdaytime" id="weekdaytime" style="display:none">
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right"  height="25"></td>
	   <td width="2%">&nbsp;</td>
	   <td>
		    <select name="weekdayfromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekdayfromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<% write(com_to); %>&nbsp;<select name="weekdayendhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekdayendminute" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="day"><input type="hidden" name="weekdaytimestart"><input type="hidden" name="weekdaytimestop">
	    </td>
	   </tr></table></td>
	   </tr>

	   <tr name="weekend" id="weekend" style="display:none">
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right"  height="25"></td>
	   <td width="2%">&nbsp;</td>
	   <td>
		    <input type="checkbox" name="date" value="6"><% write(com_saturday); %>&nbsp;
		    <input type="checkbox" name="date" value="7"><% write(com_sunday); %>
	    </td>
	   </tr></table></td>
	   </tr>

	   <tr name="weekendtime" id="weekendtime" style="display:none"> 
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right"  height="25"></td>
	   <td width="2%">&nbsp;</td>
	   <td>
		    <select name="weekendfromhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekendfromminute" class="list"><script language="javascript">CreateHOp(60);</script></select>&nbsp;&nbsp;<% write(com_to); %>&nbsp;<select name="weekendendhour" class="list"><script language="javascript">CreateHOp(24);</script></select>&nbsp;:&nbsp;<select name="weekendendminute" class="list"><script language="javascript">CreateHOp(60);</script></select></td></tr><input type="hidden" name="weekendtimestart"><input type="hidden" name="weekendtimestop">
	    </td>
	   </tr></table></td>
	   </tr>


	   <tr  style="display:none;">
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right" height="25"><% write(APMode); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="APWorkMode" class="list" onChange="selAPWorkMode(this.value,0)">
	   <script language="javascript">
	   		for(i=0;i<apmodeopt.length;i++){
			 	document.write('<option value="'+apmodeopt[i][0]+'"'+(i==5?"selected":"")+'>'+apmodeopt[i][1]+'</option>');
			}
	   </script>
	   </select></td>
	   </tr></table></td>
	   </tr>
       <tr>

<tr><td colspan="4"><table width="100%"><tr>
	  <td width="26%" align="right" height="25"><%write(wire_name1);%></td>
	  <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	  <td><input name="ssid" type="text" size="18" maxlength="32"  onChange="showfiledtip(re_checkSSIDName(this,'<%write(wire_name1);%>'),ssidtd1)"></td>
      <td  id="ssidtd1"></td>&nbsp;<td style="display:none" id="encode1"><%write(wire_code);%>&nbsp;&nbsp;
        <select name="encodeType" id="encodeType" width=8px>
            <option value="GBK" selected><%write(wire_priComp);%><%write(wire_simple);%></option>
            <option value="BIG5"><%write(wire_priComp);%><%write(wire_complex);%></option>
            <option value="UTF-8"><%write(wire_priPhone);%></option>
        </select>
	  </td>
</tr></table></td></tr>

<tr style="display:none" id="encode2"><td colspan="4"><table width="100%"><tr>
    <td width="26%" align="right" height="25"><%write(wire_name2);%></td>
    <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
    <td ><input name="ssid2" type="text" size="18" maxlength="32"  onChange="showfiledtip(re_checkSSIDName(this,'<%write(wire_name2);%>'),ssidtd2)"></td>
        <td id="ssidtd2"></td>&nbsp;<td><%write(wire_code);%>&nbsp;&nbsp;
            <select name="encodeType2" id="encodeType2" width=8px>
                <option value="GBK" selected><%write(wire_priComp);%><%write(wire_simple);%></option>
                <option value="BIG5"><%write(wire_priComp);%><%write(wire_complex);%></option>
                <option value="UTF-8"><%write(wire_priPhone);%></option>
            </select>
        </td>
</tr></table></td></tr>

<tr><td colspan="4"><table width="100%"><tr>
	  <td width="26%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription2"><% write(ShowMsg1); %></td>	  
	  </tr></table></td></tr>
	   <tr>
	   <tr>
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right" height="25" id="wireless mode"><% write(WirelessMode); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="wrlessMode" class="list" onChange="selwrlessMode(this.value)">
	   <option value="2"><script language="javascript">document.write("<% write(chose4); %>");</script></option>
	   <option value="8"><script language="javascript">document.write("<% write(chose5); %>");</script></option>
	   <option value="14"><script language="javascript">document.write("<% write(chose6); %>");</script></option>
	   <option value="15"><script language="javascript">document.write("<% write(chose7); %>");</script></option>
</select></td>
</tr></table></td>
	   </tr>
	   <tr>
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right" height="25" id="channel"><% write(Channel); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="channel" class="list"><option value="0"><script language="javascript">document.write("<% write(Auto); %>");</script></option>
	   <script language="javascript">
	   		for(i=0;i<channelopt.length;i++){
			 	document.write('<option value="'+channelopt[i]+'"'+(i==5?"selected":"")+'>'+channelopt[i]+'</option>');
			}
	   </script>
	   </select></td>
	   </tr></table></td>
	   </tr>
	   <tr><td colspan="4"><table width="100%"><tr>
	  <td width="26%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription3"><% write(ShowMsg2); %></td>	  
	  </tr></table></td></tr>
	   <tr>
	   <td colspan="4"><table width="100%" style = "display:" id = "chw"><tr>
	   <td width="26%" align="right"  height="25" id="channel bandwidth"><% write(ChannelBand); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="chanWidth" class="list" onChange = "selVhtChannel()">
	      <option value="1"><% write(Auto); %></option><option value="0">20M</option><option value="2">40M</option>
	   </select></td>
	   </tr></table></td>
	   </tr>

	   <tr>
	   <td colspan="4"><table width="100%" style = "display:" id = "VHTchw"><tr>
	   <td width="26%" align="right"  height="25" id="vht channel bandwidth"><% write(VHTChannelBand); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><select name="vhtChanWidth" class="list">
	      <option value="0">20/40</option><option value="1">80M</option>
	   </select></td>
	   </tr></table></td>
	   </tr>

	   <!--<tr>
	   <td align="right"  height="25">速率</td>
	   <td >&nbsp;</td>
	   <td><select name="rate" class="list">
	      <option value="auto">自动</option>
	   </select></td>
	   </tr>-->
	   <tr id="ssidentbl" style="display:none"><td colspan="4" ><table width="100%" >
	   <tr  >
	   <td align="right" width="26%" height="25" id="start SSID"><% write(UsingSSIDG); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><input name="broadcastEnablew" type="checkbox" value="0"><script language="javascript">document.write(wlanCurMac);</script></td>
	   </tr><input type="hidden" name="broadcastEnable">
	   <tr>
	  <td width="26%"></td>
	  <td width="2%"></td>
	  <td  class="helpfont" id="discription4"><% write(ShowMsg3); %></td>	  
	  </tr></table></td></tr>
      	<tr style="display:"><td colspan="4" ><table width="100%" border="0" cellspacing="0" cellpadding="0">
                       <tr>
                       <td align="right" width="26%" height="25" >开启WDS</td>
                       <td width="2%">&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp;</td>
                       <td align="left"><input name="wdsEnabled" type="checkbox" value="0" onClick="changeWDS(this.checked);"></td>
                     </tr><input type="hidden" name="wdsEnable" value="">
                    </table></td></tr>
                   <tr style="display:;" id="startwds" >
                   		<td colspan="4" >
                        	<table width="100%" border="0" cellspacing="0" cellpadding="0">
                        		<tr>
                                <td width="26%" align="right" height="25">(桥接的)SSID</td>
                                <td width="2%"><img src="images/qt.gif" width="7" height="9">&nbsp&nbsp&nbsp&nbsp&nbsp;</td>
                                <td align="left"><input name="bridgessid" type="text" size="18" onChange="showfiledtip(re_checkEnglishName(this,'桥接的SSID'),ssidtd)" maxlength="32"></td><td  id="ssidtd"></td>
                                </tr><!--"-->
                                
                                <tr>
                                <td width="26%" align="right" height="25">(桥接的)BSSID</td>
                                <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
                                <td align="left"><input name="bridgebssid" type="text" size="18" onChange="showfiledtip(re_checkMac(this,'桥接的BSSID'),bssidtd)" maxlength="32"  ></td><td  id="bssidtd"></td>
                                </tr><!-- "-->
                                
                                <tr>
                                <td width="26%" align="right" height="25"></td>
                                <td width="2%"></td>
                                <td align="left"><input type="button" value="扫描"  onClick="scanlocation();"id="scan" class="input-button"></td><td></td>
                                </tr>
                             </table>
                         </td>
                      </tr>
      
      
      
	   <tr id="opssidtbl" style="display:none">
	   <td colspan="4"><table width="100%"><tr>
	   <td align="right" width="26%" height="25"><% write(APsSSID); %></td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="APssid" type="text" size="18" maxlength="32" onChange="showfiledtip(re_checkEnglishName(this,'<% write(APsSSID); %>'),apssidtd)"></td><td id="apssidtd"></td>
	   </tr></table></td>
	   </tr>
	   <tr id="apenabletbl" style="display:none">
	   <td align="right" width="26%" height="25"><% write(UsingAPFun); %></td>
	   <td width="2%" >&nbsp;</td>
	   <td><input name="APEnable" type="checkbox"></td>
	   </tr>
	   <tr id="APClientmactbl" style="display:none">
	   <td colspan="4"><table width="100%"><tr>
	   <td align="right" width="26%" height="25"><% write(APsMAC); %></td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="APCliMacs" type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(APsMAC); %>'),APClientmac)" ></td><td  id="APClientmac"></td>
	   </tr></table></td>
	   </tr>
	   <tr id="mactbl" style="display:none">
	   <td colspan="4"><table width="100%"><tr>
	   <td align="right" width="26%" height="25"><% write(APsMAC); %></td>
	   <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
	   <td><input name="APMac0" type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(APsMAC); %>'),apmactd)" ></td><td  id="apmactd"></td>
	   </tr></table></td>
	   </tr>
	   <!--multapmac-->
     <tr id="mulmactbl" style="display:none;" >
<td colspan="4"><table width="100%"><tr>
	   <td align="right" width="26%" height="25"><% write(APsMAC); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><input name="APMac1"  type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(APsMAC); %>'),ap1mactd)"></td><td  id="ap1mactd"></td>
	   </tr></table></td>
	   </tr>
	   <tr id="mulmactbl1" style="display:none;" >
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right" height="25"><% write(APsMAC); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><input name="APMac2"  type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(APsMAC); %>'),ap2mactd)"></td><td  id="ap2mactd"></td>
	   </tr></table></td>
	   </tr>
	   <tr id="mulmactbl2" style="display:none;" >
	   <td colspan="4"><table width="100%"><tr>
	   <td width="26%" align="right"  height="25"><% write(APsMAC); %></td>
	   <td width="2%">&nbsp;</td>
	   <td><input name="APMac3"  type="text" size="18" maxlength="17" onKeyPress="macOnly()" onChange="showfiledtip(re_checkMac(this,'<% write(APsMAC); %>'),ap3mactd)"></td><td  id="ap3mactd"></td>
	   </tr></table></td>
	   </tr>
	   <!--//apclient security-->
	   <tr id="apclisec" style="display:" align="center"><td colspan="3" align="center" width="100%"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr> 
  <td align="right" width="26%" height="25"><% write(SafeMode); %></td>
	   <td width="3%">&nbsp;</td>
	   <td><select name="apclisecmode" id="apclisecmode" size="1" onChange="changSecMode()">
	   <option value="NONE"><script language="javascript">document.write("<% write(NoSafeMode); %>");</script></option>
	<option value="WEP">WEP</option>
	<option value="WPAPSK">WPA-PSK/WPA2-PSK</option>
      </select></td>
  </tr>
  <!--//wep-->
  <tr id="wutbl" style=" display:none"><td colspan="4"></td></tr>
	   <!--///weptbl///-->
	   <tr id="weptbl1" style=" display:none"><td colspan="4">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr id="wepauthmode" style="display:none">
	   <td align="right"  width="26%" height="25"  id="certification mode"><% write(ApproveType); %></td>
	   <td  width="3%">&nbsp;</td>
	   <td><select name="apcliwepAuthMode" class="list">
	   <option value="OPEN"><script language="javascript">document.write("<% write(OpenSys); %>");</script></option>
	   <option value="SHARED"><script language="javascript">document.write("<% write(ShareKey); %>");</script></option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="key format"><% write(PWForm); %></td>
		<td >&nbsp;</td>
		<td ><select name="apclikeyFormat" class="list" onChange="setKey()">
	   <option value="0" selected><script language="javascript">document.write("<% write(Hex16); %>");</script></option>
	   <option value="1"><script language="javascript">document.write("<% write(ASCIICode); %>");</script></option>
	   </select></td>
	   </tr>
	   <tr>
	   <td width="26%" height="25" align="right" id="key choose"><% write(PWSelect); %></td>
	   <td width="3%">&nbsp;</td>
	   <td><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr>
	   <td width="45%" id="wep key"><% write(PWWEP); %></td>
	   <td id="key mode"><% write(PWType); %></td>
	   </tr></table></td>
	   </tr>
	   <script language="javascript">
	   for (var i=1; i<5; i++)
        		{   
        			j = i-1;
        			document.write('<TR><TD  width="26%" height="25" align="right"> '+"<% write(Key); %>"+i+'：<input type=radio name=apclikeynum value='+i+'></td><td width="3%">&nbsp;</td>');
        			document.write('<TD ><table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td width="45%"><input type=text class=text name=apcliwepkey'+i+' maxLength=32 size="20" onfocus="if(key'+i+'dis) this.blur();else this.form.apclikeynum['+j+'].checked=true;" onChange="showfiledtip(re_checkEnglishName(this),wepkeytd'+i+')"></td>' );
        			document.write('<TD ><select name=apclikeyLength'+i+' class=list id=apclikeyLength'+i+' onchange="setKey();">');
        			document.write('<option value=0 >'+"<% write(Unable); %>"+'</option>');
        			document.write('<option value=5 >'+"<% write(Bit64); %>"+'</option>');
        			document.write('<option value=13>'+"<% write(Bit128); %>"+'</option>');
        			document.write('</select></td><td id=wepkeytd'+i+'></td></tr></table></td></tr>');
        		}
	   </script>
</table>
	   </td></tr>
  <!--//wpapsk-->
  <tr id="wpapsktbl1" style="display:none"><td colspan="3">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
  		 <tr>
	   <td align="right" width="26%" height="25" id="WPA version2"><% write(WPAVer); %></td>
	   <td width="3%">&nbsp;</td>
	   <td><select name="apcliwpapskAuthMode" class="list">
	   <option value="WPAPSK">WPA-PSK</option>
	   <option value="WPA2PSK">WPA2-PSK</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="encryption2"><% write(PWALG); %></td>
		<td >&nbsp;</td>
		<td ><select name="apcliwpapskCipher" class="list">
	   <option value="TKIP">TKIP</option>
	   <option value="AES">AES</option>
	   </select></td>
	   </tr>
	   <tr>
	   	<td height="25" align="right" id="pre-shared key"><% write(SharePW); %></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="apclipskPsswd" type="text" size="18" maxlength="63" onChange="showfiledtip(re_checkPassword1(this,63),psktd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range4); %>");</script></td><td id="psktd"></td><input type="hidden" name="apcli_mode"><input type="hidden" name="apcli_enc">
	   </tr>
</table>
	   </td></tr>
  <!--//end-->
  <!--//aes/tkip-->
  <tr id="wdstbl1" style="display:none"><td colspan="3">
	   <table width="100%" border="0" cellspacing="0" cellpadding="0">
	   <tr>
	   	<td height="25" align="right" id="pre-shared key" width="26%"><% write(SharePW); %></td>
		<td width="3%"><img src="images/qt.gif" width="7" height="9"></td>
		<td ><input name="wdspskPsswd" type="text" size="18" maxlength="63" onChange="showfiledtip(re_checkPassword1(this,63),wdspsktd)">&nbsp;&nbsp;<script language="javascript">document.write("<% write(range4); %>");</script></td><td id="wdspsktd"></td>
	   </tr>
</table>
	   </td></tr>
  <!--//end-->
</table>
</td></tr>
	   <!--//apclient security end-->
	   <tr><td colspan="3" height="20">&nbsp;</td></tr>
<tr><td colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td height="30" align="center" valign="top"><input type="button" class="input-button" value="<% write(com_save); %>"  onClick="save()" id="uttsave">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_reset); %>" onClick="init()" id="uttreset">&nbsp;&nbsp;&nbsp;<input type="button"  class="input-button" value="<% write(com_help); %>" onClick="window.open('<% write(com_helppage); %>#WlanBase')"></td>
  </tr>
</table></td></tr>
<input type="hidden"  name="wrlesMode_chanWid"><input type="hidden" name="wds_list">
  </table></td>
  </tr>
</table>
          <input type="hidden" name="wepkeynum" value=""/>
</form></td>
  </tr>
  <script language="javascript">init();</script>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
