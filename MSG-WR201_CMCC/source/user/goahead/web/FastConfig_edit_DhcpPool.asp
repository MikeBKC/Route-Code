 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("common");%>
<%langTran("FastConfig");%>
<%langTran("DhcpPool");%>
<%langTran("commonWords");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/config.js" type="text/javascript"></script>
<script src="js/common.js" type="text/javascript"></script>
<script type="text/javascript" src="lang/b28n.js"></script>
<script type="text/javascript" ContentType="text/xml" language="javascript">
//交互begin
<%
	aspDhcpPoolConfig();
%>
//交互end

function init(){
    var f = edit_DhcpPool;
	vlanInit();
	if(f.modifyFlag4.value == "change")
	{
		f.poolName_new.value = "<% write(Form("poolName"));%>";
		f.DhcpEnable_new.checked = ("<% write(Form("DhcpEnable"));%>" == "true")? true : false;
		f.dhcpVid_new.value = "<% write(Form("dhcpVid"));%>";
		f.dhcpStart_new.value = "<% write(Form("dhcpStart"));%>";
		f.dhcpEnd_new.value = "<% write(Form("dhcpEnd"));%>";
		f.dhcpMask_new.value = "<% write(Form("dhcpMask"));%>";
		f.dhcpGateway_new.value = "<% write(Form("dhcpGateway"));%>";
		f.dhcpLease_new.value = "<% write(Form("dhcpLease"));%>";
		f.dhcpPriDns_new.value = "<% write(Form("dhcpPriDns"));%>";
	 	f.dhcpSecDns_new.value = "<% write(Form("dhcpSecDns"));%>";
	}
	else{
		f.poolName_new.value ="";
		f.DhcpEnable_new.checked = false;
		//f.dhcpVid_new.value="";
		f.dhcpStart_new.value="";
		f.dhcpEnd_new.value="";
		f.dhcpMask_new.value="";
		f.dhcpGateway_new.value="";
		//f.dhcpLease_new.value="";
		f.dhcpPriDns_new.value="";
	 	f.dhcpSecDns_new.value="";
	}
	changeSel();
}


function changeSel()
{
	var f = edit_DhcpPool;

	var tmp = document.getElementById('dhcpVid_new').value;

	for (var i = 0; i < vlanConfig.length; i++) {
		if (tmp == vlanConfig[i]) {
			f.dhcpMask_new.value = vlanNetMask[i];
		}
	}	
	
	if (tmp == "<%write(Form("VlanName"));%>")
	{
		f.dhcpMask_new.value = f.VlanMask.value;
	}
	document.getElementById('dhcpMask_new').readOnly = true;

}
function checkData()
{
	var f= edit_DhcpPool;
	if (!checkName(f.poolName,11,_("<%write(dhcpPoolName);%>"),0)) return false;
	if (!checkIP(f.dhcpStart,_("<%write(startip);%>"),0)) return false;
	if (!checkIP(f.dhcpEnd,_("<%write(endip);%>"),0)) return false;
	if (f.dhcpStart.value=="0.0.0.0")
	{
		alert(_("<%write(warning5);%>"));
		f.dhcpStart.focus();
		return false;
	}
	if (f.dhcpEnd.value=="0.0.0.0")
	{
		alert(_("<%write(warning6);%>"));
		f.dhcpEnd.focus();
		return false;
	}
	if (compareIP(f.dhcpStart.value,f.dhcpEnd.value)==1)
	{
		alert(_("<%write(warning17);%>"));
		return false;
	}   
	if (!checkMask(f.dhcpMask,_("<%write(subnet);%>"),0)) return false;
	if (!checkIP(f.dhcpGateway,_("<%write(gateway);%>"),0)) return false;
	if (!checkNumber(f.dhcpLease,1,14400,_("<%write(usetime);%>"),0)) return false;
	if (!checkMainDns(f.dhcpPriDns,_("<%write(zhuDNS);%>"),0)) return false;
	if (!checkIP(f.dhcpSecDns,_("<%write(beiDNS);%>"),1)) return false;
	if (!sameNetCheck(f.dhcpStart.value, f.dhcpMask.value)) return false;
	/*检查地址池名字是否冲突*/
	for(var i=0;i<poolNames.length;i++ )
	{
		if(f.poolName.value == poolNames[i])
		{
			alert("地址池名称与已有的配置冲突！");
			return false;
		}
	}
	if(f.dhcpVid.value == "")
	{
		alert("VLAN ID 不能为空！请优先配置虚接口！");
		return false;
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

function vlanInit()
{
	var option, i;
	var f=edit_DhcpPool;
	for (i = f.dhcpVid_new.length; i > -1; i--)
	{
		f.dhcpVid_new.remove(i);
	}
	for (i = 0; i < vlanConfig.length; i++)
	{
		option = document.createElement("OPTION");
		option.value = vlanConfig[i];
		option.text = vlanConfig[i];
		f.dhcpVid_new.options.add(option);
	}
	var tmp_vlanName = "<%write(Form("VlanName"));%>";
	var tmp_vlanEn = "<%write(Form("VlanEnable"));%>"; 
	if(( tmp_vlanName != "")&&(tmp_vlanEn == "true"))
	{
		option = document.createElement("OPTION");
		option.value = tmp_vlanName;
		option.text = tmp_vlanName;
		f.dhcpVid_new.options.add(option);
	}
}
function Reset()
{
	edit_DhcpPool.reset();
	yutd1.innerHTML="";
	yutd2.innerHTML="";
	yutd3.innerHTML="";
	yutd4.innerHTML="";
	yutd5.innerHTML="";
	yutd6.innerHTML="";
	yutd7.innerHTML="";
	yutd9.innerHTML="";
}
 function jumpthere1()
{
     edit_DhcpPool.action="FastConfig.asp";
     edit_DhcpPool.submit();
}
  function jumpthere2()
{
     edit_DhcpPool.action="FastConfig_acWorkMode.asp";
     edit_DhcpPool.submit();
}
 function jumpthere3()
{
     edit_DhcpPool.action="FastConfig_Net_WAN1.asp";
     edit_DhcpPool.submit();
}
 function jumpthere4()
{
     edit_DhcpPool.action="FastConfig_edit_Vlan.asp";
     edit_DhcpPool.submit();
}
 function jumpthere5()
{
     edit_DhcpPool.action="FastConfig_edit_DhcpPool.asp";
     edit_DhcpPool.submit();
}
 function jumpthere6()
{
     edit_DhcpPool.action="FastConfig_edit_SZ.asp";
     edit_DhcpPool.submit();
}
 function jumpthere7()
{
     edit_DhcpPool.action="FastConfig_MacFilterGloable_Config.asp";
     edit_DhcpPool.submit();
}
// function jumpthere8()
//{
//     edit_DhcpPool.action="apManage.asp";
//     edit_DhcpPool.submit();
//}
function dopre(){
	var f=edit_DhcpPool;

	f.poolName.value = f.poolName_new.value;
	f.DhcpEnable.value = f.DhcpEnable_new.checked;
	f.dhcpVid.value = f.dhcpVid_new.value;
	f.dhcpStart.value = f.dhcpStart_new.value;
	f.dhcpEnd.value = f.dhcpEnd_new.value;
	f.dhcpMask.value = f.dhcpMask_new.value;
	f.dhcpGateway.value = f.dhcpGateway_new.value;
	f.dhcpLease.value = f.dhcpLease_new.value;
	f.dhcpPriDns.value = f.dhcpPriDns_new.value;
	f.dhcpSecDns.value = f.dhcpSecDns_new.value;
	f.modifyFlag4.value = "change";

    f.action = "FastConfig_edit_Vlan.asp";
    f.submit();
}

function donext(){
	var f=edit_DhcpPool;

	f.poolName.value = f.poolName_new.value;
	f.DhcpEnable.value = f.DhcpEnable_new.checked;
	f.dhcpVid.value = f.dhcpVid_new.value;
	f.dhcpStart.value = f.dhcpStart_new.value;
	f.dhcpEnd.value = f.dhcpEnd_new.value;
	f.dhcpMask.value = f.dhcpMask_new.value;
	f.dhcpGateway.value = f.dhcpGateway_new.value;
	f.dhcpLease.value = f.dhcpLease_new.value;
	f.dhcpPriDns.value = f.dhcpPriDns_new.value;
	f.dhcpSecDns.value = f.dhcpSecDns_new.value;
	f.modifyFlag4.value = "change";

	if(checkData()){
		f.action = "FastConfig_edit_SZ.asp";
		f.submit();
	}
}
function done()
{
	var f=edit_DhcpPool;

	f.poolName.value = f.poolName_new.value;
	f.DhcpEnable.value = f.DhcpEnable_new.checked;
	f.dhcpVid.value = f.dhcpVid_new.value;
	f.dhcpStart.value = f.dhcpStart_new.value;
	f.dhcpEnd.value = f.dhcpEnd_new.value;
	f.dhcpMask.value = f.dhcpMask_new.value;
	f.dhcpGateway.value = f.dhcpGateway_new.value;
	f.dhcpLease.value = f.dhcpLease_new.value;
	f.dhcpPriDns.value = f.dhcpPriDns_new.value;
	f.dhcpSecDns.value = f.dhcpSecDns_new.value;
	f.modifyFlag4.value = "change";

	if(checkData()){
		f.pageAction.value = "5"; 
		f.action="/goform/formConfigFastDirectionAC";
		f.submit();
	}
}
function dojump(){
    var f=edit_DhcpPool;
    
	f.poolName.value = f.poolName_new.value;
	f.DhcpEnable.value = f.DhcpEnable_new.checked;
	f.dhcpVid.value = f.dhcpVid_new.value;
	f.dhcpStart.value = f.dhcpStart_new.value;
	f.dhcpEnd.value = f.dhcpEnd_new.value;
	f.dhcpMask.value = f.dhcpMask_new.value;
	f.dhcpGateway.value = f.dhcpGateway_new.value;
	f.dhcpLease.value = f.dhcpLease_new.value;
	f.dhcpPriDns.value = f.dhcpPriDns_new.value;
	f.dhcpSecDns.value = f.dhcpSecDns_new.value;
	f.modifyFlag4.value = "";
	
    f.action = "FastConfig_edit_SZ.asp";
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
	color:#84A6Ac;
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
		<!--<td width="40"></td>
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
			</td>-->
			
			
			
			<td valign="top">&nbsp;</td>
		  	<td valign="top" align="right" width="600">
		  		<table width="100%" border="0" cellpadding="0" cellspacing="0"><!--三行开始-->
				
            		<tr>
              			<td height="20">&nbsp;</td>
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
                            		<td width="55" align="center" valign="bottom" class="STYLE6">VLAN接口</td>
									<td width="18"></td>
                            		<td width="80" align="center" valign="bottom" class="STYLE3">DHCP服务器</td>
									<td width="15"></td>
                            		<td width="42" align="center" valign="bottom" class="STYLE5">服务区</td>
						<!--			<td width="8"></td>
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
										<div onclick="jumpthere5()" style="cursor:pointer;">		<table height="30" width="32" border="0" background="images/5.png" cellpadding="0" cellspacing="0"><!--底圆图片5-->
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
<form name="edit_DhcpPool" method="post" action="">
<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
<tr><td align="center" class="style7">
<table border="0" width="600" align='center'>
  <tr><td colspan="4" height="30">&nbsp;</td></tr>
		  <tr>
		    <td width="33%" align="right"><%write(dhcpEnable);%></td>
		    <td width="3%"> &nbsp;&nbsp;</td>
		    <td width="30%" height="20"><input name="DhcpEnable_new" id="DhcpEnable_new" type="checkbox" size="18"></td>
		    <td width="34%"></td>
		    </tr>
		  <tr>
		    <td align="right" id="startip2"><%write(dhcpPoolName);%></td>
		    <td ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="poolName_new" id="poolName_new" type="text" size="18" onChange="showfiledtip(re_checkName(this,'<%write(dhcpPoolName);%>'),yutd8)" maxlength="11"></td>
		    <td id="yutd8"></td>
		    </tr>
<!--
		  <tr>
		    <td width="33%" align="right" ><%write(poolVid);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="dhcpVid_new" id="dhcpVid_new" type="text" size="18" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,0,4094,'<%write(poolVid);%>'),yutd9)" maxlength="15"></td><td id="yutd9"></td>
		    </tr>
-->
	    <tr>
		 <td align="right"  width="22%" height="25"><%write(poolVid);%></td>
		 <td ><img src="images/qt.gif" width="7" height="9"></td>
		 <td align="left" height="25"><select name="dhcpVid_new" id="dhcpVid_new" class="input" onChange="changeSel()"></select></td>
		</tr>
		  <tr>
		    <td width="33%" align="right" id="startip"><%write(startip);%></td>
		    <td width="3%" ><img src="images/qt.gif" width="7" height="9"></td>
		    <td height="20"><input name="dhcpStart_new" id="dhcpStart_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(startip);%>'),yutd1)" maxlength="15"></td><td id="yutd1"></td>
		    </tr>
		   
		  <tr>
		<td  align="right" id="endip"><%write(endip);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpEnd_new" id="dhcpEnd_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(endip);%>'),yutd2)" maxlength="15"></td><td id="yutd2"></td>
	      </tr>
		  <tr>
		<td align="right" id="subnet"><%write(subnet);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpMask_new" id="dhcpMask_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMask(this,'<%write(subnet);%>'),yutd7)" maxlength="15"></td><td id="yutd7"></td>
	      </tr>
         <tr>
		<td  align="right" id="gateway"><%write(gateway);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpGateway_new" id="dhcpGateway_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP(this,'<%write(gateway);%>'),yutd3)" maxlength="15"></td><td id="yutd3"></td>
	      </tr>
          <tr>
		<td  align="right" id="usetime"><%write(usetime);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpLease_new" id="dhcpLease_new" type="text" value="120" size="18" onKeyPress="figureOnly()" maxlength="6" onChange="showfiledtip(re_checkNumber(this,1,14400,'<%write(usetime);%>'),yutd4)">&nbsp;&nbsp;
		<%write(minute);%></script>
		</td><td id="yutd4"></td>
	      </tr>
		  <tr>
		<td align="right" id="zhuDNS"><%write(zhuDNS);%></td>
		<td ><img src="images/qt.gif" width="7" height="9"></td>
		<td height="20"><input name="dhcpPriDns_new" id="dhcpPriDns_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkMainDns(this,'<%write(zhuDNS);%>'),yutd5)" maxlength="15"></td><td id="yutd5"></td>
	      </tr>
		  <tr>
		<td  align="right" id="beiDNS"><%write(beiDNS);%></td>
		<td>&nbsp;</td>
		<td height="20"><input name="dhcpSecDns_new" id="dhcpSecDns_new" type="text" size="18" onKeyPress="ipOnly()" onChange="showfiledtip(re_checkIP_empty(this,'<%write(beiDNS);%>'),yutd6)" maxlength="15"></td><td id="yutd6"></td>
	      </tr>

	
            <tr>
              <td colspan="4" height="20">&nbsp;</td>
            </tr>
            <tr>
              <td colspan="4" height="50" align="center"><input name="button" type="button" class="input-button" onClick="dopre()" value="上一步" id="last">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="Reset()" value="重填" id="uttreset">&nbsp;&nbsp;&nbsp; <input name="button" type="button" class="input-button" onClick="goaway()" value="离开" id="leave">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="dojump()" value="跳过" id="jump">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="donext()" value="下一步" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="done()" value="完成" id="next">&nbsp;&nbsp;&nbsp;<input name="button" type="button" class="input-button" onClick="window.open('rhelpinfo.htm#WANConfig')" value="帮助" id="help"></td>
  
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
</td>
</tr>
</table>
</form>

</td>
  </tr>
        
 <script language="javascript">init();</script>

</table>
  </td>
  </tr>
  
</table>
</body>
</html>


