<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("VPNSui");%>
<title>VPN</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="script/jquery.timers-1.2.js" ></script>
<script type="text/javascript" src="script/jquery.corner.js" ></script> 
<script type="text/javascript" src="script/frame.js"></script>
<script type="text/javascript">
var PptpEnables = 0;
var errorstr = "";
var PptpClientEbls = "off";
var L2tpClientEbls = "off";
var PptpClientNatEbls = "off";
var setNms = "";
var oldNms = "";
var userNms = "";
var passws = "";
var useTms = "-";
var vpnMds = "0";
var authTys = "";
var encryptionMds = "";
var serverLIps = "";
var serverLNMs = "";
var serverIs = "";
var mts = 1492;
var started=0;
var errStr = "";
var rightStr="";
var EncryptionModes = "None";
function initPptpPageVar()
{
	<%aspPptpClientOneConfig("VPN_PPTP");%>
	PptpClientEbls = PPTPClientEnables;
	PptpEnables = (PPTPClientEnables = "on"?1:0);
	setNms = "VPN_PPTP";
	oldNms = setNames;
	userNms=userNames;
	passws = passwds;
	useTms = useTimes;
	authTys=AuthTypes;
	if(authTys == "" || authTys == "EITHER") authTys = "THRIN";
	serverLIps = serverLanIps;
	serverLNMs=serverLanNetMasks;
	serverIs=serverIps;
	encryptionMds=EncryptionModes;
	vpnMds=vpnModes;
	if(encryptionMds=="") encryptionMds="None";
	mts=mtu;
	if((errorstr=="<%write(vpn_errmsg);%>") && (oldNms==""))
	{
	    vpnMds=2;
	    errStr="";
	}
	else errStr=errorstr;
	rightStr=rightstr;
}
function initL2tpPageVar()
{
	<%OutputL2tpCliAccountData("VPN_L2TP");%>
	L2tpClientEbls = L2TPClientEnables;
	PptpEnables = (L2TPClientEnables = "on"?1:0);
	setNms = "VPN_L2TP";
	oldNms = setNames;
	userNms=userNames;
	passws = passwds;
	useTms = useTimes;
	authTys=AuthTypes;
	if(authTys == "") authTys = "EITHER";
	serverLIps = serverLanIps;
	serverLNMs=serverLanNetMasks;
	serverIs=serverIps;
	encryptionMds=EncryptionModes;
	mts=mtu;
	vpnMds=vpnModes;
	if((errorstr=="<%write(vpn_errmsg);%>") && (oldNms==""))
	{
	    vpnMds=2;
	    errStr="";
	}
	else errStr=errorstr;
	
	if(SmartModevpnModes ==1)
	{
		$("#smartModeSel").show();
	}
	else
	{
		$("#smartModeSel").hide();
	}
}
function init(){
	initPptpPageVar();
	initL2tpPageVar();
	var f=Pptp_Config;
	var switch_b = $(".on-off").children("button");
	var type=$("#type option:selected").text();
	if(started == 0)
	{
	    if(L2tpClientEbls == "on") 
	    {
		f.type.options[1].selected = true;
		type = "L2TP";
	    }
	    else 
	    {
		f.type.options[0].selected = true;
		type = "PPTP";
	    }
	}
	if(type == "PPTP")
	{
	    initPptpPageVar();
	    initEnabledButton(switch_b,PptpClientEbls);
	}
	else
	{
	    initEnabledButton(switch_b,L2tpClientEbls);
	}
	f.PPTPClientEnable.value=PptpClientEbls;
 	f.PPTPClientNATEnable.value="on";
	f.L2TPClientEnable.value=L2tpClientEbls;
 	f.setName.value=setNms;
	f.oldSetName.value=oldNms;
	f.Action.value="modify"; 
	f.userName.value=userNms;
	f.passwd.value=passws;
	f.serverLanNetMask.value=serverLNMs;

	f.vpnMode.value=vpnMds;
	f.AuthTypePptp.value=authTys;
	f.AuthTypeL2tp.value=authTys;
	f.EncryptionMode.value=encryptionMds;
	f.serverIp.value=serverIs;
	f.serverLanIp.value=serverLIps;
	f.MTU.value=mts;
	
	$("#vpnEnable").val(PptpEnables);	
	$("#error_msg").html(errStr);
	selectMode();
	showtime();
}
//PPTP&L2TP连接
function dailData(){
	var f=Pptp_Config;
	var type=$("#type option:selected").text();
	if(type=="PPTP") //PPTP连接
	{
		if(useTms != "-" && useTms != "")
		{
		    $("#error_msg1").html("<%write(vpn_msg1);%>");
		    return false;
		}
		if(($("#vpnEnable").val()=="1") && (PptpClientEbls == "on"))
		{
		$("#error_msg1").html("<%write(vpn_connecting);%>");
		f.dialstr.value="VPN_PPTP";
		f.action="/goform/formDia";
		f.submit();
		}
		else
		{
		$("#error_msg1").html("<%write(vpn_Config_not_on);%>");
		return false;
		}
	}
	else
	{
		if(useTms != "-" && useTms != "")
		{
		    $("#error_msg1").html("<%write(vpn_msg1);%>");
		    return false;
		}
		if(($("#vpnEnable").val()=="1") && (L2tpClientEbls == "on"))
		{
		$("#error_msg1").html("<%write(vpn_connecting);%>");
		f.dialstr.value="VPN_L2TP";
		f.action="/goform/formL2tpDial";
		f.submit();
		}
		else
		{
		$("#error_msg1").html("<%write(vpn_Config_not_on);%>");
		return false;
		}
	}
	return true;
}
//PPTP&L2TP挂断
function hangData(){
	var f=Pptp_Config;
	var type=$("#type option:selected").text();
	if(type=="PPTP"){  //PPTP挂断
	    if(($("#vpnEnable").val()=="1") && (PptpClientEbls == "on"))
	    {
		if(useTms == "-" || useTms == "") $("#error_msg1").html("<%write(vpn_msg2);%>");
		else
		{
		    $("#error_msg1").html("<%write(vpn_disconnecting);%>");
		    f.hangstr.value="VPN_PPTP";
		    f.hangstrsrv.value="";
		    f.action="/goform/formhang";
		    f.submit();
		}
	    }
	    else
	    {
		$("#error_msg1").html("<%write(vpn_Cpnfig_off);%>");
		return false;
	    }
	}
	else //L2TP挂断
	{
	    if(($("#vpnEnable").val()=="1") && (L2tpClientEbls == "on"))
	    {
		if(useTms == "-" || useTms == "") $("#error_msg1").html("<%write(vpn_msg2);%>");
		else
		{
		    $("#error_msg1").html("<%write(vpn_disconnecting);%>");
		    f.hangstr.value="VPN_L2TP";
		    f.hangstrsrv.value="";
		    f.action="/goform/formL2tpHang";
		    f.submit();
		}
	    }
	    else
	    {
		$("#error_msg1").html("<%write(vpn_Cpnfig_off);%>");
		return false;
	    }
	}
	return true;
}
//高级选项
var SeniorShowEn=0;
function clickSenior(frm) {
		if(SeniorShowEn == 0) {
			SeniorShowEn = 1;
		} else {
			SeniorShowEn = 0;
		}
		var type=$("#type option:selected").text();
		if(type=="PPTP"){
			showL2tp(0);
			showPptp(SeniorShowEn);	
			parent.art.dialog.list["func_box"].size(700,$("#Pptp").height());
		}
		if(type=="L2TP"){
			showPptp(0);
			showL2tp(SeniorShowEn);
			parent.art.dialog.list["func_box"].size(700,$("#Pptp").height());
		}	
		selectMode();
}
//PPTP
function showPptp(isShow)
{
			showTable(model, isShow);
			showTable(site, isShow);
			showTable(mask, isShow);
			showTable(pwdcheck, isShow);
			showTable(encryption, isShow);
			showTable(mtus, isShow);		
}
//L2TP
function showL2tp(isShow)
{
			showTable(model, isShow);
			showTable(site, isShow);
			showTable(mask, isShow);
			showTable(repwdcheck, isShow);
}
function showtab()
{
    init();
    var type=$("#type option:selected").text();
    if(type=="PPTP"){
	showL2tp(0);
	showPptp(SeniorShowEn);	
	parent.art.dialog.list["func_box"].size(700,$("#Pptp").height());
    }
    if(type=="L2TP"){
	showPptp(0);
	showL2tp(SeniorShowEn);
	parent.art.dialog.list["func_box"].size(700,$("#Pptp").height());
    }	
    selectMode();
}

//数据格式验证
function checkData(){
	var f=Pptp_Config;
	var type=$("#type option:selected").text();
	if (!checkName_P(f.serverIp,31,"<%write(vpn_IP);%>",0)) return false;
	if (!checkName_P(f.userName,32,"<%write(com_user);%>",0)) return false;
	if (!checkName_P(f.passwd, 31,"<%write(com_passwd);%>", 0)) return false;
	if (!checkIP_P(f.serverLanIp,"<%write(vpn_destIP);%>",0)) return false;
	if (!checkMask_P(f.serverLanNetMask,"<%write(vpn_destMask);%>",0)) return false;
	if((f.EncryptionMode.value == "MPPE") && (f.AuthTypePptp.value != "MS-CHAPV2"))
 	{
		$("#error_msg").html("<%write(vpn_msg3);%>");
		return false;
 	}
	if(type=="PPTP")
	{
	    if(!checkNumber_P(f.MTU,1,1492,"MTU",0)) return false;
	}
	if(f.vpnMode.value == "1")
	{
	    if(f.serverLanIp.value == "0.0.0.0" || f.serverLanNetMask.value == "0.0.0.0")
	    {
		$("#error_msg").html("<%write(vpn_msg6);%>");
		return false;
	    }
	}
	return true;
}

function checkConnStatus()
{
    var type=$("#type option:selected").text();
    if(type=="PPTP")
    {
	$.ajax({
	    url:"/goform/formPPTPConnStatus?timestamp="+(new Date()).getTime(),
	    type:"post",
	    cache:false,
	    success:function(resp){
		eval(resp);
		useTms = connStatus;
		showtime(); 
	    }
	});
    }
    else
    {
	$.ajax({
	    url:"/goform/formL2TPConnStatus?timestamp="+(new Date()).getTime(),
	    type:"post",
	    cache:false,
	    success:function(resp){
		eval(resp);
		useTms = connStatus;
		showtime(); 
	    }
	});
    } 
}


function showtime()
{
    if(useTms=="connecting")
    {
	$("#error_msg1").html("<%write(vpn_connecting);%>");
    }
    else if (useTms!="-" &&  useTms!="")
    {
	var runday=parseInt((eval(useTms)/3600)/24);
	var runhour=parseInt((eval(useTms)/3600)%24);
	var runmin=parseInt((eval(useTms)%3600)/60);
	var runsec=parseInt((eval(useTms)%3600)%60);
	var time = runday+"<%write(com_day);%>"+runhour+"<%write(com_hour);%>"+runmin+"<%write(com_minute);%>"+runsec+"<%write(com_second);%>";
	$("#error_msg1").html(time);
    }
    else
    {
	$("#error_msg1").html("<%write(com_interrupt);%>");
    }
}

function selectMode()
{
    var f=Pptp_Config;
    if(f.vpnMode.value != "1")
    {
	serverLIps = f.serverLanIp.value;
	serverLNMs = f.serverLanNetMask.value;
	f.serverLanIp.value="0.0.0.0";
	f.serverLanNetMask.value="0.0.0.0";
	showTable(site, 0);
	showTable(mask, 0);
	parent.art.dialog.list["func_box"].size(700,$("#Pptp").height());
    }
    else
    {
	f.serverLanIp.value=serverLIps;
	f.serverLanNetMask.value=serverLNMs;
	showTable(site, SeniorShowEn);
	showTable(mask, SeniorShowEn);
	parent.art.dialog.list["func_box"].size(700,$("#Pptp").height());
    }
}

//保存 提交
function savePptp_L2tp(){
    var f=Pptp_Config;
    var type=$("#type option:selected").text();
    /*if(useTms == "connecting")
    {
	$("#error_msg").html("正在拨号中，请先点击挂断！");
	return false;
    }*/
    if(type=="PPTP")
    {
	f.AuthType.value=f.AuthTypePptp.value;
	if(checkData()){
	    $("#error_msg").html("<%write(vpn_saving);%>");
	    f.action="/goform/formPptpClientConfig";
	    f.submit();
	}
    }
    else
    {
	f.AuthType.value=f.AuthTypeL2tp.value;
	if(checkData()){
	    $("#error_msg").html("<%write(vpn_saving);%>");
	    f.action="/goform/l2tpCliAccountConfig";
	    f.submit();
	}
    }
}
$(function(){
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		var f=Pptp_Config;
		if(started == 1)
		{
		    if(status_now=="on")
		    {
			$("#vpnEnable").val(0);
			f.PPTPClientEnable.value="off";
			f.L2TPClientEnable.value="off";
			swicth_act(status_now);
		    }
		    else
		    {
			var type=$("#type option:selected").text();
			if(type=="PPTP")
			{
			    if(L2tpClientEbls == "on")
			    {
				$("#error_msg1").html("<%write(vpn_msg4);%>"); 
				$("#vpnEnable").val(0);
				f.PPTPClientEnable.value="off";
			    }
			    else
			    {
				$("#vpnEnable").val(1);
				f.PPTPClientEnable.value="on";
				swicth_act(status_now);
			    }
			}
			else
			{
			    if(PptpClientEbls == "on")
			    {
				$("#error_msg1").html("<%write(vpn_msg5);%>"); 
				$("#vpnEnable").val(0);
				f.L2TPClientEnable.value="off";
			    }
			    else
			    {
				$("#vpnEnable").val(1);
				f.L2TPClientEnable.value="on";
				swicth_act(status_now);
			    }
			}
		    }
		}
	});
	showSuccessTips(rightStr);
	started=1;
	$('body').everyTime('1s','TimerB',function(){
		checkConnStatus();   
		},0,true);
});
</script>
</head>

<body>
<div id="Pptp" style="width:600px; margin:0 auto; text-align:center;"><br/>
  <form action="" mothod="post" name="Pptp_Config">
    <table border="0" cellpadding="0" cellspacing="1" id="pptpInfo" style="background:#5e5e5e;" class="group" width="600px;">
      <tr class="tab_info">
        <td style="width:130px;"><%write(com_ConnectionType);%></td>
        <td colspan="2"><select style="width:180px;font-size: 14px;color: #383838;font-family: Microsoft Yahei;" name="type" id="type" onChange="showtab()">
            <option selected="selected">PPTP</option>
            <option>L2TP</option>
          </select>
	</td>
      </tr>
      <tr class="tab_info">
        <td style="width:130px;"><%write(vpn_open);%></td>
        
        <input type="hidden" name="Action"/>
        <input type="hidden" name="PPTPClientEnable"/>
        <input type="hidden" name="PPTPClientNATEnable"/>
        <input type="hidden" name="L2TPClientEnable"/>
        <input type="hidden" name="setName"/>
        <input type="hidden" name="oldSetName"/>
	<input type="hidden" name="dialstr"/>
	<input type="hidden" name="hangstr"/>
	<input type="hidden" name="hangstrsrv"/>
	<input type="hidden" name="AuthType"/>
        
        <td colspan="2"><span class="on-off">&nbsp;
          <button type="button" class="on"></button>
          <input type="hidden" name="vpnEnable" id="vpnEnable" value=0 />
          </span></td>
      </tr>
      <tr class="tab_info">
        <td style="width:130px;"><%write(com_ConnectionStatus);%></td>
        <td align="left" colspan="2" width="450px">
        	<input type="button" style=" background-color:transparent;border:0px;text-decoration:underline;color:blue;cursor:pointer;font-size:14px;font-family: Microsoft Yahei;" value="<%write(vpn_connect);%>" onClick="dailData()"/>
   			&nbsp;&nbsp;
           <input type="button" style=" background-color:transparent;border:0px;text-decoration:underline;color:blue;cursor:pointer;" value="<%write(com_interrupt);%>" onClick="hangData()"/>
   			&nbsp;&nbsp;&nbsp;&nbsp;
        <span id="error_msg1"  style="color:red;"></span>
   		</td>
      </tr>
      <tr class="tab_info">
        <td style="width:130px;"><%write(vpn_IP);%></td>
        <td style="width:250px;" colspan="2"><input type="text" class="txt-input" name="serverIp" size="18"  maxlength="31" value="" onChange="validateName(this,31,'<%write(vpn_IP);%>',0);"/></td>
      </tr>
      <tr class="tab_info">
        <td style="width:130px;"><%write(com_user);%></td>
        <td style="width:250px;" colspan="2"><input type="text" class="txt-input" name="userName" size="18"  maxlength="32" onChange="validateName(this,11,'<%write(com_user);%>',0);"  value=""/></td>
      </tr>
      <tr class="tab_info">
        <td style="width:130px;"><%write(com_passwd);%></td>
        <td style="width:250px;" colspan="2"><input type="password" class="txt-input" name="passwd" size="19"   maxlength="31" value=""onChange="validateName(this,31,'<%write(com_passwd);%>',0);"/></td>
      </tr>
      <tr class="tab_info">
        <td height="30" align="right" colspan="3"><div style="position:relative; z-index:2;"> <a href="javascript:clickSenior(Pptp_Config)" style="color:#000000;text-decoration:underline;"> <strong><%write(com_advance);%></strong> </a> </div></td>
      </tr>
      <tr class="tab_info" style="display:none;" id="model">
        <td style="width:130px;"><%write(com_mode);%></td>
        <td colspan="2"><select style="width:180px;font-size:14px;color: #383838;font-family: Microsoft Yahei;" name="vpnMode" id="vpnMode" onChange="selectMode()">
            <option value="0"><%write(vpn_mode1);%></option>
            <option value="1"><%write(vpn_mode2);%></option>
            <option value="2" id="smartModeSel"><%write(vpn_mode3);%></option>
            
          </select></td>
      </tr>
      <tr class="tab_info" style="display:none;" id="site">
        <td style="width:130px;"><%write(vpn_destIP);%></td>
        <td style="width:250px;" colspan="2"><input type="text" class="txt-input" name="serverLanIp" size="18" maxlength="15" onKeyPress="ipOnly_P(event)" onChange="validateIP(this,'<%write(vpn_destIP);%>',0);" value=""/></td>
      </tr>
      <tr class="tab_info" style="display:none;" id="mask">
        <td style="width:130px;"><%write(vpn_destMask);%></td>
        <td style="width:250px;" colspan="2"><input type="text" class="txt-input" name="serverLanNetMask" size="18" maxlength="15" onKeyPress="ipOnly_P(event)"  onChange="checkMask_P(this,'<%write(vpn_destMask);%>',0);"  value=""/></td>
      </tr>
      
      <!--PPTP-->
      <tr class="tab_info" style="display:none;" id="pwdcheck">
        <td style="width:130px;"><%write(com_checkPasswd);%></td>
        <td colspan="2"><select style="font-size: 14px;color: #383838;font-family: Microsoft Yahei;" name="AuthTypePptp" id="authType">
            <option value="THRIN" selected="selected">ANY</option>
            <option value="PAP">PAP</option>
            <option value="CHAP">CHAP</option>
            <option value="MS-CHAPV2">MS-CHAPV2</option>
          </select></td>
      </tr>
      <tr class="tab_info" style="display:none;" id="encryption">
        <td style="width:130px;"><%write(vpn_encryption);%></td>
        <td colspan="2"><select style="font-size:14px;color: #383838;font-family: Microsoft Yahei;" name="EncryptionMode" id="encryt">
            <option value='None' selected="selected"><%write(vpn_encryption_none);%></option>
            <option value='MPPE'><%write(vpn_encryption_MPPE);%></option>
          </select></td>
      <tr class="tab_info" style="display:none;" id="mtus">
        <td style="width:130px;">MTU</td>
        <td style="width:250px;" colspan="2"><input type="text" class="txt-input" name="MTU" size="18"  maxlength="4" value="" onKeyPress="figureOnly()" onChange="validateNumber(this,'1','1492','MTU',0);"/>
          &nbsp;&nbsp;&nbsp;(1-1492) </td>
      </tr>
      <!--  L2TP -->
      <tr class="tab_info" style="display:none;" id="repwdcheck">
        <td style="width:130px;"><%write(com_checkPasswd);%></td>
        <td colspan="2"><select name="AuthTypeL2tp">
            <option value="EITHER" selected="selected">EITHER</option>
            <option value="CHAP">CHAP</option>
            <option value="PAP">PAP</option>
            <option value="NONE">NONE</option>
          </select></td>
      </tr>
      <tr height="60px" class="tab_info">
        <td colspan="3"><table cellpadding="0" cellspacing="0" border="0" width="600">
            <tr>
              <td align="left" width="320px" colspan="2" style="text-align:center"><span id="error_msg"  style="color:red;"></span></td>
              <td align="right"><input type="button" class="btn" name="save" id="savebtn" value="<%write(com_save);%>"  onClick="savePptp_L2tp()" /></td>
            </tr>
          </table></td>
      </tr>
    </table>
  </form>
  <br/>
</div>
</body>
</html>
