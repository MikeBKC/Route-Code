<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("IPSec");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script language="javascript">
<%coefficient();%>

var errorstr="";
<% getWanIfCount();
OutputPptpSrvData();
OutputPptpClientData();
aspOutputIPSecListData(); 
var edit_Name=""; 
edit_Name=Form("id");
if ( edit_Name!="")  OutputOneIPSecData(edit_Name);    //需要修改
%>

var edit_Names = "<% write(Form("id"));%>";

<!--
function init(){
    var f=IPSec_Config;
    if(edit_Names!=""&&edit_Names!=null){
		f.Action.value="modify";

		f.instID.value=cur_ids;
		f.connType.value=cur_connType;
		f.APeer.value=cur_peer;
		f.ADestAddr.value=cur_remoteAddr;
		f.ADestMask.value=cur_remoteMask;
		f.ALocalBinding.value=cur_localBind;
		f.ASrcAddr.value=cur_localAddr;
		f.ASrcMask.value=cur_localMask;
		var transform_tmp=cur_transform.split(":");
		f.ATransformset1.value=transform_tmp[0].substring(4);
		f.ATransformset2.value=transform_tmp[1].substring(4);
		f.ATransformset3.value=transform_tmp[2].substring(4);
		f.ATransformset4.value=transform_tmp[3].substring(4);
		//alert(transform_tmp[0].substring(4));
		f.AProtocol.value=cur_protocol;
		f.AsrcPort.value=cur_srcPort;
		f.AdestPort.value=cur_destPort;
		f.ALifetime.value=cur_lifetime_sec;
		f.AKBLifetime.value==cur_lifetime_kbytes;
		if(cur_anti_replay=="on")
			f.AAntiReplay.checked=true;
		else
			f.AAntiReplay.checked=false;
		if(cur_dpdEnable=="on")
			f.ADpdEnabled.checked=true;
		else
			f.ADpdEnabled.checked=false;
		f.ADpdHeartBeat.value=cur_dpdheartbeat;
		if(cur_nattEnable=="on")
		{
			f.ANatT.checked=true;
			f.ANatTPort.value=cur_natt_port;
			f.NatTPortNum.value=cur_natt_port;
			f.ANatKeepalive.value=cur_natt_keepalive;
			f.ANatTPort.disabled=true;
		}
		else
		{
			f.ANatT.checked=false;
			f.ANatTPort.disabled=false;
		}
		f.ARemoteUser.value=cur_remoteUser;
		f.ARemoteUserType.value=cur_remoteUserType;
		f.ALocalUser.value=cur_localUser
		f.ALocalUserType.value=cur_localUserType;
		f.APresharedKey.value=cur_preshareKey;
		f.ANegMode.value=cur_negMode;
		if(cur_aggresEncrypt=="on")
			f.AAggresEncrypt.checked=true;
		else
			f.AAggresEncrypt.checked=false;
		f.AIsakmpLifetime.value=cur_isakmplifetime;
		var policy_tmp=cur_policy.split(":");
		f.APolicy1.value=policy_tmp[0].substring(2);
		f.APolicy2.value=policy_tmp[1].substring(2);
		f.APolicy3.value=policy_tmp[2].substring(2);
		f.APolicy4.value=policy_tmp[3].substring(2);
		if(cur_connType!="1")
		{
			remoteID.style.display="";
			remoteType.style.display="";
			localID.style.display="";
			localType.style.display="";
			if(cur_connType=="3")
			{
				IPSec_Config.APeer.value="0.0.0.0";
				IPSec_Config.APeer.disabled=true;
			}
			negoEncrypt.style.display="none";	
		}
    }
    else{
	f.Action.value="add";
    }
}
function Reset(){
    IPSec_Config.reset();
    init();
    for(i=1;i<=3;i++){
    }
}

function checkPeer()
{
    var str="0123456789.";
    var flag=0;
    for(var i=0;i<IPSec_Config.APeer.value.length;i++)
    {
	if(str.indexOf(IPSec_Config.APeer.value.substring(i, i + 1)) == -1)
	{
	    flag = 1;
	}
    }
    if(flag == 0) {
	if (!checkIP(IPSec_Config.APeer,"<%write(ipsec_peer);%>",0)) return false;
    }else{
	if(!checkDomainName(IPSec_Config.APeer,63,"<%write(ipsec_peer);%>",0)) return false;
    }
    return true;
}

function save(){
    var f=IPSec_Config;
	//alert(f.ALocalBinding.value);
    if(checkData()){
	if(f.Action.value=="add") {
	    checkSameFilter(f.ADestAddr,f.ADestMask,f.ASrcAddr,f.ASrcMask,0);
	} else {
	    checkSameFilter(f.ADestAddr,f.ADestMask,f.ASrcAddr,f.ASrcMask,1);
	}
	f.action="/goform/IPSecIsakmp_Config";
	f.submit();
    }
}
function checkSameUser(r_user, l_user)
{
	for(var i=0;i<totalrecs;i++) {
	    if(edit_Names!=""&&edit_Names!=null){
		if(cur_ids==ids[i]){
		    continue;
		}
	    }
	    if ((r_user==remoteUser[i]) && (l_user==localUser[i])) {
		alert("<%write(ipsec_sameUser);%>");
		return false;
	    }
	}
	return true;
}
function ipToBin(ip){        
	  var subIpArray = ip.split(".");        
	var ipBin = "";        
	 for(var i = 0; i < subIpArray.length; i++){ 		     
			ipBin += binCompletion(parseInt(subIpArray[i]).toString(2));        
	}        
	return ipBin;
 }　
 function binCompletion(subBin){       
	var complBin = subBin;  			     
	if(subBin.length < 8){            
		var length = subBin.length;            
		for (var i = 0; i < (8 - length); i++){  
			complBin = "0" + complBin; 
		}        
	}        
	return complBin;    
}
function andOpration(ipBin, netmaskBin){       
	var result = "";      
	for(var i = 0; i < 32; i++){           
		result += (parseInt(ipBin.charAt(i)) & parseInt(netmaskBin.charAt(i)));    
	}      
	return result;  
 }　
 function checkSameIP(ip1,mask1,ip2,mask2){	
	if((andOpration(ipToBin(ip1),ipToBin(mask1))==andOpration(ipToBin(ip2),ipToBin(mask1))) || (andOpration(ipToBin(ip1),ipToBin(mask2))==andOpration(ipToBin(ip2),ipToBin(mask2)))){				
		return true;	
	}else{
		//alert("不在同一个网段");
		return false;
	}
}

function checkData(){
    var f = IPSec_Config;
	if (IPSec_Config.connType.selectedIndex != 2) {
	    if(!checkPeer()) return false;
	}
	//if(!checkIP(f.APeer,"远端网关",0)) return false;
	if (!checkIP(f.ADestAddr,"<%write(ipsec_remoteinner);%>",0)) return false;
	if (!checkMask(f.ADestMask,"<%write(ipsec_remotemask);%>",0)) return false;
	if (!checkIP(f.ASrcAddr,"<%write(ipsec_localinner);%>",0)) return false;
	if (!checkMask(f.ASrcMask,"<%write(ipsec_localmask);%>",0)) return false;
	if(checkSameIP(f.ADestAddr.value,f.ADestMask.value,f.ASrcAddr.value,f.ASrcMask.value)){		
		alert("<%write(ipsec_ipnotsame);%>");
		return false;
	}
	if(!checkPassword1(f.APresharedKey, 128, 0, 0)) return false;//检查密码:不能超过128个字符、不能为空、应为除% \' \\ \" , ；;以及空格之外的字符
var pre_s=trimstr(f.APresharedKey.value);
	if(pre_s.length==0 ||  pre_s.length>128) {
		alert("<%write(ipsec_keytip);%>");
		f.APresharedKey.focus();
		return false;
	}
	if (!checkNumber(f.AsrcPort,0,65535,"<%write(ipsec_selsrc);%>",1)) return false;
	if (!checkNumber(f.AdestPort,0,65535,"<%write(ipsec_seldst);%>",1)) return false;
	if (!checkNumber(f.AIsakmpLifetime,600,86400,"<%write(ipsec_lifeone);%>",1)) return false;
	if (!checkNumber(f.ALifetime,600,172800,"<%write(ipsec_lifetwo);%>",1)) return false;
	//if (!checkNumber(f.AKBLifetime,20480,2147483647,"<%write(ipsec_maxflow);%>",1)) return false;
	if (f.ANatT.checked == true)
	{
		if (!checkNumber(f.ANatKeepalive,3,60,"<%write(ipsec_keepsecond);%>",1)) return false;
	}
	if (f.ADpdEnabled.checked == true)
	{
		if (!checkNumber(f.ADpdHeartBeat,10,255,"<%write(ipsec_dpdset);%>",1)) return false;
	}
	if(IPSec_Config.connType.selectedIndex != 0) 
	{
	var ruser=trimstr(f.ARemoteUser.value);
	if(ruser.length>31)  {
		alert("<%write(ipsec_remotetip);%>");
		return false;
	}
	/*
	if(ruser.length==0) {
		alert("<%write(ipsec_remoteid);%><%write(com_null);%>");
		return false;
	}
	*/
		if(f.ARemoteUserType.value=="IKE_FQDN")
		if(!checkDomainName(f.ARemoteUser,31,"<%write(ipsec_remoteid);%>",1)) return false;
		if(f.ARemoteUserType.value=="IKE_USERFQDN")
		if(!checkEmailName(f.ARemoteUser,31,"<%write(ipsec_remoteid);%>",0)) return false;
		if(f.ARemoteUserType.value=="IKE_IPV4ADDR")
		if(!checkIP(f.ARemoteUser,"<%write(ipsec_remoteid);%>",0)) return false;
	var luser=trimstr(f.ALocalUser.value);
	if(luser.length>31)  {
		alert("<%write(ipsec_localtip);%>");
	return false;
	}
	/*
	if(luser.length==0) {
		alert("<%write(ipsec_localid);%><%write(com_null);%>");
		return false;
	}
	*/
		if(f.ALocalUserType.value=="IKE_FQDN")
		if(!checkDomainName(f.ALocalUser,31,"<%write(ipsec_localid);%>",1)) return false;
		if(f.ALocalUserType.value=="IKE_USERFQDN")
		if(!checkEmailName(f.ALocalUser,31,"<%write(ipsec_localid);%>",0)) return false;
		if(f.ALocalUserType.value=="IKE_IPV4ADDR")
		if(!checkIP(f.ALocalUser,"<%write(ipsec_localid);%>",0)) return false;
		if(!checkSameUser(ruser, luser)) return false;
	}

	return true;
}
function checkSameFilter(arg1,arg2,arg3,arg4,flag)
{
	destAddr0=arg1.value;
	destMask0=arg2.value;
	srcAddr0=arg3.value;
	srcMask0=arg4.value;
	for(var i=0;i<totalrecs;i++) {
	    if(flag==0)
	    {
		if ((destAddr0==remoteAddr[i]) && (destMask0==remoteMask[i]) && (srcAddr0==localAddr[i]) && (srcMask0==localMask[i])) {
			alert("<%write(ipsec_samefiltip);%>");
		}
	    }
	    if(flag==1)
	    {
		if(cur_ids!=ids[i]) {
		    if ((destAddr0==remoteAddr[i]) && (destMask0==remoteMask[i]) && (srcAddr0==localAddr[i]) && (srcMask0==localMask[i])) {
			alert("<%write(ipsec_samefiltip);%>");
		    }
		}
	    }
	}
}


function checkEmailName(cntrl,maxlen,tips,allownull)
{
	name=trimstr(cntrl.value);
	var t=0;
	for (var i=0;i<name.length;i++)
	{
		var c=name.substring(i,i+1);
		if ((c<'0' || c>'9') && (c<'a' || c>'z') && (c<'A' || c>'Z') && (c!='@') && (c!='.')&& c!='_'&& c!='-')
		{
			alert("<%write(ipsec_illegal);%>"+tips+".");
			cntrl.focus();
			return false;
		}
		else if(c=='@') {
			t++;
			if(t>1) {
			alert("<%write(ipsec_illegal);%>"+tips+".");
			cntrl.focus();
			return false;
			}
		}
	}
	return true;
}




function showid()
{
	remoteID.style.display="none";
	remoteType.style.display="none";
	localID.style.display="none";
	localType.style.display="none";
	IPSec_Config.APeer.disabled=false;
	IPSec_Config.ANegMode.value="Main";
	negoEncrypt.style.display="none";
	IPSec_Config.ARemoteUser.value="";
	IPSec_Config.ALocalUser.value="";
}
function showAll()
{
	remoteID.style.display="";
	remoteType.style.display="";
	localID.style.display="";
	localType.style.display="";
	IPSec_Config.APeer.disabled=false;
	IPSec_Config.ANegMode.value="Aggres";
	negoEncrypt.style.display="none";
}
function showRemote()
{
	remoteID.style.display="";
	remoteType.style.display="";
	localID.style.display="";
	localType.style.display="";
	IPSec_Config.APeer.value="0.0.0.0";
	IPSec_Config.APeer.disabled=true;
	IPSec_Config.ANegMode.value="Aggres";
	negoEncrypt.style.display="none";
}
function connTypeChg()
{
	if (IPSec_Config.connType.selectedIndex == 0) {
		showid();
	} else if (IPSec_Config.connType.selectedIndex == 1) {
		showAll();
	} else if (IPSec_Config.connType.selectedIndex == 2) {
		showRemote();
	} else {
	;
	}

}
function autoAdvClick() {
	var flag = eval(IPSec_Config.autoAdv.value);
	//showTable(selTerm, flag);
	showTable(firstSeg, flag);
	showTable(secondSeg, flag);
	showTable(Other, flag);
	IPSec_Config.autoAdv.value = !flag;
}

function NegmodeChanged()
{
	if(IPSec_Config.ANegMode.value=="Aggres")
	{
		negoEncrypt.style.display="none";
	}
	else
	{
		negoEncrypt.style.display="none";
	}
}
function dpd_click()
{
	if(IPSec_Config.ADpdEnabled.checked==true)
		IPSec_Config.ADpdHeartBeat.value="20";
	else
		IPSec_Config.ADpdHeartBeat.value="";
}
function natt_click()
{
	if(IPSec_Config.ANatT.checked==true) {
		IPSec_Config.ANatTPort.value=0
		if(IPSec_Config.ANatTPort.value=="0")	
		{
			IPSec_Config.ANatTPort.value="4500";
			IPSec_Config.NatTPortNum.value="4500";
			IPSec_Config.ANatTPort.disabled=true;
		}
		IPSec_Config.ANatKeepalive.value="20";
	}
	else {
		IPSec_Config.ANatTPort.value="";
		IPSec_Config.NatTPortNum.value="";
		IPSec_Config.ANatKeepalive.value="";
		IPSec_Config.ANatTPort.disabled=false;
	}
}
//var allTrans="<option value='200'>esp-3des</option><option value='100'>esp-des</option><option value='110'>esp-des-md5</option><option value='120'>esp-des-sha</option><option value='210'>esp-3des-md5</option><option value='220'>esp-3des-sha</option><option value='101'>esp-des-ah-md5</option><option value='102'>esp-des-ah-sha</option><option value='201'>esp-3des-ah-md5</option><option value='202'>esp-3des-ah-sha</option><option value='111'>esp-des-md5-ah-md5</option><option value='112'>esp-des-md5-ah-sha</option><option value='121'>esp-des-sha-ah-md5</option><option value='122'>esp-des-sha-ah-sha</option><option value='211'>esp-3des-md5-ah-md5</option><option value='212'>esp-3des-md5-ah-sha</option><option value='221'>esp-3des-sha-ah-md5</option><option value='222'>esp-3des-sha-ah-sha</option>";
var allTrans="<option value='200'>esp-3des</option><option value='100'>esp-des</option><option value='110'>esp-des-md5</option><option value='120'>esp-des-sha</option><option value='210'>esp-3des-md5</option><option value='220'>esp-3des-sha</option><option value='300' selected>esp-aes128</option><option value='400'>esp-aes192</option><option value='500'>esp-aes256</option><option value='310'>esp-aes128-md5</option><option value='320'>esp-aes128-sha</option><option value='410'>esp-aes192-md5</option><option value='420'>esp-aes192-sha</option><option value='510'>esp-aes256-md5</option><option value='520'>esp-aes256-sha</option>";
var allTrans_other="<option value='200'>esp-3des</option><option value='100'>esp-des</option><option value='110'>esp-des-md5</option><option value='120'>esp-des-sha</option><option value='210'>esp-3des-md5</option><option value='220'>esp-3des-sha</option><option value='300'>esp-aes128</option><option value='400'>esp-aes192</option><option value='500'>esp-aes256</option><option value='310'>esp-aes128-md5</option><option value='320'>esp-aes128-sha</option><option value='410'>esp-aes192-md5</option><option value='420'>esp-aes192-sha</option><option value='510'>esp-aes256-md5</option><option value='520'>esp-aes256-sha</option>";
var allPolicy1="<option value='212' selected>3des-md5-group2</option><option value='213'>3des-md5-group5</option><option value='222'>3des-sha-group2</option><option value='223'>3des-sha-group5</option><option value='112'>des-md5-group2</option><option value='113'>des-md5-group5</option><option value='122'>des-sha-group2</option><option value='123'>des-sha-group5</option>";
var allPolicy2="<option value='212'>3des-md5-group2</option><option value='213'>3des-md5-group5</option><option value='222' selected>3des-sha-group2</option><option value='223'>3des-sha-group5</option><option value='112'>des-md5-group2</option><option value='113'>des-md5-group5</option><option value='122'>des-sha-group2</option><option value='123'>des-sha-group5</option>";
var allPolicy3="<option value='212'>3des-md5-group2</option><option value='213'>3des-md5-group5</option><option value='222'>3des-sha-group2</option><option value='223'>3des-sha-group5</option><option value='112' selected>des-md5-group2</option><option value='113'>des-md5-group5</option><option value='122'>des-sha-group2</option><option value='123'>des-sha-group5</option>";
var allPolicy4="<option value='212'>3des-md5-group2</option><option value='213'>3des-md5-group5</option><option value='222'>3des-sha-group2</option><option value='223'>3des-sha-group5</option><option value='112'>des-md5-group2</option><option value='113'>des-md5-group5</option><option value='122' selected>des-sha-group2</option><option value='123'>des-sha-group5</option>";
//-->
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="images/body_main.png" style="background-repeat:repeat-x">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<!--*****顶上间隔****-->
<tr>
<td colspan="3">&nbsp;</td>
</tr>
<!--****中间内容****-->
<tr>
<td width="15"></td>
<!--////内容-->
<td><script language="javascript">showSecondTitle('<%write(ipsec_conf);%>');</script>
<table width="100%" border="0" height="600" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
<script language="javascript">showerror(errorstr);</script>
<tr valign="top">
<td><form action="/goform/"  name="IPSec_Config" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" id="all">
<p>&nbsp;</p>
<!--<tr>
<td height="34" align="center" class="style7">
<table border="0" width="600" align='center' >
<tr>
<td>
<table width="112">
<tr>
<td width="9">&nbsp;</td>
<td width="86" height="15" align="left">hjka</td>
<td width="10">&nbsp;</td>
</tr>
</table>
</td>
</tr>
</table>
</td>
</tr>-->

<tr>
<td align="center" class="style7">
<table border="0" width="600" align='center' id="Auto"style="display:">
	<tr>
	<td colspan="3" height="20"><input type="hidden" name="Action" ><input type="hidden" name="instID"></td>
	</tr>
	
	<tr>
	<td>
	<table border="0" width="600" align='center' id=""style="display:">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left" ><strong><%write(ipsec_conntype);%></strong></td>
	<td width="3" align="right">&nbsp;</td> 
	<td width="240"><select name="connType" id="connType" class="input" onChange="connTypeChg()">
			<option value="1"><%write(ipsec_gateway);%></option>
			<option value="2"><%write(ipsec_dynamic);%></option>
			<option value="3"><%write(ipsec_dynamicto);%></option>
			</select>
	</td>
	<td width="157" height="25" >&nbsp;</td>
	<td width="101">&nbsp;</td>
	</tr>
	</table></td>
	</tr>
	
	<tr>
	<td>
	<table width="100%" border="0" cellspacing="0" cellpadding="0"  id="">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left"><strong><%write(ipsec_remote);%></strong></td>
	<td width="115" align="right">&nbsp;</td> 
	<td width="3">&nbsp;</td>
	<td width="250" height="25" >&nbsp;</td>
	<td width="155">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="right">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_apeer);%></td>
	<td width="3">&nbsp;</td>
	<td width="250"><div align="left"><input type="text" name="APeer" value=""></div></td>
	<td width="155">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_netaddr);%></td>
	<td width="3">&nbsp;</td>
	<td width="250" ><div align="left"><input type="text" name="ADestAddr" value="0.0.0.0"></div></td>
	<td width="155">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_netmask);%></td>
	<td width="3">&nbsp;</td>
	<td width="250" ><div align="left"><input type="text" name="ADestMask" value="255.255.255.0"></div></td>
	<td width="155">&nbsp;</td>
	</tr>
	<tr id="remoteID" style="display:none">
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_userid);%></td>
	<td width="3">&nbsp;</td>
	<td width="250" ><div align="left"><input type="text" name="ARemoteUser" value=""></div></td>
	<td width="155">&nbsp;</td>
	</tr>
	<tr id="remoteType" style="display:none">
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_usertype);%></td>
	<td width="3">&nbsp;</td>
	<td width="250" ><div align="left"><select name="ARemoteUserType" id="ARemoteUserType" class="input" >
		<option value="IKE_FQDN"><%write(ipsec_domain);%></option>
		<option value="IKE_USERFQDN"><%write(ipsec_email);%></option>
		<!--<option value="IKE_KEYID"><%write(ipsec_keyid);%></option-->
		<option value="IKE_IPV4ADDR"><%write(ipsec_ipv4addr);%></option>
		</select></div></td>
	<td width="155">&nbsp;</td>
	</tr>
	</table>		   
	</td>
	</tr>
	<tr>
	<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr>
	<td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id=""style="display:">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left"><strong><%write(ipsec_local);%></strong></td>
	<td width="115" align="right">&nbsp;</td> 
	<td width="3">&nbsp;</td>
	<td width="250" height="25" >&nbsp;</td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="right">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_bind);%></td>
	<td width="3">&nbsp;</td> 
	<td width="250"><div align="left"><select name="ALocalBinding" size="1" class="input">
		<script language="javascript"> 
			for(var i=1;i<=wanIfCount;i++){
		    	if (is3GPort(i,wanIfCount)) {
					document.write('<option value="'+i+'" >3G</option>');
		    	} else if (isAPCliPort(i,wanIfCount)) {
					document.write('<option value="'+i+'" >APClient</option>');
		    	} else {
					document.write('<option value="'+i+'" >WAN'+i+'</option>');
		    	}
		 	}
			document.write('<option value="0">LAN</option>');
			for(var j=0;j<instNames_srv.length;j++) {
					document.write('<option value="PS_'+instNames_srv[j]+'" >PPTPS_'+instNames_srv[j]+'</option>');
			}
			for(var k=0;k<instNames_cli.length;k++) {
					document.write('<option value="PC_'+instNames_cli[k]+'" >PPTPC_'+instNames_cli[k]+'</option>');
			}
		</script>
		<!--<option value="0"><%write(ipsec_lan);%></option>-->
	</select></div></td>
	<td width="133" height="25" align="right">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_netaddr);%></td>
	<td width="3">&nbsp;</td>
	<td width="250"><div align="left"><input type="text" name="ASrcAddr" value="<%write(getLanIp());%>"></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_netmask);%></td>
	<td width="3">&nbsp;</td>
	<td width="250"><div align="left"><input type="text" name="ASrcMask" value="255.255.255.0"></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr id="localID" style="display:none">
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_userid);%></td>
	<td width="3">&nbsp;</td>
	<td width="250" ><div align="left"><input type="text" name="ALocalUser" value=""></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr id="localType" style="display:none">
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_usertype);%></td>
	<td width="3">&nbsp;</td>
	<td width="250" ><div align="left"><select name="ALocalUserType" id="ALocalUserType" class="input" >
		<option value="IKE_FQDN"><%write(ipsec_domain);%></option>
		<option value="IKE_USERFQDN"><%write(ipsec_email);%></option>
		<!--<option value="IKE_KEYID"><%write(ipsec_keyid);%></option>-->
		<option value="IKE_IPV4ADDR"><%write(ipsec_ipv4addr);%></option>
		</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td colspan="6" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr>
	<td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id=""style="display:">
	<tr>
	<td width="<%write(ipsec_width1);%>" colspan="2" height="25" align="left"><strong><%write(ipsec_safesel);%></strong></td>
	
	<td width="3">&nbsp;</td>
	<td width="250" height="25" >&nbsp;</td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="right">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_preshare);%></td>
	<td width="3">&nbsp;</td> 
	<td width="250"><div align="left"><input name="APresharedKey" type="password" class="input" id="APresharedKey" size="18" /></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform_p2);%></td>
	<td width="3">&nbsp;</td>
	<td width="250"><div align="left"><select name="ATransformset1" id="ATransformset1"class="input">
	<script language="javascript">document.write(allTrans);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td colspan="6" align="center" height="15"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr>
	<td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id=""style="display:">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="20" align="left"><a href="javascript:autoAdvClick()" style="color:#000000;text-decoration:underline;"><%write(ipsec_advancesel);%></a></td>
	<td width="115" align="right">&nbsp;</td> 
	<td width="10">&nbsp;</td>
	<td width="250">&nbsp;</td>
	<td width="133"><input type="hidden" name="autoAdv" id="autoAdv" value="true" /></td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id="selTerm"style="display:none">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left"><strong><%write(ipsec_selterm);%></strong></td>
	<td width="<%write(ipsec_width2);%>" align="right">&nbsp;</td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" >&nbsp;</td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(com_protocol);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="AProtocol" class="input">
		    <option value="0"><%write(ipsec_renyi);%></option>
		    <option value="6">TCP</option>
		    <option value="17">UDP</option>
		    <option value="1">ICMP</option>
		  </select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_srcport);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="AsrcPort" type="text" class="input" size="18"  value="0" /></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_dstport);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="AdestPort" type="text" class="input" size="18"  value="0" /></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td colspan="5" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr><td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table>
	</td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id="firstSeg"style="display:none">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left"><strong><%write(ipsec_phaseone);%></strong></td>
	<td width="<%write(ipsec_width2);%>" align="right">&nbsp;</td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" >&nbsp;</td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_negmode);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="ANegMode" onChange="NegmodeChanged()" class="input">
		    <option value="Main"><%write(ipsec_main);%></option>
		    <option value="Aggres"><%write(ipsec_aggres);%></option>
		  </select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr id="negoEncrypt" style="display:none">
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_aggresencrypt);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="AAggresEncrypt" type="checkbox" checked></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_lifetime);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><input name="AIsakmpLifetime" type="text" class="input" size="18" value="28800" /><%write(com_second);%></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform1);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="APolicy1" class="input">
	<script language="javascript">document.write(allPolicy1);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform2);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="APolicy2" class="input">
		<option value="000">--------------</option>
		<script language="javascript">document.write(allPolicy2);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform3);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="APolicy3" class="input">
		    <option value="000">--------------</option>
			<script language="javascript">document.write(allPolicy3);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform4);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="APolicy4" class="input">
		    <option value="000">--------------</option>
			<script language="javascript">document.write(allPolicy4);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td colspan="5" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr><td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table>
	</td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id="secondSeg"style="display:none">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left"><strong><%write(ipsec_phasetwo);%></strong></td>
	<td width="<%write(ipsec_width2);%>" align="right">&nbsp;</td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" >&nbsp;</td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform2);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="ATransformset2" class="input">
		    <option value="000">-------------</option>
			<script language="javascript">document.write(allTrans_other);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform3);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="ATransformset3" class="input">
		    <option value="000">-------------</option>
			<script language="javascript">document.write(allTrans_other);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_transform4);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><select name="ATransformset4" class="input">
		    <option value="000">-------------</option>
			<script language="javascript">document.write(allTrans_other);</script>
	</select></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_lifetime);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="ALifetime" type="text" class="input" value="3600" size="18" /><%write(com_second);%></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr style="display:none">
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_maxflow);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="AKBLifetime" type="text" class="input" id="AKBLifetime" value="4194300" size="18" /><%write(ipsec_kilobytes);%></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td colspan="5" align="center" height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr><td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table>
	</td>
	</tr>
	</table></td>
	</tr>
	<tr>
	<td>
	<table border="0" width="600" align='center' id="Other"style="display:none">
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left"><strong><%write(ipsec_others);%></strong></td>
	<td width="<%write(ipsec_width2);%>" align="right">&nbsp;</td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" >&nbsp;</td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_antireplay);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="AAntiReplay" type="checkbox"></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_dpdenable);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="ADpdEnabled" type="checkbox"  onclick="dpd_click();" /></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_dpdheart);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="ADpdHeartBeat" type="text" class="input"	size="6" /><%write(com_second);%></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_natt);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="ANatT" type="checkbox" onClick="natt_click();" /></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_port);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="ANatTPort" type="text" class="input" size="8" /></div></td>
	<td width="133">&nbsp;<input name="NatTPortNum" type="hidden"></td>
	</tr>
	<tr>
	<td width="<%write(ipsec_width1);%>" height="25" align="left">&nbsp;</td>
	<td width="<%write(ipsec_width2);%>" align="right"><%write(ipsec_keep);%></td> 
	<td width="3">&nbsp;</td>
	<td width="257" height="25" ><div align="left"><input name="ANatKeepalive" type="text" class="input" size="6" /><%write(com_second);%></div></td>
	<td width="133">&nbsp;</td>
	</tr>
	<tr>
	<td colspan="5" align="center" height="20"><table width="100%" border="0" cellspacing="0" cellpadding="0">
	<tr><td background="images/dot.gif"><img src="images/space.gif" width="8" height="1"></td>
	</tr>
	</table>
	</td>
	</tr>
	</table></td>
	</tr>
	<!--<tr><td colspan="3" height="20">&nbsp;</td></tr>-->
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr>
	<td height="25" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#IPSec')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('IPSec.asp')"></td>
	</tr>
	</table>
	</td></tr>
</table>
</td>
</tr>

<tr>
<td align="center" class="style7">
<table border="0" width="600" align='center' id="Manual"style="display:none">
	<tr>
	<td colspan="3" height="20"><input type="hidden" name="" ><input type="hidden" name="pdbPolicy" value="pdbPolicy">&nbsp;</td>
	</tr>
	<tr width="100%">
	<td width="100%">
	<table  id="" style="display:none">
	<tr>
	<td width="74" height="25" align="right"></td>
	<td width="10">&nbsp;</td>
	<td width="114">&nbsp;</td>
	<td width="8">&nbsp;</td>
	<td width="20" height="25" align="right">&nbsp;</td>
	<td width="10">&nbsp;</td>
	</tr>
	</table></td>
	</tr>
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr>
	<td height="30" align="center" valign="top"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('rhelpinfo.htm#')">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_back);%>" onClick="GoBack_Page('IPSec.asp')"></td>
	</tr>
	</table>
	</td></tr>
</table>
</td>
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
