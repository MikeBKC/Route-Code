<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("DDNS");%>
<title>DDNS</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script>
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script>  
</head>
<script type="text/javascript">
<%outputDDNS();%>
<% getWanIfCount();%>
<% outputSystemSn();%>
/*
var productIDs = "12140010";
var DDNSProviders="";
var Accounts="";
var DDNSs="sdsd.3322.org";
var Passwords="";
var Profiles="";
var DDNS_no_ip=0;
var DDNS_dyndns=0;
var ddns_times="2013-12-23:12:34:23";var ddns_ips="192.168.16.1";var errorstr="";
var wanIfCount=3;var maxwanIfCount=3;
*/
DDNSProviders=(DDNSProviders==""?"none":DDNSProviders);
var ConnStatus="1";
ConnStatus=(ddns_ips==""?"<%write(com_Disconnected);%>":"<%write(com_Connected);%>");

function init(){
	var f=DDNSConfig;
	if(DDNS_UTTCARE_NUM==1)
	{
		f.DDNSProvider.options.add(new Option("care.com","care.com"));
	}
	if(DDNS_ip_link==1)
	{
		f.DDNSProvider.options.add(new Option("iplink.com.cn","iplink.com.cn"));
	}
	if(DDNS_no_ip == 1)
	{
	    var option1 =document.createElement("OPTION");
	    option1.text = "no-ip.com";
	    option1.value = "no-ip.com";
	    f.DDNSProvider.options.add(option1);
	}
	if(DDNS_dyndns == 1)
	{
	    var option2 =document.createElement("OPTION");
	    option2.text = "dyndns.org";
	    option2.value = "dyndns.org";
	    f.DDNSProvider.options.add(option2);
	}
	selVal(f.DDNSProvider,DDNSProviders);
	selService(DDNSProviders);
	f.Account.value=Accounts;
	f.DDNS.value=DDNSs;
	f.Password.value=Passwords;
	if(DDNSProviders=="none"){
	    ConnStatus="";
	}
	$("#updatestatus").html(ConnStatus);
	$("#hostname").html(DDNSs);
	$("#ipadd").html(ddns_ips);
	$("#updatetime").html(ddns_times);
	selService(DDNSProviders);
	passtbl.innerHTML="<%write(com_passwd);%>";
	$("#error_msg").html(errorstr);
	$("#registerNum").html(productIDs);
showSuccessTips(rightstr);
}

function checkDDNS(cntrl,str)
{
    var name=cntrl.value;
    if (str == "iplink.com.cn")
    {
	    index=name.indexOf(".");
	    hostname1=name.substring(0,index);
	    hostname2=name.substring(index+1,name.length+1);
	    if(hostname2 != "iplink.com.cn") {
		$("#error_msg").html("<%write(ddn_iplinkTip);%>");
		return false;
	    }
    }
    /*
    else if(str == "3322.org")
    {
	var index=name.indexOf(".");
	hostname1=name.substring(0,index);
	hostname2=name.substring(index+1,name.length+1);
	if(hostname2 != "3322.org") {
	    $("#error_msg").html("主机名要以 .3322.org 结尾！");
	    return false;
	}
    }	
    */
	return true;
}
function checkData(){
	var f=DDNSConfig;
	var SerName=getListValue(f.DDNSProvider);

	if(SerName!="none"){
		var maxpasslen=((SerName=="iplink.com.cn" || SerName=="care.com")?63:31);
		if (SerName != "www.oray.net") {
		    if (!checkEnglishName_P(f.DDNS,31,"<%write(ddn_host);%>",0)) return false;
		}
		if (!checkDDNS(f.DDNS,SerName)) return false;
		if(SerName != "care.com") {
		    if (!checkName_P(f.Password,maxpasslen,passtbl.innerHTML,0)) return false;
		    if (!checkEnglishName_P(f.Password,maxpasslen,"<%write(com_passwd);%>",0)) return false;
		}
		else
		{
		    if (f.DDNS.value.indexOf(".tcare.com") < 0){
			f.DDNS.value = f.DDNS.value+".care.com";
		    }
		    if(!f.default_uttcare.checked)
		    {
			if (!checkEnglishName_P(f.Password,maxpasslen,"<%write(com_passwd);%>",0)) return false;
			if (!checkName_P(f.Password,maxpasslen,passtbl.innerHTML,0)) return false;
		    }
		}
		if(SerName=="3322.org") if (!checkName_P(f.Account,31,"<%write(com_user);%>",0)) return false;
		
	}
	return true;
}
function save(){
	if(checkData())	{
	    var f=DDNSConfig;
	    DDNSConfig.action="/goform/DDNS";
	    DDNSConfig.submit();
	}
}

function selUttcare(){
	var f=DDNSConfig;
	if(f.default_uttcare.checked)
	{
	    f.Account.value="";
	    f.DDNS.value=productIDs+".care.com";
	    f.Password.value="";
	    $("#pwd").hide();
	    $("input[name='DDNS']").prop("readonly",true);
	}
	else
	{
	    f.Account.value="";
	    f.DDNS.value="";
	    f.Password.value="";
	    $("#pwd").show();
	    $("input[name='DDNS']").prop("readonly",false);
	    if((f.DDNSProvider.value==DDNSProviders)&&(DDNSs!=productIDs+".care.com")){
		f.Account.value=Accounts;
		f.DDNS.value=DDNSs;
		f.Password.value=Passwords;
	    }
	}
}

function selService(val){
	var f=DDNSConfig;
	f.Account.value="";
	f.DDNS.value="";
	f.Password.value="";
		

	if (val == "iplink.com.cn")
	{
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		$("#ddnsurl").html('<span id="tips"><%write(ddn_iplinkwarn);%></span>');
		$("input[name='DDNS']").prop("readonly",false);
 		$("#ddnsurl").show();
		$("#ddnsname").show();
		$("#username").hide();
		$("#pwd").show();
		$("#registername").show();
		$("#defultcheck").hide();
	}
	else if(val=="3322.org"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		$("#ddnsurl").html('<a href="http://www.3322.org" target="_blank" style="color:#2c3091;" ><%write(ddn_regDns);%>：http://www.3322.org</a>');
		$("input[name='DDNS']").prop("readonly",false);
 		$("#ddnsurl").show();
		$("#ddnsname").show();
		$("#username").show();
		$("#pwd").show();
		$("#registername").hide();
		$("#defultcheck").hide();
	} else if (val=="www.oray.net") {
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		//$("input[name='DDNS']").prop("readonly",true);
		//$("input[name='DDNS']").val("PhLinux3.Oray.Net");
		$("#ddnsurl").html('<a href="http://hsk.oray.com/" target="_blank" style="color:#2c3091;" ><%write(ddn_regDns);%>：http://hsk.oray.com/</a>');
 		$("#ddnsurl").show();
		$("#ddnsname").hide();
		$("#username").show();
		$("#pwd").show();
		$("#registername").hide();
		$("#defultcheck").hide();
	
	}
	else if(val=="dyndns.org"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		$("#ddnsurl").html('<a href="http://www.dyndns.org" target="_blank" style="color:#2c3091;" ><%write(ddn_regDns);%>：http://www.dyndns.org</a>');
		$("input[name='DDNS']").prop("readonly",false);
 		$("#ddnsurl").show();
		$("#ddnsname").show();
		$("#username").show();
		$("#pwd").show();
		$("#registername").hide();
		$("#defultcheck").hide();
	}
	else if(val=="care.com"){
		if((DDNSProviders=="care.com")&&(DDNSs!=productIDs+".care.com"))
		{
		    f.default_uttcare.checked = 0;
		}
		else
		{
		    f.default_uttcare.checked = 1;
		}
		selUttcare();
		$("#ddnsurl").html('<a href="http://www..com.cn/ddns" target="_blank" style="color:#2c3091;" ><%write(ddn_regDns);%>：http://www.com.cn/ddns</a>');
 		$("#ddnsurl").show();
		$("#ddnsname").show();
		$("#username").hide();
		$("#registername").hide();
		$("#defultcheck").show();
	}
	else if(val=="no-ip.com"){
		if(val==DDNSProviders){
			f.Account.value=Accounts;
			f.DDNS.value=DDNSs;
			f.Password.value=Passwords;
		}
		$("#ddnsurl").html('<a href="http://www.no-ip.com" target="_blank" style="color:#2c3091;" ><%write(ddn_regDns);%>：http://www.no-ip.com</a>');
		$("input[name='DDNS']").prop("readonly",false);
 		$("#ddnsurl").show();
		$("#ddnsname").show();
		$("#username").show();
		$("#pwd").show();
		$("#registername").hide();
		$("#defultcheck").hide();
	}

	else{
		$("#ddnsurl").hide();
		$("#ddnsname").hide();
		$("#username").hide();
		$("#pwd").hide();
		$("#registername").hide();
		$("#defultcheck").hide();
		
	}
}
$(function(){
    init();
});

</script>
<body>
<div class="warp">

<br>
	<form name="DDNSConfig" action="" method="post">
	<table border="0" cellpadding="0" cellspacing="1"  id="runstatusInfo" class="group" style="background:#5e5e5e;" >
    
         <tr class="tab_title">
        	<td colspan="3"><%write(ddns_dynConfig);%></td>
           
        </tr>
        <tr class="tab_info">
        	<td width="130px"><%write(ddn_service);%></td>
            <td width="470px;" colspan="2">
            	<select name="DDNSProvider" id="DDNSProvider" class="select"  style="width:140px" onChange="selService(this.value)">
                    <option value="none" id="none"><%write(ddn_none);%></option>
					<!--<option value="iplink.com.cn">iplink.com.cn</option>-->
                    <option value="3322.org">3322.org</option>
                    <option value="www.oray.net"><%write(ddns_peaShell);%></option>
                    <!--<option value="care.com">care.com</option>-->
        		</select>
                <span id="ddnsurl">
</span>
            </td>
        </tr>
        <tr class="tab_info" id="defultcheck">
        	<td><%write(ddns_defultUttcare);%></td>
            <td colspan="2">
		<input type="checkbox" name="default_uttcare" id="default_uttcare" value=1 onClick="selUttcare()">
            </td>
        </tr>
        <tr class="tab_info" id="registername">
        	<td><%write(ddns_regID);%></td>
            <td colspan="2" id="registerNum">
            </td>
        </tr>
	
        <tr class="tab_info" id="ddnsname" >
        	<td><%write(ddn_host);%></td>
            <td colspan="2">
            	<input type="text" name="DDNS" class="txt-input"  maxlength="31" style="width:200px">
            </td>
        </tr>
        <tr class="tab_info" id="username">
        	<td><%write(com_user);%></td>
            <td colspan="2">
            	<input type="text" name="Account" size="20" class="txt-input"  maxlength="31" style="width:200px">
            </td>
        </tr>
        <tr class="tab_info" id="pwd">
        	<td id="passtbl"></td>
            <td colspan="2">
            	<input name="Password" type="password" size="20" class="txt-input" maxlength="64" style="width:200px">     <input type="hidden" name="Profile" value="WAN1" />
            </td>
        </tr>
  
		 <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" width="550">
                	<tr>
                    	<td width="170px">&nbsp;</td>
                        <td align="left"><span id="error_msg"  style="color:red;"></span></td>
                        <td align="right"><input type="button" class="btn" name="back" onclick="save();"  value="<%write(com_save);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
    </table>
    
    	<table border="0" cellpadding="0" cellspacing="1" class="tab_list" style="background:#5e5e5e;" >
    
         <tr class="tab_title">
        	<td colspan="4"><%write(ddns_dynStat);%></td>
           
        </tr>

        <tr  class="tab_info_list">
        	<th><%write(ddn_upState);%></th>
            <th><%write(ddn_host);%></th>
            <th><%write(com_IP);%></th>
            <th><%write(ddn_upTime);%></th>
        </tr>
  		<tr  class="tab_info_list">
        	<td id="updatestatus"></td>
            <td id="hostname"></td>
            <td id="ipadd"></td>
            <td id="updatetime"></td>
        </tr>
   
  
		 <tr  height="60px" class="tab_savetd"> 
        	<td colspan="4" align="center">
            	<table cellpadding="0" cellspacing="1" border="0" width="550"  >
                	<tr>
                    	<td width="170px">&nbsp;</td>
                        <td align="left"></td>
                        <td align="right"><input type="button" class="btn" name="back" onclick="javascript:window.location.reload();"  value="<%write(com_fresh);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>
    </table>
    </form>
<br>
<br>
</div>
</body>
</html>
