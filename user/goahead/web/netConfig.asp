<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("common");%>
<%langTran("netConfig");%>
<title><%write(utt_netConf);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>

</head>
<script type="text/javascript">
<%aspOutPutCacheModeConfig();%>

function checkData(){
	
	var f = NetworkBridge;;
	
	if (f.ip.value=="0.0.0.0")
	{ 
		
		alert("<%write(utt_checkIPTips2);%>");
		return false;
	}

	if (!checkIP(f.ip,"<%write(com_IP);%>",0))
	{	
		
		return false;
	}

	if (!checkMask(f.mask,"<%write(com_NetMask);%>",0))

	{	
		return false;
	}

	if (!checkIP(f.defaultGW,"<%write(com_GateWayAddr);%>",0))
	{
		return false;
	}

	if (!checkSameSubnet(f.ip.value,f.defaultGW.value,f.mask.value))
	{
		
		alert("<%write(utt_checkIPTips1);%>");
		return false;
	}

	if (!checkMainDns(f.priDns,"<%write(com_MDNS);%>",0))
	{
	
		return false;
	}
	
	if (!checkIP(f.secDns,"<%write(com_SDNS);%>",1))
	{
		
		return false;
	}
											
	return true;
}
function save(){

	var f = NetworkBridge;
	
	if(checkData()){
		document.NetworkBridge.submit();
	}
}

function init(){
	var f = NetworkBridge;
	f.ip.value=brIP;
	f.mask.value=brNM;
	f.defaultGW.value=brGW;
	f.priDns.value=brDNS1;
	f.secDns.value=brDNS2;
}

function Reset(){
	var f = NetworkBridge;
	f.reset();
	init();

}
</script>
<body background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(utt_netConf);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/formCacheModeConfig"  name="NetworkBridge" id="NetworkBridge" method="post">
		<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" class="style7">
		<p>&nbsp;</p>
								<tr>
									<td align="center" width="100%" >
										<table width="100%" border="0" align='center'>
						<tr ><td width="44%" >&nbsp;</td>
						</tr>
						<tr >	
							<td align="right"><%write(com_IP);%></td
							 ><td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
						  <td width="25%" height="27px" align="left"><input type="text" name="ip" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_IP);%>'),yutd1)" /></td>	
						  <td width="29%" align="left" id="yutd1"></td>					
						</tr>
						<tr>	
							<td align="right"><%write(com_NetMask);%></td>
							 <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
							<td align="left" height="27px"><input type="text" name="mask" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMask(this,'<%write(com_NetMask);%>'),yutd2)" /></td>	
							<td align="left" id="yutd2"></td>						
						</tr>
						
						<tr>	
							<td align="right"><%write(utt_defaultGW);%></td>
							 <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
							<td align="left" height="27px"><input type="text" name="defaultGW" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP(this,'<%write(com_GateWayAddr);%>'),yutd3)"  /></td>
							<td align="left" id="yutd3"></td>							
						</tr>
						<tr>	
							<td align="right"><%write(com_MDNS);%></td>
							 <td width="2%"><img src="images/qt.gif" width="7" height="9"></td>
							<td align="left" height="27px">
							<input type="text" name="priDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkMainDns(this,'<%write(com_MDNS);%>'),yutd4)" />							</td>
							<td align="left" id="yutd4"></td>							
						</tr>
						<tr>	
							<td align="right"><%write(com_SDNS);%></td>
							 <td width="2%">&nbsp;</td>
							<td align="left" height="27px">
							<input type="text" name="secDns" size="18"  onKeyPress="ipOnly()" maxlength="15" onChange="showfiledtip(re_checkIP_empty(this,'<%write(com_SDNS);%>'),yutd5)"  />							</td>	
							<td align="left" id="yutd5"></td>						
						</tr>
					</table>
					
									</td>
								</tr>
								
								<!-- ************* -->
																
								<tr><td>&nbsp;</td></tr>
							
								 <tr>
								 <td><table align="center">
								
								 <tr>
								 <td align="right"><input type="button" class="input-button" value="<%write(com_save);%>" id="saveBtn"  onClick="save();"/></td>
				<td align="center"><input type="button" class="input-button" value="<%write(com_reset);%>" id="resetBtn"  onClick="Reset();" /></td>
				<td align="left"><input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<%write(com_helppage);%>#netconfig')" /></td>
								 </tr>
								 
								 </table></td>
				
			</tr>
			<tr><td>&nbsp;</td></tr>
							</table>
		
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>

