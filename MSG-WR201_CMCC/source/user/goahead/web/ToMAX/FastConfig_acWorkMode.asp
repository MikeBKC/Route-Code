 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FastConfig");%>
<%langTran("acMode");%>
<%langTran("commonWords");%>

<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">



<%
	aspOutPutAcWorkMode();
%>

function selMode(val)
{
	var f=acWorkMode;
	if(val == "0")
	{
		gateWay.style.display="none";
		dns.style.display="none";
	}else if (val == "1")
	{
		gateWay.style.display=dis;
		dns.style.display=dis;
	}
	else{
		gateWay.style.display="none";
		dns.style.display="none";
	}

}

function init()
{
	var f = acWorkMode;

	if(f.modifyFlag1.value == "change")
	{
		f.acMode_new.value ="<% write(Form("acMode"));%>" ;
		f.acGateWay_new.value="<% write(Form("acGateWay"));%>" ; 
		f.acDns_new.value="<% write(Form("acDns"));%>" ;
	}
	else
	{
		f.acMode_new.value = acMode;
		f.acGateWay_new.value=ac_gateWay; 
		f.acDns_new.value=ac_Dns;
	}
	selMode(f.acMode_new.value);
}

function Reset(){
	acWorkMode.reset();
        acWorkMode.acMode.value = "";
	init();
	for(i=1;i<=2;i++){
		document.all["yutd"+i].innerHTML="";
	}
}
function jumpthere1()
{
    acWorkMode.action="FastConfig.asp";
    acWorkMode.submit();
}
function jumpthere2()
{
    acWorkMode.action="FastConfig_acWorkMode.asp";
    acWorkMode.submit();
}
function jumpthere3()
{
    acWorkMode.action="FastConfig_Net_WAN1.asp";
    acWorkMode.submit();
}
function jumpthere4()
{
    acWorkMode.action="FastConfig_edit_Vlan.asp";
    acWorkMode.submit();
}
function jumpthere5()
{
    acWorkMode.action="FastConfig_edit_DhcpPool.asp";
    acWorkMode.submit();
}
function jumpthere6()
{
    acWorkMode.action="FastConfig_edit_SZ.asp";
    acWorkMode.submit();
}
function jumpthere7()
{
    acWorkMode.action="FastConfig_MacFilterGloable_Config.asp";
    acWorkMode.submit();
}
//function jumpthere8()
//{
//    acWorkMode.action="apManage.asp";
//    acWorkMode.submit();
//}


function dopre(){

	var f=acWorkMode;

	f.acMode.value = f.acMode_new.value;	
	f.acDns.value = f.acDns_new.value;		   
	f.acGateWay.value = f.acGateWay_new.value;		   
	f.modifyFlag1.value = "change";	

    acWorkMode.action = "FastConfig.asp";
    acWorkMode.submit();
}



function donext(){
	
	var f=acWorkMode;

	f.acMode.value = f.acMode_new.value;	
	f.acDns.value = f.acDns_new.value;		   
	f.acGateWay.value = f.acGateWay_new.value;		   
	f.modifyFlag1.value = "change";	

	if(f.acMode.value == "0")
	{
	    f.action = "FastConfig_Net_WAN1.asp";
	}
	else
	{ 
       	if (!checkIP(f.acGateWay_new,"<%write(acGateWayAddr);%>",0)) return false;
       	if (!checkMainDns(f.acDns_new,"<%write(acDnsSer);%>",0)) return false;
		f.action="FastConfig_edit_Vlan.asp";
	}	
    f.submit();
}

function done(){
	
	var f=acWorkMode;

	f.acMode.value = f.acMode_new.value;	
	f.acDns.value = f.acDns_new.value;		   
	f.acGateWay.value = f.acGateWay_new.value;		   
	f.modifyFlag1.value = "change";	

	if(f.acMode.value == "0")
	{
		f.pageAction.value = "2"; 
		f.action="/goform/formConfigFastDirectionAC";
		f.submit();
	}
	else
	{ 
       	if (!checkIP(f.acGateWay_new,"<%write(acGateWayAddr);%>",0)) return false;
       	if (!checkMainDns(f.acDns_new,"<%write(acDnsSer);%>",0)) return false;

		f.pageAction.value = "2"; 
		f.action="/goform/formConfigFastDirectionAC";
		f.submit();
	}	
}


function dojump(){

	var f=acWorkMode;
	f.acMode.value = f.acMode_new.value;	
	f.acDns.value = f.acDns_new.value;		   
	f.acGateWay.value = f.acGateWay_new.value;		   
	f.modifyFlag1.value = "";	

	if(f.acMode.value == "0")	    
	{
		f.action="FastConfig_Net_WAN1.asp";	
	}
	else
	{
		f.action="FastConfig_edit_Vlan.asp";
	}
		f.submit();

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
    <td height="80"><script language="javascript">showSecondTitle(_("<%write(fas_guideConfig);%>"));</script>
	<table width="100%" height="80" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#C5C5C5"><!--上半部分-->
	   <script language="javascript">showerror(errorstr);</script>
		    <tr>
		<!--	<td width="40"></td>
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
			</td>
			
			
		-->	
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
                            		<td width="54" valign="bottom" align="center"><span class="STYLE3">接入模式</span></td>
									<td width="33"></td>
                            		<td width="40" align="center" valign="bottom" class="STYLE5">WAN口</td>
									<td width="30"></td>
                            		<td width="55" align="center" valign="bottom" class="STYLE5">VLAN接口</td>
									<td width="18"></td>
                            		<td width="80" align="center" valign="bottom" class="STYLE5">DHCP服务器</td>
									<td width="15"></td>
                            		<td width="42" align="center" valign="bottom" class="STYLE5">服务区</td>
								<!--	<td width="8"></td>
                            		<td width="110" align="center" valign="bottom" class="STYLE5">无线MAC地址过滤</td>
				-->				<!--	<td width="7"></td>
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
									<div onclick="jumpthere1()" style="cursor:pointer;">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
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
									<div onclick="jumpthere2()"  style="cursor:pointer;">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
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
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                           			<td>
								<div onclick="jumpthere3()"  style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/3.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                            		<td>
									<div onclick="jumpthere4()"  style="cursor:pointer;"><table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/3.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                            		<td>
									<div onclick="jumpthere5()"  style="cursor:pointer;">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/3.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            		<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0"><!--横条-->
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table><!--横条-->
									</td>
                           			<td>
										<div onclick="jumpthere6()"  style="cursor:pointer;"><table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
                                      		<tr>
                                       		 <td align="center">
											 	<table width="21" height="22" border="0" background="images/3.png" cellpadding="0" cellspacing="0"><!--上圆图片-->
                                              	 <tr>
                                                 	<td>&nbsp;</td>
                                              	 </tr>
                                             	</table><!--上圆图片-->
											 </td>
                                     		 </tr>
										</table><!--底圆图片--></div>
									</td>
                            	<!--	<td>
										<table width="48" border="0" height="6" bgcolor="#D7D7D7" cellpadding="0" cellspacing="0">
                                     	 <tr>
                                       		 <td></td>
                                     	 </tr>
                                    	</table>
									</td>
                            		<td>
								<div onclick="jumpthere7()">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0">
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
									<div onclick="jumpthere8()">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0">
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
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center" bgcolor="#F3F3F3">
   <script language="javascript"></script>
   <tr><td width="100%" height="80"></td></tr>
  <tr valign="top">
    <td><form action="" name="acWorkMode" method="post">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td align="center" class="style7">
  <table border="0" width="600" align='center'>
  <tr><td width="102" height="20">&nbsp;</td></tr>
	   <tr>
	     <td colspan="4"><table border="0" width="100%"><tr>
	       <td width="40%" align="right" height="25" id="workMode_td"><%write(workMode);%></td>
	       <td width="3%">&nbsp;</td>
	       <td width="26%"><select name="acMode_new" id="acMode_new" class="list" onChange="selMode(this.value)" >
		     <option value="0" id="route_m"><%write(gwMode);%></option> 
	         <option value="1" id="bypass_m"><%write(bypassMode);%></option>
	         </select></td>
			 <td width="31%"></td>
	       </tr>
			<tr id="gateWay" name="gateWay" style="display:none">
				<td align="right"><%write(acGateWayAddr);%></td>
				<td></td>
				<td height="20"><input name="acGateWay_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(acGateWayAddr);%>'),yutd1)" maxlength="15"></td><td id="yutd1"></td>
			</tr>
			<tr id="dns" name="dns" style="display:none">
				<td align="right"><%write(acDnsSer);%></td>
				<td></td>
				<td height="20"><input name="acDns_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(acDnsSer);%>'),yutd2)" maxlength="15"></td><td id="yutd2"></td>
			</tr>
	     </table></td>
	     </tr>
	   <tr><td height="5">&nbsp;</td></tr>
<tr><td colspan="4"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
  <td  height="50" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="上一步" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="重填" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="离开" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="跳过" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="下一步" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="done()" value="完成" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="window.open('rhelpinfo.htm')" value="帮助" id="help"></td>

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
</table></td></tr>
  </table></td>
  </tr>
</table>
</form></td>
           <script language="javascript">init();</script>

  </tr>
</table>
  </td>
  </tr>
</table>
</body>
</html>


