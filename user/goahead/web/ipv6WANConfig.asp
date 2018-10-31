<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312"/>
<%langTran("left");%>
<%langTran("common");%>
<%langTran("ipv6WANConfig");%>
<title><%write(com_utt);%></title>
<link rel=stylesheet type="text/css" href="css/style.css" title="cool" id="lnk">
<script type="text/javascript" src="js/config.js"></script>
<script type="text/javascript" src="js/common.js"></script>
<script language="javascript">
//---------交互开始-------------
/*
var totalrecs=2;var errorstr="";

var ipv6WAN_addr =new Array();

var WANprefixLength =new Array();
var ipv6Gateway =new Array();
var ipv6Dns1 =new Array();
var ipv6Dns2 =new Array();
ipv6WAN_addr[0] = "111";
WANprefixLength[0] = 64;
ipv6Gateway[0] = "222";
ipv6Dns1[0] = "333";
ipv6Dns2[0] = "444";
errorstr[0] = "555";
ipv6WAN_addr[1] = "666";
WANprefixLength[1] = 32;
ipv6Gateway[1] = "777";
ipv6Dns1[1] = "888";
ipv6Dns2[1] = "999";
errorstr[1] = "123";
*/
<%aspOutputIpv6Wan(); %>
//----------交互结束---------------------

function init(){
	var f = ipv6WANConfig;

	f.ipv6WAN_addr.value = ipv6WAN_addr[0];
	f.WANprefixLength.value = WANprefixLength[0];
	f.ipv6Gateway.value = ipv6Gateway[0];
	f.ipv6Dns1.value = ipv6Dns1[0];
	f.ipv6Dns2.value = ipv6Dns2[0];
	}
	


function checkDatas(){
	var f = ipv6WANConfig;
        f.ipv6WAN_addr.value = f.ipv6WAN_addr.value.trim();	
        f.ipv6Gateway.value = f.ipv6Gateway.value.trim();	
        f.ipv6Dns1.value = f.ipv6Dns1.value.trim();	
        f.ipv6Dns2.value = f.ipv6Dns2.value.trim();	
	if (!checkNumber(f.WANprefixLength,0,127,"<%write(ipv6_prefix);%>",0)) return false;
	if (!checkIpv6(f.ipv6WAN_addr,"<%write(ipv6_addr);%>",0)) return false;	
	if (!checkIpv6Sp(f.ipv6WAN_addr,"<%write(ipv6_addr);%>")) return false;
        if (!checkIpv6(f.ipv6Gateway,"<%write(ipv6_gw);%>",0)) return false;	
        if (!checkIpv6Sp(f.ipv6Gateway,"<%write(ipv6_gw);%>")) return false;	
        if (!checkIpv6(f.ipv6Dns1,"<%write(static_dns1);%>",0)) return false;	
        if (!checkIpv6Sp(f.ipv6Dns1,"<%write(static_dns1);%>")) return false;	
        if (!checkIpv6(f.ipv6Dns2,"<%write(static_dns2);%>",0)) return false;	
        if (!checkIpv6Sp(f.ipv6Dns2,"<%write(static_dns2);%>")) return false;	
	if (! checkIpv6Subnet(f.ipv6WAN_addr,f.ipv6Gateway,f.WANprefixLength)){
	    alert("<%write(alert_one);%>");
	    return false;
	}
        var dns1 = convert2CompleteIpV6(f.ipv6Dns1.value);
        var dns2 = convert2CompleteIpV6(f.ipv6Dns2.value);
	if( dns1 == dns2){
	    alert("<%write(alert_two);%>");
            return false;
	}
        var ipD = convert2CompleteIpV6(f.ipv6WAN_addr.value);         //将当前控件处IP补全
        for(var i = 0; i< ipv6WAN_addr.length;i++)
	{
            var ipO = convert2CompleteIpV6(ipv6WAN_addr[i]);          //将其它已配置好的接口IP补全
	    if((i != f.PortName.selectedIndex)&&(ipD == ipO))
		{
		    alert("WAN"+(f.PortName.selectedIndex + 1)+"<%write(yu);%>WAN"+(i+1)+" <%write(alert_thr);%>");
		    return false;

		}

        }
        if (ipv6LAN_addr != ""){
            if(f.WANprefixLength.value != LANprefixLength){
                if(f.ipv6WAN_addr.value == ipv6LAN_addr){
                    alert("<%write(alert_for);%>");
                    return false;
                }
            }else{
                if (checkIpv6Subnet2(f.ipv6WAN_addr.value,ipv6LAN_addr,f.WANprefixLength.value)){
                    alert("<%write(alert_fiv);%>");
                    return false;
                }

            }      
        }		
	return true;

}

function save(){
	if(checkDatas()){
	    ipv6WANConfig.action = "/goform/formIpv6Wan";
	    ipv6WANConfig.submit();
	}
}

function Reset(){
	ipv6WANConfig.reset();
	init();
 
	}
function EditConfig(val,i){
	var f = ipv6WANConfig;
	f.ipv6WAN_addr.value = ipv6WAN_addr[i];
	f.WANprefixLength.value = WANprefixLength[i];
	f.ipv6Gateway.value = ipv6Gateway[i];
	f.ipv6Dns1.value = ipv6Dns1[i];
	f.ipv6Dns2.value = ipv6Dns2[i];
}
function ConfigWanDel(){
    if(confirm("<%write(com_deleteTip);%>")){
	ipv6WANConfig.action="/goform/Ipv6WanDel";
	ipv6WANConfig.submit();
    }
}
</script>
</head>
<body onLoad=""onbeforeunload="logOut()" background="<%write(com_bodyMain);%>">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
  <!--*****顶上间隔****-->
  <tr>
    <td colspan="3">&nbsp;</td>
  </tr>
  <!--****中间内容****-->
  <tr>
  <td width="15"></td>
  <!--////内容-->
  <td><script language="javascript">showSecondTitle('<%write(ipv6_wan);%>');</script>
  <table width="100%" border="0" height="450" cellspacing="0" cellpadding="0" align="center"  class="table_bg">
   <script language="javascript">showerror(errorstr);</script>
  <tr valign="top">
    <td><form action="/goform/"  name="ipv6WANConfig" method="post">
<table width="80%" border="0" bordercolor="red" cellspacing="0" cellpadding="0" align="center">
<p>&nbsp;</p>
    <tr><td align="center" class="style7">
    <table border="0" width="90%" bordercolor="green" align='center'>
	<tr><td colspan="3" height="20"><input type="hidden" name="Action" value="add">&nbsp;</td></tr>
	<tr>
             <td align="right"  width="41%"  id="method"><%write(interface);%></td>
       <td width="2%">&nbsp;</td>
         <td><select name="PortName" class="list"  onChange="EditConfig(1,this.selectedIndex)">
               <script language="javascript"> 
                 document.write('<option value="1" >WAN1</option>');
           for (i = 2; i <= totalrecs; i++) {
        //         var j=i+1;
                 document.write('<option value="'+i+'" disabled="true" >WAN'+i+'</option>');
            }
            </script>
           </select></td>
          </tr>	
	<tr>
	   <td align="right" width="40%" height="25"><%write(ipv6_addr);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6WAN_addr" id="ipv6WAN_addr" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(ipv6_addr);%>',0),iptd)"></td><td width="33%" id="iptd"></td>
	</tr>
   
    <tr>
	   <td align="right" height="25"><%write(ipv6_prefix);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="WANprefixLength" id="WANprefixLength" type="text" size="10" maxlength="63" onKeyPress="figureOnly()" onChange="showfiledtip(re_checkNumber(this,0,127,'<%write(ipv6_prefix);%>'),prtd)"> </td><td id="prtd"></td>
	</tr>
    <tr>
	   <td align="right" height="25"><%write(ipv6_gw);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6Gateway" id="ipv6Gateway" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(ipv6_gw);%>',0),gwtd)"></td><td width="33%" id="gwtd"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(static_dns1);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6Dns1" id="ipv6Dns1" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(static_dns1);%>',0),dns1td)"></td><td width="33%" id="dns1td"></td>
	</tr>
	<tr>
	   <td align="right" height="25"><%write(static_dns2);%></td>
	   <td ><img src="<%write(com_imgqt);%>" width="7" height="9"></td>
	   <td ><input name="ipv6Dns2" id="ipv6Dns2" type="text" size="38" maxlength="40" onKeyPress="ipv6Only()" onChange="showfiledtip(re_checkIpv6(this,'<%write(static_dns2);%>',0),dns2td)"></td><td width="33%" id="dns2td"></td>
	</tr>
    
	
	<tr><td colspan="3" height="20">&nbsp;</td></tr>
	<tr><td colspan="4">
	<table width="80%" border="0" cellspacing="0" cellpadding="0" align="center">
	    <tr>
	    <td height="30" align="center" valign="top">
	    <input type="button" class="input-button" value="<%write(com_save);%>"  onClick="save()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" class="input-button" value="<%write(com_reset);%>" onClick="Reset()">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_help);%>" class="input-button" onClick="window.open('<% write(com_helppage); %>#Syslog')">
	    &nbsp;&nbsp;&nbsp;
	    <input type="button" value="<%write(com_delete);%>" class="input-button" onClick="ConfigWanDel();">
	    </td>
	   </tr>
	</table>
	</td></tr>
    </table>
    </td></tr>
</table>
</form></td>
</tr><script language="javascript">init();</script>
</table></td>
<td width="15"></td>
</tr>
</table>
</body>
</html>
