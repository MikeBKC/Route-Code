<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("NetSniper"); %>
<% langTran("common"); %>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/common.js"></script>
<script type="text/javascript" src="lan/b28n.js"></script>
<script language="javascript" ContentType="text/xml">
<%aspOutIpGrpList();%>
<%aspNetSniperConfig();%>
function save(){
	var f = NetSniper_Config;
	if(checkData()){
		f.action="/goform/netSniperConfigForm";
		f.submit();
	}
}
function checkData(){
	var f = NetSniper_Config;
	if(!checkNumber(f.time,1,65535,"<%write(RemainTime);%>"))
		return false;
	return true;
}
function init(){
	var f = NetSniper_Config;
	f.enable.checked=(enable == "on" ? true:false);
	f.address.value=groupName;
	f.time.value=controlTime;
}
function Reset()
{
	NetSniper_Config.reset();
	init();
}
</script>
</head>

<body background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
   <td width="15"></td>
   <!--////内容-->
   <td>
   		<script language="javascript">showSecondTitle('<%write(NetSniper);%>');</script>
  		<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   			<script language="javascript">showerror(errorstr, 80);</script>
  			<tr valign="top">
   	 			<td><form action=""  name="NetSniper_Config" method="post">
                   <p>&nbsp;</p>
                            <script language="javascript">
                                var arrayThreeMenu=new Array(
                                                "80%",
                                                3,
                                                new Array("<%write(NetSniperList);%>",'NetSniperList.asp',1,0),
                                                new Array("<%write(NetSniperConf);%>",'NetSniperConf.asp',1,1),
                                                new Array("<%write(NetSniperNotice);%>",'NetSniperNotice.asp',1,0)
                                );
                                showThreeMenu();
                            </script>
					<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
					
                    	<tr>
                            <td align="center" class="three_td" valign="top">
                          		<table border="0" width="600" align='center'> 
                                	 <tr>
                                       <td width="43%" height="25"></td>
                                       <td width="2%"></td>
                                       <td></td>
                                       <td  id="yutd1"></td>
                                    </tr>                              	
                                   <tr>
                                       <td align="right" height="25"><%write(NetSniperEnable);%></td>
                                       <td></td>
                                       <td align="left"><input name="enable" type="checkbox"></td>
                                       <td  id="yutd1"></td>
                                    </tr>
									<tr>
                                       <td align="right" height="25"><%write(ExceptIpGrp);%></td>
                                       <td ></td>
                                       <td align="left"><select name="address">
                                       		<option value=""><%write(None);%></option>
											<script language="javascript">
											for(var i=0;i<addGroupNames.length;i++)
											{
											document.write('<option value='+addGroupNames[i]+'>'+addGroupNames[i]+'</option>');
											}
											</script>
                                       		</select></td>
                                       <td  id="yutd1"></td>
                                    </tr>
                                     <tr>
                                       <td align="right" height="25"><%write(CtlTime);%></td>
                                       <td ><img src="images/qt.gif" width="7" height="9"></td>
                                       <td align="left"><input name="time" type="text" maxlength="11" size="18" onChange="showfiledtip(re_checkName(this,'Name'),yutd1)" value="300"><%write(Second);%></td><td  id="yutd1"></td>
                                       </tr>
									<tr><td colspan="4">&nbsp;</td></tr>
                                    <tr><td colspan="4" align="center"><input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" value="<%write(com_reset);%>"  onClick="Reset()">&nbsp;&nbsp;&nbsp;<input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('rhelpinfo.htm#NetSniper')"></td></tr>
                                    <tr><td colspan="4">&nbsp;</td></tr>
								</table>
                            </td>
                         </tr>
					</table>
                </form>
				<script language="javascript">init();</script>
                </td>
             </tr>
         </table>
   </td>
   <td width="15"></td>
  </tr>
</table>
</body>
</html>
