<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<% langTran("NetShareManage"); %>
<% langTran("common"); %>
<title><% write(NSTitle); %></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script src="js/common.js" type="text/javascript"></script>
<script language="javascript"> 

var enable_smb;
var need_passwd;

var deviceInfos = new Array();
var statusClt = new Array();
//deviceInfos[0]=new Array("volume1","14.9 GB","9.2 GB","5.7 GB","62%","启用","打开存储设备");
//deviceInfos[1]=new Array("volume2","14.9 GB","9.2 GB","5.7 GB","62%","启用","打开存储设备");


<%aspOutputNetShareManage();%>


for(var i =0;i<deviceInfos.length;i++){
    statusClt[i] = deviceInfos[i][5];
    if( deviceInfos[i][5] == "on" ){
        deviceInfos[i][5] = "<% write(openSamba); %>"; 
    }else{
        deviceInfos[i][5] = "<% write(closeSamba); %>"; 
    }
//    deviceInfos[i][6] = "<% write(openDevice); %>"; 
}

function init()
{
    Tethering_Config.enableDevice.checked = (enable_smb == "on");
    Tethering_Config.enablePwd.checked = (need_passwd == "on");

}

function save()
{
	Tethering_Config.action="/goform/formNetShareManage";
	Tethering_Config.submit();
}
function popStorage()
{
	Tethering_Config.action="/goform/formStoragePop";
	Tethering_Config.submit();
}
function statusChange(cntrl, index)
{
	Tethering_Config.smbNo.value = index;
//alert(deviceInfos[index][0]);
//alert(deviceInfos[index][5]);
    if( statusClt[index] == "on" ){
	    Tethering_Config.smbEn.value = "off";
    }else{
	    Tethering_Config.smbEn.value = "on";
    }

	Tethering_Config.action = "/goform/formSingleStatusChange";
	Tethering_Config.submit();
}

</script>
</head>

<body onLoad="init()"onbeforeunload="logOut()" background="images/body_main.png">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle("<% write(NetShare); %>");</script>
  		<table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
  		 <script language="javascript">showerror(errorstr);</script>
 			 <tr valign="top">
             	<td><form action=""  name="Tethering_Config" method="post">
                        <input type="hidden" value="" name="smbNo">
                        <input type="hidden" value="" name="smbEn">
                		<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
						<p>&nbsp;</p>
                        <tr>
                            <td align="center" class="style7">
                          		<table border="0" width="600" align='center' cellpadding="0" cellspacing="0">
                                	<tr>
                                    	<td align="center">
                                        	<table cellpadding="0" cellspacing="0" border="0" width="600">
                                            	<tr>
                                                	<td colspan="6" height="30">&nbsp;</td>
                                                   
                                                </tr>
                                                <tr valign="bottom">
                                                	 <td width="12%" align="left"><% write(enableSD); %></td>
                                                    <td width="2%">&nbsp;</td>
                                                    <td width="5%" align="left"><input type="checkbox" name="enableDevice" onclick="save();" id="enableDevice"/></td>													<td width="2%">&nbsp;</td>
                                                    <td width="12%" align="right"><% write(enablePasswd); %></td>
                                                    <td width="2%">&nbsp;</td>
                                                    <td align="left"><input type="checkbox" name="enablePwd" onclick="save();" id="enablePwd"/></td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>
                                
                                    <tr valign="top"><td align="center">
                                    	<table cellpadding="0"  width="600" border="0"  cellpadding="0" cellspacing="1" class="style22">
                                        	<tr height="22">
                                            	<td><% write(volume); %></td>
                                                <td><% write(total); %></td>
                                                <td><% write(used); %></td>
                                                <td><% write(surplus); %></td>
                                                <td><% write(usedvolume); %></td>
                                                <td><% write(shareControl); %></td>
                                            </tr>
                                            
                                     
                                            	<script type="text/javascript">
												
                                                if(deviceInfos.length!=0){
													for(var i =0;i<deviceInfos.length;i++){

														if(0 == (i/2)%2){
															document.write("<tr height='22' class='style18'>");
														}else{
															
															document.write("<tr height='22' >");
														}

														
														document.write("<td>"+deviceInfos[i][0]+"</td><td>"+deviceInfos[i][1]+"</td><td>"+deviceInfos[i][2]+"</td><td>"+deviceInfos[i][3]+"</td><td>"+deviceInfos[i][4]+"</td><td align='center'><input type='button' class='input-button' value='"+deviceInfos[i][5]+"' onClick='statusChange(this,"+i+")'/></td>");	
														document.write("</tr>");
													}	
												}else{
													document.write("<tr height='22'><td colspan='7' class='style18' align='center'><% write(prompt_usb); %></td></tr>")	
												}
                                                
                                                </script>
                                            	<!--<td>volume1</td>
                                                <td>14.9 GB</td>
                                                <td>9.2 GB</td>
                                                <td>5.7 GB</td>
                                                <td>62%</td>
                                                <td>启用</td>
                                                <td>打开存储设备</td>
                                            </tr>
                                            <tr height="22" id="usbdeviceinfo2">
                                            	<td>volume1</td>
                                                <td>14.9 GB</td>
                                                <td>9.2 GB</td>
                                                <td>5.7 GB</td>
                                                <td>62%</td>
                                                <td>启用</td>
                                                <td>打开存储设备</td>
                                            </tr>
                                            <tr>&nbsp;</tr>
                                            <tr height="22">
                                            	<td colspan="7" class="style18" align="center">当前列表为空。请插入USB设备，等待10秒并重新扫描。</td>
                                            </tr>-->
                                        </table>
                                    </td></tr>
                                    <tr><td height="20">&nbsp;</td></tr>
                                    <tr>
                                    	<td align="right">
                                                	<input type="button" class="input-button" name="popup" value="<% write(popupDevice); %>" onClick="popStorage()"/>&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" class="input-button" name="scan" value="<% write(scan);%>" onClick="location.reload()"/>&nbsp;&nbsp;&nbsp;&nbsp;<!--<input type="button" class="input-button" name="saveDevice" value="保存" onClick="save()"/>&nbsp;&nbsp;&nbsp;&nbsp;--><input type="button" class="input-button" name="help" value="<% write(help); %>" onClick="window.open('<% write(com_helppage); %>#NetShareManage')"/>
                                                </td>
                                    </tr>
                                <tr><td height="20">&nbsp;</td></tr>
                                </table>
                            </td>
                        </tr>
                        </table>


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
