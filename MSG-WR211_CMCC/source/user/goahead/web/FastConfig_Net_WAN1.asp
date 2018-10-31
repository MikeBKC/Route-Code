 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FastConfig");%>
<%langTran("FastConfig_Net");%>

<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">
//Butterlate.setTextDomain("admin");
var errorstr="" ;
<% aspFastConfigNet(1); %>
 
if(ConnTypes == "")
{
    ConnTypes = "DHCP" 
}
/*功能：检查是否合法用户名并返回提示信息,用户名可为全0*/
function re_checkName_allowZero(cntrl, tips) {
    var retips = "";
    var name = cntrl.value;
    if (name == "") {
            retips = tips + "<%write(com_null);%>";
            return retips;
    }
    if (name == null) {
            retips = tips + "<%write(com_null);%>";
            return retips;
    }

    if (name.indexOf(";") != -1 || name.indexOf(",") != -1 || name.indexOf("%") != -1 || name.indexOf("\"") != -1 || name.indexOf("&") != -1 || name.indexOf(" ") != -1 || name.indexOf("\'") != -1 || name.indexOf("\\") != -1 || name.indexOf("<") != -1 || name.indexOf(">") != -1) {
        retips = tips + "<%write(com_chkName);%>";
    }
    return retips;
}

function changeConnType(val){
    var f=FastConfig_Net_WAN1;
    	f.connectionType_new.value=val;
    if(val=="DHCP" || val=="3G"){
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
    var f=FastConfig_Net_WAN1;
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
    var f=FastConfig_Net_WAN1;
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
   var f=FastConfig_Net_WAN1;
   var providerId=f.ISP3G.selectedIndex;
   var providerVal=getListValue(f.ISP3G);
	 if(providerVal !=ISP3Gs){
	   f.ConnName.value=g3_para_arr[providerId][4];
	   f.DialNum.value=g3_para_arr[providerId][1];
	   f.PPPName.value=g3_para_arr[providerId][2];
	   f.PPPPasswd.value=g3_para_arr[providerId][3];
   }else{
   	//new Array("中国移动","*99***1#","","","CMNET"),/**/
   	 f.ConnName.value=ConnNames;
	 f.DialNum.value=DialNums;
	 f.PPPName.value=PPPNames;
	 f.PPPPasswd.value=PPPPasswds;
   }
}

function checkData(){
    var f=FastConfig_Net_WAN1;
    var indextext=getListValue(f.connectionType_new);
    	if(indextext=="STATIC"){
    	    if (f.staticIp.value=="0.0.0.0"){ alert(_("<%write(com_checkIPAlert);%>"));return false};
    	    if (!checkIP(f.staticIp,"<%write(com_IP);%>",0)) return false;
    	    if (!checkMask(f.staticNetmask,"<%write(com_NetMask);%>",0)) return false;
    	    if (!checkIP(f.staticGateway,"<%write(com_GateWayAddr);%>",0)) return false;
    	    if (!checkSameSubnet(f.staticIp.value,f.staticGateway.value,f.staticNetmask.value))
    	    {
    	    	alert(_("<%write(com_subnetAlert);%>"));
    	    	return false;
    	    }
			if(platform != "nv1800N")
			{
    	    	if (!checkMainDns(f.staticPriDns,"<%write(com_MDNS);%>",0)) return false;
    	    	if (!checkIP(f.staticSecDns,"<%write(com_SDNS);%>",1)) return false;
			}
    	}
    	else if(indextext=="PPPOE"){
    	    if (!checkEnglishName_allowZero(f.pppoeUser,31,"<%write(com_user);%>",0)) return false;
    	    if (!checkPassword1(f.pppoePass,"31",0)) return false;
    	}
    	return true;
}

function init(){
    var f=FastConfig_Net_WAN1;

	if(f.modifyFlag2.value == "change")
	{
	    changeConnType("<% write(Form("connectionType")); %>");
	    selVal(f.connectionType_new,"<% write(Form("connectionType")); %>");
	    var selIndex=f.connectionType_new.selectedIndex;
	    if(selIndex==1){//固定连接
	    	f.staticIp_new.value="<% write(Form("staticIp")); %>";
	    	f.staticNetmask_new.value="<% write(Form("staticNetmask")); %>";
	    	f.staticGateway_new.value="<% write(Form("staticGateway")); %>";
	    	f.staticPriDns_new.value="<% write(Form("staticPriDns")); %>";
	    	f.staticSecDns_new.value="<% write(Form("staticSecDns")); %>";
	    } else if(selIndex==2){//pppoe连接
	    	f.pppoeUser_new.value="<% write(Form("pppoeUser")); %>";
	    	f.pppoePass_new.value="<% write(Form("pppoePass")); %>";
	    }
	}
	else
	{
	    changeConnType(ConnTypes);
	    selVal(f.connectionType_new,ConnTypes);
	    var selIndex=f.connectionType_new.selectedIndex;
	    if(selIndex==1){//固定连接
	    	f.staticIp_new.value=IPs;
	    	f.staticNetmask_new.value=Masks;
	    	f.staticGateway_new.value=GateWays;
	    	f.staticPriDns_new.value=MainDnss;
	    	f.staticSecDns_new.value=SecDnss;
	    } else if(selIndex==2){//pppoe连接
	    	f.pppoeUser_new.value=UserNames;
	    	f.pppoePass_new.value=PassWds;
	    }

	}
}

function Reset(){
    FastConfig_Net_WAN1.reset();
    FastConfig_Net_WAN1.connectionType_new.value = "";
    init();
    for(i=1;i<=7;i++){
        document.all["yutd"+i].innerHTML="";
    }
}
 function jumpthere1()
{
     FastConfig_Net_WAN1.action="FastConfig.asp";
     FastConfig_Net_WAN1.submit();
}
 function jumpthere2()
{
     FastConfig_Net_WAN1.action="FastConfig_acWorkMode.asp";
     FastConfig_Net_WAN1.submit();
}

 function jumpthere3()
{
     FastConfig_Net_WAN1.action="FastConfig_Net_WAN1.asp";
     FastConfig_Net_WAN1.submit();
}

 function jumpthere4()
{
     FastConfig_Net_WAN1.action="FastConfig_edit_Vlan.asp";
     FastConfig_Net_WAN1.submit();
}

 function jumpthere5()
{
     FastConfig_Net_WAN1.action="FastConfig_edit_DhcpPool.asp";
     FastConfig_Net_WAN1.submit();
}

 function jumpthere6()
{
     FastConfig_Net_WAN1.action="FastConfig_edit_SZ.asp";
     FastConfig_Net_WAN1.submit();
}

 function jumpthere7()
{
     FastConfig_Net_WAN1.action="FastConfig_MacFilterGloable_Config.asp";
     FastConfig_Net_WAN1.submit();
}


// function jumpthere8()
//{
  //   FastConfig_Net_WAN1.action="apManage.asp";
    // FastConfig_Net_WAN1.submit();
//}

function dopre(){

    var f=FastConfig_Net_WAN1;

	f.connectionType.value=f.connectionType_new.value;
	f.staticIp.value=f.staticIp_new.value;
	f.staticNetmask.value=f.staticNetmask_new.value;
	f.staticGateway.value=f.staticGateway_new.value;
	f.staticPriDns.value=f.staticPriDns_new.value;
	f.staticSecDns.value=f.staticSecDns_new.value;
	f.pppoeUser.value=f.pppoeUser_new.value;
	f.pppoePass.value=f.pppoePass_new.value;
	f.modifyFlag2.value = "change";
    f.action = "FastConfig_acWorkMode.asp";
    f.submit();
}

function donext(){
    
	var f=FastConfig_Net_WAN1;

	f.connectionType.value=f.connectionType_new.value;
	f.staticIp.value=f.staticIp_new.value;
	f.staticNetmask.value=f.staticNetmask_new.value;
	f.staticGateway.value=f.staticGateway_new.value;
	f.staticPriDns.value=f.staticPriDns_new.value;
	f.staticSecDns.value=f.staticSecDns_new.value;
	f.pppoeUser.value=f.pppoeUser_new.value;
	f.pppoePass.value=f.pppoePass_new.value;
	f.modifyFlag2.value = "change";

	if(checkData()){
			
		f.action = "FastConfig_edit_Vlan.asp";
		f.submit();
	}	
}

function done(){
    
	var f=FastConfig_Net_WAN1;

	f.connectionType.value=f.connectionType_new.value;
	f.staticIp.value=f.staticIp_new.value;
	f.staticNetmask.value=f.staticNetmask_new.value;
	f.staticGateway.value=f.staticGateway_new.value;
	f.staticPriDns.value=f.staticPriDns_new.value;
	f.staticSecDns.value=f.staticSecDns_new.value;
	f.pppoeUser.value=f.pppoeUser_new.value;
	f.pppoePass.value=f.pppoePass_new.value;
	f.modifyFlag2.value = "change";

	if(checkData()){
			
		f.pageAction.value = "3"; 
		f.action="/goform/formConfigFastDirectionAC";
		f.submit();
	}	
}
function dojump(){
    var f=FastConfig_Net_WAN1;

	f.connectionType.value=f.connectionType_new.value;
	f.staticIp.value=f.staticIp_new.value;
	f.staticNetmask.value=f.staticNetmask_new.value;
	f.staticGateway.value=f.staticGateway_new.value;
	f.staticPriDns.value=f.staticPriDns_new.value;
	f.staticSecDns.value=f.staticSecDns_new.value;
	f.pppoeUser.value=f.pppoeUser_new.value;
	f.pppoePass.value=f.pppoePass_new.value;

	f.modifyFlag2.value = "";
    f.action = "FastConfig_edit_Vlan.asp";
    FastConfig_Net_WAN1.submit();
}
function goaway(){
	parent.main.location="firstPage.asp";
}
function initDisplay()
{
	if(platform != "nv1800N")
	{
		moduleDisplay('zhuDNS',1);
		moduleDisplay('beiDNS',1);
	}
	else
	{
		FastConfig_Net_WAN1.connectionType_new.options[2] = null;/*去掉pppoe接入方式*/
	}
}
function initPage()
{
	initDisplay();
	initIntroduce();
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
  
  <script language="javascript"> if( platform=="nv1800N") showSecondTitle("<%write(fas_guideConfig);%>");else showSecondTitle("<%write(fas_guideConfig);%>");</script>
  <script language="javascript">showerror(errorstr);</script>
 
  
  <tr >
    <td height="80">
	<table width="100%" height="80" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#C5C5C5"><!--上半部分-->
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
                            		<td width="54" valign="bottom" align="center"><span class="STYLE6">接入模式</span></td>
									<td width="33"></td>
                            		<td width="40" align="center" valign="bottom" class="STYLE3">WAN口</td>
									<td width="30"></td>
                            		<td width="55" align="center" valign="bottom" class="STYLE5">VLAN接口</td>
									<td width="18"></td>
                            		<td width="80" align="center" valign="bottom" class="STYLE5">DHCP服务器</td>
									<td width="15"></td>
                            		<td width="42" align="center" valign="bottom" class="STYLE5">服务区</td>
								<!--	<td width="8"></td>
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
									<div onclick="jumpthere1()" style="cursor:pointer;">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片1-->
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
									<div onclick="jumpthere2()" style="cursor:pointer;"><table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片2-->
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
										<div onclick="jumpthere4()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
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
											<div onclick="jumpthere5()" style="cursor:pointer;">	<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
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
										<div onclick="jumpthere6()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
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
                           <!-- 		<td>
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
									</td> -->
                            		
									
									
									
									
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
    <td><form  name="FastConfig_Net_WAN1" method="post">

    <table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr>
        <td align="center" class="style7"><table border="0" width="600" align='center'>
          <tr>
            <td colspan="3" height="20">&nbsp;</td>
          </tr>
            <tr>
              <td colspan="3" align="center" height="30" valign="top" id="jieshao"><%write(configGuideTip2);%><br></td>
            </tr>
		<tr   id="gatetable"><td colspan="3"><table width="600" border="0" cellspacing="0" cellpadding="0">
            <tr><td colspan="4"><table width="600" border="0" cellspacing="0" cellpadding="0">
            <tr>
	   <td align="right" width="41%" height="22" id="jieru"><%write(com_accessType);%></td>
	   <td width="2%">&nbsp;</td>
	   <td align="left"><select name="connectionType_new" class="list" onChange="changeConnType(this.value)">
	    <option value="DHCP" id="dhcp" ><%write(com_DynamicIP);%></option>
	   <option value="STATIC" id="static"><%write(com_StaticIP);%></option>
	    <option value="PPPOE" id="pppoe"><%write(com_PPPoE);%></option>
	   </select></td><td width="27%">&nbsp;</td></tr> </table></td></tr>
	   <tr style="display:" id="britbl"><td colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="0">
  <tr>
	   <td align="right"  height="25" width="41%" id="ipaddr"><%write(com_IP);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" style="ime-mode:Disabled" name="staticIp_new" size="18"  onKeyPress="ipOnly()"   maxlength="15"   onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),yutd1)"></td><td width="27%"  id="yutd1"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="ziwang"><%write(com_NetMask);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="staticNetmask_new" style="ime-mode:Disabled" size="18"  onKeyPress="ipOnly()" maxlength="15"  onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>'),yutd2)"></td><td  id="yutd2"></td>
	   </tr>
	   <tr>
	   <td align="right"  height="25" id="wangguan"><%write(com_GateWayAddr);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="staticGateway_new" style="ime-mode:Disabled" size="18"  onKeyPress="ipOnly()" maxlength="15"  onChange="showfiledtip(re_checkIP(this,'<%write(com_GateWayAddr);%>'),yutd3)" ></td><td  id="yutd3"></td>
	   </tr>
	   <tr id="zhuDNS" style="display:">
	   <td align="right"  height="25"><%write(com_MDNS);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="text" name="staticPriDns_new" size="18" style="ime-mode:Disabled"   onKeyPress="ipOnly()" maxlength="15"   onChange="showfiledtip(re_checkMainDns(this,'<%write(com_MDNS);%>'),yutd4)"></td><td  id="yutd4"></td>
	   </tr>
	    <tr id="beiDNS" style="display:">
	   <td align="right"  height="25"><%write(com_SDNS);%></td>
	   <td >&nbsp;</td>
	   <td><input type="text" name="staticSecDns_new" size="18" style="ime-mode:Disabled"  onKeyPress="ipOnly()" maxlength="15"    onChange="showfiledtip(re_checkIP_empty(this,'<%write(com_SDNS);%>'),yutd5)"></td><td  id="yutd5"></td>
	   </tr>
</table>
</td></tr>
<tr style="display:none" id="pppoetbl"><td colspan="4" align="center"><table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
	   <td align="right"  height="25" width="41%" id="name"><%write(com_user);%></td>
	   <td width="2%"><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input type="text" name="pppoeUser_new" size="18" maxlength="31"  onKeyPress="nameOnly()" onChange="showfiledtip(re_checkName_allowZero(this,'<%write(com_user);%>'),yutd6)" ></td>
	   <td width="27%" id="yutd6"></td>
	   </tr>
       <tr>
	   <td align="right" height="25" id="password"><%write(com_passwd);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td><input type="password" name="pppoePass_new" size="18" style="ime-mode:Disabled" maxlength="31"  onKeyPress="passwordonly()" onChange="showfiledtip(re_checkPassword1(this, 31, 0),yutd7)" ></td><td  id="yutd7"></td>
	   </tr></table></td></tr>
	   <!--//gateway end -->
</table>
</td></tr>
            <tr>
              <td colspan="3" height="20">&nbsp;</td>
            </tr>
            <tr>
              <td colspan="3"><table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
                  <tr>
 <td  height="50" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="上一步" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="重填" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="离开" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="跳过" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="下一步" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="done()" value="完成" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="window.open('rhelpinfo.htm#WANConfig')" value="帮助" id="help"></td>
  
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
<!--					<input type="hidden" name="ifAddr" value="<% write(Form("ifAddr"));%>" >
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
</table>
  </td>
  </tr>
</table>
</body>
</html>


