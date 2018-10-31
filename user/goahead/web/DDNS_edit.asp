 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("DDNS_edit");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml">

//Butterlate.setTextDomain("internet");

//function initTranslation()
//{
//	var e;	
//	e = document.getElementById("ddnsstart");
//	e.innerHTML= _("ddnsstart");
//	e = document.getElementById("shuoming");
//	e.innerHTML= _("shuoming");
//	e = document.getElementById("services");
//	e.innerHTML= _("services");
//	e= document.getElementById("dnsyuming");
//	e.innerHTML= _("dnsyuming");
//	e = document.getElementById("pcnum");
//	e.innerHTML= _("pcnum");
//	e = document.getElementById("name");
//	e.innerHTML= _("name");
//	e = document.getElementById("passtb1");
//	e.innerHTML= _("passtb1");

//	e = document.getElementById("ddns zhuangtai");
//	e.innerHTML= "<strong>"+_("ddns zhuangtai")+"</strong>";
//	e = document.getElementById("ddns update");
//	e.innerHTML= "<strong>"+_("ddns update")+"</strong>";
//	e= document.getElementById("pcnum2");
//	e.innerHTML= "<strong>"+_("pcnum")+"</strong>";
//	e = document.getElementById("ipaddr");
//	e.innerHTML= "<strong>"+_("ipaddr")+"</strong>";
//	e= document.getElementById("updatetime");
//	e.innerHTML= "<strong>"+_("updatetime")+"</strong>";
//	e= document.getElementById("update state");
//	e.value= _("update state");
//	e = document.getElementById("uttreset");
//	e.value= _("reset");
//	e = document.getElementById("uttsave");
//	e.value= _("save");
//
//}
<%  outputSystemSn();
    outputDDNSType();
    getWanIfCount();
    var edit_Name="";
        edit_Name=Form("id");
        if ( edit_Name!="") outputDDNSListOneInfo(edit_Name);//需要修改
%>
var edit_Names = "<% write(Form("id"));%>";
if (typeof(DDNSProviders)=='undefined')                                         
        var DDNSProviders = "";

/* phddns为花生壳ddns开关，需要在config.js里定义，若未定义或未赋值，此处定义值为1 */
//if("undefined" == typeof phddns)
//    var phddns = 1;
//DDNSProviders=(DDNSProviders==""?"none":DDNSProviders);
//var ConnStatus="";
//ConnStatus=(ddns_ips==""?"<%write(com_Disconnected);%>":"<%write(com_Connected);%>");
<!--
function checkDDNS(cntrl,str)
{
	var name=cntrl.value;
	if (str == "iplink.com.cn")
	{
		index=name.indexOf(".");
        	hostname1=name.substring(0,index);
        	hostname2=name.substring(index+1,name.length+1);
		if(hostname2 != "iplink.com.cn") {
			alert(_("<%write(ddn_iplinkTip);%>"));
			return false;
		}
	}
	else if(str == "3322.org")
	{
		index=name.indexOf(".");
        	hostname1=name.substring(0,index);
        	hostname2=name.substring(index+1,name.length+1);
		if((hostname2 != "3322.org")&&(hostname2 != "f3322.net")&&(hostname2 != "eatuo.com")&&(hostname2 != "webok.net")&&(hostname2 != "7766.org")&&(hostname2 != "6600.org")&&(hostname2 != "9966.org")&&(hostname2 != "2288.org")&&(hostname2 != "8800.org")&&(hostname2 != "8866.org")){
		    alert(_("<%write(ddns_alerterror);%>"));
		    return false;
		}
	}
	
	return true;
}
function checkData(){
	var f=DDNSConfig;
	var SerName=getListValue(f.DDNSProvider);
	if(SerName!="none"){
		var maxpasslen=(SerName=="iplink.com.cn"?63:31);
		if (SerName=="www.oray.net") {
		    if (!checkEnglishName(f.DDNS,31,"<%write(ddn_host);%>",1)) return false;
		} else {
		    if (!checkEnglishName(f.DDNS,31,"<%write(ddn_host);%>",0)) return false;
		}
		if(SerName=="3322.org") if (!checkName(f.Account,31,"<%write(com_user);%>",0)) return false;
		if(SerName=="no-ip.com") if (!checkName(f.Account,31,"<%write(com_user);%>",0)) return false;
		if(SerName=="dyndns.org") if (!checkName(f.Account,31,"<%write(com_user);%>",0)) return false;
		if(SerName=="www.oray.net") if (!checkName(f.Account,31,"<%write(com_user);%>",0)) return false;

		if (!checkEnglishName(f.Password,maxpasslen,"<%write(com_passwd);%>",0)) return false;
		if (!checkDDNS(f.DDNS,SerName)) return false;
		if (!checkName(f.Password,maxpasslen,passtbl.innerHTML,0)) return false;
	}
	return true;
}
function save(){
	if(checkData()){
		var f=DDNSConfig;
		f.Profile.disabled = false;
		//alert(f.Password.value);
		DDNSConfig.action="/goform/DDNS";
		DDNSConfig.submit();
		f.Profile.disabled = true;
	}	
}
function init(){
	var f=DDNSConfig;
	//f.DDNSEnable.checked=(DDNSEnables=="on"?true:false);
	if(DDNS_dyndns == 1)
	{
	var option2 =document.createElement("OPTION");
	option2.text = "dyndns.org";
	option2.value = "dyndns.org";
	f.DDNSProvider.options.add(option2);
	}
	if(DDNS_no_ip == 1)
	{
	var option1 =document.createElement("OPTION");
	option1.text = "no-ip.com";
	option1.value = "no-ip.com";
	f.DDNSProvider.options.add(option1);
	}

	/* 请在config.js 里加上phddns这个变量（参考nv512Wv3）*/
/*	if (phddns == 1)
	{
	    var option3 = document.createElement("OPTION");
	    option3.text = _("<%write(ddn_oray);%>");
	    option3.value = "www.oray.net";
	    f.DDNSProvider.options.add(option3);
	}
 */     

	selService(DDNSProviders);

	if(edit_Names!=""&&edit_Names!=null){
	    selVal(f.DDNSProvider,DDNSProviders);
	    f.Account.value=Accounts;
	    f.DDNS.value=DDNSs;
	    f.Action.value="modify";
	    f.Password.value=Passwords;
	    f.Profile.disabled = true;
	    selVal(f.Profile,Profiles);
	}
}
function selService(val){
	var f=DDNSConfig;
	f.Account.value="";
	f.DDNS.value="";
	f.Password.value="";
	var f=DDNSConfig;
		hostnametd.innerHTML="";
		usernametd.innerHTML="";
		passwdtd.innerHTML="";
	if(val=="iplink.com.cn"){
		if(val==DDNSProviders){
			f.Account.value="";
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		usernametbl.style.display="none";
		ddnsurl.innerHTML="<a href=http://www.com.cn/ddns target=_blank>http://www.com.cn/ddns</a>&nbsp;&nbsp;<br><% write(ddn_regID); %>"+productIDs;
		passtbl.innerHTML=_("<%write(ddn_key);%>");
		Profiletb.style.display = dis;
		hostnametbl.style.display=dis;
		passtbl1.style.display=dis;
		domaintbl.style.display="none";
		distbl.style.display=dis;
		setenable(true);
        document.getElementById("err").style.display="";
	}
	else if(val=="3322.org"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		usernametbl.style.display=dis;//alert(22);
		ddnsurl.innerHTML="<a href=http://www.pubyun.com target=_blank>http://www.pubyun.com</a>";
		passtbl.innerHTML=_("<%write(com_passwd);%>");

		Profiletb.style.display = dis;
		hostnametbl.style.display=dis;
		passtbl1.style.display=dis;
		domaintbl.style.display=dis;
	    	document.getElementById("err").style.display="";
		distbl.style.display="none";
		//usernametbl.style.display="none";
		setenable(true);
	}
	else if(val=="www.oray.net"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		usernametbl.style.display=dis;//alert(22);
		ddnsurl.innerHTML="<a href=http://www.oray.net target=_blank>http://www.oray.net</a>";
		passtbl.innerHTML=_("<%write(com_passwd);%>");

		Profiletb.style.display = dis;
		hostnametbl.style.display="none";
		passtbl1.style.display=dis;
		domaintbl.style.display=dis;
		distbl.style.display="none";
		//usernametbl.style.display="none";
		setenable(true);
		document.getElementById("err").style.display="none";
	}

	else if(val=="dyndns.org"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		usernametbl.style.display=dis;//alert(22);
		ddnsurl.innerHTML="<a href=http://www.dyndns.org target=_blank>http://www.dyndns.org</a>";
		passtbl.innerHTML=_("<%write(com_passwd);%>");

		Profiletb.style.display = dis;
		hostnametbl.style.display=dis;
		passtbl1.style.display=dis;
		domaintbl.style.display=dis;
		distbl.style.display="none";
		//usernametbl.style.display="none";
		setenable(true);
			document.getElementById("err").style.display="";
	}
	else if(val=="no-ip.com"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		usernametbl.style.display=dis;//alert(22);
		ddnsurl.innerHTML="<a href=http://www.no-ip.com target=_blank>http://www.no-ip.com</a>";
		passtbl.innerHTML=_("<%write(com_passwd);%>");

		Profiletb.style.display = dis;
		hostnametbl.style.display=dis;
		passtbl1.style.display=dis;
		domaintbl.style.display=dis;
		distbl.style.display="none";
		//usernametbl.style.display="none";
		setenable(true);
			document.getElementById("err").style.display="";
	}
	else{
		setenable(false);
		hostnametbl.style.display="none";
		usernametbl.style.display="none";
		passtbl1.style.display="none";
		domaintbl.style.display="none";
		distbl.style.display="none";
		Profiletb.style.display ="none";
		//ddnsurl.innerHTML="<a href=http://com.cn/ddns target=_blank>http://om.cn/ddns</a>";
		//passtbl.innerHTML=_("key");
			document.getElementById("err").style.display="";
	}
}
function Reset(){
	DDNSConfig.reset();
	hostnametd.innerHTML="";
	usernametd.innerHTML="";
	passwdtd.innerHTML="";
	init();	
}
function showDDNSStu(){
	document.write('<tr bgcolor="#ffffff"><td><table width="100%" height="20" border="0" cellpadding="1" cellspacing="0"><tr>');
	document.write('<td width="2%" height=30>&nbsp;</td><td width="25%" align=center>'+ConnStatus+'</td><td width="30%" align=center>'+DDNSs+'</td>');
	document.write('<td width="20%" align=center>'+ddns_ips+'</td><td align=center>'+ddns_times+'</td>');
	document.write('</tr></table></td></tr>');
}
function Update(){
	DDNSConfig.action="/goform/";
	DDNSConfig.submit();
}
function setenable(flag){
	var f=DDNSConfig;
	f.Account.disabled=!flag;
	f.DDNS.disabled=!flag;
	f.Password.disabled=!flag;
	ddnsurl.disabled=!flag;
}
//-->
</script>
</head>
<body onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <tr>
  <td width="15"></td>
  <td><script language="javascript">showSecondTitle(_("<%write(ddn_config);%>"));</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr,80);</script>
  <tr valign="top">
    <td><form  name="DDNSConfig" method="post">
<script language="javascript">                                                  
                var arrayThreeMenu=new Array(                                   
                                                "80%",                          
                                                2,                              
                            new Array("<%write(DDNS_status);%>",'DDNSAdvance.asp',1,0),          
                            new Array("<%write(DDNS_setting);%>",'DDNS_edit.asp',1,1) 
                );                                                              
                showThreeMenu();                                                
        </script> 
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr>
    <td align="center" class="three_td" valign="top"><table border="0" width="600" align='center'>
      <tr>
        <td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td>
      </tr>
      <!--<tr>
	   <td align="right"  height="25" width="30%" id="ddnsstart">启用DDNS服务</td>
	   <td width="2%">&nbsp;<input type="hidden" name="ddnsen" value="1"></td>
	   <td><input type="checkbox" name="DDNSEnable" onClick="setenable(this.checked)"></td>
	   </tr>
	   <tr><td></td><td></td><td colspan="2" class="helpfont" id="shuoming">打勾表示启用DDNS功能，只有启用该功能，DDNS配置才能生效。</td></tr>-->
      <tr>
        <td align="right"  height="25" width="33%" id="services"><%write(ddn_service);%></td>
        <td width="3%">&nbsp;</td>
        <td width="36%"><select name="DDNSProvider" onChange="selService(this.value)">
            <option value="none" id="none">
              <script language="javascript">document.write(_("<%write(ddn_none);%>"));</script>
              </option>
            <option value="www.oray.net"><%write(ddn_oray);%></option>
            <option value="3322.org">3322.org</option>
	    <option value="iplink.com.cn">iplink.com.cn</option>
        </select></td><td></td>
      </tr>
      <tr id="domaintbl" style="display:none">
        <td align="right" height="25" id="dnsyuming"><%write(ddn_regDns);%></td>
        <td >&nbsp;</td>
        <td id="ddnsurl"></td>
      </tr>
      <tr id="hostnametbl">
        <td align="right"  height="25" id="pcnum"><%write(ddn_host);%></td>
        <td ><img src="<%write(com_imgqt);%>" width="7" id="err" height="9"></td>
        <td><input type="text" name="DDNS"  maxlength="31" style="width:160px"></td>
        <td width="28%"  id="hostnametd"></td>
      </tr>
      <tr id="usernametbl" style="display:none">
        <td align="right"  height="25" id="name"><%write(com_user);%></td>
        <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
        <td><input type="text" name="Account" size="20"   maxlength="31" style="width:160px"></td>
        <td  id="usernametd"></td>
      </tr>
      <tr id="passtbl1">
        <td align="right"  height="25" id="passtbl"></td>
        <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
        <td><input name="Password" type="password" size="20" maxlength="64" style="width:160px"></td>
        <td  id="passwdtd"></td>
      <tr id = "Profiletb" style="display:none">
        <td align="right" height="25"><%write(com_Interface);%></td>
        <td >&nbsp;</td>
        <td ><select name="Profile">
            	<script language="javascript"> 
		 for(var i=1;i<=wanIfCount;i++){
				if (is3GPort(i,wanIfCount)) {
					document.write('<option value="WAN'+i+'" >3G</option>');
				} else if (isAPCliPort(i, wanIfCount)) {
					document.write('<option value="WAN'+i+'" >APClient</option>');
				} else {
					document.write('<option value="WAN'+i+'" '+ (i==1? 'selected="selected"':'') +'>WAN'+i+'</option>');
				}  
		 }
                </script>
        </select></td>
      </tr>
      <tr  id="distbl">
        <td  colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
            <tr>
              <td  height="30" align="center"  valign="top" class="helpfont" id="discription4"><%write(ddn_iplink);%></td>
            </tr>
	    <tr>
              <td  height="30" align="center"  valign="top" class="helpfont" id="discription5"><%write(ddns_alertnotice);%></td>
            </tr>

        </table></td>
      </tr>
      <tr>
        <td  colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
            <tr>
              <td  height="50" align="center" valign="middle"><input name="button" type="button" class="input-button" id="uttsave" onClick="save()" value="<%write(com_save);%>">
                &nbsp;&nbsp;&nbsp;
                <input name="button" type="button" class="input-button" id="uttreset" onClick="Reset()" value="<%write(com_reset);%>">
				&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#DDNS')">
		&nbsp;&nbsp;&nbsp;
              <input name="button" type="button" class="input-button" id="uttback" onClick="GoBack_Page( 'DDNSAdvance.asp')" value="<%write(com_back);%>"></td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
 </table>
  <script language="javascript">init();</script>
</form></td>
  </tr>
</table></td>
  <td width="15"></td>
  </tr>
</table>
</body>
</html>
