 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("commonWords");%>
<%langTran("FastConfig");%>
<%langTran("vlanConfig");%>
<%langTran("PdbUpdata");%>
<%langTran("JS");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">

//交互begin
<%
	aspVlanConfig();
%>
//交互end
function init()
{
     var f = edit_Vlan;
     if(f.modifyFlag3.value == "change")
	 {
		f.VlanEnable_new.checked = ("<% write(Form("VlanEnable"));%>" == "true")? true : false;
		f.VlanName_new.value = "<% write(Form("VlanName"));%>";
		f.VlanVid_new.value = "<% write(Form("VlanVid"));%>";
		f.VlanStart_new.value = "<% write(Form("VlanStart"));%>";
		f.VlanMask_new.value = "<% write(Form("VlanMask"));%>";
	 }
	 else
     {
	 f.VlanEnable_new.checked = false;
	 f.VlanName_new.value = "VIF";
	 f.VlanVid_new.value = "";
	 f.VlanStart_new.value = "";
	 f.VlanMask_new.value ="" ;
     }

}

function checkData()
{
	var f=edit_Vlan;
	if (!checkName(f.VlanName,11,_("<%write(dhcpPoolName);%>"),0)) return false;
	if (!checkNumber(f.VlanVid,1,4094,_("<%write(poolVid);%>"),0)) return false;
	if (!checkIP(f.VlanStart,_("<%write(startip);%>"),0)) return false;
	if (f.VlanStart.value=="0.0.0.0")
	{
		alert(_("<%write(warning5);%>"));
		f.VlanStart.focus();
		return false;
	}
	if (!checkMask(f.VlanMask,_("<%write(subnet);%>"),0)) return false;
	/*判断是否重复的VID*/
	for(var i = 0;i<poolVids.length;i++)
	{
		if(poolVids[i] == f.VlanVid.value)
		{
			alert("VLAN ID与已有的配置重复！");
			return false;
		}
	}
	return true;
}

function sameNetCheck(ip, mask)
{
	var ipA,maskA,ipB,maskB;
	var ip1=new Array();
	var mask1=new Array();
	var ip2=new Array();
	var mask2=new Array();
	ip1=ip.split(".");
	mask1=mask.split(".");
	ipA=((ip1[0]<<24)|(ip1[1]<<16)|(ip1[2]<<8)|(ip1[3]));
	maskA=((mask1[0]<<24))|((mask1[1]<<16))|((mask1[2]<<8))|((mask1[3]));
	for (var i=0; i<totalrecs; i++)
	{
		if ((modify == 1) && (modifyIndex == i))
		{
			continue;	
		}
		ip2=beginIp[i];
		mask2=netMask[i];
		ip2=ip2.split(".");
		mask2=mask2.split(".");
		ipB=((ip2[0]<<24))|((ip2[1]<<16))|((ip2[2]<<8))|((ip2[3]));
		maskB=((mask2[0]<<24))|((mask2[1]<<16))|((mask2[2]<<8))|((mask2[3]));
		if ((ipA & maskA) == (ipB & maskB))
		{
			alert(_("<%write(warning23);%>"));
			return false;
		}
	}
	return true;
}

function Reset()
{
	edit_Vlan.reset();
	EditConfig(1,w);
	allInputRight(edit_DhcpPool);
	yutd1.innerHTML="";
	yutd2.innerHTML="";
	yutd3.innerHTML="";
	yutd4.innerHTML="";
	yutd5.innerHTML="";
	yutd6.innerHTML="";
	yutd7.innerHTML="";
	yutd9.innerHTML="";
}


var userAgent = navigator.userAgent.toLowerCase();
var is_opera = userAgent.indexOf('opera') != -1 && opera.version();
var is_moz = (navigator.product == 'Gecko') && userAgent.substr(userAgent.indexOf('firefox') + 8, 3);
var is_ie = (userAgent.indexOf('msie') != -1 && !is_opera) && userAgent.substr(userAgent.indexOf('msie') + 5, 3);

function showfiledtip1(tips,tdname){
        if(globalCheckTypeFlag) allInputRight();
	globalCheckTypeFlag = 0;/*调用该方法。则不采用交换机的错误提示风格。以防重叠*/
	tdname.innerHTML="";//alert(tips);
	if(tips!="") tdname.innerHTML+="<font style='font-size:9px;color=#333333'><img src='<% write(com_imgerr); %>'>"+tips+"</font>";
	else  tdname.innerHTML="";
        var f = edit_Vlan;
        f.VlanName_new.value = "VIF" + f.VlanVid_new.value ;


}
 function jumpthere1()
{
     edit_Vlan.action="FastConfig.asp";
     edit_Vlan.submit();
}
 function jumpthere2()
{
     edit_Vlan.action="FastConfig_acWorkMode.asp";
     edit_Vlan.submit();
}
 function jumpthere3()
{
     edit_Vlan.action="FastConfig_Net_WAN1.asp";
     edit_Vlan.submit();
}
 function jumpthere4()
{
     edit_Vlan.action="FastConfig_edit_Vlan.asp";
     edit_Vlan.submit();
}
 function jumpthere5()
{
     edit_Vlan.action="FastConfig_edit_DhcpPool.asp";
     edit_Vlan.submit();
}
 function jumpthere6()
{
     edit_Vlan.action="FastConfig_edit_SZ.asp";
     edit_Vlan.submit();
}
 function jumpthere7()
{
     edit_Vlan.action="FastConfig_MacFilterGloable_Config.asp";
     edit_Vlan.submit();
}
// function jumpthere8()
//{
  //   edit_DhcpPool.action="apManage.asp";
  //   edit_DhcpPool.submit();
//} 



function dopre(){
	 var f = edit_Vlan;

	 f.VlanEnable.value = f.VlanEnable_new.checked;
	 f.VlanName.value = f.VlanName_new.value;
	 f.VlanVid.value = f.VlanVid_new.value;
	 f.VlanStart.value = f.VlanStart_new.value;
	 f.VlanMask.value = f.VlanMask_new.value;
	 f.modifyFlag3.value = "change";

	if(f.acMode.value == "1")	    
	{
    	f.action = "FastConfig_acWorkMode.asp";
	}else
	{
    	f.action = "FastConfig_Net_WAN1.asp";
	}
     f.submit();
}



function donext(){
	
	var f=edit_Vlan;

	f.VlanEnable.value = f.VlanEnable_new.checked;
	f.VlanName.value = f.VlanName_new.value;
	f.VlanVid.value = f.VlanVid_new.value;
	f.VlanStart.value = f.VlanStart_new.value;
	f.VlanMask.value = f.VlanMask_new.value;
	f.modifyFlag3.value = "change";
	
	if(checkData()){
					   
		f.action="FastConfig_edit_DhcpPool.asp";
		f.submit();
	}

}

function done(){
	
	var f=edit_Vlan;

	f.VlanEnable.value = f.VlanEnable_new.checked;
	f.VlanName.value = f.VlanName_new.value;
	f.VlanVid.value = f.VlanVid_new.value;
	f.VlanStart.value = f.VlanStart_new.value;
	f.VlanMask.value = f.VlanMask_new.value;
	f.modifyFlag3.value = "change";
	
	if(checkData()){
		f.pageAction.value = "4"; 
		f.action="/goform/formConfigFastDirectionAC";
		f.submit();
	}

}


function dojump(){

	var f=edit_Vlan;

	f.VlanEnable.value = f.VlanEnable_new.checked;
	f.VlanName.value = f.VlanName_new.value;
	f.VlanVid.value = f.VlanVid_new.value;
	f.VlanStart.value = f.VlanStart_new.value;
	f.VlanMask.value = f.VlanMask_new.value;
	f.modifyFlag3.value = "";

	f.action="FastConfig_edit_DhcpPool.asp";	
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
                            		<td width="40" align="center" valign="bottom" class="STYLE6">WAN口</td>
									<td width="30"></td>
                            		<td width="55" align="center" valign="bottom" class="STYLE3">VLAN接口</td>
									<td width="18"></td>
                            		<td width="80" align="center" valign="bottom" class="STYLE5">DHCP服务器</td>
									<td width="15"></td>
                            		<td width="42" align="center" valign="bottom" class="STYLE5">服务区</td>
						<!--			<td width="8"></td>
                            		<td width="110" align="center" valign="bottom" class="STYLE5">无线MAC地址过滤</td>-->
								<!--	<td width="7"></td>
                            		<td  align="left" valign="bottom" class="STYLE5">完成</td> -->
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
										<div onclick="jumpthere2()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片2-->
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
										<div onclick="jumpthere4()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片4-->
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
										<div onclick="jumpthere5()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片-->
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
                            <!--		<td>
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
										<div onclick="jumpthere8()">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0">
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
    <td>
    <table width="80%" align="center">
    <tr><td class="style7">
<form name="edit_Vlan" method="post" action="">

<input type="hidden" name="oldName" value="">
<table border="0" width="80%" align='center'>
  <tr><td colspan="4" height="30">&nbsp;</td></tr>
		  <tr>
		    <td width="33%" align="right"><%write(dhcpEnable);%></td>
		    <td width="3%" > &nbsp;&nbsp;</td>
		    <td width="30%"height="20"><input name="VlanEnable_new" id="VlanEnable_new" type="checkbox" size="18"></td>
		    <td width="34%"></td>
		    </tr>
		  <tr>
		    <td align="right" id="startip2"><%write(dhcpPoolName);%></td>
		    <td ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="VlanName_new" id="VlanName_new" type="text" readonly  size="18"  maxlength="11" ></td>
		    <td id="yutd8"></td>
		    </tr>
		  <tr>
		    <td width="33%" align="right" ><%write(poolVid);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="VlanVid_new" id="VlanVid_new" type="text" size="18" onKeyPress="figureOnly()" onChange="showfiledtip1(re_checkNumber(this,1,4094,'<%write(poolVid);%>'),yutd9)"  maxlength="15"></td><td id="yutd9"></td>
		    </tr>
		  <tr>
		    <td width="33%" align="right" id="startip"><%write(startip);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="VlanStart_new" id="VlanStart_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(startip);%>'),yutd1)"  maxlength="15"></td><td id="yutd1"></td>
		    </tr>
		 <tr>
		<td align="right" id="subnet"><%write(subnet);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="VlanMask_new" id="VlanMask_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMask(this,'<%write(subnet);%>'),yutd7)" maxlength="15"></td><td id="yutd7"></td>
	      </tr>
		
		<tr><td colspan="4" height="20">&nbsp;</td></tr>
  <tr>
    <td  colspan="4"  height="30" align="center" valign="top"><input name="button" type="button" class="input-button" onClick="dopre()" value="上一步" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="重填" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="离开" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="跳过" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="下一步" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="done()" value="完成" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="window.open('rhelpinfo.htm')" value="帮助" id="help"></td>

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
</table>
</form>
</td></tr>
</table>
           <script language="javascript">init();</script>

</td>
  </tr>
</table>
  </td>
  </tr>
</table>
</body>
</html>


