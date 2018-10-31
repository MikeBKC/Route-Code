<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>安全设置</title>
<link href="css/style_new.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="script/jquery-1.8.1.min.js"></script> 
<script type="text/javascript" src="script/frame.js"></script>  
</head>
<script type="text/javascript">
$(function(){
		<!-- 初始化按钮 -->
		var switch_b = $("#pptp_on_off").children("button");
		var pptpstatus="on";
		initEnabledButton(switch_b,pptpstatus);
		
		switch_b = $("#l2tp_on_off").children("button");
		var l2tpstatus="off";
		initEnabledButton(switch_b,l2tpstatus);
		
		switch_b = $("#ipsec_on_off").children("button");
		var ipsecstatus="off";
		initEnabledButton(switch_b,ipsecstatus);
		
		switch_b = $("#ftp_on_off").children("button");
		var ftpstatus="off";
		initEnabledButton(switch_b,ftpstatus);
		
		switch_b = $("#tftp_on_off").children("button");
		var tftpstatus="off";
		initEnabledButton(switch_b,tftpstatus);
		
		switch_b = $("#h323_on_off").children("button");
		var h323status="off";
		initEnabledButton(switch_b,h323status);
		
		
		switch_b = $("#rtsp_on_off").children("button");
		var rstpstatus="off";
		initEnabledButton(switch_b,rstpstatus);
		
		<!-- 按钮单击事件-->
		$("#pptp_on_off").click(function(){
			var switch_b = $("#pptp_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabledl2tp"));
		
		})
		
		$("#l2tp_on_off").click(function(){
			var switch_b = $("#l2tp_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabledpptp"));
		
		})
		
		$("#ipsec_on_off").click(function(){
			var switch_b = $("#ipsec_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabledipsec"));
		
		})
		
		$("#ftp_on_off").click(function(){
			var switch_b = $("#ftp_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabledftp"));
		
		})
		$("#tftp_on_off").click(function(){
			var switch_b = $("#tftp_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabledtftp"));
		
		})
		$("#h323_on_off").click(function(){
			var switch_b = $("#h323_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabled323"));
		
		})
		$("#rtsp_on_off").click(function(){
			var switch_b = $("#rtsp_on_off").children("button");
			var status_now = switch_b.attr("class");
			changebutton(switch_b,status_now,$("#ebabledrtsp"));
		
		})
		
	function changebutton(switch_b,status_now,objhi){
		if(status_now == "on"){
			//swicth_act("off");
			switch_b.removeClass("on");
			switch_b.addClass("off");
			objhi.val("off");
		} else if (status_now == "off"){
			//swicth_act("on");
			switch_b.addClass("on");
			switch_b.removeClass("off");
			objhi.val("on");
		}
		
	}
})	
</script>
<body>
<div class="warp">
<br>
<form action="" method="post" name="">
     <div id="error_msg" style="color:red;"></div>
     <table border="0" cellpadding="0" cellspacing="1" width="500px" id="runstatusInfo" style="background:#5e5e5e;" class="group" >
   
   
   		  <tr class="tab_title">
        	<td colspan="3">虚拟专用网(VPN)</td>
           
        </tr>
    	<tr class="tab_info">
        	<td width="150px">PPTP穿透</td>
            <td colspan="2" width="350px"><span class="on-off" id="pptp_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabledpptp" id="ebabledpptp" value="on" /></span></td>
        </tr>
        <tr class="tab_info">
        	<td>L2TP穿透</td>
            <td colspan="2"><span class="on-off" id="l2tp_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabledl2tp" id="ebabledl2tp" value="on" /></span></td>
           
        </tr>
        <tr class="tab_info">
        	<td>IPSec穿透</td>
            <td colspan="2">
            	<span class="on-off" id="ipsec_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabledipsec" id="ebabledipsec" value="on" /></span>
            </td>
            
          <tr class="tab_title">
        	<td colspan="3">应用层网关(ALG)</td>
           
        </tr>
        </tr>
        <tr class="tab_info">
        	<td>FTP ALG</td>
            <td colspan="2"><span class="on-off" id="ftp_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabledftp" id="ebabledftp" value="on" /></span></td>
        </tr>
        <tr class="tab_info">
        	<td>TFTP ALG</td>
            <td colspan="2"><span class="on-off" id="tftp_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabledtftp" id="ebabledtftp" value="on" /></span></td>
        </tr>
        <tr class="tab_info">
        	<td>H.323 ALG</td>
            <td colspan="2"><span class="on-off" id="h323_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabled323" id="ebabled323" value="on" /></span></td>
        </tr>
       <tr class="tab_info">
        	<td>RTSP ALG</td>
            <td colspan="2"><span class="on-off" id="rtsp_on_off"><button type="button" class="on">开关</button><input type="hidden" name="ebabledrtsp" id="ebabledrtsp" value="on" /></span></td>
        </tr>
        <tr class="tab_savetd" >
        	<td colspan="3" align="right" ><span style="padding-left:400px;"><input type="button" class="btn" name="back" onClick="save();"value="保存"/></span></td>
            
        </tr>
    </table>
    
</form>
</div>
</body>
</html>
