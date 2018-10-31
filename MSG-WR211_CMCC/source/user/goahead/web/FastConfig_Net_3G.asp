 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<% langTran("FastConfig_Net_3G"); %>
<% langTran("common"); %>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

<%getSysLang();%>

var errorstr="";
 
<% Output3GConfig(); %>
 
function changeConnType(val){
	var f=FastConfig_Net_3G;
	if(val=="DHCP"){
		britbl.style.display="none";
		pppoetbl.style.display="none";
	}
	else if(val=="PPPOE"){
		britbl.style.display="none";
		pppoetbl.style.display=dis;
	}
	else{
		britbl.style.display=dis;
		pppoetbl.style.display="none";
	}
}
function changeOpmode(val){
	var f=FastConfig_Net_3G;
	if(val=="3"){
		gatetable.style.display="none";
		g3table.style.display="none";
		apclitable.style.display=dis;
	}
	else if(val=="4"){
		gatetable.style.display="none";
		g3table.style.display=dis;
		apclitable.style.display="none";
	}
	else{
		gatetable.style.display=dis;
		g3table.style.display="none";
		apclitable.style.display="none";
	}
}
function changeAuth3g(val){
	var f=FastConfig_Net_3G;
	
	if(val=="SIM"){
		simtd.style.display=dis;
		pawdtd1.style.display="none";
		pawdtd2.style.display="none";
	}
	else if(val=="PassAuth"){
		simtd.style.display="none";
		pawdtd1.style.display=dis;
		pawdtd2.style.display=dis;
	}
}
function onProviderChange()
{
    var f=FastConfig_Net_3G;
	for(i=14;i<=17;i++){
		document.all["yutd"+i].innerHTML="";
	}
	var providerId=f.ISP3G.selectedIndex;
   	var providerVal=getListValue(f.ISP3G);
	if(f.connectionType3.value == ""){
			if(providerVal=="CMC"){
					f.ConnName.value=g3_para_arr[providerId][4];
					f.DialNum.value=g3_para_arr[providerId][1];
					f.PPPName.value=g3_para_arr[providerId][2];
					f.PPPPasswd.value=g3_para_arr[providerId][3];
			}else if(providerVal !=ISP3Gs ){
					f.ConnName.value=g3_para_arr[providerId][4];
					f.DialNum.value=g3_para_arr[providerId][1];
					f.PPPName.value=g3_para_arr[providerId][2];
					f.PPPPasswd.value=g3_para_arr[providerId][3];
			}else{ 
					f.ConnName.value=ConnNames;
					f.DialNum.value=DialNums;
					f.PPPName.value=PPPNames;
					f.PPPPasswd.value=PPPPasswds;
			}
	}else
	{
			if(providerVal=="CMC"){
					f.ConnName.value=g3_para_arr[providerId][4];
					f.DialNum.value=g3_para_arr[providerId][1];
					f.PPPName.value=g3_para_arr[providerId][2];
					f.PPPPasswd.value=g3_para_arr[providerId][3];
			}else if(providerVal != "<% write(Form("ISP3G"));%>") {
					f.ConnName.value=g3_para_arr[providerId][4];
					f.DialNum.value=g3_para_arr[providerId][1];
					f.PPPName.value=g3_para_arr[providerId][2];
					f.PPPPasswd.value=g3_para_arr[providerId][3];
			}
			else
			{ 
					f.ConnName.value = "<% write(Form("ConnName"));%>" ;
					f.DialNum.value = "<% write(Form("DialNum"));%>" ;
					f.PPPName.value = "<% write(Form("PPPName"));%>" ;
					f.PPPPasswd.value = "<% write(Form("PPPPasswd"));%>";	
			}

	}
   
}
function checkData(){
	var f=FastConfig_Net_3G;
	 
	 
		if (!checkName(f.PinCode,31,"<% write(PIN); %>",1)) return false;
		if(f.AuthType3G.value=="PassAuth"){
		if (!checkName(f.AuthName,31,"<% write(com_user); %>",1)) return false;
		if (!checkName(f.AuthPasswd,31,"<% write(com_passwd); %>",1)) return false;
		}
		if (!checkName(f.ConnName,31,"<% write(APName); %>",1)) return false;
		if (!checkName(f.DialNum,31,"<% write(Dial); %>",1)) return false;
		if (!checkName(f.PPPName,31,"<% write(com_user); %>",1)) return false;
		if (!checkName(f.PPPPasswd,31,"<% write(com_passwd); %>",1)) return false;
	 
	 
	return true;
}
function init(){
		var f=FastConfig_Net_3G;
		if(f.connectionType3.value != "")
		{

				f.Dev3G.value = "<% write(Form("Dev3G"));%>" ;
				f.ISP3G.value = "<% write(Form("ISP3G"));%>" ;
				f.AuthType3G.value ="<% write(Form("AuthType3G"));%>" ;
				f.PinCode.value= "<% write(Form("PinCode"));%>";
				f.AuthName.value= "<% write(Form("AuthName"));%>" ;
				f.AuthPasswd.value = "<% write(Form("AuthPasswd"));%>" ;
		}
		else{
				selVal(f.Dev3G,Dev3Gs);
				selVal(f.ISP3G,ISP3Gs);
				selVal(f.AuthType3G,AuthType3Gs);
				f.PinCode.value=PinCodes;
				f.AuthName.value=AuthNames;
				f.AuthPasswd.value=AuthPasswds;
		}

		onProviderChange();
		changeAuth3g(f.AuthType3G.value);

}

var SeniorShowEn=0;
function clickSenior(frm, adv) {
		if(SeniorShowEn == 0) {
				SeniorShowEn = 1;
		} else {
				SeniorShowEn = 0;
		}
		showTable(adv, SeniorShowEn);
}

function Reset(){
	FastConfig_Net_3G.reset();
	FastConfig_Net_3G.connectionType3.value = "";
	init();
	for(i=11;i<=17;i++){
		document.all["yutd"+i].innerHTML="";
	}
	    
}

function dopre(){
	if(checkData()){
		var f=FastConfig_Net_3G;
		 if(f.opMode2.value != "")
		 	FastConfig_Net_3G.action =  "FastConfig_Net2.asp";
		else if(FastConfig_Net_3G.opMode1.value != "")
		 	FastConfig_Net_3G.action =  "FastConfig_Net.asp";
			 else
				FastConfig_Net_3G.action =  "FastConfig_OpMode.asp";
		 
		 f.connectionType3.value = "PPPOE";
		 FastConfig_Net_3G.submit();
	 }
}
function donext(){
	if(checkData()){
		var f=FastConfig_Net_3G;
		if(!FastConfig_Net_3G.opMode3.value == ""){
									FastConfig_Net_3G.opMode.value = FastConfig_Net_3G.opMode3.value;
									FastConfig_Net_3G.action="FastConfig_Net_WLAN.asp";
		}else
		{
			FastConfig_Net_3G.opMode.value = FastConfig_Net_3G.opMode4.value;
			FastConfig_Net_3G.action="FastConfig_Wlan.asp";
		}
							
		
		//f.staticIp.disabled=f.staticNetmask.disabled=f.staticGateway.disabled=false;
		f.connectionType3.value = "PPPOE";
		FastConfig_Net_3G.submit();
	}	
}

function dojump(){
		var f=FastConfig_Net_3G;
		if(!FastConfig_Net_3G.opMode3.value == ""){
									FastConfig_Net_3G.opMode.value = FastConfig_Net_3G.opMode3.value;
									FastConfig_Net_3G.action="FastConfig_Net_WLAN.asp";
		}else
		{
			FastConfig_Net_3G.opMode.value = FastConfig_Net_3G.opMode4.value;
			FastConfig_Net_3G.action="FastConfig_Wlan.asp";
		}
							
		
	//	f.opMode4.value = "";
		f.connectionType3.value = "";
		FastConfig_Net_3G.submit();

}



function goaway(){
	parent.main.location="SysInfo.asp";
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
	var f = FastConfig_Net_3G;
	getdisable(f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4);
	auto_disable(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeynum,f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4);
	auto_length(f.apcliwepkey1,f.apcliwepkey2,f.apcliwepkey3,f.apcliwepkey4,f.apclikeyLength1,f.apclikeyLength2,f.apclikeyLength3,f.apclikeyLength4,f.apclikeyFormat.selectedIndex);
	/*
	getdisable();
	auto_disable();
	auto_length();*/
}
function changSecMode(){
    var frm=FastConfig_Net_3G;
	var index=getListValue(frm.apclisecmode);
	wutbl.style.display="none";
	weptbl1.style.display="none";
	wepauthmode.style.display="none";
	wpapsktbl1.style.display="none";
	var dis="";
	if(   document.all   )   
    	dis="block";   
 	else   
     	dis="table-row";
	switch(index){
		case "NONE":weptbl1.style.display="none";
					wepauthmode.style.display="none";
					wpapsktbl1.style.display="none";
					wutbl.style.display=dis;
					break;
		case "WEP":	weptbl1.style.display=dis;
					wepauthmode.style.display=dis;
					wpapsktbl1.style.display="none";
					wutbl.style.display="none";
					break;
		case "WPAPSK":weptbl1.style.display="none";
					wepauthmode.style.display="none";
					wpapsktbl1.style.display=dis;
					wutbl.style.display="none";	
					break;
		default:	break;
	}

}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(SecondTital); %>");</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form  name="FastConfig_Net_3G" method="post">

<p>&nbsp;</p>
    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr>
        <td align="center" class="style7"><table border="0" width="600" align='center'>
          <tr>
            <td colspan="4" height="20">&nbsp;</td>
          </tr>
            <tr>
              <td colspan="4" align="center" height="30" valign="top" id="jieshao"><input type="hidden" name="connectionType3" value="<% write(Form("connectionType3"));%>" ><% write(ShowMsg); %><br></td>
            </tr>
			 
			<tr   id="g3table"><td colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="0">
		<tr style="display:none">
	   <td align="right"  height="25" width="38%"><% write(KindOf3GCard); %></td>
	   <td width="3%"></td>
	   <td><select name="Dev3G">
	   <option value="HUAWEI-E169">HUAWEI E169</option>
	   <option value="HUAWEI-E1750">HUAWEI E1750</option>
	   <option value="HUAWEI-EC1260">HUAWEI EC1260</option>
	   <option value="HUAWEI-ET128" >HUAWEI ET128</option>
	   <option value="ZTE-MF637U" >ZTE MF637U</option>
	   </select></td>
	   </tr>

       <tr>
	   <td width="36%" align="right" height="25"><% write(Operators); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="34%"><select name="ISP3G" onChange="onProviderChange()">
	   <option value="CMC"><% write(ChinaMobile); %></option>
	   <option value="CUC"><% write(ChinaUnicom); %></option>
	   <option value="CDC"><% write(ChinaTelnet); %></option>
	   </select></td>
       <td width="23%"></td>
	   </tr>
       <tr id="advanceTable" ><td colspan="4"><table width="100%" border="0" cellpadding="0" cellspacing="0"><tr>
		<td width="36%" height="30" align="right" ><div style="position:relative;z-index:2;"><a href="javascript:clickSenior(FastConfig_Net_3G, adv)" style="color:#000000;text-decoration:underline;"><strong><%write(advance_3g);%></strong></a></div></td>
         <td width="2%" >&nbsp;</td>
         <td width="34%"><%write(advancetip_3g);%></td>
         <td width="23%"></td>
		</tr>
	
	<tr id="adv" style="display:none">
	<td colspan="4" >
	<table width="100%" border="0" cellpadding="0" cellspacing="0" >
	<tr>
	   <td width="36%" align="right" height="25"><% write(ApproveWay); %></td>
	   <td width="2%">&nbsp;</td>
	   <td width="34%"><select name="AuthType3G" onChange="changeAuth3g(this.value)">
	   <option value="SIM"><% write(ApproveSIM); %></option>
	   <option value="PassAuth"><% write(ApprovePW); %></option>
	   </select></td>
       <td width="23%"></td>
	   </tr>
	 	<tr id="simtd" style="display:none">
	   <td align="right"  height="25"><% write(PIN); %></td>
	   <td>&nbsp;</td>
	   <td><input type="text" name="PinCode" style="width:138px;ime-mode:Disabled" maxlength="31" size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(PIN); %>'),yutd11)" value=""></td><td  id="yutd11"></td>
	   </tr>
	   <tr id="pawdtd1" style="display:none">
	   <td align="right"  height="25"><% write(com_user); %></td>
	   <td>&nbsp;</td>
	   <td><input type="text" name="AuthName" style="width:138px;ime-mode:Disabled" maxlength="31" size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(com_user); %>'),yutd12)" value="0"></td><td  id="yutd12"></td>
	   </tr>
	   <tr id="pawdtd2" style="display:none">
	   <td align="right"  height="25"><% write(com_passwd); %></td>
	   <td>&nbsp;</td>
	   <td><input type="text" name="AuthPasswd" style="width:138px;ime-mode:Disabled" maxlength="31" size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(com_passwd); %>'),yutd13)" value="0"></td><td  id="yutd13"></td>
	   </tr>
	    <tr>
	   <td align="right" height="25"><% write(APName); %></td>
	   <td>&nbsp;</td>
	   <td><input type="text" name="ConnName" style="width:138px;ime-mode:Disabled" maxlength="31"  size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(APName); %>'),yutd14)" value="CMNET"></td><td  id="yutd14"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25"><% write(Dial); %></td>
	   <td>&nbsp;</td>
	   <td><input type="text" name="DialNum" style="width:138px;ime-mode:Disabled" maxlength="31" size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(Dial); %>'),yutd15)" value="*99***1#"></td><td  id="yutd15"></td>
	   </tr>
	    <tr>
	   <td align="right"  height="25" ><% write(SupperPPPConf); %></td>
	   <td>&nbsp;</td>
	   <td></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25"><% write(com_user); %></td>
	   <td>&nbsp;</td>
	   <td><input type="text" name="PPPName" style="width:138px;ime-mode:Disabled" maxlength="31" size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(com_user); %>'),yutd16)" value="CMNET" ></td><td  id="yutd16"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25"><% write(com_passwd); %></td>
	   <td>&nbsp;</td>
	   <td><input type="password" name="PPPPasswd" maxlength="31" style="width:138px;ime-mode:Disabled"  size="18"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName(this,'<% write(com_passwd); %>'),yutd17)" ></td><td  id="yutd17"></td>
	   </tr>
  </table></td></tr> 
</table></td></tr>
</table></td></tr>
			
			
            <tr>
              <td colspan="3" height="20">&nbsp;</td>
            </tr>
            <tr>
              <td colspan="3"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                  <tr>
                 <td  height="30" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="<% write(com_previous); %>" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="<% write(ReWrite); %>" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="<% write(LeavePage); %>" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="<% write(SkipPage); %>" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="<% write(com_next); %>" id="next"></td>		<input type="hidden" name="connectionType_OpMode"  value="<% write(Form("connectionType_OpMode"));%>" >
                    <input type="hidden" name="opMode" value="<% write(Form("opMode"));%>"  >
					<input type="hidden" name="oldopMode" value="<% write(Form("oldopMode"));%>" >
					<input type="hidden" name="opMode1"  value="<% write(Form("opMode1"));%>" >
					<input type="hidden" name="opMode2"  value="<% write(Form("opMode2"));%>">
					<input type="hidden" name="opMode3"  value="<% write(Form("opMode3"));%>">
					<input type="hidden" name="opMode4"  value="<% write(Form("opMode4"));%>">
					<input type="hidden" name="notPopUp" value="<% write(Form("notPopUp")); %>">
					<input type="hidden" name="connectionType" value="<% write(Form("connectionType")); %>">
					<input type="hidden" name="staticIp" value="<% write(Form("staticIp")); %>">
					<input type="hidden" name="staticNetmask" value="<% write(Form("staticNetmask")); %>">
					<input type="hidden" name="staticGateway" value="<% write(Form("staticGateway")); %>">
					<input type="hidden" name="staticPriDns" value="<% write(Form("staticPriDns")); %>">
					<input type="hidden" name="staticSecDns" value="<% write(Form("staticSecDns")); %>">
					<input type="hidden" name="pppoeUser" value="<% write(Form("pppoeUser")); %>">
					<input type="hidden" name="pppoePass" value="<% write(Form("pppoePass")); %>">
					
					
					<input type="hidden" name="connectionType2" value="<% write(Form("connectionType2")); %>">
					<input type="hidden" name="staticIp2" value="<% write(Form("staticIp2")); %>">
					<input type="hidden" name="staticNetmask2" value="<% write(Form("staticNetmask2")); %>">
					<input type="hidden" name="staticGateway2" value="<% write(Form("staticGateway2")); %>">
					<input type="hidden" name="staticPriDns2" value="<% write(Form("staticPriDns2")); %>">
					<input type="hidden" name="staticSecDns2" value="<% write(Form("staticSecDns2")); %>">
					<input type="hidden" name="pppoeUser2" value="<% write(Form("pppoeUser2")); %>">
					<input type="hidden" name="pppoePass2" value="<% write(Form("pppoePass2")); %>">
					
					
					<input type="hidden" name="connectionType4" value="<% write(Form("connectionType4"));%>" >
					<input type="hidden" name="APssid" value="<% write(Form("APssid"));%>" >
					<input type="hidden" name="wds_list" value="<% write(Form("wds_list"));%>" >
					<input type="hidden" name="apcli_mode" value="<% write(Form("apcli_mode"));%>" >
					<input type="hidden" name="apcli_enc" value="<% write(Form("apcli_enc"));%>" >
					<input type="hidden" name="apclikeyFormat" value="<% write(Form("apclikeyFormat"));%>" >
					<input type="hidden" name="apclikeynum" value="<% write(Form("apclikeynum"));%>" >
					<input type="hidden" name="apcliwepkey1" value="<% write(Form("apcliwepkey1"));%>" >
					<input type="hidden" name="apcliwepkey2" value="<% write(Form("apcliwepkey2"));%>" >
					<input type="hidden" name="apcliwepkey3" value="<% write(Form("apcliwepkey3"));%>" >
					<input type="hidden" name="apcliwepkey4" value="<% write(Form("apcliwepkey4"));%>" >
					<input type="hidden" name="apclikeyLength1" value="<% write(Form("apclikeyLength1"));%>" >
					<input type="hidden" name="apclikeyLength2" value="<% write(Form("apclikeyLength2"));%>" >
					<input type="hidden" name="apclikeyLength3" value="<% write(Form("apclikeyLength3"));%>" >
					<input type="hidden" name="apclikeyLength4" value="<% write(Form("apclikeyLength4"));%>" >
					<input type="hidden" name="apclipskPsswd" value="<% write(Form("apclipskPsswd"));%>" > 
					
					<input type="hidden" name="connectionType5" value="<% write(Form("connectionType5"));%>" >
					<input type="hidden" name="ssid" value="<% write(Form("ssid"));%>" >
					<input type="hidden" name="wrlessMode" value="<% write(Form("wrlessMode"));%>" >
					<input type="hidden" name="channel" value="<% write(Form("channel"));%>" >
					<input type="hidden" name="chanWidth" value="<% write(Form("chanWidth"));%>" >
					<input type="hidden" name="wrlesMode_chanWid" value="<% write(Form("wrlesMode_chanWid"));%>" >

					
                  </tr>
              </table></td>
            </tr>
          <script language="javascript">init();</script>
            <tr>
              <td colspan="3" height="20">&nbsp;</td>
            </tr>
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



