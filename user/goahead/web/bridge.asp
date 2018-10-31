<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<%langTran("common");%>
<%langTran("bridge");%>
<title><%write(utt_bridgeConf);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lang/b28n.js"></script>

</head>
<script type="text/javascript">
<%aspOutPutBridgeConfig();%>
function checkContain(text)
{
	var f = NetworkBridge;
	for(var i=0;i<f.AddPortout.options.length;i++)
	{
		if(text==f.AddPortout.options[i].text)
		{
			alert("<%write(utt_addInterfaceTips2);%>");
			return false;
		}
	}
	return true;
}
function fromLeftToRight(){
		
	var selectval =document.getElementById("DefaultPortout").value;
	if(selectval==""){
		alert("<%write(utt_addInterfaceTips1);%>");
		return;
	}
	var f = NetworkBridge;
	for(var i=0;i<f.DefaultPortout.options.length;i++)
	{
		if(selectval==f.DefaultPortout.options[i].value)
		{
				f.DefaultPortout.options.remove(i);
		}
	}
	var option = new Option();
	if(selectval=="LAN"){
		option.text = selectval+"(<%write(utt_inInterface);%>)";
	}else{
		option.text = selectval+"(<%write(utt_outInterface);%>)";
	}
	option.value= selectval;
	if(checkContain(option.text)==false)return;
	f.AddPortout.options.add(option);
}

function deleteVal(){
	var selectval = document.getElementById("AddPortout").value;

	if(selectval==""){
		alert("<%write(utt_delInterfaceTips2);%>");
		return;
	}
	var f = NetworkBridge;
	for(var i=0;i<f.AddPortout.options.length;i++)
	{
		if(selectval==f.AddPortout.options[i].value)
		{
			if(selectval=="LAN"||selectval=="WAN1"){
				alert("<%write(utt_delInterfaceTips1);%>");
			}else{
				f.AddPortout.options.remove(i);
				var option = new Option();
				option.text = selectval;
				option.value = selectval;
				f.DefaultPortout.options.add(option);
			}
		}
	}

}

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
	
	for(var i=0;i<f.DefaultPortout.options.length;i++)
	{
		var value = f.DefaultPortout.options[i].value;
		
		if(value=="LAN"){
			f.if0Enable.value=0;
		}else if(value=="WAN1"){
			f.if1Enable.value=0;
		}else if(value=="WAN2"){
			f.if2Enable.value=0;
		}else if(value=="WAN3"){
			f.if3Enable.value=0;
		}
	}
	for(var i=0;i<f.AddPortout.options.length;i++)
	{
		var value = f.AddPortout.options[i].value;
		
		if(value=="LAN"){
			f.if0Enable.value=1;
		}else if(value=="WAN1"){
			f.if1Enable.value=1;
		}else if(value=="WAN2"){
			f.if2Enable.value=1;
		}else if(value=="WAN3"){
			f.if3Enable.value=1;
		}
	}
//	alert("checkData():"+checkData());
	if(checkData()){
		//return;
		//document.NetworkBridge.action="/goform/formBridgeConfig";
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

	var defaultPortout = document.getElementById("DefaultPortout");
	var addPortout = document.getElementById("AddPortout");

	if(if0Enable){
		var option0 = document.createElement("option");
		option0.text = "LAN(<%write(utt_inInterface);%>)";
		option0.value= "LAN";
		addPortout.add(option0);
	}else{
		var option0 = document.createElement("option");
		option0.text = "LAN";
		option0.value= "LAN";
		defaultPortout.add(option0);
	}
	if(if1Enable){
		var option1 = document.createElement("option");
		option1.text = "WAN1(<%write(utt_outInterface);%>)";
		option1.value= "WAN1";
		addPortout.add(option1);
	}else{
		var option1 = document.createElement("option");
		option1.text = "WAN1";
		option1.value= "WAN1";
		defaultPortout.add(option1);
	}
	if(if2Enable){
		var option2 = document.createElement("option");
		option2.text = "WAN2(<%write(utt_outInterface);%>)";
		option2.value= "WAN2";
		addPortout.add(option2);
	}else{
		var option2 = document.createElement("option");
		option2.text = "WAN2";
		option2.value= "WAN2";
		defaultPortout.add(option2);
	}
	if(if3Enable){
		var option3 = document.createElement("option");
		option3.text = "WAN3(<%write(utt_outInterface);%>)";
		option3.value= "WAN3";
		addPortout.add(option3);
	}else{
		var option3 = document.createElement("option");
		option3.text = "WAN3";
		option3.value= "WAN3";
		defaultPortout.add(option3);
	}
	
}

function Reset(){
	var f = NetworkBridge;
	f.reset();
	 for(i=1;i<=5;i++){
		document.all["yutd"+i].innerHTML="";
	}

}
</script>

<body onLoad="init();" onbeforeunload="" background="images/body_main.gif">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
	  <td width="15"></td>
	  <!--////内容-->
	  <td><script language="javascript">showSecondTitle("<%write(utt_bridgeConf);%>");</script>
	  		  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   				<script language="javascript">showerror(errorstr);</script>
				
				<tr valign="top">
					<td> 
						<form method="post" action="/goform/formBridgeConfig" name ="NetworkBridge" id = "NetWorkBridge" accept-charset="utf-8" onsubmit="document.charset='utf-8';">
						<p>&nbsp;</p>
							<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center" class="style7">
								<tr>
									<td align="center" width="100%" >
										<table width="100%" height="175" border="0" align='center'>
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
								<tr><td align="center">
								<table border="0" cellpadding="0" cellspacing="0">
									<tr><td><%write(utt_interface);%></td> <td></td><td><%write(utt_member);%></td></tr>
								  <tr align="center">
						<td align="center"> <select class="input" name="DefaultPortout" size=7  id="DefaultPortout" ondblclick=""style="width:100px;height:90px;" border=0>		
					
		</select>
									</td>
									<td><table id="idImg">
			      <tr>
				<td><input name="m1" type="button" value="==>" onClick="fromLeftToRight();" style="width:55px"/></td>
			      </tr>
			      <tr>
				<td height="30"><!--<input name="m2" type="button" value="<==" onClick="fromRightToLeft();" style="width:65px" />--> </td>
			      </tr>
			<tr>
				<td height="30"><input name="delete" type="button" value="<==" onClick="deleteVal();" style="width:55px"/></td>
			      </tr>

			     </table></td>
									
			<td>
			<table>
			<tr><td><select class="input" name="AddPortout" size=7  id="AddPortout" ondblclick=""style="width:100px;height:90px;" border=0>
				
				</select></td></tr></table></td>
								  </tr>
								</table>
								</td>
								
	
								</tr>
								 <tr>
								 <td><table align="center">
								 <tr><td height="5">&nbsp;</td></tr>
								 <tr>
								 <td align="right"><input type="button" class="input-button" value="<%write(com_save);%>" id="saveBtn"  onClick="save();"/></td>
				<td align="center"><input type="button" class="input-button" value="<%write(com_reset);%>" id="resetBtn"  onClick="Reset();" /></td>
				<td align="left"><input type="button" class="input-button" value="<%write(com_help);%>" onClick="window.open('<%write(com_helppage);%>#bridge')" /></td>
								 </tr>
								 
								 </table></td>
				
			</tr>
			<tr><td>&nbsp;</td></tr>
							</table>
			<input type="hidden" name="if0Enable" value="1"/>
			<input type="hidden" name="if1Enable" value="1"/>
			<input type="hidden" name="if2Enable" value="0"/>
			<input type="hidden" name="if3Enable" value="0"/>
			

						</form>
					</td>
				</tr>
				

			  </table>	
	  </td>
		
	   <td width="15"></td>
	
  </tr>
</table>
</body>
</html>

