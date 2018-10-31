<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<%langTran("common");%>
<%langTran("UPnP");%>
<title>UPnP</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="js/common.js"></script> 
<script type="text/javascript" src="script/frame.js"></script> 
</head>
<script type="text/javascript">
var errorstr="";
/*
var upnpIntIp=new Array();var upnpRemoteIp=new Array();var upnpIntPort=new Array();var upnpExtPort=new Array();var upnpProt=new Array();var description=new Array();var upnpEnables="0";var totalrecs = 1;
 
 upnpIntIp[0]="192.168.1.1";upnpIntPort[0]="80";upnpProt[0]="TCP";upnpRemoteIp[0]="200.200.200.120";upnpExtPort[0]="52";description[0]="dsd";
 */
<% aspOutUpnpTableAndConfig();%>
var  upnpEnbleOld;
var listheadname="<%write(upnp_list);%>";
var titles = new Array();
var totalTitel = 7;
titles[0] = "ID";
titles[1] ="<%write(upn_innerIP);%>";
titles[2] ="<%write(upn_innerPort);%>";
titles[3] ="<%write(com_protocol);%>";
titles[4] ="<%write(upn_exterIP);%>";
titles[5] ="<%write(upn_exterPort);%>";
titles[6] ="<%write(com_descript);%>";
var arrayData = new Array(upnpIntIp,upnpIntPort,upnpProt,upnpRemoteIp,upnpExtPort,description);

function init(){
	var switch_b = $(".on-off").children("button");
	var upnpEnablesw = (upnpEnables=="1"?"on":"off");
	initEnabledButton(switch_b,upnpEnablesw);
	$("#upnpEnblew").val(upnpEnables);			
	showSuccessTips(rightstr);
}
function save(){
		var f=UPnP_Config;
		f.submit();

}
$(function(){
	init();
	$(".on-off").click(function(){
		var switch_b = $(".on-off").children("button");
		var status_now = switch_b.attr("class");
		if(status_now=="on"){
			$("#upnpEnblew").val("0");	
		}else{
			$("#upnpEnblew").val("1");	
		}
		swicth_act(status_now);
	});
	})

</script>
<body>
<div class="warp">

<br>
	<form action="/goform/formConfigUpnp" method="post" name="UPnP_Config">
    <!-- <div id="error_msg" style="color:red;"></div> -->
	<table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		
    	
       
       
       	 <tr  height="60px" class="tab_savetd"> 
        	<td colspan="3" align="center">
            	<table cellpadding="0" cellspacing="0" border="0" >
                	<tr>
                    	<td width="90px"><%write(upn_start);%></td>
                        <td align="left" width="100px"><span class="on-off">&nbsp;<button type="button" class="on"><%write(com_switch);%></button><input type="hidden" name="upnpEnblew" id="upnpEnblew" value="" /></span></td>
                        <td align="right"><input type="submit" class="btn" name="back"  value="<%write(com_save);%>"/></td>
                    </tr>
                </table>
           </td>
            
        </tr>

    </table>
    <script type="text/javascript">document.write(dspListTab(listheadname,titles,totalTitel,arrayData,0));</script>
   <table border="0" cellpadding="0" cellspacing="1" style="background:#5e5e5e;" class="group" >
   		<tr><td  class="tab_savetd" align="right"><span><input type="button" class="btn" onClick="location.reload();"  value="<%write(com_fresh);%>"/></span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
   </table>
    </form>
    <br>
    <br>
</div>
</body>
</html>
